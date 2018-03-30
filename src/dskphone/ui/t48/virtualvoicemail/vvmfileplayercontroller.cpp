#include "baseui/t4xpicpath.h"
#include "vvmfileplayercontroller.h"
#include "vvmfileplayer.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"

#include "messagebox/modmessagebox.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "settingui/include/modsettingui.h"
#include "setting/include/modsetting.h"
#include "record/include/modrecord.h"
#include "include/uicommon.h"
#include "setting/src/voicemailcontroller.h"
#include "lamp/backlight/include/modbacklight.h"
#include "threadtask/include/common.h"

#ifdef _T48
#include "settingui/src/settinguicommon.h"
#include "uicommon/messagebox/selectmessagebox.h"
#endif

#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"

#include "directoryui/include/moddirectoryui.h"
#include "contacts/directory/include/moddirinterface.h"

#include "language/common/common_function.h"
#include "mainwnd/mainwnd.h"

#include "modvoicemailuicommon.h"

namespace
{
#define MSG_ID_DOWNLOAD_LIST                                        29280
#define MSG_ID_DELETE_FOCUS_VOICEMAIL                       29281
#define MSG_ID_DOWNLOAD_DETAIL                                  29282
#define MSG_ID_NEXT_OR_PRE_MSG                                  29283
#define MSG_ID_SHOW_VVM_NOTE_BEFORE_BACK                29284

#define VOICE_MAIL_OPTION_ITEM_COUNT                            8
#define VOICE_MAIL_DETAIL_ITEM_COUNT                            7
#define VOICE_MAIL_PLAY_ITEM_COUNT                              2
#define VOICE_MAIL_PLAY_SOFTKEY_ITEM_COUNT              6
#define VOICE_MAIL_PLAY_BUTTONS_COUNT                       2

static const int s_TimeSecondStrCount(3);
}

CVVMFilePlayerController & CVVMFilePlayerController::GetInstance()
{
    static CVVMFilePlayerController myVVMController;
    return myVVMController;
}

void CVVMFilePlayerController::OnPlayVVMTopWndChange(CBaseDialog * pDlgPreTop,
        CBaseDialog * pDlgNewTop)
{
    g_VVMController.OnTopWndChange();
}

static LRESULT OnVirtualVoiceMailMsg(msgObject & objMessage)
{
    return g_VVMController.OnMyVoiceMailMsg(objMessage);
}

CVVMFilePlayerController::CVVMFilePlayerController(QObject * pParent/* = NULL*/)
    : CFilePlayerController(pParent)
/////////////////////////////
    , m_bIsListDialogExist(false)
    , m_bIsDetailDialogExist(false)
    , m_bIsPlayDialogExist(false)
/////////////////////////////
    , m_bIsLoading(false)
    , m_bListLoaded(false)
    , m_iAccountId(0)
    , m_nCurrentIndex(-1)
/////////////////////////////
#ifdef _T48
    , m_pOptionSelectDelegate(NULL)
#endif
{
    UIManager_RegisterTopWndChangeCallBack(CVVMFilePlayerController::OnPlayVVMTopWndChange);
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnVirtualVoiceMailMsg);

    SetFilePlayer(new CVVMFilePlayer(this));

    connect(m_pFilePlayer, SIGNAL(PlayFinish()), this, SLOT(OnFilePlayFinish()));

#ifdef _T48
    m_pOptionSelectDelegate = new CVVMOptionSelectDelegate(this);
#endif
}

CVVMFilePlayerController::~CVVMFilePlayerController()
{
    UIManager_UnregisterTopWndChangeCallBack(CVVMFilePlayerController::OnPlayVVMTopWndChange);
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnVirtualVoiceMailMsg);

    vvMail_SetAccountId(-1);
    vvMail_ClearListData();
}

void CVVMFilePlayerController::SetListDialogExist(bool bExist)
{
    m_bIsListDialogExist = bExist;

    if (!m_bIsListDialogExist)
    {
        vvMail_CancelAllRequest();

        vvMail_SetAccountId(-1);
        vvMail_ClearListData();

        m_iAccountId = -1;
        m_nCurrentIndex = -1;
        m_bIsLoading = false;
        m_bListLoaded = false;

        backlight_OpenBacklightEvent(true);
    }
}

void CVVMFilePlayerController::SetDetailDialogExist(bool bExist)
{
    m_bIsDetailDialogExist = bExist;
}

void CVVMFilePlayerController::SetPlayDialogExist(bool bExist)
{
    m_bIsPlayDialogExist = bExist;
}

void CVVMFilePlayerController::OnPlayStateChange()
{
    if (FILEPLAYER_STATE_PLAY == GetCurrentPlayState())
    {
        backlight_OpenBacklightEvent(false);
    }
    else
    {
        backlight_OpenBacklightEvent(true);
    }

    emit PlayStateChange();
}

void CVVMFilePlayerController::OnTopWndChange()
{
    //这个需求被去掉
// #ifdef _T48
//  if (m_bIsListDialogExist)
//  {
//      OnPlayerAction(FILEPLAYER_ACTION_STOP);
//  }
// #else
//  if (m_bIsPlayDialogExist)
//  {
//      OnPlayerAction(FILEPLAYER_ACTION_STOP);
//  }
// #endif

    if (talklogic_SessionExist()
            || UIManager_IsInTalk())
    {
        if (m_bIsPlayDialogExist)
        {
            emit DialogPlayExit();
        }

        if (m_bIsListDialogExist)
        {
            emit DialogListExit();
        }
    }
}

