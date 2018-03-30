#include "logic_inc.h"
#include "lamp/backlight/include/modbacklight.h"
#include "adapterbox/include/modadapterbox.h"
#include "extended/actionuri_url/include/action_def.h"
#include "talk/talklogic/include/callinfo.h"
#include "service_system.h"
#include "service_dms.h"
#ifdef IF_FEATURE_XMLBROWSER

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_GETINSTANCE(CXMLBrowserManager);

CXMLBrowserManager::CXMLBrowserManager()
{
    m_uPlayWavTimer = 0;
    m_nEnterDsskeyId = -1;
    m_bShowTips = true;
#if IF_BUG_32431
    vecURL.clear();
#endif
}

CXMLBrowserManager::~CXMLBrowserManager()
{
}

// 模块初始化
void CXMLBrowserManager::Init()
{

    // 注册主线程消息处理
    // 定时器处理
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, CXMLBrowserManager::OnTimer);
    // push xml消息
    etl_RegisterMsgHandle(DSK_MSG_PUSH_XML, DSK_MSG_PUSH_XML, CXMLBrowserManager::ProcessMSG);
    // sip notify消息
    etl_RegisterMsgHandle(SIP_SUB_HOT_DESKING_NOTIFY, SIP_SUB_HOT_DESKING_NOTIFY,
                          CXMLBrowserManager::ProcessMSG);

#if IF_FEATURE_XMLBROWSER_HOTELING
    etl_RegisterMsgHandle(SIP_SUB_UPDATE_HOTELING_STATUS, SIP_SUB_UPDATE_HOTELING_STATUS,
                          CXMLBrowserManager::ProcessMSG);
#endif

#if IF_BUG_30385
    etl_RegisterMsgHandle(DSK_MSG_REQUEST_XML, DSK_MSG_REQUEST_XML, CXMLBrowserManager::ProcessMSG);
#endif

#ifdef IF_FEATURE_DIAGNOSE
    etl_RegisterMsgHandle(DMS_MSG_NOTIFY_SHOW_MESSAGE, DMS_MSG_NOTIFY_SHOW_MESSAGE,
                          CXMLBrowserManager::ProcessMSG);

    // todo 暂时兼容旧版本的消息，V83提交之后删除
    etl_RegisterMsgHandle(DSK_MSG_SHOW_TEXT_INFO, DSK_MSG_SHOW_TEXT_INFO,
                          CXMLBrowserManager::ProcessMSG);
#endif // IF_FEATURE_DIAGNOSE

#if IF_FEATURE_XMLBROWSER_HOTELING
    m_bHotelingLogin = configParser_GetConfigInt(kszHotelingLoginStatus) != 0;
    if (configParser_GetConfigInt(kszHotelingPersistentLogin) == 0)
    {
        // 开机自动退出登录
        m_bHotelingLogin = false;
        timerSetThreadTimer((UINT)(&m_bHotelingLogin), 6000);
    }
    else if (m_bHotelingLogin)
    {
        timerSetThreadTimer((UINT)(&m_bHotelingLogin), 6000);
    }
#endif
}

// 入口
void CXMLBrowserManager::Enter(const char * lpszURL, bool bExitCurrent/* = true*/,
                               bool bShowTips /*= true*/)
{
    if (lpszURL == NULL)
    {
        return;
    }

    // 判断是否允许显示XML
    if (!IsAllowShowXML())
    {
        return;
    }

    m_bShowTips = bShowTips;

    // 开始下载文件并解析文件
    yl::string strURL = lpszURL;
    MyReplaceVar(strURL);
    int iDsskeyId = GetEnterDsskeyId();

    bool bsuc = _ElementManager.LoadElement(strURL.c_str(), OPT_DOWNLOAD | OPT_XML_PARSE, bExitCurrent);

    if (bsuc)
    {
        dsskey_SetDsskeyStatus(iDsskeyId, DS_XMLBROSER_LOADING);
        CloseLoading();
        if (bShowTips)
        {
            int iTipDelay = configParser_GetConfigInt(kszXMLLoadingTipDelay);
            if (iTipDelay > 0)
            {
                timerSetThreadTimer(TIMER_ID(m_bShowTips), iTipDelay);
            }
        }
    }
}

