#ifndef __UTILITIES_H
#define __UTILITIES_H

/** @defgroup utilities utilities
 * @{
 *
 * Common functions and macros - miscellaneous
 */

#include <stdio.h>

#define BIT(n) (0x1 << (n)) ///< 2^(N-1)

#define min(x,y) ((x) < (y) : (x) ? (y)) ///< Returns min value of two objects

unsigned long bit_set_all(int n);

unsigned long parse_ulong(const char *str, int base); ///< Reads unsigned long from string in a specific base

void swapl(long* a, long* b); ///< Swaps two longs

int clamp(int x, int min, int max); ///< Clamps a value between two limits

// #define DEBUG_CALLS // only enable when sh*t goes really wrong

#ifdef DEBUG_CALLS
    #define LOG printf("DEBUG: %s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
#else
    #define LOG ((void)0);
#endif

/**@}*/

#endif /* __UTILITIES_H */
