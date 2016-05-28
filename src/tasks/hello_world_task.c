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
#include "hello_world_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "httpserver_raw/LDA_debug.h"

#define STACKSIZE_HELLO_WORLD_TASK       128

static void
hello_world_task(void *pvParameters) {

    //uint32_t ip_addr;

    while (1) {

      //ip_addr = lwIPLocalIPAddrGet();
      send_debug_message( "Hello world!" , DEBUG_MESSAGE_DEFAULT );
      //wsMakeFrame();

        vTaskDelay(3000 / portTICK_RATE_MS);
    }

}

uint32_t
hello_world_init(void) {

    if (xTaskCreate(hello_world_task, (const portCHAR * const)"HELLO_WORLD", STACKSIZE_HELLO_WORLD_TASK, NULL, tskIDLE_PRIORITY + PRIORITY_HELLO_WORLD_TASK, NULL) != pdTRUE) {
        return(1);
    }

    // Success.
    return(0);
}
