// mod-this(TalkUI)
#include "dlgconferencebase.h"
#include "talkui_define.h"
#include "talkuimanager.h"
#include "contactinfoframe.h"
#include "commonfunction.h"
#include "commonwidget.h"
#include "talkuidelegate.h"

// mod-UIKernel
#include "uikernel/languagehelper.h"
#include "uikernel/qtimepiece.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"

// mod-Common
#include "imagemanager/modimagemanager.h"
#include "keymap.h"
#include <configiddefine.h>
#include "picpath.h"
#include "commonunits/modcommonunits.h"

// mod-Talk
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "talk/talklogic/include/commonfunctions.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

#include "uikernel/qscrolltextlabel.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "settingui/include/modsettingui.h"


#include "broadsoft/callrecording/include/callrecord_def.h"
#include "broadsoft/callrecording/include/modcallrecording.h"
#include "uicommon/messagebox/messageboxbase.h"
#include "uicommon/messagebox/modmessagebox.h"

#include "record/include/modrecord.h"
#include "voice/include/modvoice.h"
#include "application/include/modapplication.h"

//构造函数，需传递UI名与UI类型
CDlgConferenceBase::CDlgConferenceBase()
    : CDlgNormalTalkBase(DLG_Conference, TALK_UI_VOICE_CONF)
    , m_bLocalHold(false)
    , m_eRecrodIcon(IT_UNKNOWN)
    , m_bHasCallComming(false)
    , m_nFirstCallId(0)
    , m_bVoiceRecord(false)
    , m_bSecurity(false)
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    , m_eRecordStatue(CALL_RECORD_UNKNOWN)
#endif
    , m_bCallingRoutineExist(false)
    , m_nLastCallingCallId(0)
{
    InitData();
}

//析构函数
CDlgConferenceBase::~CDlgConferenceBase()
{

}

// 初始化页面数据
void CDlgConferenceBase::InitData()
{

}

