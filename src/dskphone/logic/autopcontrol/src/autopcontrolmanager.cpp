#include "autopcontrolmanager.h"
#include "../include/autopcontrollmsgdefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "service_autop.h"
#include "devicelib/networkserver.h"
#include "wireless/wifi/include/modwifi.h"
#include "idlescreen/include/modidlescreen.h"
#include "adapterbox/include/modadapterbox.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "threadtask/include/modthreadtask.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "account/include/modaccount.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "rpssprocessor.h"
#include "zeroprocessor.h"
#include "codeprocessor.h"

#if IF_BUG_30385
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#endif

#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#else
#include "extended/actionuri_url/include/modactionuri_url.h"
#endif

#define ATP_MSG_MAP(x) {x,#x}

struct autop_debug_map
{
    int _msg;
    const char * _name;
};

static const autop_debug_map g_autop_debug_map[] =
{
    ATP_MSG_MAP(ATP_MSG_NOTIFY_AUTOP_END),
    ATP_MSG_MAP(ATP_MSG_REQ_START_RPS),
    ATP_MSG_MAP(ATP_MSG_REQ_RPS_STATUS),
    ATP_MSG_MAP(ATP_MSG_REQ_ZERO_OVER),
    ATP_MSG_MAP(ATP_MSG_NOTIFY_AUTH_ERR),
    ATP_MSG_MAP(ATP_MSG_REQ_START_POWER_ON_UPDATE),
    ATP_MSG_MAP(ATP_MSG_NOTIFY_AUTOP_START),
    ATP_MSG_MAP(ATP_MSG_NOTIFY_EXIT_AUTH),
    ATP_MSG_MAP(MKIT_MSG_SRVREG),
    {0, NULL}

};

const char * get_autop_msg_name(int _msg)
{
    for (int i = 0; g_autop_debug_map[i]._name != NULL; i++)
    {
        if (_msg == g_autop_debug_map[i]._msg)
        {
            return g_autop_debug_map[i]._name;
        }
    }

    return NULL;
}

LRESULT OnRedirecMessage(msgObject & objMsg)
{
    return _AutopControlMamager.ProcessRedirectMsg(objMsg);
}


CAutopControlData CAutopControlManager::m_autopData;
CAutopControlManager::CAutopControlManager()
{
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_END, ATP_MSG_NOTIFY_AUTOP_END, OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_REQ_START_RPS, ATP_MSG_REQ_START_RPS, OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_REQ_RPS_STATUS, ATP_MSG_REQ_RPS_STATUS, OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_REQ_ZERO_OVER, ATP_MSG_REQ_ZERO_OVER, OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_AUTH_ERR, ATP_MSG_NOTIFY_AUTH_ERR, OnRedirecMessage);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_REQ_START_POWER_ON_UPDATE, ATP_MSG_REQ_START_POWER_ON_UPDATE,
                          OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_START, ATP_MSG_NOTIFY_AUTOP_START, OnRedirecMessage);
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_EXIT_AUTH, ATP_MSG_NOTIFY_EXIT_AUTH, OnRedirecMessage);
    etl_RegisterMsgHandle(MKIT_MSG_SRVREG, MKIT_MSG_SRVREG, OnRedirecMessage);
    m_pProcessor = NULL;
    m_pAction = NULL;

    SetAutopNow(false);
    SetStartByZero(false);
    SetAutopStatus(AUTOP_POWER_ON_UPDATING);
}


CAutopControlManager::~CAutopControlManager()
{
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_END, ATP_MSG_NOTIFY_AUTOP_END, OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_REQ_START_RPS, ATP_MSG_REQ_START_RPS, OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_REQ_RPS_STATUS, ATP_MSG_REQ_RPS_STATUS, OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_REQ_ZERO_OVER, ATP_MSG_REQ_ZERO_OVER, OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_AUTH_ERR, ATP_MSG_NOTIFY_AUTH_ERR, OnRedirecMessage);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_REQ_START_POWER_ON_UPDATE, ATP_MSG_REQ_START_POWER_ON_UPDATE,
                            OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_AUTOP_START, ATP_MSG_NOTIFY_AUTOP_START,
                            OnRedirecMessage);
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_EXIT_AUTH, ATP_MSG_NOTIFY_EXIT_AUTH, OnRedirecMessage);
    etl_UnregisterMsgHandle(MKIT_MSG_SRVREG, MKIT_MSG_SRVREG, OnRedirecMessage);
}

