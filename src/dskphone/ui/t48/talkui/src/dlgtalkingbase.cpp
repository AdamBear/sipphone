// mod-this(TalkUI)
#include "dlgtalkingbase.h"
#include "talkui_define.h"
#include "talkuimanager.h"
#include "contactinfoframe.h"
#include "commonwidget.h"

#include "talkuidelegate.h"

// mod-UIKernel
#include "uikernel/quipushbutton.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uikernel/qscrolltextlabel.h"
#include "uikernel/qtimepiece.h"

// mod-MainWnd
#include "mainwnd/mainwnd.h"

// mod-Common
#include "imagemanager/modimagemanager.h"
#include "keymap.h"
#include <configiddefine.h>
#include "picpath.h"
#include "commonunits/modcommonunits.h"

// mod-Talk
// #include "talk/talklogic_inc.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
// #include "talk/talklogicmanager.h"

// mod-CallCenter
#include "broadsoft/callcenter/include/modcallcenter.h"

// mod-MainWnd
#include "uimanager/rectdefine.h"


#include "baseui/modbaseui.h"

#include "talk/talklogic/include/commonfunctions.h"

#include "callmanager/modcallmanager.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

#include "talk/talklogic/include/modtalklogic.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "settingui/include/modsettingui.h"
#include "setting/include/common.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "broadsoft/xsiactionhelper/include/modxsicommon.h"
#include "broadsoft/callrecording/include/callrecord_def.h"
#include "broadsoft/callrecording/include/modcallrecording.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"

#if IF_FEATURE_DIAL_ON_HOLD
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "talkui/include/modtalkui.h"
#endif

#include "record/include/modrecord.h"
#include "talkuicommon.h"

#include "voice/include/modvoice.h"
#include "application/include/modapplication.h"

//构造函数，需传递UI名与UI类型
CDlgTalkingBase::CDlgTalkingBase()
    : CDlgNormalTalkBase(DLG_Talking, TALK_UI_VOICE_TALK)
    , m_bTransferFailed(false)
    , m_bLocalHold(false)
    , m_bRemoteHold(false)
    , m_bHasCallComming(false)
    , m_eTalkState(TS_TALK)
    , m_nCallID(-1)
    , m_eRecrodIcon(IT_UNKNOWN)
    , m_bCallParkFailed(false)
    , m_eTalkUIType(TALKING_UI_UNKNOWN)
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    , m_eRecordStatue(CALL_RECORD_UNKNOWN)
#endif
    , m_bCallPushFailed(false)
    , m_bSecurity(false)
    , m_eCallType(CT_UNKNOW)
{
    InitData();
}

//析构函数
CDlgTalkingBase::~CDlgTalkingBase()
{

}

// 初始化页面数据
void CDlgTalkingBase::InitData()
{

}

// 初始化
void CDlgTalkingBase::Init()
{
    CDlgNormalTalkBase::Init();

    m_bLocalHold = false;       // 是否处于Hold对方状态
    m_bRemoteHold = false;      // 是否处于对方Hold状态

    m_bTransferFailed = false;  // 是否处于Transfer失败状态
    m_bCallParkFailed = false;   // Park通话是否失败
    m_bCallPushFailed = false;
    m_strParkFailedInfo = ""; // Park失败的原因

    m_nCallID = -1;        //通话CallID
    m_eRecrodIcon = IT_UNKNOWN; // 录音图标类型

    //是否有新来电
    m_bHasCallComming = false;

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTalkingBase::Uninit()
{
    CDlgNormalTalkBase::Uninit();
}

// 焦点切换
void CDlgTalkingBase::OnFocusChanged(bool bFocus)
{

}

//设置通话数据内容
void CDlgTalkingBase::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgNormalTalkBase::SetTalkData(pData);

    //设置是否有新来电标志位
    yl::string strIncomingCallNum = "";

    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        m_bHasCallComming = true;
    }
    else
    {
        m_bHasCallComming = false;
    }

    // 入口参数检查
    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);
    if (NULL == pTalkingData)
    {
        return;
    }

    // 更新加密图标
    _TalkUIDelegate.AddTitleIcon(PIC_ICON_ENCRYPT, pTalkingData->bEncrypt);
