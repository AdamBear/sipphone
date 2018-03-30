#ifndef __CALL_RECORDING_H__
#define __CALL_RECORDING_H__

#include "ETLLib.hpp"
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"
#include "broadsoft/callrecording/include/callrecord_def.h"

typedef YLHashMap<int, CALL_RECORDING_MODE> MapAccIDRecordMode;

typedef YLHashMap<int, SCallRecord> MapRecordInfo;

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING

class CallRecordingProcesser : public XsiBaseProcesser
{
public:

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    static CallRecordingProcesser & GetInstance();

    bool CallRecord(int iCallId, const yl::string & strXsiCallId,
                    int iAccountId);

    bool StopRecord(int iCallId, const yl::string & strXsiCallId,
                    int iAccountId);

    bool PauseRecord(int iCallId, const yl::string & strXsiCallId,
                     int iAccountId);

    bool ResumeRecord(int iCallId, const yl::string & strXsiCallId,
                      int iAccountId);

    void SetRecordMode(int iAccountId, CALL_RECORDING_MODE eMode);

    CALL_RECORDING_MODE GetRecordMode(int iAccountId);

    bool IsCallRecordOn(int iAccountId);

    bool ProcesserMSG(msgObject & objMessage);

    bool SetServerRecordMode(SCallRecord sRecordInfo, int nLineID);

    bool SaveRequestCmdToFile(const yl::string & strFilePath);

    bool QueryServerRecordMode(int nLineID);

    bool ParseReponseCmdFromFile(const yl::string & strFilePath);

    void GetServerRecordMode(SCallRecord & RecordInfo, int nLineID);

    yl::string GetXmlPauseResumeMethod(CR_PAUSERESUME_METHOD eMethod);

    yl::string GetXmlRecordMode(CALL_RECORDING_MODE eMode);

    int GetValueByText(yl::string strValue);

protected:
    virtual bool ProcessByType(XsiAction * pAction);

private:

    CallRecordingProcesser(void);

    virtual ~CallRecordingProcesser(void);


private:
    MapAccIDRecordMode m_mapIdToMode;

    MapRecordInfo m_mapCallRecordInfo;

    chCriticalSection   m_cs;           // map操作加锁

    bool m_bConfRecordSuc;

    SCallRecord m_CallRecodInfo;
};

#define g_objCallRecording CallRecordingProcesser::GetInstance()
#endif


#endif
