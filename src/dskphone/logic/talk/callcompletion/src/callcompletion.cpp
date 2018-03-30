#include "callcompletion.h"

#include <assert.h>

#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "voice/include/modvoice.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#if !defined(_T4X) && !defined(_DECT)
#include "uimanager/moduimanager.h"
#endif
#include "idlescreen/include/modidlescreen.h"
#include "voip/modvoip.h"
#include "singletonclassdefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "devicelib/networkserver.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "adapterbox/include/modadapterbox.h"
#include "modcallcompletion.h"

#if !defined(min)
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

namespace NS_CC
{
IMPLEMENT_GETINSTANCE(CCallCompletion)

//////////////////////////////////////////////////////////////////////////
// Class CCallCompletion.
BOOL CCallCompletion::m_bEnableCallCompletion = FALSE;

// Static.
LRESULT CCallCompletion::OnSipMessage(msgObject & refMessage)
{
#if IF_BUG_23548
    if (configParser_GetConfigInt(kszCallCompletionCallBackSwitch) == 1
            && refMessage.message == SIP_CALL_FAIL)
    {
        if (NULL != refMessage.GetExtraData())
        {
            sip_reason_info_t * pReasonInfo = (sip_reason_info_t *)refMessage.GetExtraData();
            CC_INFO("CCallCompletion::OnSipMessage SIP_CALL_FAIL nLine[%d] code[%d]", refMessage.wParam,
                    (int)pReasonInfo->code);
            if (pReasonInfo->code == 486)
            {
                CallCompletion_SetCallBackData(refMessage.wParam, true);
            }
        }
    }
#endif

    if (!m_bEnableCallCompletion)
    {
        return 0;
    }

    // 处理消息.
    switch (refMessage.message)
    {
    case SIP_CALL_ESTABLISH:
        {
            if (NULL == refMessage.GetExtraData())
            {
                break;
            }

            // 只处理establish消息
            sip_channels_v20_t sipChannelInfo;
            Comm_GetSipChannelsData(sipChannelInfo, refMessage);

            if (SIP_CALL_HOLD_DEFAULT != sipChannelInfo.hold_state)
            {
                break;
            }

            // 因为我们在通话开始之前就订阅了, 所以在通话成功建立时,需要取消订阅.
            CC_INFO("CCallCompletion::OnSipMessage SIP_CALL_ESTABLISH");
            CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(refMessage.lParam);
            if (pCallInfo)
            {
                //去除端口号
                yl::string::size_type nPortPos = pCallInfo->m_tRemoteInfo.sServerName.find(':');
                if (nPortPos != yl::string::npos)
                {
                    pCallInfo->m_tRemoteInfo.sServerName.erase(nPortPos);
                }
                _CallCompletion.Unsubscribe(refMessage.wParam,
                                            pCallInfo->m_tRemoteInfo.sSIPName.c_str(), pCallInfo->m_tRemoteInfo.sServerName.c_str());
            }
        }
        break;
    case SIP_CALL_FAIL:
        {
            if (NULL == refMessage.GetExtraData())
            {
                break;
            }

            sip_reason_info_t * pReasonInfo = (sip_reason_info_t *)refMessage.GetExtraData();
            CC_INFO("CCallCompletion::OnSipMessage SIP_CALL_FAIL LPARAM[%d]", (int)pReasonInfo->code);

            // 目前我们只支持CCBS, 不支持CCNP, CCNR.
            // 所以只有对方繁忙的情形下, 我们才继续订阅流程, 否则取消订阅.
            // WPARAM-Call Id. LPARAM-Sip Failed Id.
            if (pReasonInfo->code != 486)
            {
                return _CallCompletion.Unsubscribe((int)refMessage.lParam);
            }
            else
            {
                // 如果订阅成功, 则更改状态为等待用户确认.
                if (_CallCompletion.GetSubscribeStatus() == CCS_SUBSCRIBE_SUCCESS)
                {
                    return _CallCompletion.UpdateSubscribeStatus((int)refMessage.lParam, CCS_WAIT_COMFIMING);
                }
            }
        }
        break;
    case SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT:
        // 订阅结果消息.
        // WPARAM-Account Id. LPARAM-1订阅成功;0订阅失败. Extra-struct sip_user_info_t.
        {
            int nAccountId = (int)refMessage.wParam;
            BasicUserInfo stSUI;
            MapSipUserInfo_BasicInfo((sip_user_info_t *)refMessage.GetExtraData(), stSUI);

            CC_INFO("CCallCompletion SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT result[%d], user name = %s, server name = %s",
                    (int)refMessage.lParam, stSUI.strNumber.c_str(), stSUI.strServer.c_str());
            if (refMessage.lParam == 1)
            {
                // 订阅成功后更改状态.
                int iStatus = _CallCompletion.GetSubscribeStatus(nAccountId, stSUI.strNumber.c_str(),
                              stSUI.strServer.c_str());
                if (CCS_SUBSCRIBING == iStatus)
                {
                    _CallCompletion.UpdateSubscribeStatus(nAccountId, stSUI.strNumber.c_str(), stSUI.strServer.c_str(),
                                                          CCS_SUBSCRIBE_SUCCESS);
                }
            }
            else if (refMessage.lParam == 0)
            {
                // 订阅失败后, 则从删除订阅.
                // 如果订阅失败在用户确认之前返回, 则对用户没有提示.
                // 如果订阅失败在用户确认之后返回, 则用户可以提示并选择.
                // 两种情况下都会删除.
                _CallCompletion.Unsubscribe(nAccountId, stSUI.strNumber.c_str(), stSUI.strServer.c_str());
            }
        }
        break;
    case SIP_SUB_UPDATE_CC_STATUS:
        // 对方状态发生变化的消息.
        // WPARAM-Account Id. LPARAM-CN_BLF_STATUS_IDLE对方空闲. 其他：对方忙
        // Extra-struct sip_user_info_t.// 该信息为本方的信息
        {
            int nAccountId = (int)refMessage.wParam;
            BasicUserInfo stSUI;
            MapSipUserInfo_BasicInfo((sip_user_info_t *)refMessage.GetExtraData(), stSUI);
            if (NULL == _CallCompletion.MyFind(nAccountId, stSUI.strNumber.c_str(), stSUI.strServer.c_str(),
                                               FALSE))
            {
                break;
            }

            // 对方空闲, 只有在订阅状态等待对方状态时, 才更改状态.
            // SIP发送的消息sip_user_info_t为发起订阅方的信息, 无法通过
            // 具体信息取消.
            // Note: 因为我们只支持一个订阅, 所以此处取消该
            // 账号下的所有订阅. 后期如果支持多个, 请更改此处.
            int nCurStatus = _CallCompletion.GetSubscribeStatus();
            CC_INFO("SIP_SUB_UPDATE_CC_STATUS, account id[%d], remote status[%d], current status[%d]",
                    nAccountId, refMessage.lParam, nCurStatus);
            if (Comm_MapSipBLFStatus(refMessage.lParam) == CN_BLF_STATUS_IDLE)
            {
                if (nCurStatus == CCS_WAIT_REMOTE_IDLE
                        || nCurStatus == CCS_WAIT_COMFIMING)
                {
                    BOOL bSuccess = _CallCompletion.UpdateSubscribeStatus(CCS_REMOTE_CALLAVALIABLE);

                    // 发送消息提示UI.
                    if (bSuccess)
                    {
                        // 本地空闲时能能弹出提示框,否则只响提示音
                        if (AdapterBox_IsInIdle())
                        {
                            voice_PlayTone(TONE_STUTTER, 1000, CHANNEL_HANDFREE);
                            FlushIdleStatus();
                        }
                        else
                        {
                            CC_INFO("call completion active, only play tone");
                            // 当前不是idle界面和通话状态,就播放提示音
                            if (!talklogic_SessionExist())
                            {
                                voice_PlayTone(TONE_STUTTER, 1000, CHANNEL_HANDFREE);
                            }
                        }
                    }
                }
            }
            else
            {
                // 对方不空闲, 则如果当前状态对可拨出时, 更改状态回等待对方空闲.
                if (nCurStatus == CCS_REMOTE_CALLAVALIABLE)
                {
                    _CallCompletion.UpdateSubscribeStatus(CCS_WAIT_REMOTE_IDLE);
                    FlushIdleStatus();
                }
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

// Static.
LRESULT CCallCompletion::OnBCMessage(msgObject & refMessage)
{
    CC_INFO("Received message[0x%x]", (unsigned int)refMessage.message);

    switch (refMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            // 配置文件发生改变的消息.
            switch (refMessage.wParam)
            {
            case ST_CALLCOMPLETION:
                // 如果Call Completion关闭了, 则取消所有订阅.
                CC_INFO("CCallCompletion::OnBCMessage ST_CALLCOMPLETION");
                m_bEnableCallCompletion = (configParser_GetConfigInt(kszCallCompletionSwitch) == 1);
                if (!m_bEnableCallCompletion)
                {
                    _CallCompletion.UnsubscribeAll();
                    // 退出提示界面
                    FlushIdleStatus();
                }
                break;
            default:
                break;
            }
        }
        break;
    case ACCOUNT_STATUS_CHANGED:
        {
            // 账号状态发生改变的消息.
            // WPARAM-Account Id. LPRAM-LINE_STATE. Extra-info(XML format).
            LINE_STATE eCurrentState = acc_GetState(refMessage.wParam);
            if (eCurrentState != LS_REGISTERED)
            {
                CC_INFO("OnBCMessage SIP_REG_UPDATE_REGISTER LPARAM[%d], wParam[%d]",
                        (int)eCurrentState, (int)refMessage.wParam);
                if (_CallCompletion.UnsubscribeAllByAccountId(refMessage.wParam))
                {
                    FlushIdleStatus();
                }
            }
        }
        break;
//     case SYS_MSG_NOTIFY_CABLE_CHANGED:
//         {
//                if(refMessage.lParam == SYS_CABLE_STATUS_OFF)
//                {
//                  // 如果网络没有连接上, 则取消所有订阅.
//                  g_objCallCompletion.UnsubscribeAll();
//                  // 退出提示界面
//                  etl_NotifyApp(FALSE, CALLCOMPLETION_MSG_ACTIVE, 0, 0);
//                  idleScreen_CloseMsgBox(IPT_CALL_COMPLETION);
//                }
//         }
//         break;
    default:
        break;
    }

    return TRUE;
}

BOOL CCallCompletion::Subscibe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                               const char * szCalleeUsername, const char * szCalleeServer)
{
    // 判断CallCaompletion是否开启, 如果未开启则不订阅.
    //  int nSwitch = configParser_GetConfigInt(kszCallCompletionSwitch);
    if (!m_bEnableCallCompletion)
    {
        CC_WARN("Subscibe: the switch[%d] is off!", m_bEnableCallCompletion);
        return FALSE;
    }

    // 取消订阅.
    assert(m_listItem.size() <= 1);

    ItemData * pItemData = NULL;
    if (m_listItem.size() >= 1)
    {
        pItemData = m_listItem.back();
        yl::string strRUserName = szCalleeUsername;
        if (pItemData != NULL)
        {
            CC_INFO("exist item, user_name = %s, server_name = %s",
                    pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str());
            if (pItemData->m_remoteInfo.strToNumber == strRUserName)
            {
                // 如果已经有订阅，则不再订阅
                return FALSE;
            }
            else
            {
                //由于只支持订阅1个，因此清除旧的订阅内容
                UnsubscribeAll();
            }
        }
    }

    if (!acc_IsAccountIDValid(nAccountId))
    {
        return FALSE;
    }

#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(nAccountId) == PROTOCOL_H323)
    {
        CC_WARN("H323 not support");
        return FALSE;
    }
#endif //IF_FEATURE_H323

    pItemData = new ItemData();
    // 首先加入到内存中.
    pItemData->m_nStatus = CCS_SUBSCRIBING;
    pItemData->m_nAccountId = nAccountId;
    pItemData->m_nCallId = nCallId;
    // 保存所有的信息
    pItemData->m_remoteInfo.strDisplay = szCalleeDisplayName;
    pItemData->m_remoteInfo.strNumber = szCalleeUsername;

    char * pTemp = (char *)strrchr(szCalleeServer, ':');
    if (NULL == pTemp)
    {
        pItemData->m_remoteInfo.strServer = szCalleeServer;
    }
    else
    {
        // 去掉端口
        pItemData->m_remoteInfo.strServer.assign(szCalleeServer, pTemp - szCalleeServer);
    }

    pItemData->m_localInfo.strDisplay = acc_GetDisplayName(nAccountId).c_str();
    yl::string strUserName;
    yl::string strServerName;
    if (acc_IsDirectIPAccount(nAccountId))
    {
        // 如果是IP直拨账号,用户名和服务器名则为对方的IP地址
        yl::string strWanIP;
        // networkInfo* pNetworkInfo = netGetNetworkInfo();
        if (netGetWanIP(strWanIP))
        {
            strServerName = strUserName = strWanIP;
        }
    }
    else
    {
        strUserName = acc_GetUsername(nAccountId);
        strServerName = acc_GetServerName(nAccountId);
    }
    pItemData->m_localInfo.strNumber = strUserName.c_str();
    pItemData->m_localInfo.strServer = strServerName.c_str();

    m_listItem.push_back(pItemData);

    return MySubscibe(nAccountId, nCallId, szCalleeDisplayName, szCalleeUsername, szCalleeServer);
}

// 取消订阅
BOOL CCallCompletion::Unsubscribe(int nCallId)
{
    // 查找相应项,需从链表中删除
    ItemData * pItemData = MyFind(nCallId, TRUE);
    if (pItemData != NULL)
    {
        MyUnsubscribe(pItemData->m_nAccountId, pItemData->m_nCallId,
                      pItemData->m_remoteInfo.strDisplay.c_str(),
                      pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str());

        delete pItemData;
        FlushIdleStatus();
        return TRUE;
    }

    return FALSE;
}

// 取消订阅
BOOL CCallCompletion::Unsubscribe(int nAccountId, const char * szCalleeUsername,
                                  const char * szCalleeServer)
{
    // 取消订阅,使用的对方的信息,需从链表中删除
    ItemData * pItemData = MyFind(nAccountId, szCalleeUsername, szCalleeServer, FALSE, TRUE);
    if (pItemData != NULL)
    {
        MyUnsubscribe(pItemData->m_nAccountId, pItemData->m_nCallId,
                      pItemData->m_remoteInfo.strDisplay.c_str(),
                      pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str());

        delete pItemData;
        FlushIdleStatus();
        return TRUE;
    }
    return FALSE;
}

// 取消所有的订阅
BOOL CCallCompletion::UnsubscribeAll()
{
    // 取消列表中的所有订阅.
    ListItem::iterator iter = m_listItem.begin();
    while (iter != m_listItem.end())
    {
        ItemData * pItemData = (*iter);
        if (pItemData != NULL)
        {
            MyUnsubscribe(pItemData->m_nAccountId, pItemData->m_nCallId,
                          pItemData->m_remoteInfo.strDisplay.c_str(),
                          pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str());
        }

        delete (*iter);
        (*iter) = NULL;
        iter = m_listItem.erase(iter);
        FlushIdleStatus();
    }  // for.

    return m_listItem.size() == 0;
}

// 根据账号取消订阅
BOOL CCallCompletion::UnsubscribeAllByAccountId(int nAccountId)
{
    // 取消账号下的所有订阅.
    ListItem::iterator iter = m_listItem.begin();
    BOOL bUnsubscribeSuccess = FALSE;
    while (iter != m_listItem.end())
    {
        ItemData * pItemData = (*iter);
        // 取消所有相应账号的订阅
        if (pItemData != NULL && pItemData->m_nAccountId == nAccountId)
        {
            bUnsubscribeSuccess |= MyUnsubscribe(pItemData->m_nAccountId, pItemData->m_nCallId,
                                                 pItemData->m_remoteInfo.strDisplay.c_str(),
                                                 pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str());

            delete (*iter);
            (*iter) = NULL;
            iter = m_listItem.erase(iter);
        }
        else
        {
            ++iter;
        }
    }  // for.

    return bUnsubscribeSuccess;
}

BOOL CCallCompletion::GetSubscribeData(CallCompletionData * pData)
{
    if (pData == NULL)
    {
        return FALSE;
    }

    if (!m_bEnableCallCompletion)
    {
        return FALSE;
    }

    if (m_listItem.size() >= 1)
    {
        // 目前只支持一项,取最后一个
        const ItemData * pItemData = m_listItem.back();
        if (pItemData != NULL)
        {
            pData->m_eStatus = static_cast<CallCompletionStatus>(pItemData->m_nStatus);
            pData->m_nCallId = pItemData->m_nCallId;
            pData->m_strCalleeDisplayName = pItemData->m_remoteInfo.strDisplay;
            pData->m_strCalleeUsername = pItemData->m_remoteInfo.strNumber;
            pData->m_strCalleeServer = pItemData->m_remoteInfo.strServer;

            CC_INFO("GetSubscribeData SUCCEED status[%d] CallId[%d] DName[%s] UName[%s] Server[%s]",
                    pData->m_eStatus, pData->m_nCallId, pData->m_strCalleeDisplayName.c_str(),
                    pData->m_strCalleeUsername.c_str(), pData->m_strCalleeServer.c_str());
            return TRUE;
        }
    }

    CC_WARN("CCallCompletion::GetSubscribeData FAILED!");
    return FALSE;
}

// 获取订阅状态
int CCallCompletion::GetSubscribeStatus(int nAccountId, const char * szCalleeUsername,
                                        const char * szCalleeServer)
{
    // 获取订阅状态.
    const ItemData * pItemData = MyFind(nAccountId, szCalleeUsername, szCalleeServer);
    return pItemData != NULL ? pItemData->m_nStatus : CCS_INVALID;
}

// 获取订阅状态
int CCallCompletion::GetSubscribeStatus()
{
    int nRet = CCS_INVALID;
    if (m_listItem.size() == 1)
    {
        ItemData * pItemData = m_listItem.back();
        if (pItemData != NULL)
        {
            nRet = pItemData->m_nStatus;
        }
    }
    return nRet;
}

// 更新订阅状态
BOOL CCallCompletion::UpdateSubscribeStatus(int nCallId, int nNewStatus)
{
    ItemData * pItemData = MyFind(nCallId, FALSE);
    if (NULL != pItemData)
    {
        CC_INFO("UpdateSubscribeStatus Old[%d]->New[%d]!", pItemData->m_nStatus, nNewStatus);
        pItemData->m_nStatus = nNewStatus;
        FlushIdleStatus();
        return TRUE;
    }

    CC_WARN("UpdateSubscribeStatus FAILED!");
    return FALSE;
}

// 更新订阅状态
BOOL CCallCompletion::UpdateSubscribeStatus(int nAccountId, const char * szCalleeUsername,
        const char * szCalleeServer, int nNewStatus)
{
    ItemData * pItemData = MyFind(nAccountId, szCalleeUsername, szCalleeServer);
    if (pItemData != NULL)
    {
        CC_INFO("UpdateSubscribeStatus Old[%d]->New[%d]!", pItemData->m_nStatus, nNewStatus);
        pItemData->m_nStatus = nNewStatus;

        FlushIdleStatus();

        return TRUE;
    }

    CC_WARN("CCallCompletion::UpdateSubscribeStatus FAILED!");
    return FALSE;
}

// 更新订阅状态
BOOL CCallCompletion::UpdateSubscribeStatus(int nNewStatus)
{
    if (m_listItem.size() == 1)
    {
        ItemData * pItemData = m_listItem.back();
        if (NULL != pItemData)
        {
            CC_INFO("UpdateSubscribeStatus Old[%d]->New[%d]!", pItemData->m_nStatus, nNewStatus);
            pItemData->m_nStatus = nNewStatus;
            FlushIdleStatus();
            return TRUE;
        }
    }

    CC_WARN("UpdateSubscribeStatus FAILED!");
    return FALSE;
}

// 呼出号码
BOOL CCallCompletion::CallOut(int nCallId)
{
    // 验证订阅状态.
    ItemData * pItemData = MyFind(nCallId, TRUE);
    if (NULL != pItemData && CCS_REMOTE_CALLAVALIABLE == pItemData->m_nStatus)
    {
        // 取消订阅.
        MyUnsubscribe(pItemData->m_nAccountId, pItemData->m_nCallId,
                      pItemData->m_remoteInfo.strDisplay.c_str(),
                      pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str());

        CC_INFO("CallOut callout DName[%s] UName[%s] Account[%d]",
                pItemData->m_remoteInfo.strDisplay.c_str(), pItemData->m_remoteInfo.strNumber.c_str(),
                pItemData->m_nAccountId);
        // 拨出.
        BOOL bCallOutResult = talklogic_CallOut(pItemData->m_remoteInfo.strNumber.c_str(),
                                                pItemData->m_remoteInfo.strDisplay.c_str(), pItemData->m_nAccountId);
        // 删除数据
        delete pItemData;
        return bCallOutResult;
    }

    return FALSE;
}

// 等待对方空闲
BOOL CCallCompletion::Wait(int nCallId)
{
    // 查找某个Call Id对应的订阅.
    ItemData * pItemData = MyFind(nCallId);
    if (pItemData != NULL && pItemData->m_nStatus == CCS_WAIT_COMFIMING)
    {
        CC_INFO("Wait Old[%d]->New[%d]", pItemData->m_nStatus, CCS_WAIT_REMOTE_IDLE);
        pItemData->m_nStatus = CCS_WAIT_REMOTE_IDLE;
        return TRUE;
    }
    return FALSE;
}

CCallCompletion::CCallCompletion()
{
    MyInit();
    m_pWaitResponseCCData = NULL;
#if IF_BUG_23548
    m_pCompletionCallBackData = new CompletionCallBackData();;
#endif
}

CCallCompletion::~CCallCompletion()
{
    if (m_pWaitResponseCCData != NULL)
    {
        delete m_pWaitResponseCCData;
        m_pWaitResponseCCData = NULL;
    }

#if IF_BUG_23548
    if (m_pCompletionCallBackData != NULL)
    {
        delete m_pCompletionCallBackData;
        m_pCompletionCallBackData = NULL;
    }
#endif

    MyFinal();
}

BOOL CCallCompletion::MyInit()
{
    // 注册SIP消息.
    SingleMsgReg(SIP_CALL_ESTABLISH, OnSipMessage);
    SingleMsgReg(SIP_CALL_FAIL, OnSipMessage);
    SingleMsgReg(SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT, OnSipMessage);
    SingleMsgReg(SIP_SUB_UPDATE_CC_STATUS, OnSipMessage);
    // 注册广播的消息.
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnBCMessage);
    SingleMsgReg(ACCOUNT_STATUS_CHANGED, OnBCMessage);

    // 初始化开关
    m_bEnableCallCompletion = (configParser_GetConfigInt(kszCallCompletionSwitch) == 1);

    return TRUE;
}

BOOL CCallCompletion::MyFinal()
{
    // 取消所有的订阅.
    UnsubscribeAll();
    // 取消广播消息的注册.
//     etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_CABLE_CHANGED, SYS_MSG_NOTIFY_CABLE_CHANGED, OnBCMessage);
    etl_UnregisterMsgHandle(SIP_REG_UPDATE_REGISTER, SIP_REG_UPDATE_REGISTER, OnBCMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnBCMessage);
    // 取消SIP消息的注册.
    etl_UnregisterMsgHandle(SIP_SUB_UPDATE_CC_STATUS, SIP_SUB_UPDATE_CC_STATUS, OnSipMessage);
    etl_UnregisterMsgHandle(SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT, SIP_SUB_UPDATE_CC_SUBSCRIBE_RESULT,
                            OnSipMessage);
    etl_UnregisterMsgHandle(SIP_CALL_FAIL, SIP_CALL_FAIL, OnSipMessage);
    etl_UnregisterMsgHandle(SIP_CALL_ESTABLISH, SIP_CALL_ESTABLISH, OnSipMessage);

    return TRUE;
}

// 订阅
BOOL CCallCompletion::MySubscibe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                                 const char * szCalleeUsername, const char * szCalleeServer)
{
    // 获取账号.
    if (acc_IsAccountAvailable(nAccountId))
    {
        BasicUserInfo stSUI = {0};
        stSUI.strDisplay = szCalleeDisplayName;
        stSUI.strNumber = szCalleeUsername;
        stSUI.strServer = szCalleeServer;

        // 发送消息.
        modVoip_SubscribCallCompletion(nAccountId, nCallId, TRUE, stSUI.strNumber.c_str());

        CC_INFO("MySubscibe: Subscribe CallId[%d] DName[%s] Uname[%s] Server[%s]",
                nCallId, stSUI.strDisplay.c_str(), stSUI.strNumber.c_str(), stSUI.strServer.c_str());
        return TRUE;
    }

    CC_WARN("CCallCompletion::MySubscibe: The account[%d] is unusable!", nAccountId);
    return FALSE;
}

// 取消订阅
BOOL CCallCompletion::MyUnsubscribe(int nAccountId, int nCallId, const char * szCalleeDisplayName,
                                    const char * szCalleeUsername, const char * szCalleeServer)
{
    if (acc_IsAccountAvailable(nAccountId))
    {
        BasicUserInfo stSUI;
        stSUI.strDisplay = szCalleeDisplayName;
        stSUI.strNumber = szCalleeUsername;
        stSUI.strServer = szCalleeServer;

        // 发送消息取消.
        modVoip_SubscribCallCompletion(nAccountId, nCallId, FALSE, stSUI.strNumber.c_str());

        CC_INFO("MyUnsubscribe: Unsubscribe CallId[%d] DName[%s] Uname[%s] Server[%s]",
                nCallId, stSUI.strDisplay.c_str(), stSUI.strNumber.c_str(), stSUI.strServer.c_str());

        return TRUE;
    }

    CC_WARN("MySubscibe: The account[%d] is unusable!", nAccountId);
    return FALSE;
}

// 查找订阅项
CCallCompletion::ItemData * CCallCompletion::MyFind(int nCallId,
        BOOL isRemoveFromList /* = FALSE */)
{
    // 遍历查找某个订阅.
    for (ListItem::iterator iter = m_listItem.begin(); iter != m_listItem.end(); ++iter)
    {
        ItemData * pItem = (*iter);
        if (pItem != NULL && pItem->m_nCallId == nCallId)
        {
            // 从链表中删除
            if (isRemoveFromList)
            {
                m_listItem.erase(iter);
            }

            return pItem;
        }
    }
    return NULL;
}

// 根据信息查找订阅项
CCallCompletion::ItemData * CCallCompletion::MyFind(int nAccountId, const char * szUsername,
        const char * szServer, BOOL bLocalInfo/* = FALSE*/, BOOL isRemoveFromList /* = FALSE */)
{
    // 遍历查找某个订阅.
    for (ListItem::iterator iter = m_listItem.begin(); iter != m_listItem.end(); ++iter)
    {
        ItemData * pItemData = (*iter);
        if (pItemData != NULL && pItemData->m_nAccountId == nAccountId)
        {
            BOOL bFound = FALSE;
            if (bLocalInfo)
            {
                CC_INFO("local user name = %s, server name = %s, match user name = %s, server = %s",
                        pItemData->m_localInfo.strNumber.c_str(), pItemData->m_localInfo.strServer.c_str(), szUsername,
                        szServer);
                // 比较本方信息
                if ((0 == strcmp(pItemData->m_localInfo.strNumber.c_str(), szUsername))
                        && (0 == strcmp(pItemData->m_localInfo.strServer.c_str(), szServer)))
                {
                    bFound = TRUE;
                }
            }
            else
            {
                CC_INFO("remote user name = %s, server name = %s, match user name = %s, server = %s",
                        pItemData->m_remoteInfo.strNumber.c_str(), pItemData->m_remoteInfo.strServer.c_str(), szUsername,
                        szServer);
                // 比较对方信息
                if ((0 == strcmp(pItemData->m_remoteInfo.strNumber.c_str(), szUsername))
                        && (0 == strcmp(pItemData->m_remoteInfo.strServer.c_str(), szServer)))
                {
                    bFound = TRUE;
                }
            }

            // 找到返回
            if (bFound)
            {
                if (isRemoveFromList)
                {
                    // 清除对应项
                    m_listItem.erase(iter);
                }
                return pItemData;
            }
        }
    }

    return NULL;
}

bool CCallCompletion::FlushIdleStatus()
{
    //先判断订阅是否有空闲
    CallCompletionData MyCallCompletionData;

    //获取订阅内容成功
    if (!CallCompletion_GetSubscribeData(&MyCallCompletionData))
    {
        idleScreen_DropStatus(PS_STATE_CALL_COMPLETION);
        return false;
    }

    if (MyCallCompletionData.m_eStatus == CCS_REMOTE_CALLAVALIABLE
            || MyCallCompletionData.m_eStatus == CCS_WAIT_COMFIMING)
    {
        idleScreen_SetStatus(PS_STATE_CALL_COMPLETION);
    }

    return true;
}

#if IF_BUG_23548
bool CCallCompletion::GetCallBackData(CompletionCallBackData & completionCallBackData)
{
    if (m_pCompletionCallBackData == NULL)
    {
        return false;
    }
    completionCallBackData = *m_pCompletionCallBackData;
    return true;
}

bool CCallCompletion::SetCallBackData(int nAccountId, bool bPopMsgBox)
{
    if (m_pCompletionCallBackData == NULL)
    {
        return false;
    }
    m_pCompletionCallBackData->nAccountId = nAccountId;
    m_pCompletionCallBackData->bPopMsgBox = bPopMsgBox;
    if (bPopMsgBox)
    {
        idleScreen_SetStatus(PS_STATE_CALL_COMPLETION_CALLBACK);
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_CALL_COMPLETION_CALLBACK);
    }
    return true;
}
#endif

}  // namespace NS_CC

