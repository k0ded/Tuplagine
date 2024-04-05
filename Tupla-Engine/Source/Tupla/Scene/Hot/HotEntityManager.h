#pragma once
#include <queue>

#include "HotDefines.h"

namespace Tupla
{
	class HotEntityManager
	{
	public:
		HotEntityManager()
		{
			// Initialize the queue with all possible entity IDs
			for (HOTEntity entity = 0; entity < MAX_ENTITIES; ++entity)
			{
				mAvailableEntities.push(entity);
			}
		}

		HOTEntity CreateEntity()
		{
			assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

			// Take an ID from the front of the queue
			HOTEntity id = mAvailableEntities.front();
			mAvailableEntities.pop();
			++mLivingEntityCount;

			return id;
		}

		void DestroyEntity(HOTEntity entity)
		{
			assert(entity < MAX_ENTITIES && "Entity out of range.");

			// Invalidate the destroyed entity's signature
			mSignatures[entity].reset();

			// Put the destroyed ID at the back of the queue
			mAvailableEntities.push(entity);
			--mLivingEntityCount;
		}

		void SetSignature(HOTEntity entity, Signature signature)
		{
			assert(entity < MAX_ENTITIES && "Entity out of range.");

			// Put this entity's signature into the array
			mSignatures[entity] = signature;
		}

		Signature GetSignature(HOTEntity entity)
		{
			assert(entity < MAX_ENTITIES && "Entity out of range.");

			// Get this entity's signature from the array
			return mSignatures[entity];
		}

	private:
		// Queue of unused entity IDs
		std::queue<HOTEntity> mAvailableEntities{};

		// Array of signatures where the index corresponds to the entity ID
		std::array<Signature, MAX_ENTITIES> mSignatures{};

		// Total living entities - used to keep limits on how many exist
		uint32_t mLivingEntityCount{};
	};
}
