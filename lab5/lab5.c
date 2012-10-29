#include <minix/drivers.h>
#include <stdlib.h>

#include "test5.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

    sef_startup(); /* Initialize service */

    if (argc == 1)
        print_usage(argv);
    else
        proc_args(argc, argv);

    return 0;
}

static void print_usage(char *argv[]) {
    printf("Usage: one of the following:\n"
       "\t service run %s -args \"test-packet\" \n"
       "\t service run %s -args \"test-asynch <seconds>\" \n"
       "\t service run %s -args \"test-config\" \n",
       argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
    int ret;
    unsigned short duration;

    /* check the function to test: if the first characters match, accept it */
    if (strncmp(argv[1], "test-packet", strlen("test-packet")) == 0) {
        if(argc != 2) {
            printf("lab5: wrong no of arguments for test of test_packet() \n");
            return 1;
        }

        printf("lab5: test_packet()\n");

        ret = test_packet();

        if (ret != 0)
            printf("lab5: test_packet() return error code %d \n", ret);
        else
            printf("lab5: test_packet() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-asynch", strlen("test-asynch")) == 0) {
        if (argc != 3) {
            printf("lab5: wrong no of arguments for test of test_asynch() \n");
            return 1;
        }

        if ((duration = parse_ulong(argv[2], 10)) == ULONG_MAX)
            return 1;

        printf("lab5: test_asynch(%u)\n", duration);

        ret = test_asynch(duration);

        if (ret != 0)
            printf("lab5: test_asynch() return error code %d \n", ret);
        else
            printf("lab5: test_asynch() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-config", strlen("test-config")) == 0) {
        if(argc != 2) {
            printf("lab5: wrong no of arguments for test of config() \n");
            return 1;
        }

        printf("lab5: test_config()\n");

        ret = test_config();

        if (ret != 0)
            printf("lab5: test_config() return error code %d \n", ret);
        else
            printf("lab5: test_config() return code %d \n", ret);

        return ret;

    } else {

        printf("lab5: non valid function \"%s\" to test\n", argv[1]);
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
        printf("lab4/parse_ulong: no digits were found in %s \n", str);
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
        printf("lab4/parse_long: no digits were found in %s \n", str);
        return LONG_MAX;
    }

    /* Successful conversion */
    return val;
}
