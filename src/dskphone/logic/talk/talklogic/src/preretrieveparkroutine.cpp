#ifdef IF_FEATURE_BROADSOFT_CALLPARK
#include "talklogic_inc.h"

CPreRetrieveParkRoutine::CPreRetrieveParkRoutine()
{
}

CPreRetrieveParkRoutine::~CPreRetrieveParkRoutine()
{
}

// 呼出当前已拨号码
bool CPreRetrieveParkRoutine::CallOut(const yl::string& strNumber, yl::string strName /*= ""*/, int iCallOutTriggerMask /*= COT_DEFAULT*/, bool bAudioOnly /*= true*/)
{
    if (configParser_GetConfigInt(kszCallParkMode) != 0)
    {
        yl::string strCalloutNumber = configParser_GetConfigString(kszRetriveParkCode);

        strCalloutNumber.append(strNumber);
        // 根据号码和名称呼出
        // http://10.2.1.199/Bug.php?BugID=117005
        if (!strCalloutNumber.empty())
        {
            CDialRoutine::CallOut(strCalloutNumber, "", iCallOutTriggerMask, bAudioOnly);
        }
    }
    else
    {
        // 取回Park 通话的操作
        callPark_RetrieveCallPark(strNumber, m_pCallInfo->GetAccountID());

        m_pCallSession->ExitSession(false);
    }
    return true;
}

#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
