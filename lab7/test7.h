#ifndef __TEST7_H__
#define __TEST7_H__

int test_conf(unsigned short base_addr);

int test_set(unsigned short base_addr, unsigned long bits, unsigned long stop,
	     long parity, /* -1: none, 0: even, 1: odd */
	     unsigned long rate);

int test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]);

int test_fifo(unsigned short base_addr, unsigned char tx, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]);

#endif
