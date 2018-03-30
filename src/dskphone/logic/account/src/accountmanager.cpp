#include "account_inc.h"

#include "accountmanager.h"
#include "modaccount.h"
#include "voip/modvoip.h"
#include "adapterbox/include/modadapterbox.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "idlescreen/include/modidlescreen.h"
#include "commonunits/commonunits_def.h"
#include "cloudaccountinfomanager.h"
#include "blf/include/modblf.h"
#ifdef IF_FEATURE_PSTN
#include "pstnadapter/include/modpstnadapter.h"
#endif // IF_FEATURE_PSTN
#include "service_h323_define.h"
#include "service_h323_message.h"

#define WEB2PHONE_QUERY_ACCOUNT_STATE 0x30006
#define WEB2PHONE_QUERY_ACCOUNT_ALLSTATE 0x30005
#define WEB2PHONE_QUERY_ACCOUNT_REGISTER_CODE 0x20000


//帐号消息回调函数
LRESULT OnAccountManagerMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case TM_TIMER:
        {
            return _AccountManager.OnTimer(objMessage.wParam);
        }
        break;
    case SIP_REG_UPDATE_REGISTER:
        {
            //帐号改变消息
            return _AccountManager.OnLineStateChange(objMessage);
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            //帐号配置改变消息
            if (objMessage.wParam == ST_SIPLINE)
            {
                return _AccountManager.OnLineConfigChange(objMessage);
            }
            else if (ST_DEFAULT_ACCOUNT == objMessage.wParam)
            {
                int iOldDefAcc = _AccountManager.GetDefaultAccountID();
                _AccountManager.NotifyDefaultAccountChanged(iOldDefAcc);
                break;
            }
            else if (ST_AUTOANSWER == objMessage.wParam)
            {
                _AccountManager.SetAutoAnswer();
            }
#ifdef IF_FEATURE_PSTN
            else if (ST_PSTN_LINE == objMessage.wParam)
            {
                return etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE,
                                     objMessage.lParam + PSTN_LINE_BEGIN_ID);
            }
#endif // IF_FEATURE_PSTN
            else if (ST_CLOUD_CONFIG == objMessage.wParam)
            {
#ifdef IF_FEATURE_CLOUD
                _CloudAccountInfoManager.ProcessCloudLoginMsg(objMessage);
#endif //#ifdef IF_FEATURE_CLOUD
#ifdef IF_FEATURE_H323
                if (objMessage.lParam == 1)
                {
                    // lParam = 1表示VC account配置变化
                    _AccountManager.OnH323ConfigChanged(objMessage);
                }
#endif // #ifdef IF_FEATURE_H323
            }
#ifdef IF_FEATURE_H323
            else if (ST_H323LINE == objMessage.wParam)
            {
                return _AccountManager.OnH323ConfigChanged(objMessage);
            }
#endif // #ifdef IF_FEATURE_H323
        }
        break;
#ifndef _T49
    case SIP_MSG_UPDATE_MWI_INFO:
        {
            //语音留言信息
            return _AccountManager.OnNewVoiceMessage(objMessage);
        }
        break;
#endif
    case WEB2PHONE_QUERY_ACCOUNT_ALLSTATE:
        {
#define WEBQUERY_ACCOUNTSTATE_FILE  TEMP_PATH "tmpAllAccountState"
            // 网页查询所有账号的状态,将状态用逗号分隔写入文件中
            int iAccountNum = _AccountManager.GetAccountNum();
            yl::string strAccountState = "";
            for (int i = 0; i < iAccountNum; i++)
            {
                CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(i);
                if (pAccount)
                {
                    strAccountState += commonAPI_FormatString("%d", pAccount->GetAccountState());
                }

                // 最后一个不加&
                if (i != iAccountNum - 1)
                {
                    strAccountState += ",";
                }
            }

            // 写入文件
            //etlSetProfileString(WEBQUERY_ACCOUNTSTATE_FILE, "Account", "strAllState", strAccountState.c_str());
            configParser_SetConfigString(kszAllAccountState, strAccountState.c_str(), CONFIG_LEVEL_PHONE);
            objMessage.ReplyMessage(1);
            return 1;
        }
        break;
    case DSK_MSG_GETLINESTATUS:
        {
            ACCOUNT_INFO("Web get line[%d] status type[%d]", objMessage.wParam, objMessage.lParam);
            //查询账号状态（Web）
            LRESULT rePly = -1L;
            CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(objMessage.wParam, false);
            if (pAccount == NULL)
            {
                ACCOUNT_ERR("Not find account");
                objMessage.ReplyMessage(LS_UNKNOW);
                return LS_UNKNOW;
            }

            // 查询账号状态
            if (0 == objMessage.lParam)
            {
                LINE_STATE eStatus = pAccount->GetAccountState();
                //LS_BE_UNREGISTERED为中间状态,回复为LS_REGISTERING
                if (eStatus != LS_BE_UNREGISTERED
                        || pAccount->GetAccountType() == ACCOUNT_TYPE_H323)
                {
                    // H323 要显示未注册 http://10.2.1.199/Bug.php?BugID=93379
                    rePly = eStatus;
                }
                else
                {
                    rePly = LS_REGISTERING;
                }
            }
            // 获取账号注册的服务器序号:目前只支持sip账号，h323底层不支持
            else if (1 == objMessage.lParam)
            {
                //CSipAccount *pSipAccount = dynamic_cast<CSipAccount*>(pAccount);
                /*if (NULL == pAccount)
                {
                    objMessage.ReplyMessage(LS_UNKNOW);
                    return LS_UNKNOW;
                }*/

                rePly = ACCOUNT_REGISTERED_SERVER_UNKOWN;
                yl::string strSipServer1 = configParser_GetCfgItemStringValue(kszAccountServerAddr,
                                           objMessage.wParam, 1);
                yl::string strSipServer2 = configParser_GetCfgItemStringValue(kszAccountServerAddr,
                                           objMessage.wParam, 2);
                if (strcmp(pAccount->GetServerName().c_str(), strSipServer1.c_str()) == 0)
                {
                    rePly = ACCOUNT_REGISTERED_SERVER_1;
                }
                else if (strcmp(pAccount->GetServerName().c_str(), strSipServer2.c_str()) == 0)
                {
                    rePly = ACCOUNT_REGISTERED_SERVER_2;
                }
            }
            // 查询账号注册原因
            else if (2 == objMessage.lParam)
            {
                /*CSipAccount *pSipAccount = dynamic_cast<CSipAccount*>(pAccount);
                if (NULL == pSipAccount)
                {
                    objMessage.ReplyMessage(LS_UNKNOW);
                    return LS_UNKNOW;
                }*/

                rePly = WEB2PHONE_QUERY_ACCOUNT_REGISTER_CODE + pAccount->GetRegisterFailId();
            }

            objMessage.ReplyMessage(rePly);
        }
        break;
#ifdef IF_FEATURE_PSTN
    case DSK_MSG_GET_PSTN_LINE_STATUS:
        {
            // 查询PSTN账号状态（Web）
            LRESULT rePly = -1L;
            CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(objMessage.wParam);
            if (pAccount == NULL)
            {
                // 找不到账号（未插PSTN线）则暂且视为LS_REGISTER_FAIL注册失败。Web端把LS_UNKNOW视为注册中的状态。
                objMessage.ReplyMessage(LS_REGISTER_FAIL);
                return LS_REGISTER_FAIL;
            }

            // 查询账号状态
            if (0 == objMessage.lParam)
            {
                rePly = pAccount->GetAccountState();
            }
            else
            {
                ACCOUNT_WARN("Unknown message param: [%d]lParam=%d", objMessage.message, objMessage.lParam);
            }

            _AccountManager.PrintAllAccountInfo();

            objMessage.ReplyMessage(rePly);
        }
        break;
#endif // IF_FEATURE_PSTN
    case SYS_MSG_NOTIFY_NETWORK_STATUS:
    case SYS_MSG_NOTIFY_PHY_STATUS:
        {
            //网络状态改变，包括网线拔插、IP改变等
            bool bRet = _AccountManager.OnNetworkStateChange(objMessage);
#if IF_FEATURE_CLOUD_ACCOUNT
            bRet |= _CloudAccountInfoManager.OnNetworkStateChange();
#endif
            return bRet;
        }
        break;
    //SIP所发送的消息，动态改变服务器
    case SIP_REG_UPDATE_SIP_SERVER:
        {
            int iServerID = objMessage.lParam + 1;
            int iAccount = objMessage.wParam;
            acc_SetServerByServerID(iAccount, iServerID);
        }
        break;
    case SIP_SUB_UPDATE_DIAL_TONE:
        {
            ACCOUNT_INFO("SIP_SUB_UPDATE_DIAL_TONE Line[%d] Tone[%d]", objMessage.wParam, objMessage.lParam);
            configParser_SetCfgItemIntValue(kszAccountDialTone, objMessage.wParam, objMessage.lParam);
        }
        break;
    case SIP_CMD_RELOAD_ACCOUNT:
        {
            int iAccount = objMessage.wParam;
            ACCOUNT_INFO("SIP_CMD_RELOAD_ACCOUNT Line[%d]", iAccount);
            acc_SetServerByServerID(iAccount, -1);
            // http://10.3.5.199/Bug.php?BugID=60910
            // 通话中不处理，否则给sip发配置改变的消息,账号模块可不用重新加载
            if (!talklogic_SessionExist())
            {
                msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, objMessage.wParam);
            }
        }
        break;
    case SIP_REG_TRANSPORT:
        {
            ACCOUNT_INFO("SIP_REG_TRANSPORT Line[%d] Status[%d]", objMessage.wParam, objMessage.lParam);
            _AccountManager.SetTransPort(objMessage);
        }
        break;
#ifdef IF_FEATURE_PSTN
    case PSTN_MSG_LINE_CHANGED:
        {
            return _AccountManager.OnPstnLineStateChange(objMessage);
        }
        break;
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
    case H323_REG_UPDATE_STATUS:
        {
            //帐号改变消息
            return _AccountManager.OnH323LineStateChange(objMessage);
        }
        break;
#endif //IF_FEATURE_H323
    default:
        break;
    }
    return 0;
}

CAccountManager::CAccountManager()
{
    m_listAccount.clear();
    m_iDefaultAccountId = 0;
    // 要包括IP直拨的账号
    m_uMaxLineCount = acc_AccountNum(ACCOUNT_TYPE_SIP | ACCOUNT_TYPE_IP_DIRECT);
#ifndef _T49
    m_iLastVoiceMailID = -1;
#endif
    m_nUsableAccount = -1;
    m_iCfgStatus = -1;
    m_iCheckLogOutTime = -1;
    m_bLastNetworkReady = netIsNetworkReady();
}

CAccountManager::~CAccountManager()
{
}

bool CAccountManager::InitializeObject()
{
    RegisterMsg();
    //读取系统默认帐号,???
    // http://10.2.1.199/Bug.php?BugID=18936
    m_iDefaultAccountId = configParser_GetConfigInt(kszAccountDefaultID);
    ACCOUNT_INFO("Default Account(%d), Max Account Count(%d)", m_iDefaultAccountId, m_uMaxLineCount);

    // 自动配置账号
    AutoSetAccount();

    //读取所有帐号
    LoadAllLineAccount();
    //刷新所有帐号状态
    RefreshAllLineState();
    return true;
}

void CAccountManager::UninitializeObject()
{
}

