#include "baseidlepopboxinfo.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "contacts/directory/include/moddirinterface.h"
#include "mainwnd/mainwnd.h"
#include "mainwnd/autovolumebar.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qstatusbar.h"
#include "account/include/modaccount.h"
#include "commonunits/modcommonunits.h"
#include "setting/include/common.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/callcompletion/include/modcallcompletion.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "qtcommon/qmisc.h"
#include "idlescreen/include/modidlescreen.h"
#include "setting/include/modmenu.h"
#include "setting/include/modsetting.h"
#include "settingui/include/modsettingui.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "setting/src/menu/menucontroller.h"
#include "setting_page_define.h"
#include "setting/include/modsetting.h"
#include "settingui/src/dlgbluetoothbase.h"
#include "wifiui/src/dlgwifibase.h"
#include "messageui/include/modmessageui.h"
#include "settingui/src/dlgbluetoothbase.h"

//lcm
#include "uicommon/messagebox/modmessagebox.h"
#include "extended/hotdesking/src/hotdesking.h"
#include "extended/hotdesking/include/modhotdesking.h"

#include "voice/include/modvoice.h"

#include "talkui/include/modtalkui.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "idlescreenui/dlgidlescreen.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#endif
namespace
{
const int MSG_ID_AUTOP_HINT = 2;
}
//////////////////////////////////////////////////////////////////////////
// PickUpMenu状态改变回调
void OnPickupMenuChange(STATUS_ACTION eAction)
{
    //退出PickupMenu
    switch (eAction)
    {
    case STATUS_ACTION_DELETE:
        {
            talkui_ExitPickupMenu();
            idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, NULL);
        }
        break;
    case STATUS_ACTION_UPDATE:
        {
            talkui_UpdatePickupMenu();
        }
        break;
    default:
        break;
    }
}


CBaseIdlePopupboxProxy::CBaseIdlePopupboxProxy(int iType)
    : m_iType(iType)
{

}

CBaseIdlePopupboxProxy::~CBaseIdlePopupboxProxy()
{

}

int  CBaseIdlePopupboxProxy::PopupBox()
{
    int iResult = MessageBox_Destory;
    if (m_PopupBoxInfo.m_iPopupBoxType == MESSAGEBOX_OkCancel)
    {

        //修改为新的MessageBoxManager 来获取弹出框
        MessageBox_ShowCommonMsgBox(&_IdleNoteManager,/*"Note", */
                                    m_PopupBoxInfo.m_strNote,
                                    MESSAGEBOX_OkCancel,
                                    -1,
                                    m_PopupBoxInfo.m_strCancelBtnTitle,
                                    m_PopupBoxInfo.m_strConfirmBtnTitle,
                                    m_PopupBoxInfo.m_nMsgBoxId);
    }
    else
    {
        if (m_PopupBoxInfo.m_iPopupBoxType == MESSAGEBOX_SPECIAL) //voicmail 单独
        {
            VecMsgBoxSoftkey voicemail;
            MsgBoxSoftkey sexit(LANG_TRANSLATE(TRID_EXIT), MessageBox_Cancel);
            MsgBoxSoftkey sconn(LANG_TRANSLATE(TRID_CONNECT), MessageBox_OK);
            MsgBoxSoftkey sview(LANG_TRANSLATE(TRID_VIEW), MessageBox_View);
            voicemail.push_back(sexit);
#if IF_BUG_30016
            if (configParser_GetConfigInt(kszShowGPickUpInMsgbox) != 0
                    && configParser_GetConfigInt(kszGPickupSwitch) != 0)
            {
                voicemail.push_back(MsgBoxSoftkey(LANG_TRANSLATE(TRID_GROUP_CALL_PICKUP), MessageBox_GPickup));
            }
#endif
            voicemail.push_back(sview);
            voicemail.push_back(sconn);
            MessageBox_ShowCommonMsgBox(&_IdleNoteManager,
                                        m_PopupBoxInfo.m_strNote, voicemail, -1, m_PopupBoxInfo.m_nMsgBoxId);


        }
        else if (m_PopupBoxInfo.m_iPopupBoxType == MESSAGEBOX_GUEST_UNLOCK) //guest unlock弹窗
        {
            MessageBox_ShowGuestUnlockBox(&_IdleNoteManager, m_PopupBoxInfo.m_nMsgBoxId);
        }
#if IF_BUG_30016
        else if (m_PopupBoxInfo.m_iPopupBoxType ==
                 MESSAGEBOX_GPICKUP)//misscall 单独,增加GPickup softkey
        {
            QString strTitle;
            VecMsgBoxSoftkey vecSoftkey;

            // cancel 按钮
            if (!m_PopupBoxInfo.m_strCancelBtnTitle.isEmpty())
            {
                strTitle = m_PopupBoxInfo.m_strCancelBtnTitle;
            }
            else
            {
                strTitle = LANG_TRANSLATE(TRID_CANCEL);
            }

            vecSoftkey.push_back(MsgBoxSoftkey(strTitle, MessageBox_Cancel));

            // gpicpup 按钮
            vecSoftkey.push_back(MsgBoxSoftkey(LANG_TRANSLATE(TRID_GROUP_CALL_PICKUP), MessageBox_GPickup));

            // ok 按钮
            if (!m_PopupBoxInfo.m_strConfirmBtnTitle.isEmpty())
            {
                strTitle = m_PopupBoxInfo.m_strConfirmBtnTitle;
            }
            else
            {
                strTitle = LANG_TRANSLATE(TRID_OK);
            }

            vecSoftkey.push_back(MsgBoxSoftkey(strTitle, MessageBox_OK));

            MessageBox_ShowCommonMsgBox(&_IdleNoteManager,
                                        m_PopupBoxInfo.m_strNote, vecSoftkey, m_PopupBoxInfo.nTimes, m_PopupBoxInfo.m_nMsgBoxId);
        }
#endif
        else
        {
            MessageBox_ShowCommonMsgBox(&_IdleNoteManager,/*LANG_TRANSLATE(TRID_NOTE), */
                                        m_PopupBoxInfo.m_strNote,
                                        m_PopupBoxInfo.m_iPopupBoxType,
                                        m_PopupBoxInfo.nTimes,
                                        m_PopupBoxInfo.m_strCancelBtnTitle,
                                        m_PopupBoxInfo.m_strConfirmBtnTitle,
                                        m_PopupBoxInfo.m_nMsgBoxId);
        }
    }

    return iResult;
}

