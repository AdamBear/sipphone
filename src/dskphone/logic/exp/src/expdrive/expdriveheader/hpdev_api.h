/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2007-2013 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2016-06-23
*
*******************************************************************/
#ifndef __HPDEVDRV_API_H__
#define __HPDEVDRV_API_H__

#ifndef __KERNEL__
#include <sys/time.h>
#endif

/************************************************************************/
/*                   driver types                                       */
/************************************************************************/
/*
* type of version
*/
struct hpd_drvver
{
#define HPD_DRVVER_SIZE     16
    char            ver[HPD_DRVVER_SIZE];
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
struct hpd_act
{
#define HPD_ACT_UNKNOW          0
#define HPD_ACT_PLUGIN          1
#define HPD_ACT_DELETE          2
#define HPD_ACT_READY           3
#define HPD_ACT_CHANGE          4
    char            action; /* action */

#define HPD_PORT_0              0
#define HPD_PORT_1              1
#define HPD_PORT_2              2
#define HPD_PORT_3              3
#define HPD_PORT_4              4
#define HPD_PORT_5              5
    char            port;   /* device port number */

#define HPD_TYP_UNKNOW          0
#define HPD_TYP_BASE            101

    /* 1~100 for video device */
#define HPD_TYP_VIDEO_BASE      1
#define HPD_TYP_VIDEO_OUT_HDMI  (HPD_TYP_VIDEO_BASE+0)
#define HPD_TYP_VIDEO_OUT_DP    (HPD_TYP_VIDEO_BASE+1)
#define HPD_TYP_VIDEO_OUT_VGA   (HPD_TYP_VIDEO_BASE+2)

#define HPD_TYP_VIDEO_IN_HDMI   (HPD_TYP_VIDEO_BASE+50)
#define HPD_TYP_VIDEO_IN_DP     (HPD_TYP_VIDEO_BASE+51)
#define HPD_TYP_VIDEO_IN_VGA    (HPD_TYP_VIDEO_BASE+52)

    /* 101~200 for usb device */
#define HPD_TYP_USB_BASE        101
#define HPD_TYP_USB_STORAGE     (HPD_TYP_USB_BASE+0)
#define HPD_TYP_USB_BT          (HPD_TYP_USB_BASE+1)
#define HPD_TYP_USB_EXP         (HPD_TYP_USB_BASE+2)

    /* 201~300 for dsplink device */
#define HPD_TYP_DSPINK_BASE     201
#define HPD_TYP_DSPINK_CPOD     (HPD_TYP_DSPINK_BASE+0)
#define HPD_TYP_DSPINK_DPOD     (HPD_TYP_DSPINK_BASE+1)
#define HPD_TYP_DSPINK_VPOD     (HPD_TYP_DSPINK_BASE+2)
#define HPD_TYP_DSPINK_CAM      (HPD_TYP_DSPINK_BASE+3)
#define HPD_TYP_DSPINK_HOST     (HPD_TYP_DSPINK_BASE+4)
#define HPD_TYP_DSPINK_BOX      (HPD_TYP_DSPINK_BASE+5)
#define HPD_TYP_DSPINK_V4L      (HPD_TYP_DSPINK_BASE+6)

    unsigned short  type;  /* device type */

    char            devid;  /* device ID */
    char            major;  /* device major number */
    char            minor;  /* device minor number */

#define HPD_EXTRA_TYPE_UNKNOW   0
#define HPD_EXTRA_TYPE_1        1
#define HPD_EXTRA_TYPE_2        2
#define HPD_EXTRA_TYPE_3        3
    char            etype;  /* extra type */
    char            reserve[8];
};

struct hpd_extra1
{
    /* chip plug status */
    char            plug_error_code;
    char            plug_rev;

#define HPD_E1_HDMI_PLUGIN      0x0001
#define HPD_E1_DP_PLUGIN        0x0002
#define HPD_E1_VIDEO_RDY        0x0004
#define HPD_E1_AUDIO_RDY        0x0008
#define HPD_E1_USE_HDMI         0x0010
#define HPD_E1_USE_DP           0x0020
    unsigned short  plug_flags;

    /* chip video mode */
    char            video_error_code;
    char            video_rev[3];
    unsigned short  video_witdh;
    unsigned short  video_height;

    unsigned short  video_fps;
    unsigned short  video_scan_mode;
    unsigned int    video_format;

    /* chip audio mode */
    char            audio_error_code;
    unsigned char   audio_channel;
    unsigned short  audio_format;
    unsigned int    audio_sample_rate;
    unsigned int    audio_sample_size;
};

struct hpd_extra2_res
{
    /* video resolution info */
    unsigned int    video_res_wid;
    unsigned int    video_res_hei;
    unsigned int    video_res_rate;
} ;

struct hpd_extra2
{
    /* video physical size(mm) */
    unsigned int    video_size_wid;
    unsigned int    video_size_hei;

    /* video resolution number */
    unsigned int    video_res_num;

    /* supported resolution group */
#define HPD_E2_RES_MAX_NUM      (32)
    struct hpd_extra2_res   video_res_info[HPD_E2_RES_MAX_NUM];
};

struct hpd_extra3
{
    unsigned short  type;
    unsigned short  code;
    int             value;
    unsigned int    ver;
};

struct hpd_event
{
    /* event size max */
#define HPD_EVENT_SIZE                 1024
#define HPD_EVENT_EXTRA_DATA_SIZE      (1024-32)

    /* event base data */
#define HPD_EVENT_VER_MSK   0x0000000F
#define HPD_EVENT_VER_0     0x00000000
    int             flags;  /* event flags */
    struct timeval  time;   /* event time */
    struct hpd_act  action; /* event action */

    /* event extra data */
    size_t          esize;  /* union size,check valid */
    union
    {
        unsigned char       pad[HPD_EVENT_EXTRA_DATA_SIZE];
        struct hpd_extra1   e1;
        struct hpd_extra2   e2;
        struct hpd_extra3   e3;
    };
};

/************************************************************************/
/*                        driver APIs                                   */
/************************************************************************/
#define HPDEV_IOC_MAGIC     'H'
#define HPDEV_IOC_MAXNR     0x60

/************************************************************************/
/*                                                                      */
/************************************************************************/
#define HPDEV_IOC_DRVVER    _IOR(HPDEV_IOC_MAGIC,  0x00, struct hpd_drvver *)

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __KERNEL__

#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
static inline int  hpdev_helper_open(void)
{
    return open("/dev/hpdev", O_RDWR);
}
static inline void hpdev_helper_close(int fd)
{
    if (fd > 0)
    {
        close(fd);
    }
}

static inline int  hpdev_helper_drvver(int fd, char * ver, size_t len)
{
    if (ver && len > 0)
    {
        struct hpd_drvver drver;

        if (ioctl(fd, HPDEV_IOC_DRVVER, (unsigned long)&drver) == 0)
        {
            len = (len < HPD_DRVVER_SIZE ? len : HPD_DRVVER_SIZE) - 1;
            strncpy(ver, drver.ver, len);
            ver[len] = 0;

            return 0;
        }
    }

    return -EFAULT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
};
#endif

#endif /*__KERNEL__*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__HPDEVDRV_API_H__*/
