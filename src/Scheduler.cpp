#include "../h/Scheduler.hpp"

Queue<TCB> Scheduler::readyThreadQueue;

TCB* Scheduler::get() {
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(TCB* tcb) {
    readyThreadQueue.addLast(tcb);
}