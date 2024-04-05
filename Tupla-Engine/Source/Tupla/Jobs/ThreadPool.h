#pragma once
#include <array>
#include <bitset>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

#include "Job.h"

namespace JobSystem
{
    class ThreadPool {
    public:
        ThreadPool(std::unordered_map<std::thread::id, int>* aThreadIdToJob);
    
        void Start();
        void QueueJob(const Job& aJob);
        void DoWork();
        void Flush();
        void Stop();
        bool IsWorking();
        bool HasScheduledJobs();
        
        std::unordered_map<uint32_t, std::atomic<int>>& GetReadAccesses() { return myReadAccesses; }
        std::unordered_map<uint32_t, std::atomic<int>>& GetWriteAccesses() { return myWriteAccesses; }
        unsigned int GetThreadCount() const;

    private:
        [[noreturn]] void ThreadLoop();
    
        bool should_terminate = false;           // Tells threads to stop looking for jobs
        std::mutex myQueueMutex;                  // Prevents data races to the job queue
        std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
        std::vector<std::thread> threads;
        std::queue<Job> jobs;

        const unsigned int myThreadCount = std::thread::hardware_concurrency();
        std::unordered_map<std::thread::id, int>* myThreadIdToJob;

        // With these we can just queue up a bunch of jobs and it will run whichever it can without creating data races
        std::unordered_map<uint32_t, std::atomic<int>> myReadAccesses{};
        std::unordered_map<uint32_t, std::atomic<int>> myWriteAccesses{};


        std::mutex myBookkeepingMutex;
        int m_TotalActiveJobs { 0 };
        int m_TotalQueuedJobs { 0 };
    };
}