bool CBaseIdlePopupboxProxy::ProcessFinishPopupBox(int nReason)
{
    bool bHandled = true;
    switch (nReason)
    {
#if IF_BUG_30016
    case MessageBox_GPickup:
        {
            bHandled = talklogic_CallOutByGPickup();
        }
        break;
#endif
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

#if IF_BUG_23548
//////////////////////////////////////////////////////////////////////////
CCompletionCallBackPopboxProxy::CCompletionCallBackPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_CALL_COMPLETION_CALLBACK)
{
}

CCompletionCallBackPopboxProxy::~CCompletionCallBackPopboxProxy()
{
}

CBaseIdlePopupboxProxy * CCompletionCallBackPopboxProxy::CreateInstance()
{
    return new CCompletionCallBackPopboxProxy();
}

bool CCompletionCallBackPopboxProxy::PreparePopupboxInfo()
{
    if (configParser_GetConfigInt(kszCallCompletionCallBackSwitch) != 1)
    {
        return false;
    }

    m_PopupBoxInfo.ReSet();

    CompletionCallBackData completionCallBackData;
    bool bRet = CallCompletion_GetCallBackData(completionCallBackData);
    if (!bRet || completionCallBackData.nAccountId == -1 || completionCallBackData.bPopMsgBox == false)
    {
        IDLEUI_WARN("completionCallBackData bRet = %d, nAccountId = %d, bPopMsgBox = %d",
                    bRet, completionCallBackData.nAccountId, completionCallBackData.bPopMsgBox);
        return false;
    }

    m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_CALL_COMPLETION);
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_INITIAL_CALL_BACK);
    return true;
}

bool CCompletionCallBackPopboxProxy::ProcessFinishPopupBox(int iResult)
{
    switch (iResult)
    {
    case MessageBox_OK:
        {
            // 拨打号码
            yl::string strName = "Call Back";
            yl::string strNumber = configParser_GetConfigString(kszInitiateCallback);
            if (!strNumber.empty())
            {
                CompletionCallBackData completionCallBackData;
                CallCompletion_GetCallBackData(completionCallBackData);
                talklogic_CallOut(strNumber, strName, completionCallBackData.nAccountId);
            }
            // 无需再弹
            CallCompletion_SetCallBackData(-1, false);
        }
        break;
    case MessageBox_Cancel:
        {
            // 无需再弹
            CallCompletion_SetCallBackData(-1, false);
        }
        break;
    default:
        break;
    }
    return true;
}
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
//////////////////////////////////////////////////////////////////////////
CRecordSavePopboxProxy::CRecordSavePopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_SAVE_RECORD_FILE)
{
}

CRecordSavePopboxProxy::~CRecordSavePopboxProxy()
{
}

CBaseIdlePopupboxProxy * CRecordSavePopboxProxy::CreateInstance()
{
    return new CRecordSavePopboxProxy();
}

bool CRecordSavePopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_NOTE);
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(
                                   TRID_THE_RECORDINGS_HAVE_BEEN_PRESERVED_IN_THE_USB_DEVICE);
    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_NOTE;
    m_PopupBoxInfo.nTimes = 2000;
    return true;
}

bool CRecordSavePopboxProxy::ProcessFinishPopupBox(int iResult)
{
    idleScreen_ClearPopupFlag(PS_STATE_SAVE_RECORD_FILE);
    idleScreen_DropStatus(PS_STATE_SAVE_RECORD_FILE);
    return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
CCallCompletionPopboxProxy::CCallCompletionPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_CALL_COMPLETION)
    , m_pCallCompletionData(NULL)
{
    m_pCallCompletionData = new CallCompletionData();
}

CCallCompletionPopboxProxy::~CCallCompletionPopboxProxy()
{
    if (NULL != m_pCallCompletionData)
    {
        delete m_pCallCompletionData;
        m_pCallCompletionData = NULL;
    }
}

CBaseIdlePopupboxProxy * CCallCompletionPopboxProxy::CreateInstance()
{
    return new CCallCompletionPopboxProxy();
}

bool CCallCompletionPopboxProxy::PreparePopupboxInfo()
{
    if (NULL == m_pCallCompletionData)
    {
        return false;
    }

    m_pCallCompletionData->Reset();
    m_PopupBoxInfo.ReSet();

    //获取订阅内容成功
    if (CallCompletion_GetSubscribeData(m_pCallCompletionData))
    {
        yl::string strTarget = "";
        //这里只需要处理有空闲状态
        if (m_pCallCompletionData->m_eStatus == CCS_REMOTE_CALLAVALIABLE)
        {
            //如果显示名不为空使用显示名，否则使用用户名
            strTarget = m_pCallCompletionData->m_strCalleeDisplayName != "" ?
                        m_pCallCompletionData->m_strCalleeDisplayName : m_pCallCompletionData->m_strCalleeUsername;
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_DIALING);
        }
        else if (m_pCallCompletionData->m_eStatus == CCS_WAIT_COMFIMING)
        {
            // 如果显示名不为空使用显示名，否则使用用户名
            strTarget = m_pCallCompletionData->m_strCalleeDisplayName != "" ?
                        m_pCallCompletionData->m_strCalleeDisplayName : m_pCallCompletionData->m_strCalleeUsername;
            // 播放提示音
            voice_PlayTone(TONE_BUSY, 3000);
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_WAIT_FOR);
        }

        if (strTarget.empty())
        {
            IDLEUI_INFO("CIdleNoteManager::GetCallCompletionTipInfo: no content to show and return !!!");
        }
        else
        {
            m_PopupBoxInfo.m_strNote = m_PopupBoxInfo.m_strNote + " " + toQString(strTarget.c_str()) + "?";
            m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_CALL_COMPLETION);
        }
    }

    return !m_PopupBoxInfo.m_strNote.isEmpty();
}