//  AddOrDeleteIcon(PIC_ICON_ENCRYPT, pTalkingData->bEncrypt);

    // 通话对应的UI类型
    m_eTalkUIType = pTalkingData->eTalkUIType;
    m_bSecurity = pTalkingData->bAllowSecurity;
    m_eCallType = CT_UNKNOW;
    m_bTransferFailed = pTalkingData->bTranFailed;
    m_bCallParkFailed = pTalkingData->bCallParkFailed;
    m_bCallPushFailed = pTalkingData->bCallPushFailed;
    m_strParkFailedInfo = pTalkingData->strParkFiled;

    //更新title
    UpdateTip(pTalkingData->eTalkUIType);

    CCallInfo * pCallInfo = pTalkingData->pCallInfo;


    if (pCallInfo != NULL)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
        m_eRecordStatue = pCallInfo->m_eRecordStatus;
        TALKUI_INFO("m_eRecordStatue : %d", m_eRecordStatue);
#endif
        AUDIO_CODEC_TYPE eAudioCodec = pCallInfo->GetAudioCodecType();
        _TalkUIDelegate.AddTitleIcon(PIC_ICON_HD, (eAudioCodec & ACT_HD));
        _TalkUIDelegate.AddTitleIcon(PIC_ICON_AMR, (eAudioCodec & ACT_AMR));
        _TalkUIDelegate.AddTitleIcon(PIC_ICON_OPUS, (eAudioCodec & ACT_OPUS));

        m_bLocalHold = pCallInfo->IsLocalHold();
        m_bRemoteHold = pCallInfo->IsRemoteHold();
        m_eTalkState = pCallInfo->m_eTalkState;
        m_nCallID = pCallInfo->GetCallID();
        m_eCallType = pCallInfo->m_eCallType;
        m_eRecrodIcon = CDlgNormalTalkBase::GetRecordIconType(pCallInfo);
    }

}


//获取通话时间内容
QCustomTime CDlgTalkingBase::GetTalkTime()
{
    return CDlgNormalTalkBase::GetTalkTime();
}

//是否显示DTMF条
bool CDlgTalkingBase::IsShowDtmfLine() const
{
#if IF_FEATURE_DIAL_ON_HOLD
    if (m_bLocalHold
            && configParser_GetConfigInt(kszDialOnHold) == 1)
    {
        return false;
    }
#endif
    return true;
}

void CDlgTalkingBase::OnTipChange(TALK_TIP_TYPE eTipType)
{

}

//是否显示加密图标,rectEncrypt hold the location of encrypt label
bool CDlgTalkingBase::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return true;
}

void CDlgTalkingBase::UpdateTip(TALKING_UI_TYPE eTalkUIType)
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
            chWARNING2(FALSE, commonAPI_FormatString("Talk ui type(%d) is unknown!", eTalkUIType).c_str());
            strTitle = "";
        }
        break;
    }

    _TalkUIDelegate.UpdateTitleInfo(strTitle);
    //emit ChangeTitle(strTitle);
}

