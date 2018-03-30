
#ifndef __I_SCROLL_NOTIFY_H__
#define __I_SCROLL_NOTIFY_H__

class IScrollNotify
{
public:
    IScrollNotify() {};
    virtual ~IScrollNotify() {};

public:
    // 通知向前滚动
    virtual void OnNotifyScrollPrevious()       = 0;
    // 通知向后滚动
    virtual void OnNotifyScrollNext()           = 0;
};

#endif