bool CCallCompletionPopboxProxy::ProcessFinishPopupBox(int iResult)
{
    if (NULL == m_pCallCompletionData)
    {
        return false;
    }

    if (m_pCallCompletionData->m_eStatus == CCS_REMOTE_CALLAVALIABLE)
    {
        switch (iResult)
        {
        case MessageBox_OK:
            {
                //点击了确定，拨打号码
                CallCompletion_CallOut(m_pCallCompletionData->m_nCallId);
            }
            break;
        case MessageBox_Cancel:
            {
                //取消.
                CallCompletion_UnsubscribeByCallId(m_pCallCompletionData->m_nCallId);
            }
            break;
        case MessageBox_Destory:
        case  MessageBox_Close:
            {
                if (idleScreen_GetPopupStatus(PS_STATE_CALL_COMPLETION) == POPUP_WINDOW_POPUPING)
                {
                    idleScreen_SetPopupStatus(PS_STATE_CALL_COMPLETION, POPUP_WINDOW_POPUP);
                }
            }
            break;
        default:
            break;
        }
    }
    else if (m_pCallCompletionData->m_eStatus == CCS_WAIT_COMFIMING)
    {
        // 停止提示音
        voice_StopTone(TONE_BUSY);
        switch (iResult)
        {
        case MessageBox_OK:
            {
                CallCompletion_Wait(m_pCallCompletionData->m_nCallId);
            }
            break;
        case MessageBox_Cancel:
            {
                //点击了取消，不订阅
                CallCompletion_UnsubscribeByCallId(m_pCallCompletionData->m_nCallId);
            }
            break;
        case MessageBox_Destory:
        case  MessageBox_Close:
            {
                if (idleScreen_GetPopupStatus(PS_STATE_CALL_COMPLETION) == POPUP_WINDOW_POPUPING)
                {
                    idleScreen_SetPopupStatus(PS_STATE_CALL_COMPLETION, POPUP_WINDOW_POPUP);
                }
            }
            break;
        default:
            break;
        }
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
CPickupMenuPopboxProxy::CPickupMenuPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_PICKUP_MENU)
{

}

CPickupMenuPopboxProxy::~CPickupMenuPopboxProxy()
{

}

bool CPickupMenuPopboxProxy::PreparePopupboxInfo()
{
    return true;
}

CBaseIdlePopupboxProxy * CPickupMenuPopboxProxy::CreateInstance()
{
    return new CPickupMenuPopboxProxy();
}

int CPickupMenuPopboxProxy::PopupBox()
{
    talkui_EnterPickupMenu();
    idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, OnPickupMenuChange);
    return 0;
}

bool CPickupMenuPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    return true;
}
//////////////////////////////////////////////////////////////////////////
CAutoPPopboxProxy::CAutoPPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION)
{

}

CAutoPPopboxProxy::~CAutoPPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CAutoPPopboxProxy::CreateInstance()
{
    return new CAutoPPopboxProxy();
}

bool CAutoPPopboxProxy::PreparePopupboxInfo()
{
    // 输入了配置文件中的特征码
    std::string strCodeName = Autop_AutopCodeGetCurrentName().c_str();

    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_AUTOP_TO) + " " + toQString(
                                   strCodeName.c_str()) + "\n"
                               + LANG_TRANSLATE(TRID_IF_PROVISION_NOW);

    return true;
}

bool CAutoPPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            idleScreen_ClearPopupFlag(PS_STATE_AUTO_PROVISION);

            AutopStartCodeConnect();

            return true;
        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_AUTO_PROVISION);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_AUTO_PROVISION) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_AUTO_PROVISION, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
CCallForwardPopboxProxy::CCallForwardPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_CALLFORWARD)
{

}

CCallForwardPopboxProxy::~CCallForwardPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CCallForwardPopboxProxy::CreateInstance()
{
    return new CCallForwardPopboxProxy();
}

bool CCallForwardPopboxProxy::PreparePopupboxInfo()
{
    yl::string strPopValue = configParser_GetConfigString(kszDisplayForwardCallPopup);
    if (!strPopValue.empty())
    {
        bool bPopNotify = atoi(strPopValue.c_str());
        if (!bPopNotify)
        {
            return false;
        }
    }

    // 获取最后一个Forward的信息
    //  int nCountInfo = Dir_GetContactCount(kszForwardNotifId, LOCAL_LOG);
    //
    //  ContactListData lsData;
    //  lsData.m_nIndexFrom = nCountInfo - 1;
    //  lsData.m_nCountToGet = 1;
    //  lsData.m_nTotalCount = 1;
    //
    //  Dir_GetContactListByGroupId(&lsData, kszForwardNotifId, LOCAL_LOG);
    //
    //  QString strTarget;
    //  if (lsData.m_listId.size() > 0)
    //  {
    //      // 此处只获取最新的一个forward号码
    //      SmartPtr pDetail = Dir_GetDetailById(lsData.m_listId[0],
    //                                           LOCAL_LOG);
    //
    //      if (pDetail.IsEmpty()
    //          || pDetail->GetType() != DIR_TYPE_LOCALLOG_CALLLOG)
    //      {
    //          return false;
    //      }
    //
    //      LocalLogItemRecord* pData = (LocalLogItemRecord *)(pDetail.Get());
    //
    //      if (pData != NULL)
    //      {
    //          if (!pData->m_StrForwardDisplayName.empty())
    //          {
    //              strTarget = toQString(pData->m_StrForwardDisplayName.c_str());
    //          }
    //          else
    //          {
    //              strTarget = toQString(pData->m_strForwardSipName.c_str());
    //          }
    //      }
    //  }

    IdleStatusItem item = idleScreen_GetTopStatusPopType();

    QString strTarget = toQString(item.m_strHint.c_str());

    if (!strTarget.isEmpty())
    {
        m_PopupBoxInfo.m_strIcon = "";
        m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_FORWARD_TO) + ": " + strTarget;
#if IF_FEATURE_FORBID_FORWARD_CALLLOG
        if (configParser_GetConfigInt(kszForwardCallLog) == 0)
        {
            m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_EXIT);
            m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Cancel;
        }
        else
#endif
        {
            m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_EXIT);
            m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_VIEW);
#if IF_BUG_30016
            if (configParser_GetConfigInt(kszShowGPickUpInMsgbox) != 0
                    && configParser_GetConfigInt(kszGPickupSwitch) != 0)
            {
                m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_GPICKUP;
            }
#endif
        }
        return true;
    }
    return false;
}

bool CCallForwardPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_CALLFORWARD);

            //用户点击View按钮情况
            //按键view 事件，打开CallLog Forward界面
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszForWardedLogId);


        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_CALLFORWARD);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_CALLFORWARD) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_CALLFORWARD, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        CBaseIdlePopupboxProxy::ProcessFinishPopupBox(nReason);
        break;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
CMissCallPopboxProxy::CMissCallPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_MISSCALL)
{

}

CMissCallPopboxProxy::~CMissCallPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CMissCallPopboxProxy::CreateInstance()
{
    return new CMissCallPopboxProxy();
}

bool CMissCallPopboxProxy::PreparePopupboxInfo()
{
    int nCountInfo = Dir_GetContactCount(kszMissNotifId, LOCAL_LOG);

    if (nCountInfo > 0)
    {
        m_PopupBoxInfo.m_strIcon = "";
        m_PopupBoxInfo.m_strNote = QString::number(nCountInfo) + " " + LANG_TRANSLATE(
                                       TRID_NEW_MISSED_CALLS);
        m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_EXIT);
        m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_VIEW);