//处理UI事件
bool CDlgTalkingBase::OnUIAction(const QString & strUIAction)
{
    TALKUI_INFO("TalkUI-OnUIAction(%s)", strUIAction.toUtf8().data());

    if (strUIAction == TALK_UI_ACTION_TRANSFER)
    {
        SendUIRequestAction(TALK_ACTION_TRAN);
    }
    else if (strUIAction == TALK_UI_ACTION_FAIL_TRANSFER)
    {
        // none
    }
    else if (strUIAction == TALK_UI_ACTION_SWAP)
    {
        SendUIRequestAction(TALK_ACTION_SWAP);
    }
    else if (strUIAction == TALK_UI_ACTION_HOLD)
    {
        SendUIRequestAction(TALK_ACTION_HOLD);
    }
    else if (strUIAction == TALK_UI_ACTION_RESUME)
    {
        SendUIRequestAction(TALK_ACTION_RESUME);
    }
    else if (strUIAction == TALK_UI_ACTION_CONFERENCE)
    {
        SendUIRequestAction(TALK_ACTION_CONF);
    }
    else if (strUIAction == TALK_UI_ACTION_SECURITY)
    {
        int iSessionId = talklogic_GetFocusedSessionID();
        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
        if (pCallInfo == NULL)
        {
            return false;
        }

        if (TALKING_UI_LINEDEVICE != m_eTalkUIType && m_bSecurity)
        {
            ShowSecurityBox(pCallInfo->GetAccountID());
        }
    }
    else if (strUIAction == TALK_UI_ACTION_NEWCALL)
    {
        SendUIRequestAction(TALK_ACTION_NEWCALL);
    }
#if IF_BUG_25055
    else if (strUIAction == TALK_UI_ACTION_CANCEL)
#else
    else if (strUIAction == TALK_UI_ACTION_CANCEL && !m_bLocalHold)
#endif
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strUIAction == TALK_UI_ACTION_MUTE_VOICE && !m_bRemoteHold)
    {
        SendUIRequestAction(TALK_ACTION_MUTE);
    }
    else if (strUIAction == TALK_UI_ACTION_ANSWER)
    {
        SendUIRequestAction(TALK_ACTION_ANSWER);
    }
    else if (strUIAction == TALK_UI_ACTION_REJECT)
    {
        if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
           )
        {
            SendUIRequestAction(TALK_ACTION_REJECT);
        }
    }
    else if (strUIAction == TALK_UI_ACTION_PRIHOLD)
    {
        SendUIRequestAction(TALK_ACTION_HOLD_PRIVATE);
    }
    else if (strUIAction == TALK_UI_ACTION_OK)
    {
        SendUIRequestAction(TALK_ACTION_OK);
    }
    else if (strUIAction == TALK_UI_ACTION_PARK)
    {
        SendUIRequestAction(TALK_ACTION_PARK);
    }
    else if (strUIAction == TALK_UI_ACTION_GROUP_PARK)
    {
        SendUIRequestAction(TALK_ACTION_GROUP_PARK);
    }
    else if (strUIAction == TALK_UI_ACTION_ACD_DISPCODE)
    {
        SubMenuData pageData(yl::string("disposition_code"));
        pageData.pBeforeSetSubpageData = (void *)&m_nCallID;
        SettingUI_OpenPage(pageData);
    }
    else if (strUIAction == TALK_UI_ACTION_ACD_TRACE)
    {
        CallCenter_OriginatedTrace(m_nCallID);
    }
    else if (strUIAction == TALK_UI_ACTION_ACD_EMERGESCAL)
    {
        SubMenuData pageData(yl::string("emergency_escalation"));
        pageData.pBeforeSetSubpageData = &m_nCallID;
        SettingUI_OpenPage(pageData);

    }
    else if (strUIAction == TALK_UI_ACTION_RTP_STATUS)
    {
        DataLogic2UI objData;
        objData.iSessionID = m_iSessionID;
        objData.eSessionState = SESSION_TALKING;        //session状态设置为通话

        yl::string strUIName = DLG_RTPStatusUI;

        _TalkUIManager.UpdateWndByData(objData, strUIName);
        //  SendUIRequestAction(TALK_ACTION_RTP_STATUS);
    }
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    else if (strUIAction == TALK_UI_ACTION_CALL_PUSH)
    {
        SendUIRequestAction(TALK_ACTION_CALL_PUSH);
    }
#endif
    else if (strUIAction == TALK_UI_ACTION_POOL)
    {
#if IF_BUG_28455
        yl::string strXmlUrl = configParser_GetConfigString(kszXmlPhonebookUrl);
        if (!strXmlUrl.empty())
        {
            xmlbrowser_Enter(strXmlUrl.c_str());
        }
        else
        {
            talkui_OpenPool();
        }
#else
        talkui_OpenPool();
#endif
    }
//#ifdef _T4X
//#if IF_BUG_24048_RTCP_XR
//  else if (strAction == TALK_UI_ACTION_RTP_STATUS)
//  {
//      SendUIRequestAction(TALK_ACTION_RTP_STATUS);
//  }
//#endif
//#endif
    else
    {
        return CDlgNormalTalkBase::OnUIAction(strUIAction);
    }

    return true;
}

void CDlgTalkingBase::GetBluetoothSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData/* = NULL*/)
{
    //更新softkey
    if (m_bHasCallComming)
    {
        //如果新来电需要改变softkey 那么才使用RingStatus的softkey
        if (true == _TalkUIDelegate.IsSoftkeyChangedCallIncome())
        {
            GetBluetoothRingStatusSoftkey(objSoftkeyData, pData);
        }
        else    //否则 还是使用TalkStatus 的softkey
        {
            GetBluetoothTalkStatusSoftkey(objSoftkeyData, pData);
        }
    }
    else
    {
        GetBluetoothTalkStatusSoftkey(objSoftkeyData, pData);
    }

    //蓝牙不需要添加USB相关softkey
}

//获取通话状态的softkey
void CDlgTalkingBase::GetBluetoothTalkStatusSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData)
{
    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);

    if (pTalkingData == NULL)
    {
        return;
    }

    AddSoftkey(objSoftkeyData, "");


    //根据lxh 意见  t48不需要显示  swap
    //设置softkey2   允许swap 且 机型需要显示swap
    if ((true == pTalkingData->bAllowSwap) && (true == _TalkUIDelegate.IsShowSwapSoftkey()))
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_SWAP);
    }
    else if (m_bLocalHold)
    {
        //蓝牙不存在transfer状态
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUME);
    }
    else
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_HOLD);
    }


    // http://10.2.1.199/Bug.php?BugID=76372
    // 判断hold状态下 添加newcall 的softkey
    //http://10.2.1.199/Bug.php?BugID=92479
