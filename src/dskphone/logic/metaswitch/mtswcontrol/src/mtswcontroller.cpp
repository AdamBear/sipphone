#if IF_FEATURE_METASWITCH
#include "mtswcontroller.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/networkfileaction.h"
#include "threadtask/include/uploadfileaction.h"
#include "threadtask/include/modthreadtask.h"
#include "dsklog/log.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "mtswcommondefine.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "service_config.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonunits/modcommonunits.h"
#include "account/include/modaccount.h"
#include "ylstringalgorithm.h"
#include "adapterbox/include/modadapterbox.h"
#include "setting_page_define.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "dsskey/include/moddsskey.h"

int CMTSWController::m_iFileIndex = 0;

namespace MTSW
{
#define kszTempFile TEMP_PATH "mtswdata"
#define kszVoiceMailFile TEMP_PATH "voicemail.wav"
#define MAX_FILE_ID 20

#define kszURL      "http://wailea.metaswitch.com/cp/"
#define kszVersion  "version=8.3.10"

#define kszHead " multipart/form-data; boundary=---------------------------7de7b272607a"
#define kszBoundary "-----------------------------7de7b272607a"

#define kszYealink "Yealink"
}


static LRESULT ProcessConfigChangeMsg(msgObject & msg)
{
    if (msg.message != CONFIG_MSG_BCAST_CHANGED)
    {
        return FALSE;
    }

    switch (msg.wParam)
    {
    case ST_MTSW_CONFIG:
        {
            _MTSWController.Login();
        }
        break;

    case ST_MTSW_KEEPALIVE:
        {
            _MTSWController.SetKeepAliveTimer();
        }
        break;

    case ST_MAX_DOWNLOAD_SIZE:
        {
            _MTSWController.LoadMaxDownloadSize();
        }
        break;

    default:
        break;
    }

    return TRUE;
}

// 定时器处理
LRESULT ProcessOnTimer(msgObject & msg)
{
    return _MTSWController.OnTimer(msg.wParam);
}



CMTSWController::CMTSWController()
    : m_strSessionID(NULL)
    , m_strCommprotalURL(NULL)
    , m_iTimerKeepAlive(0)
    , m_eDownLoading(FILE_LOADING_READY)
    , m_iTimerLoginTimeOut(5)
{
    LoadMaxDownloadSize();
}

CMTSWController::~CMTSWController()
{
    UnRegisterMsg();
    timerKillThreadTimer((UINT)&m_iTimerKeepAlive);
}


CMTSWController & CMTSWController::GetInstance()
{
    static CMTSWController s_obj;
    return s_obj;
}

void CMTSWController::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    SETTING_INFO("CMTSWController::OnTaskActionCallback");
    // 返回结果的类型为请求网络文件.
    CMTSWAction * pAction =
        dynamic_cast<CMTSWAction *>(pTaskAction);
    if (pAction == NULL)
    {
        return;
    }

    if (pAction->IsOperationSucceed())
    {
        pAction->GetActionCallback()->OnLoadSucProcess(pAction);
    }
    else
    {
        pAction->GetActionCallback()->OnRequestError(pAction->GetTaskActionErrorType(),
                pAction->GetMTSWActionType(), pAction->GetExtraParam());
        if (pAction->IsSessionExpired())
        {
            Login();
        }
    }

    if (!commonAPI_StringMatch(pAction->GetFilePath().c_str(), ".wav", false, TAIL_MATCH))
    {
        remove(pAction->GetFilePath().c_str());
    }
}


void CMTSWController::Init()
{
    RegisterMsg();

    if (!netIsNetworkReady())
    {
        SETTING_INFO("CMTSWController::Init network fail");
        timerSetThreadTimer((UINT)&m_NetTimer, 10 * 1000);
    }
    else
    {
        Login();
    }
}


