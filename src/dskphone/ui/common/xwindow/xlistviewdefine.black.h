#ifndef __X_LIST_VIEW_DEFINE_H__
#define __X_LIST_VIEW_DEFINE_H__

#include <xwindow/xwindowdefine.h>

// 是否需要Item的焦点切换
#define IF_XLISTVIEW_SWITCH_ITEM_FOCUS 1
// xListView是否按逐页来显示Item
#define IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE 0
// 滚动条是否表示位置范围(正常在滑动场景下使用)
#define IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION 0
// 滚动条是否表示Item数量(黑白屏)
#define IF_XLISTVIEW_SCROLL_BAR_DENOTE_ITEM_COUNT 1

// 是否每个Item都可以部份显示
#define IF_XLISTVIEW_SHOW_ITEM_PARTLY 0
// 是否支持滑动
#define IF_XLISTVIEW_SUPPORT_SLIDE 0
// 是否支持滑动动画
#define IF_XLISTVIEW_SUPPORT_SLIDE_ANIMATION 0

// 是否支持不定高度Item项
#define IF_XLISTVIEW_DISUNION_ITEM_HEIGHT 1

// 是否外部控制焦点切换
#define IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS 1

// 是否支持NOTE与Item一样显示
#define IF_XLISTVIEW_NOTE_SHOW_IN_ITEM 1

// 是否支持自定义样式
#define IF_XLISTVIEW_SUPPORT_CUSTOM_STYLE 0

// 是否支持字母索引
#define IF_XLISTVIEW_SUPPORT_INDICATOR 0

// 是否支持Title
#define IF_XLISTVIEW_SUPPORT_TITLE 0

// 是否用滚动条
#define IF_XLISTVIEW_SCROLL_BY_SCROLL_BAR 1
// 是否用翻页控件
#define IF_XLISTVIEW_SCROLL_BY_PAGE_CTRL 0

// 是否支持反色焦点效果
#define IF_XLISTVIEW_FOCUS_INVERT_EFFECT 1
// 是否支持在xListView中画焦点背景
#define IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER 0

// 是否支持分隔线
#define IF_XLISTVIEW_ITEM_SPACE_BY_LINE 0

// 是否支持不等宽
#define IF_XLISTVIEW_ITEM_CUSTOM_WIDTH 0

// 是否支持Group功能
#define IF_XLISTVIEW_SUPPORT_GROUP 0

// 是否支持Item动态扩展功能
#define IF_XLISTVIEW_SUPPORT_ITEM_FLATE 0

// 是否支持自定义显不显示焦点效果，何时上焦点(itemfocus、focus、focused)
#define IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG 1
#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
// 有ItemFocus就显示焦点效果，不用具备真正的焦点
#define IF_XLISTVIEW_ITEM_DEFAULT_BG_BY_ITEM_FOCUS 1
#endif

#define XLIST_VIEW_DEBUG 0
#if XLIST_VIEW_DEBUG
#define IF_XLIST_VIEW_SLIDE_DEBUG 0
#endif

#define INVALID_FRAME_LIST_PAGE_NO -1
#define INVALID_ITEM_INDEX -1

#endif
