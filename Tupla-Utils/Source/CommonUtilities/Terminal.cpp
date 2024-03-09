#include "Terminal.h"

#include <stack>
#include <conio.h>

namespace Terminal
{
    static HANDLE locStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    static std::stack<COORD> locCursorStack;

    void Free()
    {
        FreeConsole();
    }

    void Initialize()
    {
        locStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (locStdOutHandle == nullptr)
        {
            AllocConsole();
            atexit(Free);
        }
        locStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        DWORD consoleOutputMode;
        GetConsoleMode(locStdOutHandle, &consoleOutputMode);
        SetConsoleMode(locStdOutHandle, consoleOutputMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        DWORD consoleInputMode;
        HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(stdinHandle, &consoleInputMode);
        SetConsoleMode(stdinHandle, consoleInputMode | ENABLE_VIRTUAL_TERMINAL_INPUT);
    }

    bool SetTextFont(const std::wstring &aFaceName, short aFontSize, UINT aFontFamily)
    {
        CONSOLE_FONT_INFOEX fontInfo;
        assert(aFaceName.size() <= sizeof(fontInfo.FaceName));

        memset(&fontInfo, 0, sizeof(CONSOLE_FONT_INFOEX));

        fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
        fontInfo.FontFamily = aFontFamily;
        fontInfo.dwFontSize.Y = aFontSize;
        wcsncpy_s(fontInfo.FaceName, aFaceName.data(), aFaceName.size());

        if (!SetCurrentConsoleFontEx(locStdOutHandle, false, &fontInfo))
        {
            return false;
        }

        return true;
    }

    bool SetTextFont(CONSOLE_FONT_INFOEX *aFontInfo)
    {
        return SetCurrentConsoleFontEx(locStdOutHandle, false, aFontInfo);
    }

    bool GetTextFont(CONSOLE_FONT_INFOEX *aOutConsoleFontInfoEx)
    {
        aOutConsoleFontInfoEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
        return GetCurrentConsoleFontEx(locStdOutHandle, false, aOutConsoleFontInfoEx);
    }

    void SetTextStyle(WORD aTextStyle)
    {
        SetConsoleTextAttribute(locStdOutHandle, static_cast<WORD>(aTextStyle));
    }

    void ResetTextStyle()
    {
        Terminal::SetTextStyle(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    COORD GetCursorPosition()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        GetConsoleScreenBufferInfo(locStdOutHandle, &csbi);
        return csbi.dwCursorPosition;
    }

    void SetCursorPosition(COORD aCursorPosition)
    {
        SetConsoleCursorPosition(locStdOutHandle, aCursorPosition);
    }

    void SetCursorPosition(int aXPosition, int aYPosition)
    {
        SetConsoleCursorPosition(locStdOutHandle, { static_cast<short>(aXPosition), static_cast<short>(aYPosition) });
    }

    void MoveCursor(int aDeltaX, int aDeltaY)
    {
        COORD cursor = GetCursorPosition();
        cursor.X = static_cast<short>(cursor.X + aDeltaX);
        cursor.Y += static_cast<short>(cursor.Y + aDeltaY);
        Terminal::SetCursorPosition(cursor);
    }

    void PushCursorPosition(int aPositionX, int aPositionY)
    {
        locCursorStack.push(Terminal::GetCursorPosition());
        SetCursorPosition(aPositionX, aPositionY);
    }

    COORD PopCursorPosition()
    {
        COORD cursor = locCursorStack.top();
        locCursorStack.pop();
        SetCursorPosition(cursor);
        return cursor;
    }

    void ShowCursor()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(locStdOutHandle, &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(locStdOutHandle, &cursorInfo);
    }

    void HideCursor()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(locStdOutHandle, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(locStdOutHandle, &cursorInfo);
    }

    bool IsCursorVisible()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(locStdOutHandle, &cursorInfo);
        return cursorInfo.bVisible;
    }

    TerminalSize GetSize()
    {
        // Thaaaank you: https://stackoverflow.com/questions/6812224/getting--size-in-c-for-windows/12642749#12642749
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(locStdOutHandle, &csbi);

        TerminalSize size;
        size.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        return size;
    }

    void SetCharacter(short aCellX, short aCellY, wchar_t aCharacter, WORD aTextStyle)
    {
        CHAR_INFO info;
        info.Char.UnicodeChar = aCharacter;
        info.Attributes = static_cast<WORD>(aTextStyle);

        SMALL_RECT drawRegion = { aCellX, aCellY, aCellX + 1, aCellY + 1 };

        WriteConsoleOutputW(locStdOutHandle, &info, COORD{ 1, 1 }, COORD{ 0, 0 }, &drawRegion);
    }

    void Draw2DBuffer(const CHAR_INFO *a2DCharInfoString, COORD aCharInfoBuffer2DSize, SMALL_RECT aDrawRegion)
    {
        WriteConsoleOutputW(locStdOutHandle, a2DCharInfoString, aCharInfoBuffer2DSize, COORD{ 0, 0 }, &aDrawRegion);
    }

    void Draw2DString(const std::wstring &aString, COORD aCharInfoBuffer2DSize, SMALL_RECT aDrawRegion, WORD aColor = grey)
    {
        int size = aCharInfoBuffer2DSize.X * aCharInfoBuffer2DSize.Y;

        assert(aString.length() >= aCharInfoBuffer2DSize.X * aCharInfoBuffer2DSize.Y);
        CHAR_INFO *buffer = new CHAR_INFO[aCharInfoBuffer2DSize.X * aCharInfoBuffer2DSize.Y];

        for (int i = 0; i < size; ++i)
        {
            buffer[i].Char.UnicodeChar = aString[i];
            buffer[i].Attributes = aColor;
        }

        WriteConsoleOutputW(locStdOutHandle, buffer, aCharInfoBuffer2DSize, COORD{ 0, 0 }, &aDrawRegion);
    }

    void DrawString(const std::string_view &aString, int aPositionX, int aPositionY, WORD aColor)
    {
        assert(aString.length() <= INT16_MAX);

        CHAR_INFO *buffer = new CHAR_INFO[aString.length()];

        for (int i = 0; i < aString.length(); ++i)
        {
            buffer[i].Char.UnicodeChar = static_cast<wchar_t>(aString[i]);
            buffer[i].Attributes = aColor;
        }

        COORD position = { static_cast<short>(aPositionX), static_cast<short>(aPositionY) };
        SMALL_RECT drawRegion = { position.X, position.Y, position.X + static_cast<short>(aString.length()), position.Y + 1 };

        WriteConsoleOutputW(locStdOutHandle, buffer, COORD{ static_cast<short>(aString.length()), 1 }, COORD{ 0, 0 }, &drawRegion);

        delete[] buffer;
    }

    void DrawString(const std::wstring_view &aString, int aPositionX, int aPositionY, WORD aColor)
    {
        assert(aString.length() <= INT16_MAX);

        CHAR_INFO *buffer = new CHAR_INFO[aString.length()];

        for (int i = 0; i < aString.length(); ++i)
        {
            buffer[i].Char.UnicodeChar = static_cast<wchar_t>(aString[i]);
            buffer[i].Attributes = aColor;
        }

        COORD position = { static_cast<short>(aPositionX), static_cast<short>(aPositionY) };
        SMALL_RECT drawRegion = { position.X, position.Y, position.X + static_cast<short>(aString.length()), position.Y + 1 };

        WriteConsoleOutputW(locStdOutHandle, buffer, COORD{ static_cast<short>(aString.length()), 1 }, COORD{ 0, 0 }, &drawRegion);

        delete[] buffer;
    }

    void DrawChar(wchar_t aCharacter, COORD aPosition, WORD aColor)
    {
        CHAR_INFO c;
        c.Char.UnicodeChar = aCharacter;
        c.Attributes = aColor;

        SMALL_RECT drawRegion = { aPosition.X, aPosition.Y, aPosition.X + 1, aPosition.Y + 1 };
        WriteConsoleOutputW(locStdOutHandle, &c, COORD{ 1, 1 }, COORD{ 0, 0 }, &drawRegion);
    }

    void DrawChar(wchar_t aCharacter, int aPositionX, int aPositionY, WORD aColor)
    {
        DrawChar(aCharacter, COORD{ static_cast<short>(aPositionX), static_cast<short>(aPositionY) }, aColor);
    }

    void Print(const std::wstring_view &aString, WORD aColor)
    {
        DWORD charsWritten;
        SetTextStyle(aColor);
        WriteConsoleW(locStdOutHandle, aString.data(), static_cast<DWORD>(aString.length()), &charsWritten, nullptr);
        ResetTextStyle();
        assert(charsWritten == aString.length());
    }

    void Print(const std::string_view &aString, WORD aColor)
    {
        DWORD charsWritten;
        SetTextStyle(aColor);
        WriteConsoleA(locStdOutHandle, aString.data(), static_cast<DWORD>(aString.length()), &charsWritten, nullptr);
        ResetTextStyle();
        assert(charsWritten == aString.length());
    }

    void Print(const char *aString, WORD aColor)
    {
        DWORD charsWritten;
        DWORD length = static_cast<DWORD>(strlen(aString));
        SetTextStyle(aColor);
        WriteConsoleA(locStdOutHandle, aString, length, &charsWritten, nullptr);
        ResetTextStyle();
        assert(charsWritten == length);
    }

    void Print(const wchar_t *aString, WORD aColor)
    {
        DWORD charsWritten;
        DWORD length = static_cast<DWORD>(wcslen(aString));
        SetTextStyle(aColor);
        WriteConsoleW(locStdOutHandle, aString, length, &charsWritten, nullptr);
        ResetTextStyle();
        assert(charsWritten == length);
    }

    void Print(wchar_t aCharacter, WORD aColor)
    {
        DWORD charsWritten;
        SetTextStyle(aColor);
        WriteConsoleW(locStdOutHandle, &aCharacter, 1, &charsWritten, nullptr);
        ResetTextStyle();
        assert(charsWritten == 1);
    }

    void Clear()
    {
        system("cls");
    }

    void FastClear(int aStartX, int aStartY, WORD color)
    {
        TerminalSize screenSize = GetSize();

        int length = (screenSize.width - aStartX) + screenSize.width * (screenSize.height - aStartY);
        COORD startPosition = COORD{ static_cast<short>(aStartX), static_cast<short>(aStartY) };

        DWORD written;
        FillConsoleOutputCharacter(locStdOutHandle, L' ', length, startPosition, &written);
        FillConsoleOutputAttribute(locStdOutHandle, color, length, startPosition, &written);
        SetConsoleCursorPosition(locStdOutHandle, COORD{ static_cast<short>(aStartX), static_cast<short>(aStartY) });
    }

    void FastClear(int aStartX, int aStartY, int aCount, WORD color)
    {
        TerminalSize screenSize = GetSize();

        COORD startPosition = COORD{ static_cast<short>(aStartX), static_cast<short>(aStartY) };

        DWORD written;
        FillConsoleOutputCharacter(locStdOutHandle, L' ', aCount, startPosition, &written);
        FillConsoleOutputAttribute(locStdOutHandle, color, aCount, startPosition, &written);
        SetConsoleCursorPosition(locStdOutHandle, COORD{ static_cast<short>(aStartX), static_cast<short>(aStartY) });
    }

    void ClearRow(int aY, WORD color)
    {
        TerminalSize screenSize = GetSize();

        COORD startPosition = COORD{ 0, static_cast<short>(aY) };

        DWORD written;
        FillConsoleOutputCharacter(locStdOutHandle, L' ', screenSize.width, startPosition, &written);
        FillConsoleOutputAttribute(locStdOutHandle, color, screenSize.width, startPosition, &written);
    }

    void ClearInputBuffer()
    {
        FlushConsoleInputBuffer(locStdOutHandle);

        while (_kbhit())
        {
            _getch();
        }
    }

    int TextLength(const std::wstring_view &s)
    {
        int length = 0;

        for (int i = 0; i < s.size(); ++i)
        {
            if (s[i] == 0x1b)
            {
                if (i + 1 < s.size())
                {
                    if (s[i + 1] == '[')
                    {
                        i += 2;

                        while (!iswalpha(s[i]) || i == '@' || i == '?' || i == '=')
                        {
                            i += 1;
                        }
                    }
                    else if (s[i + 1] == 'M' || s[i + 1] == '7' || s[i + 1] == '8')
                    {
                        i += 2;
                    }
                }
                continue;
            }
            else
            {
                length++;
            }
        }

        return length;
    }

    int TextLength(const std::string_view &s)
    {
        int length = 0;

        for (int i = 0; i < s.size(); ++i)
        {
            if (s[i] == 0x1b)
            {
                if (i + 1 < s.size())
                {
                    if (s[i + 1] == '[')
                    {
                        i += 2;
                        while (!isalpha(i) || i == '@' || i == '?' || i == '=')
                        {
                            i += 1;
                        }
                    }
                    else if (s[i + 1] == 'M' || s[i + 1] == '7' || s[i + 1] == '8')
                    {
                        i += 2;
                    }
                }
                continue;
            }
            else
            {
                length++;
            }
        }

        return length;
    }
}