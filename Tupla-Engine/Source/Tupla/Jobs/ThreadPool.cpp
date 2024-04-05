#include "tgpch.h"
#include "ThreadPool.h"

namespace JobSystem
{
    ThreadPool::ThreadPool(std::unordered_map<std::thread::id, int>* aThreadIdToJob) : myThreadIdToJob(aThreadIdToJob)
    {
    }

    void ThreadPool::Start()
    {
        for (uint32_t index = 0; index < myThreadCount; ++index) {
            threads.emplace_back(&ThreadPool::ThreadLoop, this);
        }

        myThreadIdToJob->reserve(myThreadCount);
    }

    void ThreadPool::QueueJob(const Job& aJob)
    {
        {
            std::unique_lock<std::mutex> lock(myBookkeepingMutex);
            ++m_TotalQueuedJobs;
        }

        for (int i = 0; i < aJob.m_RDependencies.Count(); ++i)
        {
            ++myReadAccesses[aJob.m_RDependencies[i]];
        }

        for (int i = 0; i < aJob.m_WDependencies.Count(); ++i)
        {
            ++myWriteAccesses[aJob.m_WDependencies[i]];
        }

        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            jobs.push(aJob);
            
        }
    
        mutex_condition.notify_one();
    }

    void ThreadPool::DoWork()
    {
        Job job;
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
            });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
            --m_TotalQueuedJobs;
        }

        {
            std::unique_lock<std::mutex> lock(myBookkeepingMutex);
            --m_TotalQueuedJobs;
            ++m_TotalActiveJobs;
        }

        job.Invoke();

        {
            std::unique_lock<std::mutex> lock(myBookkeepingMutex);
            --m_TotalActiveJobs;
        }

        for (int i = 0; i < job.m_RDependencies.Count(); ++i)
        {
            --myReadAccesses[job.m_RDependencies[i]];
        }

        for (int i = 0; i < job.m_WDependencies.Count(); ++i)
        {
            --myWriteAccesses[job.m_WDependencies[i]];
        }
    }

    void ThreadPool::Flush()
    {
        myReadAccesses.clear();
        myWriteAccesses.clear();
    }

    void ThreadPool::Stop()
    {
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            should_terminate = true;
        }
        mutex_condition.notify_all();
        for (std::thread& activeThread : threads) {
            activeThread.join();
        }
        threads.clear();
    }

    bool ThreadPool::IsWorking()
    {
        std::unique_lock<std::mutex> lock(myBookkeepingMutex);
        return m_TotalActiveJobs != 0 || m_TotalQueuedJobs > 0;
    }

    unsigned int ThreadPool::GetThreadCount() const
    {
        return myThreadCount;
    }

    bool ThreadPool::HasScheduledJobs()
    {
        bool hasJobsScheduled;
    
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            hasJobsScheduled = !jobs.empty();
        }

        return hasJobsScheduled;
    }

    void ThreadPool::ThreadLoop()
    {
        while (!should_terminate)
        {
            DoWork();
        }
    }
}