// 初始化
void CDlgConferenceBase::Init()
{
    CDlgNormalTalkBase::Init();

    //将标志复原
    m_bLocalHold = false;
    m_eRecrodIcon = IT_UNKNOWN;
    m_bHasCallComming = false;
    m_nFirstCallId = 0;

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConferenceBase::Uninit()
{

    CDlgNormalTalkBase::Uninit();

}

// 焦点切换
void CDlgConferenceBase::OnFocusChanged(bool bFocus)
{
    CDlgNormalTalkBase::OnFocusChanged(bFocus);
}

// 通道键处理
bool CDlgConferenceBase::OnChannelKeyPress(int iKey)
{
    bool bHandled = FALSE;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (voice_IsCallCenterMode())
            {
                // callCenterMode交给基类处理
                bHandled = FALSE;
            }
            else
            {
                // 通话中摘手柄时按免提键，则打开群听模式
                if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                {
//                  if (!m_bAllPartyOfConfHold)
                    if (!m_bLocalHold)
                    {
                        voice_SwitchChannel(CHANNEL_GROUP);
                        bHandled = TRUE;
                    }
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = TRUE;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = FALSE;
        break;
    }

    if (!bHandled)
    {
        bHandled = CDlgNormalTalkBase::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

bool CDlgConferenceBase::OnFunckeyPress(int iKey, bool bEXP)
{
    switch (iKey)
    {
    case PHONE_KEY_HOLD:
        {
            //Hold or Resume
            if (m_bLocalHold)
            {
                //Resume通话
                OnUIAction(TALK_UI_ACTION_RESUME);
            }
            else
            {
                //Hold通话
                OnUIAction(TALK_UI_ACTION_HOLD);
            }
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
    case PHONE_KEY_CANCEL:
        {
#if !IF_BUG_25055
            if (false == m_bLocalHold
                    || m_bCallingRoutineExist)
#endif
            {
                OnUIAction(TALK_UI_ACTION_CANCEL);
            }

            return true;
        }
        break;
    case PHONE_KEY_OK:
        {
            OnUIAction(TALK_UI_ACTION_OK);
            return true;
        }
        break;
    default:
        break;
    }

    return CDlgNormalTalkBase::OnFunckeyPress(iKey, bEXP);
}

// 数字键处理
bool CDlgConferenceBase::OnDigitkeyPress(int iKey)
{
    SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}

//设置通话数据内容
void CDlgConferenceBase::SetTalkData(DataLogic2UIBase * pData)
{
    TALKUI_INFO("CDlgConferenceBase SetTalkData");

    CDlgNormalTalkBase::SetTalkData(pData);

    m_bHasCallComming = false;

    yl::string strIncomingCallNum = "";

    //如果有获取到新来电号码 那么有来电的标识设置为true  否则 设置为false
    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
    {
        m_bHasCallComming = true;
    }
    else
    {
        m_bHasCallComming = false;
    }

    // 清空控件的Tip
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);
    if (pLocalConfData != NULL)
    {
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
        m_bVoiceRecord = false;
        YLList<CCallInfo *>::iterator itInfo = pLocalConfData->listCallInfo.begin();
        while (itInfo != pLocalConfData->listCallInfo.end() && *itInfo)
        {
            if ((*itInfo)->IsAutoVoiceRecord())
            {
                m_bVoiceRecord = true;
                m_eRecrodIcon = CDlgBaseTalkUI::GetRecordIconType(*itInfo);
                break;
            }

            ++itInfo;
        }
#endif
        //更新title
        yl::string strTitle = m_bVoiceRecord ? yl::string(TRID_TALKING) : yl::string(TRID_CONFERENCE);
        _TalkUIDelegate.UpdateTitleInfo(strTitle);

        //更新加密图标
        _TalkUIDelegate.AddTitleIcon(PIC_ICON_ENCRYPT, pLocalConfData->bEncrypt);

        //获取Hold标志
        m_bLocalHold =
            pLocalConfData->bHoldConf;// GetCallInfoListHoldFlag(pLocalConfData->listCallInfo, false);

        //根据列表设置话路信息
        YLList<CCallInfo *>::iterator itCallInfo = pLocalConfData->listCallInfo.begin();
        //获取录音图标
        CCallInfo * pCallInfoFirst = (CCallInfo *)(*itCallInfo);
        if (pCallInfoFirst != NULL && !m_bVoiceRecord)
        {
            m_eRecrodIcon = CDlgNormalTalkBase::GetRecordIconType(pCallInfoFirst);
        }

        ////----qt 速度不够的话需要优化
        //获取第一路通话ID
        if (itCallInfo != pLocalConfData->listCallInfo.end())
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);

            if (NULL != pCallInfo)
            {
                m_nFirstCallId = pCallInfo->GetCallID();
            }
        }

        //HD 高清标识
        bool bHD = true;
        m_bCallingRoutineExist = false;
        m_nLastCallingCallId = 0;

        for (; itCallInfo != pLocalConfData->listCallInfo.end(); ++itCallInfo)
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
            if (NULL != pCallInfo)
            {
                bHD = bHD && pCallInfo->IsHDTalking();

                if (ROUTINE_CONNECTING == pCallInfo->m_eStateOfRoutine
                        || ROUTINE_RINGBACK == pCallInfo->m_eStateOfRoutine)
                {
                    m_bCallingRoutineExist = true;
                    m_nLastCallingCallId = pCallInfo->GetCallID();
                }
            }
        }

        //更新title 图标， 如果不显示联系人头像 那么是在title显示   如果是显示联系人头像那么这边全局更新一次   在界面类实例中还会更新  不影响
        _TalkUIDelegate.AddTitleIcon(PIC_ICON_HD, bHD);
    }

}

// 获取配置softkey的文件名和结点名
void CDlgConferenceBase::GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus)
{
    strFile = "Talking.xml";
#if IF_FEATURE_CUSTOM_SOFTKEY
    if (m_bHasCallComming
            && _TalkUIDelegate.IsSoftkeyChangedCallIncome())
    {
        strStatus = "ConferencedNewCallIn";
    }
    else if (m_bLocalHold)
    {
        strStatus = "ConferencedHold";
    }
    else
    {
        strStatus = "Conferenced";
    }
#else
    strStatus = "Conferenced";
#endif
}

//获取通话时间
QCustomTime CDlgConferenceBase::GetTalkTime()
{
    return CDlgNormalTalkBase::GetTalkTime();
}

//是否显示DTMF条
bool CDlgConferenceBase::IsShowDtmfLine() const
{
    return true;
}

//是否显示加密图标,rectEncrypt hold the location of encrypt label
bool CDlgConferenceBase::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return true;
}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgConferenceBase::OnTipChange(TALK_TIP_TYPE eTipType)
{
    CDlgNormalTalkBase::OnTipChange(eTipType);
}