#if IF_BUG_30016
        if (configParser_GetConfigInt(kszShowGPickUpInMsgbox) != 0
                && configParser_GetConfigInt(kszGPickupSwitch) != 0)
        {
            m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_GPICKUP;
        }
#endif

        return true;
    }
    return false ;
}

bool CMissCallPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_MISSCALL);
            //按键view 事件,打开Missed Call 界面

            if (BsftLog_IsEnable()
                    && configParser_GetConfigInt(kszBWCallLogandDir) == 1)
            {
                DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszMissLogId);
            }
            else
            {
                DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszMissLogId);
            }

        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_MISSCALL);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_MISSCALL) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_MISSCALL, POPUP_WINDOW_POPUP);
            }
        }
        break;

    default:
        CBaseIdlePopupboxProxy::ProcessFinishPopupBox(nReason);
        break;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
CVoiceMailPopboxProxy::CVoiceMailPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_VOICEMAIL)
{

}

CVoiceMailPopboxProxy::~CVoiceMailPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CVoiceMailPopboxProxy::CreateInstance()
{
    return new CVoiceMailPopboxProxy();
}

bool CVoiceMailPopboxProxy::PreparePopupboxInfo()
{
    //Voice Mail 弹窗提醒
    //读取弹窗提示信息
    m_VoiceMailData.ReSet();
    m_VoiceMailData.m_nCountInfo = 0;
    ListAccounts listAccount = acc_GetAvailableAccountList();
    ListAccounts::iterator iter = listAccount.begin();
    for (; iter != listAccount.end(); ++iter)
    {
        int nAccountId = *iter;
        //只有新的消息才要显示
        if (acc_GetUnReadVoiceMailNum(nAccountId) > 0)
        {
            ++(m_VoiceMailData.m_nCountInfo);//记录有多少个账号信息提示
            m_VoiceMailData.m_nOnlyAccountId = nAccountId; //只有单个账户信息时的账号id
            if (configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber, nAccountId) != "")
            {
                m_VoiceMailData.m_bIsSetVoiceMailNumber = true;//是否已经设置了语音信箱号码
            }
        }
    }

    m_VoiceMailData.m_nUnReadCount = acc_GetUnReadVoiceMailNum();
    if (m_VoiceMailData.m_nUnReadCount <= 0)  //当前没有未读的voicemail，则不弹出弹出框
    {
        return false;
    }
    if (configParser_GetConfigInt(kszHideUnreadVMNumber) == 1)
    {
        m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_NEW_VOICE_MAIL);
    }
    else
    {
        m_PopupBoxInfo.m_strNote = QString("%1 ").arg(m_VoiceMailData.m_nUnReadCount) + LANG_TRANSLATE(
                                       TRID_NEW_VOICE_MAIL);
    }
    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_SPECIAL;//PB_TYPE_VoiceMail;
    return true;
}

bool CVoiceMailPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch ((MESSAGEBOX_RESULT)nReason)
    {
    case MessageBox_Cancel:
        {
            //清除弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_VOICEMAIL);
        }
        break;
    case MessageBox_OK:
        {

            Setting_ConnectVoiceMail(
                acc_GetLastVMailAccountId());//connect返回true代表是进入voicemail界面,不清状态

            break;
        }
    case MessageBox_View:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_VOICEMAIL);

#if IF_FEATURE_METASWITCH_VOICEMAIL
            if (MTSWVM_IsMessagesListEnable())
            {
                SettingUI_OpenPage(SubMenuData("OpenMenu(message_list.xml)", 1), true);
            }
            else
#endif
            {
                if (m_VoiceMailData.m_nCountInfo > 1)
                {
                    //多个账号有信息提示
                    SettingUI_OpenViewVoiceMailPage();
                }
                else
                {
                    SettingUI_OpenViewVoiceMailPage(m_VoiceMailData.m_nOnlyAccountId);
                }
            }

        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            //该情况为程序主动强制关闭或来电等
            if (idleScreen_GetPopupStatus(PS_STATE_VOICEMAIL) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_VOICEMAIL, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        CBaseIdlePopupboxProxy::ProcessFinishPopupBox(nReason);
        break;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
CTextMessagePopboxProxy::CTextMessagePopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_TEXTMESSAGE)
{

}

CTextMessagePopboxProxy::~CTextMessagePopboxProxy()
{

}

CBaseIdlePopupboxProxy * CTextMessagePopboxProxy::CreateInstance()
{
    return new CTextMessagePopboxProxy();
}

bool CTextMessagePopboxProxy::PreparePopupboxInfo()
{
    AccountTextMsgListData listData;
    if (!Setting_GetTextMsgList(&listData))
    {
        return false;
    }

    //记录有多少个账号信息提示
    int nCountInfo = 0;
    //记录提示新消息个数
    int nUnReadCount = 0;

    YLList<AccountTextMsgData>::iterator it = listData.m_listMsg.begin();
    YLList<AccountTextMsgData>::iterator it_end = listData.m_listMsg.end();

    for (int i = 0; it != it_end; ++it, i++)
    {
        //只有新的消息才要显示
        if ((*it).m_nUnReadCount > 0)
        {
            nCountInfo++;
            //nUnReadCount = (*it).m_nUnReadCount;
        }
    }

    IdleStatusItem item = idleScreen_GetTopStatusPopType();

    nUnReadCount = atoi(item.m_strHint.c_str());

    if (nUnReadCount > 0)
    {
        m_PopupBoxInfo.m_strIcon = "";
        m_PopupBoxInfo.m_strNote = QString::number(nUnReadCount) + " " + QString(LANG_TRANSLATE(
                                       TRID_UNREAD_TEXT_MESSAGE));
        m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_EXIT);
        m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_VIEW);
#if IF_BUG_30016
        if (configParser_GetConfigInt(kszShowGPickUpInMsgbox) != 0
                && configParser_GetConfigInt(kszGPickupSwitch) != 0)
        {
            m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_GPICKUP;
        }
#endif
    }

    m_TextMsgData.m_nCountInfo = nCountInfo;
    m_TextMsgData.m_nUnReadCount = nUnReadCount;
    return true;
}

bool CTextMessagePopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_TEXTMESSAGE);

            //用户点击View按钮情况
            //单个账号信息单个新消息提示情况
            if (m_TextMsgData.m_nCountInfo == 1 && m_TextMsgData.m_nUnReadCount == 1)
            {
                //打开inbox detail message
                MessageUI_GotoInboxMessageDlg(true);
            }
            else if (m_TextMsgData.m_nCountInfo > 0)
            {
                //多个账号有信息提示，转向页面InBox 目录页面
                MessageUI_GotoInboxMessageDlg(false);
            }
        }
        break;
    case MessageBox_Cancel:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_TEXTMESSAGE);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_TEXTMESSAGE) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_TEXTMESSAGE, POPUP_WINDOW_POPUP);
            }
        }
        break;

    default:
        CBaseIdlePopupboxProxy::ProcessFinishPopupBox(nReason);
        break;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
