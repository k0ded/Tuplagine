#pragma once

// TODO: Implement alignment?
template <size_t BucketSizeBytes>
class FreeListAllocator
{
    static_assert(false, "FreeListAllocator needs testing!");

public:
    struct FreeListNode
    {
        // blockSize includes the space used by FreeListNode
        size_t blockSize;
        FreeListNode* next;
    };
    
    struct FreeListBlockHeader
    {
        // blockSize includes the space used by FreeListBlockHeader
        size_t blockSize;
    };
    
    FreeListAllocator()
    {
        Initialize();
    }
    
    void Initialize()
    {
        myFirstNode = reinterpret_cast<FreeListNode*>(myBuffer);
        myFirstNode->blockSize = BucketSizeBytes;
        myFirstNode->next = nullptr;
    }

    // TODO: Placement policy (first/best-fit, currently first is used)
    // TODO: What is the appropriate behaviour if there is no memory remaining? Return null or assert?
    void* Allocate(size_t aBytes)
    {
        assert(aBytes > 0);
        
        // Ensure there will be enough space for a Node when we free this memory.
        if (sizeof(FreeListBlockHeader) + aBytes < sizeof(FreeListNode))
        {
            aBytes += sizeof(FreeListNode) - sizeof(FreeListBlockHeader);
        }
        
        FreeListNode* node = myFirstNode;
        FreeListNode* previousNode = nullptr;
        
        const size_t requiredSpace = aBytes + sizeof(FreeListBlockHeader);
        
        while (node != nullptr)
        {
            if (node->blockSize >= requiredSpace)
            {
                break;
            }
            
            previousNode = node;
            node = node->next;
        }
        
        if (node == nullptr)
        {
            return nullptr;
        }

        // We found a suitable chunk of memory.

        // Create and insert new free node after the chunk we're allocating now, if there is enough memory left.
        size_t remainingSpace = node->blockSize - requiredSpace - sizeof(FreeListNode);
        if (remainingSpace > 0)
        {
            FreeListNode* nextNode = reinterpret_cast<FreeListNode*>(reinterpret_cast<char*>(node) + aBytes + sizeof(FreeListBlockHeader));
            nextNode->next = node->next;
            nextNode->blockSize = remainingSpace;
            node->next = nextNode;
        }

        // Remove 'node' from list of free memory chunks and replace with the new node.
        {
            if (previousNode != nullptr)
            {
                previousNode->next = node->next;
            }
            else // There was no previous node, so 'node' must be the first node.
            {
                myFirstNode = node->next;
            }
        }


        // We place the header where the node used to be since that memory is no longer in use.
        FreeListBlockHeader* header = reinterpret_cast<FreeListBlockHeader*>(node);
        header->blockSize = requiredSpace;

        void* memory = (reinterpret_cast<char*>(header) + sizeof(FreeListBlockHeader));
        return memory;
    }
    
    void Free(void *aMemory)
    {
        if (aMemory == nullptr)
        {
            return;
        }
        
        FreeListBlockHeader* header = reinterpret_cast<FreeListBlockHeader*>(reinterpret_cast<char*>(aMemory) - sizeof(FreeListBlockHeader));
        size_t blockSize = header->blockSize;
        
        FreeListNode* newNode = reinterpret_cast<FreeListNode*>(header);
        newNode->blockSize = blockSize;
        newNode->next = nullptr;
        
        // Find where to insert new node
        FreeListNode* next = myFirstNode;
        FreeListNode* previous = nullptr;
        while (next != nullptr && newNode < next)
        {
            previous = next;
            next = next->next;
        }
        
        // After the while loop, next is located after newNode in memory, meaning it should follow after, so we insert the new node behind it.
        // Insert new node
        newNode->next = next;
        if (previous != nullptr)
        {
            previous->next = newNode;
        }
        else
        {
            myFirstNode = newNode;
        }
        
        // Combine adjacent free memory blocks
        if (next != nullptr && reinterpret_cast<void*>(reinterpret_cast<char*>(newNode) + newNode->blockSize) == next)
        {
            // Add sizeof(FreeListNode) since the 'next' node is no longer in use (otherwise you leak memory)
            newNode->blockSize += next->blockSize + sizeof(FreeListNode);
            newNode->next = next->next;
        }
        
        if (previous != nullptr && reinterpret_cast<void*>(reinterpret_cast<char*>(previous) + previous->blockSize) == newNode)
        {
            previous->blockSize += newNode->blockSize + sizeof(FreeListNode);
            previous->next = newNode->next;
        }
    }
    
    bool Owns(void *aMemory)
    {
        void* start = reinterpret_cast<void*>(myBuffer);
        void* end   = reinterpret_cast<void*>(myBuffer + BucketSizeBytes);
        return aMemory >= start && aMemory < end;
    }
    
private:
    unsigned char myBuffer[BucketSizeBytes];
    FreeListNode *myFirstNode = reinterpret_cast<FreeListNode*>(myBuffer);
};