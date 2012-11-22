#ifndef __UTILITIES_H
#define __UTILITIES_H

#define BIT(n) (0x1 << (n))

#define min(x,y) ((x) < (y) : (x) ? (y))

unsigned long bit_set_all(int n);
unsigned long parse_ulong(const char *str, int base);

void swapl(long* a, long* b);

#endif /* __UTILITIES_H */
