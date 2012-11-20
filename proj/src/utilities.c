#include "utilities.h"
#include <minix/bitmap.h>

unsigned long bit_set_all(int n) {
    unsigned long result = 0x0;
    for (; n >= 0; n--)
        bit_set(result, n);
    return result;
}
