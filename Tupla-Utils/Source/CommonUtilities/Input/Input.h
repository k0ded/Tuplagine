#pragma once
#include "InputHandler.h"

class Input
{
public:
    static bool UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
    static void Update();
    
    [[nodiscard]] static bool GetKeyDown(int aKeyCode);
    [[nodiscard]] static bool GetKeyDown(CommonUtilities::KeyCode aKeyCode);
    [[nodiscard]] static bool GetKey(int aKeyCode);
    [[nodiscard]] static bool GetKey(CommonUtilities::KeyCode aKeyCode);
    [[nodiscard]] static bool GetKeyUp(int aKeyCode);
    [[nodiscard]] static bool GetKeyUp(CommonUtilities::KeyCode aKeyCode);
    [[nodiscard]] static CommonUtilities::Mouse GetMouse();
private:
    static CommonUtilities::InputHandler myInputHandler;
};
