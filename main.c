#include "hal.h"
#include "ch.h"

int main(void) {
    halInit();
    chSysInit(); 

    while(TRUE) {
        palSetPad(GPIOC, GPIOC_LED);
        chThdSleepMilliseconds(100);
        palClearPad(GPIOC, GPIOC_LED);
        chThdSleepMilliseconds(1000);
    }
    return 0;
}