#include "StopWatch.h"

#include <chrono>

namespace CommonUtilities
{
    
    StopWatch::StopWatch()
    {
        myStart = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        myNow = myStart;
        myLast = myStart;
    }

    void StopWatch::Update()
    {
        myLast = myNow;
        myNow = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    float StopWatch::GetDeltaTime() const
    {
        return static_cast<float>(myNow - myLast);
    }

    double StopWatch::GetTotalTime() const
    {
        return myNow - myStart;
    }
}
