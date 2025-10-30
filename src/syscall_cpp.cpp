#include "../h/syscall_cpp.hpp"
#include "../h/syscall_c.h"

Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::~Thread() {
    //delete myHandle;  ?
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread() : myHandle(nullptr), body(wrapperThread), arg(this) {}

void Thread::wrapperThread(void *thread) {
    if (thread) ((Thread*)thread)->run();
}

int Thread::getID() {
    return get_thread_id();
}

void Thread::join() {
    thread_join(myHandle);
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) : period(period) {}

void PeriodicThread::terminate() {
    period = 0;
}

void PeriodicThread::run() {
    if (period > 0) {
        time_sleep(period);
        periodicActivation();
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char character) {
    ::putc(character);
}