#ifdef IF_BT_SUPPORT_PHONE
    if (m_bLocalHold && talklogic_BeAllowNewBTCall())
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_NEWCALL);
    }
    else
#endif
    {
        //没有newcall 只能显示empty
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置最后一个softkey4
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
}

//获取来电状态的softkey
void CDlgTalkingBase::GetBluetoothRingStatusSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        void * pData)
{
    // 设置二级菜单界面对应1st SoftKeybar的数据
    //蓝牙不显示transfer
    AddSoftkey(objSoftkeyData, "");

    // 设置二级菜单界面对应2nd SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ANSWER);

    // 设置二级菜单界面对应3rd SoftKeybar的数据
    if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
       )
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_REJECT);
    }
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置二级菜单界面对应4th SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
}


//设置Softkey数据Action值
void CDlgTalkingBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
                                       DataLogic2UIBase * pData/* = NULL*/)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //是蓝牙通话 走蓝牙 获取softkey流程
    if (GetSessionState() == SESSION_BTTALKING)
    {
        GetBluetoothSoftkey(objSoftkeyData, pData);

        return ;
    }
#endif

    //更新softkey
    if (m_bHasCallComming)
    {
        //如果新来电需要改变softkey 那么才使用RingStatus的softkey
        if (true == _TalkUIDelegate.IsSoftkeyChangedCallIncome())
        {
            GetRingStatusSoftKeyBarData(objSoftkeyData, pData);
        }
        else    //否则 还是使用TalkStatus 的softkey
        {
            GetTalkStatusSoftKeyBarData(objSoftkeyData, pData);
        }
    }
    else
    {
        GetTalkStatusSoftKeyBarData(objSoftkeyData, pData);
    }
}

#if IF_FEATURE_CUSTOM_SOFTKEY
void CDlgTalkingBase::GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                                        DataLogic2UIBase * pData/* = NULL*/)
{
    if (m_bHasCallComming
            && _TalkUIDelegate.IsSoftkeyChangedCallIncome())
    {
        return;
    }

    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);
    if (pTalkingData != NULL
            && pTalkingData->pCallInfo != NULL)
    {
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
        if (pTalkingData->eTalkUIType == TALKING_UI_NORMAL
                && !pTalkingData->pCallInfo->IsLocalHold())
        {
            if (Xsi_IsExecutiveCall(m_iSessionID))
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CALL_PUSH);
            }
        }
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
        SetRecordSoftkey(objSoftkeyData);
#endif

        SetACDSoftkey(objSoftkeyData, pTalkingData->pCallInfo->GetAccountID());
    }
}

bool CDlgTalkingBase::IsSoftkeyAllowToShow(const yl::string & strSoftkeyAction,
        DataLogic2UIBase * pData/* = NULL*/)
{
    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);
    if (NULL == pTalkingData)
    {
        return false;
    }

    if (TALK_UI_ACTION_SWAP == strSoftkeyAction)
    {
        if (!pTalkingData->bAllowSwap)
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_CONFERENCE == strSoftkeyAction)
    {
        if (!pTalkingData->bAllowConf)
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_TRANSFER == strSoftkeyAction)
    {
        if (!pTalkingData->bAllowTran)
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_SECURITY == strSoftkeyAction)
    {
        if (!m_bSecurity)
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_PARK == strSoftkeyAction)
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
#else
        return false
#endif
    }
    else if (TALK_UI_ACTION_GROUP_PARK == strSoftkeyAction)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
        if (m_eSessionState == SESSION_NETWORKCONF)
        {
            return false;
        }

        if (configParser_GetConfigInt(kszGroupCallParkSwitch) != 1
                || configParser_GetConfigInt(kszBroadsoftActive) == 0)
        {
            return false;
        }
#else
        return false;
#endif
    }
    else if (TALK_UI_ACTION_RTP_STATUS == strSoftkeyAction)
    {
        if (m_bLocalHold || m_bRemoteHold
                || 0 == configParser_GetConfigInt(kszDisplayVQReportOnUI))
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_RESUME == strSoftkeyAction)
    {
        //transfered 失败的时候  还是处于transfered状态的  这边是需要显示resume 的softkey
        if (SESSION_TRANSFERRED == m_eSessionState
                && !m_bTransferFailed)
        {
            return false;
        }
    }
    else if (!CDlgBaseTalkUI::IsSoftkeyAllowToShow(strSoftkeyAction, pData))
    {
        return false;
    }

    return true;
}
#endif