int CMTSWController::Login()
{
    SETTING_INFO("CMTSWController::Login()");

    if ((configParser_GetConfigInt(kszMetaSwitchActive) == 0
            || configParser_GetConfigInt(kszMTSWCommportalEnable) == 0))
    {
        return 0;
    }

    if (m_eDownLoading == FILE_LOADING)
    {
        SETTING_INFO("CMTSWController::Login() is Loading");
        return 0;
    }

    char szBuf[1024] = {0};

    //从配置读取Server
    m_strCommprotalURL = configParser_GetConfigString(kszMTSWCommportalServer);

    if (m_strCommprotalURL.empty())
    {
        m_strSessionID.clear();
        SETTING_WARN("CommportalServer is NULL");
        return 0;
    }

    m_strSessionID.clear();
    timerKillThreadTimer((UINT)&m_iTimerKeepAlive);

    if (m_strCommprotalURL[m_strCommprotalURL.size() - 1] != '/')
    {
        m_strCommprotalURL.append("/");
    }

    sprintf(szBuf, "%s%s%s", m_strCommprotalURL.c_str(), "login?", kszVersion);

    //从配置读取账号、密码
    yl::string strUser = configParser_GetConfigString(kszMTSWCommportalUser);
    yl::string strPasswd = configParser_GetConfigString(kszMTSWCommportalPassword);

    yl::string strData = "DirectoryNumber=";

    strData.append(strUser.c_str());
    strData.append("&Password=");
    strData.append(strPasswd.c_str());

    yl::string strContentType = "application/x-www-form-urlencoded";

    SaveDataToFile(strData, kszTempFile);

    if (MyExecPostTaskAction(this, szBuf, strContentType.c_str(), MTSW_UT_Login, kszTempFile))
    {
        m_eDownLoading = FILE_LOADING;
        timerSetThreadTimer((UINT)&m_iTimerLoginTimeOut, m_iTimerLoginTimeOut * 1000);
    }
    else
    {
        m_eDownLoading = FILE_LOADING_FAIL;
    }

    return 0;
}


int CMTSWController::Logout()
{
    SETTING_INFO("CMTSWController::Logout()");

    CleanAllData();

    return 0;
}

int CMTSWController::KeepAlive()
{
    if (m_strSessionID.empty())
    {
        return 0;
    }

    yl::string strUrl = m_strCommprotalURL;
    strUrl += m_strSessionID;
    strUrl += "/keepAlive";

    MyExecDownLoadTaskAction(this, strUrl, MTSW_UT_KeepAlive, kszTempFile);

    return 0;
}


bool CMTSWController::SaveDataToFile(yl::string strData, yl::string strPath)
{
    FILE * fp;
    if (fp = fopen(strPath.c_str(), "wb"))
    {
        fputs(strData.c_str(), fp);
        fclose(fp);
    }
    return true;
}


void CMTSWController::CleanAllData()
{
    SETTING_INFO("CMTSWController::CleanAllData");
    m_strSessionID.clear();

    yl::string strServer =  configParser_GetConfigString(kszAutoProvisionUrl);

    int nIndex = strServer.find("token");

    if (nIndex != yl::string::npos)
    {
        SETTING_INFO("-------------Clear Before Url [%s]", strServer.c_str());
        //清除token部分信息
        strServer = strServer.substr(0, nIndex);
        SETTING_INFO("-------------Clear After Url [%s]", strServer.c_str());
        configParser_SetConfigString(kszAutoProvisionUrl, strServer.c_str());
    }

    yl::string strUser = configParser_GetConfigString(kszAutoProvisionUsername, CONFIG_LEVEL_FACTORY);
    yl::string strPassword = configParser_GetConfigString(kszAutoProvisionPassword,
                             CONFIG_LEVEL_FACTORY);
    int nPnp = configParser_GetConfigInt(kszAutoProvisionEnablePNP, CONFIG_LEVEL_FACTORY);
    int nDhcp = configParser_GetConfigInt(kszAutoProvisionEnableDHCPOption, CONFIG_LEVEL_FACTORY);
    if (strUser.empty())
    {
        strUser = kszYealink;
    }

    if (strPassword.empty())
    {
        strPassword = kszYealink;
    }
    configParser_SetConfigString(kszAutoProvisionUsername, strUser.c_str());
    configParser_SetConfigString(kszAutoProvisionPassword, strPassword.c_str());
    configParser_SetConfigInt(kszAutoProvisionEnablePNP, nPnp);
    configParser_SetConfigInt(kszAutoProvisionEnableDHCPOption, nDhcp);

    configParser_SetConfigString(kszMTSWCommportalServer, "");
    configParser_SetConfigString(kszMTSWCommportalUser, "");
    configParser_SetConfigString(kszMTSWCommportalPassword, "");

    //configParser_SetConfigInt(kszMTSWIsLogined, 0);
    //清除账号信息
    acc_EraseAllAccount();
    acc_EmptyAllAccount();


    //清除dsskey, exp与Programable Key
    dsskey_ResetModuleDataToFactory(DMT_LINEKEY_MODULE);
    dsskey_ResetModuleDataToFactory(DMT_PROGRAMEKEY_MODULE);

    DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(0);

    if (DT_INVALID != eType)
    {
        for (int i = 0; i < exp_GetExpCounts(DT_ALL); i++)
        {
            dsskey_ResetModuleDataToFactory((DSSKEY_MODULE_TYPE)eType, i);
        }
    }
}

