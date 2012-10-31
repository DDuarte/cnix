#include <minix/drivers.h>
#include <stdlib.h>

#include "test6.h"

#define DEBUG

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
       "\t service run %s -args \"test-conf\" \n"
       "\t service run %s -args \"test-date\" \n"
       "\t service run %s -args \"test-period\" \n",
       argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
    int ret;

    /* check the function to test: if the first characters match, accept it */
    if (strncmp(argv[1], "test-conf", strlen("test-conf")) == 0) {
        if(argc != 2) {
            printf("lab6: wrong no of arguments for test of test-conf() \n");
            return 1;
        }

        printf("lab6: test-conf()\n");

        ret = test_conf();

        if (ret != 0)
            printf("lab6: test-conf() return error code %d \n", ret);
        else
            printf("lab6: test-conf() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-date", strlen("test-date")) == 0) {
        if (argc != 3) {
            printf("lab6: wrong no of arguments for test of test_date() \n");
            return 1;
        }

        printf("lab6: test_date()\n");

        ret = test_date();

        if (ret != 0)
            printf("lab6: test_date() return error code %d \n", ret);
        else
            printf("lab6: test_date() return code %d \n", ret);

        return ret;

    } else if (strncmp(argv[1], "test-period", strlen("test-period")) == 0) {
        if(argc != 2) {
            printf("lab6: wrong no of arguments for test_period() \n");
            return 1;
        }

        printf("lab6: test_period()\n");

        ret = test_period();

        if (ret != 0)
            printf("lab6: test_period() return error code %d \n", ret);
        else
            printf("lab6: test_period() return code %d \n", ret);

        return ret;

    } else {

        printf("lab6: non valid function \"%s\" to test\n", argv[1]);
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
        printf("lab6/parse_ulong: no digits were found in %s \n", str);
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
        printf("lab6/parse_long: no digits were found in %s \n", str);
        return LONG_MAX;
    }

    /* Successful conversion */
    return val;
}

