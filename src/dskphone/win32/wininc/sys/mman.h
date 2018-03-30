#pragma once

#define PROT_READ   _IOR
#define PROT_WRITE  _IOW

#define MAP_SHARED 0
#define MAP_FAILED 0

#define mmap(a,b,c,d,e,f) 0
#define munmap(a,b) 0