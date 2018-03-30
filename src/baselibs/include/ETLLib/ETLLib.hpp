#ifndef __ETLLib_HPP__
#define __ETLLib_HPP__

#ifndef  __cplusplus
#error use <ETLLib.h> NOT <ETLLib.hpp> for c program
#endif

#ifdef _NEW_CFG_SERVICE
#define IF_USE_REGISTRY             0
#else
#define IF_USE_REGISTRY             1
#endif

#include "ETL.h"
#include "ETLTrace.h"
#include "ETLFile.h"
#include "ETLSocket.h"
#include "ETLMsgSystem.h"
#include "ETLRegistry.h"

#include "encryption.h"
#include "sub-interface.h"
#include "sub-class.h"

//TODO. 为了保证外部引用保持一致，现在这里强行包含以下头文件
#include "dskthread/moddskthread.h"
#include "dsklog/dsklog.h"

#endif //__ETLLib_HPP__
