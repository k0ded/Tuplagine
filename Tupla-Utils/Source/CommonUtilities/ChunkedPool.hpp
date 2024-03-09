#pragma once

#include <bitset>

namespace CommonUtilities
{
    template<typename T, size_t ChunkSize, size_t MinChunks>
    class ChunkedPool
    {
    public:
        ChunkedPool()
        {
            first = new Chunk();
        
            Chunk* current = first;
            for (int i = 0; i < MinChunks - 1; ++i)
            {
                current->next = new Chunk();
                current->next->prev = current;
                current = current->next;
            }
        
            chunkCount = MinChunks;
        }

        T* Allocate(int* aOutIndex = nullptr)
        {
            Chunk* chunk = first; 
            int index = 0;

            while (chunk)
            {
                for (int i = 0; i < ChunkSize; ++i)
                {
                    if (chunk->freeList[i])
                    {
                        chunk->freeList[i] = 0;

                        if (aOutIndex)
                            *aOutIndex = index;
                        return &chunk->items[i];
                    }
                    index++;
                }
            
                if (chunk->next == nullptr)
                {
                    last = chunk->next = new Chunk();
                    if (!chunk->next)
                    {
                        return nullptr;
                    } 
                    else
                    {
                        chunk->next->prev = chunk;
                        chunkCount += 1;
                    }
                }
            
                chunk = chunk->next;
            }

            return nullptr;
        }
    
        // Allocates aCount contiguous objects. Must still be freed individually.
        T* Allocate(size_t aCount, int* aOutStartIndex = nullptr)
        {
            assert(aCount < ChunkSize);
        
            Chunk* chunk = first;
            int index = 0;
            int chunkIndex = 0;

            while (chunk)
            {
                for (int i = 0; i < ChunkSize; ++i)
                {
                    if (chunk->freeList[i] && ChunkSize - i >= aCount)
                    {
                        int j;
                        for (j = 0; j < aCount; ++j)
                        {
                            if (!chunk->freeList[i + j])
                            {
                                i += j - 1;
                                break;
                            }
                        }
                    
                        if (j == aCount)
                        {
                            for (j = 0; j < aCount; ++j)
                            {
                                chunk->freeList[i + j] = false;
                            }
                        
                            if (aOutStartIndex)
                                *aOutStartIndex = chunkIndex * ChunkSize + i;

                            return chunk[i];
                        }
                    }
                }
            
                if (chunk->next == nullptr)
                {
                    last = chunk->next = new Chunk();
                    if (!chunk->next)
                    {
                        return nullptr;
                    } 
                    else
                    {
                        chunk->next->prev = chunk;
                        chunkCount += 1;
                    }
                }
            
                chunk = chunk->next;
                chunkIndex++;
            }
        }
    
        // Same as allocate, but placed default constructed object into memory
        T* New(int* aOutIndex = nullptr)
        {
            T* object = Allocate(aOutIndex);
            if (object)
            {
                new (object) T();
            }
        
            return object;
        }
    
        T* New(size_t aCount, int* aOutIndex = nullptr)
        {
            T* objects = Allocate(aCount, aOutIndex);
            if (objects)
            {
                for (int i = 0; i < aCount; ++i)
                {
                    new (objects[i]) T();
                }
            }
        
            return objects;
        }
    
        void Free(T* aObject)
        {
            Chunk* chunk = first;
            int chunkIndex = 0;
            while (chunk)
            {
                if (aObject >= &chunk->items[0] && aObject <= &chunk->items[ChunkSize - 1])
                {
                    size_t index = static_cast<size_t>(aObject - &chunk->items[0]);
                    chunk->freeList[index] = 1;
                
                    if (chunk->next = nullptr && chunk->freeList.all() && chunkCount > MinChunks)
                    {
                        Coalesce();
                        return;
                    }
                    else
                    {
#ifdef _DEBUG
                        memset(aObject, sizeof(T), 0xDEADBEEF);
#else
                        memset(aObject, sizeof(T), 0);
#endif
                    }

                    break;
                }
                else
                {
                    chunk = chunk->next;
                    chunkIndex++;
                }
            }
        }

        void Delete(T* aObject)
        {
            Chunk* chunk = first;
            int chunkIndex = 0;
            while (chunk)
            {
                if (aObject >= &chunk->items[0] && aObject <= &chunk->items[ChunkSize - 1])
                {
                    size_t index = static_cast<size_t>(aObject - &chunk->items[0]);
                    chunk->freeList[index] = 1;

                    if (chunk->next == nullptr && chunk->freeList.all() && chunkCount > MinChunks)
                    {
                        assert(chunk == last);

                        Coalesce();
                        return;
                    }
                    else
                    {
                        aObject->~T();
                    }

                    break;
                }
                else
                {
                    chunk = chunk->next;
                    chunkIndex++;
                }
            }
        }

        void Coalesce()
        {
            Chunk* chunk = last;
            while (chunk && chunk->freeList.all() && chunkCount > MinChunks)
            {
                Chunk* prev = chunk->prev;
                if (prev)
                {
                    prev->next = nullptr;
                }

                delete chunk;
                chunk = prev;
                chunkCount--;
            }
        }

        size_t Size() const
        {
            return chunkCount * ChunkSize;
        }

        size_t Chunks() const
        {
            return chunkCount;
        }

        T* operator[](size_t aIndex)
        {
            size_t chunkIndex = aIndex / ChunkSize;

            // FIXME: Should this assert or not?? Probably but then we should also do a TryGet(index) kinda thing.
            //assert(chunkIndex < chunkCount);

            Chunk* chunk = first;
            for (int i = 0; i < chunkIndex && chunk != nullptr; ++i)
            {
                chunk = chunk->next;
            }

            if (!chunk)
                return nullptr;

            if (chunk->freeList[aIndex % ChunkSize])
                return nullptr;

            return &chunk->items[aIndex % ChunkSize];
        }

        T* operator[](int aIndex)
        {
            return this->operator[]((size_t)aIndex);
        }

    private:
        static_assert(MinChunks >= 1);

        struct Chunk
        {
            T items[ChunkSize];
            std::bitset<ChunkSize> freeList = std::bitset<ChunkSize>{}.set();
            Chunk* next = nullptr;
            Chunk* prev = nullptr;
            ~Chunk()
            {
                for (int i = 0; i < ChunkSize; ++i)
                {
                    if (!freeList[i])
                    {
                        items[i].~T();
                    }
                }
            }
        };
    
        Chunk* first;
        Chunk* last;
        size_t chunkCount;
    };
}