/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*    1.0.0.0：           2010-12-20 fwj     第一版发布
*    1.0.0.1：           2012-19-27 fwj     将APIs头文件提出来统一维护
*    1.0.0.1 -> 1.0.0.2：2012-03-20 ysj     增加EXP40类型
*    1.0.0.2 -> 1.0.0.3：2012-04-29 ysj     增加扩展台主单片机的升级接口
*    1.0.0.3 -> 1.0.0.4：2012-05-10 ysj     增加升级扩展台错误返回值的判断
*
*    DATE:2010-12-20
*
*******************************************************************/
#ifndef __EXP_DRV_H__
#define __EXP_DRV_H__

#include <sys/ioctl.h>
#include "fcntl.h"
#include <sys/stat.h>
#include "string.h"
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
/*
* for HOP type
*/
#define EXP_HOP_MIN         0
#define EXP_TYPE_INV        -1
#define EXP_TYPE_38         0
#define EXP_TYPE_39         1
#define EXP_TYPE_EHS        2
#define EXP_TYPE_CM68       3
#define EXP_TYPE_CM69       4
#define EXP_TYPE_40         5
#define EXP_TYPE_20         6

/*
* for exp event
*/
/*event type*/
#define EXP_EVN_OFFLINE      0x00
#define EXP_EVN_ONLINE       0x01
#define EXP_EVN_KEYDOWN      0x02
#define EXP_EVN_KEYUP        0x03
#define EXP_EVN_READY        0x04
#define EXP_EVN_WIRELESS     0x05

#define EXP_ROW_BASE         0x00
#define EXP_ROW_KEY_MIN      0x00
#define EXP_ROW_KEY_MAX      0xFF

#define EXP_COL_BASE         0x00
#define EXP_COL_KEY_MIN      0x00
#define EXP_COL_KEY_MAX      0xFF

/*
* for HOP code
*/
#define EXP_HOP_MIN          0
#define EXP_HOP_INV          -1
#define EXP_HOP_0            0
#define EXP_HOP_1            1
#define EXP_HOP_2            2
#define EXP_HOP_3            3
#define EXP_HOP_4            4
#define EXP_HOP_5            5
#define EXP_HOP_6            6
#define EXP_HOP_7            7
#define EXP_HOP_8            8
#define EXP_HOP_MAX          8

/*
* for EHS
*/
#define EXP_HOOK_OFF         0
#define EXP_HOOK_ON          1
#define EXP_HOOK_RING        2
#define EXP_HOOK_STOPRING    3

/*
* for screen
*/
#define EXP_SCR_MIN          0
#define EXP_SCR_0            0
#define EXP_SCR_1            1
#define EXP_SCR_2            2
#define EXP_SCR_3            3
#define EXP_SCR_MAX          4

/*
* for led
*/
#define EXP_LED_OFF           0
#define EXP_LED_GREEN         1
#define EXP_LED_RED           2
#define EXP_LED_ORANGE        3
#define EXP_LED_RED_FLICK0    4
#define EXP_LED_RED_FLICK1    5
#define EXP_LED_RED_FLICK2    6
#define EXP_LED_RED_FLICK3    7
#define EXP_LED_GREEN_FLICK0  8
#define EXP_LED_GREEN_FLICK1  9
#define EXP_LED_GREEN_FLICK2  10
#define EXP_LED_GREEN_FLICK3  11
#define EXP_LED_ORANGE_FLICK0 12
#define EXP_LED_ORANGE_FLICK1 13
#define EXP_LED_ORANGE_FLICK2 14
#define EXP_LED_ORANGE_FLICK3 15


/*
* for test mode
*/
#define EXP_TM_BLACK        0x00
#define EXP_TM_GRAY0        0x01
#define EXP_TM_GRAY1        0x02
#define EXP_TM_WHITE        0x03

#define EXP_TM_PIC0         0x04
#define EXP_TM_PIC1         0x05
#define EXP_TM_PIC2         0x06
#define EXP_TM_PIC3         0x07

/*
* for message
*/
#define EXP_SCR_MIN         0
#define EXP_SCR_0           0
#define EXP_SCR_1           1
#define EXP_SCR_2           2
#define EXP_SCR_3           3
#define EXP_SCR_MAX         4

