#
#www.yealink.com
#

ROOT_PATH = ../
include $(ROOT_PATH)all.config.mk

#where to put the objects we maked,
#there is 2 options 
OBJ_LINK		 =.
OBJ_OUT 		 =.

#working dir 
#link dir
PAGING_BASE_DIR = 
PAGING_LINK_DIR = 
PAGING_WORK_DIR = 

#platform option
ifeq ($(host), $(filter $(host), t21 t19))
CC_PREFIX   =arm-linux-gnueabi-
else
CC_PREFIX   =mipsel-linux-uclibc-
endif

#
CC_PWD	    =$(shell pwd)
#
CC_D_DIR	= .d
CC_O_DIR	= .o
#
CC_ECHO     =
#
CC_DEBUG    =
#
CC_BUILTIN  = built-in.o
#
CC_TARGET_LIB	= libpaging.so
CC_TARGET   = paging

#gcc global link flags
CC_FLAGS	=-Wall -Wstrict-prototypes
LD_FLAGS	=-Wall -Wstrict-prototypes,-rpath,::$(PAGING_WORK_DIR):./:$(PAGING_LINK_DIR)
LD_FLAGS +=-L$(OBJ_LINK)
#ar  global flags
AR_FLAGS	=-crs
#strip  global flags
ST_FLAGS	=-x --strip-unneeded -R .pdr -R .comment -R .note -R .mdebug.abi32 

# Do not print "Entering directory ..."
MAKEFLAGS += --no-print-directory
##################################################################################################################
#cross options

#echo options
ifneq ($(VERBOSE),)
	CC_ECHO=
else
	CC_ECHO=@
endif
#debug options
ifeq ($(DEBUG),)
	CC_DEBUG+=
else
	CC_DEBUG+=-D_DEBUG
endif
#memcheck options
ifeq ($(MEMCHECK),)
	CC_DEBUG+=
else
	CC_DEBUG+=-DDEBUG_MEM_CHECK 
endif
#memcheck options
ifeq ($(MEMCHECKINFO),)
	CC_DEBUG+=
else
	CC_DEBUG+=-DDEBUG_MEM_CHECK_INFO
endif

##################################################################################################################
#setup the comples tools
PAGING_CC		= $(CC_PREFIX)gcc
PAGING_AR		= $(CC_PREFIX)ar
PAGING_SO		= $(CC_PREFIX)gcc -fPIC -shared
PAGING_LD		= $(CC_PREFIX)ld

ifeq ($(STRIP),)
	PAGING_Strip	= $(CC_PREFIX)strip $(ST_FLAGS)
else
	PAGING_Strip	= $(CC_PREFIX)strip
endif

#explort the tools and flags
#
export CC_RELEASE CC_VERSION
#tools
export PAGING_CC PAGING_AR PAGING_LD PAGING_SO PAGING_Strip
#flags
export LD_FLAGS
export CC_FLAGS
export CC_ECHO
export CC_DEBUG
#
export CC_PWD
export CC_D_DIR
export CC_O_DIR
export CC_BUILTIN
export CC_TARGET 
#ar flags
export AR_FLAGS 
#output dir
export OBJ_OUT
##################################################################################################################
#for config
CC_FLAGS	+= $(CC_DEBUG)
CC_FLAGS	+= -I./ -I../ -I$(CC_PWD)
#for export headers
CC_FLAGS	+= -I$(CC_PWD)/include/
#for internals headers
CC_FLAGS	+= -I$(CC_PWD)/src/include/

LD_FLAGS	+= -lpthread 

##################################################################################################################
all:	lib
	@rm -rf paging
	@$(PAGING_CC) main.c -o paging_so -I./include -lpaging -L./ -Wl,-rpath,/mnt/mine/ippaging/
	@$(PAGING_CC)  main.c -o paging libpaging.a -lpthread -I./include -I./src/include

lib:
	@rm -rf $(CC_TARGET_LIB) 
	rm -rf libpaging.a
	@echo "Build src"
	@(cd src ;$(MAKE) all)
	@$(PAGING_SO) ./src/$(CC_BUILTIN) $(LD_FLAGS)  -o $(CC_TARGET_LIB)
#	@$(PAGING_CC) ./src/$(CC_BUILTIN) $(LD_FLAGS)  -o $(CC_TARGET)
#	@echo "Copy $(CC_TARGET_LIB) To $(OBJ_OUT)"
	@(cp -rf ./libpaging.so ../T2x-Lib/lib)
	
#	@echo "Strip   $(OBJ_OUT)/$(CC_TARGET_LIB)"
#	@($(PAGING_Strip) $(OBJ_OUT)/$(CC_TARGET_LIB))

	@echo "ar  -rc libpaging.a ./src/$(CC_BUILTIN)"
	@$(PAGING_AR) -rc libpaging.a ./src/$(CC_BUILTIN)  
	@(cp -rf ./libpaging.a ../T2x-Lib/lib)

	
clean:
	@rm -rf $(CC_TARGET_LIB) 
	rm -rf libpaging.a
	rm -rf ../T2x-Lib/lib/libpaging.so
	@echo "Clean src"
	@find . -type d \
		\( -name '.o' -o -name '.d' \) -print \
		| xargs rm -fr
	@find . -type f -name *.o -print \
		| xargs rm -fr


		
help:
	@echo ""
	@echo "make help for $(CC_TARGET)"
	@echo ""
	@echo "make [CROSS=mips][VERBOSE=y][DEBUG=y][STRIP=y] [MEMCHECK=YES][MEMCHECKINFO=YES]"
	@echo ""
	@echo "make clean"
	@echo ""
	@echo "example:"
	@echo ""
	@echo "make"
	@echo "make DEBUG=YES"
	@echo "make DEBUG=YES MEMCHECK=YES"
	@echo ""
	
