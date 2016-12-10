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
 * ADC Test - param 1 = sensor de batimentos cardiacos; param 2 = sensor de temperatura:
 * {"jsonrpc": "2.0", "method": "sendADC", "params": [1], "id": 1}
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
char f_adc, adc_rdy;
int data_ok;
int param = 0;
float value[2];

void ADCConfig(void);

void ADCISRHandler(void)
{
    while (!ADCIntStatus(ADC0_BASE, 1, false)){};

    ADCIntClear(ADC0_BASE, 1);
    ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

    f_adc = 1;
    adc_rdy = 1;
}

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
    //float result = 3.32;
    char *str1;
    //estimate
    const jsmntok_t* psToksub =
                &ps_alltoks[(&ps_alltoks[ps_argtok->first_child])->first_child];

    if (pcResponse && iRespMaxLen < (2 + psToksub->end - psToksub->start)) {
        return WORKSTATUS_RPC_ERROR_OUTOFRESBUF;
    }

    // Pega o valor do parametro recebido: 1 - Heart, 2 - Temperatura
    str1 = strstr(pcJSONString+ps_argtok->start, "[");
    param = *(str1+1) - '0';

    // 1 - Sensor de batimentos cardiacos
    if (pcResponse && (param == 1)) 
    {	
	//
	if (ROM_GPIOPinRead(SENSOR_PORT, LO1_PIN) || 
	    ROM_GPIOPinRead(SENSOR_PORT, LO2_PIN))
	    ;
	else
	{
	    ADCProcessorTrigger(ADC0_BASE, 1);
	    while(!adc_rdy) {
		vTaskDelay(2 / portTICK_RATE_MS);
	    }
	    adc_rdy = 0;
	    snprintf(pcResponse, iRespMaxLen, "\"%lu\"", ui32ADC0Value[0]);
	}
    } else
    // 2 - Sensor de temperatura
    if (pcResponse && (param == 2)) {
	ADCProcessorTrigger(ADC0_BASE, 1);
	while(!adc_rdy) {
	    vTaskDelay(2 / portTICK_RATE_MS);
	}
	adc_rdy = 0;
        snprintf(pcResponse, iRespMaxLen, "\"%lu\"", ui32ADC0Value[1]);
	// Debug
	//memset(buff, 0, sizeof(buff));
	//snprintf( buff, 32, "sendADC:%lu", ui32ADC0Value[1]);
	//UARTprintf("%s\n", buff);
    }

    //return status
    return WORKSTATUS_NO_ERROR;
}

// {"jsonrpc": "2.0", "method": "sendADC", "params": [2], "id": 1}
static void json_adc(void *pvParameters) 
{
    int32_t len;
    
    data_ok = 0;
    workstatus_t eStatus = rpc_install_methods(test_methods, sizeof(test_methods)/sizeof(test_methods[0]));

    if(eStatus != WORKSTATUS_NO_ERROR) {
	assert(0);
    }
    //ADCConfig();
    //ADCProcessorTrigger(ADC0_BASE, 1);

    while (1) {
	if (data_ok == 0) {
	    //fprintf(stderr, "fread(): errno=%d\n", errno);
	} else {
	    data_ok = 0;
	    //rpc
	    eStatus = rpc_handle_command(g_input, strlen(g_input), g_output, MY_BUF_SIZE);
		//text reply?
	    if( (len = strlen(g_output)) > 0) {
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
	    value[0] = ((float)ui32ADC0Value[0] * 3.3) / 4096;
	    value[1] = ((float)ui32ADC0Value[1] * 3.3) / 4096;
	    if (param == 1)
		snprintf( buff, 32, "ADC:%lu, Heart:%2f", ui32ADC0Value[0], value[0]);
	    else if (param == 2)
		snprintf( buff, 32, "ADC:%lu, Temp:%2f", ui32ADC0Value[1], value[1]/0.01);
	    UARTprintf("%s\n", buff);
	}
	vTaskDelay(2 / portTICK_RATE_MS);
    }

}

uint32_t json_test_init(void) 
{
    adc_rdy = 0;
    f_adc = 0;

    if (xTaskCreate(json_adc, (const portCHAR * const)"JSON_RPC", 
	STACKSIZE_RPC_TEST, NULL, tskIDLE_PRIORITY + PRIORITY_HELLO_WORLD_TASK, NULL) != pdTRUE) {
        return(1);
    }
    ADCConfig();

    // Success.
    return(0);
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