CNetworkWorkFailedPopboxProxy::CNetworkWorkFailedPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_NETWORK_FAILED)
{

}

CNetworkWorkFailedPopboxProxy::~CNetworkWorkFailedPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CNetworkWorkFailedPopboxProxy::CreateInstance()
{
    return new CNetworkWorkFailedPopboxProxy();
}
//此类将支持两种状态的处理 PS_STATE_NETWORK_FAILED PS_STATE_NETWORK_OK_BUT_CONFLICT
//http://10.3.5.199/Bug.php?BugID=63336
bool CNetworkWorkFailedPopboxProxy::PreparePopupboxInfo()
{
    // 遍历获取提示信息
    NETWORK_STATUS CNWState = idleScreen_GetNetworkStatus();
    switch (CNWState)
    {
    /*
      case NS_CABLE_DISCONNECTED:
        {

         }
     break;
      case NS_REQUEST_IP:
        {

         }
     break;*/
    case NS_CHECKING_MAC:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_CHECKING_MAC);
        }
        break;
    case NS_INVALID_MAC:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_NEED_UPDATE_MAC);
        }
        break;
    case NS_ALL_IP_CONFLICT:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_ALL_IP_CONFLICT);
        }
        break;
    case NS_IPV4_CONFLICT:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_IPV4_CONFLICT);
        }
        break;
    case NS_IPV6_CONFLICT:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_IPV6_CONFLICT);
        }
        break;
    case NS_CABLE_DISCONNECTED:
    case  NS_NETWORK_STOP:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_NETWORK_UNAVAILABLE);
        }
        break;
    case NS_UPDATING:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_UPDATING_NETWORK);
        }
        break;
    case NS_REQUEST_IP:
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_OBTAINING_IP_ADDRESS);
        }
        break;
    default:
        {
            if (PS_STATE_NETWORK_FAILED == m_iType)
            {
                m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_NETWORK_UNAVAILABLE);
            }
        }
        break;

    }
    return true;
}

bool CNetworkWorkFailedPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
    case MessageBox_Cancel:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(m_iType);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(m_iType) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(m_iType, POPUP_WINDOW_POPUP);
            }
        }
        break;

    default:
        break;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
CRemoteContactDownloadFailedPopboxProxy::CRemoteContactDownloadFailedPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED)
{

}

CRemoteContactDownloadFailedPopboxProxy::~CRemoteContactDownloadFailedPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CRemoteContactDownloadFailedPopboxProxy::CreateInstance()
{
    return new CRemoteContactDownloadFailedPopboxProxy();
}

bool CRemoteContactDownloadFailedPopboxProxy::PreparePopupboxInfo()
{
    ////---- 确认功能是否除去
    // 遍历获取提示信息
    bool isFound = false;
    YLList<IdleStatusItem> listStatusItem = idleScreen_GetStatusList();
    for (YLList<IdleStatusItem>::ListIterator iter = listStatusItem.begin();
            iter != listStatusItem.end(); ++iter)
    {
        IdleStatusItem StatusItem = *iter;
        if (StatusItem.m_nId == PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED)
        {
            isFound = true;
            break;
        }
    }

    return false;
}

bool CRemoteContactDownloadFailedPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            idleScreen_ClearPopupFlag(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_REMOTE_CONTACT_DOWNLOAD_FAILED, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
CCRTRequestPopboxProxy::CCRTRequestPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_CTR_REQUEST)
{

}

CCRTRequestPopboxProxy::~CCRTRequestPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CCRTRequestPopboxProxy::CreateInstance()
{
    return new CCRTRequestPopboxProxy();
}

bool CCRTRequestPopboxProxy::PreparePopupboxInfo()
{
    if (!accessCtr_NeedRequest())
    {
        return false;
    }

    // 弹出提示
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_IF_ALLOW_REMOTE_CONTROL);
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_BLOCK);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);


    //

    return true;
}

bool CCRTRequestPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            // 成功弹出提示后清除标志
            accessCtr_PopSuccess();
            idleScreen_ClearPopupFlag(PS_STATE_CTR_REQUEST);
            accessCtr_SetPermissions(TRUE);

        }
        break;
    case  MessageBox_Cancel:
        {
            // 成功弹出提示后清除标志
            accessCtr_PopSuccess();
            idleScreen_ClearPopupFlag(PS_STATE_CTR_REQUEST);
            accessCtr_SetPermissions(FALSE);

        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_CTR_REQUEST) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_CTR_REQUEST, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;

    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
namespace
{
const int MSG_ID_HOTDESK_WAITING = 100;
}

void HotDeskWaitMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }
    if (pMessageBox->GetID() == MSG_ID_HOTDESK_WAITING)
    {
        _HotDesking.SetLogonWizard(FALSE);
        SettingUI_EnterPageByType(PAGE_TYPE_HOTDESK);
    }
}

CHotdesktPopboxProxy::CHotdesktPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_NOTE_HOTDESK)
{

}

CHotdesktPopboxProxy::~CHotdesktPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CHotdesktPopboxProxy::CreateInstance()
{
    return new CHotdesktPopboxProxy();
}

bool CHotdesktPopboxProxy::PreparePopupboxInfo()
{
    // 弹出提示
#if IF_BUG_25349
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_IF_CLEAR_ALL_ACCONT_CONFIGRATION);
    yl::string strClearAccount = configParser_GetConfigString(kszHotDeskingClearAccount);
    YLList<yl::string> listAccountID;
    if (commonAPI_splitStringBySeparator(strClearAccount, ',', listAccountID)
            && listAccountID.size() > 0)
    {
        bool bClearAll = false;
        if (listAccountID.size() == 1)
        {
            int iAccountID = atoi(strClearAccount.c_str());
            if (iAccountID > acc_AccountNum() || iAccountID <= 0)
            {
                bClearAll = true;
            }
        }

        if (!bClearAll)
        {
            m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_IF_CLEAR_YOUR_ACCONT_CONFIGRATION);
        }
    }
#else
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_IF_CLEAR_ALL_ACCONT_CONFIGRATION);
#endif
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_CANCEL);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    return true;
}

