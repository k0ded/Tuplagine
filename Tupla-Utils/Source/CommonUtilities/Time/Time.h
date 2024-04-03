#pragma once
#include <algorithm>

#include "StopWatch.h"

namespace Time
{
    inline CU::StopWatch stopWatch;
    inline float timeScale = 1.0f;
    inline double locTotalTimeOffset = 0.0;
    
    inline void Update()
    {
        stopWatch.Update();
        locTotalTimeOffset += stopWatch.GetDeltaTime() * (1.0 - timeScale);
    }
    
    inline float DeltaTime()
    {
        return std::min(stopWatch.GetDeltaTime() * timeScale, 0.03f);
    }
    
    inline double TotalTime()
    {
        return stopWatch.GetTotalTime() - locTotalTimeOffset;
    }
}
