#include "tgpch.h"
#include "Job.h"

Job::Job(const std::function<void()>& aFunction,
    const CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& aWriteDependencies,
    const CU::FixedSizeList<uint8_t, MAX_THREADED_ACCESSES>& aReadDependencies):
    m_Function(aFunction),
    m_WDependencies(aWriteDependencies),
	m_RDependencies(aReadDependencies)
{
}

void Job::Invoke() const
{
    m_Function();
}
