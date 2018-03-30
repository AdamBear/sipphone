#ifndef __TALK_INFO_REPORT_H__
#define __TALK_INFO_REPORT_H__
#if IF_FEATURE_TALKINFO_REPORT
#include "extended/event/src/eventbase.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"

class CTalkInfoReportProcesser : public CEventBase
{
public:
    static CTalkInfoReportProcesser& GetInstance();

    // 发送事件
    virtual bool SendEvent(EventDataBase* pData);
    // 发送通话事件
    bool SendTalkEvent(EventDataBase* pData);
    // 发送基础事件
    bool SendBaseEvent(EventDataBase* pData);

private:
    CTalkInfoReportProcesser();
    virtual ~CTalkInfoReportProcesser();
};
#endif

#define g_objTalkInfoReportManager (CTalkInfoReportProcesser::GetInstance())

#endif // !__TALK_INFO_REPORT_H__