// 整个XMLBrowser退出，逻辑对象都会被释放
void CXMLBrowserManager::Exit()
{
    // 退出
    DeleteAllElement();

    // http://10.2.1.199/Bug.php?BugID=38030

    AdapterBox_XmlBrowserExitXBDlg(INVALID_ELEMENT_ID, false);

    // 更新
    AdapterBox_XmlBrowserUpdateWnd();
}

// 定时器响应
LRESULT CXMLBrowserManager::OnTimer(msgObject & objMsg)
{
    if (objMsg.wParam == TIMER_ID(_XMLBrowserManager.m_uPlayWavTimer))
    {
        timerKillThreadTimer(objMsg.wParam);
        _XMLBrowserManager.StopWav();
        return TRUE;
    }
#if IF_FEATURE_XMLBROWSER_HOTELING
    else if (objMsg.wParam == TIMER_ID(_XMLBrowserManager.m_bHotelingLogin))
    {
        // 网络异常, 不处理
        if (!netIsNetworkReady())
        {
            return TRUE;
        }

        if (talklogic_SessionExist())
        {
            return TRUE;
        }

        timerKillThreadTimer(objMsg.wParam);

        _XMLBrowserManager.HotelingRequest(_XMLBrowserManager.m_bHotelingLogin);

        return TRUE;
    }
#endif
    else if (objMsg.wParam == TIMER_ID(_XMLBrowserManager.m_bShowTips))
    {
        timerKillThreadTimer(objMsg.wParam);
        // 弹出提示框
        _XMLBrowserManager.ShowMessageBox(XB_TIP_LOADING, 0);
    }

#if IF_BUG_32431
    if (_XMLBrowserManager.RefreshURL(objMsg.wParam))
    {
        return TRUE;
    }
#endif

#if IF_BUG_25845
    CElementConfig::OnTimer(objMsg.wParam);
#endif

    return CElementStatus::OnTimer(objMsg.wParam);
}

// 获取当前数据及界面类型
bool CXMLBrowserManager::GetData(XMLLogicToUIData & refToUIData)
{
    // 判断是否允许显示XML
    if (!IsAllowShowXML())
    {
        return false;
    }
    return _ElementManager.GetToUIData(refToUIData);
}

// 对UI事件的处理
bool CXMLBrowserManager::OnAction(int nElementID, XB_ACTION_TYPE eAction, void * pData/* = NULL*/)
{
    bool bHandled = true;
    switch (eAction)
    {
    case XB_ACTION_EXIT_ALL:
        {
            Exit();
        }
        break;
    default:
        {
            // 除退出所有Element以外的Action都传给当前Element来处理
            bHandled = _ElementManager.OnAction(nElementID, eAction, pData);
        }
        break;
    }
    return bHandled;
}

// 提示信息
void CXMLBrowserManager::ShowMessageBox(XB_TIP_TYPE eType, UINT uTimeout)
{
    if (!m_bShowTips)
    {
        return;
    }

    CloseLoading();
    AdapterBox_XmlBrowserShowMessage(eType, uTimeout);
}

#if IF_FEATURE_XMLBROWSER_HOTELING
void CXMLBrowserManager::ShowTips(const yl::string & strMessage, UINT uTimeout)
{
    AdapterBox_XmlBrowserShowTips(strMessage, uTimeout);
}

bool CXMLBrowserManager::HotelingRequest(bool bLogin)
{
    yl::string strHotelingServer = configParser_GetConfigString(kszHotelingServer);
    XMLBROWSER_INFO("HotelingRequest[%d] strHotelingServer[%s]", bLogin, strHotelingServer.c_str());
    if (strHotelingServer.empty())
    {
        return false;
    }

    m_bHotelingLogin = bLogin;

    if (talklogic_SessionExist())
    {
        XMLBROWSER_INFO("Talk Session Exist Set Timer");
        timerSetThreadTimer((UINT)(&m_bHotelingLogin), 2000);
        return true;
    }

    if (bLogin)
    {
        strHotelingServer.append("?login");
    }
    else
    {
        strHotelingServer.append("?logout");
    }

    MyReplaceVar(strHotelingServer);
    return _ElementManager.LoadElement(strHotelingServer.c_str(),
                                       OPT_DOWNLOAD | OPT_XML_PARSE | OPT_HOTELING, true);
}
#endif

