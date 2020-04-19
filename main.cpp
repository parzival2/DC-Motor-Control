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
// clang-format off
// Chibios includes
#include "ch.hpp"
#include "hal.h"
#include "usbcfg.h"
#include "chprintf.h"
// User includes
#include "MotorController.h"
#include "LowpassFilter.hpp"
#include "PIDController.hpp"
#include "QuadEncoder.h"
// ros includes
#include "ros.h"
#include "std_msgs/Float32.h"
// clang-format on

///
/// \brief Ros node
///
// std_msgs::Float32 rpmMessage;
// ros::NodeHandle rosNodeHandle;
// ros::Publisher rpmPublisher("rpm", &rpmMessage);

// BaseSequentialStream* chp = (BaseSequentialStream*)(&SDU1);

qeicnt_t MAX_COUNT = 65535;
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

// clang-format off
static QEIConfig qeiconfig ={
    QEI_MODE_QUADRATURE,
    QEI_BOTH_EDGES,
    QEI_DIRINV_FALSE,
    QEI_OVERFLOW_WRAP,
    0,
    MAX_COUNT,
    NULL,
    NULL
};
// clang-format on

static constexpr double ONE_BY_TWO_PI = 0.15915494309;

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

    // Set the alternate function for PA6, as it uses TIM3_CH1
    palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 7, PAL_MODE_INPUT_PULLUP);

    // Set alternate function for PB6 and PB7 for PWM outputs
    palSetPadMode(GPIOB, 6, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
    pwmStart(&PWMD4, &pwmCFG);

    // Set mode for dir pin
    palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
    // This is important
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_NOREMAP;
    qeiInit();
    qeiStart(&QEID3, &qeiconfig);
    qeiEnable(&QEID3);

    // Quadrature encoder thread
    QuadEncoder quadEncoder;
    // QuadEncoder::RpmMailBox& mailBox = quadEncoder.getRpmMailBox();
    quadEncoder.start(NORMALPRIO + 1);

    // Motor controller thread
    MotorController motorController(quadEncoder.getRpmMailBox());
    motorController.start(NORMALPRIO + 1);

    // Initialize the ros node
    // rosNodeHandle.initNode();
    // rosNodeHandle.advertise(rpmPublisher);
    /*
     * Enable channel 0 with 50% duty cycle
     */
    pwmEnableChannel(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 2000));
    // float* rpmValue;
    while(1)
    {
        // mailBox.fetch(&rpmValue, TIME_INFINITE);
        // rpmMessage.data = *rpmValue;
        // rpmPublisher.publish(&rpmMessage);
        // rosNodeHandle.spinOnce();
        palTogglePad(GPIOC, GPIOC_LED);
        chThdSleepMilliseconds(50);
    }
}
