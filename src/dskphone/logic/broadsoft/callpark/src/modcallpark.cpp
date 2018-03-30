#include "broadsoft/callpark/include/modcallpark.h"
#include "callpark_include.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talkadapter/include/sessionstateproxy.h"

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
bool callPark_CallPark(int iCallId, const yl::string & strExtensionNum,
                       int iAccountId/* = AUTO_SELECT_LINE_ID*/)
{
    yl::string strNumber = strExtensionNum;

    if (configParser_GetConfigInt(kszCallParkMode) == 1)
    {
        CallOut_Param tCallParam;
        tCallParam.eCallOutType = TCT_CALLPARK;
        return talklogic_CallOut(strNumber, "", iAccountId, &tCallParam);
    }
    else if (configParser_GetConfigInt(kszCallParkMode) == 2)
    {
        //盲转
        if (strNumber.empty())
        {
            return false;
        }

		CSessionStateProxy proxy;
        if (!proxy)
        {
            DSSKEY_INFO("[DSSKEY][Warning] m_pFocusCallSession is NULL in callPark_CallPark()");
            return false;
        }

		proxy.TransferForCallpark(strNumber);
        return true;
    }

    yl::string strXsiCallId = g_objCallIdProcesser.GetXsiCallId(iCallId);
    return g_objCallParkProcesser.CallPark(iCallId, strExtensionNum, strXsiCallId, iAccountId);
}

bool callPark_GroupCallPark(int iCallId, int iAccountId/* = AUTO_SELECT_LINE_ID*/)
{
    if (configParser_GetConfigInt(kszCallParkMode) == 1)
    {
        yl::string strNumber = configParser_GetConfigString(kszGroupCallParkCode);

        CallOut_Param tCallParam;
        tCallParam.eCallOutType = TCT_CALLPARK;
        return talklogic_CallOut(strNumber, "", iAccountId, &tCallParam);
    }

    yl::string strXsiCallId = g_objCallIdProcesser.GetXsiCallId(iCallId);
    return g_objCallParkProcesser.GroupPark(iCallId, strXsiCallId, iAccountId);
}

bool callPark_RetrieveCallPark(const yl::string & strExtensionNum,
                               int iAccountId/* = AUTO_SELECT_LINE_ID*/)
{
    if (configParser_GetConfigInt(kszCallParkMode) == 2)
    {
        yl::string strNumber = strExtensionNum;

        if (strNumber.empty())
        {
            strNumber = configParser_GetConfigString(kszRetriveParkCode);
        }

        return talklogic_CallOut(strNumber, "", iAccountId);
    }
    else if (configParser_GetConfigInt(kszCallParkMode) != 0)
    {
        yl::string strNumber = configParser_GetConfigString(kszRetriveParkCode);

        strNumber.append(strExtensionNum);

        return talklogic_CallOut(strNumber, "", iAccountId);
    }

    if (CBaseHelper::GetXSIStatus(iAccountId) != XSI_SUCCESS)
    {
        etl_NotifyApp(false, XSI_MESSAGE_REQ_FAIL, XSI_PARK_RETRIVE, 0);
        return false;
    }

    return g_objCallParkProcesser.RetrieveCallPark(strExtensionNum, iAccountId);
}

void callPark_Init()
{
    g_objCallParkProcesser;
    g_objCallIdProcesser;
}


#else
bool callPark_CallPark(int iCallId, const yl::string & strExtensionNum,
                       int iAccountId/* = AUTO_SELECT_LINE_ID*/);
{
    int i = 0;
}

void callPark_GroupCallPark(int iCallId, int iAccountId/* = AUTO_SELECT_LINE_ID*/)
{
}

void callPark_RetrieveCallPark(const yl::string & strExtensionNum,
                               int iAccountId/* = AUTO_SELECT_LINE_ID*/)
{
}

void callPark_Init()
{
}
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