CBaseAutopProcessor * CAutopControlManager::CreateProcessor(AUTOP_UPDATE_TYPE eType)
{
    ZERO_INFO("create [%d] autop process", eType);

    switch (eType)
    {
    case AUT_CODE:
        return new CCodeProcessor;
    case AUT_RPS:
        return new CRpsProcessor;
    case AUT_ZERO:
#if IF_BUG_30885
    case AUT_TELSTRA_DMS:
    case AUT_TELSTRA_OPTION66:
#endif // IF_BUG_30885
        return new CZeroProcessor;
#if IF_FEATURE_METASWITCH
    case AUT_MTSW_HTTP_DIGEST:
        return new CZeroProcessor(AUT_MTSW_HTTP_DIGEST);
#endif
    case AUT_NORMAL:
        return new CNorMalAutopProcessor;
    }

    return NULL;
}

void CAutopControlManager::Init()
{
    m_autopData.Init();

    if (mkservice_get_data(mkGetHandle(), MKSRV_ID_AUTOP, 0) <= 0)
    {
        ZERO_INFO("Init AUTOP: not running!");
        SetAutopStatus(AUTOP_POWER_ON_WAIT);
    }
    else
    {
        ProcessAutopStartup();
    }
}

void CAutopControlManager::Exit()
{
    ZERO_INFO("exit autop [%d]", GetUpdateType());

    if (NULL != m_pProcessor)
    {
        m_pProcessor->ExitProcessor();

        delete m_pProcessor;
        m_pProcessor = NULL;
    }

    SetAutopNow(false);
    SetStartByZero(false);

    // 通知外部Zero/RPS结束
    etl_NotifyApp(false, SETTING_MESSAGE_AUTOPCONTROL_EXIT, 0, 0);
}

bool CAutopControlManager::IsAutopCodeUsable()
{
    return (INVALID_ACCOUNTID == acc_GetUsableAccount(true));
}

bool CAutopControlManager::IsNeedShowUpdatingWnd()
{
    if (NULL == m_pProcessor)
    {
        return false;
    }

    return m_pProcessor->IsNeedShowUpdating();
}

bool CAutopControlManager::Start(AUTOP_UPDATE_TYPE eType)
{
    if (NULL != m_pProcessor)
    {
        return false;
    }

    m_pProcessor = CreateProcessor(eType);

    if (NULL == m_pProcessor)
    {
        return false;
    }

    return true;
}

AUTOP_UPDATE_TYPE CAutopControlManager::GetUpdateType()
{
    if (NULL == m_pProcessor)
    {
        return AUT_NONE;
    }

    return m_pProcessor->GetType();
}

bool CAutopControlManager::IsReadToConnect()
{
    POWER_ON_STATUS eStatus = (POWER_ON_STATUS) GetAutopStatus();

    return eStatus != AUTOP_POWER_ON_CONNECT && eStatus != AUTOP_POWER_ON_WAIT;
}

