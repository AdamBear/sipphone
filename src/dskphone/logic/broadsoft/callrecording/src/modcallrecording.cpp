#include "modcallrecording.h"
#include "callrecordingmanager.h"
#include "broadsoft/xsiactionhelper/include/modxsicommon.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
#include "voip/sipsender.h"
#include "service_sip_interface.h"

bool callRecording_StartRecord(int iCallId, int iAccountId, bool isNetworkConf /*= false*/)
{
    int nMode = configParser_GetConfigInt(kszCallRecordMode);
    XSI_INFO("callRecording_StartRecord Current Record Mode [%d]", nMode);
    if (nMode == 0)
    {
        if (isNetworkConf)
        {
            bool bResult = false;
            ListXsiCallsID listCallId;
            Xsi_GetConfCallId(listCallId);
            ListXsiCallsID::iterator iter = listCallId.begin();
            for (; iter != listCallId.end(); ++iter)
            {
                bResult |= g_objCallRecording.CallRecord(iCallId, *iter, iAccountId);
            }
            return bResult;
        }

        yl::string strXsiCallId = Xsi_GetXsiCallId(iCallId);

        if (strXsiCallId.empty())
        {
            XSI_INFO("XSI ID Empty");
            return false;
        }

        return g_objCallRecording.CallRecord(iCallId, strXsiCallId, iAccountId);
    }
    else if (nMode == 1)
    {
        XSI_INFO("Call Record Current Record iAccountId %d iCallId %d", iAccountId, iCallId);
        return _SIPSender.BwCallRecord(iAccountId, iCallId, SIP_BW_CALL_RECORD_OP_START);
    }

    return false;
}

bool callRecording_StopRecord(int iCallId, int iAccountId, bool isNetworkConf /*= false*/)
{
    int nMode = configParser_GetConfigInt(kszCallRecordMode);
    XSI_INFO("callRecording_StopRecord Current Record Mode [%d]", nMode);
    if (nMode == 0)
    {
        if (isNetworkConf)
        {
            bool bResult = false;
            ListXsiCallsID listCallId;
            Xsi_GetConfCallId(listCallId);
            ListXsiCallsID::iterator iter = listCallId.begin();
            for (; iter != listCallId.end(); ++iter)
            {
                bResult |= g_objCallRecording.StopRecord(iCallId, *iter, iAccountId);
            }
            return bResult;
        }

        yl::string strXsiCallId = Xsi_GetXsiCallId(iCallId);

        if (strXsiCallId.empty())
        {
            XSI_INFO("XSI ID Empty");
            return false;
        }
        return g_objCallRecording.StopRecord(iCallId, strXsiCallId, iAccountId);
    }
    else if (nMode == 1)
    {
        XSI_INFO("Call Record Current Record iAccountId %d iCallId %d", iAccountId, iCallId);
        return _SIPSender.BwCallRecord(iAccountId, iCallId, SIP_BW_CALL_RECORD_OP_STOP);
    }
    return false;
}

bool callRecording_PauseRecord(int iCallId, int iAccountId, bool isNetworkConf /*= false*/)
{
    int nMode = configParser_GetConfigInt(kszCallRecordMode);
    XSI_INFO("callRecording_PauseRecord Current Record Mode [%d]", nMode);

    if (nMode == 0)
    {
        if (isNetworkConf)
        {
            bool bResult = false;
            ListXsiCallsID listCallId;
            Xsi_GetConfCallId(listCallId);
            ListXsiCallsID::iterator iter = listCallId.begin();
            for (; iter != listCallId.end(); ++iter)
            {
                bResult |= g_objCallRecording.PauseRecord(iCallId, *iter, iAccountId);
            }
            return bResult;
        }

        yl::string strXsiCallId = Xsi_GetXsiCallId(iCallId);

        if (strXsiCallId.empty())
        {
            XSI_INFO("XSI ID Empty");
            return false;
        }
        return g_objCallRecording.PauseRecord(iCallId, strXsiCallId, iAccountId);
    }
    else if (nMode == 1)
    {
        XSI_INFO("Call Record Current Record iAccountId %d iCallId %d", iAccountId, iCallId);
        return _SIPSender.BwCallRecord(iAccountId, iCallId, SIP_BW_CALL_RECORD_OP_PAUSE);
    }
    return false;
}

bool callRecording_ResumeRecord(int iCallId, int iAccountId, bool isNetworkConf /*= false*/)
{
    int nMode = configParser_GetConfigInt(kszCallRecordMode);
    XSI_INFO("callRecording_ResumeRecord Current Record Mode [%d]", nMode);

    if (nMode == 0)
    {
        if (isNetworkConf)
        {
            bool bResult = false;
            ListXsiCallsID listCallId;
            Xsi_GetConfCallId(listCallId);
            ListXsiCallsID::iterator iter = listCallId.begin();
            for (; iter != listCallId.end(); ++iter)
            {
                bResult |= g_objCallRecording.ResumeRecord(iCallId, *iter, iAccountId);
            }
            return bResult;
        }

        yl::string strXsiCallId = Xsi_GetXsiCallId(iCallId);

        if (strXsiCallId.empty())
        {
            XSI_INFO("XSI ID Empty");
            return false;
        }
        return g_objCallRecording.ResumeRecord(iCallId, strXsiCallId, iAccountId);
    }
    else if (nMode == 1)
    {
        XSI_INFO("Call Record Current Record iAccountId %d iCallId %d", iAccountId, iCallId);
        return _SIPSender.BwCallRecord(iAccountId, iCallId, SIP_BW_CALL_RECORD_OP_RESUME);
    }
    return false;
}

void callRecord_SetRecordMode(int iAccountId, CALL_RECORDING_MODE eMode)
{
    g_objCallRecording.SetRecordMode(iAccountId, eMode);
}

CALL_RECORDING_MODE callRecord_GetRecordMode(int iAccountId)
{
    return g_objCallRecording.GetRecordMode(iAccountId);
}

bool callRecord_IsCallRecordOn(int iAccountId)
{
    return g_objCallRecording.IsCallRecordOn(iAccountId);
}


bool callRecord_SetServerRecordMode(SCallRecord sRecordInfo, int iAccountId)
{
    return g_objCallRecording.SetServerRecordMode(sRecordInfo, iAccountId);
}

bool callRecord_QueryServerRecordMode(int iAccountId)
{
    return g_objCallRecording.QueryServerRecordMode(iAccountId);
}

void callRecord_GetServerRecordMode(SCallRecord & RecordInfo, int iAccountId)
{
    g_objCallRecording.GetServerRecordMode(RecordInfo, iAccountId);
}

void callRecord_CancelRequest()
{
    g_objCallRecording.CancelAllRequest();
}

bool callRecord_IsSipRecordMode()
{
    return (1 == configParser_GetConfigInt(kszCallRecordMode));
}
#endif
