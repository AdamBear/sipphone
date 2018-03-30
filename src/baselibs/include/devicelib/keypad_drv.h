/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*    1.0.0.0           ：2011-04-29 fwj     第一版发布
*    1.0.0.1           ：2012-19-27 fwj     将APIs头文件提出来统一维护
*    1.0.0.1 -> 1.0.0.2：2013-03-20 ysj     增加灯闪烁接口，更改模拟按键测试接口，
*                                           兼容T3X
*    1.0.0.2 -> 1.0.0.3：2013-05-02 jh      添加必须头文件，keypad_helper_reset接口修改为同步系统后重启，添加keypad_helper_open和keypad_helper_close接口。
*    1.0.0.3 -> 1.0.0.4：2013-05-16 jh      keypad_helper_sync同步时不需要杀死进程
*    1.0.0.4 -> 1.0.0.5：2013-05-16 cxm     增加风扇转速设置接口
*    1.0.0.5 -> 1.0.0.6：2013-10-14 ysj     增加micbias值
*    1.0.0.6 -> 1.0.0.7：2013-10-28 ysj     增加获取micbias与headset状态接口
*    1.0.0.7 -> 1.0.0.8：2013-12-31 ysj     修改LED接口
*
*    DATE:2011-04-29
*
*       APIs for HT/Keypad device
*******************************************************************/
#ifndef __KEYPAD_DRV_API_H__
#define __KEYPAD_DRV_API_H__

/*
******************************************************************************
* basic
******************************************************************************
*/
/*
* for limitation
*/
#define MAX_PSTN_NUMBER             32
#define MAX_ENCTYPT_BYTE            2

/*
*for hardware version
*/
#define MAX_HW_NUMBER               32 /*255.255.255.255.255.255.255*/
#define MAX_SW_NUMBER               8  /*255.255*/

/*
*for backlight
*/
#define MAX_BACKLIGHT               100
#define MIN_BACKLIGHT               0

/*
******************************************************************************
* PSTN
******************************************************************************
*/
/*
*for fsk timer
*/
#define PSTN_MAX_FSK_NUMBER         4

struct pstn_fsk
{
    int month;
    int day;
    int hour;
    int min;
};
/*
* control code
*/
#define PSTN_DIALNUM_0              0  /*0*/
#define PSTN_DIALNUM_1              1  /*1*/
#define PSTN_DIALNUM_2              2  /*2*/
#define PSTN_DIALNUM_3              3  /*3*/
#define PSTN_DIALNUM_4              4  /*4*/
#define PSTN_DIALNUM_5              5  /*5*/
#define PSTN_DIALNUM_6              6  /*6*/
#define PSTN_DIALNUM_7              7  /*7*/
#define PSTN_DIALNUM_8              8  /*8*/
#define PSTN_DIALNUM_9              9  /*9*/
#define PSTN_DIALNUM_A              10 /*A*/
#define PSTN_DIALNUM_B              11 /*B*/
#define PSTN_DIALNUM_C              12 /*C*/
#define PSTN_DIALNUM_D              13 /*D*/
#define PSTN_DIALNUM_STAR           14 /***/
#define PSTN_DIALNUM_SHARP          15 /*#*/

