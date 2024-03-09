#pragma once

namespace CommonUtilities
{
    class StopWatch
    {
    public:
        StopWatch();
        StopWatch(const StopWatch &aTimer) = delete;
        StopWatch& operator=(const StopWatch &aTimer) = delete;

        void Update();

        float GetDeltaTime() const;
        double GetTotalTime() const;

    private:
        double myStart;
        double myLast;
        double myNow;
    };
}

namespace CU = CommonUtilities;