//设置监控接口
void CAccountManager::RegisterMsg()
{
    //修改账号拨号音
    etl_RegisterMsgHandle(SIP_SUB_UPDATE_DIAL_TONE, SIP_SUB_UPDATE_DIAL_TONE, OnAccountManagerMsg);
    //监视帐号改变与配置改变消息
    etl_RegisterMsgHandle(SIP_REG_UPDATE_REGISTER, SIP_REG_UPDATE_REGISTER, OnAccountManagerMsg);

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnAccountManagerMsg);
    //监视语音留言信息
#ifndef _T49
    etl_RegisterMsgHandle(SIP_MSG_UPDATE_MWI_INFO, SIP_MSG_UPDATE_MWI_INFO, OnAccountManagerMsg);
#endif
    //监视网页询问账号信息
    etl_RegisterMsgHandle(DSK_MSG_GETLINESTATUS, DSK_MSG_GETLINESTATUS, OnAccountManagerMsg);
#ifdef IF_FEATURE_PSTN
    //监视网页询问PSTN账号信息
    etl_RegisterMsgHandle(DSK_MSG_GET_PSTN_LINE_STATUS, DSK_MSG_GET_PSTN_LINE_STATUS,
                          OnAccountManagerMsg);
#endif // IF_FEATURE_PSTN
    //监视网络状态改变消息
    etl_RegisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_NETWORK_STATUS,
                          OnAccountManagerMsg);
    //SIP服务器改变消息
    etl_RegisterMsgHandle(SIP_REG_UPDATE_SIP_SERVER, SIP_REG_UPDATE_SIP_SERVER, OnAccountManagerMsg);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnAccountManagerMsg);
    // sip重新加载账号
    etl_RegisterMsgHandle(SIP_CMD_RELOAD_ACCOUNT, SIP_CMD_RELOAD_ACCOUNT, OnAccountManagerMsg);
#ifdef IF_FEATURE_PSTN
    // PSTN线路状态变化消息
    etl_RegisterMsgHandle(PSTN_MSG_LINE_CHANGED, PSTN_MSG_LINE_CHANGED, OnAccountManagerMsg);
#endif // IF_FEATURE_PSTN
    etl_RegisterMsgHandle(SIP_REG_TRANSPORT, SIP_REG_TRANSPORT, OnAccountManagerMsg);

#ifdef IF_FEATURE_H323
    //////////////////////////////////H323////////////////////////////////////////
    etl_RegisterMsgHandle(H323_REG_UPDATE_STATUS, H323_REG_UPDATE_STATUS, OnAccountManagerMsg);
#endif //#ifdef IF_FEATURE_H323

}

//打印所有帐号信息
void CAccountManager::PrintAllAccountInfo()
{
    for (CAccountList::iterator itAccount = m_listAccount.begin(); itAccount != m_listAccount.end();
            ++itAccount)
    {
        CBaseAccount * pAccount = *itAccount;
        if (pAccount)
        {
            pAccount->PrintAccoutInfo();
        }
    }
}

//刷新所有帐号状态
void CAccountManager::RefreshAllLineState()
{
    // 上电初始化以及网络状态变化才会进行刷新所有账号
    // 上电、网络状态账号变化时，需要标识该次账号状态变化信息是由gui主动查询获取的(非sip发送)
    for (CAccountList::iterator itAccount = m_listAccount.begin(); itAccount != m_listAccount.end();
            ++itAccount)
    {
        CBaseAccount * pAccount = *itAccount;
        if (pAccount && pAccount->IsDirectIPAccount())
        {
            continue;
        }

        account_register_reason_t sReason;
        //刷新对应帐号信息
        LineStateChange((*itAccount)->GetAccountId(), modVoip_QueryLineState((*itAccount)->GetAccountId()),
                        sReason, true);
    }
}

//根据帐号Id刷新帐号状态
void CAccountManager::RefreshLineStateById(int iAccountId)
{
    //根据帐号Id获取帐号信息
    CBaseAccount * pAccount = GetAccountByAccountId(iAccountId, false);
    if (pAccount != NULL
            && pAccount->NeedRegister())
    {
        account_register_reason_t sReason;
        //刷新帐号状态
        LineStateChange(pAccount->GetAccountId(), modVoip_QueryLineState(pAccount->GetAccountId()), sReason,
                        true);
    }
}

typedef bool (*AccountCompareFun)(const CBaseAccount * lhs, const CBaseAccount * rhs);

//根据帐号Id加载帐号
void CAccountManager::LoadLineAccountById(int iAccountId)
{
    // id大于最大账号个数，出错(不包括IP直拨账号)
    if ((iAccountId < SIP_LINE_BEGIN_ID
            || iAccountId >= SIP_LINE_BEGIN_ID + acc_AccountNum(ACCOUNT_TYPE_SIP))
            && iAccountId != ACCOUNTID_OF_IPDIRECT_CALL)
    {
        ACCOUNT_ERR("device [%d] accoount id[%d] out of range", devicelib_GetPhoneType(), iAccountId);
        return;
    }
    const int iDefaultAccountId = configParser_GetConfigInt(kszAccountDefaultID);
    LINE_STATE eOldStatus = LS_UNKNOW;
    // 找到已经存在于队列中该帐号对应的节点, 擦除之
    if (m_listAccount.size() > 0)
    {
        for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
        {
            CBaseAccount * pAccount = (*it);
            if (pAccount->GetAccountId() == iAccountId)
            {
                eOldStatus = pAccount->GetAccountState();
                m_listAccount.erase(it);
                break;
            }
        }
    }

    if (iAccountId >= SIP_LINE_BEGIN_ID && iAccountId <= SIP_LINE_END_ID)
    {
        //取得当前ID的Account的配置文件的路径
        yl::string strFileName = commonAPI_FormatString("%s%d.cfg", SIP_ACCOUNT_FILE_PATH, iAccountId);
        CSipAccount * pAccount = new CSipAccount();

        if (!pAccount->LoadAccountFromConfig(strFileName.c_str(), iAccountId)
                || (pAccount->IsDirectIPAccount() && !IsAllowIPCall()))
        {
            ACCOUNT_WARN("Cannot load SIP account[%d]!", iAccountId);
            delete pAccount;
            pAccount = NULL;
            return;
        }

        //加载帐号成功，将其添加入链表
        m_listAccount.push_back(pAccount);
        //将账号列表进行排序
        Sort<CBaseAccount *, AccountCompareFun>(m_listAccount, CBaseAccount::Compare);

        //打印帐号信息
        pAccount->PrintAccoutInfo();

        //加载完成后通知帐号改变，帐号状态
        pAccount->NotifyAccountStatusChanged(eOldStatus);

        //如果默认账号状态有变，则需要重新设置默认账号
        if (m_iDefaultAccountId == iAccountId || iDefaultAccountId == iAccountId
                || m_iDefaultAccountId == -1)
        {
            ResetDefaultAccount();
        }

        //语言信箱账号变换，重新设置语音信箱账号
#ifndef _T49
        if (m_iLastVoiceMailID == iAccountId)
        {
            SetLastVMailAccountID(-1);
        }
#else
        etl_NotifyApp(FALSE, ACCOUNT_VOICE_MAIL_STATE_CHANGED, iAccountId, 0);
#endif
    }
    else
    {
        ACCOUNT_WARN("Cannot load line account by ID! Unknown ID: %d", iAccountId);
    }
}

#if IF_FEATURE_PSTN
// 根据账号ID加载PSTN账号
void CAccountManager::LoadPstnAccountById(int iAccountId, int iDeviceId)
{
    // 判断账号ID是否在有效范围内
    if (iAccountId < PSTN_LINE_BEGIN_ID
            || iAccountId >= PSTN_LINE_END_ID)
    {
        ACCOUNT_ERR("device [%d] accoount id[%d] out of range", devicelib_GetPhoneType(), iAccountId);
        return;
    }
    const int iDefaultAccountId = configParser_GetConfigInt(kszAccountDefaultID);
    LINE_STATE eOldStatus = LS_UNKNOW;

    // 如果PSTN账号已经存在，则不应该删掉重新添加（会导致串口重新打开），重新设置相关状态即可
    CPstnAccount * pAccount = dynamic_cast<CPstnAccount *>(GetAccountByAccountId(iAccountId));
    if (pAccount)
    {
        bool bEnable = acc_IsAccountEnable(pAccount->GetAccountId());
        // 启用pstnadapter对应设备
        pstnadapter_SetEnable(pAccount->GetDeviceId(), bEnable);
        // 设置状态
        pAccount->SetAccountState(bEnable ? LS_REGISTERED : LS_DISABLE);
        // 打印帐号信息
        pAccount->PrintAccoutInfo();
        //如果默认账号状态有变，则需要重新设置默认账号
        if (m_iDefaultAccountId == iAccountId
                || iDefaultAccountId == iAccountId
                || m_iDefaultAccountId == acc_GetDirectIPAccountID())
        {
            ResetDefaultAccount();
        }
        return;
    }

    // 如果PSTN账号不存在，则创建新账号
    pAccount = new CPstnAccount(iDeviceId);
    // 加载配置文件
    if (!pAccount->LoadAccountFromConfig(PSTN_ACCOUNT_FILE_PATH, iAccountId))
    {
        ACCOUNT_WARN("Cannot load PSTN account[%d]!", iAccountId);
        delete pAccount;
        pAccount = NULL;
        return;
    }
    //加载帐号成功，将其添加入链表
    m_listAccount.push_back(pAccount);
    //将账号列表进行排序
    Sort<CBaseAccount *, AccountCompareFun>(m_listAccount, CBaseAccount::Compare);

    //打印帐号信息
    pAccount->PrintAccoutInfo();

    //加载完成后通知帐号改变，帐号状态
    pAccount->NotifyAccountStatusChanged(eOldStatus);

    //如果默认账号状态有变，则需要重新设置默认账号
    if (m_iDefaultAccountId == iAccountId || iDefaultAccountId == iAccountId)
    {
        ResetDefaultAccount();
    }
}
#endif // IF_FEATURE_PSTN

