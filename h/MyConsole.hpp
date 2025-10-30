#ifndef PROJECT_BASE_V1_1_MYCONSOLE_HPP
#define PROJECT_BASE_V1_1_MYCONSOLE_HPP

#include "../h/Buff.hpp"
#include "../h/Riscv.hpp"

class MyConsole {
public:
    static Buff* inputBuff;
    static Buff* outputBuff;

    static void wrapper(void*);

    static void init();

private:
    static bool initialized;

    static char get_c();
    static void put_c(char character);

    friend class Riscv;

};

#endif //PROJECT_BASE_V1_1_MYCONSOLE_HPP