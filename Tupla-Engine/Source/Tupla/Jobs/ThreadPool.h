#pragma once
#include <mutex>
#include <queue>
#include <vector>

namespace Tupla
{
    class ThreadPool {
    public:
        ThreadPool();
    
        void Start();
        void QueueJob(const std::function<void()>& aJob);
        void DoWork(bool aMarkAsWorking = true); // Set to false for jobs that run every frame until the end of the frame (ex. Update)
        void Stop();
        bool IsWorking();
        void Await();

        unsigned int GetThreadCount() const;

    private:
        [[noreturn]] void ThreadLoop();
    
        bool myShouldTerminate = false;           // Tells threads to stop looking for jobs
        std::mutex myQueueMutex;                  // Prevents data races to the job queue
        std::condition_variable myMutexCondition; // Allows threads to wait on new jobs or termination 
        std::condition_variable myAwaitCondition; // Allows threads to await for completion.
        std::vector<std::thread> myThreads;
        std::queue<std::function<void()>> myQueuedJobs;
        int myActiveJobs = 0;

        const unsigned int myThreadCount = std::thread::hardware_concurrency();
    };
}
