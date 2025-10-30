#ifndef PROJECT_BASE_V1_1_BUFF_HPP
#define PROJECT_BASE_V1_1_BUFF_HPP

#include "../h/Sem.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../lib/hw.h"

class Buff {
public:
    Buff();

    void put_char(char c);
    char get_char();

    char get_char_from_thread();

    uint64 getCurrCapacity() const { return currCapacity; }
//    void decreaseCurrCapacity() { currCapacity--; }
//    void increaseCurrCapacity() { currCapacity++; }

    static const uint64 maxCap = 2048;

private:
    Sem *mutex;
    Sem *itemAvailable, *spaceAvailable;

    char buffer[maxCap];

    uint64 head, tail, currCapacity;

};

#endif //PROJECT_BASE_V1_1_BUFF_HPP