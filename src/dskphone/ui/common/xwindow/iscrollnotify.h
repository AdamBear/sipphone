#ifndef __I_SCROLL_NOTIFY_H__
#define __I_SCROLL_NOTIFY_H__

#include <xwindow/xlistviewdefine.h>

#if !IF_XLISTVIEW_SUPPORT_SLIDE
class IScrollNotify
{
public:
    IScrollNotify() {};
    virtual ~IScrollNotify() {};

public:
    // 通知向前滚动
    virtual bool OnNotifyScrollPrevious(bool bCycle = true)     = 0;
    // 通知向后滚动
    virtual bool OnNotifyScrollNext(bool bCycle = true)         = 0;
};
#endif

#endif
