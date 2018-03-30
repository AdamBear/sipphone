/******************************************************************************

                  版权所有 (C), 2011-2021, 厦门亿联网络技术股份有限公司

 ******************************************************************************
  文 件 名   : vcsport.h
  版 本 号   : 初稿
  作    者   : sunke 00022
  生成日期   : 2013年9月24日
  最近修改   :
  功能描述   : VCS PORT模块公共头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月24日
    作    者   : sunke 00022
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
/* In Linux we need to be prepared for cross compiling */
#include <linux/ioctl.h>

/* Endian macros. */
#ifdef __KERNEL__
#  include <asm/byteorder.h>
#else
#  include <endian.h>
#endif

#define POLL_READ
#define USE_VGA
#define USE_AUDIO_DET
//#define DEC_EVENT_NOTIFY

/* Ioctl Commond */
#define PORT_MANAGE_MAGIC            'V'
#define PORT_MANAGE_IO(num)          _IO(PORT_MANAGE_MAGIC, num)
#define VIDEO_OUTPUT1_START         PORT_MANAGE_IO(0) //
#define VIDEO_OUTPUT1_STOP          PORT_MANAGE_IO(1)
#define VIDEO_OUTPUT0_START         PORT_MANAGE_IO(2)
#define VIDEO_OUTPUT0_STOP          PORT_MANAGE_IO(3)
#define VCSPORT_READ_EDID           PORT_MANAGE_IO(4)


#define VIDEO_RES_MAX_NUM           (16)

/* 分辨率制式 */
typedef struct
{
    unsigned int uiResWid;
    unsigned int uiResHei;
    unsigned int uiFrmRate;

} VCS_RES_PTN;

typedef struct
{
    /* 可用于显示的分辨率数目, 0 -> 无有效值 */
    int iResValid; /* 0 <= iResValid <= VIDEO_RES_MAX_NUM */

    /* VCS支持多种分辨率，
       如果显示器的分辨率VCS不支持，就赋值为0,表征当前分辨率是否有效 */
    VCS_RES_PTN stResPtn[VIDEO_RES_MAX_NUM];

    /* 显示器的物理尺寸:mm */
    unsigned int uiVertSize;
    unsigned int uiHoriSize;

} DRV_DevManage_DisResInfo;

typedef struct DEV_DEC_RESINFO
{
    int isSupport;
    /* 如果显示器的分辨率VCS不支持，就赋值为0,表征当前分辨率是否有效 */
    unsigned int uiResWid;
    unsigned int uiResHei;
    unsigned int uiFrmRate;
} DEV_DEC_RESINFO_ST;

/* 经上层要求将主流改为videoInput0，辅流改为videoInput1 */
typedef struct
{
#ifndef POLL_READ
    int time_out;
    /* unit is 10ms */
#endif
    char videoInput0;
    /* hotplug state of Camera input */
    char videoInput1;
    /* hotplug state of PC input */
    char videoOutput0;
    /* hotplug state of HDMI Output */
    char videoOutput1;
    /* hotplug state of video decoder Output */
    char sensorInput;
    /* hotplug state of Sensor input */
    char lightSensorInput;
    /* hotplug state of Light Sensor input */
#ifdef USE_AUDIO_DET
    char audioInput;
    /* hotplug state of audio Input */
    char audioOutput;
    /* hotplug state of audio Output */
#endif /* USE_AUDIO_DET */

#ifdef DEC_EVENT_NOTIFY
    char vin0ResStat;
    /* 7002cap reschange status */

    DEV_DEC_RESINFO_ST stVin1ResInfo;
#endif /* 1 */

    DRV_DevManage_DisResInfo stVidOut0ResInfo;
    DRV_DevManage_DisResInfo stVidOut1ResInfo;

} DRV_DevManage_HotplugState;

