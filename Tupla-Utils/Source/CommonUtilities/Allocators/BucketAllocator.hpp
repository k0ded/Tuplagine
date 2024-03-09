#pragma once

#include "FreeListAllocator.hpp"

namespace CommonUtilities
{
	// TODO: Benchmark
	template <size_t BucketSizeBytes, size_t BucketStartCount>
	class BucketAllocator
	{
		static_assert(false, "BucketAllocator needs testing!");

	public:
		// NOTE: In practice you will NOT get aBucketSize bytes of usable memory per bucket!
		// This is because the allocator uses some that memory to keep track of its buckets.

		struct BucketNode
		{
			BucketNode *nextBucket;
			FreeListAllocator<BucketSizeBytes> buffer;
		};

		BucketAllocator::BucketAllocator()
		{
			myBuckets = AllocateBuckets(BucketStartCount);
			// TODO: Maybe save pointer to last bucket for easy bucket append?
		}

		BucketNode *AllocateBuckets(size_t aBucketCount)
		{
			BucketNode* buckets = reinterpret_cast<BucketNode*>(malloc(sizeof(BucketNode) * aBucketCount));

			if (buckets)
			{
				BucketNode* currentBucket = buckets;
				for (int i = 0; i < aBucketCount; ++i)
				{
					currentBucket->buffer.Initialize();
					currentBucket->nextBucket = currentBucket + 1;
					currentBucket = currentBucket->nextBucket;
				}

				BucketNode* lastBucket = currentBucket - 1;
				lastBucket->nextBucket = nullptr;
			}

			return buckets;
		}

		void* Allocate(size_t aBytes)
		{
			assert(aBytes < BucketSizeBytes);
			assert(myBuckets != nullptr);

			BucketNode *bucket = myBuckets;
			BucketNode *previousBucket = nullptr;

			void* memory = nullptr;

			while (memory == nullptr)
			{
				if (bucket == nullptr)
				{
					// BucketAllocator and FreeListAllocator haven't been tested yet,
					// so for now we allocate from the new buffer immediately and assert if it fails
					// (in case you, for example, try to allocate all the memory in the bucket but the bucket needs extra memory for metadata)
					if (previousBucket)
					{
						bucket = previousBucket->nextBucket = AllocateBuckets(1);
						memory = bucket->buffer.Allocate(aBytes);
						assert(memory != nullptr);
					}
					break;
				}

				memory = bucket->buffer.Allocate(aBytes);
				previousBucket = bucket;
				bucket = bucket->nextBucket;
			}

			return memory;
		}

		template <typename T>
		T* Allocate(size_t aCount = 1)
		{
			return reinterpret_cast<T*>(Allocate(sizeof(T) * aCount));
		}

		void Free(void *aMemory)
		{
			BucketNode *bucket = myBuckets;

			while (bucket != nullptr && bucket->buffer.Owns(aMemory))
			{
				bucket = bucket->nextBucket;
			}

			if (bucket != nullptr)
			{
				bucket->buffer.Free(aMemory); 
			}
#ifdef _DEBUG
			else
			{
				std::cerr << "Memory probably leaked! Attempted to free memory that does not belong to this allocator!\n" __FILE__ ":" __LINE__;
			}
#endif
		}

	private:
		BucketNode* myBuckets = nullptr;
	};
}