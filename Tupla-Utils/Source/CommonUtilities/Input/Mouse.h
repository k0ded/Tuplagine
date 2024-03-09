#pragma once
#include <Windows.h>

namespace CommonUtilities
{
    class Mouse
    {
    public:
        Mouse();
        
        void Update(short aX, short aY, short aWheelDelta);

        [[nodiscard]] POINT GetPosition() const; // TODO: Use CU vector instead of Windows structs 
        [[nodiscard]] POINT GetPositionDelta() const;
        [[nodiscard]] short GetWheelDelta() const;
    private:
        short myPositionX;
        short myPositionY;
        short myLastPositionX;
        short myLastPositionY;
        short myWheelDelta;
    }; 
}
