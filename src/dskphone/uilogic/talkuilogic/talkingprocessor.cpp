#include "talkingprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "talkuilogicprocessorname.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
// #include "settingui/include/modsettingui.h"
#include "talkuilogicframemanager.h"
#if IF_TALKUI_USB_ENABLE
#include "record/include/modrecord.h"
#endif
#include "uilogictalkinguibase.h"

#include "modtalkuilogic.h"

#include "broadsoft/xsiactionhelper/include/modxsicommon.h"
#include "application/include/modapplication.h"

CTalkingProcessor::CTalkingProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_TALKING)
{
    m_pTalkUI = NULL;

    m_bShowTalkTime = true;     //是否显示通话时间

    m_bRTPSoftkeySet = false;

#warning modRecord_IsIdle
//#if IF_TALKUI_USB_ENABLE
//    if (modRecord_IsIdle(RECORD_AUDIO)
//            && modRecord_IsRecordEnable(RECORD_AUDIO)
//            && 0 != configParser_GetConfigInt(kszUSBAutoRecordEnable))
//    {
//        TALKUI_INFO("Auto record when talk established.");
//        DealBeginRecord();
//    }
//#endif
}

CTalkingProcessor::~CTalkingProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CTalkingProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objTalkingUIData.Reset();

    m_bRTPSoftkeySet = false;
}

//释放前解除UI绑定前调用
void CTalkingProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CTalkingProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicTalkingUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CTalkingProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取callid
int CTalkingProcessor::GetCallID()
{
    return m_objTalkingUIData.m_nCallID;
}

//是否是视频通话
bool CTalkingProcessor::IsVideoTalk()
{
    return m_objTalkingUIData.m_bIsVideoTalk;
}

//获取是否是transferfiled
bool CTalkingProcessor::IsTransferFailed()
{
    return m_objTalkingUIData.m_bTransferFailed;
}

//获取是否parkfailed
bool CTalkingProcessor::IsParkFailed()
{
    return m_objTalkingUIData.m_bCallParkFailed;
}

//是否hold
bool CTalkingProcessor::IsHold()
{
    return m_objTalkingUIData.m_bLocalHold;

}

//是否转移后的通话状态
bool CTalkingProcessor::IsTransferred()
{
    if (TALKING_UI_TRANSFERRED == m_objTalkingUIData.m_eTalkUIType)
    {
        return true;
    }

    return false;
}

//获取保存的talkingui数据
CTalkingUIData * CTalkingProcessor::GetTalkingUIData()
{
    return &m_objTalkingUIData;
}

//是否显示resume的softkey
//因为transered 的判断较复杂  所以特别使用一个函数来判断
bool CTalkingProcessor::IsShowResumeSoftkey()
{
    //必须要处于hold 状态
    //http://10.2.1.199/Bug.php?BugID=88426 当m_bAllowHold 为true时候才可以显示 resume
    if (IsHold()
            && m_objTalkingUIData.m_bAllowHold
            && SESSION_TRANSFERRED != m_eSessionState
            //而且不是 transferd 处于不是transfer failed的时候 才能显示  http://10.3.5.199/Bug.php?BugID=69733
            && !(IsTransferred() && !IsTransferFailed())
       )
    {
        return true;
    }

    return false;
}

//是否显示Hold的softeky
bool CTalkingProcessor::IsShowHoldSoftkey()
{
    if (m_objTalkingUIData.m_bAllowHold
            && !m_objTalkingUIData.m_bLocalHold)
    {
        return true;
    }

    return false;
}

bool CTalkingProcessor::IsShowSecurityClassification()
{
    if (m_objTalkingUIData.m_bSecurity
#if IF_TALKUI_LINE_DEVICE
            && CALL_LINE_DEVICE != m_objTalkingUIData.m_objContactInfo.m_eCallLineType
#endif
       )
    {
        return true;
    }

    return false;
}

void CTalkingProcessor::DealSecurityResult(const yl::string & strSecurityLevel)
{
    //设置当前账号的安全等级
    talklogic_ModifyAccountOverrideLevel(m_objTalkingUIData.m_iSipAccount, strSecurityLevel);
}

//resume通话
void CTalkingProcessor::ResumeTalk()
{
    SendUIRequestAction(TALK_ACTION_RESUME);
}

//获取当前话路的焦点联系人信息
bool CTalkingProcessor::GetFocusContactInfo(CContactInfo & objContactInfo)
{
    objContactInfo = m_objTalkingUIData.m_objContactInfo;

    return true;
}

//设置数据
void CTalkingProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    // 入口参数检查
    if (NULL == pData)
    {
        return;
    }

    //这边不能重置，因为有些数据是不能在这修改的
    //先把数据重置一下
