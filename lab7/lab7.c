#include <minix/drivers.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test7.h"
#include "uart.h"

#define DEBUG

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
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
       "\t service run %s -args \"test-conf COM1|COM2\" \n"
       "\t service run %s -args \"test-set COM1|COM2 <bits> <stop> even|odd|none <rate>\" \n"
       "\t service run %s -args \"test-poll COM1|COM2 tx|rx <bits> <stop> even|odd|none <rate> <string>*\" \n"
       "\t service run %s -args \"test-fifo COM1|COM2 tx|rx <bits> <stop> even|odd|none <rate> <string>*\" \n",
       argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
    int ret;
    unsigned short base_addr;

    /* check the function to test: if the first characters match, accept it */
    if (strncmp(argv[1], "test-conf", strlen("test-conf")) == 0) {
        char* com_str;

        if(argc != 3) {
            printf("lab7: wrong no of arguments for test of test-conf() \n");
            return 1;
        }

        com_str = argv[2];
        if (strcmp(com_str, "COM1") != 0 && strcmp(com_str, "COM2") != 0) {
            printf("lab7: first argument needs to be COM1 or COM2 \n");
            return 1;
        }

        base_addr = strcmp(com_str, "COM1") == 0 ? UART_COM1_ADDR : UART_COM2_ADDR;

        printf("lab7: test-conf(0x%X)\n", base_addr);

        ret = test_conf(base_addr);

        if (ret != 0)
            printf("lab7: test-conf(0x%X) return error code %d \n", base_addr, ret);
        else
            printf("lab7: test-conf(0x%X) return code %d \n", base_addr, ret);

        return ret;

    } else if (strncmp(argv[1], "test-set", strlen("test-set")) == 0) {
        char* com_str, *parity_str;
        long parity;
        unsigned long bits, stop, rate;

        if (argc != 7) {
            printf("lab7: wrong no of arguments for test of test_set() \n");
            return 1;
        }

        com_str = argv[2];
        if (strcmp(com_str, "COM1") != 0 && strcmp(com_str, "COM2") != 0) {
            printf("lab7: first argument needs to be COM1 or COM2 \n");
            return 1;
        }

        base_addr = strcmp(com_str, "COM1") == 0 ? UART_COM1_ADDR : UART_COM2_ADDR;

        if ((bits = parse_ulong(argv[3], 10)) == ULONG_MAX)
            return 1;

        if ((stop = parse_ulong(argv[4], 10)) == ULONG_MAX)
            return 1;

        parity_str = argv[5];
        if (strcmp(parity_str, "even") == 0)
            parity = 0;
        else if (strcmp(parity_str, "odd") == 0)
            parity = 1;
        else if (strcmp(parity_str, "none") == 0)
            parity = -1;
        else {
            printf("lab7: third argument needs to be even, odd or none \n");
            return 1;
        }

        if ((rate = parse_ulong(argv[6], 10)) == ULONG_MAX)
            return 1;

        printf("lab7: test_set(0x%X, %lu, %lu, %lu, %lu)\n", base_addr, bits, stop, parity, rate);

        ret = test_set(base_addr, bits, stop, parity, rate);

        if (ret != 0)
            printf("lab7: test_set(0x%X, %lu, %lu, %lu, %lu) return error code %d \n", base_addr, bits, stop, parity, rate, ret);
        else
            printf("lab7: test_set(0x%X, %lu, %lu, %lu, %lu) return code %d \n", base_addr, bits, stop, parity, rate, ret);

        return ret;

    } else if (strncmp(argv[1], "test-poll", strlen("test-poll")) == 0) {
        char* com_str, *tx_str, *parity_str;
        long parity;
        unsigned long bits, stop, rate;
        int stringc;
        char** strings;
        unsigned char tx;

        if (argc < 8) {
            printf("lab7: wrong no of arguments for test_poll() \n");
            return 1;
        }

        com_str = argv[2];
        if (strcmp(com_str, "COM1") != 0 && strcmp(com_str, "COM2") != 0) {
            printf("lab7: first argument needs to be COM1 or COM2 \n");
            return 1;
        }

        base_addr = strcmp(com_str, "COM1") == 0 ? UART_COM1_ADDR : UART_COM2_ADDR;

        tx_str = argv[3];
        if (strcmp(tx_str, "tx") != 0 && strcmp(tx_str, "rx") != 0) {
            printf("lab7: second argument needs to be tx or rx \n");
            return 1;
        }

        tx = strcmp(tx_str, "tx") == 0 ? TRUE : FALSE;

        if ((bits = parse_ulong(argv[4], 10)) == ULONG_MAX)
            return 1;

        if ((stop = parse_ulong(argv[5], 10)) == ULONG_MAX)
            return 1;

        parity_str = argv[5];
        if (strcmp(parity_str, "even") == 0)
            parity = 0;
        else if (strcmp(parity_str, "odd") == 0)
            parity = 1;
        else if (strcmp(parity_str, "none") == 0)
            parity = -1;
        else {
            printf("lab7: fouth argument needs to be even, odd or none \n");
            return 1;
        }

        if ((rate = parse_ulong(argv[7], 10)) == ULONG_MAX)
            return 1;

        stringc = argc - 8;
        strings = NULL;
        if (stringc) {
            int i;
            strings = malloc(sizeof(char*) * stringc);
            for (i = 0; i < stringc; ++i) {
                strings[i] = malloc(sizeof(char) * strlen(argv[8 + i]));
                strcpy(strings[i], argv[8 + i]);
            }
        }

        printf("lab7: test_poll(0x%X, %d, %lu, %lu, %lu, %lu, %d, char**)\n", base_addr, tx, bits, stop, parity, rate, stringc);

        ret = test_poll(base_addr, tx, bits, stop, parity, rate, stringc, strings);

        if (ret != 0)
            printf("lab7: test_poll(0x%X, %d, %lu, %lu, %lu, %lu, %d, char**) return error code %d \n", base_addr, tx, bits, stop, parity, rate, stringc, ret);
        else
            printf("lab7: test_poll(0x%X, %d, %lu, %lu, %lu, %lu, %d, char**) return code %d \n", base_addr, tx, bits, stop, parity, rate, stringc, ret);

        return ret;

    } else if (strncmp(argv[1], "test-fifo", strlen("test-fifo")) == 0) {
        char* com_str, *tx_str, *parity_str;
        long parity;
        unsigned long bits, stop, rate;
        int stringc;
        char** strings;
        unsigned char tx;

        if (argc < 8) {
            printf("lab7: wrong no of arguments for test_fifo() \n");
            return 1;
        }

        com_str = argv[2];
        if (strcmp(com_str, "COM1") != 0 && strcmp(com_str, "COM2") != 0) {
            printf("lab7: first argument needs to be COM1 or COM2 \n");
            return 1;
        }

        base_addr = strcmp(com_str, "COM1") == 0 ? UART_COM1_ADDR : UART_COM2_ADDR;

        tx_str = argv[3];
        if (strcmp(tx_str, "tx") != 0 && strcmp(tx_str, "rx") != 0) {
            printf("lab7: second argument needs to be tx or rx \n");
            return 1;
        }

        tx = strcmp(tx_str, "tx") == 0 ? TRUE : FALSE;

        if ((bits = parse_ulong(argv[4], 10)) == ULONG_MAX)
            return 1;

        if ((stop = parse_ulong(argv[5], 10)) == ULONG_MAX)
            return 1;

        parity_str = argv[5];
        if (strcmp(parity_str, "even") == 0)
            parity = 0;
        else if (strcmp(parity_str, "odd") == 0)
            parity = 1;
        else if (strcmp(parity_str, "none") == 0)
            parity = -1;
        else {
            printf("lab7: fouth argument needs to be even, odd or none \n");
            return 1;
        }

        if ((rate = parse_ulong(argv[7], 10)) == ULONG_MAX)
            return 1;

        stringc = argc - 8;
        strings = NULL;
        if (stringc) {
            int i;
            strings = malloc(sizeof(char*) * stringc);
            for (i = 0; i < stringc; ++i) {
                strings[i] = malloc(sizeof(char) * strlen(argv[8 + i]));
                strcpy(strings[i], argv[8 + i]);
            }
        }

        printf("lab7: test_fifo(0x%X, %d, %lu, %lu, %lu, %lu, %d, char**)\n", base_addr, tx, bits, stop, parity, rate, stringc);

        ret = test_fifo(base_addr, tx, bits, stop, parity, rate, stringc, strings);

        if (ret != 0)
            printf("lab7: test_fifo(0x%X, %d, %lu, %lu, %lu, %lu, %d, char**) return error code %d \n", base_addr, tx, bits, stop, parity, rate, stringc, ret);
        else
            printf("lab7: test_fifo(0x%X, %d, %lu, %lu, %lu, %lu, %d, char**) return code %d \n", base_addr, tx, bits, stop, parity, rate, stringc, ret);

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
