#pragma once

#include <Windows.h>
#include <assert.h>
#include <string>
#include <string_view>
//#include <format>

#define BLACK "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define GREY "\x1b[37m"
#define INTENSEBLACK "\x1b[90m"
#define INTENSERED "\x1b[91m"
#define INTENSEGREEN "\x1b[92m"
#define INTENSEYELLOW "\x1b[93m"
#define INTENSEBLUE "\x1b[94m"
#define INTENSEMAGENTA "\x1b[95"
#define INTENSECYAN "\x1b[96m"
#define WHITE "\x1b[97m"
#define BACKGROUNDBLACK "\x1b[40m"
#define BACKGROUNDRED "\x1b[41m"
#define BACKGROUNDGREEN "\x1b[42m"
#define BACKGROUNDYELLOW "\x1b[43m"
#define BACKGROUNDBLUE "\x1b[44m"
#define BACKGROUNDMAGENTA "\x1b[45m"
#define BACKGROUNDCYAN "\x1b[46m"
#define BACKGROUNDGREY "\x1b[47m"
#define BACKGROUNDINTENSEBLACK "\x1b[100m"
#define BACKGROUNDINTENSERED "\x1b[101m"
#define BACKGROUNDINTENSEGREEN "\x1b[102m"
#define BACKGROUNDINTENSEYELLOW "\x1b[103m"
#define BACKGROUNDINTENSEBLUE "\x1b[104m"
#define BACKGROUNDINTENSEMAGENTA "\x1b[105m"
#define BACKGROUNDINTENSECYAN "\x1b[106m"
#define BACKGROUNDGREY "\x1b[47m"
#define BACKGROUNDWHITE "\x1b[107m"

namespace Terminal
{
    constexpr WORD brightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    constexpr WORD grey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    constexpr WORD white  = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

    struct TerminalSize
    {
        short width, height;
    };

    void Initialize();

    bool SetTextFont(const std::wstring& aFaceName, short aFontSize, UINT aFontFamily = TMPF_TRUETYPE | TMPF_VECTOR);

    bool SetTextFont(CONSOLE_FONT_INFOEX* aFontInfo);

    bool GetTextFont(CONSOLE_FONT_INFOEX* aOutConsoleFontInfoEx);

    void SetTextStyle(WORD aTextStyle);

    void ResetTextStyle();

    COORD GetCursorPosition();

    void SetCursorPosition(COORD aCursorPosition);

    void SetCursorPosition(int aXPosition, int aYPosition);

    void MoveCursor(int aDeltaX, int aDeltaY);

    void PushCursorPosition(int aPositionX, int aPositionY);

    COORD PopCursorPosition();

    void ShowCursor();

    void HideCursor();

    bool IsCursorVisible();

    TerminalSize GetSize();

    void SetCharacter(short aCellX, short aCellY, wchar_t aCharacter, WORD aTextStyle = grey);

    void Draw2DBuffer(const CHAR_INFO *a2DCharInfoString, COORD aCharInfoBuffer2DSize, SMALL_RECT aDrawRegion);

    void Draw2DString(const std::wstring& aString, COORD aCharInfoBuffer2DSize, SMALL_RECT aDrawRegion, WORD aColor);

    void DrawString(const std::wstring_view &aString, int aPositionX, int aPositionY, WORD aColor = Terminal::grey);
    void DrawString(const std::string_view &aString, int aPositionX, int aPositionY, WORD aColor = Terminal::grey);
    void DrawChar(wchar_t aCharacter, COORD aPosition, WORD aColor = Terminal::grey);

    void DrawChar(wchar_t aCharacter, int aPositionX, int aPositionY, WORD aColor);

    void Print(const std::wstring_view& aString, WORD aColor = Terminal::grey);
    void Print(const std::string_view& aString, WORD aColor = grey);

    void Print(const char* aString, WORD aColor = Terminal::grey);
    void Print(const wchar_t* aString, WORD aColor = Terminal::grey);

    void Print(wchar_t aCharacter, WORD aColor = Terminal::grey);

    void Clear();

    void FastClear(int aStartX = 0, int aStartY = 0, WORD color = 0);

    void FastClear(int aStartX, int aStartY, int aCount, WORD color);

    void ClearRow(int aY, WORD color = 0);

    void ClearInputBuffer();

    int TextLength(const std::wstring_view &s);
    int TextLength(const std::string_view &s);

    inline void DrawStringCentered(const std::string &aString, COORD position, WORD color = Terminal::grey)
    {
        short aStringWidth = static_cast<short>(aString.length());
        position.X -= aStringWidth / 2;

        DrawString(aString, position.X, position.Y, color);
    }

    inline void DrawStringCentered(const std::wstring& aString, COORD position, WORD color = grey)
    {
        short aStringWidth = static_cast<short>(aString.length());
        position.X -= aStringWidth / 2;

        DrawString(aString, position.X, position.Y, color);
    }

    inline short GetWidth()
    {
        return GetSize().width;
    }

    inline short GetHeight()
    {
        return GetSize().height;
    }

}