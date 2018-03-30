#ifndef _USB_UI_COMMON_H
#define _USB_UI_COMMON_H

#include "yllist.h"
#include "ylstring.h"

#define USB_FILE_UNAVAILABEL_TIME               ""//"--:--:--"

#define USB_CAPACITY_KB         1024
#define USB_CAPACITY_MB         1048576
#define USB_CAPACITY_GB         1073741824
#define USB_CAPACITY_TB         1099511627776

#define USB_TIME_RETURN_TO_IDLE_TIME        60000

#define USB_STRING_BUFFER_SIZE      64

#define USB_F_ZERO              0.00000000000
#define USB_F_FAST_SLIP     1000.0
#define USB_F_SINGPOINT     15

#define USB_PROCESSBAR_OFFSET   150
#define USB_PROCESSBAR_TOTAL    100

//单位是毫秒
#define USB_SWIFT_LONG_PRESS    1000
#define USB_SWIFT_INTERVAL      200

#define USB_LOGIC_READING_TIME      10
#define USB_LOGIC_READING_TIME_MIN      700
#define USB_MESSAGEBOX_NOTE_HINT_TIME       3000

//单位是秒
#define USB_SWIFT_SECONDS           8
#define USB_NEAR_COMPLETE_SECONDS   2

#define USB_COLOR_BLACK     QColor(0, 0, 0)
#define USB_COLOR_RED       QColor(255, 0, 0)
#define USB_COLOR_GRAY      QColor(137, 129, 137)
#define USB_COLOR_WHITE     QColor(255, 255, 255)
#define USB_COLOR_GREEN     QColor(53, 174, 139)

enum USB_PLAY_STATE
{
    USB_PLAY_STATE_PLAY,
    USB_PLAY_STATE_PAUSE,
    USB_PLAY_STATE_STOP,

    //该状态用来标示一个AudioItem在播放时其他Item的状态
    //即要显示播放图标但不要“删除”图标的状态
    USB_PLAY_STATE_WAIT,
};

enum USB_CLICK_ACTION
{
    //USB_PROCESSBAR_OFFSET到USB_PROCESSBAR_OFFSET + USB_PROCESSBAR_TOTAL的数用表示于进度条拖动

    //在进度条上拖动
    USB_CLICK_ACTION_MOVE       = 59,

    //从60开始表示点击事件
    USB_CLICK_ACTION_PLAY           = 60,
    USB_CLICK_ACTION_PAUSE,
    USB_CLICK_ACTION_RESUME,
    USB_CLICK_ACTION_STOP,
    USB_CLICK_ACTION_DELETE,
    USB_CLICK_ACTION_SEEK,
    USB_CLICK_ACTION_COMPLETE,
};

enum USB_CLICK_BUTTON_SIDE
{
    USB_CLICK_BUTTON_SIDE_LEFT,
    USB_CLICK_BUTTON_SIDE_RIGHT,
    USB_CLICK_BUTTON_SIDE_SLIDEBAR,
};

/////////////////////////////////////////
// 操作设备的类型
enum USB_OPERATION_TYPE
{
    OPERATION_START = 0,    // 开始
    OPERATION_PAUSE,        // 暂停
    OPERATION_RESUME,       // 恢复
    OPERATION_STOP,         // 停止
    OPERATION_POSITION      // 定位
};

//////////////////////////////////

#endif // _USB_UI_COMMON_H