#define PSTN_CTRL_GRABCTRL          16  /*接管PSTN*/
#define PSTN_CTRL_ENABLE            17  /*允许PSTN来电*/
#define PSTN_CTRL_DISABLE           18  /*禁用PSTN来电*/
#define PSTN_CTRL_OFFHOOK           19  /*告诉PSTN摘机*/
#define PSTN_CTRL_ONHOOK            20  /*告诉PSTN挂机*/
#define PSTN_CTRL_OPEN_HFMIC        21  /*打开免提MIC*/
#define PSTN_CTRL_CLOSE_HFMIC       22  /*关闭免提MIC*/
#define PSTN_CTRL_OPEN_HSMIC        23  /*打开手柄MIC*/
#define PSTN_CTRL_CLOSE_HSMIC       24  /*关闭手柄MIC*/
#define PSTN_CTRL_OPEN_HFSPK        25  /*打开免提SPK*/
#define PSTN_CTRL_CLOSE_HFSPK       26  /*关闭免提SPK*/
#define PSTN_CTRL_OPEN_HSSPK        27  /*打开手柄SPK*/
#define PSTN_CTRL_CLOSE_HSSPK       28  /*关闭手柄SPK*/
#define PSTN_CTRL_OPEN_HSSEND_PSTN  29  /*打开PSTN手柄声音发送通道，关闭SIP声音发送通道*/
#define PSTN_CTRL_OPEN_HSSEND_SIP   30  /*关闭PSTN声音发送通道，打开SIP手柄声音发送通道*/
#define PSTN_CTRL_OPEN_HFSEND_PSTN  31  /*打开PSTN免提声音发送通道， 关闭SIP声音发送通道*/
#define PSTN_CTRL_OPEN_HFSEND_SIP   32  /*关闭PSTN声音发送通道， 打开SIP免提声音发送通道*/
#define PSTN_CTRL_OPEN_HSRECV_PSTN  33  /*打开PSTN手柄声音接收通道，关闭SIP声音接收通道*/
#define PSTN_CTRL_OPEN_HSRECV_SIP   34  /*关闭PSTN声音接收通道, 打开SIP手柄声音接收通道*/
#define PSTN_CTRL_OPEN_HFRECV_PSTN  35  /*打开PSTN免提声音接收通道, 关闭SIP声音接收通道*/
#define PSTN_CTRL_OPEN_HFRECV_SIP   36  /*关闭PSTN声音接收通道,打开SIP免提声音接收通道*/
#define PSTN_CTRL_OPEN_SEND_PSTN    37  /*打开PSTN手柄和免提发送通道, 关闭SIP手柄和免提发送通道*/
#define PSTN_CTRL_OPEN_SEND_SIP     38  /*关闭PSTN手柄和免提发送通道, 打开SIP手柄和免提发送通道*/
#define PSTN_CTRL_OPEN_RECV_PSTN    39  /*打开PSTN手柄和免提接收通道, 关闭SIP手柄和免提接收通道*/
#define PSTN_CTRL_OPEN_RECV_SIP     40  /*关闭PSTN手柄和免提接收通道, 打开SIP手柄和免提接收通道*/
#define PSTN_CTRL_PSTN_MUTE         41  /*PSTN mute*/
#define PSTN_CTRL_PSTN_UNMUTE       42  /*PSTN unmute*/
#define PSTN_CTRL_PSTN_HOLD         43  /*PSTN Hold*/
#define PSTN_CTRL_PSTN_UNHOLD       44  /*PSTN unhold*/
#define PSTN_CTRL_PSTN_FLASH        45  /*PSTN flash*/
#define PSTN_CTRL_OPEN_PSTN_HSSEND  46  /*打开PSTN手柄声音发送通道*/
#define PSTN_CTRL_CLOSE_PSTN_HSSEND 47  /*关闭PSTN手柄声音发送通道*/
#define PSTN_CTRL_OPEN_PSTN_HFSEND  48  /*打开PSTN免提声音发送通道*/
#define PSTN_CTRL_CLOSE_PSTN_HFSEND 49  /*关闭PSTN免提声音发送通道*/
#define PSTN_CTRL_OPEN_PSTN_HSRECV  50  /*打开PSTN手柄声音接收通道*/
#define PSTN_CTRL_CLOSE_PSTN_HSRECV 51  /*关闭PSTN手柄声音接收通道*/
#define PSTN_CTRL_OPEN_PSTN_HFRECV  52  /*打开PSTN免提声音接收通道*/
#define PSTN_CTRL_CLOSE_PSTN_HFRECV 53  /*关闭PSTN免提声音接收通道*/
#define PSTN_CTRL_OPEN_SIP_HSSEND   54  /*打开sip手柄声音发送通道*/
#define PSTN_CTRL_CLOSE_SIP_HSSEND  55  /*关闭sip手柄声音发送通道*/
#define PSTN_CTRL_OPEN_SIP_HFSEND   56  /*打开sip免提声音发送通道*/
#define PSTN_CTRL_CLOSE_SIP_HFSEND  57  /*关闭sip免提声音发送通道*/
#define PSTN_CTRL_OPEN_SIP_HSRECV   58  /*打开sip手柄声音接收通道*/
#define PSTN_CTRL_CLOSE_SIP_HSRECV  59  /*关闭sip手柄声音接收通道*/
#define PSTN_CTRL_OPEN_SIP_HFRECV   60  /*打开sip免提声音接收通道*/
#define PSTN_CTRL_CLOSE_SIP_HFRECV  61  /*关闭sip免提声音接收通道*/

