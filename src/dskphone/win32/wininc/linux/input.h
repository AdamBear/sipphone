#pragma once

struct input_event
{
    struct timeval time;
    unsigned short type;
    unsigned short code;
    signed int     value;
};

#define __s32 unsigned long

struct input_absinfo
{
    __s32 value;         // current value of the axis
    __s32 minimum;       // current limits of the axis
    __s32 maximum;       // current limits of the axis
    __s32 fuzz;
    __s32 flat;
};

#define EVIOCGVERSION        _IOR('E', 0x01, int)            /* get driver version */
#define EVIOCGID        _IOR('E', 0x02, struct input_id)    /* get device ID */
#define EVIOCGREP        _IOR('E', 0x03, int[2])            /* get repeat settings */
#define EVIOCSREP        _IOW('E', 0x03, int[2])            /* set repeat settings */
#define EVIOCGKEYCODE        _IOR('E', 0x04, int[2])            /* get keycode */
#define EVIOCSKEYCODE        _IOW('E', 0x04, int[2])            /* set keycode */

#define EVIOCGNAME(len)        _IOC(_IOC_READ, 'E', 0x06, len)        /* get device name */
#define EVIOCGPHYS(len)        _IOC(_IOC_READ, 'E', 0x07, len)        /* get physical location */
#define EVIOCGUNIQ(len)        _IOC(_IOC_READ, 'E', 0x08, len)        /* get unique identifier */

#define EVIOCGKEY(len)        _IOC(_IOC_READ, 'E', 0x18, len)        /* get global keystate */
#define EVIOCGLED(len)        _IOC(_IOC_READ, 'E', 0x19, len)        /* get all LEDs */
#define EVIOCGSND(len)        _IOC(_IOC_READ, 'E', 0x1a, len)        /* get all sounds status */
#define EVIOCGSW(len)        _IOC(_IOC_READ, 'E', 0x1b, len)        /* get all switch states */

#define EVIOCGBIT(ev,len)    _IOC(_IOC_READ, 'E', 0x20 + ev, len)    /* get event bits */
#define EVIOCGABS(abs)        _IOR('E', 0x40 + abs, struct input_absinfo)        /* get abs value/limits */
#define EVIOCSABS(abs)        _IOW('E', 0xc0 + abs, struct input_absinfo)        /* set abs value/limits */

#define EVIOCSFF        _IOC(_IOC_WRITE, 'E', 0x80, sizeof(struct ff_effect))    /* send a force effect to a force feedback device */
#define EVIOCRMFF        _IOW('E', 0x81, int)            /* Erase a force effect */
#define EVIOCGEFFECTS        _IOR('E', 0x84, int)            /* Report number of effects playable at the same time */

#define EVIOCGRAB        _IOW('E', 0x90, int)            /* Grab/Release device */

#define EV_SYN 0x00
#define EV_KEY 0x01
#define EV_REL 0x02
#define EV_ABS 0x03
#define EV_MSC 0x04
#define EV_SW 0x05
#define EV_LED 0x11
#define EV_SND 0x12
#define EV_REP 0x14
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