bool CHotdesktPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    if (nReason == MessageBox_OK)
    {
        idleScreen_ClearPopupFlag(PS_STATE_NOTE_HOTDESK);
        idleScreen_DropStatus(PS_STATE_NOTE_HOTDESK);

        MessageBox_ShowCommonMsgBox(WRAPPER_MSGBOX_CBFUN(HotDeskWaitMsgBoxCallBack),
                                    LANG_TRANSLATE(TRID_CLEARING_ACCOUNT_PLEASE_WAIT)
                                    , MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_HOTDESK_WAITING);

        hotDesking_ClearUserInfo();

        return true;
    }
    else if (nReason == MessageBox_Cancel)
    {
        idleScreen_ClearPopupFlag(PS_STATE_NOTE_HOTDESK);
        idleScreen_DropStatus(PS_STATE_NOTE_HOTDESK);
    }
    return false;
}
//////////////////////////////////// bluetooth //////////////////////////////////////
CAdapterAddPopboxProxy::CAdapterAddPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_ADAPTER_ADD)
{

}

CAdapterAddPopboxProxy::~CAdapterAddPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CAdapterAddPopboxProxy::CreateInstance()
{
    return new CAdapterAddPopboxProxy();
}

bool CAdapterAddPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_strIcon = "";
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(
                                   TRID_ADAPTER_HAS_ADDED_WHETHER_SCAN_BLUETOOTH_DEVICES_RIGHT_NOW);
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_CANCEL);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    return true;
}

bool CAdapterAddPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_ADAPTER_ADD);
            idleScreen_DropStatus(PS_STATE_ADAPTER_ADD);

            //用户点击OK按钮情况
            if (SettingUI_OpenPage(SubMenuData("OpenSubPage(bluetooth)"), true))
            {
                CBaseDialog * pBaseDlg = UIManager_GetPrivateDlg("CDlgBluetoothBase");
                if (NULL != pBaseDlg
                        && pBaseDlg->inherits("CDlgBluetoothBase"))
                {
                    CDlgBluetoothBase * pBlueDlg = static_cast<CDlgBluetoothBase *>(pBaseDlg);
                    if (NULL != pBlueDlg)
                    {
                        pBlueDlg->SetBluetooth(true);
                    }
                }
            }

        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_ADAPTER_ADD);
            idleScreen_DropStatus(PS_STATE_ADAPTER_ADD);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_ADAPTER_ADD) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_ADAPTER_ADD, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }
    return true;
}
////////////////////////////////// bluetooth ////////////////////////////////////////
CAdapterRemovePopboxProxy::CAdapterRemovePopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_ADAPTER_REMORE)
{

}

CAdapterRemovePopboxProxy::~CAdapterRemovePopboxProxy()
{

}

CBaseIdlePopupboxProxy * CAdapterRemovePopboxProxy::CreateInstance()
{
    return new CAdapterRemovePopboxProxy();
}

bool CAdapterRemovePopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_strIcon = "";
    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Cancel;
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_ADAPTER_HAS_REMOVED_OUT);
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_OK);
    return true;
}

bool CAdapterRemovePopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_ADAPTER_REMORE);
            idleScreen_DropStatus(PS_STATE_ADAPTER_REMORE);
        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_ADAPTER_REMORE);
            idleScreen_DropStatus(PS_STATE_ADAPTER_REMORE, false);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_ADAPTER_REMORE) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_ADAPTER_REMORE, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

//////////////////////////////////// Wi-Fi adapter add //////////////////////////////////////
CWifiAdapterAddPopboxProxy::CWifiAdapterAddPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_WIFI_ADAPTER_ADD)
{

}

CWifiAdapterAddPopboxProxy::~CWifiAdapterAddPopboxProxy()
{

}

CBaseIdlePopupboxProxy * CWifiAdapterAddPopboxProxy::CreateInstance()
{
    return new CWifiAdapterAddPopboxProxy();
}

bool CWifiAdapterAddPopboxProxy::PreparePopupboxInfo()
{
    bool bForbidWifi = (configParser_GetConfigInt(kszForbidWifi) == 1);

    if (bForbidWifi)
    {
        return false;
    }
    else
    {
        m_PopupBoxInfo.m_strIcon = "";
        m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(
                                       TRID_WIFI_ADAPTER_HAS_ADDED_SCANNING_THE_WIFI_DEVICES_RIGHT_NOW);
        m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_CANCEL);
        m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    }
    return true;
}

bool CWifiAdapterAddPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_WIFI_ADAPTER_ADD);
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);

            //用户点击OK按钮情况
            if (SettingUI_OpenPage(SubMenuData("OpenSubPage(wifi)"), true))
            {
                CBaseDialog * pBaseDlg = UIManager_GetPrivateDlg("CDlgWifiBase");
                if (NULL != pBaseDlg
                        && pBaseDlg->inherits("CDlgWifiBase"))
                {
                    CDlgWifiBase * pWifiDlg = static_cast<CDlgWifiBase *>(pBaseDlg);
                    if (NULL != pWifiDlg)
                    {
                        pWifiDlg->SetWifiOpenClose(true);
                    }
                }
            }

        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_WIFI_ADAPTER_ADD);
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);
        }
        break;
    case MessageBox_Destory:
    case MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_WIFI_ADAPTER_ADD) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_WIFI_ADAPTER_ADD, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }
    return true;
}
////////////////////////////////// Wi-Fi adapter remove ////////////////////////////////////////
CWifiAdapterRemovePopboxProxy::CWifiAdapterRemovePopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_WIFI_ADAPTER_REMOVE)
{

}

CWifiAdapterRemovePopboxProxy::~CWifiAdapterRemovePopboxProxy()
{

}

CBaseIdlePopupboxProxy * CWifiAdapterRemovePopboxProxy::CreateInstance()
{
    return new CWifiAdapterRemovePopboxProxy();
}

bool CWifiAdapterRemovePopboxProxy::PreparePopupboxInfo()
{
    bool bForbidWifi = (configParser_GetConfigInt(kszForbidWifi) == 1);

    if (bForbidWifi)
    {
        return false;
    }
    else
    {
        m_PopupBoxInfo.m_strIcon = "";
        m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Cancel;
        m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_WIFI_ADAPTER_HAS_REMOVED_OUT);
        m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_OK);
    }

    return true;
}

bool CWifiAdapterRemovePopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_WIFI_ADAPTER_REMOVE);
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_REMOVE);
        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_WIFI_ADAPTER_REMOVE);
            idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_REMOVE, false);
        }
        break;
    case MessageBox_Destory:
    case  MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_WIFI_ADAPTER_REMOVE) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_WIFI_ADAPTER_REMOVE, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

//////////////////////////////////// Wi-Fi Connect Over Time //////////////////////////////////////
CWifiConnectOverTimePopboxProxy::CWifiConnectOverTimePopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_WIFI_CONNECTOVERTIME)
{

}

