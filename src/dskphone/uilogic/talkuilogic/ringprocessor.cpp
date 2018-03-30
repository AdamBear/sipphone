#include "ringprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uilogicringuibase.h"
#include "talkuilogicframemanager.h"
#include "talkuilogicprocessorname.h"
#include "application/include/modapplication.h"

CRingProcessor::CRingProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_RING)
{
    m_pTalkUI = NULL;
    m_bACDSoftkeySet = false;
}

CRingProcessor::~CRingProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CRingProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objRingUIData.Reset();
}

//释放前解除UI绑定前调用
void CRingProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CRingProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicRingUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CRingProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取保存的talkingui数据
CRingUIData * CRingProcessor::GetRingUIData()
{
    return &m_objRingUIData;
}

#if IF_FEATURE_NFON_RTP
bool CRingProcessor::SendUIRequestAction(TALK_ACTION_TYPE eActionType, WPARAM wActionParam /*= 0*/, LPARAM lActionParam /*= 0*/, void * pExtraData /*= NULL*/)
{
    if (TALK_ACTION_ANSWER == eActionType
            && NULL != m_pTalkUI)
    {
        m_pTalkUI->StopUIAnimation();
    }

    return CBaseTalkUIProcessor::SendUIRequestAction(eActionType, wActionParam, lActionParam, pExtraData);
}
#endif

//获取转接信息
yl::string CRingProcessor::GetVIAInfo()
{
    return m_objRingUIData.m_strViaInfo;
}

//设置数据
void CRingProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //有些数据是不能重置的
    //先把数据重置一下
    //m_objRingUIData.Reset();

    DataRing * pRingingtData = (DataRing *)pData;

    m_objRingUIData.m_bCallDeclineEnable = pRingingtData->bCallDeclineEnable;
    m_objRingUIData.m_bEmergencyCalled = pRingingtData->bEmergencyCalled;

    CCallInfo * pCallInfo = pRingingtData->pCallInfo;

    if (NULL == pCallInfo)
    {
        return ;
    }

    m_objRingUIData.m_strTargetName = "";       //目标名称
    m_objRingUIData.m_strTargetNumber = "";     //目标号码
    m_objRingUIData.m_strTargetFullContactInfo = "";    //目标完整号码

    //是否recall状态
    m_objRingUIData.m_bIsRecalling = (m_eSessionState == SESSION_RECALL);
    //这个 主要是用于 获取softkey时候是否显示CallInfo 的softkey使用的
    // V80 中 pRingingtData->bShowCallCenterInfo 与  pRingData->pCCInfo 一起判断是显示ACDInfo 界面还是 Ringing界面 而不是用于 判断是否显示TALK_UI_ACTION_ACD_CALL_INFO 的softkey了
    // 是否显示 TALK_UI_ACTION_ACD_CALL_INFO 的softkey 判断是 用 pRingingtData->bCallCenterInfoAvaliable 来判断
    m_objRingUIData.m_bCallCenterInfoAvaliable = pRingingtData->bCallCenterInfoAvaliable;

    //设置静音状态
    m_objRingUIData.m_bIsSilence = pRingingtData->bShowSilence;

    //设置是否允许显示fwd标志位
    m_objRingUIData.m_bAllowFwd = true;

#if IF_TALKUI_FIVE_PARTY
    m_objRingUIData.m_bAllowJoin = pRingingtData->bAllowJoin;
#endif

    //这边要重新设置一下
    m_objRingUIData.m_bIsVoiceKeyPress = false;     //是否在刷新数据后按了音量条键

    //跟lxh 确认， 之前有某些机型是 recall 写在name或number 上的，  以后统一所有的机型都写在title上就行
    //联系人名称还是正常显示
    //在这 bAnswer 不能重置
    GetContactInfoFromCallInfo(m_objRingUIData.m_objContactInfo, pRingingtData->pCallInfo);

    //配置文件中 需要显示via信息 而且不是Recall  那么 才需要给viainfo 赋值
    if (_TalkUILogicDelegate.IsShowViaInfo() && (!m_objRingUIData.m_bIsRecalling))
    {
        m_objRingUIData.m_strViaInfo = pRingingtData->m_strViaName;
        ChangeNameAndNumberByInfoMethod(m_objRingUIData.m_strViaInfo, pRingingtData->m_strViaNumber, pRingingtData->m_strViaFullContact, _TalkUILogicDelegate.GetDisplayMethod(), false, true);
    }
    else
    {
        m_objRingUIData.m_strViaInfo = "";
    }

