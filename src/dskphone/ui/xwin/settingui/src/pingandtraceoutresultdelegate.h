#ifndef _PING_AND_TRACEOUT_RESULT_DELEGATE_H_
#define _PING_AND_TRACEOUT_RESULT_DELEGATE_H_

#include "settinguiscrollbypagedelegatebase.h"

class CPingAndTraceOutResultDelegate : public CSettinguiScrollByPageDelegateBase
{
public:
    CPingAndTraceOutResultDelegate();
    virtual ~CPingAndTraceOutResultDelegate();
    static CSettingUIDelegateBase * CreatResultDelegate();
public:
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
protected:
    void GetShowText(const yl::string & strText, VEC_TEXT & vecText);
};

#endif //_PING_AND_TRACEOUT_RESULT_DELEGATE_H_
