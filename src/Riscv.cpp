#include "../h/Riscv.hpp"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Sem.hpp"
#include "../h/ThreadState.hpp"
#include "../h/SleepingThreads.hpp"
#include "../h/MyConsole.hpp"
#include "../lib/console.h"
#include "../h/Print.hpp"

void Riscv::popSppSpie() {

    __asm__ volatile ("csrc sstatus, %0" : : "r"(SSTATUS_SPP));

    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap() {

    uint64 scause = r_scause();
    volatile uint64 sepc = r_sepc();
    volatile uint64 sstatus = r_sstatus();
    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL) {
        // interrupt: no, cause code: environment call from S-mode (9) or U-mode (8)

        sepc += 4;

        uint64 a0;
        __asm__ volatile("ld %0, 10 * 8(fp)" : "=r"(a0));

        switch (a0) {
            case 0x01: { // mem_alloc
                size_t blocks;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(blocks));
                size_t bytes = blocks * MEM_BLOCK_SIZE;
                void* ptr = MemoryAllocator::getInstance().m_alloc(bytes);
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(ptr));
                break;
            }
            case 0x02: { // mem_free
                void* ptr;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(ptr));

                int rv = MemoryAllocator::getInstance().m_free(ptr);
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(rv));
                break;
            }
            case 0x03: { // mem_get_free_space
                size_t val = MemoryAllocator::getInstance().m_get_free_space();
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(val));
                break;
            }
            case 0x04: { // mem_get_largest_free_block
                size_t val = MemoryAllocator::getInstance().m_get_largest_free_block();
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(val));
                break;
            }
            case 0x11: { // thread_create
                TCB** handle;
                TCB::Body start_routine;
                void* arg;
                uint64* stack;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(handle));
                __asm__ volatile("ld %0, 12 * 8(fp)" : "=r"(start_routine));
                __asm__ volatile("ld %0, 13 * 8(fp)" : "=r"(arg));
                __asm__ volatile("ld %0, 14 * 8(fp)" : "=r"(stack));

                TCB* newTCB = TCB::createThread(start_routine, arg, stack);
                *handle = newTCB;

                if (!(*handle)) __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(-1));
                else __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(0));
                break;
            }
            case 0x12: { // thread_exit
                TCB::running->setIsFinished(true);
                TCB::running->setState(States::FINISHED);
                TCB::timeSliceCnt = 0;
                TCB::dispatch();
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(0));
                break;
            }
            case 0x13: { // thread_dispatch
                TCB::timeSliceCnt = 0;
                TCB::dispatch();
                break;
            }
            case 0x14: { // get_thread_id
                uint64 id = TCB::running->id;
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(id));
                TCB::dispatch();
                break;
            }
            case 0x15: { // thread_join
                TCB* handle;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(handle));
                TCB::join(handle);
                break;
            }
            case 0x21: { // sem_open
                Sem** handle;
                uint64 init;

                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(handle));
                __asm__ volatile("ld %0, 12 * 8(fp)" : "=r"(init));

                Sem* newSem = new Sem(init);
                *handle = newSem;

                if (!(*handle)) __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(-1));
                else __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(0));
                break;
            }
            case 0x22: { // sem_close
                Sem* handle;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(handle));

                if (!handle) __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(-1));
                else {
                    size_t val = handle->close();
                    __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(val));
                }
                break;
            }
            case 0x23: { // sem_wait
                Sem* handle;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(handle));

                if (!handle) __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(-1));
                else {
                    size_t val = handle->wait();
                    __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(val));
                }
                break;
            }
            case 0x24: { // sem_signal
                Sem* handle;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(handle));

                if (!handle) __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(-1));
                else {
                    size_t val = handle->signal();
                    __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(val));
                }
                break;
            }
            case 0x31: { // time_sleep
                time_t time;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(time));

                if (time == 0) __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(-1));
                else {
                    TCB::running->setSleepingTime(time);
                    TCB::timeSliceCnt = 0;
                    SleepingThreads::putToSleep();
                    TCB::dispatch();
                    __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(0));
                }
                break;
            }
            case 0x41: { // getc
                char character = MyConsole::inputBuff->get_char();
                __asm__ volatile ("sd %0, 10 * 8(fp)" :: "r"(character));
                break;
            }
            case 0x42: { // putc
                char character;
                __asm__ volatile("ld %0, 11 * 8(fp)" : "=r"(character));

                MyConsole::outputBuff->put_char(character);
                break;
            }
        }
    }
    else if (scause == 0x8000000000000001UL) {
        // interrupt: yes, cause code: supervisor software interrupt (timer)

        TCB::timeSliceCnt++;
        mc_sip(SIP_SSIP);
        SleepingThreads::tick();
        if (TCB::timeSliceCnt >= TCB::running->getTimeSlice()) {
            TCB::timeSliceCnt = 0;
            TCB::dispatch();
        }
    }
    else if (scause == 0x8000000000000009UL) {
        // interrupt: yes, cause code: supervisor external interrupt (console)
        int interruptNumber = plic_claim();

        if (interruptNumber == CONSOLE_IRQ) {
            while (MyConsole::inputBuff->getCurrCapacity() < Buff::maxCap && (*((volatile char*)(CONSOLE_STATUS)) & CONSOLE_RX_STATUS_BIT)) {
                char c = (*(volatile char *) CONSOLE_RX_DATA);
                MyConsole::inputBuff->put_char(c);
            }
        }
        plic_complete(interruptNumber);
    }
    else {
        // unexpected trap cause

        uint64 sepc = r_sepc();
        sysPrintString("scause = ");
        sysPrintInteger(scause);
        sysPrintString("\n");
        sysPrintString("sepc = ");
        sysPrintInteger(sepc);
        sysPrintString("\n");

        while (MyConsole::outputBuff->getCurrCapacity() > 0 && (*((volatile char *) (CONSOLE_STATUS)) & CONSOLE_TX_STATUS_BIT)) {
            char c = MyConsole::outputBuff->get_char();
            *(volatile char*)CONSOLE_TX_DATA = c;
        }

        while(1);
    }

    w_sepc(sepc);
    w_sstatus(sstatus);

}