void CDlgTalkingBase::RefreshRecordSoftKey(CArraySoftKeyBarData & objSoftkeyData)
{
    this->AddUSBRecordSoftkey(objSoftkeyData);
    this->SetRecordSoftkey(objSoftkeyData);
    this->BindSoftkeyAction(objSoftkeyData);
}

//talk状态下的softkey data
bool CDlgTalkingBase::GetTalkStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData)
{
    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);
    if (pTalkingData != NULL)
    {
        CCallInfo * pCallInfo = pTalkingData->pCallInfo;
        if (pCallInfo != NULL)
        {
#ifdef IF_FEATURE_ENTRANCE_GUARD
            if (application_IsDoorLineCall(GetSessionId()))
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_OPENDOOR);
            }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
            //V81需求：不使用二级softkey菜单
//          if (SetRecordSoftkey(objSoftkeyData))
//          {
//              return true;
//          }
#endif
            //设置softkey1,Transfer内容
            if (pTalkingData->bAllowTran
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
                    && !pTalkingData->bAttachVoice
#endif
               )
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_TRANSFER);
            }
            else if (m_bTransferFailed)
            {
                //http://10.3.5.199/Bug.php?BugID=62577
                //这边还是用 transfer 的softkey， 因为每按一次就会刷新一次界面 这个时候 其实还是有transfer 的功能
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_TRANSFER);
                //  AddSoftkey(objSoftkeyData, "", LANG_TRANSLATE(TRID_TRANSFER));
            }
            // http://10.3.5.199/Bug.php?BugID=65715
            else if (TALKING_UI_NETWORKCONF == pTalkingData->eTalkUIType)
            {
                CArraySoftKeyBarData vecSoftkeyBar;
                SetRTPSoftkey(vecSoftkeyBar, pTalkingData);
                AddSoftkey(objSoftkeyData, vecSoftkeyBar.empty() ? "" : vecSoftkeyBar[0].m_strSoftkeyAction,
                           vecSoftkeyBar.empty() ? "" : vecSoftkeyBar[0].m_strSoftkeyTitle);
            }
            else
            {
                AddSoftkey(objSoftkeyData, "");
            }

#ifndef _T48
            //设置softkey2
            if (pTalkingData->bAllowSwap)
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_SWAP);
            }
            else if (pCallInfo->IsLocalHold())
#else
            //根据lxh 意见  t48不需要显示  swap
            if (pCallInfo->IsLocalHold())
#endif
            {
                //  AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUME);
                //  http://10.3.5.199/Bug.php?BugID=62591
                //在transfered 的时候不显示resume 的softkey
                if (SESSION_TRANSFERRED == m_eSessionState)
                {
                    //transfered 失败的时候  还是处于transfered状态的  这边是需要显示resume 的softkey
                    if (true == m_bTransferFailed)
                    {
                        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUME);

                    }
                    else
                    {
                        AddSoftkey(objSoftkeyData, "");
                    }

                }
                else
                {
                    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUME);
                }

            }
            else
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_HOLD);
            }

            //设置softkey3
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
            if (pTalkingData->eTalkUIType == TALKING_UI_NORMAL
                    && !pTalkingData->pCallInfo->IsLocalHold())
            {
                if (Xsi_IsExecutiveCall(m_iSessionID))
                {
                    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CALL_PUSH);
                }
            }
#endif
#if !IF_BUG_23438
            if (pCallInfo->GetBLAType() == BLA_BROADSOFT_SCA
                    && !pCallInfo->IsLocalHold())
            {
                //v80 移植v73技术支持 在开启SCA功能的时候 优先使用conference功能
                if (pTalkingData->bAllowConf
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
                        && !pTalkingData->bAttachVoice
#endif
                   )
                {
                    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CONFERENCE);
                }
                //移到网页去处理PRIHOLD softeky http://10.2.1.199/Bug.php?BugID=96370
                else
                {
                    // 如果该路通话时BroadSoft Share Line通话,则将第三个按键设置成Private Hold
                    AddSoftkey(objSoftkeyData, "");
                }
            }
            else