//  m_objTalkingUIData.Reset();

    //设置是否有新来电标志位
    yl::string strIncomingCallNum = "";
    m_objTalkingUIData.m_strIncomingCallNum = "";

    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        m_objTalkingUIData.m_bHasCallComming = true;
        m_objTalkingUIData.m_strIncomingCallNum = strIncomingCallNum;
    }
    else
    {
        m_objTalkingUIData.m_bHasCallComming = false;
    }

    //更新音频Mute状态
    m_objTalkingUIData.m_bAudioMute = _TalkUILogicDelegate.GetAudioMuteStatus();

    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);

    // 通话对应的UI类型
    m_objTalkingUIData.m_eTalkUIType = pTalkingData->eTalkUIType;

    m_objTalkingUIData.m_bTransferFailed = pTalkingData->bTranFailed;
    m_objTalkingUIData.m_bCallParkFailed = pTalkingData->bCallParkFailed;
    m_objTalkingUIData.m_strParkFailedInfo = "";
    m_objTalkingUIData.m_strParkFailedInfoFirst =
        "";   //park失败原因的第一部分       //因为t46 在有头像时失败原因要分两行显示  这边为兼容t46
    m_objTalkingUIData.m_strParkFailedInfoSecond = "";  //park失败原因的第二部分

    m_objTalkingUIData.m_bEncrypt = pTalkingData->bEncrypt;

    //安全等级
    m_objTalkingUIData.m_bSecurity = pTalkingData->bAllowSecurity;
    if (m_objTalkingUIData.m_bSecurity)
    {
        // 获取话路对应的安全等级
        m_objTalkingUIData.m_objContactInfo.strSecurityLevel = talklogic_GetTalkSecurityLevel(m_iSessionID);
    }
    else
    {
        m_objTalkingUIData.m_objContactInfo.strSecurityLevel = "";
    }

    //有callpark 失败提醒才需要获取callpark失败信息
    if (m_objTalkingUIData.m_bCallParkFailed)
    {
        //失败提示长度
        int iParkFailedLen = pTalkingData->strParkFiled.length();
        // : 的位置
        int iFindIndex = pTalkingData->strParkFiled.find_first_of(':');
        //没有找到 : 那么直接翻译就好了
        if (iFindIndex < 0)
        {
            m_objTalkingUIData.m_strParkFailedInfo = _UILOGIC_LANG_TRAN(
                        pTalkingData->strParkFiled.c_str());    //能在uilogic翻译的就翻译
        }
        else    //有找到  :
        {
            // : 前半部分   长度为iFindIndex 就行了，因为到 : 前一位
            yl::string strParkFailedFirst = pTalkingData->strParkFiled.substr(0, iFindIndex);
            strParkFailedFirst = _UILOGIC_LANG_TRAN(strParkFailedFirst.c_str());
            m_objTalkingUIData.m_strParkFailedInfoFirst =
                strParkFailedFirst;   //park失败原因的第一部分       //因为t46 在有头像时失败原因要分两行显示  这边为兼容t46

            m_objTalkingUIData.m_strParkFailedInfo = strParkFailedFirst;
            //因为已经有找到 : 了  所以 : 是必须加的
            m_objTalkingUIData.m_strParkFailedInfo += ":";

            //错误提示 的最后一位 不是 :  那么要获取 后半部分 翻译
            if (iParkFailedLen > (iFindIndex + 1))
            {
                //要从: 的下一位 开始获取
                yl::string strParkFailedSecond = pTalkingData->strParkFiled.substr(iFindIndex + 1,
                                                 (iParkFailedLen - (iFindIndex + 1)));
                strParkFailedSecond = _UILOGIC_LANG_TRAN(strParkFailedSecond.c_str());
                m_objTalkingUIData.m_strParkFailedInfoSecond =
                    strParkFailedSecond; //park失败原因的第二部分

                m_objTalkingUIData.m_strParkFailedInfo += strParkFailedSecond;
            }
        }
    }

    m_objTalkingUIData.m_bAllowTran = pTalkingData->bAllowTran;                 //是否允许trans
    m_objTalkingUIData.m_bAllowSwap = pTalkingData->bAllowSwap;                 //是否允许swap
    m_objTalkingUIData.m_bAllowConf =
        pTalkingData->bAllowConf;                 //是否允许组建会议
//  m_objTalkingUIData.m_bVoiceEncryt = pTalkingData->bEncrypt;     //通话语音是否加密

    m_objTalkingUIData.m_bAllowNewCall = pTalkingData->bAllowNewCall;       //是否允许newcall

    CCallInfo * pCallInfo = pTalkingData->pCallInfo;

    if (pCallInfo != NULL)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
        m_eRecordStatue = pCallInfo->m_eRecordStatus;
        TALKUI_INFO("m_eRecordStatue : %d", m_eRecordStatue);
#endif

        m_objTalkingUIData.m_eAudioCodec = pCallInfo->GetAudioCodecType();

        //从callinfo获取联系人信息
        GetContactInfoFromCallInfo(m_objTalkingUIData.m_objContactInfo, pCallInfo);

        //网络会议要改变名称和 显示图片
        // 如果是网络会议则需要单独刷图片（此时无法从联系人获取到头像）
        if (pTalkingData->eTalkUIType == TALKING_UI_NETWORKCONF)
        {
            //根据lxh意见 这边不用翻译  意义不大， 所以只需要更换图片就可以了
            ////多次翻译不会出现问题， 如果多次返回会出问题 那么需要 在具体看情况了
            ////名称确认要翻译
            //m_objTalkingUIData.m_objContactInfo.strName = _UILOGIC_LANG_TRAN(m_objTalkingUIData.m_objContactInfo.strName.c_str());
            ////号码也翻译一下 因为都是conference
            //m_objTalkingUIData.m_objContactInfo.strNumber = _UILOGIC_LANG_TRAN(m_objTalkingUIData.m_objContactInfo.strNumber.c_str());


#if IF_TALKUI_CONTACT_PHOTO
            //图片确认
            //  yl::string strNetworkConfImage = PIC_CONTACTS_CONFERENCE;
            yl::string strNetworkConfImage = _TalkUILogicFrameManager.GetPicContactsConference();
            m_objTalkingUIData.m_objContactInfo.strContactImage = strNetworkConfImage;
#endif
        }

        //更新本地hold属性
        m_objTalkingUIData.m_bLocalHold = pCallInfo->IsLocalHold();
        m_objTalkingUIData.m_bRemoteHold = pCallInfo->IsRemoteHold();        //更新远程hold属性
        m_objTalkingUIData.m_eTalkState = pCallInfo->m_eTalkState;
        m_objTalkingUIData.m_nCallID = pCallInfo->GetCallID();
        m_objTalkingUIData.m_iSipAccount = pCallInfo->GetAccountID();        //sip账号

        //      m_objTalkingUIData.m_eRecrodIcon = GetRecordIconTypeFromCallInfo(pCallInfo);
        m_objTalkingUIData.m_blaType = pCallInfo->GetBLAType();
        m_objTalkingUIData.m_callType = pCallInfo->m_eCallType;
        //      m_objTalkingUIData.m_bIsHD = pCallInfo->IsHDTalking();          //是否高清通话
        //http://10.2.1.199/Bug.php?BugID=102785 之前获取的值是错误的  pCallInfo->m_bHoldAvailable

        m_objTalkingUIData.m_bAllowHold = true;     //是否允许显示Hold
        m_objTalkingUIData.m_bInTransferCanHold = true;

#if IF_TALKUI_FIVE_PARTY
        m_objTalkingUIData.m_bAllowJoin = pTalkingData->bAllowJoin;
#endif

