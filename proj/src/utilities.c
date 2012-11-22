#include "utilities.h"
#include <minix/bitmap.h>
#include <errno.h>
#include <limits.h>

unsigned long bit_set_all(int n) {
    unsigned long result = 0x0;
    for (; n >= 0; n--)
        bit_set(result, n);
    return result;
}

unsigned long parse_ulong(const char *str, int base) {
    char *endptr;
    unsigned long val;

    val = strtoul(str, &endptr, base);

    if ((errno == ERANGE && val == ULONG_MAX )
        || (errno != 0 && val == 0)) {
        perror("strtol");
        return ULONG_MAX;
    }

    if (endptr == str) {
        printf("cnix/parse_ulong: no digits were found in %s \n", str);
        return ULONG_MAX;
    }

    /* Successful conversion */
    return val;
}

void swapl(long* a, long* b) {
    long aux = *a;
    *a = *b;
    *b = aux;
}
