#ifndef __FRAME_LIST_DEFINE__
#define __FRAME_LIST_DEFINE__

// 是否支持QT
#define GUI_LIB_QT

#ifdef GUI_LIB_QT
// 是否支持触屏
#define QT_TOUCH_SUPPORT
// 是否支持按键
//#define QT_KEYBOARD_SUPPORT
#endif

// 是否支持dotWindows
// #define GUI_LIB_DW

// 是否支持翻页
#define FL_SUPPORT_SCROLL_PAGE

// 是否支持滚动条
//#define FL_SUPPORT_SCROLL_SLIDE

// 是否支持标题
#define FL_SUPPORT_TITLE

// 是否支持FrameList自定义风格
#define FL_SUPPORT_CUSTOM_FRAME_STYLE

// 是否支持ListItem自定义风格
//#define FL_SUPPORT_CUSTOM_ITEM_STYLE

#ifdef GUI_LIB_QT
#include <QWidget>
typedef QWidget CWidgetBase;
#endif

#ifdef GUI_LIB_DW
#include "dotWidget.h"
typedef dotWidget CWidgetBase;
#define Q_OBJECT
#endif

#define INVALID_FRAME_LIST_PAGE_NO -1
#define INVALID_ITEM_INDEX -1

#endif
