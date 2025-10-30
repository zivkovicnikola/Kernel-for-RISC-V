#include "../h/Sem.hpp"
#include "../h/Scheduler.hpp"
#include "../h/ThreadState.hpp"

Sem::~Sem() {
    close();
}

int Sem::wait() {
    if (closed) return -1;

    if (--val < 0)
        block();

    return 0;
}

int Sem::signal() {
    if (closed) return -1;

    if (++val <= 0)
        deblock();

    return 0;
}

void Sem::block() {
    TCB::running->setState(States::BLOCKED);
    blocked.addLast(TCB::running);
    TCB::dispatch();
}

void Sem::deblock() {
    TCB* tmp = blocked.removeFirst();
    tmp->setState(States::READY);
    Scheduler::put(tmp);
}

int Sem::close() {
    if (closed) return -1;
    closed = true;

    while (blocked.peekFirst()) {
        deblock();
    }

    val = 0;

    return 0;
}
