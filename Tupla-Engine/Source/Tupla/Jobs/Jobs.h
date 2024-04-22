#pragma once
#include "ThreadPool.h"

namespace Tupla
{
	class Jobs
	{
	public:
		static void Await();
		static void Queue(const std::function<void()>& aFunc);
		static void ForEach(u64 aJobDepth, u64 aCount, const std::function<void(u64, u64)>& aFunc);
		static void Start();
		static void Stop();

	private:
		static inline ThreadPool myPool{};
	};
}