#if IF_TALKUI_PSTN
        // 显示PSTN账号无效的提示（PSTN通话）
        if (CALL_LINE_PSTN == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
        {
            if (acc_IsAccountIDValid(m_objTalkingUIData.m_iSipAccount))
            {
                m_eNetworkStatus = TALK_NETWORK_STATUS_READY;
            }
            else
            {
                // PSTN账号无效
                TALKUI_INFO("Current PSTN account is unavailable.");
                // 如果PSTN账号失效，则重新插入也不能使当前通话恢复
                m_eNetworkStatus = TALK_NETWORK_STATUS_PSTN_FAIL;
            }
        }

        // Flashing提示
        if (CALL_LINE_PSTN == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
        {
            m_objTalkingUIData.m_bPstnFlashing = pstnadapter_IsFlashing(acc_GetDeviceIDByAccountID(
                    m_objTalkingUIData.m_iSipAccount));
            TALKUI_INFO("Current PSTN account is %sflashing.",
                        m_objTalkingUIData.m_bPstnFlashing ? "" : "not ");
        }
#endif

        m_objTalkingUIData.m_bIsVideoTalk = false;
        m_objTalkingUIData.m_objContactInfo.bVideoTalk =
            false;         //这边联系人相关信息 的标识位 也设置为 非 视频通话

#if IF_TALKUI_LINE_DEVICE
        if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
        {
            m_bShowTalkTime = false;
        }
        else
        {
            m_bShowTalkTime = true;
        }
#endif
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (m_pTalkUI != NULL)
    {
        m_pTalkUI->ShowErrorNote(pCallInfo->m_eRecordResult);
    }
#endif
}

// 通道键处理
bool CTalkingProcessor::OnChannelKeyPress(int iKey)
{
#if IF_TALKUI_PSTN
    // flash状态下过滤挂通道键
    if (m_objTalkingUIData.m_bPstnFlashing)
    {
        TALKUI_WARN("Flashing, will not response to Channel key.");

        return true;
    }
#endif

    bool bHandled = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
#if IF_TALKUI_HANDFREE_DIAL
            if (acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL))
            {
                // 新建一路
                SendUIRequestAction(TALK_ACTION_NEWCALL);
            }
            else
#endif
                if (voice_IsCallCenterMode())
                {
                    // callCenterMode交给基类处理
                    bHandled = false;
                }
                else
                {
                    // 通话中摘手柄时按免提键，则打开群听模式
                    if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                    {
                        if (m_objTalkingUIData.m_eTalkState == TS_TALK)
                        {
                            TALKUI_INFO("CTalkingUI : Open GroupListening");
                            if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
                            {
                                voice_SwitchChannel(CHANNEL_GROUP);
                            }
                            else
                            {
                                voice_SwitchChannel(CHANNEL_HANDFREE);
                            }
                            bHandled = true;
                        }
                    }
                    else
                    {
                        bHandled = false;
                    }
                }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
#if IF_TALKUI_LINE_DEVICE
            if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
            {
                bHandled = true;
            }
            else
#endif
            {
                if (!IsChannelKeyToExit(iKey))
                {
                    bHandled = true;
                    voice_SwitchChannel();
                }
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    if (!bHandled)
    {
        bHandled = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

bool CTalkingProcessor::DealKeyPreProcess(int iKey/* = 0*/)
{
    //为什么之前只判断 transfer 失败 ？？？？
    //  if (m_bTransferFailed)
    if (m_objTalkingUIData.m_bTransferFailed || m_objTalkingUIData.m_bCallParkFailed)
    {
        //能不能由UI自己来移除
        SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
        // 通道按键继续往下传
    }

    return false;
}

// 功能键处理
bool CTalkingProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_HOLD:
        {
            // http://10.3.5.199/Bug.php?BugID=62591
            // 在transfered 状态下  不处理   且当前不是transferfailed状态
            if (TALKING_UI_TRANSFERRED == m_objTalkingUIData.m_eTalkUIType
                    && false == m_objTalkingUIData.m_bTransferFailed)
            {
                return true;
            }

            //http://10.2.1.199/Bug.php?BugID=88426 当m_bAllowHold为false的时候界面不显示hold/resume，所以硬按键要拦截
            if (!m_objTalkingUIData.m_bAllowHold)
            {
                return true;
            }

            //改变hold状态
            SendUIRequestAction(TALK_ACTION_HOLD_SWITCH);

            return true;
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            //Transfer按键
            SendUIRequestAction(TALK_ACTION_TRAN);

            return true;
        }
        break;
    case PHONE_KEY_CONFERENCE:
        {
            //Conference按键
            SendUIRequestAction(TALK_ACTION_CONF);

            return true;
        }
        break;
    case PHONE_KEY_MUTE:
        {
            //hwz 确认  hold状态下 不能mute 这个在逻辑层会有限制
            //所以 其实这边不需要在做限制可以
            //Mute按键
            if (false == m_objTalkingUIData.m_bRemoteHold)
            {
                SendUIRequestAction(TALK_ACTION_MUTE);
            }
            return true;
        }
        break;
    case PHONE_KEY_OK:
        {
#ifdef IF_FEATURE_OK_TO_FLASH
            if (_TalkUILogicDelegate.IsJoyce())
            {
                // Joyce要求按OK键的时候将Flash发给SIP处理
                SendUIRequestAction(TALK_ACTION_OEM_FLASH);
            }
            else
#endif
            {
                SendUIRequestAction(TALK_ACTION_OK);
            }

            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            //如果x键要实现mute功能 _T21和_T23才需要
            if (true == _TalkUILogicDelegate.IsMuteReplaceX())
            {
                if (false == m_objTalkingUIData.m_bRemoteHold)
                {
                    SendUIRequestAction(TALK_ACTION_MUTE);
                }

                return true;
            }

            bool bCanExit = true;

#if IF_TALKUI_PSTN
            // flash状态下不能endcall
            if (m_objTalkingUIData.m_bPstnFlashing)
            {
                TALKUI_WARN("Flashing, will not response to PHONE_KEY_CANCEL.");
                bCanExit = false;
            }
#endif
#if IF_TALKUI_LINE_DEVICE
            if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
            {
                bCanExit = false;
            }
#endif

#if !IF_BUG_25055
            //本地没有hold 才能发送cancel事件
            if (m_objTalkingUIData.m_bLocalHold)
            {
                bCanExit = false;
            }
#endif

            if (bCanExit)
            {
                SendUIRequestAction(TALK_ACTION_CANCEL);
            }

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

// 数字键处理
bool CTalkingProcessor::OnDigitkeyPress(int iKey)
{
#if IF_TALKUI_PSTN
    // flash状态下过滤数字按键
    if (m_objTalkingUIData.m_bPstnFlashing)
    {
        TALKUI_WARN("Flashing, will not response to digit key.");
        return true;
    }
#endif

#if IF_FEATURE_DIAL_ON_HOLD
    if ((m_objTalkingUIData.m_bLocalHold
            || m_objTalkingUIData.m_bRemoteHold)
            && configParser_GetConfigInt(kszDialOnHold) == 1
            && acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL) // 如果没有可用账号则视为DTMF按键
       )
    {
        SendUIRequestAction(TALK_ACTION_DIAL_ON_HOLD, iKey);
        return true;
    }
#endif

    //通话界面按数字键响dtmf音 是在逻辑层中处理的
    SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}

//softKey 按键处理
bool CTalkingProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;
    switch (eSoftkey)
    {
    case ST_CONFERENCE:
        {
            SendUIRequestAction(TALK_ACTION_CONF);

            bHandled = true;
        }
        break;
    case ST_TRANSFER:
        {
#if IF_TALKUI_LINE_DEVICE
            if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
            {
                bHandled = true;
            }
            else
#endif
            {
                SendUIRequestAction(TALK_ACTION_TRAN);

                bHandled = true;
            }
        }
        break;
    case ST_PRIVATEHOLD:
        {
            SendUIRequestAction(TALK_ACTION_HOLD_PRIVATE);

            bHandled = true;
        }
        break;
    case ST_ANSWER:
        {
            // 有来电时的响应
            SendUIRequestAction(TALK_ACTION_ANSWER);

            bHandled = true;
        }
        break;
#if IF_TALKUI_FIVE_PARTY
    case ST_JOIN:
        {
            if (m_objTalkingUIData.m_bAllowJoin)
            {
                SendUIRequestAction(TALK_ACTION_JOIN);
            }

            bHandled = true;
        }
        break;
#endif
    case ST_REJECT:
        {
            // 有来电时的响应
            SendUIRequestAction(TALK_ACTION_REJECT);

            bHandled = true;
        }
        break;
    case ST_SWAP:
        {
            SendUIRequestAction(TALK_ACTION_SWAP);

            bHandled = true;
        }
        break;
    case ST_CANCEL:
    case ST_ENDCALL:
        {
#if IF_TALKUI_PSTN
            // flash状态下过滤EndCall软键
            if (m_objTalkingUIData.m_bPstnFlashing)
            {
                TALKUI_WARN("Flashing, will not response to ENDCALL softkey.");

                bHandled = true;
            }
            else
#endif
#if IF_TALKUI_LINE_DEVICE
                if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
                {
                    bHandled = true;
                }
                else
#endif
                {
                    SendUIRequestAction(TALK_ACTION_CANCEL);

                    bHandled = true;
                }
        }
        break;
    case ST_SECURITY:
        {
            if (IsShowSecurityClassification())
            {
                int iSessionId = talklogic_GetFocusedSessionID();
                CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
                if (pCallInfo == NULL || m_pTalkUI == NULL)
                {
                    return false;
                }

                m_pTalkUI->ShowSecurityBox(pCallInfo->GetAccountID());
            }

            bHandled = true;
        }
        break;
    case ST_POOL:
        {
            if (m_pTalkUI)
            {
                m_pTalkUI->DealPool();
            }

            bHandled = true;
        }
        break;
    case ST_HOLD:
        {
            SendUIRequestAction(TALK_ACTION_HOLD);

            bHandled = true;
        }
        break;
    case ST_RESUME:
        {
            ResumeTalk();

            bHandled = true;
        }
        break;
    case ST_NEWCALL:
        {
            SendUIRequestAction(TALK_ACTION_NEWCALL);

            bHandled = true;
        }
        break;
    case ST_PARK:
        {
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
            // http://10.3.5.199/Bug.php?BugID=61603
//          if (/*configParser_GetConfigInt(kszCallParkSwitch) == 1
//              && */configParser_GetConfigInt(kszBroadsoftActive) != 0)
//          {
//              SendUIRequestAction(TALK_ACTION_PARK);
//          }

            bool bPrefixPark = false;
#if IF_BUG_27796
            //根据账号服务器类型，选择进行Park操作
            if (SST_BROADSOFT != configParser_GetCfgItemIntValue(kszAccountSIPServerType,
                    m_objTalkingUIData.m_iSipAccount))
            {
                bPrefixPark = true;
            }
#endif

            if (bPrefixPark)
            {
                SendUIRequestAction(TALK_ACTION_PREFIX_PARK);
            }
            else
            {
                // http://bugfree.yealink.com/Bug.php?BugID=124363
                SendUIRequestAction(TALK_ACTION_PARK);
            }

            bHandled = true;
#endif
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case ST_GROUP_PARK:
        {
            // http://10.3.5.199/Bug.php?BugID=61603
//             if (IsAllowGroupParkSoftkey())
//             {
            SendUIRequestAction(TALK_ACTION_GROUP_PARK);
//             }

            bHandled = true;
        }
        break;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_ACD_DISPCODE:
        {
            if (CallCenter_IsDispCodeOn()
                    && m_objTalkingUIData.m_nCallID > 0)
            {
                if (NULL != m_pTalkUI)
                {
                    m_pTalkUI->ShowACDDispCodeMenu(m_objTalkingUIData.m_nCallID);
                }
            }

            bHandled = true;
        }
        break;
    case ST_ACD_EMERGENCY:
        {
            if (CallCenter_IsEmergencyEscalationOn()
                    && m_objTalkingUIData.m_nCallID > 0)
            {
                if (NULL != m_pTalkUI)
                {
                    m_pTalkUI->ShowEmergencyMenu(m_objTalkingUIData.m_nCallID);
                }
            }

            bHandled = true;
        }
        break;
    case ST_ACD_TRACE:
        {
            if (CallCenter_IsTraceOn())
            {
                CallCenter_OriginatedTrace(m_objTalkingUIData.m_nCallID);
            }

            bHandled = true;
        }
        break;
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_RTP_STATUS:
        {
            //http://10.2.1.199/Bug.php?BugID=82351 本地hold或者远程hold的时候都不响应
            if (!m_objTalkingUIData.m_bLocalHold
                    && !m_objTalkingUIData.m_bRemoteHold
#if IF_TALKUI_LINE_DEVICE
                    && CALL_LINE_DEVICE != m_objTalkingUIData.m_objContactInfo.m_eCallLineType
#endif
               )
            {
                yl::string strUIName = PROCESSORNAME_RTPSTATUS;
                _TalkUILogicFrameManager.UpdateUIByProcessorName(strUIName);
            }

            bHandled = true;
        }
        break;

#if IF_TALKUI_PSTN
    case ST_FLASH:
        {
            // flash状态下过滤FLASH软键
            if (m_objTalkingUIData.m_bPstnFlashing)
            {
                TALKUI_WARN("Flashing, will not response to FLASH softkey.");

                bHandled = true;
            }
            else
            {
                //此处逻辑层不会触发刷新这个状态，需要uilogic主动去刷ui
                m_objTalkingUIData.m_bPstnFlashing = true;
                talkui_UpdateCurrentWnd();

                SendUIRequestAction(TALK_ACTION_FLASH);

                bHandled = true;
            }
        }
        break;
#endif

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    case ST_CALLPUSH:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_CALL_PUSH);
        }
        break;
#endif

    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

////处理UI动作事件
//void CTalkingProcessor::OnUIAction(yl::string strAction)
//{
//
//}

//设置title
void CTalkingProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);

    if (NULL != pTalkingData)
    {
        //后面和 lxh 、kyle 讨论 held 的时候还是需要显示held
        //和lxh确认 如果是本地hold 要显示hold
        //held 和 mute 不需要特殊显示
        if (NULL != pTalkingData->pCallInfo)
        {
#if IF_TALKUI_LINE_DEVICE
            //PC Mode不显示title
            if (CALL_LINE_DEVICE == pTalkingData->pCallInfo->GetCallLineType())
            {
                m_strTitle = "";

                return;
            }
#endif

            if (pTalkingData->pCallInfo->IsLocalHold())
            {
                m_strTitle = _UILOGIC_LANG_TRAN(TRID_HOLD);

                //这边一定要返回  否则 后面会重新再设置一次
                return ;
            }
            //远程hold 还是显示 held
            else if (pTalkingData->pCallInfo->IsRemoteHold())
            {
                m_strTitle = _UILOGIC_LANG_TRAN(TRID_HELD);

                //这边一定要返回  否则 后面会重新再设置一次
                return ;
            }
        }

        // 通话对应的UI类型
        m_strTitle = GetTitleByTalkingUIType(pTalkingData->eTalkUIType);
    }
}

bool CTalkingProcessor::GetCustomChannelIcon(TALKUI_CHANNEL_ICON & eIconType)
{
#if IF_TALKUI_LINE_DEVICE
    if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
    {
        eIconType = TALKUI_CHANNEL_ICON_UNKNOWN;
        return true;
    }
#endif

    return false;
}

//设置图标列表
void CTalkingProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先清空列表
    m_listIcons.clear();

    DataTalking2UI * pTalkingData = (DataTalking2UI *)(pData);

    //通话语音是否加密
    if (true == pTalkingData->bEncrypt)
    {
        AddTalkUIIcon(TALKUI_ICON_VOICEENCRY, true);
    }

    CCallInfo * pCallInfo = pTalkingData->pCallInfo;

    if (NULL != pCallInfo)
    {
        //是否高清通话
//         if (true == pCallInfo->IsHDTalking())
//         {
//             AddTalkUIIcon(TALKUI_ICON_HD, true);
//         }
        AUDIO_CODEC_TYPE eAudioCodec = pCallInfo->GetAudioCodecType();
        TALKUI_INFO("CTalkingProcessor AudioCodec Is [%d]", eAudioCodec);

        if (eAudioCodec & ACT_HD)
        {
            AddTalkUIIcon(TALKUI_ICON_HD, true);
        }
        else if (eAudioCodec & ACT_AMR)
        {
            AddTalkUIIcon(TALKUI_ICON_AMR, true);
        }
        else if (eAudioCodec & ACT_OPUS)
        {
            AddTalkUIIcon(TALKUI_ICON_OPUS, true);
        }

        ////不是UNKNOWN类型的 要添加录音图标
        ICON_TYPE eIconType = GetRecordIconTypeFromCallInfo(pCallInfo);
        TALKUI_ICON eTalkUIIcon = GetTalkUIIconByIconType(eIconType);

        //不是UNKNOWN类型的 要添加录音图标
        if (TALKUI_ICON_NONE != eTalkUIIcon)
        {
            AddTalkUIIcon(eTalkUIIcon, true);
        }

        ////录音图标
        //if(IT_UNKNOWN != GetRecordIconTypeFromCallInfo(pCallInfo))
        //{
        //  AddTalkUIIcon(TALKUI_ICON_RECORD, true);
        //}
    }

}