//加载所有帐号
void CAccountManager::LoadAllLineAccount()
{
    ACCOUNT_INFO("CAccountManager::LoadAllLineAccount");
    m_listAccount.clear();
    // 加载SIP账号
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {
        for (int i = 0; i <= ACCOUNTID_OF_IPDIRECT_CALL; i++)
        {
            // 比如 12个账号的情况下，由于IP直拨账号在account16 我们读取account0-account11,
            // 跳过account12-account15,然后读取account16；
            if (i >= acc_AccountNum(ACCOUNT_TYPE_SIP) && i < ACCOUNTID_OF_IPDIRECT_CALL)
            {
                continue;
            }
            CSipAccount * pAccount = new CSipAccount();
            if (pAccount->LoadAccountFromConfig("", i))
            {
                //超出范围而且不是IP直播账号则为无效账号不加入账号链表
                if (pAccount->GetAccountId() < m_uMaxLineCount - 1
                        || (pAccount->IsDirectIPAccount() && IsAllowIPCall()))
                {
                    //普通帐号或允许IP直拨帐号
                    //加载帐号成功，将其添加入链表
                    m_listAccount.push_back(pAccount);
                }
                else
                {
                    delete pAccount;
                    pAccount = NULL;
                }
                //加载完成后通知帐号改变，帐号状态
                //objAccount.NotifyAccountStatusChanged(objAccount.GetAccountState());
            }
            else
            {
                delete pAccount;
                pAccount = NULL;
            }
        }
    }
    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {
        // 查找对应目录下的指定文件名格式的文件
        YLList<yl::string> listAccountFile;
        printf("Account Path is [%s]\n", CONFIG_VOIP_PATH);
        if (!commonAPI_getFilesInDirectoryByExt(&listAccountFile, CONFIG_VOIP_PATH, "", true)
                || listAccountFile.size() == 0)
        {
            ACCOUNT_WARN("Can't find any account file, please check it!");
            return;
        }

        ACCOUNT_INFO("Find %d SIP account files.", listAccountFile.size());
        UINT uAccountNum = 0;
        const UINT uSipAccountNum = acc_AccountNum(ACCOUNT_TYPE_SIP | ACCOUNT_TYPE_IP_DIRECT);
        // 遍历文件列表，并使用其加载
        for (YLList<yl::string>::iterator it = listAccountFile.begin(); it != listAccountFile.end()
                && uAccountNum < uSipAccountNum; ++it)
        {
            yl::string & strAccountFile = (*it);
            if (strAccountFile.find(SIP_ACCOUNT_FILE_NAME) == yl::string::npos)
            {
                //不是账号文件，跳过
                ACCOUNT_INFO("'%s' is not SIP account file, skip.", strAccountFile.c_str());
                continue;
            }
            ACCOUNT_INFO("'%s' is SIP account file, parse it.", strAccountFile.c_str());
            //若文件路径不为空，则进行帐号加载
            if (!strAccountFile.empty())
            {
                CSipAccount * pAccount = new CSipAccount();
                if (pAccount->LoadAccountFromConfig(strAccountFile))
                {
                    //超出范围而且不是IP直播账号则为无效账号不加入账号链表
                    if (pAccount->GetAccountId() < acc_AccountNum(ACCOUNT_TYPE_SIP)
                            || pAccount->IsDirectIPAccount() && IsAllowIPCall())
                    {
                        //普通帐号或允许IP直拨帐号
                        //加载帐号成功，将其添加入链表
                        m_listAccount.push_back(pAccount);
                        uAccountNum++;
                    }
                    else
                    {
                        delete pAccount;
                        pAccount = NULL;
                    }
                    //加载完成后通知帐号改变，帐号状态
                    //objAccount.NotifyAccountStatusChanged(objAccount.GetAccountState());
                }
                else
                {
                    delete pAccount;
                    pAccount = NULL;
                }
            }
        }
    }
#ifdef IF_FEATURE_PSTN
    // 加载PSTN账号配置
    for (int i = 0; i < pstnadapter_GetLineCount(); i++)
    {
        CPstnAccount * pAccount = new CPstnAccount(i);
        if (pAccount->LoadAccountFromConfig(PSTN_ACCOUNT_FILE_PATH, PSTN_LINE_BEGIN_ID + i))
        {
            m_listAccount.push_back(pAccount);
        }
        else
        {
            ACCOUNT_WARN("Cannot load config file for PSTN account #%d!", PSTN_LINE_BEGIN_ID + i);
            delete pAccount;
            pAccount = NULL;
        }
    }
#endif // IF_FEATURE_PSTN

#ifdef IF_FEATURE_H323
    // 加载h323账号
    LoadH323Account();
#endif // IF_FEATURE_H323

    //对帐号列表进行排序
    Sort<CBaseAccount *, AccountCompareFun>(m_listAccount, CBaseAccount::Compare);

    ACCOUNT_INFO("Account List Size(%d)", m_listAccount.size());
    PrintAllAccountInfo();
    //判断信号灯与背光灯指示
    ProcessAccountDeviceTip();
}

//处理帐号状态改变消息
bool CAccountManager::OnLineStateChange(msgObject & objMessage)
{
    if (objMessage.message != SIP_REG_UPDATE_REGISTER)
    {
        //若非对应消息，则不予处理
        return false;
    }

    int iAccountId = (int)objMessage.wParam;
    LINE_STATE eState = acc_MapState(objMessage.lParam);
    ACCOUNT_INFO("Line State Change AccountId:%d, State:%d", iAccountId, (int)eState);
    sip_reason_info_t * pInfo = (sip_reason_info_t *)objMessage.GetExtraData();
    account_register_reason_t sReason;
    if (pInfo)
    {
        sReason.m_iCode = pInfo->code;
        sReason.m_strReason = pInfo->reason;
    }
    return LineStateChange(iAccountId, eState, sReason);
}

#ifdef IF_FEATURE_PSTN
bool CAccountManager::OnPstnLineStateChange(msgObject & objMessage)
{
    ACCOUNT_INFO("CAccountManager::OnPstnLineStateChange msg=%d, wParam=%d, lParam=%d",
                 objMessage.message, objMessage.wParam, objMessage.lParam);
    if (PSTN_MSG_LINE_CHANGED != objMessage.message)
    {
        return false;
    }
    int iDeviceId = objMessage.wParam;
    if (PSTN_LINE_CONNECTED == objMessage.lParam)
    {
        // PSTN线路插入，需要添加PSTN账号
        int iAccountId = PSTN_LINE_BEGIN_ID;
        // 遍历查找可用的PSTN账号ID
        for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
        {
            if ((*it)->GetAccountType() == ACCOUNT_TYPE_PSTN && (*it)->GetAccountId() == iAccountId)
            {
                ++iAccountId;
            }
        }
        // 校验PSTN账号ID是否合法
        if (iAccountId - PSTN_LINE_BEGIN_ID >= MAX_PSTN_LINE_COUNT || iAccountId > PSTN_LINE_END_ID)
        {
            ACCOUNT_WARN("Too many PSTN account already!");
            return false;
        }
        // PSTN线路插入，需要预先把账号设为Enable
        acc_SetAccountEnable(iAccountId, true);
        // 通过配置文件添加账号
        LoadPstnAccountById(iAccountId, iDeviceId);
        // 关联设备ID
        CPstnAccount * pAccount = dynamic_cast<CPstnAccount *>(GetAccountByAccountId(iAccountId));
        if (NULL == pAccount)
        {
            ACCOUNT_WARN("Cannot get PSTN account!");
            return false;
        }

        ACCOUNT_INFO("PSTN account #%d added. Current account count: %d.", iAccountId,
                     m_listAccount.size());

        // 广播PSTN账号创建消息
        etl_NotifyAppEx(false, ACCOUNT_PSTN_CREATED, iAccountId, 0, 0, NULL);

        // 重置默认账号状态
        ResetDefaultAccount();

        return true;
    }
    else if (PSTN_LINE_DISCONNECTED == objMessage.lParam)
    {
        // PSTN线路拔出，需要移除PSTN账号
        CPstnAccount * pAccount = NULL;
        for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
        {
            pAccount = dynamic_cast<CPstnAccount *>(*it);
            if (pAccount && pAccount->GetDeviceId() == iDeviceId)
            {
                break;
            }
        }
        if (NULL == pAccount)
        {
            ACCOUNT_WARN("Cannot get PSTN account of device #%d!", iDeviceId);
            return false;
        }
        // 先把账号状态改为禁用
        pAccount->SetAccountState(LS_DISABLE);
        // 移除账号
        int iAccountId = pAccount->GetAccountId();
        EraseAccountById(iAccountId);
        ACCOUNT_INFO("PSTN account #%d erased. Current account count: %d.", iAccountId,
                     m_listAccount.size());

        // PSTN线路拔出，自动清空Label
        acc_SetAccountLabel(iAccountId, "");

        // 广播PSTN账号销毁消息
        etl_NotifyAppEx(false, ACCOUNT_PSTN_REMOVED, iAccountId, 0, 0, NULL);

        // 重置默认账号状态
        ResetDefaultAccount();

        return true;
    }
    else if (PSTN_LINE_STATUS_CHANGED == objMessage.lParam)
    {
        // PSTN线路状态变化时不需要重新选举默认账号，默认账号以注册成功LS_REGISTERED为准
        return true;
    }
    else
    {
        ACCOUNT_WARN("Unknown lParam[%d]!", objMessage.lParam);
    }
    return false;
}
#endif // IF_FEATURE_PSTN

// 处理账号配置改变消息
bool CAccountManager::OnLineConfigChange(msgObject & objMessage)
{
    if (objMessage.message != CONFIG_MSG_BCAST_CHANGED
            || objMessage.wParam != ST_SIPLINE)
    {
        // 不处理非对应的消息
        return false;
    }
    int iAccountId = objMessage.lParam;
    if (iAccountId == m_iDefaultAccountId)
    {
        SetAutoAnswer();
    }
    // V73与Autop协议-1表示默认账号改变
    if (iAccountId == -1)
    {
        ResetDefaultAccount();
    }
    else
    {
#ifdef IF_FEATURE_PSTN
        if (iAccountId >= PSTN_LINE_BEGIN_ID && iAccountId <= PSTN_LINE_END_ID)
        {
            // 如果PSTN账号不存在（未插入PSTN线路），则不需要重新加载配置文件
            CPstnAccount * pAccount = dynamic_cast<CPstnAccount *>(GetAccountByAccountId(iAccountId));
            if (pAccount)
            {
                LoadPstnAccountById(iAccountId, pAccount->GetDeviceId());
            }
        }
        else
#endif // IF_FEATURE_PSTN
        {
            //读取对应帐号
            LoadLineAccountById(iAccountId);
        }
#ifdef IF_FEATURE_PSTN
        // PSTN账号不需要刷新状态
        if (!acc_IsPstnAccount(iAccountId))
#endif // IF_FEATURE_PSTN
        {
#if IF_BUG_26237
            //刷新对应帐号状态
            RefreshLineStateById(iAccountId);
#endif
        }
    }
    return true;
}

#ifdef IF_FEATURE_H323

bool CAccountManager::IsAllAccountEmpty()
{
    CAccountList::iterator it = m_listAccount.begin();
    for (; it != m_listAccount.end(); ++it)
    {
        CSipAccount * pAccount = dynamic_cast<CSipAccount *>(*it);
        if (pAccount == NULL)
        {
            continue;
        }


        if (pAccount->IsDirectIPAccount())
        {
            continue;
        }

        if (!pAccount->IsEmptyAccount())
        {
            return false;
        }
    }
    return true;
}

bool CAccountManager::LoadH323Account()
{
    CH323Account * pAccount = new CH323Account();
    if (pAccount->LoadAccountFromConfig("", H323ACCOUNTID))
    {
        //加载帐号成功，将其添加入链表
        m_listAccount.push_back(pAccount);
    }
    else
    {
        ACCOUNT_ERR("load h323 account fail");
        delete pAccount;
    }


    CH323Account * pAccountP2P = new CH323Account();
    if (pAccountP2P->LoadAccountFromConfig("", H323ACCOUNTID_IP))
    {
        //加载帐号成功，将其添加入链表
        m_listAccount.push_back(pAccountP2P);
    }
    else
    {
        ACCOUNT_ERR("load h323 p2p account fail");

        delete pAccountP2P;
    }

    return true;
}

bool CAccountManager::OnH323ConfigChanged(msgObject & objMessage)
{
    bool bChanged = false;
    CAccountList::iterator it = m_listAccount.begin();
    for (; it != m_listAccount.end(); ++it)
    {
        CH323Account * pAccount = dynamic_cast<CH323Account *>(*it);
        if (NULL == pAccount)
        {
            continue;
        }

        if (pAccount->GetAccountType() != ACCOUNT_TYPE_H323)
        {
            continue;
        }

        LINE_STATE eOldStatus = pAccount->GetAccountState();
        pAccount->LoadAccountFromConfig("", pAccount->GetAccountId());

        pAccount->NotifyAccountStatusChanged(eOldStatus);

        if (eOldStatus != pAccount->GetAccountState())
        {
            bChanged = true;
        }
    }

    if (bChanged)
    {
        //如果默认账号状态有变，则需要重新设置默认账号
        ResetDefaultAccount();
    }

    // 测试默认账号为h323，不支持voicemail
    //语言信箱账号变换，重新设置语音信箱账号
    //etl_NotifyApp(FALSE, ACCOUNT_VOICE_MAIL_STATE_CHANGED, iAccountId, 0);

    return true;
}