/*
******************************************************************************
* event
******************************************************************************
*/
#define KEYPAD_EV_KEY_MIN            0
#define KEYPAD_EV_KEY_MID            90
#define KEYPAD_EV_KEY_MAX            179

/*key for fix*/
#define KEYPAD_EV_KEY_HOOK           89
#define KEYPAD_EV_KEY_HEADSET        88
#define KEYPAD_EV_KEY_PAGING         87  /*paging*/
#define KEYPAD_EV_KEY_RESET          86  /*reset*/
#define KEYPAD_EV_KEY_MICBIAS        85

#define KEYPAD_EV_PSTN_MIN           180
#define KEYPAD_EV_PSTN_MAX           209

#define KEYPAD_EV_DIALIN             180
#define KEYPAD_EV_CIDOUT             196
#define KEYPAD_EV_CIDPRI             197
#define KEYPAD_EV_CIDERR             198
#define KEYPAD_EV_FSK                199
#define KEYPAD_EV_RING               200
#define KEYPAD_EV_FXO_ON             201
#define KEYPAD_EV_FXO_OFF            202
#define KEYPAD_EV_REMOTE_HOOKOFF     203
#define KEYPAD_EV_REMOTE_HOOKON      204
#define KEYPAD_EV_LOCAL_HOOKOFF      205
#define KEYPAD_EV_LOCAL_HOOKON       206

/*dial key*/
#define KEYPAD_EV_DIALNUM_MIN        180
#define KEYPAD_EV_DIALNUM_MAX        195
#define KEYPAD_EV_DIALNUM_0          180  /*0*/
#define KEYPAD_EV_DIALNUM_1          181  /*1*/
#define KEYPAD_EV_DIALNUM_2          182  /*2*/
#define KEYPAD_EV_DIALNUM_3          183  /*3*/
#define KEYPAD_EV_DIALNUM_4          184  /*4*/
#define KEYPAD_EV_DIALNUM_5          185  /*5*/
#define KEYPAD_EV_DIALNUM_6          186  /*6*/
#define KEYPAD_EV_DIALNUM_7          187  /*7*/
#define KEYPAD_EV_DIALNUM_8          188  /*8*/
#define KEYPAD_EV_DIALNUM_9          189  /*9*/
#define KEYPAD_EV_DIALNUM_A          190  /*A*/
#define KEYPAD_EV_DIALNUM_B          191  /*B*/
#define KEYPAD_EV_DIALNUM_C          192  /*C*/
#define KEYPAD_EV_DIALNUM_D          193  /*D*/
#define KEYPAD_EV_DIALNUM_STAR       194  /***/
#define KEYPAD_EV_DIALNUM_SHARP      195  /*#*/

/*
******************************************************************************
* holtek
******************************************************************************
*/
/*
*for watchdog
*/
#define WATCHDOG_ID_CLOSE            0
#define WATCHDOG_ID_SECMIN           1
#define WATCHDOG_ID_SECMAX           200
#define WATCHDOG_ID_RESET            201
#define WATCHDOG_ID_5MIN             202
#define WATCHDOG_ID_10MIN            203
#define WATCHDOG_ID_POWEROFF         204
#define WATCHDOG_ID_SLEEP            205
#define WATCHDOG_ID_WAKEUP           206
#define WATCHDOG_ID_UNUSE            207-255

