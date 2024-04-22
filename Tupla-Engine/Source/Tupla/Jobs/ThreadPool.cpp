#include "tgpch.h"
#include "ThreadPool.h"

namespace Tupla
{
    ThreadPool::ThreadPool()
    {
    }

    void ThreadPool::Start()
    {
        for (uint32_t index = 0; index < myThreadCount; ++index) {
            myThreads.emplace_back(&ThreadPool::ThreadLoop, this);
        }
    }

    void ThreadPool::QueueJob(const std::function<void()>& aJob)
    {
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            myQueuedJobs.push(aJob);
        }

        myMutexCondition.notify_one();
    }

    void ThreadPool::DoWork(bool aMarkAsWorking)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            myMutexCondition.wait(lock, [this] {
                return !myQueuedJobs.empty() || myShouldTerminate;
            });
            if (myShouldTerminate) {
                return;
            }
            job = myQueuedJobs.front();
            myQueuedJobs.pop();

            if(aMarkAsWorking)
				myActiveJobs++;

            // Regardless since the job popped might not be marked as "working".
            myAwaitCondition.notify_all();
        }

        job();

        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            if(aMarkAsWorking)
            {
                myActiveJobs--;
                myAwaitCondition.notify_all();
            }
        }
    }

    void ThreadPool::Stop()
    {
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            myShouldTerminate = true;
        }
        myMutexCondition.notify_all();
        myAwaitCondition.notify_all();
        for (std::thread& activeThread : myThreads) {
            activeThread.join();
        }
        myThreads.clear();
    }

    bool ThreadPool::IsWorking()
    {
        std::unique_lock<std::mutex> lock(myQueueMutex);
        return !myQueuedJobs.empty() || myActiveJobs > 0;
    }

    void ThreadPool::Await()
    {
        std::unique_lock lock(myQueueMutex);
        myAwaitCondition.wait(lock, [this]
            {
                return myQueuedJobs.empty() && myActiveJobs <= 0;
            });
    }

    unsigned int ThreadPool::GetThreadCount() const
    {
        return myThreadCount;
    }

    void ThreadPool::ThreadLoop()
    {
        while (!myShouldTerminate)
        {
            DoWork();
        }
    }
}
