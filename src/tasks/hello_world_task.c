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
#include "snprintf.h"

#include "main.h"
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
#include "driverlib/uartstdio.h"
#include "httpserver_raw/websockd.h"


#define STACKSIZE_HELLO_WORLD_TASK       1024

//#include <stdio.h>  //printf
#include <string.h> //strlen
//#include <assert.h> //asert
#include <errno.h>  //errno

#include "rpc.h"

#define MY_BUF_SIZE 250
char g_input[MY_BUF_SIZE];
char g_output[MY_BUF_SIZE];

int data_ok;


int websocket_get_data(char *data, int dataLength)
{
    int ret = 0;
    if (dataLength > 0) {
	memcpy(g_input, data, dataLength);
	ret = 1;
	data_ok = 1;
    }
    return ret;
}

//rpc prototype
static
workstatus_t echo(const char* const pcJSONString, const jsmntok_t* const ps_argtok,
          const jsmntok_t* const ps_alltoks, char* pcResponse, int RespMaxLen);

//rpc sig
static methodtable_entry_t test_methods[] = {
    {"echo", "(S)P", echo},
};

//rpc body
static
workstatus_t echo(const char* const pcJSONString, const jsmntok_t* const ps_argtok,
          const jsmntok_t* const ps_alltoks, char* pcResponse, int iRespMaxLen)
{
    //estimate
    const jsmntok_t* psTokEchoValue =
                &ps_alltoks[(&ps_alltoks[ps_argtok->first_child])->first_child];
    if (pcResponse && iRespMaxLen < (2 + psTokEchoValue->end - psTokEchoValue->start)) {
        return WORKSTATUS_RPC_ERROR_OUTOFRESBUF;
    }

    //do function
    //nothing

    //write retval
    if (pcResponse) {
        snprintf(pcResponse, iRespMaxLen, "\"%.*s\"", psTokEchoValue->end - psTokEchoValue->start, &pcJSONString[psTokEchoValue->start]);
    }

    //return status
    return WORKSTATUS_NO_ERROR;
}

static void
hello_world_task(void *pvParameters) 
{
    //uint8_t sendstr[23] = "Testing Websocket API";
    //struct websock_state *hs = websock_state_alloc();
    data_ok = 0;
    workstatus_t eStatus = rpc_install_methods(test_methods, sizeof(test_methods)/sizeof(test_methods[0]));
    
    if(eStatus != WORKSTATUS_NO_ERROR) {
	assert(0);
    }

    while (1) {      
	if (data_ok == 0) {
	    //fprintf(stderr, "fread(): errno=%d\n", errno);
	} else {
	    data_ok = 0;
	    UARTprintf("RPC recv: ");
	    UARTprintf(g_input);
	    UARTprintf("\n\n");
	    //rpc
	    eStatus = rpc_handle_command(g_input, strlen(g_input), g_output, MY_BUF_SIZE);
		//text reply?
	    if(strlen(g_output) > 0) {
		UARTprintf(">> %s\n", g_output);
	    } else {
		UARTprintf(">> no reply\n");
	    }
	    UARTprintf("%s\n", workstatus_to_string(eStatus));
	}
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
