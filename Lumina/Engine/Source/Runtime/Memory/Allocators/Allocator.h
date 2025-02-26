#pragma once

#include "Memory/Memory.h"


template <typename T>
class PoolAllocator
{
private:
    struct Block {
        Block* next; // Pointer to the next free block
    };

    Block* m_FreeList;          // The free list of blocks
    size_t m_BlockSize;         // Size of the objects being allocated
    size_t m_BlockCount;        // Total number of blocks available
    void* m_Memory;            // Pointer to the memory pool

public:
    // Constructor: Allocates memory for the pool
    PoolAllocator(size_t blockCount = 1024)
        : m_FreeList(nullptr), m_BlockSize(sizeof(T)), m_BlockCount(blockCount) {
        // Allocate a block of memory large enough to hold all objects
        m_Memory = ::operator new(m_BlockSize * m_BlockCount);

        // Initialize free list
        Block* currentBlock = reinterpret_cast<Block*>(m_Memory);
        for (size_t i = 0; i < m_BlockCount - 1; ++i) {
            currentBlock->next = reinterpret_cast<Block*>(reinterpret_cast<uint8_t*>(currentBlock) + m_BlockSize);
            currentBlock = currentBlock->next;
        }
        currentBlock->next = nullptr;  // Last block points to null
        m_FreeList = reinterpret_cast<Block*>(m_Memory); // Set free list to the first block
    }

    // Destructor: Frees the memory pool
    ~PoolAllocator() {
        ::operator delete(m_Memory);
    }

    // Allocate memory for a new object of type T, forwarding arguments to the constructor of T
    template <typename... Args>
    T* Allocate(Args&&... args) {
        if (!m_FreeList) {
            return nullptr;  // No more memory available
        }

        // Get a free block from the free list
        Block* block = m_FreeList;
        m_FreeList = m_FreeList->next; // Move the free list pointer to the next block

        // Return the memory pointer for the allocated object
        return new (reinterpret_cast<void*>(block)) T(std::forward<Args>(args)...);  // Forward arguments to the constructor
    }

    // Deallocate memory for an object of type T
    void Deallocate(T* ptr) {
        if (!ptr) return;

        // Destroy the object by calling its destructor
        ptr->~T();

        // Return the block back to the free list
        Block* block = reinterpret_cast<Block*>(ptr);
        block->next = m_FreeList;  // Link the block to the free list
        m_FreeList = block;        // Set the free list to the newly deallocated block
    }

    // Get the total number of blocks in the pool
    size_t GetBlockCount() const {
        return m_BlockCount;
    }

    // Get the size of each block (size of T)
    size_t GetBlockSize() const {
        return m_BlockSize;
    }
};
