#ifndef __MYSYSTEM_H__
#define __MYSYSTEM_H__

#ifndef IF_COMMONAPI_SYSTEM
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

long open_max(void);
FILE * vpopen(const char * cmdstring, const char * type);
int vpclose(FILE * fp);
#endif // IF_COMMONAPI_SYSTEM


#endif //__MYSYSTEM_H__
