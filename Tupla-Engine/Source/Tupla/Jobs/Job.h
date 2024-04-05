#pragma once
#include <functional>

#include "JobDefines.h"
#include "CommonUtilities/FixedSizeList.hpp"

namespace JobSystem
{
    class JobManager;
}

/**
 * \note First MAX_COMPONENTS (See HotDefines.h) reserved for ECS read/write accesses.
 */
class Job
{
public:
    Job() = default;
    Job(const std::function<void()>& aFunction,
        const CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& aWriteDependencies = {},
        const CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& aReadDependencies = {});
    virtual ~Job() = default;
    
    virtual void Invoke() const;

    CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES> m_WDependencies {};
    CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES> m_RDependencies {};

protected:
    std::function<void()> m_Function;
};
