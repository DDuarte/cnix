#include <minix/drivers.h>
#include <stdlib.h>
#include <stdio.h>

#include "test7.h"

#define DEBUG

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {
    
    sef_startup();
    
    if (argc == 1)
        print_usage(argv);
    else
        proc_args(argc, argv);

    return 0;
}

static void print_usage(char *argv[]) {
    printf("Usage: one of the following:\n"
       "\t service run %s -args \"test-conf <base_addr>\" \n"
       "\t service run %s -args \"test-set <base_addr> <bits> <stop> <parity> <rate>\" \n"
       "\t service run %s -args \"test-poll \" \n"
       "\t service run %s -args \"test-int \" \n"
       "\t service run %s -args \"test-fifo \" \n",
       argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
    int ret;
    unsigned short base_addr;
    unsigned long bits, stop, rate;
	long parity;

    /* check the function to test: if the first characters match, accept it */
    if (strncmp(argv[1], "test-conf", strlen("test-conf")) == 0) {
        if(argc != 3) {
            printf("lab7: wrong no of arguments for test of test-conf() \n");
            return 1;
        }

        if ((base_addr = parse_ulong(argv[2], 16)) == ULONG_MAX)
            return 1;
        
        printf("lab7: test-conf(0x%X)\n", base_addr);

        ret = test_conf(base_addr);

        if (ret != 0)
            printf("lab7: test-conf() return error code %d \n", ret);
        else
            printf("lab7: test-conf() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-set", strlen("test-set")) == 0) {
        if (argc != 7) {
            printf("lab7: wrong no of arguments for test of test_set() \n");
            return 1;
        }
        
        if ((base_addr = parse_ulong(argv[2], 16)) == ULONG_MAX)
            return 1;
            
        if ((bits = parse_ulong(argv[2], 16)) == ULONG_MAX)
            return 1;
            
        if ((stop = parse_ulong(argv[2], 16)) == ULONG_MAX)
            return 1;
            
        if ((parity = parse_long(argv[2], 16)) == ULONG_MAX)
            return 1;
            
        if ((rate = parse_ulong(argv[2], 16)) == ULONG_MAX)
            return 1;
        
        
        printf("lab7: test_set(0x%X, 0x%X, 0x%X, 0x%X, 0x%X)\n", base_addr, bits, stop, parity, rate);

        ret = test_set(base_addr, bits, stop, parity, rate);

        if (ret != 0)
            printf("lab7: test_set(0x%X, 0x%X, 0x%X, 0x%X, 0x%X) return error code %d \n", base_addr, bits, stop, parity, rate, ret);
        else
            printf("lab7: test_set(0x%X, 0x%X, 0x%X, 0x%X, 0x%X) return code %d \n", base_addr, bits, stop, parity, rate, ret);

        return ret;

    } else if (strncmp(argv[1], "test-poll", strlen("test-poll")) == 0) {
        if(argc != 2) {
            printf("lab7: wrong no of arguments for test_poll() \n");
            return 1;
        }
        

        printf("lab7: test_poll()\n");

        ret = test_poll();

        if (ret != 0)
            printf("lab7: test_poll() return error code %d \n", ret);
        else
            printf("lab7: test_poll() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-int", strlen("test-int")) == 0) {
        if(argc != 2) {
            printf("lab7: wrong no of arguments for test_int() \n");
            return 1;
        }
        

        printf("lab7: test_int()\n");

        ret = test_int();

        if (ret != 0)
            printf("lab7: test_int() return error code %d \n", ret);
        else
            printf("lab7: test_int() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-fifo", strlen("test-fifo")) == 0) {
        if(argc != 2) {
            printf("lab7: wrong no of arguments for test_fifo() \n");
            return 1;
        }
        

        printf("lab7: test_fifo()\n");

        ret = test_fifo();

        if (ret != 0)
            printf("lab7: test_fifo() return error code %d \n", ret);
        else     
            printf("lab7: test_fifo() return code %d \n", ret);

        return ret;

    } else {

        printf("lab7: non valid function \"%s\" to test\n", argv[1]);
        return 1;
    }
}

static unsigned long parse_ulong(char *str, int base) {
    char *endptr;
    unsigned long val;

    val = strtoul(str, &endptr, base);

    if ((errno == ERANGE && val == ULONG_MAX )
        || (errno != 0 && val == 0)) {
        perror("strtol");
        return ULONG_MAX;
    }

    if (endptr == str) {
        printf("lab7/parse_ulong: no digits were found in %s \n", str);
        return ULONG_MAX;
    }

    /* Successful conversion */
    return val;
}

static long parse_long(char *str, int base) {
    char *endptr;
    unsigned long val;

    val = strtol(str, &endptr, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
        perror("strtol");
        return LONG_MAX;
    }

    if (endptr == str) {
        printf("lab7/parse_long: no digits were found in %s \n", str);
        return LONG_MAX;
    }

    /* Successful conversion */
    return val;
}