void CMTSWController::RegisterMsg()
{
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, ProcessConfigChangeMsg);
    SingleMsgReg(TM_TIMER, ProcessOnTimer);
}

void CMTSWController::UnRegisterMsg()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, ProcessConfigChangeMsg);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, ProcessOnTimer);
}

void CMTSWController::SetKeepAliveTimer()
{
    m_iTimerKeepAlive = configParser_GetConfigInt(kszMTSWKeepAliveTimer);

    m_iTimerKeepAlive = m_iTimerKeepAlive * 60 * 1000;

    if (m_iTimerKeepAlive <= 0)
    {
        m_iTimerKeepAlive = 30 * 60 * 1000; //30min
    }

    timerSetThreadTimer((UINT)&m_iTimerKeepAlive, m_iTimerKeepAlive);
}

bool CMTSWController::OnTimer(UINT uTimerID)
{
    bool bHandled = false;

    if (uTimerID == (UINT)&m_iTimerKeepAlive)
    {
        KeepAlive();
        bHandled = true;
    }
    else if (uTimerID == (UINT)&m_NetTimer)
    {
        if (netIsNetworkReady())
        {
            SETTING_INFO("CMTSWController::OnTimer Network");
            timerKillThreadTimer((UINT)&m_NetTimer);
            Login();
            bHandled = true;
        }
    }
    else if (uTimerID == (UINT)&m_iTimerLoginFail)
    {
        timerKillThreadTimer((UINT)&m_iTimerLoginFail);
        Login();
        bHandled = true;
    }
    else if (uTimerID == (UINT)&m_iTimerLoginTimeOut)
    {
        timerKillThreadTimer((UINT)&m_iTimerLoginTimeOut);
        m_eDownLoading = FILE_LOADING_FAIL;
        bHandled = true;
    }

    return bHandled;
}

bool CMTSWController::ExecPost(CMTSWActionCmdBase * pCallback, yl::string strURL,
                               yl::string strData,  MtswActionType nType)
{
    if (m_strSessionID.empty()
            || pCallback == NULL
            || strURL.empty()
            || strData.empty())
    {
        return FALSE;
    }

    yl::string strUrl = m_strCommprotalURL;
    strUrl += m_strSessionID;
    strUrl += strURL.c_str();
    //strUrl += "&";
    strUrl += kszVersion;

    yl::string strHead = kszHead;

    yl::string strBody = kszBoundary;
    strBody.append(strData.c_str());
    strBody.append(kszBoundary);
    strBody.append("--");

    char szTemp[100];
    memset(szTemp, 0, 100);

    if (++m_iFileIndex == MAX_FILE_ID)
    {
        m_iFileIndex = 0;
    }

    sprintf(szTemp, "%s%d", kszTempFile, m_iFileIndex);

    SaveDataToFile(strBody.c_str(), szTemp);

    return MyExecPostTaskAction(pCallback, strUrl.c_str(), strHead.c_str(), nType, szTemp);
}

bool CMTSWController::ExecDownLoad(CMTSWActionCmdBase * pCallback, yl::string strURL,
                                   MtswActionType nType, yl::string strFilePath /*= ""*/, int nExParm/* = -1*/)
{
    if (m_strSessionID.empty()
            || pCallback == NULL
            || strURL.empty())
    {
        return FALSE;
    }

    yl::string strUrl = m_strCommprotalURL;
    strUrl += m_strSessionID;
    strUrl += strURL.c_str();
    strUrl += "&";
    strUrl += kszVersion;

    if (strFilePath.empty())
    {
        char szTemp[100];
        memset(szTemp, 0, 100);

        if (++m_iFileIndex == MAX_FILE_ID)
        {
            m_iFileIndex = 0;
        }

        sprintf(szTemp, "%s%d", kszTempFile, m_iFileIndex);

        strFilePath = szTemp;
    }

    return MyExecDownLoadTaskAction(pCallback, strUrl.c_str(), nType, strFilePath.c_str(), nExParm);
}

