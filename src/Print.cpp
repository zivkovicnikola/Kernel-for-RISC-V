#include "../h/Print.hpp"
#include "../h/syscall_c.h"

void sysPrintString(char const *string) {

    while (*string != '\0') {
        putc(*string);
        string++;
    }
}

void sysPrintInteger(uint64 integer) {

    static char digits[] = "0123456789ABCDEF";
    char buf[16];
    int i;
    uint x;

    x = integer;

    i = 0;
    do {
        buf[i++] = digits[x % 16];
    } while((x /= 16) != 0);

    while (--i >= 0) putc(buf[i]);
}