//给子界面设置数据
void CTalkingProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objTalkingUIData, eReason);
    }
}

void CTalkingProcessor::GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    m_bRTPSoftkeySet = false;
}

void CTalkingProcessor::RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    for (int i = 0; i < VecSoftkey.size();)
    {
        SoftKey_TYPE eSoftkeyType = VecSoftkey[i];

        if (ST_RTP_STATUS == eSoftkeyType)
        {
            m_bRTPSoftkeySet = true;
        }
        if (ST_HOLD == VecSoftkey[i])
        {
            if (IsShowResumeSoftkey())
            {
                VecSoftkey[i] = ST_FAR_RESUME;
            }
        }

        ++i;
    }
}

void CTalkingProcessor::GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_bUseCustomSoftkey
            && !_TalkUILogicDelegate.IsCustomDynamicSoftkeyOn())
    {
#if IF_TALKUI_USB_ENABLE
        AddUSBRecordSoftkey(VecSoftkey);
#endif
        return;
    }

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (TALKING_UI_NORMAL == m_objTalkingUIData.m_eTalkUIType
            && !m_objTalkingUIData.m_bLocalHold
            && Xsi_IsExecutiveCall(m_iSessionID))
    {
        VecSoftkey.push_back(ST_CALLPUSH);
    }