#if IF_BUG_26914
    m_objRingUIData.m_strViaInfo = pRingingtData->m_strViaName;
#else
    //需要显示目标账号信息  才需要获取相关数据
    if (_TalkUILogicDelegate.IsShowTargetInfo())
    {
        m_objRingUIData.m_strTargetName = pCallInfo->m_tLocalInfo.sDisplayName;
        m_objRingUIData.m_strTargetNumber = pCallInfo->m_tLocalInfo.sSIPName;
        m_objRingUIData.m_strTargetFullContactInfo =
            pCallInfo->m_tLocalInfo.sFullContactInfo;  //目标完整号码
    }
#endif

}

//这边就不用做了  直接在basetalkuiprocessor 的时候 也同时执行 volumnvalue 改变的事件
////音量条显示状态改变  要不然逻辑层不会刷新状态  ring界面的silence， softkey 按下 可能无效， 因为逻辑层认为当前已经处于silence状态了
//这边就不用做了  直接在basetalkuiprocessor 的时候 也同时执行 volumnvalue 改变的事件
////音量条显示状态改变
//void CRingProcessor::OnTalkVolumnVisibleChange(bool bVisible)
//{
//  //先调用基类的处理方式
//  CBaseTalkUIProcessor::OnTalkVolumnVisibleChange(bVisible);
//
////    DealVolumnChange();
//}

//音量条变化调用函数
void CRingProcessor::OnTalkVolumnValueChange()
{
    //先调用基类的处理方式
    CBaseTalkUIProcessor::OnTalkVolumnValueChange();

    DealVolumnChange();

}

//处理音量改变
void CRingProcessor::DealVolumnChange()
{
    m_objRingUIData.m_bIsVoiceKeyPress = true;      //是否在刷新数据后按了音量条键

    //加上Mute 静音图标 因为这边有按了 音量条 所以 只要用音量大小判断
    //如果音量为0  那么列表中需要有mute的图标
    if (m_objRingUIData.m_bIsSilence || voice_GetVolume(VT_RING) == 0)
    {
        //添加mute图标
        AddTalkUIIcon(TALKUI_ICON_SILENCE, true);
    }
    else
    {
        //删除mute图标
        AddTalkUIIcon(TALKUI_ICON_SILENCE, false);
    }

    //刷新一下softkey
    UpdateTalkSoftkey();

    //刷新图标
    _TalkUILogicFrameManager.UpdateTalkUIIcons();

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //这边顺便刷新一下 蓝牙图标  因为蓝牙图标一般和通话图标放一块  如果这边不处理 那么后面UI要处理的地方会更多
    _TalkUILogicFrameManager.UpdateBlueToothIcon();
#endif

    //if(NULL != m_pTalkUI)
    //{
    //  //刷新一下图标
    //  m_pTalkUI->UpdateTalkUIIcons();
    //}
}

// 通道键处理
bool CRingProcessor::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
    case PHONE_KEY_HEADSET_CTRL:
    case PHONE_KEY_HANDFREE:
        {
            // 耳麦键接起来电时，需要将通道切到耳麦，比如耳麦通话中新来电，按耳麦接起
            if (iKey == PHONE_KEY_HEADSET_CTRL)
            {
                if (voice_IsCallCenterMode()
                        && voice_GetHandsetStatus())
                {
                    // 摘机状态打开双耳麦模式
                    voice_SwitchChannel(CHANNEL_2HEADSET);
                }
                else
                {
                    voice_SwitchChannel(CHANNEL_HEADSET);
                }
            }

            //m_objRingUIData.m_bAnswered = true;
            ////逻辑层会根据 第二个参数 是否为TRUE判断是否还要再次切换通道
            //SendUIRequestAction(TALK_ACTION_ANSWER, 1);

            AnswerIncomingCall(true);
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            //问题：Wildix T28\T26\T22 手柄不能挂掉还没应答的通话
            //修改：来电界面挂机时不执行 ExitRoutine()操作
            //http://10.2.1.199/Bug.php?BugID=16809
            if (m_objRingUIData.m_bAnswered
#if IF_TALKUI_ONHOOK_EXITCALL
                    || true
#endif
               )
            {
#if IF_TALKUI_PSTN
                if (m_objRingUIData.m_objContactInfo.m_eCallLineType != CALL_LINE_PSTN)
#endif
                {
                    // http://10.2.1.199/Bug.php?BugID=87362
                    // 通话接起后, 挂断应该是cancel而不是reject
                    SendUIRequestAction(TALK_ACTION_CANCEL, 1, 0, NULL);

                    // 如果由通道键引发的退出，且还有通话存在，则切换通道
                    if (talklogic_SessionExist())
                    {
                        voice_SwitchChannel();
                    }
                }
            }
            else
            {
                // http://10.2.1.199/Bug.php?BugID=46003
                // 挂机不挂断通话，但要切换通道
                voice_SwitchChannel();
            }
        }
        break;
    case PHONE_KEY_EHS_OPEN:
        {
            // 来电时打开EHS则接起来电
            //m_objRingUIData.m_bAnswered = true;
            ////逻辑层会根据 第二个参数 是否为TRUE判断是否还要再次切换通道
            //SendUIRequestAction(TALK_ACTION_ANSWER, 1);
            AnswerIncomingCall(true);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;

}