LRESULT CVVMFilePlayerController::OnMyVoiceMailMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("CVVMFilePlayerController::OnVoiceMailMsg, %d, %d, %d",
                   objMessage.message, objMessage.wParam, objMessage.lParam);

    if (!m_bIsListDialogExist)
    {
        return 0;
    }

    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_VOICEMESG_LIST_GET == objMessage.wParam)
            {
                SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Get VoiceMail List");

                m_bIsLoading = false;
                m_bListLoaded = true;

                MessageBox_RemoveMessageBox(this, MessageBox_Destory);

                emit DialogUpdate(0);
            }
            else if (XSI_VOICEMESG_DETAIL_GET == objMessage.wParam)
            {
                SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Get VoiceMail Detail");

                MessageBox_RemoveMessageBox(this, MessageBox_Destory);

                LoadPlayDataToPlayer();

                SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Play Start");

#ifdef _T48
                emit DialogRefreshPlayInfo();
                //读取成功后直接开始播放
                OnPlayerAction(FILEPLAYER_ACTION_PLAY);
#else
                EnterPlayDialog();
#endif
            }
            else
            {
                SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Get Other");

                MessageBox_RemoveMessageBox(this, MessageBox_Destory);

                //将某条标记为已读或未读，会受到这种other消息
                emit DialogUpdate(m_nCurrentIndex);
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Get VoiceMail Fail, lParam = %d",
                           objMessage.lParam);

            MessageBox_RemoveMessageBox(this, MessageBox_Destory);
            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();

            if (NULL == pErrInfo)
            {
                return 0;
            }

            int nError = pErrInfo->m_nError;
            if (401 == nError)
            {
                ProcessMsgBoxPassword(m_iAccountId);
            }
            else if (NS_TA::TA_FILE_EXCEED_MAX_SIZE == nError)
            {
                if (m_bIsPlayDialogExist)
                {
                    SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Play Dialog Exist");

                    //如果存在play界面，又去获取新的邮件，说明是在play界面点击前一条或后一条
                    //如果获取失败，直接退出播放界面
                    emit DialogPlayExit();
                }

                ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                         LANG_TRANSLATE(TRID_VOICEMAIL_FILE_TOO_LARGE),
                                         MESSAGEBOX_NOTE, 2000, "", "");
            }
            else if (NS_TA::TA_FILE_OPERATION_NOT_SUPPORTED == nError)
            {
                if (m_bIsPlayDialogExist)
                {
                    SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Play Dialog Exist");

                    //如果存在play界面，又去获取新的邮件，说明是在play界面点击前一条或后一条
                    //如果获取失败，直接退出播放界面
                    emit DialogPlayExit();
                }

                ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                         LANG_TRANSLATE(TRID_VOICEMAIL_NOT_SUPPORT_VIDEO_MESSAGE),
                                         MESSAGEBOX_NOTE, 2000, "", "");
            }
            else
            {
                if (XSI_VOICEMESG_LIST_GET == objMessage.wParam)
                {
                    SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Get List Fail");

                    ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_WARNING), LANG_TRANSLATE(TRID_LOAD_FAILED),
                                             MESSAGEBOX_NOTE, 2000, "", "", MSG_ID_SHOW_VVM_NOTE_BEFORE_BACK);
                }
                else
                {
                    SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Get Detail Fail");

                    MessageBox_RemoveMessageBox(this, MessageBox_Destory);

                    if (m_bIsPlayDialogExist)
                    {
                        SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Play Dialog Exist");

                        //如果存在play界面，又去获取新的邮件，说明是在play界面点击前一条或后一条
                        //如果获取失败，直接退出播放界面
                        emit DialogPlayExit();
                    }
                    else
                    {
                        SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Play Dialog Not Exist");

                        emit DialogUpdate(0);
                    }

                    ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE), LANG_TRANSLATE(TRID_FAILED_PLEASE_RETRY),
                                             MESSAGEBOX_NOTE, 2000, "", "");
                }
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

void CVVMFilePlayerController::SetCurrentAccount(void * pData)
{
    if (NULL != pData)
    {
        SETTINGUI_INFO("CVVMFilePlayerController::SetCurrentAccount, AccountId == pData");
        m_iAccountId = *(int *)pData;
    }
    else
    {
        SETTINGUI_INFO("CVVMFilePlayerController::SetCurrentAccount, AccountId == DefaultAccount");
        m_iAccountId = acc_GetDefaultAccount();
    }

    SETTINGUI_INFO("CVVMFilePlayerController::SetCurrentAccount, m_iAccountId = %d", m_iAccountId);
    vvMail_SetAccountId(m_iAccountId);
}

void CVVMFilePlayerController::OnVVMDialogShow()
{
    SETTINGUI_INFO("CVVMFilePlayerController::OnVVMDialogShow");

    if (m_bListLoaded)
    {
        return;
    }

    m_bIsLoading = true;
    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (!ProcessMsgBoxPassword(m_iAccountId))
        {
            return;
        }

        vvMail_ReqVoicemessageList();
    }
    else
    {
        m_bIsLoading = false;
    }

    if (m_bIsLoading)
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE)
                                 , LANG_TRANSLATE(TRID_VOICEMAIL_DOWNLOADING), MESSAGEBOX_Cancel, -1, "", "", MSG_ID_DOWNLOAD_LIST);
    }
    else
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_WARNING),
                                 LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                 MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_SHOW_VVM_NOTE_BEFORE_BACK);
    }
}

void CVVMFilePlayerController::EnterPlayDialog()
{
    if (m_bIsPlayDialogExist)
    {
        SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Play Dialog Exist");

        //如果存在play界面，又去获取新的邮件，说明是在play界面点击前一条或后一条
        emit DialogRefreshPlayInfo();
        emit DialogRefreshSoftkey();
        //读取成功后直接开始播放
        OnPlayerAction(FILEPLAYER_ACTION_PLAY);
    }
    else if (m_bIsDetailDialogExist)
    {
        SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, Detail Dialog Exist");

        //如果存在详细信息界面，则要从详细信息界面打开
        emit DetailDialogOpenPage(VVM_DIALOG_TYPE_PLAY);
    }
    else
    {
        SETTINGUI_INFO("CVVMFilePlayerController::OnMyVoiceMailMsg, List Exist");

        //不存在播放界面说明是从列表界面进入
        emit ListDialogOpenPage(VVM_DIALOG_TYPE_PLAY);
    }
}