LRESULT CAutopControlManager::ProcessRedirectMsg(msgObject & objMsg)
{
    bool bHandle = false;

    const char * pMsgName = get_autop_msg_name(objMsg.message);

    if (NULL != pMsgName)
    {
        ZERO_INFO("process autop msg[%s] wparam[%d] lparam[%d]", pMsgName, objMsg.wParam, objMsg.lParam);
    }

    if (MKIT_MSG_SRVREG == objMsg.message
            && objMsg.wParam == MKSRV_ID_AUTOP)
    {
        ProcessAutopStartup();

        //处理完退出
        return TRUE;
    }

    if (NULL != m_pProcessor
            && objMsg.message == ATP_MSG_NOTIFY_AUTH_ERR)
    {
        // UI 弹窗，但是未执行更新时，收到鉴权弹窗信息,取消本次更新
        AUTOP_UPDATE_TYPE eType = GetUpdateType();
        if (!m_pProcessor->IsProcessEndMsg() && AUT_NORMAL != eType)
        {
            ZERO_WARN("[%d] processor is running", eType);
            Autop_NotifyAutoServer(false, ATP_MSG_REQ_RETURN_AUTH_INFO, 1, 0);
            return false;
        }
    }

    if (NULL != m_pProcessor)
    {
        bHandle = m_pProcessor->ProcessRedirectMsg(objMsg);
    }

    //processor 决定要不要外部再处理
    if (bHandle)
    {
        return TRUE;
    }

    switch (objMsg.message)
    {
    case TM_TIMER:
        {
            if (m_pProcessor == NULL
                    || (NULL != m_pProcessor && !m_pProcessor->OnTimer(objMsg.wParam)))
            {

                bHandle = _AutopControlMamager.OnTimer(objMsg.wParam);
            }
        }
        break;

    case ATP_MSG_NOTIFY_AUTOP_END:
        {
            //TODO. 删除处理类结束流程(也可以采取reset 的方式，避免重复申请释放内存)
            delete m_pProcessor;
            m_pProcessor = NULL;
            powerled_FlushPowerLed();
            etl_NotifyApp(FALSE, AUTOP_AUTOP_FINISH, objMsg.wParam, objMsg.lParam);
        }
        break;
    case ATP_MSG_REQ_START_RPS:
        {
            ZERO_INFO("receive rps start msg, wparam=%d, lparam=%d", objMsg.wParam, objMsg.lParam);
            if (!idleScreen_IsNetworkAvailable())
            {
                ZERO_INFO("net work is not ok");
                objMsg.ReplyMessage(2);
                return 0;
            }

            ZERO_INFO("net work is ok");
            objMsg.ReplyMessage(1);

            // 判断通话中
            if (talklogic_SessionExist())
            {
                return 0;
            }

            // 测试模式不进入
            if (AdapterBox_IsTestMode())
            {
                return 0;
            }

            if (1 == configParser_GetConfigInt(kszRedirectEnable))
            {
                AdapterBox_ZeroOpenDlg(ZERO_DLG_RPS);
            }
        }
        break;
    case ATP_MSG_NOTIFY_AUTOP_START:
        {
            if (NULL == m_pProcessor)
            {
                AdapterBox_ZeroOpenDlg(ZERO_DLG_EXIT);
                m_pProcessor = CreateProcessor(AUT_NORMAL);
                bHandle = m_pProcessor->ProcessRedirectMsg(objMsg);
            }
        }
        break;
    case ATP_MSG_NOTIFY_AUTH_ERR:
        {
            //autop 是先下载配置，再执行更新，所以会直接就收到了鉴权消息
            if (NULL == m_pProcessor)
            {
                m_pProcessor = CreateProcessor(AUT_NORMAL);
                bHandle = m_pProcessor->ProcessRedirectMsg(objMsg);
            }
        }
        break;
    case ATP_MSG_REQ_RPS_STATUS:
        {
            ZERO_INFO("receive ATP_MSG_REQ_RPS_STATUS, wparam=%d, lparam=%d", objMsg.wParam, objMsg.lParam);

            if (AUT_RPS == GetUpdateType())
            {
                objMsg.ReplyMessage(1);
            }
            else
            {
                objMsg.ReplyMessage(2);
            }
        }
        break;
    case ATP_MSG_REQ_ZERO_OVER:
        {
            AUTOP_UPDATE_TYPE eType = GetUpdateType();
            ZERO_INFO("receive ATP_MSG_REQ_ZERO_OVER[%d], current status[%d]", ATP_MSG_REQ_ZERO_OVER,
                      eType);
            if (AUT_ZERO != eType)
            {
                objMsg.ReplyMessage(1);
            }
        }
        break;
    case ATP_MSG_REQ_START_POWER_ON_UPDATE:
        {
#ifdef IF_SUPPORT_WIFI
            bool bSwitch = configParser_GetConfigInt(kszWifiSwitch) && WIFI_IsDongleVaild();
            ZERO_INFO("custom receive autop power on start msg, wparam=%d, lparam=%d wifi switch = %d",
                      objMsg.wParam, objMsg.lParam, bSwitch);
            if (bSwitch)
            {
                //是否是wifi网络
                sys_wifi_nw_stat_t stat_t;
                if (0 == sys_wifi_get_network_status(&stat_t, sizeof(sys_wifi_nw_stat_t))
                        && stat_t.wpa_state == WIFI_WPA_COMPLETED
                        && idleScreen_IsNetworkAvailable())
                {
                    ZERO_INFO("custom net work is ok");
                    objMsg.ReplyMessage(1);
                }
                else
                {
                    ZERO_INFO("custom net work is not ok");
                    objMsg.ReplyMessage(2);
                    return 0;
                }
            }
            else
#endif
            {
                if (!netIsNetworkReady())
                {
                    ZERO_INFO("custom net work is not ok");
                    objMsg.ReplyMessage(2);
                    return 0;
                }

                ZERO_INFO("custom net work is ok");
                objMsg.ReplyMessage(1);
            }
        }
        break;
    case ATP_MSG_NOTIFY_EXIT_AUTH:
        {
            AdapterBox_ZeroOpenDlg(ZERO_DLG_EXIT);
            if (m_pProcessor != NULL)
            {
                delete m_pProcessor;
                m_pProcessor = NULL;
            }
        }
        break;
    default:
        break;
    }

    return bHandle;
}

