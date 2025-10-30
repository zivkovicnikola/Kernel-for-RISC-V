#ifndef PROJECT_BASE_V1_1_SLEEPINGTHREADS_HPP
#define PROJECT_BASE_V1_1_SLEEPINGTHREADS_HPP

#include "../h/TCB.hpp"
#include "../lib/hw.h"

class SleepingThreads {
public:
    static void putToSleep();
    static void awakeThread();

    static void tick();

private:
    static TCB* head;
};

#endif //PROJECT_BASE_V1_1_SLEEPINGTHREADS_HPP