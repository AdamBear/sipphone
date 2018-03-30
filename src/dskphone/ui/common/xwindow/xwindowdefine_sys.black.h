#ifndef __X_WINDOW_DEFINE_SYS_H__
#define __X_WINDOW_DEFINE_SYS_H__

////////////////////////////////////////////////////////////////////////////
// 系统相关
////////////////////////////////////////////////////////////////////////////
// 是否支持按键按下后，按一定频率生效
#define IF_KEY_REPEAT 0
// 是否支持鼠标
#define IF_MOUSE_ENABLE 0
// 是否支持触屏
#define IF_MULTI_TOUCH_ENABLE 0
// 是否支持手势
#define IF_GESTURE_ENABLE 0
// 是否支持PC
#define IF_PC_ENABLE 0
// 是否支持按键切换焦点
#define IF_MOVE_FOCUS_BY_KEY 1

// 是否支持宏定义布局文件
#define IF_SUPPORT_MACRO_LAYOUT_FILE 0
// 是否同步使用重新布局
#define IF_XWINDOW_SYNC_RELAYOUT    0
// 是否支持RTTI的类型检查
#define IF_XWINDOW_RTTI_CHECK 0
// 是否支持窗口对象缓存
#define IF_XWINDOW_CACHE_WINDOW_OBJECT 1
// 窗口对象缓存数量
#define XWINDOW_OBJECT_CACHE_MAX_COUNT 10

// 是否支持全局透明度
#define IF_XWINDOW_STATION_ALPHA 0

#endif
