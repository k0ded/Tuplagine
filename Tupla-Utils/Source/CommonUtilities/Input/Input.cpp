#include "Input.h"

CommonUtilities::InputHandler Input::myInputHandler = CommonUtilities::InputHandler();

bool Input::UpdateEvents(const UINT aMessage, const WPARAM aWParam, const LPARAM aLParam)
{
    return myInputHandler.UpdateEvents(aMessage, aWParam, aLParam);
}

void Input::Update()
{
    myInputHandler.UpdateInput();
}

bool Input::GetKeyDown(const int aKeyCode)
{
    return myInputHandler.GetKeyDown(aKeyCode);
}

bool Input::GetKeyDown(const CommonUtilities::KeyCode aKeyCode)
{
    return myInputHandler.GetKeyDown(aKeyCode);
}

bool Input::GetKey(const int aKeyCode)
{
    return myInputHandler.GetKey(aKeyCode);
}

bool Input::GetKey(const CommonUtilities::KeyCode aKeyCode)
{
    return myInputHandler.GetKey(aKeyCode);
}

bool Input::GetKeyUp(const int aKeyCode)
{
    return myInputHandler.GetKey(aKeyCode);
}

bool Input::GetKeyUp(const CommonUtilities::KeyCode aKeyCode)
{
    return myInputHandler.GetKey(aKeyCode);
}

CommonUtilities::Mouse Input::GetMouse()
{
    return myInputHandler.GetMouse();
}

