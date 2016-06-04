#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
//#include <sys/types.h>

int snprintf(char *str, size_t count, const char *fmt, ...);
int vsnprintf(char *str, size_t count, const char *fmt, va_list arg);