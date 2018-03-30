#ifndef __XWINDOW_COMMON_H__
#define __XWINDOW_COMMON_H__

#include <ETLLib/ETLLib.hpp>
#include <ylhashmap.h>
#include <yllist.h>
#include <ylstring.h>
#include <ylvector.h>
#include <xwindow/xlistviewdefine.h>
#include <xwindow/xwindowdefine.h>
#include "xwindow-kernel/include/xkernelcommon.h"
#include "dsklog/log.h"

typedef YLHashMap<yl::string, yl::string> MAP_STRING;
typedef YLList<yl::string> LIST_STRING;
typedef YLVector<yl::string> VEC_STRING;

extern const char* XWINDOW_ENUM_TRUE;
extern const char* XWINDOW_ENUM_FALSE;
extern const char* XWINDOW_ENUM_AUTO;
extern const char* XWINDOW_ENUM_WRAP_CONTENT;
extern const char* XWINDOW_ENUM_MATCH_PARENT;
extern const char* XWINDOW_ENUM_FILL_PARENT;
extern const char* XWINDOW_ENUM_TOP;
extern const char* XWINDOW_ENUM_BOTTOM;
extern const char* XWINDOW_ENUM_LEFT;
extern const char* XWINDOW_ENUM_RIGHT;
extern const char* XWINDOW_ENUM_CENTER_VERTICAL;
extern const char* XWINDOW_ENUM_CENTER_HORIZONTAL;
extern const char* XWINDOW_ENUM_CENTER;
extern const char* XWINDOW_ENUM_VISIBLE;
extern const char* XWINDOW_ENUM_INVISIBLE;
extern const char* XWINDOW_ENUM_GONE;
extern const char* XWINDOW_ENUM_VERTICAL;
extern const char* XWINDOW_ENUM_HORIZONTAL;
extern const char* XWINDOW_ENUM_LEFT_TOP;
extern const char* XWINDOW_ENUM_RIGHT_TOP;
extern const char* XWINDOW_ENUM_LEFT_BOTTOM;
extern const char* XWINDOW_ENUM_RIGHT_BOTTOM;
extern const char* XWINDOW_ENUM_ALL;

extern const char* XWINDOW_ENUM_DIRECT_CUT;
extern const char* XWINDOW_ENUM_CUT_WITH_ELLIPSIS;
extern const char* XWINDOW_ENUM_SCROLL;
extern const char* XWINDOW_ENUM_AUTO_RETURN;
extern const char* XWINDOW_ENUM_VSCROLL;

