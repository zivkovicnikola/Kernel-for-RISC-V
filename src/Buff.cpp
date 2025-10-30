#include "../h/Buff.hpp"
#include "../h/syscall_c.h"

Buff::Buff() : head(0), tail(0), currCapacity(0) {
    mutex = new Sem(1);
    itemAvailable = new Sem(0);
    spaceAvailable = new Sem(maxCap);
}

void Buff::put_char(char c) {
    spaceAvailable->wait();
    mutex->wait();
    buffer[tail] = c;
    tail = (tail + 1) % maxCap;
    currCapacity++;
    mutex->signal();
    itemAvailable->signal();
}

char Buff::get_char() {
    itemAvailable->wait();
    mutex->wait();
    char retC = buffer[head];
    head = (head + 1) % maxCap;
    currCapacity--;
    mutex->signal();
    spaceAvailable->signal();
    return retC;
}

char Buff::get_char_from_thread() {
    sem_wait((sem_t)itemAvailable);
    sem_wait((sem_t)mutex);
    char retC = buffer[head];
    head = (head + 1) % maxCap;
    currCapacity--;
    sem_signal((sem_t)mutex);
    sem_signal((sem_t)spaceAvailable);
    return retC;
}