void CVVMFilePlayerController::OnFilePlayFinish()
{
    //收到播放完成的信号，才能将当前的标记为已读
    if (m_nCurrentIndex >= 0)
    {
        vvMail_ReqMarkByIndex(m_nCurrentIndex, true);
        _MainWND.SetTimerLock(false);
        //  DialogUpdate(m_nCurrentIndex);
        emit FilePlayFinish();
    }
}

bool CVVMFilePlayerController::OnPlayerAction(QString strAction)
{
    return OnPlayerAction(strAction, m_nCurrentIndex);
}

bool CVVMFilePlayerController::OnPlayerAction(QString strAction, QVariant var)
{
    SETTINGUI_INFO("CVVMFilePlayerController::OnPlayerAction, strAction = %s, CurrentIndex = %d",
                   strAction.toUtf8().data(), m_nCurrentIndex);

    bool bHandled = true;

    if (FILEPLAYER_ACTION_PLAY == strAction
            || FILEPLAYER_ACTION_PAUSE == strAction
            || FILEPLAYER_ACTION_RESUME == strAction)
    {
        _MainWND.SetTimerLock(true);
    }
    else
    {
        _MainWND.SetTimerLock(false);
    }

    if (FILEPLAYER_ACTION_DELETE == strAction)
    {
        m_nCurrentIndex = var.value<int>();

        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE)
                                 , LANG_TRANSLATE(TRID_VOICEMAIL_DELETE_VOICE_MAIL), MESSAGEBOX_OkCancel, -1, "", "",
                                 MSG_ID_DELETE_FOCUS_VOICEMAIL);
    }
    else if (FILEPLAYER_ACTION_ENTER_PLAY == strAction)
    {
        OnPlayerAction(FILEPLAYER_ACTION_STOP, m_nCurrentIndex);
        emit FIlePlayStart();

        m_nCurrentIndex = var.value<int>();

        if (m_nCurrentIndex >= 0)
        {
            vvMail_ReqPlayContentByIndex(m_nCurrentIndex);

            ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                     LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                     MESSAGEBOX_Cancel, -1, "", "", MSG_ID_DOWNLOAD_DETAIL);
        }
    }
    else if (FILEPLAYER_ACTION_OPTION == strAction)
    {
        m_nCurrentIndex = var.value<int>();

#ifdef _T48
        if (NULL != m_pOptionSelectDelegate)
        {
            m_pOptionSelectDelegate->InitOptionData();

            QString strTitle = LANG_TRANSLATE(TRID_OPTION);

            VecMsgBoxSoftkey vecSoftKey;
            vecSoftKey.push_back(MsgBoxSoftkey(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel));
            MessageBox_ShowSelectBox(m_pOptionSelectDelegate, vecSoftKey, SELECT_POP_BOX_TYPE_NORMAL,
                                     m_pOptionSelectDelegate, strTitle);
        }
#else
        emit ListDialogOpenPage(VVM_DIALOG_TYPE_OPTION);
#endif
    }
    else if (FILEPLAYER_ACTION_DETAIL == strAction)
    {
        emit ListDialogOpenPage(VVM_DIALOG_TYPE_DETAIL);
    }
    else if (FILEPLAYER_ACTION_MARK_AS_READ == strAction)
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                 MESSAGEBOX_Cancel, -1, "", "", MSG_ID_DOWNLOAD_DETAIL);

        if (m_nCurrentIndex >= 0)
        {
            vvMail_ReqMarkByIndex(m_nCurrentIndex, true);
        }
    }
    else if (FILEPLAYER_ACTION_MARK_AS_UNREAD == strAction)
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                 MESSAGEBOX_Cancel, -1, "", "", MSG_ID_DOWNLOAD_DETAIL);

        if (m_nCurrentIndex >= 0)
        {
            vvMail_ReqMarkByIndex(m_nCurrentIndex, false);
        }
    }
    else if (FILEPLAYER_ACTION_ADDTO_CONTACT == strAction)
    {
        if (m_nCurrentIndex >= 0)
        {
            sVoiceMailData refData;
            vvMail_GetDetailByIndex(refData, m_nCurrentIndex);

            DM_EnterAddPersonalContact(refData.m_strName, refData.m_strNumber);
        }
    }
    else if (FILEPLAYER_ACTION_ADDTO_NET_CONTACT == strAction)
    {

    }
    else if (FILEPLAYER_ACTION_CALL_BACK == strAction)
    {
        OnPlayerAction(FILEPLAYER_ACTION_STOP, m_nCurrentIndex);

        ProcessCallBackKey();
    }
    else if (FILEPLAYER_ACTION_CALL_VOICEMAIL == strAction)
    {
        OnPlayerAction(FILEPLAYER_ACTION_STOP, m_nCurrentIndex);

        //连接语音邮箱
        VoiceMailCallOut(m_iAccountId);
    }
    else if (FILEPLAYER_ACTION_MARK_ALL_AS_READ == strAction)
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                 MESSAGEBOX_Cancel, -1, "", "", MSG_ID_DOWNLOAD_DETAIL);

        vvMail_ReqMarkAllRead();
    }
    else if (FILEPLAYER_ACTION_MARK_ALL_AS_UNREAD == strAction)
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                 MESSAGEBOX_Cancel, -1, "", "", MSG_ID_DOWNLOAD_DETAIL);

        vvMail_ReqMarkAllUnRead();
    }
    else if (FILEPLAYER_ACTION_SWITCH_NEXT == strAction)
    {
        OnFilePlayFinish();
        OnPlayerAction(FILEPLAYER_ACTION_STOP);

        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                 MESSAGEBOX_Cancel, -1, "", "", MSG_ID_NEXT_OR_PRE_MSG);

        VoiceMailListData voiceMailListData;
        vvMail_GetVoiceMailListData(&voiceMailListData);

        if (m_nCurrentIndex + 1 < voiceMailListData.m_nTotalCount)
        {
            ++m_nCurrentIndex;
            vvMail_ReqPlayContentByIndex(m_nCurrentIndex);
        }
    }
    else if (FILEPLAYER_ACTION_SWITCH_PRE == strAction)
    {
        OnFilePlayFinish();
        OnPlayerAction(FILEPLAYER_ACTION_STOP);

        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                 MESSAGEBOX_Cancel, -1, "", "", MSG_ID_NEXT_OR_PRE_MSG);

        if (m_nCurrentIndex > 0)
        {
            --m_nCurrentIndex;
            vvMail_ReqPlayContentByIndex(m_nCurrentIndex);
        }
    }
    else
    {
        if (FILEPLAYER_ACTION_PLAY == strAction
                || FILEPLAYER_ACTION_PAUSE == strAction
                || FILEPLAYER_ACTION_RESUME == strAction)
        {
            bHandled = CFilePlayerController::OnPlayerAction(strAction, var);
            if (bHandled)
            {
                emit DialogRefreshSoftkey();
            }
        }
        else if (FILEPLAYER_ACTION_STOP == strAction)
        {
            bHandled = CFilePlayerController::OnPlayerAction(strAction, var);
            if (bHandled)
            {
                vvMail_ReqMarkByIndex(m_nCurrentIndex, true);
                emit FIlePlayStop();
            }
        }
        else if (FILEPLAYER_ACTION_STOPONLY == strAction)
        {
            CFilePlayerController::OnPlayerAction(strAction, var);
        }
        else
        {
            bHandled = CFilePlayerController::OnPlayerAction(strAction, var);
        }
    }

    return bHandled;
}

