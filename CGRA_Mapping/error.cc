#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"
//错误输出同时换行
void error(char *msg, ...)
  { va_list ap;
    va_start(ap, msg);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
  }

