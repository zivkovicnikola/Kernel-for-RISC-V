#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:
    static MemoryAllocator& getInstance();

    void* m_alloc(size_t size);
    int m_free(void* ptr);

    size_t m_get_free_space();
    size_t m_get_largest_free_block();

private:
    struct Block {
        size_t size;
        bool free;
        Block* next;
        Block* prev;
        Block* prevPhys;
        Block* nextPhys;
    };
    static Block* freeHead;

    static bool initialized;
    void init();
    void removeFromFree(Block* blk);
    void insertIntoFree(Block* blk);
    Block* findFirstFit(size_t needed);
    Block* split(Block* blk, size_t neededBytes);
    Block* mergeBlocks(Block* blk, Block* other);
    Block* tryToJoin(Block* blk);


};

#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP