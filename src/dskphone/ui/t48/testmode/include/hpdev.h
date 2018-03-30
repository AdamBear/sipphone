/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2012-10-08
*
*******************************************************************/
#ifndef _HPDEV_H_
#define _HPDEV_H_

#include <poll.h>
#include <linux/fs.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* micro
*/
/*events*/
#define HPDEV_EVENT_INVALID     -1
#define HPDEV_EVENT_ADD         0
#define HPDEV_EVENT_REMOVE      1
#define HPDEV_EVENT_CHANGE      2

/*dev type*/
#define HPDEV_TYPE_INVALID      -1
#define HPDEV_TYPE_STORAGE      0
#define HPDEV_TYPE_BT_HOST      1
#define HPDEV_TYPE_INPUT        2
#define HPDEV_TYPE_MOUSE        3
#define HPDEV_TYPE_KEYBOARD     4
#define HPDEV_TYPE_WLAN         5
#define HPDEV_TYPE_DECT         6
#define HPDEV_TYPE_SERIAL       7
#define HPDEV_TYPE_UNKNOWN      255

/*subsystem*/
#define HPDEV_SUB_INVALID       -1
#define HPDEV_SUB_BLUETOOTH     0
#define HPDEV_SUB_BLOCK         1
#define HPDEV_SUB_INPUT         2
#define HPDEV_SUB_NET           3
#define HPDEV_SUB_USB           4
#define HPDEV_SUB_TTY           5

/*property*/
#define HPDEV_PRO_DISK          0x00000001
#define HPDEV_PRO_PART          0x00000002
#define HPDEV_PRO_RO            0x00010000
#define HPDEV_PRO_RW            0x00020000

#define HPDEV_INFTIM           -1

/************************************************************************/
/* use for debug                                                        */
/************************************************************************/
#define HPDEV_DUMP_HPDEV_EVT(hpdev_evt) do \
{\
    printf("hpdev->event:%d\n", hpdev_evt->dev_event); \
    printf("hpdev->type:%d\n", hpdev_evt->dev_type);\
    printf("hpdev->subsystem:%d\n", hpdev_evt->dev_subsystem);\
    printf("hpdev->major:%d\n", hpdev_evt->dev_major);\
    printf("hpdev->minor:%d\n", hpdev_evt->dev_minor);\
    printf("hpdev->node:%s\n", hpdev_evt->dev_node);\
} while (0)

#define  DEBUG_LEVEL            0

#define  DEBUG_LEVEL_ERR        0
#define  DEBUG_LEVEL_WARN       1
#define  DEBUG_LEVEL_INFO       2
#define  DEBUG_LEVEL_DEBUG      3

#ifdef WIN32
#define DEBUG(fmt,...)       APP_DEBUG(fmt, ##__VA_ARGS__)
#define INFO(fmt,...)        APP_INFO(fmt, ##__VA_ARGS__)
#define WARNING(fmt,...)     APP_WARN(fmt, ##__VA_ARGS__)
#define ERR(fmt,...)         APP_ERR(fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt,args...)      do{if(DEBUG_LEVEL>=DEBUG_LEVEL_DEBUG)printf("hpdev:"fmt,##args);}while(0)
#define INFO(fmt,args...)       do{if(DEBUG_LEVEL>=DEBUG_LEVEL_INFO) printf("hpdev:"fmt,##args);}while(0)
#define WARNING(fmt,args...)    do{if(DEBUG_LEVEL>=DEBUG_LEVEL_WARN) printf("\033[1;33mhpdev[%s:%4d]-WARN-"fmt"\033[0m",__FUNCTION__,__LINE__,##args);}while(0)
#define ERR(fmt,args...)        do{if(DEBUG_LEVEL>=DEBUG_LEVEL_ERR)  printf("\033[1;31mhpdev[%s:%4d]-ERR-"fmt"\033[0m",__FUNCTION__,__LINE__,##args);}while(0)
#endif

/*
* type
*/
typedef unsigned long HPDEV_HANDLE;

typedef struct
{
    /*event*/
    int  dev_event;
    /*info*/
    int  dev_type;
    int  dev_major;
    int  dev_minor;
    /*subsystem*/
    int  dev_subsystem;
    /*usb port*/
    unsigned char dev_port;
    /*reserved*/
    unsigned char reserve[3];
    /*node in /dev*/
    char dev_node[PATH_MAX];
} HPDEV_EVT;

typedef void (*HPDEV_CB)(void * private_data, HPDEV_EVT * dev_evt);

#define PHONE_BIN_T48_SCREEN_CALIBRATION_PATH   PHONE_SCRIPTS_PATH "ts-tool.sh"
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*create/destroy*/
int hpdev_create(HPDEV_HANDLE * pHandle, HPDEV_CB hpdev_cb, void * private_data);
int hpdev_destroy(HPDEV_HANDLE hHandle);

/*run/halt*/
int hpdev_run(HPDEV_HANDLE hHandle);
int hpdev_halt(HPDEV_HANDLE hHandle);

/*poll/process*/
int hpdev_set_events(HPDEV_HANDLE hHandle, short events);
int hpdev_poll(HPDEV_HANDLE hHandle, int time);
int hpdev_process(HPDEV_HANDLE hHandle);
int hpdev_dir_scan(HPDEV_HANDLE hHandle);

/*helper*/
int hpdev_helper_mounted(const char * mount_point);
int hpdev_helper_mount(const char * dev, const char * mount_point, const char * fs_type,
                       unsigned long mountflags, void * data);
int hpdev_helper_umount(const char * mount_point);

int hpdev_helper_format(const char * dev_node);
int hpdev_helper_create_devnode(const char * path, int major, int minor);

struct pollfd * hpdev_helper_get_pollfd(HPDEV_HANDLE hHandle);

int hpdev_helper_ispoint_mounted(const char * mount_point);

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
}
#endif

/* 声音路径 */
#define DEFAULT_RING_FILE   RESOURCE_PATH "testmode/ring.wav"

//残影测试图片路径
#define     PIC_TESTMODE_GRID                       RESOURCE_PATH "testmode/t48/grid.png"
#define     PIC_TESTMODE_GRAY80                     RESOURCE_PATH "testmode/t48/gray80.png"
#define     PIC_TESTMODE_GRAY50                     RESOURCE_PATH "testmode/t48/gray50.png"
#define     PIC_TESTMODE_GRAY30                     RESOURCE_PATH "testmode/t48/gray30.png"

/* 图片路径 */

#define CHECKBOX_PATH_01        RESOURCE_PATH "testmode/t48/btn_checkbox_hd01.png"
#define CHECKBOX_PATH_05        RESOURCE_PATH "testmode/t48/btn_checkbox_hd05.png"

//#define COLORBAR_PATH     RESOURCE_PATH "testmode/colorBar.jpg"
//#define LCD_TEST_PATH     RESOURCE_PATH "testmode/lcdTest.jpg"


/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* _HPDEV_H_ */