// 取消提示框
void CXMLBrowserManager::ExitMessageBox()
{
    AdapterBox_XmlBrowserDestroyMessage();
}

/*
<?xml version="1.0" encoding="ISO-8859-1"?>
<YealinkIPPhoneTextScreen
Beep="yes"
doneAction="http://10.3.3.235/TestStatus.xml"
wrapList="yes"
Timeout="50"
LockIn="yes">
<Title wrap="yes">TextScreen This interface is just used to Test TextScreen Object</Title>
<Text>Yealink T-38 Xml Browser</Text>
</YealinkIPPhoneTextScreen>
*/

#ifdef IF_FEATURE_DIAGNOSE
bool TransferXmlFile(const yl::string & strPath, const yl::string & strText, int nTime)
{
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    xml_node nodeRootNode = doc.append_child("YealinkIPPhoneTextScreen");
    nodeRootNode.append_attribute("Beep", "no");

    nodeRootNode.append_attribute("wrapList", "yes");

    yl::string strTmp = commonAPI_FormatString("%d", nTime);
    nodeRootNode.append_attribute("Timeout", strTmp.c_str());

    xml_node nodeTmp = nodeRootNode.append_child("Text");
    nodeTmp.text().set(strText.c_str());

    doc.save_file(strPath.c_str());
    return true;
}
#endif // IF_FEATURE_DIAGNOSE

// 处理主线程消息
LRESULT CXMLBrowserManager::ProcessMSG(msgObject & msg)
{
    XMLBROWSER_INFO("ProcessMSG msg(%d)", msg.message);
    switch (msg.message)
    {
    case DSK_MSG_PUSH_XML:
        {
            // exit others
            //_XMLBrowserManager.Exit();
            if (!_XMLBrowserManager.IsAllowShowXML())
            {
                return -1;
            }

            // 亮背光灯
            backlight_Reset();

            yl::string strFilename = xGetUniqueLoadName();
            rename(szXmlBrowser, strFilename.c_str());
            XMLBROWSER_INFO("Rename file (%s)to(%s)", szXmlBrowser, strFilename.c_str());

            // load
            _ElementManager.LoadElement(strFilename.c_str(), OPT_XML_PARSE, true);

            return 1;
        }
        break;
#ifdef IF_FEATURE_DIAGNOSE
    case DSK_MSG_SHOW_TEXT_INFO:
    case DMS_MSG_NOTIFY_SHOW_MESSAGE:
        {
            // 通过现成的xml界面来显示设备管理下发的内容
            if (msg.GetExtraData() == NULL)
            {
                XMLBROWSER_INFO("Null data");
                return -1;
            }

            yl::string strFilename = xGetUniqueLoadName();
            //printf("---[%s]", (char*)msg.GetExtraData());
            if (TransferXmlFile(strFilename, (char *)msg.GetExtraData(), msg.wParam))
            {
                // 亮背光灯
                backlight_Reset();

                XMLBROWSER_INFO("file(%s) [%s]", strFilename.c_str(), (char *)msg.GetExtraData());

                // load
                _ElementManager.LoadElement(strFilename.c_str(), OPT_XML_PARSE, true);
            }

            return 1;
        }
        break;
#endif // IF_FEATURE_DIAGNOSE
    case SIP_SUB_HOT_DESKING_NOTIFY:
        {
            // 判断是否开启了sip notify功能
            if (configParser_GetConfigInt(kszEnableSIPNotifyPushXML) != 1)
            {
                XMLBROWSER_WARN("not accept sip notify.");
                return -1;
            }

            // 将数据写入文件
            char * pXMLData = (char *)msg.GetExtraData();
            if (NULL == pXMLData)
            {
                return -1;
            }

            // exit others
            //_XMLBrowserManager.Exit();

            // 亮背光灯
            backlight_Reset();

            yl::string strFilename = xGetUniqueLoadName();
            // 写入长度根据消息中Extradata的大小来确定
            etlWriteBinFile(strFilename.c_str(), pXMLData, msg.GetExtraSize());

            // load
            _ElementManager.LoadElement(strFilename.c_str(), OPT_XML_PARSE, true);

            return 1;
        }
        break;
    case SYS_MSG_NOTIFY_PHY_STATUS:
        {
            if (msg.lParam == SYS_NET_STATUS_OFF)
            {
                // 取消下载
                _XMLBrowserManager.CancelDownLoad();
                // 提示网络异常
                _XMLBrowserManager.ShowMessageBox(XB_TIP_NET_UNAVAILABLE, 3000);
            }
        }
        break;
#if IF_BUG_30385
    case DSK_MSG_REQUEST_XML:
        {
            yl::string strUrl = configParser_GetConfigString(kszXMLActionURL);
            _ElementManager.LoadElement(strUrl.c_str(), OPT_DOWNLOAD | OPT_XML_PARSE, false);
            configParser_SetConfigInt(kszActionURLFlag, 0);
        }
        break;
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
    case SIP_SUB_UPDATE_HOTELING_STATUS:
        {
            if (msg.lParam == SIP_HOTELING_REQUIRE_LOGOUT)
            {
                _XMLBrowserManager.HotelingRequest(false);
            }
        }
        break;
#endif
    default:
        break;
    }

    return 0;
}

