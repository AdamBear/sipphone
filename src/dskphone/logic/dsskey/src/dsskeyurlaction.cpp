#include "config.h"
#include "dsskeyurlaction.h"
#include "../include/dsskeystructure.h"
#include "../include/moddsskey.h"
#include <threadtask/include/networkfileaction.h>
#include <threadtask/include/modthreadtask.h>
#include <taskaction/modtaskaction.h>
#include "configiddefine.h"
#include "modconfigparser.h"

#define NETWORK_AVAILABLE       "Net available"     // label for url
#define NETWORK_UNAVAILABLE     "Net unavailable"   // label for url
#define NETWORK_UNKNOW          "Net unknow"        // label for url
using namespace NS_TA;
YLHashMap<CTaskAction *, int> m_TaskactionMap;

CDSSkeyURLAction::CDSSkeyURLAction(void): IDsskeyImplAction(DT_URL)
{
}

CDSSkeyURLAction::~CDSSkeyURLAction(void)
{

}

bool CDSSkeyURLAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                  void * pExtraData)
{
    // 判断value是否为空
    if (logicData.strValue.empty())
    {
        return false;
    }


    yl::string strUrl = logicData.strValue;
    if (strUrl.find("http://") == yl::string::npos
            && strUrl.find("https://") == yl::string::npos)
    {
        strUrl = "http://" + logicData.strValue;
    }

    // 实例化CNetworkFileAction.
    CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(TEMP_PATH "DsskeyURL.txt",
                                   strUrl.c_str());
    if (pAction == NULL || this == NULL)
    {
        return false;
    }

    pAction->SetCheckUrl(true);
    pAction->SetSync(false);  // 设置异步.
    pAction->SetRead(true);  // 设置读.
    pAction->SetCallback(this);  // 设置回调类.
    bool isSucceed = TaskAction_ExecAction(pAction);  // 开始请求.
    m_TaskactionMap.put(pAction, dsskeyDataID);

    return isSucceed;
}

void CDSSkeyURLAction::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (configParser_GetConfigInt(kszFlashURLDsskey) != 1)
    {
        m_TaskactionMap.erase(pTaskAction);
        return;
    }

    YLHashMap<CTaskAction *, int>::iterator iter = m_TaskactionMap.find(pTaskAction);
    if (iter != m_TaskactionMap.end())
    {

        int dsskeyID = iter->second;
        TaskActionErrorType eErrorType = pTaskAction->GetTaskActionErrorType();

        DSSKEY_INFO("eErrorType = %d", eErrorType);

        switch (eErrorType)
        {
        case TA_NO_ERROR:
            {
                dsskey_SetDsskeyStatus(dsskeyID, DS_URL_OK);
                dsskey_SetDssekeyExtFiled(dsskeyID, "network_status", NETWORK_AVAILABLE);
            }
            break;
        case TA_URL_US_NOKTEXT:
            {

                dsskey_SetDsskeyStatus(dsskeyID, DS_URL_NETWORK_UNAVAILABLE);
                dsskey_SetDssekeyExtFiled(dsskeyID, "network_status", NETWORK_UNAVAILABLE);
            }
            break;
        case TA_URL_BLANK_NRECOGNIZED:
        case TA_URL_TIMEOUT:
        case TA_URL_ERROR:
            {
                dsskey_SetDsskeyStatus(dsskeyID, DS_URL_NETWORK_UNKNOW);
                dsskey_SetDssekeyExtFiled(dsskeyID, "network_status", yl::string(NETWORK_UNKNOW));
            }
            break;
        default:
            break;
        }

        dsskey_refreshDSSKeyUI(dsskeyID);
        //lcm在这边要设置一个定时器
        dsskey_SetTimer(dsskeyID, 5000);
        m_TaskactionMap.erase(pTaskAction);
    }
}
bool CDSSkeyURLAction::OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData & logicData,
                               void * pExtraData)
{
    if (timerID == dsskey_GetRelatedTimerID(dsskeyDataID))
    {
        dsskey_ResetDsskeyStatus(dsskeyDataID);
        return true;
    }
    return false;
}
