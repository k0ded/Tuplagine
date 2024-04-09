#include "tgpch.h"
#include "JobManager.h"

#include <mutex>

#define SINGLE_THREADED 0

namespace JobSystem
{

    JobManager::JobManager(): myThreadPool(&myThreadIdToJob)
    {
        myThreadPool.Start();
    }

    JobManager::~JobManager()
    {
        myThreadPool.Stop();
    }

    /**
     * \brief Schedules a job and bursts them in memory access order. DOES NOT GUARANTEE THAT THE ORDER OF MEMORY ACCESS WILL BE THE SAME
     * \param aJob to schedule.
     */
    void JobManager::ScheduleJob(const Job& aJob)
    {
        {
            std::unique_lock<std::mutex> lock(myScheduleMutex);
            myScheduledJobs.emplace_back(aJob);
            ++m_JobsScheduled;
        }
    }

    void JobManager::BurstJobs()
    {
#if SINGLE_THREADED
        for (auto& job : myScheduledJobs)
        {
            job.Invoke();
            --m_JobsScheduled;
        }
#else

        {
            std::unique_lock<std::mutex> lock(myScheduleMutex);

            for (int i = 0; i < myScheduledJobs.size(); ++i)
            {
                const auto job = myScheduledJobs[i];

                auto& r = myThreadPool.GetReadAccesses();
                auto& w = myThreadPool.GetWriteAccesses();
                bool blocked = false;

                // If we're trying to read. We cannot access any data that is being written.
                for (int i = 0; i < job.m_RDependencies.Count(); ++i)
                {
                    if(w[job.m_RDependencies[i]] > 0)
                    {
                        blocked = true;
                        break;
                    }
                }

                // If we're trying to write. We cannot access any data that is being read or written.
                for (int i = 0; i < job.m_WDependencies.Count() || blocked; ++i)
                {
                    if (w[job.m_WDependencies[i]] > 0 || r[job.m_WDependencies[i]] > 0)
                    {
                        blocked = true;
                        break;
                    }
                }

                if(!blocked)
                {
                    myThreadPool.QueueJob(job);
                    myScheduledJobs.erase(myScheduledJobs.begin() + i);
                    --i;
                    --m_JobsScheduled;
                }
            }
        }
#endif
    }

    void JobManager::Await()
    {
        while (HasJobs())
        {
            BurstJobs();
        }
    }

    ThreadPool& JobManager::GetInternalPool()
    {
        return myThreadPool;
    }

    void JobManager::Flush()
    {
        myThreadPool.Flush();
    }

    bool JobManager::HasJobs()
    {
        return m_JobsScheduled > 0 || myThreadPool.IsWorking();
    }
}
