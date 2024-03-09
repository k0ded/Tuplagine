#pragma once
#include <bitset>
#include <Windows.h>

#include "KeyCode.h"
#include "Mouse.h"

namespace CommonUtilities
{
    class InputHandler
    {
    public:
        InputHandler();

        bool UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
        void UpdateInput();

        /// \brief 
        /// \param aKeyCode the number value of \see KeyCode
        /// \return Returns true on the first frame that the key was pressed on
        [[nodiscard]] bool GetKeyDown(int aKeyCode) const;
        [[nodiscard]] bool GetKeyDown(KeyCode aKeyCode) const;
        [[nodiscard]] bool GetKey(int aKeyCode) const;
        [[nodiscard]] bool GetKey(KeyCode aKeyCode) const;
        [[nodiscard]] bool GetKeyUp(int aKeyCode) const;
        [[nodiscard]] bool GetKeyUp(KeyCode aKeyCode) const;
        [[nodiscard]] Mouse GetMouse() const;
    private:
        std::bitset<256> myIsUpTicked;
        std::bitset<256> myIsUp;
        std::bitset<256> myIsHeld;
        std::bitset<256> myIsDown;
        std::bitset<256> myIsDownFresh;
        
        Mouse myMouse;
        short myMousePositionX;
        short myMousePositionY;
        short myMouseWheelDelta;

        void UpdateMouse(const UINT& aMessage, const WPARAM& aWParam, const LPARAM& aLParam);
        void UpdateMouseButton(KeyCode aCode, bool aUp);
        void UpdateMouseXButton(WPARAM aWParam, bool aUp);
        void UpdateKeyCode(int aCode, bool aUp);
    };
}
