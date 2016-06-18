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
#include "config/lwiplib.h"
#include "json_adc.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "driverlib/uartstdio.h"
#include "httpserver_raw/websockd.h"


#define STACKSIZE_RPC_TEST       1024

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

/*
 * Json-rpc 2.0 Examples
 * A simple request and response:
 * --> {"jsonrpc": "2.0", "method": "echo", "params": ["Hello JSON-RPC"], "id": 1}
 * <-- {"result": "Hello JSON-RPC", "error": null, "id": 1}
 * 
 * Procedure call with positional parameters:
 * --> {"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}
 * <-- {"jsonrpc": "2.0", "result": 19, "id": 1}
 * 
 * --> {"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2}
 * <-- {"jsonrpc": "2.0", "result": -19, "id": 2}
 * 
 * Procedure call with named parameters:
 * --> {"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3}
 * <-- {"jsonrpc": "2.0", "result": 19, "id": 3}
 * 
 * Notification:
 * --> {"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]}
 * 
 * ADC Test:
 * {"jsonrpc": "2.0", "method": "sendADC", "params": [1], "id": 1}
 */

//rpc prototype
static
workstatus_t echo(const char* const pcJSONString, const jsmntok_t* const ps_argtok,
          const jsmntok_t* const ps_alltoks, char* pcResponse, int RespMaxLen);

static
workstatus_t sendADC(const char* const pcJSONString, const jsmntok_t* const ps_argtok,
          const jsmntok_t* const ps_alltoks, char* pcResponse, int RespMaxLen);

//rpc sig
static methodtable_entry_t test_methods[] = {
    {"echo", "(S)P", echo},
    {"sendADC", "(P)P", sendADC},
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
        snprintf(pcResponse, iRespMaxLen, "\"%.*s\"", 
		 psTokEchoValue->end - psTokEchoValue->start, &pcJSONString[psTokEchoValue->start]);
    }

    //return status
    return WORKSTATUS_NO_ERROR;
}

static
workstatus_t sendADC(const char* const pcJSONString, const jsmntok_t* const ps_argtok,
          const jsmntok_t* const ps_alltoks, char* pcResponse, int iRespMaxLen)
{
    int param;
    float result = 3.32;
    char *str1;
    //estimate
    const jsmntok_t* psToksub =
                &ps_alltoks[(&ps_alltoks[ps_argtok->first_child])->first_child];

    if (pcResponse && iRespMaxLen < (2 + psToksub->end - psToksub->start)) {
        return WORKSTATUS_RPC_ERROR_OUTOFRESBUF;
    }

    // Pega o valor do parametro recebido
    str1 = strstr(pcJSONString+ps_argtok->start, "[");
    param = *(str1+1) - '0';

    // 1 - Sensor de batimentos cardiacos
    if (pcResponse && (param == 1)) {
        snprintf(pcResponse, iRespMaxLen, "\"%f\"", result);
    }
    // 2 - Sensor de temperatura
    if (pcResponse && (param == 2)) {
        snprintf(pcResponse, iRespMaxLen, "\"%f\"", result-1);
    }

    //return status
    return WORKSTATUS_NO_ERROR;
}


static void
json_adc(void *pvParameters) 
{
	int32_t len;
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
	    if( (len = strlen(g_output)) > 0) {
		//hs->allocated = 0;
		//hs->size = len;
		UARTprintf(">> %s\n", g_output);
		libwebsock_send_text((uint8_t *)g_output, len);
	    } else {
		UARTprintf(">> no reply\n");
	    }
	    UARTprintf("%s\n", workstatus_to_string(eStatus));
	    memset(g_output, 0, sizeof(g_output));
	}
	vTaskDelay(10 / portTICK_RATE_MS);
    }

}

uint32_t
json_test_init(void) {

    if (xTaskCreate(json_adc, (const portCHAR * const)"JSON_RPC", 
	STACKSIZE_RPC_TEST, NULL, tskIDLE_PRIORITY + PRIORITY_HELLO_WORLD_TASK, NULL) != pdTRUE) {
        return(1);
    }

    // Success.
    return(0);
}
