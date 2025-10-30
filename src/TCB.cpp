#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_c.h"

TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCnt = 0;
uint64 TCB::nextId = 0;

TCB* TCB::createThread(Body body, void* args, uint64* stack) {
    return new TCB(body, args, stack);
}

void TCB::dispatch() {
    TCB* old = running;
    if (!old->getIsFinished() && old->getState() != States::BLOCKED && old->getState() != States::SLEEPING) {
        old->setState(READY);
        Scheduler::put(old);
    }
    running = Scheduler::get();
    running->setState(RUNNING);

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->args);
    thread_exit();
}

void TCB::join(TCB* handle) {
    while(!handle->isFinished) {
        dispatch();
    }
}