#endif

#if IF_TALKUI_USB_ENABLE
    //添加USB截图相关softkey
    AddUSBScreenshotSoftkey(VecSoftkey);

    //添加USB录音相关softkey
    if (!AddUSBRecordSoftkey(VecSoftkey))
#endif
    {
        //没有本地录制softkey的情况下，才添加bsft录制softkey
        SetRecordSoftkey(VecSoftkey);
    }

#ifdef IF_TALKUI_LINE_DEVICE
    if (CALL_LINE_DEVICE != m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
#endif
    {
        //设置acd的softkey
        SetACDSoftkey(VecSoftkey, m_objTalkingUIData.m_iSipAccount);
    }
}

//获取softkey数据信息
void CTalkingProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //是蓝牙通话 走蓝牙 获取softkey流程
    if (GetSessionState() == SESSION_BTTALKING)
    {
        GetBluetoothSoftkey(VecSoftkey);

        return ;
    }
#endif

    //更新softkey
    if (m_objTalkingUIData.m_bHasCallComming)
    {
        //如果新来电需要改变softkey 那么才使用RingStatus的softkey
        if (_TalkUILogicDelegate.IsSoftkeyChangedCallIncome())
        {
            GetRingStatusSoftkey(VecSoftkey);
        }
        else    //否则 还是使用TalkStatus 的softkey
        {
            GetTalkStatusSoftkey(VecSoftkey);
        }
    }
    else
    {
        GetTalkStatusSoftkey(VecSoftkey);
    }
}

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//获取蓝牙softkey数据信息
void CTalkingProcessor::GetBluetoothSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //更新softkey
    if (m_objTalkingUIData.m_bHasCallComming)
    {
        //如果新来电需要改变softkey 那么才使用RingStatus的softkey
        if (true == _TalkUILogicDelegate.IsSoftkeyChangedCallIncome())
        {
            GetBluetoothRingStatusSoftkey(VecSoftkey);
        }
        else    //否则 还是使用TalkStatus 的softkey
        {
            GetBluetoothTalkStatusSoftkey(VecSoftkey);
        }
    }
    else
    {
        GetBluetoothTalkStatusSoftkey(VecSoftkey);
    }

    //蓝牙不需要添加USB相关softkey
}
#endif

