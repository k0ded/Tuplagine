#include "tgpch.h"
#include "Jobs.h"

namespace Tupla
{
	void Jobs::Await()
	{
		myPool.Await();
	}

	void Jobs::Queue(const std::function<void()>& aFunc)
	{
		myPool.QueueJob(aFunc);
	}

	void Jobs::ForEach(const u64 aJobDepth, const u64 aCount, const std::function<void(u64 depth, u64 count)>& aFunc)
	{
		for (u64 i = 0; i < aJobDepth; ++i)
		{
			myPool.QueueJob([i, aCount, aFunc]
				{
					for (u64 j = 0; j < aCount; ++j)
					{
						aFunc(i, j);
					}
				}
			);
		}
	}

	void Jobs::Start()
	{
		myPool.Start();
	}

	void Jobs::Stop()
	{
		myPool.Stop();
	}
}