bool CAccountManager::OnH323LineStateChange(msgObject & objMessage)
{
    if (configParser_GetConfigInt(kszVCAccountActive) == 0
            || configParser_GetConfigInt(kszVCAccountEnable) == 0)
    {
        // 总开关关闭后，不处理H323消息：通过逻辑层控制，实际底层账号可能还是注册上的
        return true;
    }

    ACCOUNT_INFO("H323 State[%d]", objMessage.lParam);

    // h323只有一个账号
    int iAccountId = H323ACCOUNTID; /*MapH323AccountId2Logic((int)objMessage.wParam)*/;
    LINE_STATE eState = acc_MapH323State2Logic(objMessage.lParam);

    ACCOUNT_INFO("H323 Line State Change Id[%d] State[%d]", iAccountId, (int)eState);
    account_register_reason_t sReason;

    return LineStateChange(iAccountId, eState, sReason);
}

CBaseAccount * CAccountManager::GetH323DirectIPAccount()
{
    CAccountList::iterator it = m_listAccount.begin();
    for (; it != m_listAccount.end(); ++it)
    {
        CBaseAccount * objAccount = (*it);
        if (objAccount
                && objAccount->GetAccountType() == ACCOUNT_TYPE_H323
                && objAccount->IsDirectIPAccount())
        {
            // H323 IP直拨帐号
            return objAccount;
        }
    }

    return NULL;
}

int CAccountManager::MapH323AccountId2Logic(int iAccountId)
{
    // 目前底层上报的H323账号只会是0，映射上层
    CBaseAccount * pAccount = GetAccountByAccountId(H323ACCOUNTID, false);
    if (pAccount != NULL
            && pAccount->GetAccountState() == LS_REGISTERED)
    {
        return H323ACCOUNTID;
    }

    return H323ACCOUNTID_IP;
}
#endif //#ifdef IF_FEATURE_H323
//帐号状态改变
bool CAccountManager::LineStateChange(int iAccountId, LINE_STATE eState,
                                      account_register_reason_t & sReason, bool bGuiQuery/*=false*/)
{
    //若网络状态为不可用，则直接将帐号状态设置为失败

    if (!netIsNetworkReady())
    {
        eState = LS_REGISTER_FAIL;
    }

    //根据帐号Id获取账号对象指针
    CBaseAccount * pAccount = GetAccountByAccountId(iAccountId, false);
    if (pAccount == NULL)
    {
        //获取帐号状态失败，不予处理
        return false;
    }

    //处理某账号状态改变的情况
    if (!pAccount->NeedRegister())
    {
        // http://10.2.1.199/Bug.php?BugID=94507 不需要注册的账号也要发送注册注册上的消息，保证外部状态正常
        eState = LS_REGISTERED;
    }

    pAccount->LineStateChange(eState, sReason, bGuiQuery);

    //设置系统默认帐号为配置文件中的那个，若设置不成功则设置可用帐号
    ResetDefaultAccount();

    //判断信号灯与背光灯指示
    ProcessAccountDeviceTip();

#ifndef _T49
#ifdef _T42
    //账户不可用时,检测是否需要删除voiceMail图标
    if (!pAccount->IsAccountUsable())
    {
        bool bNoNewVoiceMail = true;
        CAccountList::iterator it = m_listAccount.begin();
        for (; it != m_listAccount.end(); ++it)
        {
            CSipAccount * pSipAccount = dynamic_cast<CSipAccount *>(*it);
            if (pSipAccount && pSipAccount->IsAccountUsable()
                    && pSipAccount->GetUnReadVoiceMailCount() > 0)
            {
                //帐号可用，且存在新语音留言
                bNoNewVoiceMail = false;
                break;
            }
        }
        if (bNoNewVoiceMail)
        {

            //通知VoiceMail数量改变
            etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, false, 0);
        }
    }
    else
    {
        CSipAccount * pSipAccount = dynamic_cast<CSipAccount *>(pAccount);

        LINE_STATE estat = pAccount->GetAccountState();
        if (estat == LS_REGISTERED
                && pSipAccount->GetUnReadVoiceMailCount() > 0)
        {
            //通知VoiceMail数量改变
            etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, 2, 0);
        }
    }
#else
    LINE_STATE estat = pAccount->GetAccountState();
    //账号变成不可用的时候才弹窗,变成可用的时候sip会发消息弹窗,中间状态不发消息
    if (estat == LS_REGISTER_FAIL
            || estat ==  LS_DISABLE
            || estat == LS_EXIT
            || estat == LS_BE_UNREGISTERED
            || estat == LS_DISABLE_REG)
    {
        //通知VoiceMail数量改变
        etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, false, 0);
    }
    else if (estat == LS_REGISTERED)
    {
        CSipAccount * pSipAccount = dynamic_cast<CSipAccount *>(pAccount);

        if (pSipAccount->GetUnReadVoiceMailCount() > 0)
        {
            //通知VoiceMail数量改变
            etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, 2, 0);
        }
    }
#endif

    if (m_iLastVoiceMailID == iAccountId)
    {
        // 账号状态改变时需要重新设置m_iLastVoiceMailID
        SetLastVMailAccountID(-1);
    }
#else
    //写云账号配置
    _CloudAccountInfoManager.ProcessCloudStateChange(iAccountId, eState, sReason);

    etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_STATE_CHANGED, iAccountId, 1);
#endif

    m_nUsableAccount = -1;
    GetUsableAccountCount();
    return true;
}

//处理网络状态改变消息
bool CAccountManager::OnNetworkStateChange(msgObject & objMessage)
{
    if (objMessage.message != SYS_MSG_NOTIFY_NETWORK_STATUS
            && objMessage.message != SYS_MSG_NOTIFY_PHY_STATUS)
    {
        //若非对应消息，则不予处理
        return false;
    }

    CheckNetworkStatus();
    return true;
}


//检测网络状态发生改变，从而帐号状态相应修改
void CAccountManager::CheckNetworkStatus()
{
    //网络状态会有很多种，但Account只关心是否连上了网络
    if (m_bLastNetworkReady == netIsNetworkReady())
    {
        return;
    }
    m_bLastNetworkReady = netIsNetworkReady();
    ACCOUNT_INFO("Network Chanage[%d][%d]. Check Account Status. ", m_bLastNetworkReady,
                 netIsNetworkReady());

    //检测网络状态，并根据网络状态刷新帐号或设置注册状态
    if (m_bLastNetworkReady)
    {
        //网络状态正常，重新刷新帐号状态
        RefreshAllLineState();

        // http://10.2.1.199/Bug.php?BugID=18628
        // 特殊账号,在网络可用时,状态设为注册上
        for (CAccountList::iterator itAccount = m_listAccount.begin(); itAccount != m_listAccount.end();
                ++itAccount)
        {
            CBaseAccount * pAccount = (*itAccount);
            if (NULL == pAccount)
            {
                continue;
            }

            if (pAccount->IsDirectIPAccount()
                    && acc_IsAccountEnable(pAccount->GetAccountId()))
            {
                pAccount->SetAccountState(LS_REGISTERED, true);
            }

            if (pAccount->GetAccountType() == ACCOUNT_TYPE_SIP)
            {
                CSipAccount * pSipAccount = dynamic_cast<CSipAccount *>(pAccount);
                if (pSipAccount && pSipAccount->IsSpecialCloudAccount())
                {
                    // 免注册云账号，直接设置为注册上，要设置重置默认账号
                    account_register_reason_t sReason;

                    //刷新对应帐号信息
                    LineStateChange(pAccount->GetAccountId(), LS_REGISTERED, sReason, false);
                }
            }
        }
    }
    else
    {
        //网络状态异常直接将SipAccount帐号状态置为注册失败状态
        for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
        {
            CBaseAccount * pAccount = *it;
            if (pAccount && pAccount->GetAccountState() != LS_DISABLE)
            {
                pAccount->SetAccountState(LS_REGISTER_FAIL, true);
            }
        }

        // 重置默认帐号
        ResetDefaultAccount();
#ifndef _T49
        //通知VoiceMail数量改变，无新语音留言
        etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, false, 0);
#endif
    }
}

#ifndef _T49
//收到新语音信息消息，刷新帐号的语音信息相关内容
bool CAccountManager::OnNewVoiceMessage(msgObject & objMessage)
{
    if (objMessage.message != SIP_MSG_UPDATE_MWI_INFO
            || NULL == objMessage.GetExtraData())
    {
        //若不是对应的新语音信息消息，则不予处理
        return false;
    }

    //wParam为帐号Id
    int iAccountId = (int)objMessage.wParam;
    //根据帐号Id获取帐号信息
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(GetAccountByAccountId(iAccountId));
    if (pAccount == NULL
            || !pAccount->IsAccountUsable()
            || pAccount->IsSpecialAccount()) //特殊账号不处理http://10.2.1.199/Bug.php?BugID=60932
    {
        ACCOUNT_INFO("Invaild AccountID[%d], Ignore MWI Message.", iAccountId);
        //获取帐号状态失败,或账号不可用，不予处理
        return false;
    }

    // 若开关未开，则不处理VoiceMail信息
    if (!pAccount->GetVoiceMailSwitch())
    {
        return true;
    }

    sip_mwi_info_t * pMWIInfo = (sip_mwi_info_t *)objMessage.GetExtraData();

    //lParam为新语音信息数量
    UINT uUnReadVoiceMailCount = (UINT)pMWIInfo->new_msg;
    UINT uReadedVoiceMailCount = (UINT)pMWIInfo->old_msg;

    ACCOUNT_INFO("Voice Mail new[%d] old[%d]", uUnReadVoiceMailCount, uReadedVoiceMailCount);
    UINT uOriginalUnRead = pAccount->GetUnReadVoiceMailCount();
    UINT uOriginalReaded = pAccount->GetReadedVoiceMailCount();

    bool bAlertVoiceMail = configParser_GetConfigInt(kszAlertVoiceMail) != 0;
    if (!bAlertVoiceMail
            && uUnReadVoiceMailCount == uOriginalUnRead
            && uReadedVoiceMailCount == uOriginalReaded)
    {
        // 如果语音信息数量未改变，认为不需要更新提示状态
        ACCOUNT_INFO("Voice mail count no change");
        return true;
    }

    //将语音信息内容赋值
    pAccount->SetReadedVoiceMailCount(uReadedVoiceMailCount);
    pAccount->SetUnReadVoiceMailCount(uUnReadVoiceMailCount);

    if (uUnReadVoiceMailCount > 0)
    {
        // http://10.2.1.199/Bug.php?BugID=32965，记录最近的VoiceMial账号
        SetLastVMailAccountID(iAccountId);
    }
    else
    {
        SetLastVMailAccountID(-1);
    }

    ACCOUNT_INFO("uLastNewVoice[%d] uNewNumber[%d]", uOriginalUnRead, uUnReadVoiceMailCount);

    bool bNewVoiceMail = bAlertVoiceMail ? uUnReadVoiceMailCount >= uOriginalUnRead
                         : uUnReadVoiceMailCount > uOriginalUnRead;

    //通知VoiceMail数量改变
    etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, bNewVoiceMail, 1);

    return true;
}

