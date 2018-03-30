#ifndef __XVIEW_EVENT_H__
#define __XVIEW_EVENT_H__

#include "xwindowcommon.h"
#include "xGraphic.h"
#if IF_MULTI_TOUCH_ENABLE
#include "touchdriver/include/touchdrivercommon.h"
#endif


enum ViewEventE
{
    VIEW_EVENT_FIRST        = xKernel::XEVENT_TYPE_CUSTOM + 1,
    VIEW_EVENT_NONAME,
    VIEW_KEY_PRESS,
    VIEW_KEY_RELEASE,
    VIEW_KEY_LONG_PRESS,
    VIEW_MOUSE_MOVE,
    VIEW_MOUSE_ENTER,
    VIEW_MOUSE_LEAVE,
    VIEW_MOUSE_WHEEL,
    VIEW_MOUSE_KEY_PRESS,
    VIEW_MOUSE_KEY_RELEASE,

    VIEW_FOCUS_CUSTOM,
    VIEW_FOCUS_ENTER,
    VIEW_FOCUS_LEAVE,
    VIEW_FOCUS_MOVE,

    VIEW_ACTIVE,
    VIEW_PAINT_BACKGROUND,
    VIEW_PAINT_CONTENT,
#if IF_VIEW_MASK
    VIEW_PAINT_MASK,
#endif

#if IF_MULTI_TOUCH_ENABLE
    VIEW_TOUCH_BEGAN,
    VIEW_TOUCH_MOVED,
    VIEW_TOUCH_ENDED,
    //窗口隐藏或销毁时发送取消
    VIEW_TOUCH_CANCEL,
#endif
#if IF_GESTURE_ENABLE
    VIEW_GESTURE,
#endif
};

class XWINDOW_EXPORT xViewEvent
{
public:
    inline bool isAccepted() const
    {
        return m_accept;
    }

    inline void accept()
    {
        m_accept = true;
    }
    inline void ignore()
    {
        m_accept = false;
    }
public:
    explicit xViewEvent(int _type = VIEW_EVENT_NONAME) : m_accept(false), type(_type)  {}
public:
    UINT type;
private:
    bool m_accept;
};

class XWINDOW_EXPORT xViewEvent_bool : public xViewEvent
{
public:
    xViewEvent_bool(int type, bool b) : xViewEvent(type), value_bool(b) {}
public:
    bool value_bool;
};

class XWINDOW_EXPORT xViewEvent_int : public xViewEvent
{
public:
    xViewEvent_int(int type, int value) : xViewEvent(type), value_int(value) {}

public:
    int value_int;
};

class XWINDOW_EXPORT xViewEvent_paint : public xViewEvent
{
public:
    xViewEvent_paint(int type, xPainter& painter) : xViewEvent(type), value_painter(painter) {}
public:
    xPainter& value_painter;
};

class XWINDOW_EXPORT xViewEvent_key : public xViewEvent
{
public:
    xViewEvent_key(int type, const int& key, bool pressed) : xViewEvent(type), value_key(key), value_pressed(pressed) {}
public:
    int             value_key;
    bool            value_pressed;
};

class xView;
class XWINDOW_EXPORT xViewEvent_FocusMove : public xViewEvent
{
public:
    xViewEvent_FocusMove(int nType, xView* pNewFocus, xView* pOldFocus)
        : xViewEvent(nType)
        , m_pNewFocus(pNewFocus)
        , m_pOldFocus(pOldFocus)
    {
    }

public:
    xView*          m_pNewFocus;
    xView*          m_pOldFocus;
};

#if IF_MULTI_TOUCH_ENABLE
class XWINDOW_EXPORT xTouchEvent : public xViewEvent
{
public:
    xTouchEvent(int nType, xView* pAnchorView)
        : xViewEvent(nType)
        , m_pAnchorView(pAnchorView)
    {
    }

    ~xTouchEvent()
    {
        m_listPoint.clear();
    }

    bool AddTouchPoint(ST_TOUCH_POINT* pTouchPoint)
    {
        if (NULL == pTouchPoint)
        {
            return false;
        }

        m_listPoint.push_back(pTouchPoint);

        return true;
    }

public:
    xView*                  m_pAnchorView;
    LIST_RAW_TOUCH_POINT    m_listPoint;
};
#endif

#endif
