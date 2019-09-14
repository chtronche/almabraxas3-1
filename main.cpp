#include <stdio.h>

#include "adebug.h"
#include "mbed.h"
#include "reporting.h"

DigitalOut myled(LED1);
static Thread reporting;

int main()
{
    chenillard();
    int i = 1;
    reporting_debug_print_serial("Hello World !\n");
    reporting.start(reporting_loop);
    while(1) {
        wait(1);
        myled = !myled;
    }
}
