#include "../h/SleepingThreads.hpp"

TCB* SleepingThreads::head = nullptr;

void SleepingThreads::putToSleep() {

    if (TCB::running->getSleepingTime() == 0) {
        TCB::running->setState(States::READY);
        return;
    }

    TCB::running->setState(States::SLEEPING);
    TCB* prev = nullptr;
    time_t sleeping_time = 0;
    for (TCB* curr = head; curr; curr = curr->nextAsleep) {
        if (sleeping_time + curr->getSleepingTime() > TCB::running->getSleepingTime()) {
            TCB::running->nextAsleep = curr;
            curr->setSleepingTime(sleeping_time + curr->getSleepingTime() - TCB::running->getSleepingTime());
            break;
        }

        sleeping_time += curr->getSleepingTime();
        prev = curr;
    }

    if (prev) {
        prev->nextAsleep = TCB::running;
        TCB::running->setSleepingTime( TCB::running->getSleepingTime() - sleeping_time);
    }
    else
        head = TCB::running;
}

void SleepingThreads::awakeThread() {

    if (!head) return;

    TCB *curr = nullptr;
    TCB *next = nullptr;
    for (curr = head; curr && curr->getSleepingTime() == 0; curr = next) {
        curr->setState(States::READY);
        next = curr->nextAsleep;
        curr->nextAsleep = nullptr;
        Scheduler::put(curr);
    }

    head = curr;
}

void SleepingThreads::tick() {
    if (head) {
        head->sleepingTime--;
        awakeThread();
    }
}
