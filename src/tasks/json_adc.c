/*
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
#include "httpserver_raw/websockd.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/uartstdio.h"


#define STACKSIZE_RPC_TEST       1024

//#include <stdio.h>  //printf
#include <string.h> //strlen
//#include <assert.h> //asert
#include <errno.h>  //errno

#include "rpc.h"

#define MY_BUF_SIZE 128

char g_input[MY_BUF_SIZE];
char g_output[MY_BUF_SIZE];

uint32_t ui32ADC0Value[4];
char buff[32];
char f_adc;
int data_ok;

void ADCConfig(void);

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
	ADCProcessorTrigger(ADC0_BASE, 1);
	//while(!f_adc);
        snprintf(pcResponse, iRespMaxLen, "\"%f\"", result-1);
    }

    //return status
    return WORKSTATUS_NO_ERROR;
}

// {"jsonrpc": "2.0", "method": "sendADC", "params": [2], "id": 1}
static void
json_adc(void *pvParameters) 
{
    int32_t len;
    float value;
    
    data_ok = 0;
    workstatus_t eStatus = rpc_install_methods(test_methods, sizeof(test_methods)/sizeof(test_methods[0]));

    if(eStatus != WORKSTATUS_NO_ERROR) {
	assert(0);
    }
    ADCConfig();
    ADCProcessorTrigger(ADC0_BASE, 1);

    while (1) {
	if (data_ok == 0) {
	    //fprintf(stderr, "fread(): errno=%d\n", errno);
	} else {
	    data_ok = 0;
	    //UARTprintf("RPC recv: ");
	    //UARTprintf(g_input);
	    //UARTprintf("\n\n");
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
	
	if (f_adc) {
	    f_adc = 0;
	    memset(buff, 0, sizeof(buff));
	    value = ((float)ui32ADC0Value[1] * 3.3) / 4096;
	    snprintf( buff, 32, "Tensao:%2f, Temp:%2f", value, value/0.01);
	    UARTprintf("%s\n", buff);
	    vTaskDelay(1000 / portTICK_RATE_MS);
	    //ADCProcessorTrigger(ADC0_BASE, 1);
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

void ADCISRHandler(void)
{
    while (!ADCIntStatus(ADC0_BASE, 1, false)){};

    ADCIntClear(ADC0_BASE, 1);
    ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

    f_adc = 1;
    //ui32TempAvg = ui32ADC0Value[3];
    //ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096) / 10;
}

void ADCConfig(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);

	ADCSequenceDisable(ADC0_BASE, 1);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3); // PE2(Temp) e PE3(Heart)

	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
	//ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH4);
	//ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);
	//ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH3 |	ADC_CTL_IE | ADC_CTL_END);

	IntEnable(INT_ADC0SS1);
	ADCIntEnable(ADC0_BASE, 1);
	ADCSequenceEnable(ADC0_BASE, 1);
}