extern const char* XWINDOW_NAME_LAYOUT_MARGIN;
extern const char* XWINDOW_NAME_LAYOUT_MARGIN_LEFT;
extern const char* XWINDOW_NAME_LAYOUT_MARGIN_RIGHT;
extern const char* XWINDOW_NAME_LAYOUT_MARGIN_TOP;
extern const char* XWINDOW_NAME_LAYOUT_MARGIN_BOTTOM;
extern const char* XWINDOW_NAME_LAYOUT_HEIGHT;
extern const char* XWINDOW_NAME_LAYOUT_WIDTH;
extern const char* XWINDOW_NAME_PADDING;
extern const char* XWINDOW_NAME_PADDING_LEFT;
extern const char* XWINDOW_NAME_PADDING_RIGHT;
extern const char* XWINDOW_NAME_PADDING_TOP;
extern const char* XWINDOW_NAME_PADDING_BOTTOM;
extern const char* XWINDOW_NAME_ID;
extern const char* XWINDOW_NAME_BACKGROUND;
extern const char* XWINDOW_NAME_STYLE;
extern const char* XWINDOW_NAME_FOCUSABLE;
extern const char* XWINDOW_NAME_FOCUS_PRESENT;
extern const char* XWINDOW_NAME_CLICKABLE;
extern const char* XWINDOW_NAME_FOCUS_ON_CLICK;
extern const char* XWINDOW_NAME_OVERLAP;
extern const char* XWINDOW_NAME_DISABLE;
#if IF_MULTI_TOUCH_ENABLE
extern const char* XWINDOW_NAME_TOUCHABLE;
#endif
#if IF_VIEW_MASK
extern const char* XWINDOW_NAME_MASK;
#endif
extern const char* XWINDOW_NAME_VISIBLITY;
#if IF_MOVE_FOCUS_BY_KEY
extern const char* XWINDOW_NAME_NEXT_FOCUS_UP;
extern const char* XWINDOW_NAME_NEXT_FOCUS_DOWN;
extern const char* XWINDOW_NAME_NEXT_FOCUS_LEFT;
extern const char* XWINDOW_NAME_NEXT_FOCUS_RIGHT;
#if IF_PC_ENABLE
extern const char* XWINDOW_NAME_NEXT_FOCUS_PREVIOUS;
extern const char* XWINDOW_NAME_NEXT_FOUCS_NEXT;
#endif
#endif
#if IF_VIEW_USER_ACTION
extern const char* XWINDOW_NAME_ON_CLICK;
#endif
#if IF_VIEW_RENDER
extern const char* XWINDOW_NAME_RENDERER;
#endif
extern const char* XWINDOW_NAME_SRC;
extern const char* XWINDOW_NAME_INVERT;
#if IF_IMAGE_VIEW_BORDER
extern const char* XWINDOW_NAME_BORDER_WIDTH;
extern const char* XWINDOW_NAME_BORDER_COLOR;
#endif
extern const char* XWINDOW_NAME_GRAVITY;
extern const char* XWINDOW_NAME_ALIGN;
extern const char* XWINDOW_NAME_TEXT;
extern const char* XWINDOW_NAME_TEXT_COLOR;
extern const char* XWINDOW_NAME_TEXT_SIZE;
extern const char* XWINDOW_NAME_TEXT_BOLD;
extern const char* XWINDOW_NAME_MULTILINE;
#if IF_BUTTON_SUPPORT_DISABLE_STATE
extern const char* XWINDOW_NAME_COLOR_DISABLE;
extern const char* XWINDOW_NAME_PIXMAP_DISABLE;
#endif
#if IF_BUTTON_SUPPORT_FOCUS_STATE
extern const char* XWINDOW_NAME_COLOR_FOCUS;
extern const char* XWINDOW_NAME_PIXMAP_FOCUS;
#endif
#if IF_BUTTON_SUPPORT_PRESS_STATE
extern const char* XWINDOW_NAME_COLOR_PRESS;
extern const char* XWINDOW_NAME_PIXMAP_PRESS;
#endif
extern const char* XWINDOW_NAME_ITEM_STYLE;
extern const char* XWINDOW_NAME_FOCUS_ITEM_BG;
extern const char* XWINDOW_NAME_PAGE;
extern const char* XWINDOW_NAME_BG;
extern const char* XWINDOW_NAME_BLOCK;
extern const char* XWINDOW_NAME_RANGE;
extern const char* XWINDOW_NAME_BAR;
extern const char* XWINDOW_NAME_SLIDE_ICON;
extern const char* XWINDOW_NAME_RC_ICON;
extern const char* XWINDOW_NAME_ON;
extern const char* XWINDOW_NAME_OFF;
extern const char* XWINDOW_NAME_RC_TEXT;
extern const char* XWINDOW_NAME_LAYOUT_GRAVITY;
extern const char* XWINDOW_NAME_LAYOUT_WEIGHT;
extern const char* XWINDOW_NAME_ORIENTATION;
extern const char* XWINDOW_NAME_CONTENT;
extern const char* XWINDOW_NAME_SCROLL_BAR;
extern const char* XWINDOW_NAME_THUMB;
extern const char* XWINDOW_NAME_SCALE_LTWH;
extern const char* XWINDOW_NAME_SCALE_HEIGHT;
extern const char* XWINDOW_NAME_SCALE_WIDTH;
extern const char* XWINDOW_NAME_RECT_CORNER_SIZE;
extern const char* XWINDOW_NAME_RECT_COLOR;
extern const char* XWINDOW_NAME_RECT_CORNER_TYPE;
extern const char* XWINDOW_NAME_ACTIVABLE;
extern const char* XWINDOW_NAME_LOGIC_LTWH;
extern const char* XWINDOW_NAME_LAYOUT;
extern const char* XWINDOW_NAME_COLUMN;
extern const char* XWINDOW_NAME_COLUMN_SPACE;
extern const char* XWINDOW_NAME_BOUND_LINE_TYPE;
extern const char* XWINDOW_NAME_LIST_STYLE;
#if IF_XLISTVIEW_SUPPORT_SLIDE
extern const char* XWINDOW_NAME_SLIDABLE;
#endif
extern const char* XWINDOW_NAME_SCROLL_TYPE;
extern const char* XWINDOW_NAME_FOCUS_TYPE;
extern const char* XWINDOW_NAME_PAGE_ITEM_COUNT;
extern const char* XWINDOW_NAME_CONTENT_HEIGHT;
extern const char* XWINDOW_NAME_PAGE_HEIGHT;
extern const char* XWINDOW_NAME_IM_TOP;
extern const char* XWINDOW_NAME_IM_LEFT;
extern const char* XWINDOW_NAME_IM_RIGHT;
extern const char* XWINDOW_NAME_IM_BOTTOM;
extern const char* XWINDOW_NAME_ITEM_WIDTH;
extern const char* XWINDOW_NAME_ITEM_HEIGHT;
extern const char* XWINDOW_NAME_ITEM_SPACE;
extern const char* XWINDOW_NAME_SCM_TOP;
extern const char* XWINDOW_NAME_SCM_BOTTOM;
extern const char* XWINDOW_NAME_SCM_RIGHT;
extern const char* XWINDOW_NAME_FIX_POS;
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
extern const char* XWINDOW_NAME_SPACE_LINE_COLOR;
extern const char* XWINDOW_NAME_SPACE_LINE_STYLE;
extern const char* XWINDOW_NAME_SPACE_LINE_IGNORE_END;
#endif
#if IF_XLISTVIEW_SUPPORT_GROUP
extern const char* XWINDOW_NAME_ITEMS_STYLE;
extern const char* XWINDOW_NAME_SECTION_COUNT;
extern const char* XWINDOW_NAME_SECTION_STYLE;
extern const char* XWINDOW_NAME_SECTION_TITLE_BG;
extern const char* XWINDOW_NAME_SECTION_TEXT_COLOR;
extern const char* XWINDOW_NAME_SECTION_TEXT_SIZE;
extern const char* XWINDOW_NAME_SECTION_TEXT_BOLD;
extern const char* XWINDOW_NAME_SECTION_TEXT_ALIGN;
extern const char* XWINDOW_NAME_SECTION_TEXT_MARGIN;
#endif
#if IF_XLISTVIEW_SUPPORT_TITLE
extern const char* XWINDOW_NAME_TITLE_SPACE;
extern const char* XWINDOW_NAME_TITLE_LINE_TYPE;
extern const char* XWINDOW_NAME_TITLE_LINE_POS_TYPE;
#endif
extern const char* XWINDOW_NAME_ITEM;
extern const char* XWINDOW_NAME_MAX_LENGTH;
extern const char* XWINDOW_NAME_SHOW_TIME;
extern const char* XWINDOW_NAME_HIDE_TIME;
extern const char* XWINDOW_NAME_INTERVAL;
extern const char* XWINDOW_NAME_PICS;
extern const char* XWINDOW_NAME_TEXT_EDIT_BG;
extern const char* XWINDOW_NAME_PREFERENCE;
extern const char* XWINDOW_NAME_VALUE;
extern const char* XWINDOW_NAME_THUMB_NORMAL;
extern const char* XWINDOW_NAME_THUMB_SELECTED;
extern const char* XWINDOW_NAME_MIN;
extern const char* XWINDOW_NAME_MAX;
extern const char* XWINDOW_NAME_SINGLE_STEP;
extern const char* XWINDOW_NAME_PAGE_STEP;
extern const char* XWINDOW_NAME_POPUP_GRAVITY;
extern const char* XWINDOW_NAME_MAX_LENGTH;
extern const char* XWINDOW_NAME_NORMAL;
extern const char* XWINDOW_NAME_HOVER;
extern const char* XWINDOW_NAME_PRESSED;
extern const char* XWINDOW_NAME_HIGH_LIGHT;
extern const char* XWINDOW_NAME_COUNT;
extern const char* XWINDOW_NAME_FLUSH_TIME;
extern const char* XWINDOW_NAME_IMAGE_NAME;
extern const char* XWINDOW_NAME_DISABLED;
extern const char* XWINDOW_NAME_VISIBLE;
extern const char* XWINDOW_NAME_FILE;
extern const char* XWINDOW_NAME_SIZE;
extern const char* XWINDOW_NAME_COLOR;
extern const char* XWINDOW_NAME_FAMILY;
extern const char* XWINDOW_NAME_PRESENT;
extern const char* XWINDOW_NAME_TITLE;
extern const char* XWINDOW_NAME_ON_LONG_CLICK;
extern const char* XWINDOW_NAME_WRAPLIMITED;