bool CVVMFilePlayerController::ProcessCallBackKey()
{
    SETTINGUI_INFO("CVVMFilePlayerController::ProcessCallBackKey");

    bool bResult = false;
    sVoiceMailData refData;
    if (m_nCurrentIndex >= 0)
    {
        vvMail_GetDetailByIndex(refData, m_nCurrentIndex);

        yl::string strNumber = refData.m_strNumber;
        yl::string strName = refData.m_strName;

        bResult = talklogic_CallOut(strNumber.c_str(), strName.c_str(), m_iAccountId);
    }
    else
    {
        bResult = false;
    }

    return bResult;
}

bool CVVMFilePlayerController::ProcessMsgBoxPassword(int nLineId)
{
    SETTINGUI_INFO("CVVMFilePlayerController::ProcessMsgBoxPassword, nLineId = %d", nLineId);

    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        ShowVirtualVoiceMailNote(this, LANG_TRANSLATE(TRID_NOTE),
                                 LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                 MESSAGEBOX_NOTE, 1500, "", "",
                                 MSG_ID_SHOW_VVM_NOTE_BEFORE_BACK);

        return false;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SETTINGUI_INFO("CVVMFilePlayerController::ProcessMsgBoxPassword, XSI_AUTH_ERROR");
#ifdef _T48
        SettingUI_PopPasswordDlg(this, nLineId);
#else
        yl::string  strSubmitPassword = SLOT(OnSubmitXSIPassword(const yl::string &));
        yl::string  strCancel = SLOT(OnCancelInputXSIPassword());
        SettingUI_PopPasswordDlg(this, strSubmitPassword.c_str(), strCancel.c_str(), nLineId);
#endif
        return false;
    }
    else
    {
        SETTINGUI_INFO("CVVMFilePlayerController::ProcessMsgBoxPassword, xsi status [%d]",
                       CBaseHelper::GetXSIStatus(nLineId));
        return true;
    }
}

void CVVMFilePlayerController::OnSubmitXSIPassword(const yl::string & strPswd)
{
    SETTINGUI_INFO("CVVMFilePlayerController::OnSubmitXSIPassword, Account = %d", m_iAccountId);
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);

#ifdef _T48
    //http://10.2.1.199/Bug.php?BugID=86795
    //T46会发生界面回退，能自行进行重新加载，但是T48是弹框，所以需要手动重新加载
    m_bListLoaded = false;
    OnVVMDialogShow();
#endif
}

void CVVMFilePlayerController::OnCancelInputXSIPassword()
{
    SETTINGUI_INFO("CVVMFilePlayerController::OnCancelInputXSIPassword");
    emit DialogListExit();
}

void CVVMFilePlayerController::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    SETTINGUI_INFO("CVVMFilePlayerController::MessageBoxCallBack");

    int iMsgID = pMessageBox->GetID();
    MESSAGEBOX_RESULT eResault = pMessageBox->GetResult();

    switch (iMsgID)
    {
#ifdef _T48
    case MSGBOX_ID_POP_XSI_PWD:
        {
            if (MessageBox_OK == eResault)
            {
                QString strPwd = pMessageBox->GetExtraData().value<QString>();
                OnSubmitXSIPassword(strPwd.toUtf8().data());
            }
            else if (MessageBox_Cancel == eResault)
            {
                OnCancelInputXSIPassword();
            }
        }
        break;
#endif
    case MSG_ID_DELETE_FOCUS_VOICEMAIL:
        {
            if (MessageBox_OK == eResault)
            {
                OnPlayerAction(FILEPLAYER_ACTION_STOPONLY);

                VoiceMailListData voiceMailListData;
                vvMail_GetVoiceMailListData(&voiceMailListData);

                if (m_nCurrentIndex < 0 || !vvMail_ReqDeleteMessageByIndex(m_nCurrentIndex))
                {
                    return;
                }

                if (1 == voiceMailListData.m_nTotalCount)
                {
                    emit DialogUpdate(0);

                    if (m_bIsPlayDialogExist)
                    {
                        emit DialogPlayExit();
                    }
                }
                else
                {
                    int iLastIndex = voiceMailListData.m_nTotalCount - 1;
                    //删除的是最后一条则焦点上移
                    if (m_nCurrentIndex == iLastIndex)
                    {
                        --m_nCurrentIndex;
                        emit DialogUpdate(m_nCurrentIndex);
                    }
                    else
                    {
                        emit DialogUpdate(m_nCurrentIndex);
                    }

#ifdef _T48
//              OnPlayerAction(FILEPLAYER_ACTION_STOP);
#else
                    if (m_bIsPlayDialogExist)
                    {
//                  //重新请求新的邮件
//                  OnPlayerAction(FILEPLAYER_ACTION_ENTER_PLAY);
//
//                  emit DialogRefreshPlayInfo();
//                  emit DialogRefreshSoftkey();

                        //根据需求，删除后回退，不直接播放下一条
                        emit DialogPlayExit();
                    }
#endif
                }
            }
        }
        break;
    case MSG_ID_DOWNLOAD_LIST:
        {
            if (MessageBox_Cancel == eResault)
            {
                vvMail_CancelAllRequest();
                emit DialogListExit();
            }
        }
        break;
    case MSG_ID_DOWNLOAD_DETAIL:
        {
            if (MessageBox_Cancel == eResault)
            {
                vvMail_CancelAllRequest();
                if (m_bIsPlayDialogExist)
                {
                    emit DialogPlayExit();
                }
            }
        }
        break;
    case MSG_ID_NEXT_OR_PRE_MSG:
        {
            if (MessageBox_Cancel == eResault)
            {
                vvMail_CancelAllRequest();
                if (m_bIsPlayDialogExist)
                {
                    emit DialogPlayExit();
                }
            }
        }
        break;
    case MSG_ID_SHOW_VVM_NOTE_BEFORE_BACK:
        {
            emit DialogPlayExit();
            emit DialogListExit();
        }
        break;
    default:
        break;
    }
}

