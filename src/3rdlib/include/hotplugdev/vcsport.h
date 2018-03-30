/******************************************************************************

                  ��Ȩ���� (C), 2011-2021, �����������缼���ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : vcsport.h
  �� �� ��   : ����
  ��    ��   : sunke 00022
  ��������   : 2013��9��24��
  ����޸�   :
  ��������   : VCS PORTģ�鹫��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��9��24��
    ��    ��   : sunke 00022
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
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

/* �ֱ�����ʽ */
typedef struct
{
    unsigned int uiResWid;
    unsigned int uiResHei;
    unsigned int uiFrmRate;

} VCS_RES_PTN;

typedef struct
{
    /* ��������ʾ�ķֱ�����Ŀ, 0 -> ����Чֵ */
    int iResValid; /* 0 <= iResValid <= VIDEO_RES_MAX_NUM */

    /* VCS֧�ֶ��ֱַ��ʣ�
       �����ʾ���ķֱ���VCS��֧�֣��͸�ֵΪ0,������ǰ�ֱ����Ƿ���Ч */
    VCS_RES_PTN stResPtn[VIDEO_RES_MAX_NUM];

    /* ��ʾ��������ߴ�:mm */
    unsigned int uiVertSize;
    unsigned int uiHoriSize;

} DRV_DevManage_DisResInfo;

typedef struct DEV_DEC_RESINFO
{
    int isSupport;
    /* �����ʾ���ķֱ���VCS��֧�֣��͸�ֵΪ0,������ǰ�ֱ����Ƿ���Ч */
    unsigned int uiResWid;
    unsigned int uiResHei;
    unsigned int uiFrmRate;
} DEV_DEC_RESINFO_ST;

/* ���ϲ�Ҫ��������ΪvideoInput0��������ΪvideoInput1 */
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