//m_iLastVoiceMailID对应的账号发生改变，
void CAccountManager::SetLastVMailAccountID(int iAccountID)
{
    if (iAccountID < 0)
    {
        // 当m_iLastVoiceMailID对应的账号状态发生改变时，如果默认账号有语音留言，则设置为默认账号
        // 否则获取第一个有语音留言的账号

        m_iLastVoiceMailID = -1;
        if (acc_GetUnReadVoiceMailNum(m_iDefaultAccountId) > 0)
        {
            m_iLastVoiceMailID = m_iDefaultAccountId;
        }
        else
        {
            for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
            {
                CSipAccount * pAccount = dynamic_cast<CSipAccount *>(*it);
                if (pAccount && pAccount->IsAccountUsable()
                        && pAccount->GetUnReadVoiceMailCount() > 0)
                {
                    //帐号可用，且存在新语音留言
                    m_iLastVoiceMailID = pAccount->GetAccountId();
                    break;
                }
            }
        }
    }
    else
    {
        m_iLastVoiceMailID = iAccountID;
    }
    ACCOUNT_INFO("CAccountManager::SetLastVMailAccountID(iAccountID = %d) m_iLastVoiceMailID = %d",
                 iAccountID, m_iLastVoiceMailID);
}
#endif

//获取IP直拨帐号
CSipAccount * CAccountManager::GetDirectIPAccount(bool bCheck/* = true*/)
{
    if ((bCheck && !IsAllowIPCall())
            || 0 == m_listAccount.size())
    {
        //若不支持IP直拨或帐号列表为空，则直接返回空指针
        return NULL;
    }

    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CSipAccount * pAccount = dynamic_cast<CSipAccount *>(*it);
        if (pAccount && pAccount->IsDirectIPAccount())
        {
            //IP直拨帐号
            return pAccount;
        }
    }

    return NULL;
}

//根据帐号Id获取帐号信息
CBaseAccount * CAccountManager::GetAccountByAccountId(int iAccountId, bool bCheck /* = true */)
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        //比较Account Id是否一致
        if (pAccount && pAccount->GetAccountId() == iAccountId)
        {
            if (bCheck
                    && !IsAllowIPCall()
                    && pAccount->IsDirectIPAccount()
                    && pAccount->GetProtocol() == PROTOCOL_SIP)
            {
                ACCOUNT_WARN("get sip account return null");
                //不允许SIP IP直拨，返回空指针
                return NULL;
            }
            else
            {
                return pAccount;
            }
        }
    }

#if DEBUG_ACCOUNT
    ACCOUNT_ERR("Get account by id not find [%d] [%d]", iAccountId, bCheck);
#endif // DEBUG_ACCOUNT
    return NULL;
}

//由服务器名获得帐号信息
CBaseAccount * CAccountManager::GetAccountByServerName(const yl::string & strServer)
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && pAccount->GetServerName() == strServer)
        {
            //比较SIPServer信息是否一致
            return pAccount;
        }
    }
    return NULL;
}

//根据帐号获取帐号信息
CBaseAccount * CAccountManager::GetAccountBySIPName(LPCSTR lpszSIPName,
        LPCSTR lpszSIPServer /*= NULL*/)
{
    if (lpszSIPName == NULL || 0 == strcmp(lpszSIPName, ""))
    {
        return NULL;
    }

    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && pAccount->GetUserName().compare(lpszSIPName) == 0)
        {
            //比较SIPServer信息是否一致
            if (lpszSIPServer == NULL)
            {
                return pAccount;
            }
            // 比较服务器名
            if (pAccount->GetServerName().compare(lpszSIPServer) == 0)
            {
                return pAccount;
            }
        }
    }
    return NULL;
}

#ifdef IF_FEATURE_PSTN
CPstnAccount * CAccountManager::GetAccountByDeviceId(int iDeviceId)
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CPstnAccount * pAccount = dynamic_cast<CPstnAccount *>(*it);
        if (pAccount && pAccount->GetDeviceId() == iDeviceId)
        {
            return pAccount;
        }
    }
    return NULL;
}

CPstnAccount * CAccountManager::GetAccountByPstnName(const char * szPstnFullName)
{
    char szPstnName[8] = { 0 };
    const char * posBegin = strrchr(szPstnFullName, '(');
    const char * posEnd = strrchr(szPstnFullName, ')');
    if (posBegin == NULL || posEnd == NULL)
    {
        // 没有括号，则全部复制
        strncpy(szPstnName, szPstnFullName, 8);
        szPstnName[7] = '\0';
    }
    else if (posEnd > posBegin && posEnd - posBegin - 1 < 8)
    {
        // 括号顺序正确
        strncpy(szPstnName, posBegin + 1, posEnd - posBegin - 1);
        szPstnName[posEnd - posBegin - 1] = '\0';
    }

    int id = INVALID_ACCOUNTID;
    if (sscanf(szPstnName, "PSTN%d", &id) > 0)
    {
        id = id - 1 + PSTN_LINE_BEGIN_ID;
    }
    return dynamic_cast<CPstnAccount *>(GetAccountByAccountId(id));
}

CBaseAccount * CAccountManager::GetUsableAccountLike(int iFirstChoiceAccountId)
{
    ACCOUNT_DEBUG("CAccountManager::GetUsableAccountLike(%d)", iFirstChoiceAccountId);
    /*
     * 1.如果首选账号可用，则返回首选账号ID。
     * 2.其次优先使用相同类型的可用最小编号账号。
     * 3.再次使用其他类型的可用账号。
     * 4.如果没有可用账号，则返回NULL。
     */

    // 判断首选账号
    CBaseAccount * pAccount = GetAccountByAccountId(iFirstChoiceAccountId);
    if (pAccount && pAccount->IsAccountUsable())
    {
        return pAccount;
    }
    ACCOUNT_DEBUG("First choice account not usable.");

    // 判断同类型账号
    if (iFirstChoiceAccountId >= SIP_LINE_BEGIN_ID && iFirstChoiceAccountId <= SIP_LINE_END_ID)
    {
        pAccount = GetUsableAccount(true, ACCOUNT_TYPE_SIP);
    }
    else if (iFirstChoiceAccountId >= PSTN_LINE_BEGIN_ID && iFirstChoiceAccountId <= PSTN_LINE_END_ID)
    {
        pAccount = GetUsableAccount(true, ACCOUNT_TYPE_PSTN);
    }
    if (pAccount && pAccount->IsAccountUsable())
    {
        return pAccount;
    }
    ACCOUNT_DEBUG("No usable account of the same type.");

    // 判断其他账号
    pAccount = GetUsableAccount(false, ACCOUNT_TYPE_ALL);

    return pAccount;
}
#endif // IF_FEATURE_PSTN

//根据帐号全名获取帐号信息
CBaseAccount * CAccountManager::GetAccountByFullName(LPCSTR lpszFullName)
{
    yl::string strFullName = "";
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (NULL == pAccount)
        {
            continue;
        }
        //组合帐号全名
        strFullName = pAccount->GetUserName();
        strFullName += "@";
        strFullName += pAccount->GetServerName();

        if (strFullName.compare(lpszFullName) == 0)
        {
            //比较帐号全名是否一致
            return pAccount;
        }
    }
    return NULL;
}

//自动获得一个可用的账号
CBaseAccount * CAccountManager::GetUsableAccount(bool bSearchFirst /* = false */,
        int eAccountType /* = ACCOUNT_TYPE_SIP | ACCOUNT_TYPE_PSTN */)
{
    CBaseAccount * pDefaultAccount = GetAccountByAccountId(m_iDefaultAccountId);
    if (!bSearchFirst
            && pDefaultAccount != NULL
            && (eAccountType & pDefaultAccount->GetAccountType())
            && pDefaultAccount->IsAccountUsable())
    {
        //若不是获取第一个可用帐号且默认帐号存在且类型符合且可用，则使用默认帐号
        return pDefaultAccount;
    }
    // 若默认帐号不存在，则使用第一个可用的账号
    pDefaultAccount = NULL;
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        // 账号类型符合且可用
        if ((eAccountType & (*it)->GetAccountType()) && (*it)->IsAccountUsable())
        {
            pDefaultAccount = *it;
            // IP直拨账号优先级最低（低于PSTN账号）
            if (pDefaultAccount && !pDefaultAccount->IsDirectIPAccount())
            {
                return pDefaultAccount;
            }
        }
    }
    return pDefaultAccount;
}

// 自动获得一个已注册的账号
CBaseAccount * CAccountManager::GetRegisteredAccount(bool bSearchFirst /* = false */,
        int eAccountType /* = ACCOUNT_TYPE_SIP | ACCOUNT_TYPE_PSTN */)
{
    CBaseAccount * pDefaultAccount = GetAccountByAccountId(m_iDefaultAccountId);
    if (!bSearchFirst
            && pDefaultAccount != NULL
            && (eAccountType & pDefaultAccount->GetAccountType())
            && (pDefaultAccount->GetAccountState() == LS_REGISTERED
                || pDefaultAccount->GetAccountState() == LS_DISABLE_REG))
    {
        // 若不是获取第一个已注册帐号且默认帐号存在且类型符合且已注册，则使用默认帐号
        return pDefaultAccount;
    }
    // 若默认帐号不存在，则使用第一个已注册的账号
    pDefaultAccount = NULL;
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        // 账号类型符合且已注册
        if ((eAccountType & (*it)->GetAccountType())
                && ((*it)->GetAccountState() == LS_REGISTERED || (*it)->GetAccountState() == LS_DISABLE_REG))
        {
            pDefaultAccount = *it;
            // IP直拨账号优先级最低（低于PSTN账号）
            if (!pDefaultAccount->IsDirectIPAccount())
            {
                return pDefaultAccount;
            }
        }
    }
    return pDefaultAccount;
}

CBaseAccount * CAccountManager::GetUsableAccount(const yl::string & strServer,
        const yl::string & strName/* = ""*/)
{
    CBaseAccount * pFirstUsableAccount = NULL;
    CBaseAccount * pFirstSIPServerAccount = NULL;
    CBaseAccount * pSpecifyAccount = NULL;
    CBaseAccount * pAccount = NULL;

    //非Redial时,用当前账号呼出
    if (m_iDefaultAccountId != -1 && strName.empty() && strServer.empty())
    {
        pAccount = GetAccountByAccountId(m_iDefaultAccountId);
        if (pAccount && pAccount->IsAccountUsable())
        {
            return pAccount;
        }
    }

    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        pAccount = *it;
        if (NULL == pAccount)
        {
            continue;
        }
        //保存第一个可用的账号
        if (pFirstUsableAccount == NULL && pAccount->IsAccountUsable())
        {
            pFirstUsableAccount = pAccount;
        }

        if (pFirstSIPServerAccount == NULL
                && !pAccount->GetServerName().empty())
        {
            pFirstSIPServerAccount = pAccount;
        }

        if (!strName.empty() && !strServer.empty()) //Redial时
        {
            if (pAccount->GetRegisterName() == strName
                    && pAccount->GetServerName() == strServer)
            {
                pSpecifyAccount = pAccount;
                break;
            }
        }
        else if (!strServer.empty())    //DSK_MSG_MAKE_CALL时
        {
            if (pAccount->GetServerName() == strServer)
            {
                // 应付通话记录是无注册上账号呼出的情况
                pSpecifyAccount = pAccount;
                if (pAccount->IsAccountUsable())
                {
                    break;
                }
            }
        }
    }

    if (pSpecifyAccount)
    {
        return pSpecifyAccount;
    }

    CBaseAccount * pTmpAccount = GetAccountByAccountId(m_iDefaultAccountId);
    if (pTmpAccount && pTmpAccount->IsAccountUsable())
    {
        return pTmpAccount;
    }

    // 如果显示当前账号为IP直拨账号（证明没有注册上的账号），且存在配置有SIP Server的账号，则返回配置有SIP Server的账号. -xww
    if (pFirstUsableAccount && pFirstUsableAccount->IsDirectIPAccount() && pFirstSIPServerAccount)
    {
        return pFirstSIPServerAccount;
    }
    return pFirstUsableAccount;
}