#define EXP_BUF_MAX         (46)
#define EXP_MSG_MIN         (sizeof(struct exp_msg))
#define EXP_MSG_MAX         (EXP_MSG_MIN+EXP_BUF_MAX)

struct exp_msg
{
    unsigned short hop;
    unsigned short cmd;
    unsigned int   flags;
#define EXP_MSGFL_NONE    0x00000000
#define EXP_MSGFL_IOMASK  0xFFFF0000
#define EXP_MSGFL_SYNC    0x10000000
#define EXP_MSGFL_SEND    0x20000000
#define EXP_MSGFL_FREE    0x20000000
#define EXP_MSGFL_QUEUE   0x01000000
#define EXP_MSGFL_ACK     0x02000000

#define EXP_MSGFL_NORMAL  0x00000000

#define EXP_MSGFL_HWMASK  0x0000FFFF
#define EXP_MSGFL_POSKASK 0x0000F000
#define EXP_MSGFL_CENTER  0x00001000

#define EXP_MSGFL_MODMASK 0x00000F00
#define EXP_MSGFL_GRAY0   0x00000100
#define EXP_MSGFL_GRAY1   0x00000200
#define EXP_MSGFL_REVERSE 0x00000400

    unsigned short screen;
    unsigned short row;
    unsigned short col;
    unsigned short len;
    unsigned int   val;
#if __KERNEL__
    struct list_head node;
    unsigned long    com;
#else
    unsigned long    reserve[3];
#endif
    unsigned char  * data;
    unsigned char    buff[0];
};


#if __KERNEL__
struct exp_raw
{
#define  EXP_RAW_DATA_MAX  64

    unsigned int cmd;
    unsigned int cmd_tx_len;   /*count as bit*/
    unsigned int cmd_rx_len;   /*count as bit*/

    unsigned int hop;
    unsigned int hop_tx_len;   /*count as bit*/
    unsigned int hop_rx_len;   /*count as bit*/

    unsigned int crc;
    unsigned int crc_tx_len;   /*count as bit*/
    unsigned int crc_rx_len;   /*count as bit*/

    unsigned int  data_tx_len;  /*count as bit*/
    unsigned int  data_rx_len;  /*count as bit*/
    unsigned char data[EXP_RAW_DATA_MAX];

};

/*
* for hop info
*/
struct exp_hop
{
    unsigned long  type;

    unsigned long  status;
#define EXP_HOPST_NONE     0x00000000
#define EXP_HOPST_INIT     0x00000001
#define EXP_HOPST_DESTROY  0x00000002

#define EXP_HOPST_RUNING   0x00000010
#define EXP_HOPST_OFFLINE  0x00000020
#define EXP_HOPST_READY    0x00000040

    unsigned long  flags;
#define EXP_HOPFL_NONE     0x00000000
#define EXP_HOPFL_SWVER    0x10000000
#define EXP_HOPFL_HWVER    0x20000000
#define EXP_HOPFL_ENC      0x40000000
#define EXP_HOPFL_ACK      0x01000000

    unsigned int hw_version;
    unsigned int sw_version;
    unsigned int enc_result;
};
#endif

/*
* for hop event
*/
struct exp_event
{
    struct timeval time;
    unsigned short hop;
    unsigned short code;
    unsigned short row;
    unsigned short col;
    unsigned int   value;
};

/*
* for upgrade
*/
struct exp_upgrade_file
{
    unsigned char * img_ptr;
    unsigned int  img_len;
};

/*
* for IOCTL CMD ID
*/
#define EXPIOC_MAGIC          'Z'