/*
******************************************************************************
* ioctl control
******************************************************************************
*/
#define KPIOCS_MIN                   _IO ('X', 0x00)
#define KPIOCS_MAX                   _IO ('X', 0xFF)

/*
******************************************************************************
* basic control
******************************************************************************
*/
/*mask of led ,64bit as design*/
typedef unsigned long long led64;

/*policy of led*/
struct led_policy
{
    led64    mask;  /*mask of led*/
    int      flags; /*flags of led*/
    short    on;    /*time of on*/
    short    off;   /*time of off*/
};
/*flick of led*/
struct led_flick
{
    int               count;  /*count of policy*/
    struct led_policy * policy;
};

#define KPIOCS_BASE_MIN              _IO ('X', 0x10)

/*read hardware version*/
#define KPIOCG_HWVERSION             _IOR('X', 0x10, char[MAX_HW_NUMBER])
/*read software version*/
#define KPIOCG_SWVERSION             _IOR('X', 0x11, char[MAX_SW_NUMBER])
/*set encrypt word*/
#define KPIOCS_ENCRYPT               _IOW('X', 0x12, unsigned int *)
/*get encrypt word*/
#define KPIOCG_ENCRYPT               _IOR('X', 0x13, unsigned int *)
/*set watchdog*/
#define KPIOCS_WATCHDOG              _IOW('X', 0x14, int*)
/*get watchdog*/
#define KPIOCG_WATCHDOG              _IOR('X', 0x15, int*)
/*set led value*/
#define KPIOCS_SETLED                _IOW('X', 0x16, unsigned int*)
/*get led value*/
#define KPIOCG_GETLED                _IOR('X', 0x17, unsigned int*)
/*get led flick*/
#define KPIOCG_GETLED_FLICK          _IOR('X', 0x18, struct led_flick*)
/*set led flick*/
#define KPIOCS_SETLED_FLICK          _IOW('X', 0x19, struct led_flick*)
/*get led mask*/
#define KPIOCS_GETLED_MASK           _IOR('X', 0x1A, int*)
/*go self verify*/
#define KPIOCS_VERIFY                _IOW('X', 0x1B, unsigned int)
/*set FanSpeed*/
#define KPIOCS_FANSPEED              _IOW('X', 0x1C, int*)
/*get FanSpeed*/
#define KPIOCG_FANSPEED              _IOR('X', 0x1D, int*)
/*get headset status*/
#define KPIOCG_HEADSET_STA           _IOR('X', 0x1E, int*)
/*get micbias status*/
#define KPIOCG_MICBIAS_STA           _IOR('X', 0x1F, int*)

#define KPIOCS_BASE_MAX              _IO ('X', 0x3F)




/*
******************************************************************************
* PSTN control
******************************************************************************
*/
#define KPIOCS_PSTN_MIN              _IO ('X', 0x40)

#define KPIOCS_PSTN_CMD              _IOW('X', 0x40,int*)

#define KPIOCG_PSTN_DIAL             _IOR('X', 0x41,char*)

#define KPIOCG_PSTN_FSK              _IOR('X', 0x42,struct pstn_fsk*)

#define KPIOCS_PSTN_MAX              _IO ('X', 0x4F)

/*
******************************************************************************
* LCD control
******************************************************************************
*/
/*set pwm*/
#define KPIOCS_PWM                   _IOW('X', 0x80, unsigned int*)
/*get pwm*/
#define KPIOCG_PWM                   _IOR('X', 0x81, unsigned int*)

/*set backlight*/
#define KPIOCS_LCD_BACKLIGHT         _IOW('X', 0x82, unsigned int*)
/*get backlight*/
#define KPIOCG_LCD_BACKLIGHT         _IOR('X', 0x83, unsigned int*)

/*
*******************************************************************************
* test interface
*******************************************************************************
*/
#define KPIOC_TEST_MIN               _IO ('X', 0xB0)
/*simulate key report*/
#define KPIOC_TEST_KEY               _IOW('X', 0xB0, struct input_event*)