CWifiConnectOverTimePopboxProxy::~CWifiConnectOverTimePopboxProxy()
{

}

CBaseIdlePopupboxProxy * CWifiConnectOverTimePopboxProxy::CreateInstance()
{
    return new CWifiConnectOverTimePopboxProxy();
}

bool CWifiConnectOverTimePopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_strIcon = "";
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_WIFI_OVERTIME);
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_CANCEL);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    return true;
}

bool CWifiConnectOverTimePopboxProxy::ProcessFinishPopupBox(int nReason)
{
    switch (nReason)
    {
    case MessageBox_OK:
        {
            //清空弹出标志
            idleScreen_ClearPopupFlag(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);

            //用户点击OK按钮情况
            if (SettingUI_OpenPage(SubMenuData("OpenSubPage(wifi)"), true))
            {
                CBaseDialog * pBaseDlg = UIManager_GetPrivateDlg("CDlgWifiBase");
                if (NULL != pBaseDlg
                        && pBaseDlg->inherits("CDlgWifiBase"))
                {
                    CDlgWifiBase * pWifiDlg = static_cast<CDlgWifiBase *>(pBaseDlg);
                    if (NULL != pWifiDlg)
                    {
                        pWifiDlg->SetWifiOpenClose(true);
                    }
                }
            }

        }
        break;
    case MessageBox_Cancel:
        {
            idleScreen_ClearPopupFlag(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
        }
        break;
    case MessageBox_Destory:
    case MessageBox_Close:
        {
            if (idleScreen_GetPopupStatus(PS_STATE_WIFI_CONNECTOVERTIME) == POPUP_WINDOW_POPUPING)
            {
                idleScreen_SetPopupStatus(PS_STATE_WIFI_CONNECTOVERTIME, POPUP_WINDOW_POPUP);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

///////////////////////////////CAutopErrorPopboxProxy//////////////////////////////////
CAutopErrorPopboxProxy::CAutopErrorPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION_ERROR)
{

}

CAutopErrorPopboxProxy::~CAutopErrorPopboxProxy()
{

}

bool CAutopErrorPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_iPopupBoxType  = MESSAGEBOX_NOTE;
    m_PopupBoxInfo.nTimes = 2000;
    m_PopupBoxInfo.m_strNote =   LANG_TRANSLATE(TRID_WRONG_USER_NAME_OR_PASSWORD);
    m_PopupBoxInfo.m_strCancelBtnTitle = "";
    m_PopupBoxInfo.m_strConfirmBtnTitle = "";
    m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_NOTE);
    return true;
}

CBaseIdlePopupboxProxy * CAutopErrorPopboxProxy::CreateInstance()
{
    return new CAutopErrorPopboxProxy();
}

bool CAutopErrorPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    //关闭弹窗有3种情况，1.定时时间到，这是最好的。2.被close关闭。3被来电关闭
    //定时时间未到，弹出框就被关闭，是否会异常？
    idleScreen_ClearPopupFlag(PS_STATE_AUTO_PROVISION_ERROR);
    return true;
}

///////////////////////////////CHotelingGuestOutPopboxProxy//////////////////////////////////
CHotelingGuestOutPopboxProxy::CHotelingGuestOutPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION_ERROR)
{

}

CHotelingGuestOutPopboxProxy::~CHotelingGuestOutPopboxProxy()
{

}

bool CHotelingGuestOutPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_iPopupBoxType  = MESSAGEBOX_OkCancel;
    m_PopupBoxInfo.m_strNote =   LANG_TRANSLATE(TRID_WANTT_LOGOUT);
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_CANCEL);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_WARNING);
    return true;
}

CBaseIdlePopupboxProxy * CHotelingGuestOutPopboxProxy::CreateInstance()
{
    return new CHotelingGuestOutPopboxProxy();
}

bool CHotelingGuestOutPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    idleScreen_DropStatus(PS_STATE_HOTELING_GUESTOUT);

    switch (nReason)
    {
    case MessageBox_OK:
        CallCenter_Logout();
        break;
    }

    return true;
}

///////////////////////////////CHotelingUnlockPopboxProxy//////////////////////////////////
CHotelingUnlockPopboxProxy::CHotelingUnlockPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION_ERROR)
{

}

CHotelingUnlockPopboxProxy::~CHotelingUnlockPopboxProxy()
{

}

bool CHotelingUnlockPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_iPopupBoxType  = MESSAGEBOX_GUEST_UNLOCK;
    m_PopupBoxInfo.m_strNote =   LANG_TRANSLATE(TRID_PLEASE_ENTER_UNLOCK_PIN);
    m_PopupBoxInfo.m_strCancelBtnTitle = LANG_TRANSLATE(TRID_CANCEL);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_GUEST_UNLOCK);
    return true;
}

CBaseIdlePopupboxProxy * CHotelingUnlockPopboxProxy::CreateInstance()
{
    return new CHotelingUnlockPopboxProxy();
}

bool CHotelingUnlockPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    idleScreen_DropStatus(PS_STATE_HOTELING_GUESTOUT_CHECK);

    switch (nReason)
    {
    case MessageBox_OK:
        {
            CallCenter_GuestLock(false);
            g_pIdleScreenUI->OnProgramableKeyConfigChange();
        }
        break;
    }

    return true;
}

#if IF_FEATURE_XMLBROWSER_HOTELING
CHotelingLogoutPopboxProxy::CHotelingLogoutPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_HOTELING_LOGOUT_PROMPT)
{
}

CHotelingLogoutPopboxProxy::~CHotelingLogoutPopboxProxy()
{
}

CBaseIdlePopupboxProxy * CHotelingLogoutPopboxProxy::CreateInstance()
{
    return new CHotelingLogoutPopboxProxy();
}

bool CHotelingLogoutPopboxProxy::PreparePopupboxInfo()
{
    return true;
}

bool CHotelingLogoutPopboxProxy::ProcessFinishPopupBox(int iResult)
{
    idleScreen_DropStatus(PS_STATE_HOTELING_LOGOUT_PROMPT);
    switch (iResult)
    {
    case MessageBox_Timeout:
    case MessageBox_OK:
        {
            xmlbrowser_HotelingRequest(false);
        }
        break;
    default:
        break;
    }
    return true;
}

