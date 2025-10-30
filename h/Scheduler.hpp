#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

#include "../h/Queue.hpp"
class TCB;

class Scheduler {
public:
    static TCB* get();
    static void put(TCB* tcb);

private:
    static Queue<TCB> readyThreadQueue;

};

#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP