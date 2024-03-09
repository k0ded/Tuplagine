#include "Mouse.h"

namespace CommonUtilities
{
    Mouse::Mouse(): myPositionX(0), myPositionY(0), myLastPositionX(0), myLastPositionY(0), myWheelDelta(0)
    {}

    void Mouse::Update(short aX, short aY, short aWheelDelta)
    {
        myLastPositionX = myPositionX;
        myLastPositionY = myPositionY;
        myPositionX = aX;
        myPositionY = aY;
        myWheelDelta = aWheelDelta;
    }
    
    POINT Mouse::GetPosition() const
    {
        return { myPositionX, myPositionY };
    }

    POINT Mouse::GetPositionDelta() const
    {
        return { myPositionX - myLastPositionX, myPositionY - myLastPositionY };
    }

    short Mouse::GetWheelDelta() const
    {
        return myWheelDelta;
    }
}
