# Copyright (c) 2012, Mauro Scomparin
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Mauro Scomparin nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY Mauro Scomparin ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Mauro Scomparin BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# File:			Makefile.
# Author:		Mauro Scomparin <http://scompoprojects.worpress.com>.
# Version:		1.0.1.
# Description:	Sample makefile, modified version inspired by Wollw @ github
#
# Modified by:  Joakim Myrland
# website:      www.LDA.as
# email:        joakim.myrland@LDA.as
# project:      https://github.com/Lindem-Data-Acquisition-AS/iot_tiva_template/
#

#global names
LIB_PORT_NAME   = tm4c129
PROJECT_NAME    = iot_template_$(LIB_PORT_NAME)

#path names relative to makefile:
ROOT                    = ..
#path names relative to ROOT:
SRC_PATH                = src
BIN_PATH                = bin
TIVAWARE_PATH           = iot_mcu_lib/TivaWare_C_Series-2.1.0.12573
FreeRTOS_PATH           = iot_mcu_lib/FreeRTOSV8.2.0
lwIP_PATH               = iot_mcu_lib/lwip-1.4.1
jsmn_PATH               = iot_mcu_lib/jsmn

#compiler setup
COMPILER    = gcc
TOOLCHAIN   = arm-none-eabi
PART        = TM4C1294NCPDT
#TM4C1294KCPDT
#TM4C1294NCPDT
#TM4C129ENCPDT
CPU         = cortex-m4
FPU         = fpv4-sp-d16
FABI        = softfp

LINKER_FILE     = ${LIB_PORT_NAME}.ld
STARTUP_FILE    = main

CC = ${TOOLCHAIN}-gcc
LD = ${TOOLCHAIN}-ld
CP = ${TOOLCHAIN}-objcopy
OD = ${TOOLCHAIN}-objdump

CFLAGS  = -mthumb -mcpu=$(CPU) -mfpu=$(FPU) -mfloat-abi=$(FABI)
CFLAGS += -Os -ffunction-sections -fdata-sections
CFLAGS += -MD -std=gnu99 -Wall
CFLAGS += -DPART_$(PART) -c -DTARGET_IS_TM4C129_RA0
CFLAGS += -c -g
CFLAGS += -DDEBUG
CFLAGS += -DRTOS_FREERTOS


#include header directories
CFLAGS += -I ${ROOT}
CFLAGS += -I ${ROOT}/${SRC_PATH}
CFLAGS += -I ${ROOT}/${SRC_PATH}/config
CFLAGS += -I ${ROOT}/${SRC_PATH}/tasks
CFLAGS += -I ${ROOT}/${TIVAWARE_PATH}
#include FreeRTOS
CFLAGS += -I ${ROOT}/${FreeRTOS_PATH}/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I ${ROOT}/${FreeRTOS_PATH}/FreeRTOS/Source/include
#include lwIP
CFLAGS += -I ${ROOT}/${lwIP_PATH}/ports/${LIB_PORT_NAME}/include
CFLAGS += -I ${ROOT}/${lwIP_PATH}/src/include
CFLAGS += -I ${ROOT}/${lwIP_PATH}/src/include/ipv4
CFLAGS += -I ${ROOT}/${lwIP_PATH}/src/include/netif
#include lwIP apps
CFLAGS += -I ${ROOT}/${lwIP_PATH}/apps
#include jsmn
CFLAGS += -I ${ROOT}/${jsmn_PATH}

