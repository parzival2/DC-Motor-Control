/*
    ChibiOS - Copyright (C) 2006..2016 Nicolas Reinecke
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "LowpassFilter.hpp"
#include "PIDController.hpp"
#include "QuadEncoder.hpp"
#include "ch.hpp"
#include "chprintf.h"
#include "hal.h"
#include "string.h"
#include "usbcfg.h"
/**
 * PWM Config
 */
// We will be using PB6 for TIM4_CH1
static PWMConfig pwmCFG = {10000000, /* 10MHZ PWM clock frequency */
                           1000, /* PWM period = ticks / PWM clock frequency =1second*/
                           // Frequency = 10000000/1000 = 10KHZ;
                           NULL, /* No callback */
                           {{PWM_OUTPUT_ACTIVE_HIGH, NULL},
                            {PWM_OUTPUT_DISABLED, NULL},
                            {PWM_OUTPUT_DISABLED, NULL},
                            {PWM_OUTPUT_DISABLED, NULL}},
                           0,
                           0};

PIDController pidController(0.0, 0.00541, 0.0);

// The interrupts will be too much at about 1000 RPM there will be no time
// for the processor to do other things.
static QEIConfig qeicfg = {
    QEI_MODE_QUADRATURE, QEI_BOTH_EDGES, QEI_DIRINV_FALSE, QEI_OVERFLOW_WRAP, 0, 8192, NULL, NULL,
};

QuadEncoder quadEncoder(&QEID3, &qeicfg);

BaseSequentialStream* chp = (BaseSequentialStream*)(&SDU1);

static constexpr double ONE_BY_TWO_PI = 0.15915494309;

void handleInterrupt(void* arg)
{
    (void)arg;
    quadEncoder.handlePinAInterrupt();
}

/* Thread definition */
static THD_WORKING_AREA(angvelCalArea, 512);
static THD_FUNCTION(angVelThread, arg)
{
    (void)arg;
    /* Set thread name */
    chRegSetThreadName("angVelThread");
    pidController.setReference(1000);
    // last angle
    float lastAngle = 0.;
    float currentAngle = 0.;
    float rps = 0.;
    float output = 0.;
    float time = 0.;
    // Lowpass filter
    LowpassFilter lowPassFilter(0.2, 0.01);
    float filteredRpm = 0.;
    msg_t charbuf;
    char number[3];
    uint32_t currentReference = 1000;
    bool stopPrinting = false;
    /* Thread should not return, so begin a while loop */
    while(TRUE)
    {
        // Calculate angular velocity
        currentAngle = quadEncoder.getCurrentAngleRad();
        // We will be calculating rotations per second
        rps = (ONE_BY_TWO_PI * (currentAngle - lastAngle)) / 0.01;
        // Convert the RPS value to RPM
        rps = -rps * 60.0;
        // Filter the RPM
        filteredRpm = lowPassFilter.update(rps);
        // Calculate the output from the PID controller
        output = pidController.calculateOutput(filteredRpm, 0.01);
        do
        {
            charbuf = chnGetTimeout((BaseChannel*)chp, TIME_IMMEDIATE);
            if(charbuf == 's')
            {
                number[0] = chnGetTimeout((BaseChannel*)chp, TIME_IMMEDIATE);
                number[1] = chnGetTimeout((BaseChannel*)chp, TIME_IMMEDIATE);
                number[2] = chnGetTimeout((BaseChannel*)chp, TIME_IMMEDIATE);
                currentReference = atoi(number);
                pidController.setReference(currentReference);
            }
            else if(charbuf == 'c')
            {
                pidController.reset();
                pidController.setProportionalValue(0.);
                pidController.setIntegralValue(0.00541);
                pidController.setDerivativeValue(0.);
                stopPrinting = false;
            }
        } while(charbuf != Q_TIMEOUT);
        pwmEnableChannel(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, output * 1000));
        if(!stopPrinting)
        {
            chprintf(chp, "%f %f %f %d\n", time, filteredRpm, output, currentReference);
        }

        lastAngle = currentAngle;
        time += 0.01;
        chThdSleepMilliseconds(10);
	}
}

/*
 * Application entry point.
 */
int main(void)
{
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chibios_rt::System::init();

    // Initialize Serial-USB driver
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    quadEncoder.init();
    // Initialize Quadrature Encoder
    quadEncoder.setGpioAParams(GPIOA, 6);
    quadEncoder.setGpioBParams(GPIOB, 7);

    // Set the alternate function for PA6, as it uses TIM3_CH1
    palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 7, PAL_MODE_INPUT_PULLUP);
    palEnableLineEvent(PAL_LINE(GPIOA, 6U), PAL_EVENT_MODE_RISING_EDGE);
    palEnableLineEvent(PAL_LINE(GPIOA, 7U), PAL_EVENT_MODE_BOTH_EDGES);
    palSetPadCallback(GPIOA, 6, handleInterrupt, NULL);

    // Set alternate function for PB6 and PB7 for PWM outputs
    palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
    pwmStart(&PWMD4, &pwmCFG);

    // Set mode for dir pin
    palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
    /*
     * Enable channel 0 with 50% duty cycle
     */
    // pwmEnableChannel(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 2000));
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_NOREMAP;
    // Create thread
    chThdCreateStatic(angvelCalArea, sizeof(angvelCalArea), NORMALPRIO + 1, angVelThread, NULL);
    while(1)
    {
        chThdSleepMilliseconds(500);
    }
}
