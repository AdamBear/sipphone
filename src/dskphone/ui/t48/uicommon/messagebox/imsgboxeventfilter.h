#ifndef __I_MSGBOX_EVENT_FILTER_H__
#define __I_MSGBOX_EVENT_FILTER_H__

class IMsgBoxEventFilter
{
public:
    IMsgBoxEventFilter() {};
    virtual ~IMsgBoxEventFilter() {};

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress, bool & bFilter)      = 0;
};
#endif
