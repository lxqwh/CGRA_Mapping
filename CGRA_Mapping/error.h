
#ifndef ERROR_H

#include <stddef.h>

void error(char *msg, ...);



#define FAIL(reason)    error("%s in %s:%d", (reason), __FILE__, \
                                                          (int)__LINE__)

#define OUT_OF_MEMORY()  FAIL("Out of memory")
#define CANT_HAPPEN()    FAIL("Can't happen")




#endif
