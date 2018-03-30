#pragma once

#include "basetalkproxy.h"
#include "talk/talklogic/include/modtalklogic.h"

class CRecordASyncProxy
    : public CBaseTalkASyncProxy
{
public:

    explicit CRecordASyncProxy(int iSessionID);

    void SetRecord(INF_RECORD_TYPE eRecord);

    bool OnRecord(CCallInfo* pCallInfo, int iDsskeyID, DssKey_Type eType);

    bool OnCallBack();

    bool OnSipRecordCallBack(bool bStart, bool bSuccess);

    bool OnURLRecordCallBack(int iSessionID, bool bSuccess);

    bool IsWaitForResult(DssKey_Type eType);

    void UpdateRecordStatus(bool bUsing);

private:

    static void SetRecordLight(int iDsskeyID, bool bUsing);

public:
    SIP_RECORD_STATUS   m_eSIPRecordStatus;
    URL_RECORD_ERROR    m_eURLRecordError;  // URL录音错误类型
    size_t              m_eRecordType;      // 录音类型

private:
    int                 m_iSIPRecordKey;    // 通过SIP通话录音对应的Dsskey按键
    int                 m_iURLRecordKey;    // 通过URL通话录音对应的Dsskey按键
    bool                m_bWaitURLRecord;   // 是否在等待URL录音返回
    bool                m_bWaitSIPRecord;   // 是否在等待SIP录音返回
};

