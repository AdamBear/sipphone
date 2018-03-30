/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2013-06-19
*
* for export APIs header file
*
*    history:
*------------------------------------------------------------------
* 0.0.0.0  ->  1.0.0.0 : 统一所有平台发布版本
* 1.0.0.0  ->  1.0.0.1 : 增加硬件loop mode的功能
* 1.0.0.1  ->  1.0.0.2 : 增加消息接收Log
* 1.0.0.2  ->  1.0.0.3 : 将API的函数和ID分开两个文件定义
* 1.0.0.3  ->  1.0.0.4 : 增加热插拔事件上报功能
* 1.0.0.4  ->  1.0.0.5 : 根据需求调整set_layout,video_creat,快进快退接口
* 1.0.0.5  ->  1.0.0.6 : 增加通知电机转动的接口,sip参数与ipvp参数的转换归UI维护
* 1.0.0.6  ->  1.0.0.7 : 修改模式mic和spk配套使用的方式,增加获取Hot-plug 状态
* 1.0.0.7  ->  1.0.0.8 : 将video request质量的参数合并,修改初始化操作
* 1.0.0.8  ->  1.0.0.9 : ipvp接口内部调整,扩充两个事件ID
* 1.0.0.9  ->  1.0.0.10: 增加蓝牙handsfree模式接口和产线测试模式事件ID
* 1.0.0.10  ->  1.0.0.11: 增加同步调用ipvp API的方式
* 1.0.0.11  ->  1.0.0.15: 统一ipvp API
* 1.0.0.x   ->  1.0.0.15(55):统一server，V83使用版本55
*------------------------------------------------------------------
*
*/

#ifndef __SERVICE_IPVP_H__
#define __SERVICE_IPVP_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* for c
*/
#ifdef WIN32
/*Win32 platform*/
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#include <io.h>
#else
/*Linux platform*/
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#endif

/*
* for framework
*/
#if    defined(IPVP_HAVE_MSGKIT)
#include <mkmessage.h>
#include <mkservice.h>
#elif !defined(IPVP_BUILD)
typedef unsigned long mkit_handle_t;
#endif

/*
* API export
*/
#ifdef WIN32
#  ifdef IPVP_BUILD
#    define IPVP_API __declspec (dllexport)
#  else
#    define IPVP_API __declspec (dllimport)
#  endif
#else /*Linux*/
#  define IPVP_API
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*
    * include basic types
    */
#include "service_ipvp_types.h"

    /*
    * include basic APIs
    */
#include "service_ipvp_api.h"


#ifdef __cplusplus
}
#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __SERVICE_IPVP_H__ */