void CVVMFilePlayerController::InitOptionData()
{
    m_vecOptionData.clear();
    m_vecOptionData.reserve(VOICE_MAIL_OPTION_ITEM_COUNT);
}

const vecVVMItemPair & CVVMFilePlayerController::GetOptionData(bool & bResult)
{
    InitOptionData();

    if (m_nCurrentIndex < 0)
    {
        bResult = false;
        return m_vecOptionData;
    }

    sVoiceMailData refData;
    vvMail_GetDetailByIndex(refData, m_nCurrentIndex);

    VVM_ITEM_PAIR vvm_ItemPair;
    vvm_ItemPair.m_strTitle2 = "";

#ifdef _T48
    //
#else
    vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_DETAIL;
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_DETAIL);
    m_vecOptionData.push_back(vvm_ItemPair);
#endif

    //匿名Voicemail无号码无法回拨、添加到联系人
    if (!VoiceMailUI_IsPrivacyVoiceMail(m_nCurrentIndex))
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_CALL_BACK;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_CALL_BACK);
        m_vecOptionData.push_back(vvm_ItemPair);

        SmartPtr pDetail = Dir_GetDetailById(kszPersonalId, BSFT_DIR);
        if (!pDetail.IsEmpty())
        {
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_ADDTO_CONTACT;
            vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_ADD_TO_PERSONAL);
            m_vecOptionData.push_back(vvm_ItemPair);
        }
    }

    //无该需求
//  vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_ADDTO_NET_CONTACT;
//  vvm_ItemPair.m_strTitle1 = "LANG_TRANSLATE(TRID_ADD_TO_MTSW_CONTACT)";
//  m_vecOptionData.push_back(vvm_ItemPair);

    if (refData.m_bRead)
    {
        //已读的可以标志为未读
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_MARK_AS_UNREAD;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_MARK_NEW);
        m_vecOptionData.push_back(vvm_ItemPair);
    }
    else
    {
        //未读的可以标志为已读
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_MARK_AS_READ;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_MARK_HEARD);
        m_vecOptionData.push_back(vvm_ItemPair);
    }

    vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_MARK_ALL_AS_UNREAD;
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_MARK_ALL_NEW);
    m_vecOptionData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_MARK_ALL_AS_READ;
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_MARK_ALL_HEARD);
    m_vecOptionData.push_back(vvm_ItemPair);

#ifdef _T48
    vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_DELETE;
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_DELETE);
    m_vecOptionData.push_back(vvm_ItemPair);
#else
    //
#endif

    bResult = true;
    return m_vecOptionData;
}

void CVVMFilePlayerController::InitDetailData()
{
    m_vecDetailData.clear();
    m_vecDetailData.reserve(VOICE_MAIL_DETAIL_ITEM_COUNT);
}

const vecVVMItemPair & CVVMFilePlayerController::GetDetailData(bool & bResult)
{
    InitDetailData();

    if (m_nCurrentIndex < 0)
    {
        bResult = false;
        return m_vecDetailData;
    }

    sVoiceMailData refData;
    vvMail_GetDetailByIndex(refData, m_nCurrentIndex);

    VVM_ITEM_PAIR vvm_ItemPair;
    vvm_ItemPair.m_strAction = "";
    vvm_ItemPair.m_strTitle2 = "";

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_NAME);
    if (refData.m_bPrivacy)
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_ANONYMOUS);
    }
    else
    {
        vvm_ItemPair.m_strTitle2 = toQString(refData.m_strName);
    }
    m_vecDetailData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_NUMBER);
    if (refData.m_bPrivacy)
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_ANONYMOUS);
    }
    else
    {
        vvm_ItemPair.m_strTitle2 = toQString(refData.m_strName);
    }
    m_vecDetailData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_TIME);
    yl::string strTime = refData.m_strTime.substr(0, 10);
    time_t tTime = atoi(strTime.c_str());
    strTime = Timet2DetailString(tTime);
    vvm_ItemPair.m_strTitle2 = toQString(strTime);
    m_vecDetailData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_LINE);
    vvm_ItemPair.m_strTitle2 = toQString(acc_GetAccountShowText(refData.m_nLine));
    m_vecDetailData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_DURATION);
    vvm_ItemPair.m_strTitle2 = toQString(modRecord_TransTimeToString(refData.m_nDuration /
                                         FILE_PLAYER_ONE_SECOND + 1));
    m_vecDetailData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_STATUS);
    if (refData.m_bRead)
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_VOICEMAIL_READ);
    }
    else
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_VOICEMAIL_UNREAD);
    }
    m_vecDetailData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_MEDIA_TYPE);
    if (MEDIA_audio == refData.m_eMediaType)
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_VOICEMAIL_TYPE_VOICE);
    }
    else// if (MEDIA_video == refData.m_eMediaType)
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_VOICEMAIL_TYPE_VIDEO);
    }
    m_vecDetailData.push_back(vvm_ItemPair);

    bResult = true;
    return m_vecDetailData;
}

