/*
 * Copyright (c) 2015 Lindem Data Acquisition AS. All rights reserved.
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
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/tm4c129xnczad.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/uartstdio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "led_task.h"
#include "lwip_task.h"
#include "hello_world_task.h"
#include "httpserver_raw/LDA_debug.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Number of bytes to send and receive.
#define NUM_SSI_DATA            32

#define GPIO_TEST
#define SPI_ASYNC

uint32_t g_ui32SysClock;
uint32_t pui32DataTx[NUM_SSI_DATA];
uint32_t pui32DataRx[NUM_SSI_DATA];
uint8_t gpio_st;

uint32_t captureMSec;
uint32_t captureTime;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line) {
    send_debug_assert(pcFilename, ui32Line);
}
#endif

void
vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {

    while(1) {
    }

}

void SSI2IntHandler(void)
{
    unsigned long ulStatus;

	// Read interrupt status
	ulStatus = SSIIntStatus(SSI2_BASE, 1);

    //if (ulStatus & SSI_TXEOT)
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, GPIO_PIN_2);

    // Clear interrupts
    SSIIntClear(SSI2_BASE, ulStatus);
}

void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    captureMSec++;

    if(captureMSec <= captureTime) {
#ifdef SPI_ASYNC
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, 0);
        SSIDataPutNonBlocking(SSI2_BASE, pui32DataTx[0]);
        SSIDataPutNonBlocking(SSI2_BASE, pui32DataTx[1]);
        SSIDataPutNonBlocking(SSI2_BASE, pui32DataTx[2]);
        //SSIDataPutNonBlocking(SSI2_BASE, pui32DataTx[3]);
        SSIAdvDataPutFrameEndNonBlocking(SSI2_BASE, pui32DataTx[3]);
#else
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, 0);
        SSIDataPut(SSI2_BASE, pui32DataTx[0]);
        SSIDataPut(SSI2_BASE, pui32DataTx[1]);
        SSIDataPut(SSI2_BASE, pui32DataTx[2]);
        SSIDataPut(SSI2_BASE, pui32DataTx[3]);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, GPIO_PIN_2);
#endif
    }

#ifdef GPIO_TEST
	if (gpio_st) {
		gpio_st = 0;
		ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_PIN_3);
	} else {
		gpio_st = 1;
		ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, 0);
	}
#endif
}

void TimerConfig(void)
{
    // Enable the peripherals used by this example.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configure the two 32-bit periodic timers.  The period of the timer for
    // FreeRTOS run time stats must be at least 10 times faster than the tick
    // rate
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // 100 uS
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (g_ui32SysClock) /
                                           (10000));

    // Setup the interrupts for the timer timeouts.
    ROM_IntEnable(INT_TIMER0A);
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerEnable(TIMER0_BASE, TIMER_A);
}

void pin_init(void)
{

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
    // PA0-1 is used for the user LED
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_1 | GPIO_PIN_0,  GPIO_PIN_0);
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

void InitSPI()
{
    //
    // The SSI2 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);

    //
    // For this example SSI2 is used with PortD[0:3].
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Configure the pin muxing for SSI2 functions on port Q0, Q1, Q2 and Q3.
    // This step is not necessary if your part does not support pin muxing.
    //
    GPIOPinConfigure(GPIO_PD3_SSI2CLK); // SCLK
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);   // MOSI
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);   // MISO
    GPIOPinConfigure(GPIO_PD2_SSI2FSS); // CS
    //GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2); // CS - configure it as GPIO output
    //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, GPIO_PIN_2); // CS - set it to logic HIGH

    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PD0 - SSI2Rx MISO
    //      PD1 - SSI2Tx MOSI
    //      PN2 - SSI2Fss
    //      PD3 - SSI2CLK
    //
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI2,
    // system clock supply, idle clock level low and active low clock in
    // Freescale SPI mode, master mode, 3 MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.
    //
    SSIConfigSetExpClk(SSI2_BASE, g_ui32SysClock, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 3000000, 8);

    SSIAdvModeSet(SSI2_BASE, SSI_ADV_MODE_READ_WRITE);
    // de-assert the SSIFss signal during the entire data transfer
    SSIAdvFrameHoldEnable(SSI2_BASE);

    //
    // Enable the SSI2 module.
    //
    SSIEnable(SSI2_BASE);
#ifdef SPI_ASYNC
    //SSIIntEnable(SSI2_BASE, SSI_TXEOT);
    //SSIIntClear(SSI2_BASE, SSI_TXEOT);
#endif

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(SSIDataGetNonBlocking(SSI2_BASE, &pui32DataRx[0]))
    {
    }
}

int main(void) {

    // Run from the PLL at 120 MHz.
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480),
                                            configCPU_CLOCK_HZ);

    // Initialize the device pinout appropriately for this board.
    pin_init();
    
    // Make sure the main oscillator is enabled because this is required by
    // the PHY.  The system must have a 25MHz crystal attached to the OSC
    // pins.  The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    ConfigureUART();
    InitSPI();
    TimerConfig();

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); //Enable the GPIO port N
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
	ROM_GPIODirModeSet(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT);
	ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_PIN_3);

    pui32DataTx[0] = 0x80;
    pui32DataTx[1] = 0x00;
    pui32DataTx[2] = 0x00;
    pui32DataTx[3] = 0x00;
    captureTime = 10 * 1000000; // 10 sec in uS
    captureTime = (captureTime / 1000);
    captureMSec = 0;

    // Create the LED task.
    if (LEDTaskInit() != 0) {

        while (1) {
        }

    }

    // Create the lwIP tasks.
    /*if (lwIPTaskInit() != 0) {
        while (1) {
        }
    }*/

    // Create the hello world task.
    /*if (hello_world_init() != 0) {
        while (1) {
        }
    }*/
    
    UARTprintf("FreeRTOS + Lwip\n");

    // Start the scheduler. This should not return.
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, loop forever.
    while (1) {
    }
}