SRC += ${ROOT}/${TIVAWARE_PATH}/driverlib/uartstdio.c
#include source directories/files
SRC += $(wildcard ${ROOT}/${SRC_PATH}/*.c)
SRC += $(wildcard ${ROOT}/${SRC_PATH}/tasks/*.c)
#include FreeRTOS
SRC += $(wildcard ${ROOT}/${FreeRTOS_PATH}/FreeRTOS/Source/portable/GCC/ARM_CM4F/*.c)
SRC += $(wildcard ${ROOT}/${FreeRTOS_PATH}/FreeRTOS/Source/portable/MemMang/heap_2.c)
SRC += $(wildcard ${ROOT}/${FreeRTOS_PATH}/FreeRTOS/Source/*.c)
#include lwIP
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/ports/${LIB_PORT_NAME}/*.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/ports/${LIB_PORT_NAME}/netif/*.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/core/*.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/core/ipv4/*.c)
#SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/core/ipv6/*.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/core/snmp/*.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/netif/*.c)
#SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/netif/ppp/*.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/src/api/*.c)
#include lwIP apps
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/apps/httpserver_raw/httpd.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/apps/httpserver_raw/httpd_post.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/apps/httpserver_raw/fs.c)
SRC += $(wildcard ${ROOT}/${lwIP_PATH}/apps/httpserver_raw/LDA_debug.c)

#include jsmn
SRC += $(wildcard ${ROOT}/${jsmn_PATH}/jsmn.c)

LIB_GCC_PATH    = ${shell ${CC} ${CFLAGS} -print-libgcc-file-name}
LIBC_PATH       = ${shell ${CC} ${CFLAGS} -print-file-name=libc.a}
LIBM_PATH       = ${shell ${CC} ${CFLAGS} -print-file-name=libm.a}
LFLAGS          = --gc-sections
CPFLAGS         = -Obinary
ODFLAGS         = -S

OBJS = $(SRC:.c=.o)
D_FILES = $(SRC:.c=.d)

#this ECHO functions prints compile progress [nn%] to stdout
ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      ECHO="COUNTTHIS" | grep -c "COUNTTHIS")

N := x
C = $(words $N)$(eval N := x $N)
ECHO = echo "`expr "   \`expr $C '*' 100 / $T\`" : '.*\(....\)$$'`%"
endif

#rules
all: $(OBJS) ${PROJECT_NAME}.axf ${PROJECT_NAME}
	size ${ROOT}/${BIN_PATH}/${PROJECT_NAME}.axf
	@echo make complete

%.o: %.c
#@echo .
	@mkdir -p ${ROOT}/${BIN_PATH}
	@$(CC) -c $(CFLAGS) $< -o $@
	@$(ECHO)

${PROJECT_NAME}.axf: $(OBJS)
	@$(LD) -T $(LINKER_FILE) $(LFLAGS) -o ${ROOT}/${BIN_PATH}/${PROJECT_NAME}.axf $(OBJS) $(LIBM_PATH) $(LIBC_PATH) $(LIB_GCC_PATH) ${ROOT}/${TIVAWARE_PATH}/driverlib/${COMPILER}/libdriver.a

${PROJECT_NAME}: ${PROJECT_NAME}.axf
	@$(CP) $(CPFLAGS) ${ROOT}/${BIN_PATH}/${PROJECT_NAME}.axf ${ROOT}/${BIN_PATH}/${PROJECT_NAME}.bin
	@$(OD) $(ODFLAGS) ${ROOT}/${BIN_PATH}/${PROJECT_NAME}.axf > ${ROOT}/${BIN_PATH}/${PROJECT_NAME}.lst

# make clean rule
clean:
	@rm -fr ${ROOT}/${BIN_PATH}
	@rm -f ${OBJS}
	@rm -f ${D_FILES}
	@echo clean complete

# Rule to flash the project to the board
flash: all
	openocd \
	-c "source [find interface/ftdi/olimex-arm-usb-ocd-h.cfg]" \
	-c "set WORKAREASIZE 0x40000" \
	-c "set CHIPNAME TM4C1294NCPDT" \
	-c "source [find target/stellaris.cfg]" \
	-c "init" \
	-c "targets" \
	-c "reset halt" \
	-c "flash info 0" \
	-c "flash banks" \
	-c "flash write_image erase $(ROOT)/$(BIN_PATH)/$(PROJECT_NAME).axf" \
	-c "verify_image $(ROOT)/$(BIN_PATH)/$(PROJECT_NAME).axf"
	-c "reset" \
	-c "shutdown"

debug: flash
	arm-none-eabi-gdb -ex \
	'target extended-remote | openocd \
	-c "source [find interface/ftdi/olimex-arm-usb-ocd-h.cfg]" \
	-c "set WORKAREASIZE 0x40000" \
	-c "set CHIPNAME TM4C1294NCPDT" \
	-c "transport select jtag" \
	-c "source [find target/stellaris.cfg]" \
	"gdb_port pipe; log_output openocd.log"; monitor reset halt'\
	 $(ROOT)/$(BIN_PATH)/$(PROJECT_NAME).axf