//获取不包括特殊帐号的可用帐号数
UINT CAccountManager::GetUsableAccountCount()
{
    if (m_nUsableAccount >= 0)
    {
        return (UINT)m_nUsableAccount;
    }

    m_nUsableAccount = 0;
    for (CAccountList::iterator itAccount = m_listAccount.begin(); itAccount != m_listAccount.end();
            ++itAccount)
    {
        CBaseAccount * pAccount = (*itAccount);
        if (pAccount
                && !pAccount->IsSpecialAccount()
                && pAccount->IsAccountUsable())
        {
            //非特殊帐号，且帐号为注册上，则计数+1
            m_nUsableAccount++;
        }
    }

    //ACCOUNT_INFO("Usable Account Number:(%d)", m_nUsableAccount);
    return m_nUsableAccount;
}

//获取可用帐号数
UINT CAccountManager::GetUsableAccountCount(bool bExceptSpecialAccount,
        int eAccountType/* = ACCOUNT_TYPE_ALL*/)
{
    UINT nUsableAccountNum = 0;

    for (CAccountList::iterator itAccount = m_listAccount.begin();
            itAccount != m_listAccount.end();
            ++ itAccount)
    {
        CBaseAccount *pAccount = (*itAccount);
        if (!pAccount)
        {
            continue;
        }

        if (bExceptSpecialAccount && pAccount->IsSpecialAccount())
        {
            continue;
        }

        if (eAccountType & pAccount->GetAccountType()
                && pAccount->IsAccountUsable())
        {
            // 无需注册账号/SIP账号注册上/PSTN账号空闲状态则+1
            nUsableAccountNum ++;
        }
    }

    return nUsableAccountNum;
}

// 获取第一个Server不为空的账号
CBaseAccount * CAccountManager::GetNoEmptyServerAccount(bool bChekEnable/* = false*/)
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++ it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && !pAccount->IsSpecialAccount()
                && !pAccount->GetServerName().empty())
        {
            if (bChekEnable && !acc_IsAccountEnable(pAccount->GetAccountId()))
            {
                continue;
            }
            //非特殊帐号
            //第一个server不为空的账号，用于无账号呼出功能
            return pAccount;
        }
    }

    return NULL;
}

CBaseAccount * CAccountManager::GetFirstUserNameAccount()
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount
                && LS_DISABLE != pAccount->GetAccountState()
                && !pAccount->IsDirectIPAccount()
                && !pAccount->GetUserName().empty())
        {
            return pAccount;
        }
    }

    return NULL;
}

bool CAccountManager::GetValidAccountList(CAccountList & listValidAccount)
{
    listValidAccount.clear();
    //遍历全部账号数据，获取有效账号
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        if (!(*it)->IsDirectIPAccount()
                && acc_IsAccountEnable((*it)->GetAccountId()))
        {
            CBaseAccount * pAccount = NULL;
            switch ((*it)->GetAccountType())
            {
            case ACCOUNT_TYPE_SIP:
                pAccount = new CSipAccount(*dynamic_cast<CSipAccount *>(*it));
                break;
#ifdef IF_FEATURE_PSTN
            case ACCOUNT_TYPE_PSTN:
                pAccount = new CPstnAccount(*dynamic_cast<CPstnAccount *>(*it));
                break;
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
            case ACCOUNT_TYPE_H323:
                pAccount = new CH323Account(*dynamic_cast<CH323Account *>(*it));
                break;
#endif // IF_FEATURE_H323
            default:
                ACCOUNT_WARN("Unknown account type [%d] for account #%d!", (*it)->GetAccountType(),
                             (*it)->GetAccountId());
                continue;
            }
            listValidAccount.push_back(pAccount);
        }
    }
    //数据非空表示获取成功
    return listValidAccount.size() > 0;
}

bool CAccountManager::GetAvailableAccountList(CAccountList & listValidAccount)
{
    listValidAccount.clear();
    //遍历全部账号数据，获取可用账号
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        if (!(*it)->IsDirectIPAccount()
                && (*it)->IsAccountUsable())
        {
            CBaseAccount * pAccount = NULL;
            switch ((*it)->GetAccountType())
            {
            case ACCOUNT_TYPE_SIP:
            case ACCOUNT_TYPE_IP_DIRECT:
                pAccount = new CSipAccount(*dynamic_cast<CSipAccount *>(*it));
                break;
#ifdef IF_FEATURE_PSTN
            case ACCOUNT_TYPE_PSTN:
                pAccount = new CPstnAccount(*dynamic_cast<CPstnAccount *>(*it));
                break;
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
            case ACCOUNT_TYPE_H323:
                pAccount = new CH323Account(*dynamic_cast<CH323Account *>(*it));
                break;
#endif // IF_FEATURE_H323
            default:
                ACCOUNT_WARN("Unknown account type [%d] for account #%d!", (*it)->GetAccountType(),
                             (*it)->GetAccountId());
                continue;
            }
            listValidAccount.push_back(pAccount);
        }
    }
    //数据非空表示获取成功
    return listValidAccount.size() > 0;
}

//使用帐号Id设置默认帐号
bool CAccountManager::SetDefaultAccount(int iAccountId)
{
    //根据帐号Id获取帐号信息
    CBaseAccount * pAccount = GetAccountByAccountId(iAccountId);
    if (pAccount
            && pAccount->IsRegistered())
    {
        //设置当前帐号的状态为默认帐号，并赋值默认帐号指针
        if (iAccountId != m_iDefaultAccountId)
        {
            int iOldDefAcc = m_iDefaultAccountId;
            m_iDefaultAccountId = iAccountId;
            NotifyDefaultAccountChanged(iOldDefAcc);
            return true;
        }
        return false;
    }
    if (m_iDefaultAccountId != INVALID_ACCOUNTID)
    {
        //若设置默认帐号信息不成功，则将默认帐号指针设为空
        int iOldDefAcc = m_iDefaultAccountId;
        m_iDefaultAccountId = -1;
        NotifyDefaultAccountChanged(iOldDefAcc);
    }
    return false;
}

//将默认帐号Id保存至文件中
bool CAccountManager::SaveDefaultAccount(int iAccountId)
{
    CBaseAccount * pAccount = GetAccountByAccountId(iAccountId);
    //TO DO:设置默认帐号Id
    // http://10.2.1.199/Bug.php?BugID=18936
    if (NULL ==
            pAccount /*|| PROTOCOL_H323 == pAccount->GetProtocol()*/)// 需要处理两个账号类型定义冲突
    {
        return false;
    }

    configParser_SetConfigInt(kszAccountDefaultID, iAccountId, CONFIG_LEVEL_PHONE);
    if (m_iDefaultAccountId != iAccountId)
    {
        //若写入文件的内容与当前系统默认帐号Id不同，则重新设置默认帐号
        return SetDefaultAccount(iAccountId);
    }

    return false;
}

int CAccountManager::GetDefaultAccountID()
{
    return m_iDefaultAccountId;
}

//切换默认帐号
int CAccountManager::SwitchDefaultAccount(bool bForward /* = true */)
{
    CAccountList usableAccounts;
#ifdef _DECT
    GetValidAccountList(usableAccounts);
#else
    GetAvailableAccountList(usableAccounts);
#endif
    CAccountList::iterator it = usableAccounts.begin();
    while (it != usableAccounts.end())
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && pAccount->GetAccountId() == m_iDefaultAccountId)
        {
            break;
        }
        ++it;
    }
    if (it == usableAccounts.end())
    {
        SetDefaultAccount(INVALID_ACCOUNTID);
        return INVALID_ACCOUNTID;
    }

    if (bForward)
    {
        CBaseAccount * pAccount = ++it != usableAccounts.end() ? *it : usableAccounts.front();
        if (pAccount)
        {
            SetDefaultAccount(pAccount->GetAccountId());
        }
    }
    else
    {
        CBaseAccount * pAccount = it != usableAccounts.begin() ? *(--it) : usableAccounts.back();
        if (pAccount)
        {
            SetDefaultAccount(pAccount->GetAccountId());
        }
    }

    ACCOUNT_INFO("Default Account Switch to [%d]", m_iDefaultAccountId);

    return m_iDefaultAccountId;
}

//获取帐号列表
void CAccountManager::GetAccountList(CAccountList & listAccount, bool bOnlyUser /* = true */)
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        if (!(*it)->IsSpecialAccount()
                || !bOnlyUser)
        {
            //若不为特殊帐号或需要取包含系统帐号则添入列表中
            CBaseAccount * pAccount = NULL;
            switch ((*it)->GetAccountType())
            {
            case ACCOUNT_TYPE_SIP:
            case ACCOUNT_TYPE_IP_DIRECT:
                pAccount = new CSipAccount(*dynamic_cast<CSipAccount *>(*it));
                break;
#ifdef IF_FEATURE_PSTN
            case ACCOUNT_TYPE_PSTN:
                pAccount = new CPstnAccount(*dynamic_cast<CPstnAccount *>(*it));
                break;
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
            case ACCOUNT_TYPE_H323:
                pAccount = new CH323Account(*dynamic_cast<CH323Account *>(*it));
                break;
#endif //IF_FEATURE_H323
            default:
                ACCOUNT_WARN("Unknown account type [%d] for account #%d!", (*it)->GetAccountType(),
                             (*it)->GetAccountId());
                continue;
            }
            listAccount.push_back(pAccount);
        }
    }
}

//通知默认账号改变
void CAccountManager::NotifyDefaultAccountChanged(int iOldDefAcc)
{
    // 确保id有对应改变
    ACCOUNT_INFO("Notify Default Account Changed, [%d] -> [%d]", iOldDefAcc, m_iDefaultAccountId);

    //更新autoanswer 状态
    if (iOldDefAcc != m_iDefaultAccountId)
    {
        SetAutoAnswer();
    }

    //异步通知默认账号改变
#if IS_CODE_TEST
    etl_NotifyApp(true, ACCOUNT_DEFAULT_CHANGED, iOldDefAcc, 0);
#else
    etl_NotifyApp(false, ACCOUNT_DEFAULT_CHANGED, iOldDefAcc, 0);
#endif
}

//获取是否允许IP直播功能
bool CAccountManager::IsAllowIPCall()
{
    return 1 == configParser_GetConfigInt(kszEnableDirectIPCall);
}

// 返回账号个数
int CAccountManager::GetAccountNum(bool bOnlyUser)
{
    CAccountList listAccount;
    GetAccountList(listAccount, bOnlyUser);
    return listAccount.size();
}

// 获取注册上的账号数
int CAccountManager::GetRegisteredAccountNum(bool bOnlyUser /*= true */,
        int eAccountType /* = ACCOUNT_TYPE_ALL */)
{
    int iNum = 0;
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = (*it);
        if (NULL == pAccount)
        {
            continue;
        }

        if (bOnlyUser
                && pAccount->IsSpecialAccount())
        {
            continue;
        }
        if ((eAccountType & pAccount->GetAccountType()) && pAccount->IsRegistered())
        {
            ++iNum;
        }
    }

    return iNum;
}

