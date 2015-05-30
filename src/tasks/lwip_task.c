//*****************************************************************************
//
// lwip_task.c - Tasks to serve web pages over Ethernet using lwIP.
//
// Copyright (c) 2009-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.0.12573 of the DK-TM4C129X Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "lwiplib.h"
#include "httpserver_raw/httpd.h"
#include "httpserver_raw/fsdata.h"
#include "httpserver_raw/websockd.h"
#include "led_task.h"
#include "lwip_task.h"

extern uint32_t g_ui32SysClock;

//*****************************************************************************
//
// Sets up the additional lwIP raw API services provided by the application.
//
//*****************************************************************************
void
SetupServices(void *pvArg)
{
    //
    // Initialize the sample httpd server.
    //
    //httpd_init();
    //http_server_netconn_init();
    
    // Initialize websocket server
    websockd_init();

}

//*****************************************************************************
//
// Initializes the lwIP tasks.
//
//*****************************************************************************
uint32_t
lwIPTaskInit(void)
{
    uint8_t pui8MAC[6];    

    uint32_t ui32User0, ui32User1;
    ROM_FlashUserGet(&ui32User0, &ui32User1);

    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff)) {

        // MAC address has not been programmed, use default.
        pui8MAC[0] = 0x02;
        pui8MAC[1] = 0x41;
        pui8MAC[2] = 0x42;
        pui8MAC[3] = 0x43;
        pui8MAC[4] = 0x00;
        pui8MAC[5] = 0x00;

    } else {

        pui8MAC[0] = ((ui32User0 >>  0) & 0xff);
        pui8MAC[1] = ((ui32User0 >>  8) & 0xff);
        pui8MAC[2] = ((ui32User0 >> 16) & 0xff);
        pui8MAC[3] = ((ui32User1 >>  0) & 0xff);
        pui8MAC[4] = ((ui32User1 >>  8) & 0xff);
        pui8MAC[5] = ((ui32User1 >> 16) & 0xff);

    }

    //
    // Lower the priority of the Ethernet interrupt handler.  This is required
    // so that the interrupt handler can safely call the interrupt-safe
    // FreeRTOS functions (specifically to send messages to the queue).
    //
    ROM_IntPrioritySet(INT_EMAC0, 0xC0);

    //
    // Initialize lwIP.
    //
    lwIPInit(g_ui32SysClock, pui8MAC, 0, 0, 0, IPADDR_USE_DHCP);

    //
    // Setup the remaining services inside the TCP/IP thread's context.
    //
    tcpip_callback(SetupServices, 0);

    //
    // Success.
    //
    return(0);
}