//处理UI事件
bool CDlgConferenceBase::OnUIAction(const QString & strAction)
{
    TALKUI_INFO("ConfUI-OnUIAction(%s)", strAction.toUtf8().data());

    //当前的 CallID
    int iCallID = -1;

    if (TALK_UI_ACTION_UNMUTE == strAction) //解除Mute
    {
        iCallID = GetCurrentActionCallID();

        if (iCallID > -1)
        {
            SendUIRequestAction(TALK_ACTION_CONF_FAR_MUTE, iCallID, 0);
        }
    }
    else if (TALK_UI_ACTION_FARMUTE == strAction)   //锁上Mute
    {
        iCallID = GetCurrentActionCallID();

        if (iCallID > -1)
        {
            SendUIRequestAction(TALK_ACTION_CONF_FAR_MUTE, iCallID, 1);
        }

    }
    else if (TALK_UI_ACTION_FARHOLD == strAction)
    {
        iCallID = GetCurrentActionCallID();

        if (iCallID > -1)
        {
            SendUIRequestAction(TALK_ACTION_CONF_FAR_HOLD, iCallID, 1);
        }
    }
    else if (strAction == TALK_UI_ACTION_SECURITY)  //响应security操作
    {
        if (m_bSecurity)
        {
            int iLeastAccount = talklogic_GetAccountLeastLevFromLocalConf();
            ShowSecurityBox(iLeastAccount);
        }
    }
    else if (TALK_UI_ACTION_REMOVE == strAction) //移除会议一方
    {
        iCallID = GetCurrentActionCallID();

        if (iCallID > -1)
        {
            SendUIRequestAction(TALK_ACTION_REMOVE_CONF_PARTY, iCallID);
        }

    }
    else if (strAction == TALK_UI_ACTION_SPLIT)
    {
        SendUIRequestAction(TALK_ACTION_SPLIT);
    }
    else if (strAction == TALK_UI_ACTION_HOLD)
    {
        // http://10.2.1.199/Bug.php?BugID=33941
        // 开启softkeyLayout的情况下按Hold键需要解Hold
        //if (IsAllowDefineSoftkey()
        //  && (bool)configParser_GetConfigInt(kszCustomSoftkeySwitch)
        //  && m_bLocalHold)
        // 任何情况下 按hold 只要是已经hold了 都需要解hold
        if (m_bLocalHold)
        {
            SendUIRequestAction(TALK_ACTION_RESUME);
        }
        else
        {
            SendUIRequestAction(TALK_ACTION_HOLD);
        }
    }
    else if (strAction == TALK_UI_ACTION_RESUME)
    {
        iCallID = GetCurrentActionCallID();
        if (iCallID > -1)
        {
            SendUIRequestAction(TALK_ACTION_CONF_FAR_HOLD, iCallID, 0);
        }
        else
        {
            SendUIRequestAction(TALK_ACTION_RESUME);
        }
    }
    else if (strAction == TALK_UI_ACTION_NEWCALL)
    {
        SendUIRequestAction(TALK_ACTION_NEWCALL);
    }
    else if (strAction == TALK_UI_ACTION_CANCEL)
    {
        if (m_bCallingRoutineExist
                && m_nLastCallingCallId != 0)
        {
            SendUIRequestAction(TALK_ACTION_REMOVE_CONF_PARTY, m_nLastCallingCallId);
        }
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_MUTE_VOICE)
    {
        SendUIRequestAction(TALK_ACTION_MUTE);
    }
    else if (strAction == TALK_UI_ACTION_CONF_MGR)
    {
        SendUIRequestAction(TALK_ACTION_ENTER_CONF_MANAGER);
    }
    else if (strAction == TALK_UI_ACTION_TRANSFER)
    {
        SendUIRequestAction(TALK_ACTION_TRAN);
    }
    else if (strAction == TALK_UI_ACTION_ANSWER)
    {
        SendUIRequestAction(TALK_ACTION_ANSWER);
    }
    else if (strAction == TALK_UI_ACTION_REJECT)
    {
        if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
           )
        {
            SendUIRequestAction(TALK_ACTION_REJECT);
        }
    }
    else if (strAction == TALK_UI_ACTION_ACD_DISPCODE)
    {
        if (m_nFirstCallId > 0)
        {
            SubMenuData subMenuData;
            subMenuData.strMenuAction = "disposition_code";
            subMenuData.pBeforeSetSubpageData = (void *)&m_nFirstCallId;
            SettingUI_OpenPage(subMenuData);
        }
    }
    else if (strAction == TALK_UI_ACTION_ACD_TRACE)
    {
        if (m_nFirstCallId > 0)
        {
            CallCenter_OriginatedTrace(m_nFirstCallId);
        }
    }
    else if (strAction == TALK_UI_ACTION_ACD_EMERGESCAL)
    {
        if (m_nFirstCallId > 0)
        {
            SubMenuData subMenuData;
            subMenuData.strMenuAction = "emergency_escalation";
            subMenuData.pBeforeSetSubpageData = (void *)&m_nFirstCallId;
            SettingUI_OpenPage(subMenuData);
        }
    }
    else if (strAction == TALK_UI_ACTION_RTP_STATUS)
    {
        DataLogic2UI objData;
        objData.iSessionID = m_iSessionID;
        objData.eSessionState = SESSION_LOCALCONF;      //session状态设置为会议

        yl::string strUIName = DLG_RTPStatusUI;

        _TalkUIManager.UpdateWndByData(objData, strUIName);
        //  SendUIRequestAction(TALK_ACTION_RTP_STATUS);
    }