YLList<int> CAccountManager::GetRegisteFailAccountList(int eAccountType /* = ACCOUNT_TYPE_ALL */)
{
    YLList<int> listAccount;
    for (CAccountList::iterator it = m_listAccount.begin();
            it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = (*it);
        if (NULL == pAccount)
        {
            continue;
        }

        if ((eAccountType & pAccount->GetAccountType()) && pAccount->IsRegisteFailed())
        {
            listAccount.push_back(pAccount->GetAccountId());
        }
    }

    return listAccount;
}

// 返回最多账号数
int CAccountManager::GetMaxAccountNum(bool bIgnoreDirectIPAccount /*= true*/)
{
    return bIgnoreDirectIPAccount ? m_uMaxLineCount - 1 : m_uMaxLineCount;
}

#ifndef _T49
// 获取未读voicemail数量，iAccount=-11表示获取所有账号的总数
int CAccountManager::GetVoiceMailNum(bool bReaded, int iAccountID /*= -1*/)
{
    int iVoiceMailNum = 0;
    if (iAccountID != -1)
    {
        CSipAccount * pAccount = dynamic_cast<CSipAccount *>(GetAccountByAccountId(iAccountID));
        if (NULL == pAccount)
        {
            return -1;
        }

        iVoiceMailNum =  bReaded ? pAccount->GetReadedVoiceMailCount() :
                         pAccount->GetUnReadVoiceMailCount();
    }
    else
    {
        for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
        {
            CSipAccount * pAccount = dynamic_cast<CSipAccount *>(*it);
            if (NULL == pAccount)
            {
                continue;
            }

            if (pAccount->IsAccountUsable())
            {
                if (bReaded)
                {
                    iVoiceMailNum += pAccount->GetReadedVoiceMailCount();
                }
                else
                {
                    iVoiceMailNum += pAccount->GetUnReadVoiceMailCount();
                }
            }
        }
    }

    return iVoiceMailNum;
}
#endif

//处理帐号信号灯流程
void CAccountManager::ProcessAccountDeviceTip()
{
}

// 自动填写Account账号配置
void CAccountManager::AutoSetAccount()
{
#ifdef IF_FEATURE_AUTO_CONFIG_ACCOUNT
    BOOL bAutoSet = (configParser_GetConfigInt(kszEnableAutoSetAccount) != 0);
    if (!bAutoSet)
    {
        return;
    }

    yl::string strPrefix = configParser_GetConfigString(kszAutoConfigAccountPrefix);
    if (strPrefix.length() > 256)
    {
        strPrefix = strPrefix.substr(0, 256);
    }
    yl::string strUserName = strPrefix + netGetMACText('\0');

    char bufPath[100];
    sprintf(bufPath, "%s0.cfg", SIP_ACCOUNT_FILE_PATH);

    // 只配置第一个账号
    if ((GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS) && (!pathFileExist(bufPath)))
    {
        ACCOUNT_WARN("%s no exist", bufPath);
        return;
    }

    configParser_SetCfgItemStringValue(kszAccountUserName, 0, strUserName);
    configParser_SetCfgItemStringValue(kszAccountRegisterName, 0, strUserName);

    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, 0);
    ACCOUNT_INFO("Account Name:%s", strUserName.c_str());

    configParser_SetConfigInt(kszEnableAutoSetAccount, 0);
#endif // #ifdef IF_FEATURE_AUTO_CONFIG_ACCOUNT
}

CAccountManager & CAccountManager::GetInstance()
{
    static CAccountManager objAccountManager;
    return objAccountManager;
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

//获取ACD账号(首个ACD开启的账号)
int CAccountManager::GetACDAccountId()
{
    if (m_listAccount.size() == 0)
    {
        return -1;
    }

    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CSipAccount * pAccount = dynamic_cast<CSipAccount *>(*it);
        if (NULL == pAccount)
        {
            continue;
        }
        ACCOUNT_INFO("accountId = %d,IsAccountUsable : %d, acd:%d",
                     pAccount->GetAccountId(), pAccount->IsAccountUsable(), pAccount->GetACDSwitch());
        if (pAccount->IsAccountUsable() && pAccount->GetACDSwitch())
        {
            //返回首个ACD开启的账号
            return pAccount->GetAccountId();
        }
    }
    return -1;
}

//获取Hoteling账号
int CAccountManager::GetHotelingAccountId()
{
    if (m_listAccount.size() == 0)
    {
        return -1;
    }

    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CSipAccount * pAccount = dynamic_cast<CSipAccount *>(*it);
        if (NULL == pAccount)
        {
            continue;
        }
        if (pAccount->IsAccountUsable() && (pAccount->GetHotelingSwitch()
                                            || pAccount->GetFlexibleSeatingSwitch()))
        {
            //返回首个Hoteling开启的账号
            return pAccount->GetAccountId();
        }
    }
    return -1;
}
#endif

#ifndef _T49
int CAccountManager::GetLastVMailAccountId()
{
    return m_iLastVoiceMailID;
}
#endif

// 擦除账号链表中的账号, eAccountType -- 账号主类型, iType -- 账号子类型
void CAccountManager::EraseAllAccount(int eAccountType /* = ACCOUNT_TYPE_ALL */, int iType/* = -1*/)
{
    // 擦除账号链表中对应类型的账号
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end();)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && (eAccountType & pAccount->GetAccountType()))
        {
            if (eAccountType == ACCOUNT_TYPE_SIP
                    && (SipAccountType)iType != ACCOUNT_NONE
                    && (SipAccountType)iType != acc_GetSipAccountType(pAccount->GetAccountId()))
            {
                ++it;
                continue;
            }
            ACCOUNT_INFO("Erase account:%d", pAccount->GetAccountId());
            it = m_listAccount.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// 根据账号ID擦除账号链表中的账号
void CAccountManager::EraseAccountById(int id)
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && pAccount->GetAccountId() == (UINT)id)
        {
            m_listAccount.erase(it);
            break;
        }
    }
}

// 清空所有账号文件中配置指定的账号信息, eAccountType -- 账号主类型, iType -- 账号子类型
void CAccountManager::EmptyAllAccount(int eAccountType /* = ACCOUNT_TYPE_ALL */, int iType/* = -1*/)
{
    // 清空SIP账号文件
    if (eAccountType & ACCOUNT_TYPE_SIP)
    {
        for (int i = SIP_LINE_BEGIN_ID; i < SIP_LINE_BEGIN_ID + acc_AccountNum(ACCOUNT_TYPE_SIP); i++)
        {
            CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(i);
            if (pAccount
                    && (SipAccountType)iType != ACCOUNT_NONE
                    && (SipAccountType)iType != acc_GetSipAccountType(pAccount->GetAccountId()))
            {
                continue;
            }

            ACCOUNT_INFO("Empty account:%d", i);
            EmptyAccountById(i);
        }
    }

#ifdef IF_FEATURE_PSTN
    // 清空PSTN账号文件
    if (eAccountType & ACCOUNT_TYPE_PSTN)
    {
        for (int i = PSTN_LINE_BEGIN_ID; i < PSTN_LINE_BEGIN_ID + MAX_PSTN_LINE_COUNT; i++)
        {
            EmptyAccountById(i);
        }
    }
#endif // IF_FEATURE_PSTN

#ifdef IF_FEATURE_H323
    if (eAccountType & ACCOUNT_TYPE_H323)
    {
        // TODO
    }
#endif // IF_FEATURE_H323
}

// 根据账号ID清空账号文件中配置指定的账号信息
void CAccountManager::EmptyAccountById(int id)
{
    if ((id >= SIP_LINE_BEGIN_ID && id <= SIP_LINE_END_ID)
#ifdef IF_FEATURE_PSTN
            || (id >= PSTN_LINE_BEGIN_ID && id <= PSTN_LINE_END_ID)
#endif // IF_FEATURE_PSTN
       )
    {
        // 保存之前的状态
        bool bEnable = acc_IsAccountEnable(id);

        acc_SetAccountEnable(id, false, CONFIG_LEVEL_PHONE);
        acc_SetAccountLabel(id, "", CONFIG_LEVEL_PHONE);
        acc_SetAccountAutoAnswer(id, false, CONFIG_LEVEL_PHONE);
        acc_SetAccountAutoAnswerMute(id, false, CONFIG_LEVEL_PHONE);
        acc_SetAccountRing(id, "Common", CONFIG_LEVEL_PHONE);

        if (id >= SIP_LINE_BEGIN_ID && id <= SIP_LINE_END_ID)
        {
            // 清除帐号是Hot Desking帐号的标志
            if (id == 0)
            {
                configParser_SetCfgItemIntValue(kszAccountEnableHoteling, id, 0, CONFIG_LEVEL_PHONE);
            }

            configParser_SetCfgItemStringValue(kszAccountDisplayName, id, "", CONFIG_LEVEL_PHONE);

            bool bLocked = configParser_GetConfigInt(kszLockLevel) != 0;
            if (configParser_GetConfigInt(kszHotDeskingSipServer) != 0 && !bLocked)
            {
                configParser_SetCfgItemStringValue(kszAccountSIPServerHost, id, "", CONFIG_LEVEL_PHONE);
                for (int i = 1; i <= SIPSERVER_NUM; ++i)
                {
                    configParser_SetCfgItemStringValue(kszAccountServerAddr, id, i, "", CONFIG_LEVEL_PHONE);
                }
            }

            if (configParser_GetConfigInt(kszHotDeskingOutbound) != 0)
            {
                configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, id, 0, CONFIG_LEVEL_PHONE);
                if (!bLocked)
                {
                    for (int i = 1; i <= SIPSERVER_NUM; ++i)
                    {
                        configParser_SetCfgItemStringValue(kszAccountOutboundProxy, id, i, "", CONFIG_LEVEL_PHONE);
                    }
                }
            }

            if (configParser_GetConfigInt(kszHotDeskingUserName) != 0
                    || configParser_GetConfigInt(kszHotDeskingRegisterName) != 0)
            {
                configParser_SetCfgItemStringValue(kszAccountUserName, id, "", CONFIG_LEVEL_PHONE);
                configParser_SetCfgItemStringValue(kszAccountRegisterName, id, "", CONFIG_LEVEL_PHONE);
            }

            if (configParser_GetConfigInt(kszHotDeskingPassword) != 0)
            {
                configParser_SetCfgItemStringValue(kszAccountPassword, id, "", CONFIG_LEVEL_PHONE);
            }
        }

#ifdef _T49
        configParser_SetCfgItemIntValue(kszAccountType, id, ACCOUNT_COMMON, CONFIG_LEVEL_PHONE);
        configParser_SetCfgItemIntValue(kszAccountBfcpSwitch, id, 0, CONFIG_LEVEL_PHONE);
#endif // _T49

        // http://10.2.1.199/Bug.php?BugID=87912
        //if (bEnable)
        {
            msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE,
                               id);
            msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, id);
        }
    }
    else
    {
        ACCOUNT_WARN("Unknown account ID [%d] to empty!", id);
    }
}

bool CAccountManager::IsAllAccountDisable()
{
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = (*it);
        if (NULL == pAccount)
        {
            continue;
        }

        if (pAccount->GetAccountType() == ACCOUNT_TYPE_H323)
        {
            continue;
        }

        if (acc_IsAccountEnable(pAccount->GetAccountId())
                && !pAccount->IsDirectIPAccount())
        {
            return false;
        }
    }
    return true;
}

