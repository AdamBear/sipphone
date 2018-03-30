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

    // �����¼�
    virtual bool SendEvent(EventDataBase* pData);
    // ����ͨ���¼�
    bool SendTalkEvent(EventDataBase* pData);
    // ���ͻ����¼�
    bool SendBaseEvent(EventDataBase* pData);

private:
    CTalkInfoReportProcesser();
    virtual ~CTalkInfoReportProcesser();
};
#endif

#define g_objTalkInfoReportManager (CTalkInfoReportProcesser::GetInstance())

#endif // !__TALK_INFO_REPORT_H__


