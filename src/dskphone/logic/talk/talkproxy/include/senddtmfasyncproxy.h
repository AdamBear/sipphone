#pragma once

#include "talk/talkproxy/include/basetalkproxy.h"
#include "yllist.h"
#include "ylstring.h"

// 封装通话异步处理事件
class CSendDTMFASyncProxy
    : public CBaseTalkASyncProxy
{
protected:

    struct CDTMFDetails
    {
        int             interval;
        yl::string      strValue;

        CDTMFDetails(int i = 0, const yl::string& s = yl::string())
            : interval(i), strValue(s)
        {}
    };

    typedef YLList<CDTMFDetails>  LIST_DTMF;

public:
    explicit CSendDTMFASyncProxy(int iSessionID) : CBaseTalkASyncProxy(iSessionID)
    {}

    /* 以间隔时间逐个发送 */
    bool Send(const yl::string& str, int interval = 0);

private:
    virtual bool OnCallBack();

    bool SendDTMF(char ch, int interval);

    bool SendDTMF(int nKey);

    bool SendOnce();

    static int GetDtmfKey(int iDtmf);

private:
    LIST_DTMF  m_listDTMF;
};