//#if IF_BUG_24048_RTCP_XR
//  else if (strAction == TALK_UI_ACTION_RTP_STATUS)
//  {
//      SendUIRequestAction(TALK_ACTION_RTP_STATUS);
//  }
//#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    else if (strAction == TALK_UI_ACTION_STARTRECORD)
    {
        CCallInfo * pCallInfo = GetFocusCallInfo();
        if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
            SendUIRequestAction(TALK_ACTION_RECORD_START);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
        }
    }
    else if (strAction == TALK_UI_ACTION_STOPRECORD)
    {
        CCallInfo * pCallInfo = GetFocusCallInfo();
        if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
            SendUIRequestAction(TALK_ACTION_RECORD_STOP);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
        }
    }
    else if (strAction == TALK_UI_ACTION_PAUSERECORD)
    {
        CCallInfo * pCallInfo = GetFocusCallInfo();
        if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
            SendUIRequestAction(TALK_ACTION_RECORD_PAUSE);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
        }
    }
    else if (strAction == TALK_UI_ACTION_RESUMERECORD)
    {
        CCallInfo * pCallInfo = GetFocusCallInfo();
        if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
        {
            pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
            SendUIRequestAction(TALK_ACTION_RECORD_RESUME);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_DEAL_LAST_ACTION), MESSAGEBOX_NOTE, 2000);
        }
    }
#endif
    else
    {
        return CDlgNormalTalkBase::OnUIAction(strAction);
    }

    return true;
}

//获取蓝牙softkey数据信息
void CDlgConferenceBase::GetBlueToothConnectSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        void * pData/* = NULL*/)
{
    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, "");

    //只需要显示1个softkey
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);

}

//设置Softkey数据Action值
void CDlgConferenceBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData/* = NULL*/)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙通话使用蓝牙的softkey
    if (GetSessionState() == SESSION_BTCONNECTING)
    {
        GetBlueToothConnectSoftkey(objSoftkeyData, pData);

        return ;
    }
#endif

    //更新softkey
    if (m_bHasCallComming)
    {
        GetRingStatusSoftKeyBarData(objSoftkeyData, pData);
    }
    else
    {
        GetTalkStatusSoftKeyBarData(objSoftkeyData, pData);
    }
}

#if IF_FEATURE_CUSTOM_SOFTKEY
bool CDlgConferenceBase::IsSoftkeyAllowToShow(const yl::string & strSoftkeyAction,
        DataLogic2UIBase * pData/* = NULL*/)
{
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);
    if (NULL == pLocalConfData)
    {
        return false;
    }

    if (TALK_UI_ACTION_SECURITY == strSoftkeyAction && \
            !m_bSecurity)
    {
        return false;
    }
    else if (TALK_UI_ACTION_RTP_STATUS == strSoftkeyAction && \
             (m_bLocalHold || 0 == configParser_GetConfigInt(kszDisplayVQReportOnUI)) || \
             GetCallInfoListHeldFlag(pLocalConfData->listCallInfo, true))
    {
        return false;
    }
    else if (TALK_UI_ACTION_CONF_MGR == strSoftkeyAction)
    {
        return false;
    }
    else if (!CDlgBaseTalkUI::IsSoftkeyAllowToShow(strSoftkeyAction, pData))
    {
        return false;
    }

    return true;
}