#endif
            {
                // 设置softkey3设置Conference内容
                if ((pCallInfo->m_eCallType == CT_MULTICAST_OUT) || (pCallInfo->m_eCallType == CT_MULTICAST_IN))
                {
                    AddSoftkey(objSoftkeyData, "");
                }
                else if (pTalkingData->bAllowConf
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
                         && !pTalkingData->bAttachVoice
#endif
                        )
                {
                    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CONFERENCE);
                }
                else
                {
                    //不允许Conf则显示New Call
                    if (pCallInfo->IsLocalHold())
                    {
                        if (talklogic_BeAllowNewCall())
                        {
                            //未达到最大通话路数限制，显示New Call
                            if (pTalkingData->bAllowNewCall)
                            {
                                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_NEWCALL);
                            }
                            else
                            {
                                AddSoftkey(objSoftkeyData, "");
                            }
                        }
                        else
                        {
                            //达到最大通话路数限制，显示为空
                            AddSoftkey(objSoftkeyData, "");
                        }
                    }
                    else
                    {
                        //达到最大通话路数限制，显示为空
                        AddSoftkey(objSoftkeyData, "");
                    }
                }
            }

#if IF_FEATURE_DIAL_ON_HOLD
            if (pCallInfo->IsLocalHold()
                    && configParser_GetConfigInt(kszDialOnHold) == 1)
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_POOL);
            }
            else
#endif
            {
                // 设置最后一个softkey4
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
            }

            if (pTalkingData->bAllowSecurity)   //在endcall之后插入security softkey
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_SECURITY);
            }

            SetACDSoftkey(objSoftkeyData, pCallInfo->GetAccountID());

            SetRTPSoftkey(objSoftkeyData, pTalkingData);
        }
    }
    else
    {
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//      if (SetRecordSoftkey(objSoftkeyData))
//      {
//          return true;
//      }
#endif
        // 设置前三个softkey
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");

        // 设置最后一个softkey4
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    //V81需求：不使用二级softkey菜单
    SetRecordSoftkey(objSoftkeyData);
#endif

    //http://10.2.1.199/Bug.php?BugID=87226 调整softkey顺序Park在callrecord的softkey之后
    SetParkSoftkey(objSoftkeyData);

    return true;
}

bool CDlgTalkingBase::GetRingStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData)
{
    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(pData);
    //http://10.2.1.199/Bug.php?BugID=89621 在出现softkey为空的情况下把RPTStatus softkey移到第一位
    bool bAddRtpstatus = false;

#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD

    if (pTalkingData == NULL)
    {
        return false;
    }

    if (!pTalkingData->bAttachVoice)
#endif
    {
#ifdef IF_FEATURE_ENTRANCE_GUARD
        if (m_bHasCallComming && application_IsDoorLineCall())
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_OPENDOOR);
        }
        else
#endif
        {
            // 设置二级菜单界面对应1st SoftKeybar的数据
            if (SESSION_NETWORKCONF != m_eSessionState
                    && SESSION_LOCALCONF != m_eSessionState
                    && (pTalkingData->bAllowTran || pTalkingData->bTranFailed)
#ifdef IF_FEATURE_EMERGENCY
                    && (!talklogic_IsEmergencyRecall())
#endif
               )
            {

                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_TRANSFER);

            }
            else
            {
                // t48不会显示空的softkey，所以不用添加空softkey
                SetRTPSoftkey(objSoftkeyData, pTalkingData);
                bAddRtpstatus = true;
            }
        }
    }

    // 设置二级菜单界面对应2nd SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ANSWER);

    // 设置二级菜单界面对应3rd SoftKeybar的数据
    if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
#ifdef IF_FEATURE_EMERGENCY
            && (!talklogic_IsEmergencyRecall())
#endif
       )
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_REJECT);
    }
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置二级菜单界面对应4th SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);

    //hold和held情况下不显示rtp status按钮
    if (!bAddRtpstatus)
    {
        SetRTPSoftkey(objSoftkeyData, pTalkingData);
    }

    return true;
}

// 添加Broadsoft ACD功能的Softkey
void CDlgTalkingBase::SetACDSoftkey(CArraySoftKeyBarData & objSoftkeyData, int iAccountID)
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
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_DISPCODE);
        }

        if (CallCenter_IsEmergencyEscalationOn())
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_EMERGESCAL);
        }

        if (CallCenter_IsTraceOn())
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_TRACE);
        }
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
}

