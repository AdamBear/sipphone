#ifndef __CALLPARK_PROCESS_H__
#define __CALLPARK_PROCESS_H__

#include "callparkcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

class CParkProcesser : public XsiBaseProcesser
{
public:

    static CParkProcesser & GetInstance();

    bool CallPark(int iCallId, const yl::string & strTarget, const yl::string & strXsiCallId,
                  int iAccountId);

    bool GroupPark(int iCallId, const yl::string & strXsiCallId,
                   int iAccountId);

    // 执行RetrieveCallPark操作
    bool RetrieveCallPark(const yl::string & strTarget,
                          int iAccountId);

    void ProcessCallParkNotifMSG(msgObject & msg);


private:
    CParkProcesser();

    virtual ~CParkProcesser();


private:

    DataParkNotify m_objNotifyData;

};

#define g_objCallParkProcesser (CParkProcesser::GetInstance())

#endif