bool CMTSWController::MyExecDownLoadTaskAction(CMTSWActionCmdBase * pCallback, yl::string strURL,
        MtswActionType nType, yl::string strFilePath, int nExParm/* = -1*/)
{
    SETTING_INFO("CMTSWController::MyExecDownLoadTaskAction FilePath[%s] URL[%s] m_nMaxDownloadSize[%d]",
                 strFilePath.c_str(), strURL.c_str(), m_nMaxDownloadSize);

    CMTSWAction * pAction = new CMTSWAction(strFilePath.c_str(), strURL.c_str());

    if (pAction != NULL)
    {
        pAction->SetRead(true);
        pAction->SetSync(false);
        pAction->SetCallback(this);
        pAction->SetActionCallback(pCallback);
        pAction->SetMTSWActionType(nType);
        pAction->SetExtraParam(nExParm);
        pAction->SetMaxDownloadSize(m_nMaxDownloadSize);
        return TaskAction_ExecAction(pAction);
    }

    return 0;
}

bool CMTSWController::MyExecPostTaskAction(CMTSWActionCmdBase * pCallback, yl::string strURL,
        yl::string strHead, MtswActionType nType, yl::string strFilePath)
{
    SETTING_INFO("CMTSWController::MyExecPostTaskAction FilePath[%s] URL[%s]", strFilePath.c_str(),
                 strURL.c_str());

    CMTSWAction * pAction = new CMTSWAction(strFilePath.c_str(), strURL.c_str());

    if (pAction != NULL)
    {
        pAction->SetHeader(strHead.c_str());
        pAction->SetRead(false);
        pAction->SetSync(false);
        pAction->SetCallback(this);
        pAction->SetActionCallback(pCallback);
        pAction->SetMTSWActionType(nType);
        return TaskAction_ExecAction(pAction);
    }

    return false;
}

void CMTSWController::OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId)
{
    SETTING_INFO("CMTSWController::OnRequestError [%d][%d]", nType, eErrCode);

    if (nType == MTSW_UT_Login)
    {
        if (eErrCode == NS_TA::TA_HTTP_ERR408)
        {
            timerSetThreadTimer((UINT)&m_iTimerLoginFail, 60 * 1000);
        }

        if ((eErrCode == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
                || eErrCode == NS_TA::TA_HTTP_ERR403)
        {
            configParser_SetConfigString(kszMTSWCommportalPassword, "");
        }
        etl_NotifyApp(false, MTSW_MESSAGE_AUTH_ERR, 0, eErrCode);
        m_eDownLoading = FILE_LOADING_FAIL;

        timerKillThreadTimer((UINT)&m_iTimerLoginTimeOut);
    }
    else if (nType == MTSW_UT_KeepAlive)
    {
        if (eErrCode == NS_TA::TA_HTTP_ERR408)
        {
            Login();
        }
    }
}

bool CMTSWController::OnLoadSucProcess(CMTSWAction * pAction)
{
    SETTING_INFO("CMTSWController::OnMTSWActionResponse[%d]", pAction->GetMTSWActionType());

    if (pAction->GetMTSWActionType() == MTSW_UT_Login)
    {
        yl::string strData = yl::dump_file(pAction->GetFilePath());

        IDLE_INFO("strData = %s", strData.c_str());

        yl::string::iterator it = strData.begin();
        for (; it != strData.end(); it++)
        {
            if (*it == '=')
            {
                strData.erase(it);
                break;
            }
        }

        m_strSessionID = commonAPI_TrimString(strData, " ");

        IDLE_INFO("m_strSessionID = %s", m_strSessionID.c_str());

        //广播消息，通知其他模块更新数据
        etl_NotifyApp(false, MTSW_MESSAGE_LOGIN, 0, 0);
        SetKeepAliveTimer();

        m_eDownLoading = FILE_LOADING_SUC;
        timerKillThreadTimer((UINT)&m_iTimerLoginTimeOut);
    }
    else if (pAction->GetMTSWActionType() == MTSW_UT_KeepAlive)
    {
        Login();
    }

    return true;
}

void CMTSWController::FormatString(yl::string & strString)
{
    if (!strString.empty() && strString.substr(0, 1) == "?")
    {
        strString = strString.substr(1, strString.size() - 1);
    }
}

void CMTSWController::LoadMaxDownloadSize()
{
    int nSize = configParser_GetConfigInt(kszMaxDownloadSize);
    if (nSize < 1
            || nSize > 2 * 1024)
    {
        m_nMaxDownloadSize = 1024 * 1024;
    }
    else
    {
        m_nMaxDownloadSize = nSize * 1024;
    }

    IDLE_INFO("m_nMaxDownloadSize = %d", m_nMaxDownloadSize);
}

#endif