#define KPIOC_TEST_MAX               _IO ('X', 0xBF)

/*
******************************************************************************
* voice control
******************************************************************************
*/
/*open handfree IO*/
#define KPIOCS_VOICE_HF              _IOW('X', 0x90, unsigned int*)

/*
******************************************************************************
* compatible control
******************************************************************************
*/
#define KEYPAD_EV_COMPAT_KEY_HOOK    81
#define KEYPAD_EV_COMPAT_KEY_HEADSET 83

#define KPCOMPAT_GFSKTIME            _IOR('E', 0xA0, int)            /* get FSK Time */
#define KPCOMPAT_GHARDVERSION        _IOR('E', 0xA1, int)            /* FIXME:can not be call anymore!!!! */
#define KPCOMPAT_SSKEYLED            _IOW('E', 0xA2, int)            /* set Key LED */
#define KPCOMPAT_SWATCHDOG           _IOW('E', 0xA3, int)            /* set watchdog */
#define KPCOMPAT_SIPADDRPORT         _IOW('E', 0xA4, int[3])         /* FIXME:can not be call anymore!!!!*/
#define KPCOMPAT_SCLEARWATCHDOG      _IOW('E', 0xA5, int)            /* clear watchdog*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __KERNEL__

#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#ifdef WIN32
#include <winimp/winimp.h>
#include <io.h>
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <unistd.h>
#endif

#ifdef WIN32
struct input_event {
	struct timeval time; 
	unsigned short type;
	unsigned short code;
	signed int	   value;
};

#define EV_SYN 0x00
#define EV_KEY 0x01 //按键
#define EV_REL 0x02 //相对坐标(轨迹球)
#define EV_ABS 0x03 //绝对坐标
#define EV_MSC 0x04 //其他
#define EV_SW 0x05
#define EV_LED 0x11 //LED
#define EV_SND 0x12//声音
#define EV_REP 0x14//repeat
#define EV_FF 0x15 
#define EV_PWR 0x16
#define EV_FF_STATUS 0x17
#define EV_MAX 0x1f
#define EV_CNT (EV_MAX+1)

#define ABS_X           0x00  
#define ABS_Y           0x01  
#define ABS_Z           0x02  
#define ABS_RX          0x03  
#define ABS_RY          0x04  
#define ABS_RZ          0x05  
#define ABS_THROTTLE        0x06  
#define ABS_RUDDER      0x07  
#define ABS_WHEEL       0x08  
#define ABS_GAS         0x09  
#define ABS_BRAKE       0x0a  
#define ABS_HAT0X       0x10  
#define ABS_HAT0Y       0x11  
#define ABS_HAT1X       0x12  
#define ABS_HAT1Y       0x13  
#define ABS_HAT2X       0x14  
#define ABS_HAT2Y       0x15  
#define ABS_HAT3X       0x16  
#define ABS_HAT3Y       0x17  
#define ABS_PRESSURE        0x18  
#define ABS_DISTANCE        0x19  
#define ABS_TILT_X      0x1a  
#define ABS_TILT_Y      0x1b  
#define ABS_TOOL_WIDTH      0x1c  
#define ABS_VOLUME      0x20  
#define ABS_MISC        0x28  

#define ABS_MT_TOUCH_MAJOR  0x30      
#define ABS_MT_TOUCH_MINOR  0x31      
#define ABS_MT_WIDTH_MAJOR  0x32      
#define ABS_MT_WIDTH_MINOR  0x33      
#define ABS_MT_ORIENTATION  0x34      
#define ABS_MT_POSITION_X   0x35      
#define ABS_MT_POSITION_Y   0x36      
#define ABS_MT_TOOL_TYPE    0x37      
#define ABS_MT_BLOB_ID      0x38      
#define ABS_MT_TRACKING_ID  0x39      
#define ABS_MT_PRESSURE     0x3a      

#define ABS_MAX         0x3f  
#define ABS_CNT         (ABS_MAX+1) 
#endif