// 添加Broadsoft Park功能的Softkey
void CDlgTalkingBase::SetParkSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    if (m_eTalkUIType == TALKING_UI_NETWORKCONF)
    {
        return;
    }
    if (configParser_GetConfigInt(kszCallParkSwitch) == 1
            /*&& configParser_GetConfigInt(kszBroadsoftActive) != 0*/)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_PARK);
    }

    if (configParser_GetConfigInt(kszGroupCallParkSwitch) == 1
            && configParser_GetConfigInt(kszBroadsoftActive) != 0)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_GROUP_PARK);
    }
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
}

// 添加RTP Softkey
bool CDlgTalkingBase::SetRTPSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                                    const DataTalking2UI * pTalkData)
{
    // 若已存在RTP, 则不添加
    for (int i = 0; i < objSoftkeyData.size(); ++i)
    {
        if (TALK_UI_ACTION_RTP_STATUS == objSoftkeyData[i].m_strSoftkeyAction)
        {
            return true;
        }
    }

    // Hold/Held/配置未开, 则不添加
    if (NULL == pTalkData
            || NULL == pTalkData->pCallInfo
            || pTalkData->pCallInfo->IsLocalHold()
            || pTalkData->pCallInfo->IsRemoteHold()
            || 0 == configParser_GetConfigInt(kszDisplayVQReportOnUI))
    {
        return false;
    }

    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RTP_STATUS);
    return true;
}

// 是否运行自定义softkey
bool CDlgTalkingBase::IsAllowDefineSoftkey()
{
    return true;
}

// 是否有显示提示信息
bool CDlgTalkingBase::IsShowTip()
{
#ifdef _T48
    return false;
#else
    return m_bHasCallComming;
#endif
}

