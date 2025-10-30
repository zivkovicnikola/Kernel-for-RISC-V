#include "../h/MyConsole.hpp"
#include "../h/syscall_c.h"

Buff* MyConsole::inputBuff;
Buff* MyConsole::outputBuff;
bool MyConsole::initialized = false;

void MyConsole::init() {
    if (initialized)
        return;

    initialized = true;
    inputBuff = new Buff();
    outputBuff = new Buff();
}

char MyConsole::get_c() {
    char character = inputBuff->get_char();
    return character;
}

void MyConsole::put_c(char character) {
    outputBuff->put_char(character);
}

void MyConsole::wrapper(void *) {

    while (true) {
        while (MyConsole::outputBuff->getCurrCapacity() > 0 && (*((volatile char *) (CONSOLE_STATUS)) & CONSOLE_TX_STATUS_BIT)) {
            char c = MyConsole::outputBuff->get_char_from_thread();
            *(volatile char*)CONSOLE_TX_DATA = c;
        }
        thread_dispatch();
    }
}