void CDlgConferenceBase::GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData/* = NULL*/)
{
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);
    if (pLocalConfData != NULL)
    {
        SetACDSoftkey(objSoftkeyData, pLocalConfData->listCallInfo);
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    SetRecordSoftkey(objSoftkeyData);
#endif
}
#endif

void CDlgConferenceBase::RefreshRecordSoftKey(CArraySoftKeyBarData & objSoftkeyData)
{
    this->AddUSBRecordSoftkey(objSoftkeyData);
    this->SetRecordSoftkey(objSoftkeyData);
    this->BindSoftkeyAction(objSoftkeyData);
}

//talk状态下的softkey data
bool CDlgConferenceBase::GetTalkStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData)
{
#ifdef _T4X
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall(GetSessionId()))
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_OPENDOOR);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    //V81需求：不使用二级softkey菜单
//  if (SetRecordSoftkey(objSoftkeyData))
//  {
//      return true;
//  }
#endif
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);
    if (pLocalConfData != NULL)
    {
        // 设置第一个按钮的softkey数据
        if (pLocalConfData->bAllowSplit)
        {
            AddSoftkey(objSoftkeyData, m_bVoiceRecord ? "" : TALK_UI_ACTION_SPLIT);
        }
        else
        {
            AddSoftkey(objSoftkeyData, "");
        }
        //!!!，任意取出一路CallInfo作为Conf-Hold信息
        if (pLocalConfData->bHoldConf/*GetCallInfoListHoldFlag(pLocalConfData->listCallInfo, false)*/)
        {
            // 设置Hold状态下的softkey数据
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUME);
            if (pLocalConfData->bAllowNewCall)
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_NEWCALL);
            }
        }
        else
        {
            // 设置Hold状态下的softkey数据
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_HOLD);
        }

        SetACDSoftkey(objSoftkeyData, pLocalConfData->listCallInfo);
    }
    else
    {
        // 插入两个空的softkey
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");
    }

    //T48 没有 COnfManager 界面
#ifndef _T48
    // 设置二级菜单界面对应SoftKeybar的数据
    AddSoftkey(objSoftkeyData, m_bVoiceRecord ? "" : TALK_UI_ACTION_CONF_MGR);
#endif

    // 设置二级菜单界面对应SoftKeybar的数据
    if (m_bCallingRoutineExist)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);
    }
    else
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
    }

    if (pLocalConfData != NULL && pLocalConfData->bAllowSecurity) //在endcall之后插入security softkey
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_SECURITY);
    }

    if (!m_bLocalHold
            && 0 != configParser_GetConfigInt(kszDisplayVQReportOnUI)
            && pLocalConfData != NULL
            && !GetCallInfoListHeldFlag(pLocalConfData->listCallInfo, true))
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RTP_STATUS);
    }

//#if IF_BUG_24048_RTCP_XR
//  if (0 != configParser_GetConfigInt(kszDisplayVQReportOnUI))
//  {
//      AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RTP_STATUS);
//  }
//#endif

#else
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);
    if (pLocalConfData != NULL)
    {
        //!!!，任意取出一路CallInfo作为Conf-Hold信息
        if (pLocalConfData->bHoldConf/*GetCallInfoListHoldFlag(pLocalConfData->listCallInfo, false)*/)
        {
            if (!_TalklogicManager.IsMaxSessionNum())
            {
                //未达到最大通话路数限制，显示New Call
                if (pLocalConfData->bAllowNewCall)
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
                AddSoftkey(objSoftkeyData, "");
            }

            // 设置Hold状态下的softkey数据
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUME);
        }
        else
        {
            AddSoftkey(objSoftkeyData, "");

            // 设置Hold状态下的softkey数据
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_HOLD);
        }

        // 设置第一个按钮的softkey数据
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_HOLD);
    }
    else
    {
        // 插入三个空的softkey
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置二级菜单界面对应SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    //V81需求：不使用二级softkey菜单
    SetRecordSoftkey(objSoftkeyData);
#endif

    return true;
}

