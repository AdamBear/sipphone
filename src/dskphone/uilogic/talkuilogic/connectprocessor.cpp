#include "connectprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "uilogicconnectuibase.h"

#include "talkuilogicframemanager.h"

CConnectProcessor::CConnectProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_CONNECT)
{
    m_pTalkUI = NULL;
}

CConnectProcessor::~CConnectProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CConnectProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objConnectUIData.Reset();
}

//释放前解除UI绑定前调用
void CConnectProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CConnectProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicConnectUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CConnectProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取callid
int CConnectProcessor::GetCallID()
{
    return m_objConnectUIData.m_objContactInfo.iCallID;
}

//是否是视频通话
bool CConnectProcessor::IsVideoTalk()
{
    return m_objConnectUIData.m_objContactInfo.bVideoTalk;
}

//获取数据
CConnectUIData * CConnectProcessor::GetConnectUIData()
{
    return &m_objConnectUIData;
}

//设置数据
void CConnectProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
//  m_objDialUIData.Reset();
    DataConnect * pConnectData = (DataConnect *)pData;

    if (NULL != pConnectData->pCallInfo)
    {
        //更新本地hold属性
        m_objConnectUIData.m_bLocalHold = pConnectData->pCallInfo->IsLocalHold();
        m_objConnectUIData.m_bRemoteHold =
            pConnectData->pCallInfo->IsRemoteHold();      //更新远程hold属性
    }

    //更新音频Mute状态
    m_objConnectUIData.m_bAudioMute = _TalkUILogicDelegate.GetAudioMuteStatus();

    //获取联系人信息
    GetContactInfoFromCallInfo(m_objConnectUIData.m_objContactInfo, pConnectData->pCallInfo);

    //test
#if TALKUI_DEBUG_TEST
    m_objConnectUIData.m_objContactInfo.bVideoTalk = true;
#endif
}

// 通道键处理
bool CConnectProcessor::OnChannelKeyPress(int iKey)
{
    return CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
}

// 功能键处理
bool CConnectProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_TRANSFER:
        {
            //Transfer按键
            SendUIRequestAction(TALK_ACTION_TRAN);
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

            SendUIRequestAction(TALK_ACTION_CANCEL);
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
bool CConnectProcessor::OnDigitkeyPress(int iKey)
{
    // 彩铃时发送DTMF
    SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}

//softKey 按键处理
bool CConnectProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    if (eSoftkey == ST_CANCEL)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);

        bHandled = true;
    }
    else if (eSoftkey == ST_TRANSFER)
    {
        // http://192.168.1.99/Bug.php?BugID=14505
        // !!!T2X是否增加1s的延迟

        SendUIRequestAction(TALK_ACTION_TRAN);

        bHandled = true;
    }
#if IF_BUG_23548
    else if (eSoftkey == ST_CALLCOMPLETIONCALLBACK)
    {
        SendUIRequestAction(TALK_ACTION_CALL_BACK);
        bHandled = true;
    }
#endif

    return bHandled;
}

//设置title
void CConnectProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    DataConnect * pConnectData = (DataConnect *)pData;

    if (pConnectData->bConnectByTran
#if IF_BUG_30961
            && !(pConnectData->pCallInfo != NULL && pConnectData->pCallInfo->m_bTranByBLF)
#endif
       )
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_TRANSFERING);
    }
    //跟lxh确认过，  以后所有机型都显示calling
    else
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALLING);
    }
}

//设置图标列表
void CConnectProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    DataConnect * pConnectData = (DataConnect *)pData;

    //这边是sip信令加密
    if (true == pConnectData->bEncrypt)
    {
        AddTalkUIIcon(TALKUI_ICON_SIPENCRY, true);
    }

    //http://10.2.1.199/Bug.php?BugID=61033
    //在连接的时候不判断显示 高清图标
}

//给子界面设置数据
void CConnectProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objConnectUIData, eReason);
    }
}


//获取softkey数据信息
void CConnectProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙通话使用蓝牙的softkey
    if (GetSessionState() == SESSION_BTCONNECTING)
    {
        GetBlueToothConnectSoftkey(VecSoftkey);

        return ;
    }
#endif

    //t49 做了特殊处理  因为呼出键偏小
    //  // http://10.2.1.199/Bug.php?BugID=75535
    // 特殊处理 只显示一个softkey的时候 由UI来显示  但是connectprocessor 不做修改

    //http://10.2.1.199/Bug.php?BugID=59075
    //Transfer Connecting状态下，服务器还没有回复180回铃信息，此时Transfer按键无效
    if (SESSION_RINGBACK_TRAN == m_eSessionState)
    {
        VecSoftkey.push_back(ST_TRANSFER);
    }
#if IF_BUG_23548
    else if (SESSION_RINGBACK == m_eSessionState
             && configParser_GetConfigInt(kszCallCompletionCallBackSwitch) == 1)
    {
        VecSoftkey.push_back(ST_CALLCOMPLETIONCALLBACK);
    }
#endif
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    // 设置SoftKeybar1
    VecSoftkey.push_back(ST_EMPTY);

    // 设置SoftKeybar2
    VecSoftkey.push_back(ST_EMPTY);

    // 设置SoftKeybar3
    VecSoftkey.push_back(ST_CANCEL);

}

//获取蓝牙softkey数据信息
void CConnectProcessor::GetBlueToothConnectSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);

    //只需要显示1个softkey
    VecSoftkey.push_back(ST_CANCEL);

}

// 是否允许自定义softkey
bool CConnectProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_PSTN
    if (CALL_LINE_PSTN ==
            m_objConnectUIData.m_objContactInfo.m_eCallLineType) // 当前通话为PSTN，不支持Forward/Reject
    {
        return false;
    }
#endif

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTCONNECTING)
    {
        return false;
    }
#endif

    return true;
}

// softkey处理(自定义类型)
void CConnectProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    switch (m_eSessionState)
    {
    case SESSION_RINGBACK:
    case SESSION_RINGBACK_TRAN:
        {
            strFile = "RingBack.xml";

            if (m_eSessionState == SESSION_RINGBACK)
            {
                strStatus = "RingBack";
            }
            else
            {
                strStatus = "SemiAttendTransBack";
            }
        }
        break;
    case SESSION_CONNECTING:
    case SESSION_CONNECTING_TRAN:
    case SESSION_NETWORKCONF_CONNECT:
    case SESSION_BLACONNECTING:
        {
            strFile = "Connecting.xml";

            if (m_eSessionState == SESSION_CONNECTING_TRAN)
            {
                strStatus = "SemiAttendTrans";
            }
            else
            {
                strStatus = "Connecting";
            }
        }
        break;
    default:
        break;
    }
}