/************************************************************************/
/*                          user helper API  for keypad                 */
/************************************************************************/
/*
* function: open keypad
*/
static inline int keypad_helper_open(void)
{
    return open("/dev/keypad0", O_RDWR);
}

/*
* function: release keypad
*/
static inline void keypad_helper_close(int fd)
{
    if (fd > 0)
    {
        close(fd);
    }
}

/*
* function: get hardware version from keypad
* fd:       file descriptions
* buff:     hardware version string buffer
* buff_len: string length
*/
static inline int keypad_helper_get_hwversion(int fd, char * buff, int len)
{
    if (fd > 0)
    {
        char hwver[32] ;

        if (ioctl(fd, KPIOCG_HWVERSION, (unsigned long)hwver) >= 0)
        {
            strncpy(buff, hwver, len);
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get software version from keypad
* fd:       file descriptions
* buff:     software version string buffer
* buff_len: string length
*/
static inline int keypad_helper_get_swversion(int fd, char * buff, int len)
{
    if (fd > 0)
    {
        char swver[16] ;

        if (ioctl(fd, KPIOCG_SWVERSION, (unsigned long)swver) >= 0)
        {
            strncpy(buff, swver, len);
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set encrypt
* fd:       file descriptions
* enc:      encrypt data
*/
static inline int keypad_helper_set_encrypt(int fd, unsigned int * enc)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_ENCRYPT, (unsigned long)enc) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get encrypt
* fd:       file descriptions
* enc:      encrypt data
*/
static inline int keypad_helper_get_encrypt(int fd, unsigned int * enc)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_ENCRYPT, (unsigned long)enc) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set watchdog
* fd:       file descriptions
* watchdog:
*/
static inline int keypad_helper_set_watchdog(int fd, int watchdog)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_WATCHDOG, (unsigned long)&watchdog) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get watchdog
* fd:       file descriptions
* watchdog:
*/
static inline int keypad_helper_get_watchdog(int fd, int * watchdog)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_WATCHDOG, (unsigned long)watchdog) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: close watchdog
* fd:       file descriptions
*/
static inline int keypad_helper_close_watchdog(int fd)
{
    if (fd > 0)
    {
        int watchdog = WATCHDOG_ID_CLOSE;

        if (ioctl(fd, KPIOCS_WATCHDOG, (unsigned long)&watchdog) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: sync host
*/
static inline int keypad_helper_sync(void)
{
#ifndef WIN32
    sync();
    sleep(1);

    sync();
    sleep(1);
#endif
    return 0;
}

/*
* function: reset host
* fd:       file descriptions
*/
static inline int keypad_helper_reset(int fd)
{
    if (fd > 0)
    {
        int watchdog = WATCHDOG_ID_RESET;

        keypad_helper_sync();

        if (ioctl(fd, KPIOCS_WATCHDOG, (unsigned long)&watchdog) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set led
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_set_led(int fd, led64 led)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_SETLED, (unsigned long)&led) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get led
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_get_led(int fd, led64 * led)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_GETLED, (unsigned long)led) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get led mask
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_get_led_mask(int fd, led64 * mask)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_GETLED_MASK, (unsigned long)mask) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set led flick
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_set_led_flick(int fd, led64 mask, int ms_on, int ms_off)
{
    if (fd > 0 && mask)
    {
        struct led_policy policy;
        struct led_flick  flick;

        policy.flags = 0;
        policy.mask  = mask;
        policy.on    = ms_on;
        policy.off   = ms_off;
        flick.count  = 1;
        flick.policy = &policy;

        if (ioctl(fd, KPIOCS_SETLED_FLICK, (unsigned long)&flick) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get led flick
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_get_led_flick(int fd, struct led_flick * flick)
{
    if (fd > 0 && flick)
    {
        if (ioctl(fd, KPIOCG_GETLED_FLICK, (unsigned long)flick) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: reset led flick
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_rst_led_flick(int fd)
{
    if (fd > 0)
    {
        struct led_flick  flick;

        flick.count  = 0;
        flick.policy = NULL;

        if (ioctl(fd, KPIOCS_SETLED_FLICK, (unsigned long)&flick) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: verify in driver
* fd:       file descriptions
* led:
*/
static inline int keypad_helper_verify(int fd, unsigned int code)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_VERIFY, (unsigned long)code) == 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set bklight
* fd:       file descriptions
*/
static inline int keypad_helper_set_bklight(int fd, int bklight)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_LCD_BACKLIGHT, (unsigned long)&bklight) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set bklight
* fd:       file descriptions
*/
static inline int keypad_helper_get_bklight(int fd, int * bklight)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_LCD_BACKLIGHT, (unsigned long)bklight) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set fanspeed
* fd:       file descriptions
* fanspeed:
*/
static inline int keypad_helper_set_fanspeed(int fd, int fanspeed)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_FANSPEED, (unsigned long)&fanspeed) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get fanspeed
* fd:       file descriptions
* fanspeed:
*/
static inline int keypad_helper_get_fanspeed(int fd, int * fanspeed)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_FANSPEED, (unsigned long)fanspeed) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/************************************************************************/
/*                          user helper API  for PSTN                   */
/************************************************************************/
/*
* function: get PSTN event
* fd:       file descriptions
*/
static inline int pstn_helper_get_event(int fd, int * event)
{
    if (fd > 0)
    {
        struct input_event iss_event;

        if (read(fd, &iss_event, sizeof(iss_event)) == sizeof(iss_event))
        {
            if (iss_event.value)
            {
                *event = iss_event.code;
                return 0;
            }
        }
    }
    return -EFAULT;
}

/*
* function: pstn cmd
* fd:       file descriptions
* cmd:
*/
static inline int pstn_helper_cmd(int fd, int cmd)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_PSTN_CMD, (unsigned long)&cmd) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: pstn dial
* fd:       file descriptions
* cmd:
*/
static inline int pstn_helper_dial(int fd, const char * num)
{
    if (fd > 0)
    {
        while (num && *num)
        {
            int cmd;

            if (*num >= '0' && *num <= '9')
            {
                cmd = PSTN_DIALNUM_0 + (*num - '0');
            }
            else if (*num >= 'a' && *num <= 'd')
            {
                cmd = PSTN_DIALNUM_A + (*num - 'a');
            }
            else if (*num >= 'A' && *num <= 'D')
            {
                cmd = PSTN_DIALNUM_A + (*num - 'A');
            }
            else if (*num == '*')
            {
                cmd = PSTN_DIALNUM_STAR;
            }
            else if (*num == '#')
            {
                cmd = PSTN_DIALNUM_SHARP;
            }
            else
            {
                return -1;
            }

            if (ioctl(fd, KPIOCS_PSTN_CMD, (unsigned long)&cmd) < 0)
            {
                return -1;
            }

            num++;
        }

        return 0;
    }
    return -EFAULT;
}

/*
* function: pstn dial
* fd:       file descriptions
* cmd:
*/
static inline int pstn_helper_fsk(int fd, struct pstn_fsk * fsk)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_PSTN_FSK, (unsigned long)fsk) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: test key
* fd:       file descriptions
*/
static inline int keypad_helper_test_key(int fd, struct input_event * event)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOC_TEST_KEY, (unsigned long)event) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: set voice hf
* fd:       file descriptions
*/
static inline int keypad_helper_set_voice_hf(int fd, int voice_hf)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCS_VOICE_HF, (unsigned long)&voice_hf) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get headset status
* fd:       file descriptions
*/
static inline int keypad_helper_get_hsdsta(int fd, int * hsdsta)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_HEADSET_STA, (unsigned long)hsdsta) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}

/*
* function: get micbias status
* fd:       file descriptions
*/
static inline int keypad_helper_get_mbssta(int fd, int * mbssta)
{
    if (fd > 0)
    {
        if (ioctl(fd, KPIOCG_MICBIAS_STA, (unsigned long)mbssta) >= 0)
        {
            return 0;
        }
    }
    return -EFAULT;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__KERNEL__*/
#endif /*__KEYPAD_DRV_API_H__*/
