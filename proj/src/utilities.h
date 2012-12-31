#ifndef __UTILITIES_H
#define __UTILITIES_H

#include <stdio.h>

#define BIT(n) (0x1 << (n))

#define min(x,y) ((x) < (y) : (x) ? (y))

unsigned long bit_set_all(int n);

unsigned long parse_ulong(const char *str, int base);

void swapl(long* a, long* b);

int clamp(int x, int min, int max);

// #define DEBUG_CALLS // only enable when sh*t goes really wrong

#ifdef DEBUG_CALLS
    #define LOG printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
#else
    #define LOG ((void)0);
#endif

#endif /* __UTILITIES_H */
