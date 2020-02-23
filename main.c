#include "hal.h"
#include "ch.h"

/*
 * Define the PWM driver config
 */
static PWMConfig pwmCFG =
{
   10000,                   /* 10kHz PWM clock frequency */
   10000,                   /* PWM period = ticks / PWM clock frequency =1second*/
   NULL,                    /* No callback */
   /*
    * Enable two channels, since TIM_CH1 and TIM_CH2 are attached to LEDs
    */
   {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL}
   },
   0,
   0
};


int main(void) {
    halInit();
    chSysInit();

    // Set the alternate function for PA6, as it uses TIM3_CH1
    palSetPadMode(GPIOA, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
    pwmStart(&PWMD3, &pwmCFG);
    /*
   * Enable channel 0 with 50% duty cycle
   */
    pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));


    while(TRUE) {
        palSetPad(GPIOC, GPIOC_LED);
        chThdSleepMilliseconds(100);
        palClearPad(GPIOC, GPIOC_LED);
        chThdSleepMilliseconds(1000);
    }
    return 0;
}