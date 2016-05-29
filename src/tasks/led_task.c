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
 * Author:       Joakim Myrland
 * website:      www.LDA.as
 * email:        joakim.myrland@LDA.as
 * project:      https://github.com/Lindem-Data-Acquisition-AS/iot_tiva_template/
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "config/lwiplib.h"
#include "led_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// The stack size for the LED toggle task.
#define STACKSIZE_LEDTASK       128

// The amount of time to delay between toggles of the LED.
#define LED_DELAY_ON    750
#define LED_DELAY_OFF   250

// This task simply toggles the user LED at a 1 Hz rate.
static void
LEDTask(void *pvParameters) {

    portTickType ui32LastTime;
    uint32_t ui32Temp;

    // Get the current tick count.
    ui32LastTime = xTaskGetTickCount();

    // Loop forever.
    while (1) {

        ui32Temp = lwIPLocalIPAddrGet();

        /* No IP acquired */
        if (ui32Temp == IPADDR_NONE || ui32Temp == IPADDR_ANY) {

            // Turn off the user LED.
            #ifdef DEVKIT
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, GPIO_PIN_5);
            #else
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
            #endif

            // Wait for the required amount of time.
            vTaskDelayUntil(&ui32LastTime, LED_DELAY_OFF / portTICK_RATE_MS);

            // Turn on the user LED.
            #ifdef DEVKIT
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0);
            #else
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
            #endif

            // Wait for the required amount of time.
            vTaskDelayUntil(&ui32LastTime, LED_DELAY_OFF / portTICK_RATE_MS);

        /* Auto IP acquired */
        } else if ( (ui32Temp & 0xFFFF) == 0xFEA9 ) {

            // Turn off the user LED.
            #ifdef DEVKIT
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, GPIO_PIN_5);
            #else
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
            #endif

            // Wait for the required amount of time.
            vTaskDelayUntil(&ui32LastTime, LED_DELAY_ON / portTICK_RATE_MS);

            // Turn on the user LED.
            #ifdef DEVKIT
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0);
            #else
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
            #endif

            // Wait for the required amount of time.
            vTaskDelayUntil(&ui32LastTime, LED_DELAY_OFF / portTICK_RATE_MS);

        /* DHCP IP acquired */
        } else if ( (ui32Temp & 0xFFFF) == 0xA8C0 ) {

            // Turn off the user LED.
            #ifdef DEVKIT
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, GPIO_PIN_5);
            #else
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
            #endif

            // Wait for the required amount of time.
            vTaskDelayUntil(&ui32LastTime, LED_DELAY_OFF / portTICK_RATE_MS);

            // Turn on the user LED.
            #ifdef DEVKIT
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0);
            #else
                ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
            #endif

            // Wait for the required amount of time.
            vTaskDelayUntil(&ui32LastTime, LED_DELAY_ON / portTICK_RATE_MS);

        } else {
            vTaskDelayUntil(&ui32LastTime, 2000 / portTICK_RATE_MS);
        }

    }

}

// Initializes the LED task.
uint32_t
LEDTaskInit(void) {

    // Create the LED task.
    if (xTaskCreate(LEDTask, (const portCHAR * const)"LED", STACKSIZE_LEDTASK, NULL, tskIDLE_PRIORITY + PRIORITY_LED_TASK, NULL) != pdTRUE) {
        return(1);
    }

    // Success.
    return(0);
}
