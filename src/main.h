#include <stdbool.h>
#include <stdint.h>
#include "snprintf.h"
#include "driverlib/debug.h"

#define true 		1
#define false		0

#define assert(x) ASSERT(x)

//void assert(void *msg);
int websocket_get_data(char *data, int dataLength);
