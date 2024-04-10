#include "InputHandler.h"

#include <string>

#include "KeyCode.h"

namespace CommonUtilities
{
    InputHandler::InputHandler() = default;

    bool InputHandler::UpdateEvents(const UINT aMessage, const WPARAM aWParam, const LPARAM aLParam)
    {
        int virtualKey;
        const UINT scancode = (aLParam & 0x00ff0000) >> 16;
        const int extended  = (aLParam & 0x01000000) != 0;

        switch (aWParam) {
        case VK_SHIFT:
            virtualKey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
            break;
        case VK_CONTROL:
            virtualKey = extended ? VK_RCONTROL : VK_LCONTROL;
            break;
        case VK_MENU:
            virtualKey = extended ? VK_RMENU : VK_LMENU;
            break;
        default:
            // not a key we map from generic to left/right specialized
            //  just return it.
            virtualKey = static_cast<int>(aWParam);
            break;    
        }
        
        switch (aMessage)
        {
        case WM_KEYUP:
            UpdateKeyCode(virtualKey, true);
            break;
        case WM_KEYDOWN:
            UpdateKeyCode(virtualKey, false);
            break;
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHOVER:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            UpdateMouse(aMessage, aWParam, aLParam);
            break;
        default:
            return false;
        }

        return true;
    }

    void InputHandler::UpdateInput()
    {
        myMouse.Update(myMousePositionX, myMousePositionY, myMouseWheelDelta);
        myMouseWheelDelta = 0;

        for (int i = 0; i < 256; ++i)
        {

            myIsHeld[i] = myIsDown[i] || myIsHeld[i];
            myIsDown[i] = myIsDownFresh[i];
            myIsDownFresh[i] = false;

            if(myIsUp[i])
            {
                myIsHeld[i] = false;
                myIsUpFresh[i] = false;
                myIsUp[i] = false;
            }
            else if(myIsUpFresh[i] && myIsHeld[i])
            {
                myIsUp[i] = true;
            }
        }
    }

    bool InputHandler::GetKeyDown(const int aKeyCode) const
    {
        return myIsDown[aKeyCode] && !myIsHeld[aKeyCode];
    }

    bool InputHandler::GetKeyDown(KeyCode aKeyCode) const
    {
        return GetKeyDown(static_cast<int>(aKeyCode));
    }

    bool InputHandler::GetKey(const int aKeyCode) const
    {
        return (myIsHeld[aKeyCode] || myIsDown[aKeyCode]) && !myIsUp[aKeyCode];
    }

    bool InputHandler::GetKey(KeyCode aKeyCode) const
    {
        return GetKey(static_cast<int>(aKeyCode));
    }

    bool InputHandler::GetKeyUp(const int aKeyCode) const
    {
       return myIsHeld[aKeyCode] && myIsUp[aKeyCode];
    }

    bool InputHandler::GetKeyUp(KeyCode aKeyCode) const
    {
        return GetKeyUp(static_cast<int>(aKeyCode));
    }

    Mouse InputHandler::GetMouse() const
    {
        return myMouse;
    }

    void InputHandler::UpdateMouse(const UINT& aMessage, const WPARAM& aWParam, const LPARAM& aLParam)
    {
        switch (aMessage)
        {
        case WM_XBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
            UpdateMouseXButton(aWParam, false);
            break;
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
            UpdateMouseButton(KeyCode::MOUSE_LBUTTON, false);
            break;
        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
            UpdateMouseButton(KeyCode::MOUSE_MBUTTON, false);
            break;
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
            UpdateMouseButton(KeyCode::MOUSE_RBUTTON, false);
            break;

        case WM_XBUTTONUP:
            UpdateMouseXButton(aWParam, true);
            break;
        case WM_LBUTTONUP:
            UpdateMouseButton(KeyCode::MOUSE_LBUTTON, true);
            break;
        case WM_MBUTTONUP:
            UpdateMouseButton(KeyCode::MOUSE_MBUTTON, true);
            break;
        case WM_RBUTTONUP:
            UpdateMouseButton(KeyCode::MOUSE_RBUTTON, true);
            break;
        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL:
            myMouseWheelDelta += GET_WHEEL_DELTA_WPARAM(aWParam);
            return;
        default:
            break;
        }

        myMousePositionX = static_cast<short>(aLParam & 0xffff);
        myMousePositionY = static_cast<short>(aLParam >> 16);
    }

    void InputHandler::UpdateMouseButton(KeyCode aCode, const bool aUp)
    {
        UpdateKeyCode(static_cast<int>(aCode), aUp);
    }

    void InputHandler::UpdateMouseXButton(const WPARAM aWParam, const bool aUp)
    {
        const bool isForward = GET_XBUTTON_WPARAM(aWParam) & XBUTTON2;
        KeyCode keyCode = isForward ? KeyCode::MOUSE_BUTTON_FORWARD : KeyCode::MOUSE_BUTTON_BACKWARD;

        UpdateKeyCode(static_cast<int>(keyCode), aUp);
    }

    void InputHandler::UpdateKeyCode(int aCode, bool aUp)
    {
    	(aUp ? myIsUpFresh : myIsDownFresh)[aCode] = true;
    }
}