POWER_ON_STATUS CAutopControlManager::CheckAutopStatus()
{
    POWER_ON_STATUS eRet = AUTOP_POWER_ON_UPDATING;

    if (netIsNetworkReady())
    {
        eRet = (POWER_ON_STATUS) msgSendMsgToThreadTimeoutEx(
                   msgGetThreadByName(VP_AP_THREAD_NAME), ATP_MSG_REQ_GET_POWER_ON_STATUS, 0, 0, 0, NULL, 500);
    }

    ZERO_INFO("Check power on autop update status[%d]", eRet);

    return eRet;
}

void CAutopControlManager::ProcessAutopStartup()
{
    ZERO_INFO("Autop Startup AutopStatus[%d]", GetAutopStatus());

    bool bReadyConnect = (GetAutopStatus() == AUTOP_POWER_ON_CONNECT);

    SetAutopStatus(CheckAutopStatus());

    timerSetThreadTimer((UINT)&m_iAutopStatus, 3000);

    // 更新电源灯
    powerled_FlushPowerLed();

    if (bReadyConnect)
    {
        ReadyToConnect();
    }
}

bool CAutopControlManager::ReadyToConnect()
{
#if IF_BUG_30885
    CZeroProcessor * pZeroProcess = dynamic_cast<CZeroProcessor *>(m_pProcessor);
    if (NULL == pZeroProcess)
    {
        return false;
    }
    if (AUT_TELSTRA_OPTION66 == eType)
    {
        return pZeroProcess->ProcessOption66Connect();
    }
    else if (AUT_TELSTRA_DMS == eType)
    {
        return pZeroProcess->ProcessDMSConnect();
    }
#endif //IF_BUG_30885

    return m_pProcessor->ProcessConnect();
}