// 功能键处理
bool CRingProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_OK:
    case PHONE_KEY_ANSWER:
        {
            //Answer, OK键需要接起来电
            //非通道键接起 后面不能带 1
            AnswerIncomingCall(false);
            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            //x键复用为 mute
            if (_TalkUILogicDelegate.IsMuteReplaceX())
            {
                SendUIRequestAction(TALK_ACTION_MUTE);

                return true;
            }

#if IF_TALKUI_PSTN
            if (m_objRingUIData.m_objContactInfo.m_eCallLineType != CALL_LINE_PSTN)
#endif
            {
                //拒绝来电
                SendUIRequestAction(TALK_ACTION_REJECT, 0, 0, NULL);
            }

            return true;
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            //在ring情况下  transfer被当做 forward 处理
            //如果是Recall 不用进入forward
            //如果是已经在Forward 界面了 也不需要继续执行
            //如果是显示CallInfo 那么也不需要进入forward
            //这边是进行容错处理
            if (SESSION_RECALL != m_eSessionState
#if IF_TALKUI_PSTN
                    && m_objRingUIData.m_objContactInfo.m_eCallLineType != CALL_LINE_PSTN
#endif
               )
            {
                SendUIRequestAction(TALK_ACTION_FWD_TO);
            }

            return true;
        }
        break;
    case PHONE_KEY_MUTE:
        {
            SendUIRequestAction(TALK_ACTION_MUTE);
            return true;
        }
        break;
    default:
        break;
    }


    return false;
}

// 数字键处理
//来电界面不需要处理数字键
bool CRingProcessor::OnDigitkeyPress(int iKey)
{
    return false;
}

//softKey 按键处理
bool CRingProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    switch (eSoftkey)
    {
    case ST_ANSWER:
        {
            AnswerIncomingCall(false);
            bHandled = true;
        }
        break;
    case ST_CALLINFORWARD:
        {
            //如果是Recall 不用进入forward
            //如果是已经在Forward 界面了 也不需要继续执行
            //如果是显示CallInfo 那么也不需要进入forward
            //这边是进行容错处理
            if (m_eSessionState != SESSION_RECALL)
            {
                bHandled = SendUIRequestAction(TALK_ACTION_FWD_TO, 0, 0, NULL);
            }
        }
        break;
    case ST_SILENCE:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_SILENCE);
        }
        break;
    case ST_CANCEL:
    case ST_REJECT:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_REJECT);
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_ACD_CALLINFO:
        {
            //消息中心打开  而且当前不是recall状态
            if (CallCenter_IsCallInfoOn() && (SESSION_RECALL != m_eSessionState))
            {
                //进入CallInfo界面
                bHandled = SendUIRequestAction(TALK_ACTION_CALLINFO);
            }
        }
        break;
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
    case ST_IGNORE:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_CALL_DECLINE, FALSE, 0, NULL);
        }
        break;
    case ST_DECLINE:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_CALL_DECLINE, TRUE, 0, NULL);
        }
        break;

