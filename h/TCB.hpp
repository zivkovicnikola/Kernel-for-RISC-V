#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
#include "../h/Scheduler.hpp"
#include "../h/ThreadState.hpp"
#include "../h/SleepingThreads.hpp"

class TCB {
public:
    ~TCB() { delete stack; }

    using Body = void (*)(void*);

    static TCB* createThread(Body body, void* args, uint64* stack);

    bool getIsFinished() const { return isFinished; }
    void setIsFinished(bool finished) { isFinished = finished; }

    uint64 getTimeSlice() const { return timeSlice; }

    States getState() const { return state; }
    void setState(States s) { state = s; }

    static TCB* running;

    time_t getSleepingTime() const { return sleepingTime; }
    void setSleepingTime(time_t time) { sleepingTime = time; }

    static void join(TCB*  handle);

    static uint64 nextId;

    explicit TCB(Body body, void* args, uint64 *stack) :
        body(body),
        stack(body != nullptr ? stack : nullptr),
        args(args),
        context({(uint64) &threadWrapper,
                 (uint64)(stack != nullptr ? &((char*)stack)[DEFAULT_STACK_SIZE] : nullptr)
                }),
        timeSlice(DEFAULT_TIME_SLICE),
        state(READY),
        isFinished(false),
        id(nextId++) {
        if (body != nullptr) Scheduler::put(this);
    }

private:
    struct Context {
        uint64 ra;
        uint64 sp;
    };
    Body body;
    uint64* stack;
    void* args;
    Context context;
    uint64 timeSlice;
    States state;
    bool isFinished;
    uint64 id;

    time_t sleepingTime = 0;
    TCB* nextAsleep = nullptr;

    static void threadWrapper();
    static void contextSwitch(Context* oldContext, Context* runningContext);
    static void dispatch();

    static uint64 timeSliceCnt;

    friend class Riscv;
    friend class Sem;
    friend class SleepingThreads;
};

#endif //PROJECT_BASE_V1_1_TCB_HPP