// 是否允许显示XML
bool CXMLBrowserManager::IsAllowShowXML()
{
    // 有来电，或者 有通话存在且开启了BlockInCalling，则不处理XML
    // http://10.2.1.199/Bug.php?BugID=30041
    if (talklogic_SessionExist()
            && configParser_GetConfigInt(kszPushXMLBlockInCalling) == 1)
    {
        return false;
    }

    return true;
}

// 更新界面
void CXMLBrowserManager::UpdateWnd()
{
    if (!IsAllowShowXML())
    {
        XMLBROWSER_WARN("no allow to display !");
        return;
    }

    AdapterBox_XmlBrowserUpdateWnd();
}

// 清除所有Element
void CXMLBrowserManager::DeleteAllElement()
{
    _ElementManager.DeleteAllElement();
}

// 取消下载
void CXMLBrowserManager::CancelDownLoad()
{
    _XMLBrowserLoadMng.CancelLoad();
}

// 删除逻辑对象的事件响应，可用于通知UI
void CXMLBrowserManager::OnDeleteElement(int nElement)
{
    AdapterBox_XmlBrowserExitXBDlg(nElement, false);
}

//  播放提示音
void CXMLBrowserManager::PlayBeep()
{
    // 如果存在通话则不播放beep音，否则会引发通话中播放拨号音、铃声等等一些问题
    if (!talklogic_SessionExist())
    {
        XMLBROWSER_INFO("PlayBeep");
        voice_PlayTone(TONE_CALLWAIT1, 500, CHANNEL_RING/*CHANNEL_HANDFREE*/);
        // voice_StopTone(TONE_STUTTER);
        // http://10.2.1.199/Bug.php?BugID=37654
        // 保证提示音可以播放完全，做延时处理
        etlSleep(500);
    }
}

// 停止提示音
void CXMLBrowserManager::StopBeep()
{
    XMLBROWSER_INFO("StopBeep");
    voice_StopTone(TONE_CALLWAIT1);
}

// 获取文件大小
LRESULT GetFileSize(LPCSTR FileName)
{
    struct stat buf;
    if (stat(FileName, &buf) < 0)
    {
        return 0;
    }
    return (LRESULT)buf.st_size;
}