#define EXPIOCS_MIN           _IO (EXPIOC_MAGIC, 0x00)
/*
* basic control
*/
#define EXPIOCS_BASE_MIN      _IO (EXPIOC_MAGIC, 0x10)
/*read hardware version,return in data as string */
#define EXPIOCG_HWVERSION     _IOR(EXPIOC_MAGIC, 0x10, struct exp_msg*)
/*read software version,return in data as string */
#define EXPIOCG_SWVERSION     _IOR(EXPIOC_MAGIC, 0x11, struct exp_msg*)
/*set hardware init*/
#define EXPIOCS_INIT          _IOW(EXPIOC_MAGIC, 0x12, struct exp_msg*)
/*set hardware reset*/
#define EXPIOCS_RESET         _IOW(EXPIOC_MAGIC, 0x13, struct exp_msg*)
/*set encrypt word*/
#define EXPIOCS_ENCRYPT       _IOW(EXPIOC_MAGIC, 0x14, struct exp_msg*)
/*get encrypt word*/
#define EXPIOCG_ENCRYPT       _IOR(EXPIOC_MAGIC, 0x15, struct exp_msg*)
/*set led */
#define EXPIOCS_LED           _IOW(EXPIOC_MAGIC, 0x16, struct exp_msg*)
/*get led */
#define EXPIOCG_LED           _IOR(EXPIOC_MAGIC, 0x17, struct exp_msg*)

/*set LCD item */
#define EXPIOCS_LCDITEM       _IOW(EXPIOC_MAGIC, 0x18, struct exp_msg*)
/*get LCD item */
#define EXPIOCG_LCDITEM       _IOR(EXPIOC_MAGIC, 0x19, struct exp_msg*)

/*set LCD icon */
#define EXPIOCS_LCDICON       _IOW(EXPIOC_MAGIC, 0x20, struct exp_msg*)
/*get LCD icon */
#define EXPIOCG_LCDICON       _IOR(EXPIOC_MAGIC, 0x21, struct exp_msg*)

/*set LCD back light ,form 0 to 100*/
#define EXPIOCS_LCDBKLIGHT    _IOW(EXPIOC_MAGIC, 0x22, struct exp_msg*)
/*get LCD back light */
#define EXPIOCG_LCDBKLIGHT    _IOR(EXPIOC_MAGIC, 0x23, struct exp_msg*)

/*set LCD contrast  ,form 0 to 100*/
#define EXPIOCS_LCDCONTRAST   _IOW(EXPIOC_MAGIC, 0x24, struct exp_msg*)
/*get LCD contrast */
#define EXPIOCG_LCDCONTRAST   _IOR(EXPIOC_MAGIC, 0x25, struct exp_msg*)

/*set LCD switch */
#define EXPIOCS_LCDSWITCH     _IOW(EXPIOC_MAGIC, 0x26, struct exp_msg*)
/*get LCD switch */
#define EXPIOCG_LCDSWITCH     _IOR(EXPIOC_MAGIC, 0x27, struct exp_msg*)

/*set test mode */
#define EXPIOCS_TESTMODE      _IOW(EXPIOC_MAGIC, 0x28, struct exp_msg*)
/*get test mode */
#define EXPIOCG_TESTMODE      _IOR(EXPIOC_MAGIC, 0x29, struct exp_msg*)

/*wait sync */
#define EXPIOCS_SYNC          _IO(EXPIOC_MAGIC, 0x30)

/*get hop count */
#define EXPIOCS_HOPCOUNT      _IOW(EXPIOC_MAGIC,0x31, struct exp_msg*)

/*get hop type */
#define EXPIOCS_HOPTYPE       _IOW(EXPIOC_MAGIC,0x32, struct exp_msg*)

/*send ehs hook*/
#define EXPIOCS_EHSHOOK       _IOW(EXPIOC_MAGIC,0x33, struct exp_msg*)
/*get ehs hook*/
#define EXPIOCG_EHSHOOK       _IOR(EXPIOC_MAGIC,0x34, struct exp_msg*)

/*firmware upgrade */
#define EXPIOCS_UPGRADE       _IOW(EXPIOC_MAGIC, 0x50, struct exp_upgrade_file*)

/*sending raw*/
#define EXPIOCS_SENDRAW       _IO (EXPIOC_MAGIC,0x51)

/*recv raw*/
#define EXPIOCS_RECVRAW       _IO (EXPIOC_MAGIC,0x52)

#define EXPIOCS_BASE_MAX      _IO (EXPIOC_MAGIC, 0x60)

#define EXPIOCS_MAX           _IO (EXPIOC_MAGIC, 0xFF)
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __KERNEL__
/************************************************************************/
/*                          user helper API                             */
/************************************************************************/

static inline int exp_helper_open(void)
{
    return open("/dev/exp0", O_RDWR);
}


