#include <minix/drivers.h>
#include <stdlib.h>

#include "test4.h"

/*int main(int argc, char* argv[])
{
    unsigned short a[18] = {1, 0, 1, 2, 1, 0, 0, 2, 1, 2, 1, 0, 1, 2, 0, 2, 1, 0};
    sef_startup();

    test_leds(18, a);
    test_scan();

    return 0;
}*/

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {

  /* Initialize service */
  sef_startup();


  if ( argc == 1 ) {
      print_usage(argv);
  } else {
      proc_args(argc, argv);
  }

  return 0;
}

static void print_usage(char *argv[]) {
  printf("Usage: one of the following:\n"
     "\t service run %s -args \"test-scan\" \n"
     "\t service run %s -args \"test-leds <leds_array>\n",
     argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

  unsigned long num, i, ret;
  unsigned short* leds;

  /* check the function to test: if the first characters match, accept it */
  if (strncmp(argv[1], "test-scan", strlen("test-scan")) == 0) {
      if( argc != 2 ) {
          printf("lab4: wrong no of arguments for test of test_scan() \n");
          return 1;
      }
      printf("lab4: test_scan()\n");

      ret = test_scan();

      if (ret != 0)
      {
        printf("lab4: test_scan() return error code %d \n", ret);
        return ret;
      } else {
        printf("lab4: test_scan() return code %d \n", ret);
      }

      return ret;
  } else if (strncmp(argv[1], "test-leds", strlen("test-leds")) == 0) {
      if( argc < 3 ) {
          printf("lab4: wrong no of arguments for test of test_leds() \n");
          return 1;
      }

      num = argc - 2;
      leds = (unsigned short*) malloc(num * sizeof(unsigned short));

      for (i = 0; i < num; i++) {
        unsigned long numTemp;
        if ( (numTemp = parse_ulong(argv[i+2], 10)) == ULONG_MAX) {
            free(leds);
            return 1;
        } else {
            leds[i] = (unsigned short)numTemp;
        }
      }

      printf("lab4: test_leds(%d)\n", num);

      ret = test_leds(num, leds);

      free(leds);

      if (ret != OK)
      {
        printf("lab4: test_leds() return error code %d \n", ret);
        return ret;
      } else {
        printf("lab4: test_leds() return code %d \n", ret);
      }

      return ret;
  } else {
      printf("lab4: non valid function \"%s\" to test\n", argv[1]);
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

  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
      || (errno != 0 && val == 0)) {
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