extern const char* XWINDOW_NAME_OVER_LENGTH_STYLE;
extern const char* XWINDOW_NAME_ACTIVE_FONTS;
extern const char* XWINDOW_NAME_SCROLL_DIRECTION;
extern const char* XWINDOW_NAME_SCROLL_TICK;
extern const char* XWINDOW_NAME_SCROLL_SHIFT;
extern const char* XWINDOW_NAME_ROUND_DURATION;
extern const char* XWINDOW_NAME_LINE_DURATION;

extern const char* XWINDOW_TAG_NAME_VIEW_STUB;
extern const char* XWINDOW_TAG_NAME_VIEW;
extern const char* XWINDOW_TAG_NAME_DATA;
extern const char* XWINDOW_TAG_NAME_GROUP;

class xTextTranslate
{
public:
    xTextTranslate();
    virtual ~xTextTranslate();

public:
    virtual void SetTranText(const yl::string& strText)             = 0;
};

#if IF_XWINDOW_DEBUG
void XWIN_TRACE_TIME(const char* lpszFmt, ...);
#endif

BEGIN_XWINDOW_NAMESPACE
//是否为空白字符，包括空格、制表符、回车换行
bool IsWhiteSpace(int ch);

//跳过空白字符，返回第一个正常字符的字符串指针
LPCSTR SkipWhiteSpace(LPCSTR lpszText);

//根据lpszToken将lpszText截取，将所截取到的第一段字符串赋值给strText
int PeekPhraseByCharTokens(LPCSTR& lpszText, yl::string& strText, LPCSTR lpszToken);

LPCSTR PeekContentFromQuotaString(LPCSTR lpszParamText, yl::string& strContent);
END_XWINDOW_NAMESPACE

void xWindowUI_Init();

#endif