// 是否允许自定义softkey
bool CTalkingProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_PSTN
    CALL_LINE_TYPE eCallLineType;
    if ((CALL_LINE_PSTN == m_objTalkingUIData.m_objContactInfo.m_eCallLineType) //当前为PSTN
            || ((talklogic_GetIncomingCallLineType(eCallLineType)
                 && CALL_LINE_PSTN == eCallLineType))) // 来电为PSTN
    {
        return false;
    }
#endif

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTTALKING)
    {
        return false;
    }
#endif

    return true;
}

bool CTalkingProcessor::AllowSoftkeyShow(SoftKey_TYPE eType)
{
    if (ST_SWAP == eType)
    {
        return IsAllowSwapSoftkey();
    }
    else if (ST_CONFERENCE == eType)
    {
        return IsAllowConfSoftkey();
    }
    else if (ST_TRANSFER == eType)
    {
        return m_objTalkingUIData.m_bAllowTran;
    }
    else if (ST_SECURITY == eType)
    {
        return IsShowSecurityClassification();
    }
    else if (ST_PARK == eType)
    {
        return IsAllowParkSoftkey();
    }
    else if (ST_GROUP_PARK == eType)
    {
        return IsAllowGroupParkSoftkey();
    }
    else if (ST_RTP_STATUS == eType)
    {
        return IsAllowRTPSoftkey();
    }
    else if (ST_RESUME == eType)
    {
        return IsShowResumeSoftkey();
    }

    return CBaseTalkUIProcessor::AllowSoftkeyShow(eType);
}


bool CTalkingProcessor::IsAllowSwapSoftkey()
{
    if ((m_objTalkingUIData.m_bAllowSwap)
            && (_TalkUILogicDelegate.IsShowSwapSoftkey()))
    {
        return true;
    }

    return false;
}


bool CTalkingProcessor::IsAllowConfSoftkey()
{
    if (m_objTalkingUIData.m_bAllowConf
            && acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL)/* 存在可用账号*/)
    {
        return true;
    }

    return false;
}

bool CTalkingProcessor::IsAllowParkSoftkey()
{
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    if (m_eSessionState == SESSION_NETWORKCONF)
    {
        return false;
    }

    if (configParser_GetConfigInt(kszCallParkSwitch) != 1)
    {
        return false;
    }

    return true;
#else
    return true;
#endif
}


bool CTalkingProcessor::IsAllowGroupParkSoftkey()
{
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    if (m_eSessionState == SESSION_NETWORKCONF)
    {
        return false;
    }

    if (configParser_GetConfigInt(kszGroupCallParkSwitch) != 1)
    {
        return false;
    }

    return true;
#else
    return true;
#endif
}

bool CTalkingProcessor::IsAllowRTPSoftkey()
{
    if (!m_bRTPSoftkeySet
            && 1 == configParser_GetConfigInt(kszDisplayVQReportOnUI)
#if IF_TALKUI_PSTN
            && CALL_LINE_PSTN != m_objTalkingUIData.m_objContactInfo.m_eCallLineType
#endif
            && !m_objTalkingUIData.m_bLocalHold
            && !m_objTalkingUIData.m_bRemoteHold
            /*&& 0 != configParser_GetConfigInt(kszIntervalReportSwitch)*/)
    {
        return true;
    }

    return false;
}

// softkey处理(自定义类型)
void CTalkingProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    strFile = "Talking.xml";

    switch (m_objTalkingUIData.m_eTalkState)
    {
    case TS_TALK:
        strStatus = "Talk";
        break;
    case TS_HOLD_HELD:
    case TS_HOLD:
        strStatus = "Hold";
        break;
    case TS_HELD:
        strStatus = "Held";
        break;
    default:
        break;
    }

#if IF_FEATURE_CUSTOM_SOFTKEY
    SESSION_STATE eSessionState = GetSessionState();
    bool bHoldState = false;
    if (eSessionState == SESSION_ATSCONF
            || eSessionState == SESSION_NETWORKCONF)
    {
        strStatus = "NetworkConferenced";
        bHoldState = true;
    }
    else if (m_objTalkingUIData.m_callType == CT_MULTICAST_IN)
    {
        strStatus = "ListeningPaging";
        bHoldState = true;
    }
    else if (m_objTalkingUIData.m_callType == CT_MULTICAST_OUT)
    {
        strStatus = "Paging";
        bHoldState = true;
    }
    else if (eSessionState == SESSION_BETRANSFERRED)
    {
        strStatus = "BeTransferred";
    }

    if (bHoldState
            && (m_objTalkingUIData.m_eTalkState == TS_HOLD_HELD
                || m_objTalkingUIData.m_eTalkState == TS_HOLD))
    {
        strStatus.append("Hold");
    }

    //http://10.2.1.199/Bug.php?BugID=89152 添加m_eTalkState == TS_TALK的限制
    if (m_objTalkingUIData.m_bHasCallComming
            && _TalkUILogicDelegate.IsSoftkeyChangedCallIncome()
            && m_objTalkingUIData.m_eTalkState == TS_TALK)
    {
        strStatus = "NewCallIn";
    }
#endif
}

//获取通话状态的softkey
void CTalkingProcessor::GetTalkStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //VecSoftkey.clear();
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall(GetSessionID()))
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
#endif