void CVVMFilePlayerController::JudgeAndCutTime(const time_t & tTime, QString & strTime1,
        QString & strTime2)
{
    static const long klSecPerDay = 24 * 60 * 60;
    time_t tCurrent = netGetLocalTime();
    time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    time_t tTemp = tTime;
    tm * ptmTransit = localtime(&tTemp);
    if (ptmTransit == NULL)
    {
        SETTING_WARN("Setting_CallLogTime2String tmLocal == NULL");
        return;
    }
    //用临时变量解决localtime()函数的缓存区问题
    tm objtmTransit = *ptmTransit;
    tm * tmLocal = &objtmTransit;

    //判断是显示 “今天、昨天、星期”还是显示日期
    //bWithTime表示是 “今天 + 时间”的格式，否则是日期+时间的格式
    bool bWithTime = false;
    yl::string strRet;

    if (tTime >= tTodayZero
            && tTime < tTomorrowZero)
    {
        // 如果是今天, 则显示为Today.
        strRet = LANG_TRANSLATE(TRID_TODAY).toUtf8().data();
        bWithTime = true;
    }
    else if (tTime >= tTodayZero - klSecPerDay
             && tTime < tTodayZero)
    {
        // 如果是昨天
        strRet = LANG_TRANSLATE(TRID_YESTERDAY).toUtf8().data();
        bWithTime = true;
    }
    else if (tTime >= tSixDayBeforeZero
             && tTime < tTodayZero - klSecPerDay)
    {
        // 如果是7天前的0点到昨天23:59之间, 则直接显示星期.
        strRet = LANG_TRANSLATE(GetWeedayFull(tmLocal->tm_wday)).toUtf8().data();
        bWithTime = true;
    }
    else
    {
        // 其他情况都显示年月日.
        bWithTime = false;
    }

    //判断时间字串长度
    int nTimeSize = 0;
    TimeFormatType timeformat = (TimeFormatType)Setting_GetCurTimeFormatType();
    if (TIME_FORMAT_12HOUR == timeformat)
    {
        nTimeSize = 11;
    }
    else if (TIME_FORMAT_24HOUR == timeformat)
    {
        nTimeSize = 8;
    }

    yl::string strTime = Timet2DetailString(tTime);
    int nSize = strTime.size();

    if (bWithTime)
    {
        strTime1 = toQString(strRet);
    }
    else
    {
        strTime1 = toQString(strTime.substr(0, nSize - nTimeSize));
    }

    if (TIME_FORMAT_12HOUR == timeformat)
    {
        strTime2 = toQString(strTime.substr(nSize - nTimeSize,
                                            nTimeSize - s_TimeSecondStrCount - s_TimeSecondStrCount));
        strTime2 += toQString(strTime.substr(nSize - s_TimeSecondStrCount, s_TimeSecondStrCount));
    }
    else if (TIME_FORMAT_24HOUR == timeformat)
    {
        strTime2 = toQString(strTime.substr(nSize - nTimeSize, nTimeSize - s_TimeSecondStrCount));
    }
}

void CVVMFilePlayerController::InitPlayInfo()
{
    m_vecPlayData.clear();
    m_vecPlayData.reserve(VOICE_MAIL_PLAY_ITEM_COUNT);
}

const vecVVMItemPair & CVVMFilePlayerController::GetPlayInfo(bool & bResult, int nIndex/* = -1*/)
{
    InitPlayInfo();

    sVoiceMailData refData;
    if (-1 == nIndex)
    {
        nIndex = m_nCurrentIndex;
    }

    if (nIndex < 0)
    {
        bResult = false;
        return m_vecPlayData;
    }

    vvMail_GetDetailByIndex(refData, nIndex);

    VVM_ITEM_PAIR vvm_ItemPair;
    vvm_ItemPair.m_strAction = "";
    vvm_ItemPair.m_strTitle1 = "";
    vvm_ItemPair.m_strTitle2 = "";

#ifdef _T48
    if (refData.m_bRead)
    {
        vvm_ItemPair.m_strAction = VVM_FILE_READ;
    }
    else
    {
        vvm_ItemPair.m_strAction = VVM_FILE_UNREAD;
    }

    if (refData.m_bUrgent)
    {
        vvm_ItemPair.m_strTitle1 = "!";
    }
    if (refData.m_bConfidential)
    {
        vvm_ItemPair.m_strTitle2 = "*";
    }
    m_vecPlayData.push_back(vvm_ItemPair);

    if (refData.m_bPrivacy)
    {
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_ANONYMOUS);
    }
    else
    {
        yl::string strNameNumber = refData.m_strName + " - " + refData.m_strNumber;
        vvm_ItemPair.m_strTitle1 = toQString(strNameNumber);
    }
    vvm_ItemPair.m_strTitle2 = toQString(modRecord_TransTimeToString(refData.m_nDuration /
                                         FILE_PLAYER_ONE_SECOND + 1));
    m_vecPlayData.push_back(vvm_ItemPair);

    yl::string strTime = refData.m_strTime.substr(0, 10);
    time_t tTime = atoi(strTime.c_str());

    JudgeAndCutTime(tTime, vvm_ItemPair.m_strTitle1, vvm_ItemPair.m_strTitle2);
    m_vecPlayData.push_back(vvm_ItemPair);
#else
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_FROM);
    if (refData.m_bPrivacy)
    {
        vvm_ItemPair.m_strTitle2 = LANG_TRANSLATE(TRID_ANONYMOUS);
    }
    else
    {
        yl::string strNameNumber = refData.m_strName + " - " + refData.m_strNumber;
        vvm_ItemPair.m_strTitle2 = toQString(strNameNumber);
    }
    m_vecPlayData.push_back(vvm_ItemPair);

    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_TIME);

    yl::string strTime = refData.m_strTime.substr(0, 10);
    time_t tTime = atoi(strTime.c_str());
    strTime = Timet2DetailString(tTime);

    vvm_ItemPair.m_strTitle2 = toQString(strTime);
    m_vecPlayData.push_back(vvm_ItemPair);
