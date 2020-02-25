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

#include "ch.h"
#include "hal.h"
#include "usbcfg.h"
#include "chprintf.h"

static QEIConfig qeicfg = {
    QEI_MODE_QUADRATURE,
    QEI_BOTH_EDGES,
    QEI_DIRINV_FALSE,
    QEI_USE_OVERFLOW_MINMAX,
    0,
    2048,
    NULL,
    NULL,
  };

int16_t THRESHOLD = -2000;
uint16_t qei;
uint16_t lastQei = 0;
bool direction;

BaseSequentialStream *chp = (BaseSequentialStream *)(&SDU1);

void notifyCallbackPin6(void *arg)
{
  (void)arg;
  chSysLockFromISR();
  // http://www.creative-robotics.com/quadrature-intro
  qei = qeiGetCountI(&QEID3);
  if(palReadPad(GPIOA, 7) == 0)
  {
    direction = 1;
  }
  else if(palReadPad(GPIOA, 7) == 1)
  {
    direction = 0;
  }
  palTogglePad(GPIOC, GPIOC_LED);
  lastQei = qei;
  chSysUnlockFromISR();
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

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
  palEnableLineEvent(PAL_LINE(GPIOA, 6U), PAL_EVENT_MODE_RISING_EDGE);
  palEnableLineEvent(PAL_LINE(GPIOA, 7U), PAL_EVENT_MODE_BOTH_EDGES);
  palSetPadCallback(GPIOA, 6, notifyCallbackPin6, NULL);

  AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_NOREMAP;
  qeiStart(&QEID3, &qeicfg);
  qeiEnable(&QEID3);
  qeiSetCount(&QEID3, 0);
  while (1) {
     // qei = qeiGetCount(&QEID3);
     // chprintf(chp, "QEI Count : %d\n", qei);
     // if (qei & 1)
     //   palSetPad(GPIOC, GPIOC_LED);
     // else
     //   palClearPad(GPIOC, GPIOC_LED);
     chprintf(chp, "Count : %d \nDirection : %d\n", qei, direction);
     chThdSleepMilliseconds(10);
     
  }
}