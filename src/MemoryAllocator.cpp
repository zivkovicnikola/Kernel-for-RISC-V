#include "../h/MemoryAllocator.hpp"

bool MemoryAllocator::initialized = false;
MemoryAllocator::Block* MemoryAllocator::freeHead = nullptr;

MemoryAllocator& MemoryAllocator::getInstance() {
    static MemoryAllocator instance;
    return instance;
}

void MemoryAllocator::init() {
    if (initialized) return;
    initialized = true;

    freeHead = (Block*)HEAP_START_ADDR;
    freeHead->size = (size_t)((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - MEM_BLOCK_SIZE + 1);
    freeHead->next = nullptr;
    freeHead->prev = nullptr;
    freeHead->free = true;
    freeHead->prevPhys = nullptr;
    freeHead->prevPhys = nullptr;
}

void* MemoryAllocator::m_alloc(size_t size) {

    init();

    size_t neededSize = size % MEM_BLOCK_SIZE ? (size + (MEM_BLOCK_SIZE - size % MEM_BLOCK_SIZE)) : size; // in bytes
    Block* newBlk = findFirstFit(neededSize);
    if (!newBlk) return nullptr;

    removeFromFree(newBlk);

    if (newBlk->size >= neededSize + (sizeof(Block) + MEM_BLOCK_SIZE)) {
        Block* rest = split(newBlk, neededSize);
        insertIntoFree(rest);
    }

    newBlk->free = false;

    return (void*)(newBlk + 1);
}

int MemoryAllocator::m_free(void *ptr) {

    if (!ptr) return -1;

    Block* blk = ((Block*)ptr) - 1;

    blk->free = true;

    blk = tryToJoin(blk);
    insertIntoFree(blk);

    return 0;

}

MemoryAllocator::Block* MemoryAllocator::findFirstFit(size_t needed) {
    for (Block* curr = freeHead; curr; curr = curr->next) {
        if (curr->size >= needed) return curr;
    }
    return nullptr;
}

void MemoryAllocator::removeFromFree(MemoryAllocator::Block *blk) {
    if (!blk) return;

    if (blk->prev) blk->prev->next = blk->next;
    else freeHead = blk->next;

    if (blk->next) blk->next->prev = blk->prev;
    blk->prev = blk->next = nullptr;
}

void MemoryAllocator::insertIntoFree(MemoryAllocator::Block *blk) {
    blk->prev = nullptr;
    blk->next = freeHead;
    if (freeHead) freeHead->prev = blk;
    freeHead = blk;
}

MemoryAllocator::Block* MemoryAllocator::split(MemoryAllocator::Block *blk, size_t neededBytes) {

    const size_t totalOrig = sizeof(Block) + blk->size;
    const size_t totalKeep = sizeof(Block) + neededBytes;
    const size_t totalRest = totalOrig - totalKeep;

    if (totalOrig < MEM_BLOCK_SIZE + sizeof(Block)) return nullptr;

    Block* rest = (Block*)((char*)(blk + 1) + neededBytes);
    rest->size = totalRest - sizeof(Block);
    rest->free = true;
    rest->prevPhys = blk;
    rest->nextPhys = blk->nextPhys;
    rest->prev = rest->next = nullptr;

    if (blk->nextPhys) blk->nextPhys->prevPhys = rest;
    blk->nextPhys = rest;

    blk->size = neededBytes;
    return rest;

}

MemoryAllocator::Block* MemoryAllocator::mergeBlocks(MemoryAllocator::Block *blk, MemoryAllocator::Block *other) {

    const size_t totalSize = sizeof(Block) * 2 + blk->size + other->size;

    blk->size = totalSize - sizeof(Block);
    blk->nextPhys = other->nextPhys;
    if (other->nextPhys) other->nextPhys->prevPhys = blk;

    other->prevPhys = other->nextPhys = nullptr;
    other->prev = other->next = nullptr;
    other->free = false;

    return blk;

}

MemoryAllocator::Block* MemoryAllocator::tryToJoin(MemoryAllocator::Block *blk) {

    Block* left = blk->prevPhys;
    Block* right = blk->nextPhys;

    if (left && left->free) {
        removeFromFree(left);
        blk = mergeBlocks( left, blk);
    }
    if (right && right->free) {
        removeFromFree(right);
        blk = mergeBlocks(blk, right);
    }

    return blk;
}

size_t MemoryAllocator::m_get_free_space() {
    init();
    size_t total = 0;
    for (Block* curr = freeHead; curr; curr = curr->next) {
        total += curr->size;
    }
    return total;
}

size_t MemoryAllocator::m_get_largest_free_block() {
    init();
    size_t max = 0;
    for (Block* curr = freeHead; curr; curr = curr->next) {
        if (curr->size > max) max = curr->size;
    }
    return max;
}