bool CAutopControlManager::OnTimer(unsigned int uTimerID)
{
    if (uTimerID == (UINT)&m_iAutopStatus)
    {
        SetAutopStatus(CheckAutopStatus());

        if (GetAutopStatus() == AUTOP_POWER_ON_END)
        {
            timerKillThreadTimer(uTimerID);
            // 上报上电更新autop完成
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_AUTOP_FINISH);
#else
            actURL_SendActionURL(ACTION_URL_AUTOP_FINISH);
#endif
            // 更新电源灯
            powerled_FlushPowerLed();

#if IF_BUG_30385
            xmlbrowser_RequestAutopXml();
#endif
        }
        return true;
    }
    return false;
}

void CAutopControlManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (NULL == pTaskAction)
    {
        return;
    }

    if (GetUpdateType() == AUT_NONE)
    {
        AdapterBox_ZeroOpenDlg(ZERO_DLG_CANCLE_AUTOP);

        m_pAction = NULL;
        return;
    }

    ZERO_INFO("OnTaskActionCallback result[%d] current type[%d]", pTaskAction->GetTaskActionErrorType(),
              GetUpdateType());

    CNetworkFileAction * pAction = dynamic_cast<CNetworkFileAction *>(pTaskAction);
    if (NULL == pAction)
    {
        return;
    }

    if (NULL == m_pProcessor)
    {
        return;
    }

    bool bHandle = m_pProcessor->OnTaskActionCallback(pAction);

    // 处理类不处理的，manager 处理
    if (!bHandle)
    {
        ;
    }

    m_pAction = NULL;
}


bool CAutopControlManager::ConnectServer(const yl::string & strUrl,
        AUTOP_UPDATE_TYPE eType/* = AUT_NONE*/)
{
//#warning V82
    ZERO_DEBUG("connect server [%s]...", strUrl.c_str());
    m_pAction = TaskAction_CreateNetworkFileAction(TEMP_PATH "xxx.cfg", strUrl.c_str());

    if (NULL == m_pAction)
    {
        return FALSE;
    }

    m_pAction->SetRead(TRUE);
    m_pAction->SetSync(FALSE);
    m_pAction->SetCallback(this);

    TASKACTION_DL_MODE eMode = MODE_NONE;

    if (AUT_RPS == eType)
    {
        m_pAction->SetRedirect(true);
    }

    m_pAction->SetDownLoadMode(eMode);

    // 开始连接服务器
    TaskAction_ExecAction(m_pAction);
    return TRUE;
}

bool CAutopControlManager::ProcessStartConnect(AUTOP_UPDATE_TYPE eType,
        const yl::string & strUserName /* = "" */, const yl::string & strPasswd /* = "" */)
{
    if (NULL == m_pProcessor)
    {
        return false;
    }

    //当前只能进行一种连接
    if (GetUpdateType() != eType)
    {
        return false;
    }

    ZERO_INFO("start connect type[%d],current type", eType, GetUpdateType());

//#warning V82
    ZERO_DEBUG("start connect username[%s] passwd[%s]", strUserName.c_str(), strPasswd.c_str());

    //if (!strUserName.empty() && !strPasswd.empty())
    {
        SetUserName(strUserName);
        SetPassword(strPasswd);
    }

    // 设置外部输入的鉴权信息
    m_pProcessor->SetAuthInfo(GetUserName(), GetPasswd());

    if (!IsReadToConnect())
    {
        ZERO_INFO("ConnectCheck Wait for autop");
        SetAutopStatus(AUTOP_POWER_ON_CONNECT);
        return true;
    }

    return ReadyToConnect();
}

bool CAutopControlManager::WaitingForNetwork()
{
    if (!netIsCableConnected()
#ifdef IF_SUPPORT_WIFI
            && !WIFI_IsEnable(false)
#endif //IF_SUPPORT_WIFI        
       )
    {
        ZERO_INFO("Not get IP Address[%d][%d]!", configParser_GetConfigInt(kszWifiSwitch),
                  netIsCableConnected());
        return false;
    }

    // http://192.168.1.99/Bug.php?BugID=19538
    int iWaitTimes = configParser_GetConfigInt(kszRedirectWaitTimes);
    int iDelayTime = configParser_GetConfigInt(kszRedirectDelayTime);
    if (iWaitTimes <= 0
            || iDelayTime <= 0)
    {
        iWaitTimes = 5;
        iDelayTime = 1000;
    }

    // 等待网络时，需要显示提示。
    int iTimes = 0;

    // 重定向中加长至50s
    while (iTimes++ < iWaitTimes)
    {
        // 网络正常直接返回
        if (idleScreen_IsNetworkAvailable())
        {
            return true;
        }

        etlSleep(iDelayTime);
    }

    ZERO_INFO("Not get IP Address!");
    return false;
}


