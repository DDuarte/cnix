#include <minix/drivers.h>
#include <stdlib.h>

#include "timer.h"
#include "speaker.h"

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
     "\t service run %s -args \"timer-test-square <freq>\" \n"
     "\t service run %s -args \"timer-test-int <time>\" \n"
     "\t service run %s -args \"speaker-test <freq> <time>\" \n",
     argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

  unsigned long freq, time;
  int ret;

  /* check the function to test: if the first characters match, accept it */
  if (strncmp(argv[1], "timer-test-square", strlen("timer-test-square")) == 0) {
      if( argc != 3 ) {
          printf("lab3: wrong no of arguments for test of timer_test_square() \n");
          return 1;
      }
      if( (freq = parse_ulong(argv[2], 10)) == ULONG_MAX )
          return 1;
      printf("lab3: timer_test_square(%d)\n", freq);

      ret = timer_test_square(freq);
      if (ret != OK)
      {
        printf("lab3: timer_test_square() return error code %d \n", ret);
        return ret;
      }

      return ret;
  } else if (strncmp(argv[1], "timer-test-int", strlen("timer-test-int")) == 0) {
      if( argc != 3 ) {
          printf("lab3: wrong no of arguments for test of timer_test_int() \n");
          return 1;
      }
      if( (time = parse_ulong(argv[2], 10)) == ULONG_MAX )
          return 1;
      printf("lab3: timer_test_int(%d)\n", time);

      ret = timer_test_int(time);
      if (ret != OK)
      {
        printf("lab3: timer_test_int() return error code %d \n", ret);
        return ret;
      } else {
        printf("lab3: timer_test_int() return code %d \n", ret);
      }

      return ret;
  } else if (strncmp(argv[1], "speaker-test", strlen("speaker-test")) == 0) {
      if( argc != 4 ) {
          printf("lab3: wrong no of arguments for test of speaker-test() \n");
          return 1;
      }
      if( (freq = parse_ulong(argv[2], 10)) == ULONG_MAX )
          return 1;

      if( (time = parse_ulong(argv[3], 10)) == ULONG_MAX )
          return 1;
      printf("lab3: speaker_test(%d, %d)\n", freq, time);
      
      ret = speaker_test(freq, time);
      if (ret != OK)
      {
        printf("lab3: speaker_test() return error code %d \n", ret);
        return ret;
      } else {
        printf("lab3: speaker_test() return code %d \n", ret);
      }
  } else {
      printf("lab3: non valid function \"%s\" to test\n", argv[1]);
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
      printf("lab3/parse_ulong: no digits were found in %s \n", str);
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
      printf("lab3/parse_long: no digits were found in %s \n", str);
      return LONG_MAX;
  }

  /* Successful conversion */
  return val;
}