//  播放提示音
void CXMLBrowserManager::PlayWav(const yl::string & strWavPath)
{
    // 如果存在通话则不播放声音
    if (!talklogic_SessionExist())
    {
        // 先停止掉上一次未播放完的铃声
        timerKillThreadTimer((UINT)(&m_uPlayWavTimer));
        StopWav();

        // 播放新的铃声
        XMLBROWSER_INFO("PlayWav(%s)", strWavPath.c_str());
        voice_PlayRingFromFilePath(strWavPath.c_str());
        m_strWavFile = strWavPath;

        LRESULT ulFileSize = GetFileSize(strWavPath.c_str());
        ulFileSize = ulFileSize >> 3;
        timerSetThreadTimer((UINT)(&m_uPlayWavTimer), (UINT)ulFileSize + 1);
    }
    else
    {
        remove(strWavPath.c_str());
    }
}

void CXMLBrowserManager::StopWav()
{
    XMLBROWSER_INFO("StopWav");
    voice_StopRingInMenu();

    if (!m_strWavFile.empty())
    {
        remove(m_strWavFile.c_str());
        m_strWavFile.clear();
    }
}

void CXMLBrowserManager::MyReplaceVar(yl::string & strInput)
{
    int iAccount = talklogic_GetSessionAccount();
    if (iAccount == -1)
    {
        iAccount = acc_GetDefaultAccount();
    }

    yl::string strIP;
    netGetWanIP(strIP);

    yl::string strMAC = netGetMACText('\0');

    yl::string strSIPURLUser = acc_GetUsername(iAccount);
    yl::string strSIPURLHost = acc_GetServerName(iAccount);
    yl::string strSIPURL = "sip:" + strSIPURLUser + "@" + strSIPURLHost;

    // get phone type from cfg file
    char bufProductName[512] = {0};
    sys_replace_wildcards((char *)configParser_GetConfigString(kszProductName).c_str(), 0,
                          bufProductName, sizeof(bufProductName) / sizeof(char));
    yl::string strPhoneType = bufProductName;

    // get firmware from cfg file
    // cfgserver 不能使用configParser获取这个配置，统一使用sys服务获取
    char szFirmware[64] = { 0 };
    yl::string strFirmware = "";
    if (0 == sys_get_sw_version(szFirmware, 64))
    {
        strFirmware = szFirmware;
    }


    XMLBROWSER_INFO("Replace before Url [%s]", strInput.c_str());
    string_replace(strInput, VAR_MAC, strMAC);
    string_replace(strInput, VAR_IP, strIP);
    string_replace(strInput, VAR_ACTIVE_URL, strSIPURL);
    string_replace(strInput, VAR_ACTIVE_USER, strSIPURLUser);
    string_replace(strInput, VAR_ACTIVE_HOST, strSIPURLHost);
    string_replace(strInput, VAR_PHONE_TYPE, strPhoneType);
    string_replace(strInput, VAR_FIRMWARE, strFirmware);

    SetTalkInfoURL(strInput);
    XMLBROWSER_INFO("Replace after Url [%s]", strInput.c_str());
}

#if IF_BUG_30385
void CXMLBrowserManager::RequestAutopXml()
{
    XMLBROWSER_INFO("CXMLBrowserManager::RequestAutopXml");
    if (configParser_GetConfigInt(kszActionURLFlag) == 1)
    {
        yl::string strUrl = configParser_GetConfigString(kszXMLActionURL);
        _ElementManager.LoadElement(strUrl.c_str(), OPT_DOWNLOAD | OPT_XML_PARSE, false);
        configParser_SetConfigInt(kszActionURLFlag, 0);
    }
}
#endif

bool CXMLBrowserManager::ParseFile(const yl::string & strFilename)
{
    return _ElementManager.LoadElement(strFilename.c_str(), OPT_XML_PARSE, true);
}

void CXMLBrowserManager::CloseLoading()
{
    timerKillThreadTimer(TIMER_ID(m_bShowTips));
}

