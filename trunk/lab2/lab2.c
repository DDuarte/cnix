#include <minix/drivers.h>
#include <stdlib.h>

#include "video_gr.h"

#define WAIT_TIME_S 5

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {
  /* Initialize service */
  
  sef_startup();

  if ( argc == 1 ) {
      print_usage(argv);
      return 0;
  } else {   
      if( proc_args(argc, argv) == 0 ) {
      }
  }
  return 0;

}

static void print_usage(char *argv[]) {
  printf("Usage: one of the following:\n"
	 "\t service run %s -args \"fill <graphic-hex-mode> <color-hex-code>\" \n"
	 "\t service run %s -args \"set-pixel <graphic-hex-mode> <x-coord> <y-coord> <color-hex-code>\" \n"
	 "\t service run %s -args \"get-pixel <graphic-hex-mode> <x-coord> <y-coord> <color-hex-code>\" \n"
	 "\t service run %s -args \"draw-line <graphic-hex-mode> <xi-coord> <yi-coord> <xf-coord> <yf-coord>  <color-hex-code>\" \n"
	 "\t service run %s -args \"draw-rectangle <graphic-hex-mode> <xi-coord> <yi-coord> <xf-coord> <yf-coord>  <color-hex-code>\" \n",
	 argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

  unsigned long grMode, col, xiCoord, yiCoord, xfCoord, yfCoord;
  int ret;
  char *video_mem;

  /* check the function to test: if the first characters match, accept it */
  if (strncmp(argv[1], "fill", strlen("fill")) == 0) {
	  if( argc != 4 ) {
		  printf("video_gr: wrong no of arguments for test of vg_fill() \n");
		  return 1;
	  }
	  if( (grMode = parse_ulong(argv[2], 16)) == ULONG_MAX )
		  return 1;
	  if( (col = parse_ulong(argv[3], 16)) == ULONG_MAX )
		  return 1;
	  printf("video_gr:: vg_init(0x%X)\n",
			  (unsigned) grMode);
	  printf("video_gr:: vg_fill(0x%X)\n",
			  (unsigned)col);

	  video_mem = vg_init(grMode);
	  if (!video_mem)
	  {
		vg_exit();
		printf("video_gr: vg_init() return NULL");
		return 1;
	  }
	  
	  ret = vg_fill(col);
	  if (ret != OK)
	  {
		vg_exit();
		printf("video_gr: vg_fill() return error code %d \n", ret);
		return ret;
	  }
	  
	  sleep(WAIT_TIME_S);
	  vg_exit();
	  return ret;
	  
  } else if (strncmp(argv[1], "set-pixel", strlen("set-pixel")) == 0) {
	  if( argc != 6 ) {
		  printf("video_gr: wrong no of arguments for test of vg_set_pixel() \n");
		  return 1;
	  }
	  if( (grMode = parse_ulong(argv[2], 16)) == ULONG_MAX )
		  return 1;
	  if( (xiCoord = parse_ulong(argv[3], 10)) == ULONG_MAX )
		  return 1;
	  if( (yiCoord = parse_ulong(argv[4], 10)) == ULONG_MAX )
		  return 1;
	  if( (col = parse_ulong(argv[5], 16)) == ULONG_MAX )
		  return 1;
	  printf("video_gr:: vg_init(0x%X)\n",
			  (unsigned) grMode);
	  printf("video_gr:: vg_set_pixel(%lu, %lu, 0x%X)\n",
			  xiCoord, yiCoord, (unsigned)col);
			  
	  video_mem = vg_init(grMode);
	  if (!video_mem)
	  {
		vg_exit();
		printf("video_gr: vg_init() return NULL \n");
		return 1;
	  }
	  
	  ret = vg_set_pixel(xiCoord, yiCoord, col);
	  if (ret != OK)
	  {
		vg_exit();
		printf("video_gr: vg_set_pixel() return error code %d \n", ret);
		return ret;
	  }
	  
	  sleep(WAIT_TIME_S);
	  vg_exit();
	  return ret;
	  
  } else if (strncmp(argv[1], "get-pixel", strlen("get-pixel")) == 0) {
	  if( argc != 6 ) {
		  printf("video_gr: wrong no of arguments for test of vg_get_pixel() \n");
		  return 1;
	  }
	  if( (grMode = parse_ulong(argv[2], 16)) == ULONG_MAX )
		  return 1;
	  if( (xiCoord = parse_ulong(argv[3], 10)) == ULONG_MAX )
		  return 1;
	  if( (yiCoord = parse_ulong(argv[4], 10)) == ULONG_MAX )
		  return 1;
	  if( (col = parse_ulong(argv[5], 16)) == ULONG_MAX )
		  return 1;
	  printf("video_gr:: vg_init(0x%X)\n",
			  (unsigned) grMode);
	  printf("video_gr:: vg_set_pixel(%lu, %lu, 0x%X)\n",
			  xiCoord, yiCoord, col);
	  printf("video_gr:: vg_get_pixel(%lu, %lu)\n",
			  xiCoord, yiCoord);
	  			  
	  video_mem = vg_init(grMode);
	  if (!video_mem)
	  {
		vg_exit();
		printf("video_gr: vg_init() return NULL\n");
		return 1;
	  }
	  
	  ret = vg_set_pixel(xiCoord, yiCoord, col);
	  if (ret != OK)
	  {
		vg_exit();
		printf("video_gr: vg_set_pixel() return error code %d \n", ret);
		return ret;
	  }
	  
	  col = vg_get_pixel(xiCoord, yiCoord);
	  
	  sleep(WAIT_TIME_S);
	  vg_exit();
	  printf("Returned color: 0X%X\n", col);
	  return ret;
			  
			  
  } else if (strncmp(argv[1], "draw-line", strlen("draw-line")) == 0) {
	  if( argc != 8 ) {
		  printf("video_gr: wrong no of arguments for test of vg_draw_line() \n");
		  return 1;
	  }
	  if( (grMode = parse_long(argv[2], 16)) == LONG_MAX )
		  return 1;
	  if( (xiCoord = parse_ulong(argv[3], 10)) == ULONG_MAX )
		  return 1;
	  if( (yiCoord = parse_ulong(argv[4], 10)) == ULONG_MAX )
		  return 1;
	  if( (xfCoord = parse_ulong(argv[5], 10)) == ULONG_MAX )
		  return 1;
	  if( (yfCoord = parse_ulong(argv[6], 10)) == ULONG_MAX )
		  return 1;
	  if( (col = parse_ulong(argv[7], 16)) == ULONG_MAX )
		  return 1;
	  printf("video_gr:: vg_init(0x%X)\n",
			  (unsigned) grMode);
	  printf("video_gr:: vg_draw_line(%lu, %lu, %lu, %lu, 0x%X)\n",
		 xiCoord, yiCoord, xfCoord, yfCoord, (unsigned)col);
	  
	  video_mem = vg_init(grMode);
	  if (!video_mem)
	  {
		vg_exit();
		printf("video_gr: vg_init() return NULL");
		return 1;
	  }
	  
	  ret = vg_draw_line(xiCoord, yiCoord, xfCoord, yfCoord, col);
	  if (ret != OK)
	  {
		vg_exit();
		printf("video_gr: vg_draw_line() return error code %d \n", ret);
		return ret;
	  }
	  
	  sleep(WAIT_TIME_S);
	  vg_exit();
	  return ret;
	  
  } else if (strncmp(argv[1], "draw-rectangle", strlen("draw-rectangle")) == 0) {
	  if( argc != 8 ) {
		  printf("video_gr: wrong no of arguments for test of vg_draw-rectangle() \n");
		  return 1;
	  }
	  if( (grMode = parse_long(argv[2], 16)) == LONG_MAX )
		  return 1;
	  if( (xiCoord = parse_ulong(argv[3], 10)) == ULONG_MAX )
		  return 1;
	  if( (yiCoord = parse_ulong(argv[4], 10)) == ULONG_MAX )
		  return 1;
	  if( (xfCoord = parse_ulong(argv[5], 10)) == ULONG_MAX )
		  return 1;
	  if( (yfCoord = parse_ulong(argv[6], 10)) == ULONG_MAX )
		  return 1;
	  if( (col = parse_ulong(argv[7], 16)) == ULONG_MAX )
		  return 1;
	  printf("video_gr:: vg_init(0x%X)\n",
			  (unsigned) grMode);
	  printf("video_gr:: vg_draw_rectangle(%lu, %lu, %lu, %lu, 0x%X)\n",
		 xiCoord, yiCoord, xfCoord, yfCoord, (unsigned)col);
	  
	  video_mem = vg_init(grMode);
	  if (!video_mem)
	  {
		vg_exit();
		printf("video_gr: vg_init() return NULL\n");
		return 1;
	  }
	  
	  ret = vg_draw_rectangle(xiCoord, yiCoord, xfCoord, yfCoord, col);
	  if (ret != OK)
	  {
		vg_exit();
		printf("video_gr: vg_draw_rectangle() return error code %d \n", ret);
		return ret;
	  }
	  
	  sleep(WAIT_TIME_S);
	  vg_exit();
	  return ret;
  } else {
	  printf("video_gr: non valid function \"%s\" to test\n", argv[1]);
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
	  printf("video_txt: parse_ulong: no digits were found in %s \n", str);
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
	  printf("video_txt: parse_long: no digits were found in %s \n", str);
	  return LONG_MAX;
  }

  /* Successful conversion */
  return val;
}