CAutopControlData::CAutopControlData()
{
    m_lpPressing = NULL;
}

CAutopControlData::~CAutopControlData()
{
    //释放数据
    ReleaseAutopCode();
}

void CAutopControlData::Init()
{
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {
        const char * lpWildCard = "autoprovision.*.code";
        YLList<Config_Item> listCfgItem;
        int iResult = configParse_FindCfgItem(lpWildCard, listCfgItem);

        for (YLList<Config_Item>::iterator itBeg = listCfgItem.begin();
                itBeg != listCfgItem.end();
                ++itBeg)
        {
            Config_Item & cfgItem = (*itBeg);

            int iIdx = -1;

            if (sscanf(cfgItem.m_strKey.c_str(), "autoprovision.%d.code", &iIdx) != 1)
            {
                continue;
            }

            if (cfgItem.m_strValue.empty())
            {
                continue;
            }

            LPAUTOP_PARAM lpData = new AUTOP_PARAM();

            lpData->iIdx = iIdx;
            lpData->strCode = cfgItem.m_strValue.c_str();
            configParser_GetCfgItemStringValue(kszAutopCodeServer, iIdx)
            .copy(lpData->lpAutoPData->url, ATP_MAX_INFO_URL_LEN);

            if (!ParseUserName(lpData->lpAutoPData->url, lpData))
            {
                configParser_GetCfgItemStringValue(kszAutopCodeUser, iIdx)
                .copy(lpData->lpAutoPData->username, ATP_INFO_FIELD_LEN);

                configParser_GetCfgItemStringValue(kszAutopCodePassword, iIdx)
                .copy(lpData->lpAutoPData->password, ATP_INFO_FIELD_LEN);
            }

            lpData->name = configParser_GetCfgItemStringValue(kszAutopCodeName, iIdx);

            configParser_GetCfgItemStringValue(kszAutopCodeAES, iIdx)
            .copy(lpData->lpAutoPData->AES, ATP_INFO_AES_LEN);

            configParser_GetCfgItemStringValue(kszAutopCodeMACAES, iIdx)
            .copy(lpData->lpAutoPData->MAC_AES, ATP_INFO_AES_LEN);

            m_listAutopParam.push_back(lpData);
        }
    }

    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {
        // 读取文件,加载数据
        chIniFileA file;
        if (!file.LoadFromFile(szAutopCode))
        {
            return;
        }

        iniObjList * pCurrentSection = &file;

        // section，链表中的元素为section指针
        iniObjList::iterator it = pCurrentSection->begin();
        for (; it != pCurrentSection->end(); ++it)
        {
            iniObject * pLine = *it;
            if (NULL == pLine || LT_SECTION != pLine->type())
            {
                continue;
            }

            int iIdx = -1;
            if (sscanf(pLine->key().c_str(), "autoprovision%d", &iIdx) != 1)
            {
                continue;
            }

            yl::string strCode = configParser_GetCfgItemStringValue(kszAutopCodeCode, iIdx);
            if (strCode.empty())
            {
                continue;
            }

            LPAUTOP_PARAM lpData = new AUTOP_PARAM();
            if (NULL == lpData)
            {
                continue;
            }

            lpData->iIdx = iIdx;
            lpData->strCode = strCode;

            configParser_GetCfgItemStringValue(kszAutopCodeServer, iIdx)
            .copy(lpData->lpAutoPData->url, ATP_MAX_INFO_URL_LEN);

            if (!ParseUserName(lpData->lpAutoPData->url, lpData))
            {
                configParser_GetCfgItemStringValue(kszAutopCodeUser, iIdx)
                .copy(lpData->lpAutoPData->username, ATP_INFO_FIELD_LEN);

                configParser_GetCfgItemStringValue(kszAutopCodePassword, iIdx)
                .copy(lpData->lpAutoPData->password, ATP_INFO_FIELD_LEN);
            }

            lpData->name = configParser_GetCfgItemStringValue(kszAutopCodeName, iIdx);

            configParser_GetCfgItemStringValue(kszAutopCodeAES, iIdx)
            .copy(lpData->lpAutoPData->AES, ATP_INFO_AES_LEN);

            configParser_GetCfgItemStringValue(kszAutopCodeMACAES, iIdx)
            .copy(lpData->lpAutoPData->MAC_AES, ATP_INFO_AES_LEN);

            m_listAutopParam.push_back(lpData);
        }
    }
}

