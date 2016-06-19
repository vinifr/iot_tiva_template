/*
 * Copyright (c) 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use
 * these files except in compliance with the License. You may obtain a copy of the
 * License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/tm4c129xnczad.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uartstdio.h"
#include "led_task.h"
#include "lwip_task.h"
#include "json_adc.h"
#include "httpserver_raw/LDA_debug.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

uint32_t g_ui32SysClock;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line) {
    //send_debug_assert(pcFilename, ui32Line);
}
#endif

void
vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {

    while(1) {
    }

}

void
pin_init(void) {

    // Enable all the GPIO peripherals
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);

    // PF1/PK4/PK6 are used for Ethernet LEDs
    ROM_GPIOPinConfigure(GPIO_PK4_EN0LED0);
    ROM_GPIOPinConfigure(GPIO_PK6_EN0LED1);
    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_4);
    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_6);

#ifdef DEVKIT
    // PN5 is used for the user LED
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_5);
    ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0);
#else
    // PN0-1 is used for the user LED
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0,  GPIO_PIN_0);
    // PH2-3 is used for the user LED
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_3 | GPIO_PIN_2);
    ROM_GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_3 | GPIO_PIN_2, GPIO_PIN_3 | GPIO_PIN_2);
#endif

}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

int
main(void) {

    // Run from the PLL at 120 MHz.
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480),
                                            configCPU_CLOCK_HZ);

    // Initialize the device pinout appropriately for this board.
    pin_init();
    
    //
    // Initialize the UART and write status.
    //
    ConfigureUART();

    // Make sure the main oscillator is enabled because this is required by
    // the PHY.  The system must have a 25MHz crystal attached to the OSC
    // pins.  The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    // Create the LED task.
    if (LEDTaskInit() != 0) {

        while (1) {
        }

    }

    // Create the lwIP tasks.
    if (lwIPTaskInit() != 0) {

        while (1) {
        }

    }

    // Create the json_test_init task.
    if (json_test_init() != 0) {
        while (1) {
        }
    }

    UARTprintf("FreeRTOS + Lwip\n");

    // Start the scheduler. This should not return.
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, loop forever.
    while (1) {
    }
}

/*
void assert(void *msg)
{
    UARTprintf("ASSERT FAIL at line %d of %s: %s\n", __LINE__, __FILE__, msg);
}
*/

#ifdef PROJ_IMPLEMMENT_SYSCALLS

extern uint32_t __heap_end__;
extern uint32_t _end;

/*==================================================================================================
  Function    : _sbrk

  Description : Increase program data space. As malloc and related functions depend on this, it is
                useful to have a working implementation. The following suffices for a standalone
                system; it exploits the symbol _end automatically defined by the GNU linker.

  Parameters  : incr  [IN]  The size in bytes of a new block of heap memory.

  Returns     : A pointer to a new block of available heap memory.
==================================================================================================*/

caddr_t _sbrk(int incr)
{
  static unsigned char  * heap = NULL;
  static unsigned char  * new_heap;
  unsigned char         * prev_heap_end;

  if (heap == NULL) {
    heap = (unsigned char *) &_end;
  }

  prev_heap_end = heap;
  new_heap = heap + incr;

  if ((new_heap > (unsigned char *) &__heap_end__) || (new_heap < (unsigned char *) &_end)) {
    return NULL;
  }

  heap += incr;
  return (caddr_t) prev_heap_end;
}

#endif /* PROJ_IMPLEMMENT_SYSCALLS */