bool CDlgConferenceBase::GetRingStatusSoftKeyBarData(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData)
{
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);

#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (m_bHasCallComming && application_IsDoorLineCall())
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_OPENDOOR);
    }
    else
#endif
    {
        // 设置二级菜单界面对应1st SoftKeybar的数据
        //http://bugfree.yealink.com/Bug.php?BugID=87047 来电界面不显示split
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置二级菜单界面对应2nd SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ANSWER);

    if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
#ifdef IF_FEATURE_EMERGENCY
            && (!talklogic_IsEmergencyRecall())
#endif
       )
    {
        // 设置二级菜单界面对应3rd SoftKeybar的数据
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_REJECT);
    }
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置二级菜单界面对应SoftKeybar的数据
    if (m_bCallingRoutineExist)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);
    }
    else
    {
        //这边统一不使用Cancel  改成使用 EndCall
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ENDCALL);
    }

    if (NULL != pLocalConfData)
    {
        if (!m_bLocalHold
                && 0 != configParser_GetConfigInt(kszDisplayVQReportOnUI)
                && !GetCallInfoListHeldFlag(pLocalConfData->listCallInfo, true))
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RTP_STATUS);
        }
    }

    return true;
}

bool CDlgConferenceBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    return false;
}

// 添加Broadsoft ACD功能的Softkey
void CDlgConferenceBase::SetACDSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                                       YLList<CCallInfo *> & listCallInfo)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (CallCenter_IsACDOn()
            && ACD_BROADSOFT == Get_ACDType()
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && ACD_GetAcdStatus() != AS_IDLE)
    {
        //检测是否是ACD关联的账号，不是的话不显示
        bool bShowSoftKey = false;
        for (YLList<CCallInfo *>::const_iterator it = listCallInfo.begin();
                it != listCallInfo.end(); it++)
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*it);
            if (pCallInfo != NULL
                    && pCallInfo->GetAccountID() == ACD_GetAccountId())
            {
                bShowSoftKey = true;
                break;
            }
        }
        if (!bShowSoftKey)
        {
            return;
        }

        //http://10.2.1.199/Bug.php?BugID=36509
        //http://10.2.1.199/Bug.php?BugID=33934
        //在conf 界面 去掉 trace 和 dispcode 两个softkey
        //if(CallCenter_IsDispCodeOn())
        //{
        //  AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_DISPCODE);
        //}

        if (CallCenter_IsEmergencyEscalationOn())
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_EMERGESCAL);
        }
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER
}

//conference 特有的接口

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
CCallInfo * CDlgConferenceBase::GetFocusCallInfo()
{
    int iSessionId = talklogic_GetFocusedSessionID();
    return (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
}

bool CDlgConferenceBase::SetRecordSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
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

    CALL_RECORDING_MODE eMode = callRecord_GetRecordMode(pCallInfo->GetAccountID());

    if (CALL_RECORD_NORMAL == m_eRecordStatue)
    {
        if (eMode == CRM_ALWAYS_PAUSE_RESUME
                || eMode == CRM_DEMAND
                || eMode == CRM_DEMAND_USER_START)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_STARTRECORD);
        }
    }
    else if (CALL_RECORD_START == m_eRecordStatue)
    {
        if (!pCallInfo->IsLocalHold())
        {
            if (eMode == CRM_ALWAYS_PAUSE_RESUME
                    || eMode == CRM_DEMAND
                    || eMode == CRM_DEMAND_USER_START)
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_PAUSERECORD);
            }
        }

        if (eMode == CRM_DEMAND_USER_START)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_STOPRECORD);
        }
    }
    else if (CALL_RECORD_PAUSE == m_eRecordStatue)
    {
        if (!pCallInfo->IsLocalHold())
        {
            if (eMode == CRM_ALWAYS_PAUSE_RESUME
                    || eMode == CRM_DEMAND
                    || eMode == CRM_DEMAND_USER_START)
            {
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RESUMERECORD);
            }
        }

        if (eMode == CRM_DEMAND_USER_START)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_STOPRECORD);
        }
    }

    return true;
}

// 是否运行自定义softkey
bool CDlgConferenceBase::IsAllowDefineSoftkey()
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTCONNECTING)
    {
        return false;
    }
#endif

    return true;
}
#endif
