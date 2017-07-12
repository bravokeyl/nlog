/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

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

#include <stdio.h>
#include <string.h>
/* ChibiOS Includes */
#include "ch.h"
#include "hal.h"
/* ChibiOS Supplementary Includes */
#include "chprintf.h"
#include "shell.h"
/* FatFS */
#include "ff.h"
/* Project includes */

#include "usbserial.h"

#include "log.h"
#include "lwipthread.h"

extern void readsensor_init(BaseSequentialStream *chp);


/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(1024)


static const ShellCommand commands[] = { { "loginit", log_init }, { "logexit",
		log_exit }, { "logdata", log_data }, { NULL, NULL } };

static const ShellConfig shell_cfg1 =
		{ (BaseSequentialStream *)&SDU1, commands };

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/




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

	/* init usb serial */
	usbserialinit();

	/*
	 *  Activate Serial Drivers 4 for outputing information.
	 * UART1, 2, 3, 6 are used to connect to the sensors.
	 */
//	sdStart(&SD6, NULL);
//	// PC6(TX) and PC7(RX) are routed to USART6.
//	palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));
//	palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));

	/* do not use SD4, which conflicts with MicroSD */
//	sdStart(&SD4, NULL);
//	// PC10(TX) and PC11(RX) are routed to UART4.
//	palSetPadMode(GPIOC, 10, PAL_MODE_ALTERNATE(8));
//	palSetPadMode(GPIOC, 11, PAL_MODE_ALTERNATE(8));


    /**USE USART3 to send data to WIFI module. GPIO Configuration
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX
    */
	sdStart(&SD3, NULL);
    palSetPadMode(GPIOD, 8, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOD, 9, PAL_MODE_ALTERNATE(7));






	/*
	 * Start SD Driver
	 */
	sdcStart(&SDCD1, NULL);

	// Create shell on USART2
	//shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO + 3);

	/*
	 * Creates the LWIP threads (it changes priority internally).
	 */
	chThdCreateStatic(wa_lwip_thread, LWIP_THREAD_STACK_SIZE,
	NORMALPRIO, lwip_thread, NULL);

	readsensor_init((BaseSequentialStream *)&SD3);

	/*
	 * Set the thread name and set it to the lowest user priority
	 * Since it is just going to be in a while loop.
	 */
	chRegSetThreadName("main");
	chThdSetPriority(LOWPRIO-1);
	while (TRUE) {

	}
	return (int) NULL;
}