void CAutopControlData::ReleaseAutopCode()
{
    YLList<LPAUTOP_PARAM>::iterator itr = m_listAutopParam.begin();
    YLList<LPAUTOP_PARAM>::iterator itrEnd = m_listAutopParam.end();
    while (itr != itrEnd)
    {
        DEL_OBJ((*itr));
        ++itr;
    }

    m_listAutopParam.clear();
}

int CAutopControlData::MatchAutoPCodePrefix(const yl::string & strNum, char cKey /* = 0 */)
{
    int nMatch = -1;
    m_lpPressing = NULL;


    YLList<LPAUTOP_PARAM>::iterator itr = m_listAutopParam.begin();
    YLList<LPAUTOP_PARAM>::iterator itrEnd = m_listAutopParam.end();
    LPAUTOP_PARAM lpQuery = NULL;
    char lpNum[512] = "";
    if (cKey == 0)
    {
        sprintf(lpNum, "%s", strNum.c_str());
    }
    else
    {
        sprintf(lpNum, "%s%c", strNum.c_str(), cKey);
    }

    int iLength = strlen(lpNum);

    for (; itr != itrEnd; ++itr)
    {
        lpQuery = (LPAUTOP_PARAM)(*itr);
        if (NULL == lpQuery)
        {
            ++itr;
            continue;
        }

        int iCodeLength = lpQuery->strCode.size();
        if (strncmp(lpQuery->strCode.c_str(), lpNum, iLength) == 0)
        {
            if (iCodeLength == iLength)
            {
                // 正好完全匹配上
                nMatch = 1;
                m_lpPressing = lpQuery;
                idleScreen_SetStatus(PS_STATE_AUTO_PROVISION);
                break;
            }
            else
            {
                nMatch = 0;
            }
        }
    }

    ZERO_INFO("CAutoPCode::MatchAutoPCodePrefix Result = [%s], [%d]", lpNum, nMatch);

    return nMatch;
}

bool CAutopControlData::ParseUserName(const yl::string & strUrl, LPAUTOP_PARAM lpPressing)
{
    if (strUrl.empty()
            || NULL == lpPressing)
    {
        return false;
    }

    char bufType[ATP_MAX_INFO_URL_LEN] = {0};
    char bufUserName[ATP_INFO_FIELD_LEN] = {0};
    char bufPasswd[ATP_INFO_FIELD_LEN] = {0};
    char bufURL[ATP_MAX_INFO_URL_LEN] = {0};

    if (4 == sscanf(strUrl.c_str(), "%[^:]://%[^:]:%[^@]@%s", bufType, bufUserName, bufPasswd, bufURL))
    {
        strcpy(lpPressing->lpAutoPData->url, bufType);
        strcat(lpPressing->lpAutoPData->url, "://");
        strcat(lpPressing->lpAutoPData->url, bufURL);

        strcpy(lpPressing->lpAutoPData->username, bufUserName);
        strcpy(lpPressing->lpAutoPData->password, bufPasswd);

        return true;
    }

    return false;
}
