#pragma once

#include <assert.h>

namespace CommonUtilities
{
    static class LinkedList
    {
    public:
        template<typename T>
        void LinkedListInsert(T *&aNode, T *aNewNode, T* T::*aNodeNext)
        {
            if (aNode == nullptr)
            {
                aNode = aNewNode;
            }
            else
            {
                T *end = nullptr;
                T *current = aNewNode;
                while (current != nullptr)
                {
                    end = current;
                    current = *current->aNodeNext;
                }
                
                *end->aNodeNext = *aNode->aNodeNext;
                aNode->aNodeNext = aNewNode;
            }
        }
        
        template<typename T>
        void LinkedListAdd(T *&aList, T *aNewNode, T::*aNodeNext)
        {
            if (aList == nullptr)
            {
                aList = aNewNode;
            }
            else
            {
                T *current = aList;
                T *end = nullptr;
                
                while (current != nullptr)
                {
                    end = current;
                    current = *current->aNodeNext;
                }
                
                *end->aNodeNext = aNewNode;
            }
        }
        
        template<typename T>
        void LinkedListRemove(T *&aList, T *aNode, T::*aNodeNext)
        {
            if (aList == aNode)
            {
                aList = *aList->aNodeNext;
            }
            else
            {
                T *prev = aList;
                T *current = aList;
                
                while (current != aNode)
                {
                    prev = current;
                    current = *current->aNodeNext;
                    
                    if (current == nullptr)
                    {
                        return;
                    }
                }
                
                *prev->aNodeNext = *current->aNodeNext;
            }
        }
    }
}