#if 1
//定时器处理
BOOL CAccountManager::OnTimer(UINT uTimeID)
{
    BOOL bHandle = FALSE;
    if (uTimeID == (UINT)&m_iCheckLogOutTime)
    {
        bHandle = TRUE;
        --m_iCheckLogOutTime;

        if (GetRegisteredAccountNum() == 0 || 0 == m_iCheckLogOutTime)
        {
            m_iCheckLogOutTime = 0;
            timerKillThreadTimer((UINT)&m_iCheckLogOutTime);
            AdapterBox_ResetFactory((int)RESET_OPTION_ALL);
        }
    }
    return bHandle;
}

// 恢复出厂配置前，通知SIP到服务器注销帐号，已经全部注销返回TRUE，否则返回FALSE
bool CAccountManager::LogoutBeforeReset()
{
    // 是否所有账号都注销了
    bool bAllAccDisable = true;
    for (CAccountList::iterator it = m_listAccount.begin(); it != m_listAccount.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount
                && pAccount->IsDirectIPAccount() == false
                && pAccount->GetAccountState() != LS_DISABLE
#ifdef IF_FEATURE_PSTN
                && pAccount->GetAccountType() != ACCOUNT_TYPE_PSTN
#endif
           )
        {
            bAllAccDisable = FALSE;
            acc_SetAccountEnable(pAccount->GetAccountId(), false);
            msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, pAccount->GetAccountId());
        }
    }

    // 如果有未注销账号，则设置0.5秒的定时器检测30次
    if (!bAllAccDisable)
    {
        m_iCheckLogOutTime = 30;
        timerSetThreadTimer((UINT)&m_iCheckLogOutTime, 500);
    }

    return bAllAccDisable;
}

void CAccountManager::ResetDefaultAccount()
{
    ACCOUNT_INFO("CAccountManager::ResetDefaultAccount");
    int iDefaultAccountId = -1;
    int iAccountId = configParser_GetConfigInt(kszAccountDefaultID);
    CBaseAccount * pAccount = GetAccountByAccountId(iAccountId);
    CBaseAccount * pUsableAccount = GetUsableAccount(true);
#ifdef IF_FEATURE_H323
    CBaseAccount * pH323P2P = GetH323DirectIPAccount();
#endif //#ifdef IF_FEATURE_H323

#if DEBUG_ACCOUNT
    ACCOUNT_INFO("Default account ID in config: %d(ptr=%p), first usable account: %d(ptr=%p)",
                 iAccountId, pAccount, (pUsableAccount ? pUsableAccount->GetAccountId() : INVALID_ACCOUNTID),
                 pUsableAccount);
#endif // DEBUG_ACCOUNT
    if (pAccount != NULL
            && pAccount->IsAccountUsable())
    {
        iDefaultAccountId = iAccountId;
    }
    else if (pUsableAccount != NULL)
    {
        iDefaultAccountId = pUsableAccount->GetAccountId();
    }
#ifdef IF_FEATURE_H323
    else if (pH323P2P != NULL
             && pH323P2P->IsAccountUsable())
    {
        iDefaultAccountId = pH323P2P->GetAccountId();
    }
#endif //#ifdef IF_FEATURE_H323
    else if (IsAllowIPCall() && GetDirectIPAccount() != NULL)
    {
        iDefaultAccountId = GetDirectIPAccount()->GetAccountId();
    }
    else
    {
        iDefaultAccountId = INVALID_ACCOUNTID;
    }

    SetDefaultAccount(iDefaultAccountId);
}

void CAccountManager::GetAccountInfo(yl::string & strInfoOut,
                                     int eAccountType /* = ACCOUNT_TYPE_ALL */)
{
    //  [Accounts]
    //  AccountX=Enabled/Disabled|Label|UserName|DisplayName|AnonymousCall|
    //  AccountX=Enabled/Disabled|Label|UserName|DisplayName|AnonymousCall|FullName|
    //  AccountX=Enabled/Disabled|Label|

    CAccountList::iterator it = m_listAccount.begin();
    yl::string strTemp = "";
    strInfoOut += "[Accounts]\n";
    while (it != m_listAccount.end())
    {
        CBaseAccount * pAccount = (*it);
        if (pAccount
                && (eAccountType & pAccount->GetAccountType())
                && !pAccount->IsDirectIPAccount())
        {
            int iAccount = pAccount->GetAccountId();
            strTemp = commonAPI_FormatString("Account%d=", iAccount);
            strTemp = strTemp + (acc_IsAccountEnable(iAccount) ? "Enabled" : "Disabled") + "|"
                      + acc_GetLabel(iAccount) + "|";
            if (pAccount->GetAccountType() == ACCOUNT_TYPE_SIP
                    || pAccount->GetAccountType() == ACCOUNT_TYPE_H323)
            {
                strTemp = strTemp + acc_GetUsername(iAccount) + "|"
                          + acc_GetDisplayName(iAccount) + "|"
                          + (configParser_GetCfgItemIntValue(kszAnonymousCallSwitch, iAccount) ?  "On" : "Off") + "|";
                if (!acc_GetUsername(iAccount).empty())
                {
                    strTemp = strTemp + acc_GetFullName(iAccount) + "|";
                }
            }
            strTemp = strTemp + "\n";

            strInfoOut = strInfoOut + strTemp;
        }
        ++it;
    }
}

bool CAccountManager::SetAccountInfo(yl::string & strInfoIn)
{
    LPCSTR lpCfgContent = strInfoIn.c_str();

    int iAccID = 0;
    commonUnits_GetIntValue(lpCfgContent, "account", iAccID);
    iAccID = (iAccID - 1 < 0) ? 0 : iAccID - 1;
    if (iAccID < SIP_LINE_BEGIN_ID || iAccID > SIP_LINE_END_ID)
    {
        return false;
    }

    int iEnable = 1;
    commonUnits_GetIntValue(lpCfgContent, "Enable", iEnable);
    acc_SetAccountEnable(iAccID, iEnable != 0);

    char lpValue[128] = "";
    commonUnits_GetStringValue(lpCfgContent, "sipServer", lpValue);
    if (0 != strlen(lpValue))
    {
        if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
        {
            configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccID, 1, lpValue);
        }
        else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
        {
            configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccID, lpValue);
        }
    }

    memset(lpValue, 0, sizeof(lpValue));
    commonUnits_GetStringValue(lpCfgContent, "sipServerPort", lpValue);
    if (0 != strlen(lpValue))
    {
        if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
        {
            configParser_SetCfgItemStringValue(kszAccountSIPServer1Port, iAccID, 1, lpValue);
        }
        else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
        {
            configParser_SetCfgItemStringValue(kszAccountSIPServer1Port, iAccID, lpValue);
        }
    }

    memset(lpValue, 0, sizeof(lpValue));
    commonUnits_GetStringValue(lpCfgContent, "Name", lpValue);
    if (0 != strlen(lpValue))
    {
        configParser_SetCfgItemStringValue(kszAccountUserName, iAccID, lpValue);
        configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccID, lpValue);
        acc_SetAccountLabel(iAccID, lpValue);
        configParser_SetCfgItemStringValue(kszAccountDisplayName, iAccID, lpValue);
    }

    memset(lpValue, 0, sizeof(lpValue));
    commonUnits_GetStringValue(lpCfgContent, "password", lpValue);
    if (0 != strlen(lpValue))
    {
        configParser_SetCfgItemStringValue(kszAccountPassword, iAccID, lpValue);
    }

    // 广播账号配置改变
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE,
                       iAccID);
    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccID);

    return true;
}

void CAccountManager::BackupRestoreCfg(int iAccountId, bool bIsRestore)
{
//     if (acc_IsAccountIDValid(iAccountId))
//     {
//         return;
//     }
//     yl::string strCfg = commonAPI_FormatString("%s%d.cfg", szSIPAccountFile, iAccountId);
//     yl::string strBackupCfg = commonAPI_FormatString("%s%d.cfg.HotDesk", szSIPAccountFile, iAccountId);
//     yl::string strCommand = "cp ";
//     if (bIsRestore)
//     {
//         if (!pathFileExist((LPCSTR)strBackupCfg))
//         {
//             return;
//         }
//         strCommand += strBackupCfg + " " + strCfg;
//     }
//     else
//     {
//         if (!pathFileExist((LPCSTR)strCfg))
//         {
//             return;
//         }
//         strCommand += strCfg + " " + strBackupCfg;
//     }
// #ifdef WIN32
// #else
//     m_objCfgMutex.Lock();
//     system((char*)(LPCTSTR)strCommand);
//     m_objCfgMutex.Unlock();
//     if (bIsRestore)
//     {
//         m_iCfgStatus = m_iCfgStatus & ~(0x01 << iAccountId);
//         msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountId);
//     }
//     else
//     {
//         m_iCfgStatus = m_iCfgStatus | (0x01 << iAccountId);
//     }
// #endif
}


#endif  //IF_BUG_20555

CBaseAccount * CAccountManager::GetEmergencyAccount()
{
#ifdef IF_FEATURE_EMERGENCY
    // 1. 找可用账号（不含IP直播）：默认账号、第一个可用的非IP直播账号
    CBaseAccount * pAccount = GetUsableAccount();
    if (pAccount != NULL && !pAccount->IsSpecialAccount())
    {
        return pAccount;
    }

    // 2. 取第一个没被禁用的账号
    pAccount = GetNoEmptyServerAccount(true);
    if (pAccount != NULL)
    {
        return pAccount;
    }

    // 5. 取IP直播账号
    pAccount = GetDirectIPAccount(false);
    if (pAccount != NULL)
    {
        return pAccount;
    }

    // 6. 没得挑了第一个账号（warring：应该不会跑到这里）
    if (m_listAccount.size() > 0)
    {
        return *(m_listAccount.begin());
    }
#endif
    return NULL;
}

bool CAccountManager::SetAutoAnswer()
{
    if (acc_IsAccountAutoAnswerEnable(m_iDefaultAccountId))
    {
        //默认账号不是无效账号，IP直拨账号不显示AA状态
        if (m_iDefaultAccountId != INVALID_ACCOUNTID
                && !acc_IsDirectIPAccount(m_iDefaultAccountId))
        {
            idleScreen_SetStatus(PS_STATE_AA, "");
            return true;
        }
    }

    return idleScreen_DropStatus(PS_STATE_AA);
}

void CAccountManager::SetTransPort(msgObject & objMessage)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(GetAccountByAccountId(objMessage.wParam));
    if (pAccount != NULL)
    {
        pAccount->SetTransPort(objMessage.lParam);
    }
}


// 账号是否支持H264
bool CAccountManager::IsSupportH264(int iAccountId)
{
    ACCOUNT_TYPE eAccType = acc_GetAccountType(iAccountId);
    CBaseAccount * pAccount = GetAccountByAccountId(iAccountId, false);
#ifdef IF_SUPPORT_VIDEO
    if (pAccount != NULL
            && eAccType == ACCOUNT_TYPE_SIP)
    {
        CSipAccount * pSipAccount = dynamic_cast<CSipAccount *>(pAccount);
        if (pSipAccount != NULL)
        {
            return pSipAccount->IsSupportH264();
        }
    }
#endif //#ifdef IF_SUPPORT_VIDEO
#ifdef IF_FEATURE_H323
    else if (pAccount != NULL
             && eAccType == ACCOUNT_TYPE_H323)
    {
        CH323Account * pH323Account = dynamic_cast<CH323Account *>(pAccount);
        if (pH323Account != NULL)
        {
            return pH323Account->IsSupportH264();
        }
    }
#endif //#ifdef IF_FEATURE_H323

    return false;
}