#if IF_TALKUI_PSTN
    if (CALL_LINE_PSTN == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
    {
        // PSTN通话不支持Transfer，替换成Flash
        VecSoftkey.push_back(ST_FLASH);
    }
    else
#endif
    {
        bool bAllowTransfer = true;
#ifdef IF_TALKUI_LINE_DEVICE
        if (CALL_LINE_DEVICE == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
        {
            bAllowTransfer = false;
        }
        else
#endif
        {
            if (talklogic_IsEmergencyRecall())
            {
                bAllowTransfer = false;
            }
            //设置softkey1,Transfer内容
            else if (m_objTalkingUIData.m_bAllowTran)
            {
                bAllowTransfer = true;
            }
            else if (m_objTalkingUIData.m_bTransferFailed)
            {
                //http://10.3.5.199/Bug.php?BugID=62577
                //这边还是用 transfer 的softkey， 因为每按一次就会刷新一次界面 这个时候 其实还是有transfer 的功能
                bAllowTransfer = true;
            }
        }

        if (bAllowTransfer)
        {
            VecSoftkey.push_back(ST_TRANSFER);
        }
        else
        {
            VecSoftkey.push_back(ST_EMPTY);
        }
    }

    //根据lxh 意见  t48不需要显示  swap
    //设置softkey2   允许swap 且 机型需要显示swap
    if (IsAllowSwapSoftkey())
    {
        VecSoftkey.push_back(ST_SWAP);
    }
    //当sip收到服务器返回的消息时会通知逻辑层，此时允许显示hold或者resume  http://10.2.1.199/Bug.php?BugID=88426
    //http://10.2.1.199/Bug.php?BugID=89764被转界面hold，增加下面的判断条件 m_DataToUI.bAllowHold
    else if (m_objTalkingUIData.m_bAllowHold && m_objTalkingUIData.m_bInTransferCanHold)
    {
        if (m_objTalkingUIData.m_bLocalHold)
        {
            //  http://10.3.5.199/Bug.php?BugID=62591
            //在transfered 的时候不显示resume 的softkey
            //transfered 失败的时候  还是处于transfered状态的  这边是需要显示resume 的softkey
            if (IsShowResumeSoftkey())
            {
                VecSoftkey.push_back(ST_RESUME);
            }
            else
            {
                VecSoftkey.push_back(ST_EMPTY);
            }
        }
        else
        {
            VecSoftkey.push_back(ST_HOLD);
        }
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    //http://10.2.1.199/Bug.php?BugID=77363
    //http://10.2.1.199/Bug.php?BugID=83172
    //与测试部确认 通话UI中 非softkeylayout 显示的softkey   关闭bsft 的时候 不需要显示 prihold  ， 且sca不存在相关的softkey显示
#if !IF_BUG_23438
    if (m_objTalkingUIData.m_blaType == BLA_BROADSOFT_SCA
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && !m_objTalkingUIData.m_bLocalHold)
    {
        //v80 移植v73技术支持 在开启SCA功能的时候 优先使用conference功能
        if (IsAllowConfSoftkey())
        {
            VecSoftkey.push_back(ST_CONFERENCE);
        }
        //移到网页去处理PRIHOLD softeky http://10.2.1.199/Bug.php?BugID=96370
        else
        {
            // 如果该路通话时BroadSoft Share Line通话,则将第三个按键设置成Private Hold
            VecSoftkey.push_back(ST_EMPTY);
        }
    }
    else
#endif
    {
        // 设置softkey3设置Conference内容
        if ((m_objTalkingUIData.m_callType == CT_MULTICAST_OUT)
                || (m_objTalkingUIData.m_callType == CT_MULTICAST_IN))
        {
            VecSoftkey.push_back(ST_EMPTY);
        }
        else if (IsAllowConfSoftkey())
        {
            VecSoftkey.push_back(ST_CONFERENCE);
        }
        else
        {
            //不允许Conf则显示New Call
            if (m_objTalkingUIData.m_bLocalHold)
            {
                if (talklogic_BeAllowNewCall()
                        && acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL)/* 存在可用账号*/)
                {
                    //未达到最大通话路数限制，显示New Call
                    //http://10.2.1.199/Bug.php?BugID=102589
                    if (m_objTalkingUIData.m_bAllowNewCall)
                    {
                        VecSoftkey.push_back(ST_NEWCALL);
                    }
                    else
                    {
                        VecSoftkey.push_back(ST_EMPTY);
                    }
                }
                else
                {
                    //达到最大通话路数限制，显示为空
                    VecSoftkey.push_back(ST_EMPTY);
                }
            }
            else
            {
                //达到最大通话路数限制，显示为空
                VecSoftkey.push_back(ST_EMPTY);
            }
        }
    }

#ifdef IF_TALKUI_LINE_DEVICE
    if (CALL_LINE_DEVICE != m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
#endif
    {
        // 设置最后一个softkey4
#if IF_FEATURE_DIAL_ON_HOLD
        if ((m_objTalkingUIData.m_bLocalHold
                || m_objTalkingUIData.m_bRemoteHold)
                && configParser_GetConfigInt(kszDialOnHold) == 1)
        {
            VecSoftkey.push_back(ST_POOL);
        }
        else
#endif
        {
            VecSoftkey.push_back(ST_ENDCALL);
        }

        //在endcall之后插入security softkey
        if (IsShowSecurityClassification())
        {
            VecSoftkey.push_back(ST_SECURITY);
        }

        // http://10.3.5.199/Bug.php?BugID=72126
        //hold和held情况下不显示rtp status按钮
        if (IsAllowRTPSoftkey())
        {
            m_bRTPSoftkeySet = true;
            VecSoftkey.push_back(ST_RTP_STATUS);
        }

        //设置park相关的softkey
        SetParkSoftkey(VecSoftkey);
    }
}

//获取来电状态的softkey
void CTalkingProcessor::GetRingStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall())
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
    else
#endif
        // 设置二级菜单界面对应1st SoftKeybar的数据
        //http://10.2.1.199/Bug.php?BugID=93677 根据标志m_bAllowTran是否显示transfer
    {
#if IF_TALKUI_PSTN
        if (CALL_LINE_PSTN == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
        {
            // PSTN通话不支持Transfer，替换成Flash
            VecSoftkey.push_back(ST_FLASH);
        }
        else
#endif
            if (m_objTalkingUIData.m_bAllowTran)
            {
                VecSoftkey.push_back(ST_TRANSFER);
            }
    }

#if IF_TALKUI_FIVE_PARTY
    if (m_objTalkingUIData.m_bAllowJoin)
    {
        VecSoftkey.push_back(ST_JOIN);
    }
#endif

    // 设置二级菜单界面对应2nd SoftKeybar的数据
    VecSoftkey.push_back(ST_ANSWER);

    bool bAllowReject = true;
    CALL_LINE_TYPE eCallLineType;
    if (talklogic_GetIncomingCallLineType(eCallLineType))
    {
#if IF_TALKUI_PSTN
        if (CALL_LINE_PSTN == eCallLineType)
        {
            // PSTN通话不支持拒接
            bAllowReject = false;
        }
#endif
    }
    else
    {
        bAllowReject = false;
    }

#if IF_BUG_38956
    if (0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
    {
        bAllowReject = false;
    }
#endif
#ifdef IF_FEATURE_EMERGENCY
    if (talklogic_IsEmergencyRecall())
    {
        bAllowReject = false;
    }
#endif

    if (bAllowReject)
    {
        // 设置二级菜单界面对应3rd SoftKeybar的数据
        VecSoftkey.push_back(ST_REJECT);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    // 设置二级菜单界面对应4th SoftKeybar的数据
    VecSoftkey.push_back(ST_ENDCALL);

    //hold和held情况下不显示rtp status按钮
    if (IsAllowRTPSoftkey())
    {
        VecSoftkey.push_back(ST_RTP_STATUS);
    }
}

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//获取通话状态的softkey
void CTalkingProcessor::GetBluetoothTalkStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //VecSoftkey.clear();
    VecSoftkey.push_back(ST_EMPTY);

    //根据lxh 意见  t48不需要显示  swap
    //设置softkey2   允许swap 且 机型需要显示swap
    if (IsAllowSwapSoftkey())
    {
        VecSoftkey.push_back(ST_SWAP);
    }
    ////http://10.2.1.199/Bug.php?BugID=89764被转界面hold，增加下面的判断条件 m_DataToUI.bAllowHold
    else if (m_objTalkingUIData.m_bAllowHold && m_objTalkingUIData.m_bInTransferCanHold)
    {
        if (m_objTalkingUIData.m_bLocalHold)
        {
            //蓝牙不存在transfer状态
            VecSoftkey.push_back(ST_RESUME);
        }
        else
        {
            VecSoftkey.push_back(ST_HOLD);
        }
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

#ifdef IF_BT_SUPPORT_PHONE
    // http://10.2.1.199/Bug.php?BugID=76372
    // 判断hold状态下 添加newcall 的softkey
    //http://10.2.1.199/Bug.php?BugID=92479
    if (m_objTalkingUIData.m_bLocalHold && talklogic_BeAllowNewBTCall())
    {
        VecSoftkey.push_back(ST_NEWCALL);
    }
    else
    {
        //没有newcall 只能显示empty
        VecSoftkey.push_back(ST_EMPTY);
    }
#endif

    // 设置最后一个softkey4
    VecSoftkey.push_back(ST_ENDCALL);

}
#endif

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//获取来电状态的softkey
void CTalkingProcessor::GetBluetoothRingStatusSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_FIVE_PARTY
    if (m_objTalkingUIData.m_bAllowJoin)
    {
        VecSoftkey.push_back(ST_JOIN);
    }
    else
#endif
    {
        // 设置二级菜单界面对应1st SoftKeybar的数据
        //蓝牙不显示transfer
        VecSoftkey.push_back(ST_EMPTY);
    }

    // 设置二级菜单界面对应2nd SoftKeybar的数据
    VecSoftkey.push_back(ST_ANSWER);

    // 设置二级菜单界面对应3rd SoftKeybar的数据
    CALL_LINE_TYPE eCallLineType;
    if (talklogic_GetIncomingCallLineType(eCallLineType))
    {
#if IF_TALKUI_PSTN
        if (CALL_LINE_PSTN == eCallLineType)
        {
            // PSTN通话不支持拒接
            VecSoftkey.push_back(ST_EMPTY);
        }
        else
#endif
        {
            // 设置二级菜单界面对应3rd SoftKeybar的数据
            VecSoftkey.push_back(ST_REJECT);
        }
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    // 设置二级菜单界面对应4th SoftKeybar的数据
    VecSoftkey.push_back(ST_ENDCALL);
}
#endif

//设置ACD相关的softkey
void CTalkingProcessor::SetACDSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey, int iAccountID)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (CallCenter_IsACDOn()
            && ACD_BROADSOFT == Get_ACDType() //!!!!! 等逻辑层移植后取消注释
            && ACD_GetAccountId() == iAccountID
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && ACD_GetAcdStatus() != AS_IDLE)
    {
        if (CallCenter_IsDispCodeOn())
        {
            VecSoftkey.push_back(ST_ACD_DISPCODE);
        }

        if (CallCenter_IsEmergencyEscalationOn())
        {
            VecSoftkey.push_back(ST_ACD_EMERGENCY);
        }

        if (CallCenter_IsTraceOn())
        {
            VecSoftkey.push_back(ST_ACD_TRACE);
        }
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
}

// 添加Broadsoft Park功能的Softkey
void CTalkingProcessor::SetParkSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_PSTN
    if (CALL_LINE_PSTN == m_objTalkingUIData.m_objContactInfo.m_eCallLineType)
    {
        // PSTN通话不支持Park的Softkey
        return;
    }
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    //http://10.2.1.199/Bug.php?BugID=104219 网络会议下不显示park功能相关的softkey
    if (TALKING_UI_NETWORKCONF == m_objTalkingUIData.m_eTalkUIType)
    {
        return;
    }

    // http://10.3.5.199/Bug.php?BugID=71884
    if (IsAllowParkSoftkey())
    {
        VecSoftkey.push_back(ST_PARK);
    }

    // http://10.3.5.199/Bug.php?BugID=71884
    //broadsoft 开启的时候才能显示相关 softkey
    if (IsAllowGroupParkSoftkey())
    {
        VecSoftkey.push_back(ST_GROUP_PARK);
    }

#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
}

yl::string CTalkingProcessor::GetTitleByTalkingUIType(TALKING_UI_TYPE eTalkUIType)
{
    yl::string strTitle("");
    switch (eTalkUIType)
    {
    case TALKING_UI_NORMAL:
        //  case TALKING_UI_NETWORKCONF:
        {
            strTitle = yl::string(TRID_TALKING);
        }
        break;
    case TALKING_UI_TRANSFERRING:
        {
            strTitle = yl::string(TRID_TRANSFERING);
        }
        break;
    case TALKING_UI_TRANSFERRED:
        {
            strTitle = yl::string(TRID_TRANSFERED);
        }
        break;
    case TALKING_UI_BETRANSFER:
        {
            strTitle = yl::string(TRID_BE_TRANSGERRED);
        }
        break;
    case TALKING_UI_NETWORKCONF:
        {
            strTitle = yl::string(TRID_CONFERENCE);
        }
        break;
    default:
        {
            //  TALKUI_INFO(FALSE, fmtString("Talk ui type(%d) is unknown!", eTalkUIType).c_str());
            strTitle = "";
        }
        break;
    }

    strTitle = _UILOGIC_LANG_TRAN(strTitle.c_str());

    return strTitle;
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CTalkingProcessor::IfNeedCallRecording()
{
    return !m_objTalkingUIData.m_bLocalHold;
}
#endif