static inline void exp_helper_close(int fd)
{
    if (fd > 0)
    {
        close(fd);
    }
}

/*
* function: get hardware version from exp hop
* fd:       file descriptions
* hop:      exp hop
* buff:     hardware version string buffer
* buff_len: string length
*/
static inline int exp_helper_event(int fd, struct exp_event * event)
{
    if (fd > 0)
    {
        if (read(fd, event, sizeof(struct exp_event)) >= sizeof(struct exp_event))
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get hardware version from exp hop
* fd:       file descriptions
* hop:      exp hop
* buff:     hardware version string buffer
* buff_len: string length
*/
static inline int exp_helper_get_hwversion(int fd, int hop, int buff_len, char * buff)
{
    if (fd > 0 && buff)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop  = hop;
        msg.data = (unsigned char *)buff;
        msg.len  = buff_len;

        return ioctl(fd, EXPIOCG_HWVERSION, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function: get software version from exp hop
* fd:       file descriptions
* hop:      exp hop
* buff:     software version string buffer
* buff_len: string length
*/
static inline int exp_helper_get_swversion(int fd, int hop, int buff_len, char * buff)
{
    if (fd > 0 && buff)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop  = hop;
        msg.data = (unsigned char *)buff;
        msg.len  = buff_len;

        return ioctl(fd, EXPIOCG_SWVERSION, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function: inititial exp hop
* fd:       file descriptions
* hop:      exp hop
*/
static inline int exp_helper_init(int fd, int hop)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop  = hop;

        return ioctl(fd, EXPIOCS_INIT, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function: reset exp hop
* fd:       file descriptions
* hop:      exp hop
*/
static inline int exp_helper_reset(int fd, int hop)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop  = hop;

        return ioctl(fd, EXPIOCS_RESET, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function: send Encryption factor to exp hop
* fd:       file descriptions
* hop:      exp hop
* enc:      Encryption factor
*/
static inline int exp_helper_set_encrypt(int fd, int hop, int enc)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop  = hop;
        msg.val  = enc;

        return ioctl(fd, EXPIOCS_ENCRYPT, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     get Encryption result from exp hop
* fd:           file descriptions
* hop:          exp hop
* enc_result:   Encryption result point
*/
static inline int exp_helper_get_encrypt(int fd, int hop, int * enc_result)
{
    if (fd > 0 && enc_result)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop  = hop;
        *enc_result = 0;

        if (!ioctl(fd, EXPIOCG_ENCRYPT, (unsigned long)&msg))
        {
            *enc_result = msg.val;
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function:     set led status to exp hop
* fd:           file descriptions
* hop:          exp hop
* row:          led position row
* col:          led position col
* led_status:   led status
*/
static inline int exp_helper_set_led_status(int fd, int hop, int row, int col, int led_status)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop = hop;
        msg.row = row;
        msg.col = col;
        msg.val = led_status;

        return ioctl(fd, EXPIOCS_LED, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     draw line infomation to exp hop
* fd:           file descriptions
* hop:          exp hop
* screen:       lcd screen number
* row           line row
* col           line col
* icon_number:  display icon number
* flags:        display properties
* str_len:      display string length
* str_buff:     string buffer
*/
static inline int exp_helper_set_lcd_item(int fd, int hop, int screen, int row, int col,
        int icon_number, int flags, int str_len, char * str_buff)
{
    if (fd > 0 && str_buff && str_len >= 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        msg.len    = str_len > EXP_BUF_MAX ? EXP_BUF_MAX : str_len;
        msg.screen = screen;
        msg.row    = row;
        msg.col    = col;
        msg.val    = icon_number;
        msg.flags  = flags;
        msg.data   = (unsigned char *)str_buff;

        return ioctl(fd, EXPIOCS_LCDITEM, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     set icon to exp hop
* fd:           file descriptions
* hop:          exp hop
* screen:       lcd screen number
* row           line row
* col           line col
* icon_number:  display icon number
*/
static inline int exp_helper_set_lcd_icon(int fd, int hop, int screen, int row, int col,
        int icon_number)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        msg.screen = screen;
        msg.row    = row;
        msg.col    = col;
        msg.val    = icon_number;

        return ioctl(fd, EXPIOCS_LCDICON, (unsigned long)&msg);
    }
    return -EFAULT;
}


/*
* function:     set lcd back ground light to exp hop
* fd:           file descriptions
* hop:          exp hop
* bk:           back ground light level
*/
static inline int exp_helper_set_lcd_bklight(int fd, int hop, int bk)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        msg.val    = bk;

        return ioctl(fd, EXPIOCS_LCDBKLIGHT, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     set lcd contrast to exp hop
* fd:           file descriptions
* hop:          exp hop
* contrast:     lcd contrast level
*/
static inline int exp_helper_set_lcd_contrast(int fd, int hop, int contrast)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        msg.val    = contrast;

        return ioctl(fd, EXPIOCS_LCDCONTRAST, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     set lcd screen to exp hop
* fd:           file descriptions
* hop:          exp hop
* screen:       lcd screen number
*/
static inline int exp_helper_set_lcd_screen(int fd, int hop, int screen)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        msg.screen = screen;

        return ioctl(fd, EXPIOCS_LCDSWITCH, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     set lcd test mode to exp hop
* fd:           file descriptions
* hop:          exp hop
* status:       screen status
*/
static inline int exp_helper_set_lcd_testmode(int fd, int hop, int status)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        msg.val = status;

        return ioctl(fd, EXPIOCS_TESTMODE, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     set lcd sync to exp hop
* fd:           file descriptions
* hop:          exp hop
*/
static inline int exp_helper_sync(int fd)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));

        return ioctl(fd, EXPIOCS_SYNC, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     get hop count form exp hop
* fd:           file descriptions
* hop_count:    hop count point
*/
static inline int exp_helper_get_hopcount(int fd, int * hop_count)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        *hop_count = 0;

        if (!ioctl(fd, EXPIOCS_HOPCOUNT, (unsigned long)&msg))
        {
            *hop_count = msg.val;
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function:     get hop type form exp hop
* fd:           file descriptions
* hop:          exp hop
* hop_type:     hop type point
*/
static inline int exp_helper_get_hoptype(int fd, int hop, int * hop_type)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop    = hop;
        *hop_type = 0;

        if (!ioctl(fd, EXPIOCS_HOPTYPE, (unsigned long)&msg))
        {
            *hop_type = msg.val;
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function:     set EHS hook status
* fd:           file descriptions
* hop:          exp hop
* hook:         hook status
*/
static inline int exp_helper_set_ehs_hook(int fd, int hop, int hook)
{
    if (fd > 0)
    {
        struct exp_msg msg;
        memset(&msg, 0, sizeof(msg));
        msg.hop = hop;
        msg.val = hook;

        return ioctl(fd, EXPIOCS_EHSHOOK, (unsigned long)&msg);
    }
    return -EFAULT;
}

/*
* function:     set EHS/EXP hook status
* fd:           file descriptions
* img_path:     img文件路径
*/
static inline int exp_helper_upgrade(int fd, const char * img_path)
{
    if (fd > 0 && img_path)
    {
        struct exp_upgrade_file upgrade;
        struct stat stat;
        int    fd_img;
        int    retval;

        memset(&upgrade, 0, sizeof(upgrade));

        fd_img = open(img_path, O_RDONLY);
        if (fd_img <= 0)
        {
            return -EFAULT;
        }

        if (fstat(fd_img, &stat) < 0)
        {
            retval = -EFAULT;
            goto upgrad_done;
        }

        upgrade.img_len = stat.st_size;
        upgrade.img_ptr = (unsigned char *)malloc(upgrade.img_len);
        if (upgrade.img_ptr == NULL)
        {
            retval = -ENOMEM;
            goto upgrad_done;
        }

        if (read(fd_img, upgrade.img_ptr, upgrade.img_len) != upgrade.img_len)
        {
            retval = -EFAULT;
            goto upgrad_done;
        }

        retval = ioctl(fd, EXPIOCS_UPGRADE, (unsigned long)&upgrade);
        if (retval == ENODEV)
        {
            retval = -ENODEV;
        }

upgrad_done:
        if (fd_img)
        {
            close(fd_img);
        }
        if (upgrade.img_ptr)
        {
            free(upgrade.img_ptr);
        }

        return retval;
    }

    return -EFAULT;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__EXP_DRV_H__*/


