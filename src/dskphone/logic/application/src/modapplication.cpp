#include "app_inc.h"

void application_Init()
{
    //_DoorPhoneManager;
#ifdef IF_FEATURE_ENTRANCE_GUARD
    g_refEntranceGuard;
#endif
}

#ifdef IF_FEATURE_ENTRANCE_GUARD
#define ENTRANCE_GUARD_SELF_TEST 0

bool application_IsDoorLineEnable()
{
    if (DSK_PHONE_TYPE_COLOR != commonUnits_GetDskPhoneType())
    {
        return false;
    }
    return CEntranceGuardLogic::IsEnable();
}

yl::string application_GetDTMFString(int iSessionId)
{
    return g_refEntranceGuard.GetDTMFString(iSessionId);
}

int application_GetCurrentTaskId()
{
    return g_refEntranceGuard.GetFocusTaskId();
}

#if ENTRANCE_GUARD_SELF_TEST
// 自测代码，因该客户没有环境，该函数用于自测
static bool application_ModTest(int iTaskId)
{
    // 形如： 3011 40110 511010 第一个数字代表个数， 接下来的Session对应是否开启DoorLine
    yl::string strConfig = configParser_GetConfigString(kszPushXMLServerIP);
    APP_INFO("Talk DoorLine Info config=[%s]", strConfig.c_str());
    if (!strConfig.empty() && Comm_IsAllDigit(strConfig.c_str()))
    {
        int iCount = strConfig[0] - '0';

        APP_INFO("Talk DoorLine Info session=[%d] count=[%d]", iTaskId, iCount);
        if (iCount == strConfig.size() - 1)
        {
            int iIndex = iTaskId % iCount;
            yl::string strUrl;
            yl::string strDtmf;

            if (strConfig[iIndex] - '0' > 0)
            {
                struct DoorLineDetails
                {
                    const char* strUrl;
                    const char* strDtmf;
                };

                static DoorLineDetails strCache[] =
                {
                    {"http://10.3.19.10/image/target.jpg",   "0000"},
                    {"http://10.3.19.10/image/doorline.jpg", "8846"},
                    //{"http://localhost/screencapture",      "4848"},
                    //{"http://10.3.19.10/image/1080.jpg",     "1080"},
                    {"http://10.3.19.10/image/girl.jpg",   "1111"},
                    {"http://10.3.19.10/image/men.jpg",    "2222"},
                    {"http://10.3.19.10/image/dog.jpg",    "3333"},
                    {"http://10.3.19.10/image/coco.jpg",   "4444"},
                    {"http://10.3.19.10/image/boy.jpg",    "5555"},
                };

                // Cache循环显示
                int iCacheIndex = iIndex % (sizeof(strCache) / sizeof(strCache[0]));
                strUrl = strCache[iCacheIndex].strUrl;
                strDtmf = strCache[iCacheIndex].strDtmf;
            }

            APP_INFO("Talk DoorLine Info index=[%d] ch=[%c] url=[%s] dtmf=[%s]", iIndex, strConfig[iIndex],
                     strUrl.c_str(), strDtmf.c_str());
            return g_refEntranceGuard.CreateTask(iTaskId, strDtmf, strUrl);
        }
    }
    return false;
}
#endif

bool application_CreateDoorLineTask(int iTaskId, const yl::string& strDtmf,
                                    const yl::string& strUrl)
{
#if ENTRANCE_GUARD_SELF_TEST
    if (application_ModTest(iTaskId))
    {
        return true;
    }
#endif
    return application_IsDoorLineEnable() && g_refEntranceGuard.CreateTask(iTaskId, strDtmf, strUrl);
}

void application_SetAutoAnswer(int iSessionId, bool bAuto /*= false*/)
{
    g_refEntranceGuard.SetAutoAnswer(iSessionId, bAuto);
}

EDL_COMPATIBLE application_GetEDLCType()
{
#if ENTRANCE_GUARD_SELF_TEST
    return (EDL_COMPATIBLE)(configParser_GetConfigInt(kszAURILimitIP) % 4);
#else
    return (EDL_COMPATIBLE)configParser_GetConfigInt(kszSipInfoInRing);
#endif
}

bool application_IsAutoAnswer(int iSessionId)
{
    EDL_COMPATIBLE eType = application_GetEDLCType();
    return ((eType == EDLC_PHONE_SIP || EDLC_PHONE == eType)
            &&  g_refEntranceGuard.IsAutoAnswer(iSessionId));
}

bool application_IsNeedAutoUnHold()
{
    return application_GetEDLCType() == EDLC_PHONE_SIP;
}

#endif