#if IF_TALKUI_FIVE_PARTY
    case ST_JOIN:
        {
            if (m_objRingUIData.m_bAllowJoin)
            {
                SendUIRequestAction(TALK_ACTION_JOIN);
            }

            bHandled = true;
        }
        break;
#endif

    default:
        break;
    }

    return bHandled;
}

//设置title
void CRingProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //根据是否是recall 来判断
    if (SESSION_RECALL == m_eSessionState)
    {
        //跟lxh 确认， 之前有某些机型是 recall 写在name或number 上的，  以后统一所有的机型都写在title上就行
        //联系人名称还是正常显示
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_RECALL);
    }
    else
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_RINGING);
    }
}

//设置图标列表
void CRingProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    DataRing * pRingingtData = (DataRing *)pData;

    //这边显示的是静音图标
    //需要判断逻辑层上传的 静音标识和音量条是否为0
    if ((true == pRingingtData->bShowSilence) || (voice_GetVolume(VT_RING) == 0))
    {
        AddTalkUIIcon(TALKUI_ICON_SILENCE, true);
    }
}

//给子界面设置数据
void CRingProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objRingUIData, eReason);
    }
}

void CRingProcessor::GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    m_bACDSoftkeySet = false;
}

void CRingProcessor::GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_bUseCustomSoftkey
            && !_TalkUILogicDelegate.IsCustomDynamicSoftkeyOn())
    {
        return;
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (IsAllowACDInfoSoftkey())
    {
        m_bACDSoftkeySet = true;
        //如果当前来电有CallInfo信息，则把第三个Softkey换成CallInfo
        VecSoftkey.push_back(ST_ACD_CALLINFO);
    }
#endif
}

//获取softkey数据信息
void CRingProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //是蓝牙通话 走蓝牙 获取softkey流程
    if (GetSessionState() == SESSION_BTRINGING)
    {
        GetBlueToothRingSoftkey(VecSoftkey);

        return ;
    }
#endif

#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall(GetSessionID()))
    {
        VecSoftkey.push_back(ST_OPENDOOR);
    }
#endif

#if IF_TALKUI_FIVE_PARTY
    if (m_objRingUIData.m_bAllowJoin)
    {
        VecSoftkey.push_back(ST_JOIN);
    }
    else
    {
//          VecSoftkey.push_back(ST_EMPTY);
    }
#endif

    // 设置二级菜单界面对应SoftKeybar的数据
    VecSoftkey.push_back(ST_ANSWER);

    if (m_objRingUIData.m_bCallDeclineEnable)
    {
        VecSoftkey.push_back(ST_IGNORE);
    }

    if (SESSION_RECALL == m_eSessionState)
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
    else
    {
        if (IsAllowCallFwdSoftkey())
        {
            VecSoftkey.push_back(ST_CALLINFORWARD);
        }
        else
        {
            VecSoftkey.push_back(ST_EMPTY);
        }
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (IsAllowACDInfoSoftkey())
    {
        m_bACDSoftkeySet = true;
        //如果当前来电有CallInfo信息，则把第三个Softkey换成CallInfo
        VecSoftkey.push_back(ST_ACD_CALLINFO);
    }
    else
#endif
    {
        //当只有一路来电时，m_objRingUIData.m_bIsSilence || (voice_GetVolume(VT_RING) == 0)条件已经可以满足
        //当已有一路通话时又有来电，当前情况下silence是针对来电话路的，而此时按音量键，调整的则是通话中话路的音量，因此按音量键不能影响此处的判断
        if (IsAllowSilenceSoftkey())
        {
            VecSoftkey.push_back(ST_EMPTY);
        }
        else
        {
            VecSoftkey.push_back(ST_SILENCE);
        }
    }

    bool bAllowReject = true;
#if IF_TALKUI_PSTN
    if (CALL_LINE_PSTN == m_objRingUIData.m_objContactInfo.m_eCallLineType) // PSTN通话无法Reject
    {
        bAllowReject = false;
    }
#endif
#ifdef IF_FEATURE_EMERGENCY
    // 如果是紧急号码的回呼，屏蔽reject键
    if (m_objRingUIData.m_bEmergencyCalled && m_objRingUIData.m_bIsRecalling)
    {
        bAllowReject = false;
    }
#endif

    if (bAllowReject)
    {
        if (m_objRingUIData.m_bCallDeclineEnable)
        {
            VecSoftkey.push_back(ST_DECLINE);
        }
        else
        {
#if IF_BUG_38956
            if (0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
            {
                VecSoftkey.push_back(ST_EMPTY);
            }
            else
#endif
            {
                VecSoftkey.push_back(ST_REJECT);
            }
        }
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
}

//获取蓝牙来电 softkey
void CRingProcessor::GetBlueToothRingSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_objRingUIData.m_bAllowJoin)
    {
#ifndef _WIN32
#warning join
#endif
//      VecSoftkey.push_back(ST_JOIN);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }


    VecSoftkey.push_back(ST_ANSWER);

    //当只有一路来电时，m_objRingUIData.m_bIsSilence || (voice_GetVolume(VT_RING) == 0)条件已经可以满足
    //当已有一路通话时又有来电，当前情况下silence是针对来电话路的，而此时按音量键，调整的则是通话中话路的音量，因此按音量键不能影响此处的判断
    if (m_objRingUIData.m_bIsSilence || (voice_GetVolume(VT_RING) == 0))
    {
        VecSoftkey.push_back(ST_EMPTY);
    }
    else
    {
        VecSoftkey.push_back(ST_SILENCE);
    }

    VecSoftkey.push_back(ST_REJECT);
}

// 是否允许自定义softkey
bool CRingProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_PSTN
    if (CALL_LINE_PSTN ==
            m_objRingUIData.m_objContactInfo.m_eCallLineType) // 当前通话为PSTN，不支持Forward/Reject
    {
        return false;
    }
#endif

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTRINGING)
    {
        return false;
    }