#if IF_BUG_32431
bool CXMLBrowserManager::SetAutoRefresh(const int & nTimeOut, const yl::string & strURL)
{
    XMLBROWSER_INFO("CXMLBrowserManager::SetAutoRefresh  nTimeOut[%d] strURL[%s]", nTimeOut,
                    strURL.c_str());
    if (nTimeOut <= 0
            || strURL.empty())
    {
        return false;
    }

    int nIndex = GetURLIndex(strURL);
    if (nIndex != -1)
    {
        timerKillThreadTimer(TIMER_ID(vecURL[nIndex]));
        timerSetThreadTimer(TIMER_ID(vecURL[nIndex]), nTimeOut * 1000 - 500);
    }
    else
    {
        vecURL.push_back(strURL);
        timerSetThreadTimer(TIMER_ID(vecURL[vecURL.size() - 1]), nTimeOut * 1000 - 500);
    }
    return true;
}

bool CXMLBrowserManager::RefreshURL(UINT uTimerID)
{
    if (vecURL.size() == 0)
    {
        return false;
    }

    for (int nIndex = 0; nIndex < vecURL.size(); nIndex++)
    {
        if (uTimerID != TIMER_ID(vecURL[nIndex]))
        {
            continue;
        }

        timerKillThreadTimer(uTimerID);

        if (!IsAllowShowXML()
                || !netIsNetworkReady())
        {
            timerSetThreadTimer(TIMER_ID(vecURL[nIndex]), 2500);
            return true;
        }

        XMLBROWSER_INFO("CXMLBrowserManager::RefreshURL vecURL[%d] URL[%s] ID[%d]", nIndex,
                        vecURL[nIndex].c_str(), TIMER_ID(vecURL[nIndex]));
        Enter(vecURL[nIndex].c_str(), true, false);

        vecURL.removeAt(nIndex);

        return true;
    }

    return false;
}

int CXMLBrowserManager::GetURLIndex(const yl::string & strURL)
{
    if (vecURL.size() == 0)
    {
        return -1;
    }

    for (int nIndex = 0; nIndex < vecURL.size(); nIndex++)
    {
        if (strURL == vecURL[nIndex])
        {
            return nIndex;
        }
    }

    return -1;
}

bool CXMLBrowserManager::StopAutoRefresh(const yl::string & strURL)
{
    if (strURL.empty())
    {
        return false;
    }

    int nIndex = GetURLIndex(strURL);
    if (nIndex != -1)
    {
        timerKillThreadTimer(TIMER_ID(vecURL[nIndex]));
        return true;
    }

    return false;
}
#endif

bool CXMLBrowserManager::SetTalkInfoURL(yl::string & strURL)
{
    if (!talklogic_SessionExist())
    {
        return false;
    }

    int nSessionID = talklogic_GetFocusedSessionID();
    const CCallInfo * pCallInfo = talklogic_GetCallInfoBySessionID(nSessionID);
    if (NULL == pCallInfo)
    {
        return false;
    }

    yl::string strUserName = acc_GetUsername(pCallInfo->GetAccountID());
    yl::string strRemoteNum = pCallInfo->m_tRemoteInfo.sSIPName;

    string_replace(strURL, XML_USER_NAME, strUserName);
    string_replace(strURL, XML_REMOTE_NUM, strRemoteNum);

    return true;
}

#if IF_FEATURE_XMLB_SCREENSAVER
// 获取xml屏保数据
bool CXMLBrowserManager::GetSSData(XMLLogicToUIData & objToUIData)
{
    // 判断是否允许显示XML
    if (!IsAllowShowXML())
    {
        return false;
    }
    return _ElementManager.GetSSData(objToUIData);
}

// 清除xml屏保缓存数据
void CXMLBrowserManager::DeleteSSData()
{
    int nElementID = INVALID_ELEMENT_ID;
    XMLLogicToUIData objToUIData;

    _ElementManager.GetSSData(objToUIData);
    nElementID = objToUIData.nElementID;

    if (INVALID_ELEMENT_ID == nElementID)
    {
        return;
    }

    _ElementManager.DeleteSSElement();
    OnDeleteElement(nElementID);
}
#endif //#if IF_FEATURE_XMLB_SCREENSAVER

#endif // #ifdef IF_FEATURE_XMLBROWSER
