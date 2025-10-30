#include "../h/Print.hpp"
#include "../h/syscall_c.h"
#include "../h/TCB.hpp"
#include "../h/MyConsole.hpp"
#include "../h/syscall_cpp.hpp"

extern void userMain();

void userWrapper(void* s) {
    userMain();
    ((Semaphore*)s)->signal();
}

int main() {

    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB* mainThr = new TCB(nullptr, nullptr, nullptr);
    TCB::running = mainThr;
    mainThr->setState(RUNNING);

    MyConsole::init();

    Semaphore userSem(0);

    Thread userThread(userWrapper, (void*)&userSem);
    Thread consoleThread(MyConsole::wrapper, nullptr);

    consoleThread.start();
    userThread.start();

    userSem.wait();

    return 0;
}