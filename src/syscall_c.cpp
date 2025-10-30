#include "../h/syscall_c.h"

void* mem_alloc(size_t size) {

    size_t numOfBlocks = size % MEM_BLOCK_SIZE ? (size + (MEM_BLOCK_SIZE - size % MEM_BLOCK_SIZE)) / MEM_BLOCK_SIZE : size / MEM_BLOCK_SIZE;

    __asm__ volatile ("mv a1, %0" : : "r"(numOfBlocks));
    __asm__ volatile ("mv a0, %0" : : "r"(0x01));

    __asm__ volatile ("ecall");

    uint64 retAddr;
    __asm__ volatile ("mv %0, a0" : "=r"(retAddr));

    return (void*)retAddr;
}

int mem_free(void* ptr) {

    __asm__ volatile ("mv a1, %0" : : "r"((uint64)ptr));
    __asm__ volatile ("mv a0, %0" : : "r"(0x02));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

size_t mem_get_free_space() {

    __asm__ volatile ("mv a0, %0" : : "r"(0x03));

    __asm__ volatile ("ecall");

    uint64 val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return (size_t)val;
}

size_t mem_get_largest_free_block() {

    __asm__ volatile ("mv a0, %0" : : "r"(0x04));

    __asm__ volatile ("ecall");

    uint64 val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return (size_t)val;
}

int thread_create(thread_t* handle, void (*start_routine) (void*), void* arg) {

    if (!handle) return -1;

    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stack) return -1;

    __asm__ volatile ("mv a4, %0" : : "r"(stack));
    __asm__ volatile ("mv a3, %0" : : "r"(arg));
    __asm__ volatile ("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile ("mv a1, %0" : : "r"(handle));
    __asm__ volatile ("mv a0, %0" : : "r"(0x11));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

int thread_exit() {

    __asm__ volatile ("mv a0, %0" : : "r"(0x12));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

void thread_dispatch() {

    __asm__ volatile ("mv a0, %0" : : "r"(0x13));

    __asm__ volatile ("ecall");
}

int get_thread_id() {

    __asm__ volatile ("mv a0, %0" : : "r"(0x14));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

void thread_join(thread_t handle) {

    __asm__ volatile ("mv a1, %0" : : "r"(handle));
    __asm__ volatile ("mv a0, %0" : : "r"(0x15));

    __asm__ volatile ("ecall");
}

int sem_open (sem_t* handle, unsigned init) {

    if (!handle) return -1;

    __asm__ volatile ("mv a2, %0" : : "r"(init));
    __asm__ volatile ("mv a1, %0" : : "r"(handle));
    __asm__ volatile ("mv a0, %0" : : "r"(0x21));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

int sem_close(sem_t handle) {

    __asm__ volatile ("mv a1, %0" : : "r"(handle));
    __asm__ volatile ("mv a0, %0" : : "r"(0x22));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

int sem_wait(sem_t id) {

    __asm__ volatile ("mv a1, %0" : : "r"(id));
    __asm__ volatile ("mv a0, %0" : : "r"(0x23));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

int sem_signal(sem_t id) {

    __asm__ volatile ("mv a1, %0" : : "r"(id));
    __asm__ volatile ("mv a0, %0" : : "r"(0x24));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

int time_sleep(time_t time) {

    __asm__ volatile ("mv a1, %0" : : "r"(time));
    __asm__ volatile ("mv a0, %0" : : "r"(0x31));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return val;
}

char getc() {

    __asm__ volatile ("mv a0, %0" : : "r"(0x41));

    __asm__ volatile ("ecall");

    int val;
    __asm__ volatile ("mv %0, a0" : "=r"(val));

    return (char)val;
}

void putc(char character) {

    __asm__ volatile ("mv a1, %0" : : "r"(character));
    __asm__ volatile ("mv a0, %0" : : "r"(0x42));

    __asm__ volatile ("ecall");
}
