#ifndef PROJECT_BASE_V1_1_SEM_HPP
#define PROJECT_BASE_V1_1_SEM_HPP

#include "../h/TCB.hpp"
#include "../h/Queue.hpp"
#include "../lib/hw.h"

class Sem {
public:
    Sem(uint64 init = 1) : val(init), closed(false) {}

    ~Sem();

    int wait();
    int signal();

    void block();
    void deblock();

    int close();

private:
    int val;
    bool closed;
    Queue<TCB> blocked;
};


#endif //PROJECT_BASE_V1_1_SEM_HPP