#pragma once

#include "Job.h"
#include "ThreadPool.h"

namespace JobSystem
{
    /**
     * \brief A wrapper around a ThreadPool which allows functions to be ran with dependencies.
     * \note May create an infinite loop if a circular dependency is involved!!!
     */
    class JobManager
    {
    public:
        JobManager();

        /**
         * \brief Schedules a job to be performed in parallel after the conditions are met.
         * \param aJob is a function intended to run in parallel which also has the possibility of having dependencies
         */
        void ScheduleJob(const Job& aJob);

        /**
         * \brief Finds all scheduled jobs who's conditions are met and schedules them to be performed in parallel.
         */
        void BurstJobs();

        /**
         * \brief Figures out whether or not the ThreadPool is working and or has scheduled work.
         * \return if the JobManager is currently working or not
         */
        bool HasJobs();
        
        /**
         * \brief Flushes the list of finished jobs.
         */
        void Flush();

        void Await();
        ThreadPool& GetInternalPool();

       private:
        std::unordered_map<std::thread::id, int> myThreadIdToJob;
    
        std::mutex myScheduleMutex;
        std::vector<Job> myScheduledJobs;
        std::atomic<int> m_JobsScheduled;
        ThreadPool myThreadPool;
    };
}