#endif

    return true;
}

bool CRingProcessor::AllowSoftkeyShow(SoftKey_TYPE eType)
{
    if (ST_SILENCE == eType)
    {
        return IsAllowSilenceSoftkey();
    }
    else if (ST_DELETE == eType)
    {
        return m_objRingUIData.m_bCallDeclineEnable;
    }
    else if (ST_CALLINFORWARD == eType)
    {
        return IsAllowCallFwdSoftkey();
    }

    return CBaseTalkUIProcessor::AllowSoftkeyShow(eType);
}


bool CRingProcessor::IsAllowSilenceSoftkey()
{
    if (m_objRingUIData.m_bIsSilence || (voice_GetVolume(VT_RING) == 0))
    {
        return false;
    }

    return true;
}


bool CRingProcessor::IsAllowCallFwdSoftkey()
{
    //http://10.2.1.199/Bug.php?BugID=91316 H323通过标志位bAllowFwd判断是否显示fwd
    if (!m_objRingUIData.m_bIsRecalling
            && m_objRingUIData.m_bAllowFwd
#if IF_TALKUI_PSTN
            && (m_objRingUIData.m_objContactInfo.m_eCallLineType != CALL_LINE_PSTN) // PSTN通话无法Forward
#endif
#if IF_FEATURE_FORBIDDEN_FWD_TRAN
            && configParser_GetConfigInt(kszForwardForbidden) == 0
#endif
#if IF_FEATURE_FORBIDDEN_TRAN_IN_RING
            && configParser_GetConfigInt(kszForwardForbiddenInRinging) == 0
#endif
       )
    {
        return true;
    }

    return false;
}


bool CRingProcessor::IsAllowACDInfoSoftkey()
{
    if (!m_bACDSoftkeySet
            && CallCenter_IsCallInfoOn()
            && m_objRingUIData.m_bCallCenterInfoAvaliable
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && SESSION_RECALL != m_eSessionState)   //非recall状态
    {
        return true;
    }

    return false;
}

// softkey处理(自定义类型)
void CRingProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    strFile = "CallIn.xml";
    strStatus = "CallIn";
}

// 应答来电
void CRingProcessor::AnswerIncomingCall(bool bChannelKey)
{
    //修改应答的标志位
    m_objRingUIData.m_bAnswered = true;
    //如果是通道键接起那么需要发送1  因为逻辑层还需要设置一下通道

    SendUIRequestAction(TALK_ACTION_ANSWER);
}

void CRingProcessor::RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    for (int i = 0; i < VecSoftkey.size();)
    {
        SoftKey_TYPE eSoftkeyType = VecSoftkey[i];

        if (ST_ACD_CALLINFO == eSoftkeyType)
        {
            m_bACDSoftkeySet = true;
        }

        ++i;
    }
}