#endif

    bResult = true;
    return m_vecPlayData;
}

void CVVMFilePlayerController::InitPlaySoftkey()
{
    m_vecPlaySoftkey.clear();
    m_vecPlaySoftkey.reserve(VOICE_MAIL_PLAY_SOFTKEY_ITEM_COUNT);
}

vecVVMItemPair CVVMFilePlayerController::GetPlaySoftkey(bool & bResult)
{
    if (NULL == m_pFilePlayer)
    {
        SETTINGUI_INFO("CVVMFilePlayerController::GetPlaySoftkey Size Fail");
        bResult = false;
        static vecVVMItemPair tmpVecPlaySoftkey;
        return tmpVecPlaySoftkey;
    }

    InitPlaySoftkey();

    VVM_ITEM_PAIR vvm_ItemPair;
    vvm_ItemPair.m_strTitle2 = "";

    if (!VoiceMailUI_IsPrivacyVoiceMail(m_nCurrentIndex))
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_CALL_BACK;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_CALL_BACK);
        m_vecPlaySoftkey.push_back(vvm_ItemPair);
    }

    vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_DELETE;
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_DELETE);
    m_vecPlaySoftkey.push_back(vvm_ItemPair);

    FILEPLAYER_STATE ePlayState = GetCurrentPlayState();

    if (FILEPLAYER_STATE_PLAY == ePlayState)
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_PAUSE;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_PAUSE);
        m_vecPlaySoftkey.push_back(vvm_ItemPair);
    }
    else if (FILEPLAYER_STATE_PAUSE == ePlayState)
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_RESUME;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_RESUME);
        m_vecPlaySoftkey.push_back(vvm_ItemPair);
    }
    else //if (FILEPLAYER_STATE_STOP == ePlayState)
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_PLAY;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_PLAY);
        m_vecPlaySoftkey.push_back(vvm_ItemPair);
    }

    //与黑白屏一致，将快进、快退功能放到左右键中
//  vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_SWIFT_NEXT_START;
//  vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_SWITCH_FORWARD);
//  m_vecPlaySoftkey.push_back(vvm_ItemPair);
//
//  vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_SWIFT_PRE_START;
//  vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_SWITCH_REWIND);
//  m_vecPlaySoftkey.push_back(vvm_ItemPair);

    VoiceMailListData voiceMailListData;
    vvMail_GetVoiceMailListData(&voiceMailListData);

    int iLastIndex = voiceMailListData.m_nTotalCount - 1;
    //如果是第一条，则没有“前一个”
    if (m_nCurrentIndex > 0)
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_SWITCH_PRE;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_PRE);
        m_vecPlaySoftkey.push_back(vvm_ItemPair);
    }

    //如果是最后一条，则没有“后一个”
    if (m_nCurrentIndex >= 0 && m_nCurrentIndex < iLastIndex)
    {
        vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_SWITCH_NEXT;
        vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_VOICEMAIL_NEXT);
        m_vecPlaySoftkey.push_back(vvm_ItemPair);
    }

    vvm_ItemPair.m_strAction = ACTION_QUIT;
    vvm_ItemPair.m_strTitle1 = LANG_TRANSLATE(TRID_BACK);
    m_vecPlaySoftkey.push_back(vvm_ItemPair);

    bResult = true;
    return m_vecPlaySoftkey;
}

void CVVMFilePlayerController::InitPlayButtons()
{
    m_vecPlaySoftkey.clear();
    m_vecPlaySoftkey.reserve(VOICE_MAIL_PLAY_BUTTONS_COUNT);
}

vecVVMItemPair CVVMFilePlayerController::GetPlayButtons(bool & bResult, int nDataIndex)
{
    InitPlayButtons();

    VVM_ITEM_PAIR vvm_ItemPair;
    vvm_ItemPair.m_strAction = "";
    vvm_ItemPair.m_strTitle1 = "";
    vvm_ItemPair.m_strTitle2 = "";

#ifdef _T48
    FILEPLAYER_STATE ePlayerState = GetCurrentPlayState();

    sVoiceMailData refData;
    vvMail_GetDetailByIndex(refData, nDataIndex);
    QString strButton = PIC_USB_BROWSE_ITEM_PLAY;
    QString strButtonPress = PIC_USB_BROWSE_ITEM_PLAY_PRESS;
    if (MEDIA_video == refData.m_eMediaType)
    {
        strButton = PIC_USB_BROWSE_ITEM_VIDEO_PLAY;
        strButtonPress = PIC_USB_BROWSE_ITEM_VIDEO_PLAY_PRESS;
    }

    //相等说明当前item是正在播放的item
    if (nDataIndex == m_nCurrentIndex)
    {
        //如果是正在播放
        if (FILEPLAYER_STATE_PLAY == ePlayerState)
        {
            //左侧：显示暂停图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_PAUSE;
            vvm_ItemPair.m_strTitle1 = PIC_USB_BROWSE_ITEM_PAUSE;
            vvm_ItemPair.m_strTitle2 = PIC_USB_BROWSE_ITEM_PAUSE_PRESS;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
            //右侧：显示停止图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_STOP;
            vvm_ItemPair.m_strTitle1 = PIC_USB_BROWSE_ITEM_STOP;
            vvm_ItemPair.m_strTitle2 = PIC_USB_BROWSE_ITEM_STOP_PRESS;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
        }
        //如果在暂停中
        else if (FILEPLAYER_STATE_PAUSE == ePlayerState)
        {
            //左侧：显示恢复（播放）图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_RESUME;
            vvm_ItemPair.m_strTitle1 = strButton;
            vvm_ItemPair.m_strTitle2 = strButtonPress;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
            //右侧：显示停止图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_STOP;
            vvm_ItemPair.m_strTitle1 = PIC_USB_BROWSE_ITEM_STOP;
            vvm_ItemPair.m_strTitle2 = PIC_USB_BROWSE_ITEM_STOP_PRESS;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
        }
        //否则处于停止状态
        else
        {
            //左侧：显示播放图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_ENTER_PLAY;
            vvm_ItemPair.m_strTitle1 = strButton;
            vvm_ItemPair.m_strTitle2 = strButtonPress;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
            //右侧：显示选项图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_OPTION;
            vvm_ItemPair.m_strTitle1 = PIC_BLUE_MORE;
            vvm_ItemPair.m_strTitle2 = PIC_BLUE_MORE_PRESS;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
        }
    }
    //如果不等说明该item不是当前播放的item
    else
    {
        //其他item处于暂停或者播放状态
        if (FILEPLAYER_STATE_PLAY == ePlayerState
                || FILEPLAYER_STATE_PAUSE == ePlayerState)
        {
            //左侧：显示播放图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_ENTER_PLAY;
            vvm_ItemPair.m_strTitle1 = strButton;
            vvm_ItemPair.m_strTitle2 = strButtonPress;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
            //右侧：不显示图标
            vvm_ItemPair.m_strAction = "";
            vvm_ItemPair.m_strTitle1 = "";
            vvm_ItemPair.m_strTitle2 = "";
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
        }
        //其他item处于停止状态
        else
        {
            //左侧：显示播放图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_ENTER_PLAY;
            vvm_ItemPair.m_strTitle1 = strButton;
            vvm_ItemPair.m_strTitle2 = strButtonPress;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
            //右侧：显示选项图标
            vvm_ItemPair.m_strAction = FILEPLAYER_ACTION_OPTION;
            vvm_ItemPair.m_strTitle1 = PIC_BLUE_MORE;
            vvm_ItemPair.m_strTitle2 = PIC_BLUE_MORE_PRESS;
            m_vecPlaySoftkey.push_back(vvm_ItemPair);
        }
    }
#endif

    bResult = true;
    return m_vecPlaySoftkey;
}

