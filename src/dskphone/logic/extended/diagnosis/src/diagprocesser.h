#ifndef __DIAG_PROCESSER_H__
#define __DIAG_PROCESSER_H__

#ifdef IF_FEATURE_DIAGNOSE

#include "extended/event/src/eventbase.h"
#include <service_dsk_define.h>
#include "ylhashmap.h"

// 是否包含记录用户数据
//#define IF_FEATURE_USER_DATA

#ifdef IF_FEATURE_USER_DATA
typedef YLHashMap<int, EventStatsDataBase * > MapDiagInfo;

typedef MapDiagInfo::iterator        IterMapDiagInfo;
#endif //#ifdef IF_FEATURE_USER_DATA

class EventInfo;

class CDiagProcesser : public CEventBase
{
public:
    static CDiagProcesser & GetInstace();

    // 初始化
    void Init();

    // 发送诊断事件
    virtual bool SendEvent(EventDataBase * pData);

#ifdef IF_FEATURE_USER_DATA
    bool OnTime(UINT uTimerID);

    bool SaveEvent(EventDataBase * pData);

    bool SaveStaticData();

    void AppendSysData(xml_node & node);

    EventStatsDataBase * CreateStatsData(EventType eType);
#endif // IF_FEATURE_USER_DATA

private:
    CDiagProcesser();

    virtual ~CDiagProcesser();

	UINT MapEvent2Msg(int eEvent);
private:

#ifdef IF_FEATURE_USER_DATA

    // 设备信息
    UINT m_nMinMem;     // 最小内存

    MapDiagInfo m_mapData;
#endif // IF_FEATURE_USER_DATA

};

#define g_objDiagManager (CDiagProcesser::GetInstace())

#endif //#ifdef IF_FEATURE_DIAGNOSE
#endif // __DIAG_PROCESSER_H__