// 通道键处理
bool CDlgTalkingBase::OnChannelKeyPress(int iKey)
{
    //为什么之前只判断 transfer 失败 ？？？？
//  if (m_bTransferFailed)
    if (m_bTransferFailed || m_bCallParkFailed || m_bCallPushFailed)
    {
//      ExitNotify();
        SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
        // 通道按键继续往下传
    }

    bool bHandled = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
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
                    if (m_eTalkState == TS_TALK)
                    {
                        TALKUI_INFO("CTalkingUI : Open GroupListening");
                        //if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
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
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = true;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }
    if (!bHandled)
    {
        bHandled = CDlgNormalTalkBase::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

// 功能键处理
bool CDlgTalkingBase::OnFunckeyPress(int iKey, bool bEXP)
{
    if (m_bTransferFailed || m_bCallParkFailed || m_bCallPushFailed)
    {
        SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
    }

    switch (iKey)
    {
    case PHONE_KEY_HOLD:
        {
            // http://10.3.5.199/Bug.php?BugID=62591
            // 在transfered 状态下  不处理   且当前不是transferfailed状态
            if (TALKING_UI_TRANSFERRED == m_eTalkUIType && false == m_bTransferFailed)
            {
                return true;
            }

            //Hold or Resume
            if (m_bLocalHold)
            {
                //Resume通话
                OnUIAction(TALK_UI_ACTION_RESUME);
                return true;
            }
            else
            {
                //Hold通话
                OnUIAction(TALK_UI_ACTION_HOLD);
                return true;
            }
            return true;
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            //Transfer按键
            OnUIAction(TALK_UI_ACTION_TRANSFER);
            return true;
        }
        break;
    case PHONE_KEY_CONFERENCE:
        {
            //Conference按键
            OnUIAction(TALK_UI_ACTION_CONFERENCE);
            return true;
        }
        break;
    case PHONE_KEY_MUTE:
        {
            //Mute按键
            OnUIAction(TALK_UI_ACTION_MUTE_VOICE);
            return true;
        }
        break;
    case PHONE_KEY_OK:
        {
            OnUIAction(TALK_UI_ACTION_OK);

            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
#if !IF_BUG_25055
            if (false == m_bLocalHold)
#endif
            {
                OnUIAction(TALK_UI_ACTION_CANCEL);
            }
            return true;
        }
        break;
    default:
        break;
    }

    return CDlgNormalTalkBase::OnFunckeyPress(iKey, bEXP);
}

// 数字键处理
bool CDlgTalkingBase::OnDigitkeyPress(int iKey)
{
    if (m_bTransferFailed || m_bCallParkFailed || m_bCallPushFailed)
    {
        SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
    }
#if IF_FEATURE_DIAL_ON_HOLD
    if (m_bLocalHold
            && configParser_GetConfigInt(kszDialOnHold) == 1)
    {
        SendUIRequestAction(TALK_ACTION_DIAL_ON_HOLD, iKey);
        return true;
    }
#endif
    SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}


//是否显示音量条
bool CDlgTalkingBase::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                      QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = TalkUIlogic_IsAllowSilenceDuringCall() ? 0 : 1;
    iMax = 15;

    rtVolume = RECT_VOLUMEBAR;
    QRect rectTalkUI = geometry();
    rtVolume.moveBottom(rectTalkUI.bottom() - 6);// 离底边6像素

    return true;
}

// 获取配置softkey的文件名和结点名
void CDlgTalkingBase::GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus)
{
    strFile = "Talking.xml";

    switch (m_eTalkState)
    {
    case TS_TALK:
        strStatus = "Talk";
        break;
    case TS_HOLD_HELD: // 和TS_HOLD一样界面
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
    bool bHoldState = false;
    if (m_eSessionState == SESSION_ATSCONF
            || m_eSessionState == SESSION_NETWORKCONF)
    {
        strStatus = "NetworkConferenced";
        bHoldState = true;
    }
    else if (m_eCallType == CT_MULTICAST_IN)
    {
        strStatus = "ListeningPaging";
        bHoldState = true;
    }
    else if (m_eCallType == CT_MULTICAST_OUT)
    {
        strStatus = "Paging";
        bHoldState = true;
    }
    else if (m_eSessionState == SESSION_BETRANSFERRED)
    {
        strStatus = "BeTransferred";
    }

    if (bHoldState
            && (m_eTalkState == TS_HOLD_HELD
                || m_eTalkState == TS_HOLD))
    {
        strStatus.append("Hold");
    }

    //http://10.2.1.199/Bug.php?BugID=89152 添加m_eTalkState == TS_TALK的限制
    if (m_bHasCallComming
            && _TalkUIDelegate.IsSoftkeyChangedCallIncome() \
            && m_eTalkState == TS_TALK)
    {
        strStatus = "NewCallIn";
    }
#endif
}

//事件过滤器
bool CDlgTalkingBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

//  transfe 会自动消失，  park不会
    //switch(pEvent->type())
    //{
    //case QEvent::MouseButtonPress:
    //  {
    //      // 如果有提示失败，在有按键时，要取消失败的提示
    //      if (m_bTransferFailed || m_bCallParkFailed)
    //      {
    //          SendUIRequestAction(TALK_ACTION_EXIT_FAILD_INFO);
    //      }
    //  }
    //  break;
    //default:
    //  break;
    //}

    return false;
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CDlgTalkingBase::SetRecordSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    TALKUI_INFO("CDlgTalkingBase::SetRecordSoftkey m_eRecordStatue : %d", m_eRecordStatue);

    // 先服务器录音再插入USB再拔出，先清掉record softkey
    for (int i = 0; i < objSoftkeyData.size();)
    {
        if (objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_STOPRECORD
                || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_STARTRECORD
                || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_PAUSERECORD
                || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_RESUMERECORD)
        {
            objSoftkeyData.erase(objSoftkeyData.begin() + i);
        }
        else
        {
            i++;
        }
    }

    //本地录制和blsf录制不共存
    if (modRecord_IsRecordEnable(RECORD_AUDIO))
    {
        return false;
    }

    int iSessionId = talklogic_GetFocusedSessionID();
    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
    if (pCallInfo == NULL)
    {
        return false;
    }
    if (pCallInfo->IsLocalHold())
    {
        return false;
    }
    CALL_RECORDING_MODE eType = callRecord_GetRecordMode(pCallInfo->GetAccountID());

    TALKUI_INFO("CDlgTalkingBase::SetRecordSoftkey Mode : %d", eType);
    if (CALL_RECORD_NORMAL == m_eRecordStatue)
    {
        if (eType == CRM_ALWAYS_PAUSE_RESUME
                || eType == CRM_DEMAND
                || eType == CRM_DEMAND_USER_START)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_STARTRECORD);
        }
    }
    else if (CALL_RECORD_START == m_eRecordStatue)
    {
        if (eType == CRM_ALWAYS_PAUSE_RESUME
                || eType == CRM_DEMAND)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_PAUSERECORD);
        }
        else if (eType == CRM_DEMAND_USER_START)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_PAUSERECORD);
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_STOPRECORD);
        }
    }
    else if (CALL_RECORD_PAUSE == m_eRecordStatue)
    {
        if (eType == CRM_ALWAYS_PAUSE_RESUME
                || eType == CRM_DEMAND)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUMERECORD);
        }
        else if (eType == CRM_DEMAND_USER_START)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUMERECORD);
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_STOPRECORD);
        }
    }

    return true;
}
#endif
