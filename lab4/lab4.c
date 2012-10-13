#include "lab4.h"
#include "test4.h"

int main(int argc, char* argv[])
{
    unsigned short a[18] = {1, 0, 1, 2, 1, 0, 0, 2, 1, 2, 1, 0, 1, 2, 0, 2, 1, 0};
    sef_startup();
      
    test_scan();
    /*test_leds(2, a);*/
    return 0;
}