void CVVMFilePlayerController::LoadPlayDataToPlayer()
{
    if (m_nCurrentIndex < 0)
    {
        return;
    }

    sVoiceMailData refData;
    vvMail_GetDetailByIndex(refData, m_nCurrentIndex);

    FilePlayerData tmpPlayData;
    tmpPlayData.m_eFilePlayerType = FILEPLAYERTYPE_INDEX;
    tmpPlayData.m_nFileIndex = m_nCurrentIndex;
    tmpPlayData.m_lTotalSeconds = (refData.m_nDuration / FILE_PLAYER_ONE_SECOND) + 1;
    tmpPlayData.m_lCurrentSeconds = FILE_PLAYER_ZERO_SECOND;
    tmpPlayData.m_nNextTime = FILE_PLAYER_DEFAULT_SWIFT_NEXT_TIME / FILE_PLAYER_ONE_SECOND;
    tmpPlayData.m_nPreTime = FILE_PLAYER_DEFAULT_SWIFT_PRE_TIME / FILE_PLAYER_ONE_SECOND;
    LoadFilePlayData(tmpPlayData);
}

void CVVMFilePlayerController::ShowVirtualVoiceMailNote(CMsgBoxCallBackAction * pAction
        , const QString & strTitle
        , const QString & strNote
        , MESSAGEBOX_TYPE eType
        , int nMsTime /*= -1*/
        , const QString & strCancelBtnTitle /*= ""*/
        , const QString & strConfirmBtnTitle /*= ""*/
        , int iID /*= -1*/
        , const QVariant & extraData /*= QVariant()*/)
{
#ifdef _T48
    MessageBox_ShowCommonMsgBox(pAction, strNote, eType, nMsTime,
                                strCancelBtnTitle, strConfirmBtnTitle, iID, extraData);
#else
    MessageBox_ShowCommonMsgBox(pAction, strTitle, strNote, eType, nMsTime,
                                strCancelBtnTitle, strConfirmBtnTitle, iID, extraData);
#endif
}

bool CVVMFilePlayerController::VoiceMailCallOut(int nAccountId)
{
    VoiceMailInfoListData listData;
//  //账号是否可用
//  bool bIsUsed = false;
    //语音信箱号码是否设置
    bool bIsSetVoiceMailNumber = false;
    if (g_pVoiceMailController->GetInfoList(&listData))
    {
        YLList<VoiceMailInfoData>::iterator it = listData.m_listInfo.begin();
        YLList<VoiceMailInfoData>::iterator it_end = listData.m_listInfo.end();
        for (int i = 0; it != it_end; i++, ++it)
        {
            if (nAccountId == i)
            {
                VoiceMailInfoData & infoData = *it;
                if (infoData.m_strMailNumber == "")
                {
                    bIsSetVoiceMailNumber = false;
                }
                else
                {
                    bIsSetVoiceMailNumber = true;
                }
                break;
            }
        }
    }

    VoiceMailItemClicked(nAccountId, bIsSetVoiceMailNumber);
    return true;
}

void CVVMFilePlayerController::VoiceMailItemClicked(int nAccountId, bool bIsSetVoiceMailNumber)
{
    if (bIsSetVoiceMailNumber)
    {
        int eAccountStatus = acc_GetState(nAccountId);
        if (eAccountStatus != LS_REGISTERED && eAccountStatus != LS_SYSTEM_DEFAULT_ACCOUNT)
        {
            ShowVirtualVoiceMailNote(NULL
                                     , LANG_TRANSLATE(TRID_WARNING)
                                     , LANG_TRANSLATE(TRID_ACCOUNT_NOT_USABLE)
                                     , MESSAGEBOX_NOTE
                                     , 2000);
            return;
        }
        //调用控制层：呼叫语音信箱号码
        g_pVoiceMailController->ClickInfoByIndex(nAccountId);
    }
    else
    {
        emit ListDialogOpenPage(VVM_DIALOG_TYPE_SET);
    }
}