int CHotelingLogoutPopboxProxy::PopupBox()
{
    int iResult = MessageBox_Destory;

    int iTimeout = configParser_GetConfigInt(kszHotelingLogOutPromptDuration);

    if (iTimeout > 0
            && iTimeout <= 86400)
    {
        iTimeout = iTimeout * 1000;
    }
    else
    {
        iTimeout = -1;
    }

    //idleScreen_DropStatus(PS_STATE_HOTELING_LOGOUT_PROMPT);

    //MessageBox_RemoveAllMessageBox();

    MessageBox_ShowCommonMsgBox(&_IdleNoteManager,
                                LANG_TRANSLATE(TRID_TIP_LOGOUT),
                                MESSAGEBOX_OkCancel,
                                iTimeout,
                                "",
                                "",
                                -1, NULL, false, MessageBox_Timeout);

    return iResult;
}
#endif
///////////////////////////////CAtuoPUpdatingPopboxProxy//////////////////////////////////
CAtuoPUpdatingPopboxProxy::CAtuoPUpdatingPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION_UPDATING)
{
}

CAtuoPUpdatingPopboxProxy::~CAtuoPUpdatingPopboxProxy()
{
}

bool CAtuoPUpdatingPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_nMsgBoxId = MSG_ID_AUTOP_HINT;
    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Ok;
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_CONFIG_UPDATEING);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_HIDE);
    return true;
}

CBaseIdlePopupboxProxy * CAtuoPUpdatingPopboxProxy::CreateInstance()
{
    return new CAtuoPUpdatingPopboxProxy();
}

bool CAtuoPUpdatingPopboxProxy::IsUrgentMsgBox(int nMsgBoxId)
{
    if (MSG_ID_AUTOP_HINT == nMsgBoxId)
    {
        return false;
    }

    return true;
}

bool CAtuoPUpdatingPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATING);

    return true;
}

///////////////////////////////CAtuoPUpdateFailedPopboxProxy//////////////////////////////////
CAtuoPUpdateFailedPopboxProxy::CAtuoPUpdateFailedPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION_UPDATE_FAILED)
{
}

CAtuoPUpdateFailedPopboxProxy::~CAtuoPUpdateFailedPopboxProxy()
{
}

bool CAtuoPUpdateFailedPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_nMsgBoxId = MSG_ID_AUTOP_HINT;
    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Ok;
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_CONFIG_UPDATE_FAILED);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    return true;
}

CBaseIdlePopupboxProxy * CAtuoPUpdateFailedPopboxProxy::CreateInstance()
{
    return new CAtuoPUpdateFailedPopboxProxy();
}

bool CAtuoPUpdateFailedPopboxProxy::IsUrgentMsgBox(int nMsgBoxId)
{
    if (MSG_ID_AUTOP_HINT == nMsgBoxId)
    {
        return false;
    }

    return true;
}

bool CAtuoPUpdateFailedPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_FAILED);

    return true;
}

///////////////////////////////CAtuoPUpdateSuccessPopboxProxy//////////////////////////////////
CAtuoPUpdateSuccessPopboxProxy::CAtuoPUpdateSuccessPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS)
{
}

CAtuoPUpdateSuccessPopboxProxy::~CAtuoPUpdateSuccessPopboxProxy()
{
}

bool CAtuoPUpdateSuccessPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_nMsgBoxId = MSG_ID_AUTOP_HINT;
    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Ok;
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_CONFIG_UPDATE_SUCCESS);
    m_PopupBoxInfo.m_strConfirmBtnTitle = LANG_TRANSLATE(TRID_OK);
    return true;
}

CBaseIdlePopupboxProxy * CAtuoPUpdateSuccessPopboxProxy::CreateInstance()
{
    return new CAtuoPUpdateSuccessPopboxProxy();
}

bool CAtuoPUpdateSuccessPopboxProxy::IsUrgentMsgBox(int nMsgBoxId)
{
    if (MSG_ID_AUTOP_HINT == nMsgBoxId)
    {
        return false;
    }

    return true;
}

bool CAtuoPUpdateSuccessPopboxProxy::ProcessFinishPopupBox(int nReason)
{
    idleScreen_DropStatus(PS_STATE_AUTO_PROVISION_UPDATE_SUCCESS);

    return true;
}


///////////////////////////////CBluetoothMobileAskSyncDirectory//////////////////////////////////
CBluetoothMobileAskSyncDirectory::CBluetoothMobileAskSyncDirectory()
    : CBaseIdlePopupboxProxy(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY)
{
}

CBluetoothMobileAskSyncDirectory::~CBluetoothMobileAskSyncDirectory()
{
}

bool CBluetoothMobileAskSyncDirectory::PreparePopupboxInfo()
{
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_WOULD_YOU_LIKE_TO_SYNC_PHONE_CONTACTS_TEMPORARILY);
    return true;
}

CBaseIdlePopupboxProxy * CBluetoothMobileAskSyncDirectory::CreateInstance()
{
    return new CBluetoothMobileAskSyncDirectory();
}

bool CBluetoothMobileAskSyncDirectory::ProcessFinishPopupBox(int nReason)
{
    if (nReason != MessageBox_OK && nReason != MessageBox_Cancel)
    {
        return true;
    }

    idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILE_ASK_SYNC_DIRECTORY);

    bool bSyncDirectory = false;

    if (MessageBox_OK == nReason)
    {
        bSyncDirectory = true;
    }

    QString strPinMac;

    if (CDlgBluetoothBase::GetSyncPinMac(strPinMac))
    {
#ifdef IF_BT_SUPPORT_PHONE
        Bluetooth_SetSyncContact(strPinMac.toUtf8().data(), bSyncDirectory);
#endif
    }

    return true;
}

#if IF_FEATURE_EMERGENCY_HELD
CE911CallBackPopboxProxy::CE911CallBackPopboxProxy()
    : CBaseIdlePopupboxProxy(PS_STATE_E911_ERROR)
{
}

CE911CallBackPopboxProxy::~CE911CallBackPopboxProxy()
{
}

CBaseIdlePopupboxProxy * CE911CallBackPopboxProxy::CreateInstance()
{
    return new CE911CallBackPopboxProxy();
}

bool CE911CallBackPopboxProxy::PreparePopupboxInfo()
{
    m_PopupBoxInfo.ReSet();

    m_PopupBoxInfo.m_iPopupBoxType = MESSAGEBOX_Cancel;

    m_PopupBoxInfo.m_strTitle = LANG_TRANSLATE(TRID_WARNING);
    m_PopupBoxInfo.m_strNote = LANG_TRANSLATE(TRID_E911_LOCATION_FAIL);
    return true;
}

bool CE911CallBackPopboxProxy::ProcessFinishPopupBox(int iResult)
{
    switch (iResult)
    {
    case MessageBox_Cancel:
        {
            idleScreen_DropStatus(PS_STATE_E911_ERROR);
        }
        break;
    default:
        break;
    }
    return true;
}
#endif
