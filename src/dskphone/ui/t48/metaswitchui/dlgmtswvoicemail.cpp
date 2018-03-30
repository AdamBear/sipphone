#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "dlgmtswvoicemail.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "contacts/directory/include/moddirinterface.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "baseui/t4xpicpath.h"
#include "setting/include/modsetting.h"
#include "settingui/include/modsettingui.h"
#include "directoryui/include/moddirectoryui.h"
#include "uicommon/qtcommon/qmisc.h"
#include "include/uicommon.h"
#include "threadtask/include/common.h"
#include "mainwnd/mainwnd.h"
#include "voice/include/modvoice.h"
#include "uicommon/globalinputhook/modinputhook.h"

namespace
{
#define Pageaction_Opensubpage_MetaswitchCommportal "OpenSubPage(mtswcommportal)"
#define MSG_ID_LOAD_VOICEMAIL_LIST 29280
#define MSG_ID_DELETE_MESSAGE 29281
#define MSG_ID_DELETE_ALL_MESSAGE 29282
#define MSG_ID_LOAD_VOICEMAIL 29283
#define MSG_ID_LOAD_FAIL 29284
#define MSG_ID_OPTION 29285
#define MSG_ID_COMMPORTAL 29286
}

bool MTSWVoicemail_KeypadHookCallbackFun(int nKeyCode, bool bPress)
{
    if (!voice_IsChannelKey(nKeyCode))
    {
        return false;
    }

    // 不在顶层窗口不处理声道键
    CDlgMTSWVoicemail * pMTSWVoicemail = qobject_cast<CDlgMTSWVoicemail *>(UIManager_GetPrivateDlg(
            DLG_CDlgMTSWVoicemail));
    if (NULL == pMTSWVoicemail
            || !pMTSWVoicemail->IsTopPage())
    {
        return false;
    }

    if (MTSWVM_GetPlayStatus() != STATUS_PLAY
            && MTSWVM_GetPlayStatus() != STATUS_MTSW_PAUSE)
    {
        return false;
    }

    if (bPress)
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_HANDSET_ON_HOOK:
            {
                voice_SwitchChannel(CHANNEL_RING);
            }
            break;
        case PHONE_KEY_HEADSET_CTRL:
            {
                if (!voice_GetHeadsetStatus())
                {
                    voice_SwitchChannel(CHANNEL_RING);
                }
            }
            break;
        default:
            break;
        }
    }

    return true;
}

static LRESULT OnMTSWMessage(msgObject & objMessage)
{
    CDlgMTSWVoicemail * pMTSWVoicemail = qobject_cast<CDlgMTSWVoicemail *>(UIManager_GetPrivateDlg(
            DLG_CDlgMTSWVoicemail));
    if (NULL == pMTSWVoicemail)
    {
        return 0;
    }

    if (pMTSWVoicemail->IsTopPage())
    {
        pMTSWVoicemail->OnReceiveMsg(objMessage);
    }
    return 0;
}

CDlgMTSWVoicemail::CDlgMTSWVoicemail(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    SetWindowType(eWT_Message);
    InitData();
    etl_RegisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END, OnMTSWMessage);
}

CDlgMTSWVoicemail::~CDlgMTSWVoicemail()
{
    MTSWVM_SetPlayLoading(false);
    MTSWVM_Stop();
    etl_UnregisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END, OnMTSWMessage);
    //清空数据
    MTSWVM_Clean();
    _MainWND.SetTimerLock(false);
    inputHook_UnRegisterKeyPadHookProc(MTSWVoicemail_KeypadHookCallbackFun);
}

void CDlgMTSWVoicemail::InitData()
{
    m_bAddNewItem = true;
    m_iPlayIndex = -1;

    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_MTSW_VOICEMAIL_MESSAGE);

    m_pMTSWListDelegate = new MTSWVoicemailListDelegate();

    if (m_pGeneralFrame != NULL && NULL != m_pMTSWListDelegate)
    {
        m_pMTSWListDelegate->setParent(this);
        m_pMTSWListDelegate->SetItemPat(this);
        //绑定列表
        m_pMTSWListDelegate->BindFramelist(m_pGeneralFrame);
        m_pMTSWListDelegate->InitSettingListStyle();
        m_pMTSWListDelegate->SetItemParent(this);
        m_pMTSWListDelegate->PreCreateItem();
        m_pListDelegate = m_pMTSWListDelegate;
    }

    QObject::connect(m_pMTSWListDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));

    QObject::connect(m_pMTSWListDelegate, SIGNAL(ItemMoreAction(CListItem *)), this,
                     SLOT(OnItemMoreAction(CListItem *)));

    QObject::connect(m_pMTSWListDelegate, SIGNAL(ItemStopAction(CListItem *)), this,
                     SLOT(OnItemStopAction(CListItem *)));

    QObject::connect(m_pMTSWListDelegate, SIGNAL(ItemFinishAction(CListItem *)), this,
                     SLOT(OnItemFinishAction(CListItem *)));

    MTSWVM_SetDownLoading(true);
    _MainWND.SetTimerLock(true);

    // 注册按键回调函数
    inputHook_RegisterKeyPadHookProc(MTSWVoicemail_KeypadHookCallbackFun, HOOK_MODULE_VVM);
}

bool CDlgMTSWVoicemail::LoadPageData()
{
    this->setFocus();
    return true;
}

void CDlgMTSWVoicemail::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgMTSWVoicemail::OnShow()
{

}

void CDlgMTSWVoicemail::ExtraDialogInitData()
{
    if (!MTSW_IsLogined())
    {
        MessageBox_ShowMTSWCommportalBox(this, MSG_ID_COMMPORTAL);
    }
    else
    {
        MTSWVM_LoadVoiceMailList();

        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOAD_VOICEMAIL_LIST);
    }
}

bool CDlgMTSWVoicemail::LoadVoicemailList(int nDataIndex, int nDataCount)
{
    if (MTSWVM_IsDownLoading())
    {
        return false;
    }

    if (nDataIndex < 0 || nDataIndex > MTSWVM_GetTotalCount() || nDataCount < 1)
    {
        return false;
    }

    if (!m_bAddNewItem)
    {
        m_bAddNewItem = true;
    }
    else
    {
        m_vecWidgetPair.clear();
        VoiceMsgItemDetailData VMIDD;
        for (int i = 0; MTSWVM_GetMessageDetail(&VMIDD, nDataIndex + i) && i < nDataCount; i++)
        {
//          SETTINGUI_INFO(" [%d] m_strName:[%s] m_strTime:[%s] m_eStatus:[%d] m_bUrgent[%d] m_strDuration[%s]", nDataIndex + i, VMIDD.m_strName.c_str(),
//              VMIDD.m_strTime.c_str(), VMIDD.m_eStatus, VMIDD.m_bUrgent, VMIDD.m_strDuration.c_str());
            QWidgetPair qPair;
            yl::string strDisplay;
            strDisplay.append(VMIDD.m_strName).append(" - ").append(VMIDD.m_strNumber);
            qPair.first = toQString(strDisplay);
            qPair.eType = VOICEMAIL_LIST_ITEM;

            QLabel * pItemLab = new QLabel(this);
            pItemLab->setText(toQString(VMIDD.m_strDuration));
            qPair.second = pItemLab;
            qPair.iIndex = nDataIndex + i;
            qPair.bAddIcon = true;

            if (qPair.iIndex == m_iPlayIndex)
            {
                if (MTSWVM_GetPlayStatus() == STATUS_MTSW_PAUSE)
                {
                    qPair.strIcon = PIC_USB_BROWSE_ITEM_PLAY;
                    qPair.third = DM_SOFTKEY_MTSW_RESUME;
                }
                else if (MTSWVM_GetPlayStatus() == STATUS_PLAY)
                {
                    qPair.strIcon = PIC_USB_BROWSE_ITEM_PAUSE;
                    qPair.third =  DM_SOFTKEY_MTSW_PAUSE;
                }
            }
            else
            {
                qPair.strIcon = PIC_USB_BROWSE_ITEM_PLAY;
                qPair.third =  DM_SOFTKEY_MTSW_PLAY;
            }

            m_vecWidgetPair.push_back(qPair);
        }
    }

    if (NULL != m_pListDelegate && m_pListDelegate->inherits("MTSWVoicemailListDelegate"))
    {
        MTSWVoicemailListDelegate * pMTSWVoicemailListDelegate = static_cast<MTSWVoicemailListDelegate *>
                (m_pListDelegate);
        pMTSWVoicemailListDelegate->SetItemsData(m_vecWidgetPair, MTSWVM_GetTotalCount());
    }
    return true;
}

bool CDlgMTSWVoicemail::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    return true;
                }
                break;
            case PHONE_KEY_VOLUME_DECREASE:
            case PHONE_KEY_VOLUME_INCREASE:
                {
                    int iVolume = voice_GetVolume();
                    iVolume = (PHONE_KEY_VOLUME_INCREASE == pKeyEvent->key()) ? iVolume + 1 : iVolume - 1;
                    voice_SetVolume(iVolume, VT_AUTO, TRUE);
                    return false;
                }
                break;
            case PHONE_KEY_0:
            case PHONE_KEY_1:
            case PHONE_KEY_2:
            case PHONE_KEY_3:
            case PHONE_KEY_4:
            case PHONE_KEY_5:
            case PHONE_KEY_6:
            case PHONE_KEY_7:
            case PHONE_KEY_8:
            case PHONE_KEY_9:
                {
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgMTSWVoicemail::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_MTSW_VOICEMAIL_OPTION_CALL_VOICEMAIL);
    objWndData[0].m_strSoftkeyAction = DM_SOFTKEY_MTSW_CALL_VOICEAMIL;
    objWndData[0].m_strIconPath = PIC_SOFTKEY_CALLINFO;

    objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
    objWndData[1].m_strSoftkeyAction = DM_SOFTKEY_CLICK_CLEAR;
    objWndData[1].m_strIconPath = PIC_SOFTKEY_REMOVE;

    objWndData[2].m_strSoftkeyTitle = "";
    objWndData[2].m_strSoftkeyAction = "";
    objWndData[2].m_strIconPath = "";

    objWndData[3].m_strSoftkeyTitle = "";
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strIconPath = "";
    return true;
}

bool CDlgMTSWVoicemail::OnAction(const QString & strAction)
{
    int iCurrentIndex = m_pListDelegate->GetCurrentIndex();
    if (iCurrentIndex < 0)
    {
        SETTINGUI_INFO("CDlgMTSWVoicemail::OnAction fail !! strAction[%s] CurrentIndex[%d]", strAction.toUtf8().data(),
                       iCurrentIndex);
        return false;
    }
    MTSWVM_SetItemIndex(iCurrentIndex);

    SETTINGUI_INFO("CDlgMTSWVoicemail::OnAction[%s] CurrentIndex[%d]", strAction.toUtf8().data(),
                   iCurrentIndex);

    if (strAction == DM_SOFTKEY_MTSW_MORE)
    {
        ShowMore(iCurrentIndex);
    }
    else if (strAction == DM_SOFTKEY_CLICK_SEND)
    {
        ProcessCallBackKey();
    }
    else if (strAction == DM_SOFTKEY_MTSW_PLAY)
    {
        ProcessPlayKey();
    }
    else if (strAction == DM_SOFTKEY_MTSW_PAUSE)
    {
        PauseVoiceMail(iCurrentIndex);
    }
    else if (strAction == DM_SOFTKEY_MTSW_RESUME)
    {
        ResumeVoiceMail(iCurrentIndex);
    }
    else if (strAction == DM_SOFTKEY_MTSW_STOP)
    {
        StopVoiceMail(iCurrentIndex, true);
    }
    else if (strAction == DM_SOFTKEY_MTSW_MARK_AS_NEW)
    {
        MarkReadStatus(STATUS_UNREAD);
    }
    else if (strAction == DM_SOFTKEY_MTSW_MARK_AS_HEARD)
    {
        MarkReadStatus(STATUS_READ);
    }
    else if (strAction == DM_SOFTKEY_MTSW_ADD_TO_LOCAL)
    {
        AddToContacts(true);
    }
    else if (strAction == DM_SOFTKEY_MTSW_ADD_TO_NETWORK)
    {
        AddToContacts(false);
    }
    else if (strAction == DM_SOFTKEY_MTSW_CALL_VOICEAMIL)
    {
        Setting_ConnectVoiceMail();
    }
    else if (strAction == DM_SOFTKEY_CLICK_DELETE)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_DELETE_MESSAGE), MESSAGEBOX_OkCancel, -1,
                                    "", "", MSG_ID_DELETE_MESSAGE);
    }
    else if (strAction == DM_SOFTKEY_CLICK_CLEAR)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_DELETE_ALL_MESSAGES), MESSAGEBOX_OkCancel,
                                    -1, "", "", MSG_ID_DELETE_ALL_MESSAGE);
    }
    else
    {
        return false;
    }
    return true;
}

void CDlgMTSWVoicemail::OnItemMoreAction(CListItem * pItem)
{
    CSettingItem * pSetItem = (CSettingItem *)pItem;

    if (NULL != pSetItem)
    {
        m_pListDelegate->SetCurrentIndex(pSetItem->GetDataIndex());
    }

    OnAction(DM_SOFTKEY_MTSW_MORE);
}

void CDlgMTSWVoicemail::OnItemStopAction(CListItem * pItem)
{
    CSettingItem * pSetItem = (CSettingItem *)pItem;

    if (NULL != pSetItem)
    {
        m_pListDelegate->SetCurrentIndex(pSetItem->GetDataIndex());
    }

    OnAction(DM_SOFTKEY_MTSW_STOP);
}

void CDlgMTSWVoicemail::OnItemFinishAction(CListItem * pItem)
{
    /*
    // 若有未读留言则自动播放下一条未读留言
    if (MTSWVM_GetTotalUnRead() > 0)
    {
        MTSWVM_SetPlayLoading(true);
        if (MTSWVM_DownloadNextVoiceMail())
        {
            MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOAD_VOICEMAIL);
            StopVoiceMail(m_iPlayIndex, false);
            return;
        }
        else
        {
            MTSWVM_SetPlayLoading(false);
        }
    }
    */
    StopVoiceMail(m_iPlayIndex, true);
}

// CallBack处理
bool CDlgMTSWVoicemail::ProcessCallBackKey()
{
    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    if (iIndex < 0)
    {
        return true;
    }

    MTSWVM_GetMessageDetail(&VMIDD, iIndex);

    yl::string strNumber = VMIDD.m_strNumber;
    yl::string strName = VMIDD.m_strName;

    talklogic_CallOut(strNumber.c_str(), strName.c_str());

    return true;
}

bool CDlgMTSWVoicemail::AddToContacts(bool bIsLocal)
{
    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    if (iIndex < 0)
    {
        return true;
    }
    MTSWVM_GetMessageDetail(&VMIDD, iIndex);
    yl::string strName = VMIDD.m_strName;
    yl::string strNum = VMIDD.m_strNumber;

    if (bIsLocal)
    {
        //添加到本地联系人
        DM_AddDirectory(strName, strNum, DIR_ACTION_LOCAL_COPY_TO_CONTACTS, DIRECTORY_TYPE_LOCAL);
    }
    else
    {
        //添加到metaswitch联系人
        DM_AddDirectory(strName, strNum, DIR_ACTION_MTSW_ADD_CONTACTLIST, DIRECTORY_TYPE_METASWICTH);
    }
    return true;
}

bool CDlgMTSWVoicemail::MarkReadStatus(ReadStatus readStatus)
{
    if (!netIsNetworkReady())
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_FAILED_PLEASE_RETRY),
                                    MESSAGEBOX_NOTE, 2000);
    }
    else
    {
        MTSWVM_MarkReadStatus(readStatus);
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, 1500);
        UpdateWnd();
    }

    return true;
}

QWidgetPair * CDlgMTSWVoicemail::FindWidgetPair(int iCurrentIndex)
{
    if (iCurrentIndex < 0)
    {
        return NULL;
    }

    QWPVector::iterator iter = m_vecWidgetPair.begin();
    for (; iter != m_vecWidgetPair.end(); ++iter)
    {
        if (iter->iIndex == iCurrentIndex)
        {
            return iter;
        }
    }
    return NULL;
}

bool CDlgMTSWVoicemail::ProcessPlayKey()
{
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOAD_VOICEMAIL);

    StopVoiceMail(m_iPlayIndex, false, false);

    _MainWND.SetTimerLock(true);

    MTSWVM_SetPlayLoading(true);
    MTSWVM_DownloadVoiceMail();
    return true;
}

void CDlgMTSWVoicemail::PlayVoiceMail(int iCurrentIndex)
{
    if (iCurrentIndex < 0)
    {
        return;
    }

    MTSWVM_Play();
    MTSWVM_MarkReadStatus(STATUS_READ);

    //记录当前播放文件的下标
    m_iPlayIndex = iCurrentIndex;

    QWidgetPair * pWidgetPair = FindWidgetPair(iCurrentIndex);
    if (pWidgetPair == NULL)
    {
        m_pListDelegate->SetCurrentIndex(iCurrentIndex);
        UpdateWnd();
        return;
    }

    pWidgetPair->strIcon = PIC_USB_BROWSE_ITEM_PLAY_PRESS;
    pWidgetPair->third = DM_SOFTKEY_MTSW_PAUSE;
    m_bAddNewItem = false;
    UpdateWnd();
}

void CDlgMTSWVoicemail::PauseVoiceMail(int iCurrentIndex)
{
    if (iCurrentIndex < 0)
    {
        return;
    }

    MTSWVM_Pause();

    QWidgetPair * pWidgetPair = FindWidgetPair(iCurrentIndex);
    if (pWidgetPair == NULL)
    {
        return;
    }

    pWidgetPair->strIcon = PIC_USB_BROWSE_ITEM_PLAY;
    pWidgetPair->third = DM_SOFTKEY_MTSW_RESUME;
    m_bAddNewItem = false;
    UpdateWnd();
}

void CDlgMTSWVoicemail::ResumeVoiceMail(int iCurrentIndex)
{
    if (iCurrentIndex < 0)
    {
        return;
    }

    MTSWVM_Resume();

    QWidgetPair * pWidgetPair = FindWidgetPair(iCurrentIndex);
    if (pWidgetPair == NULL)
    {
        return;
    }

    pWidgetPair->strIcon = PIC_USB_BROWSE_ITEM_PAUSE;
    pWidgetPair->third = DM_SOFTKEY_MTSW_PAUSE;
    m_bAddNewItem = false;
    UpdateWnd();
}

void CDlgMTSWVoicemail::StopVoiceMail(int iCurrentIndex, bool bOpenTimer, bool bResetChannel/* = true*/)
{
    if (iCurrentIndex < 0)
    {
        return;
    }

    if (iCurrentIndex < 0 || iCurrentIndex >= MTSWVM_GetTotalCount())
    {
        return;
    }

    if (bOpenTimer)
    {
        _MainWND.SetTimerLock(false);
    }

    MTSWVM_Stop(bResetChannel);
    m_iPlayIndex = -1;

    QWidgetPair * pWidgetPair = FindWidgetPair(iCurrentIndex);
    if (pWidgetPair == NULL)
    {
        return;
    }

    pWidgetPair->strIcon = PIC_USB_BROWSE_ITEM_STOP_PRESS;
    pWidgetPair->third = DM_SOFTKEY_MTSW_PLAY;
    m_bAddNewItem = false;
    UpdateWnd();
}

void CDlgMTSWVoicemail::ShowMore(int iCurrentIndex)
{
    if (iCurrentIndex < 0)
    {
        return;
    }

    StopVoiceMail(m_iPlayIndex, true);

    VoiceMsgItemDetailData VMIDD;
    MTSWVM_GetMessageDetail(&VMIDD, iCurrentIndex);

    bool bRead = true;
    if (VMIDD.m_eStatus == STATUS_UNREAD)
    {
        bRead = false;
    }

    bool bAddToNetwork = MtswDir_IsEnable();
    MessageBox_ShowMTSWVoiceMailOptionBox(this, toQString(VMIDD.m_strName),
                                          bRead, bAddToNetwork, -1, MSG_ID_OPTION);
}

void CDlgMTSWVoicemail::DealWithMore(const int iResult)
{
    switch (iResult)
    {
    case MessageBox_CallBack:
        {
            OnAction(DM_SOFTKEY_CLICK_SEND);
        }
        break;
    case MessageBox_MarkAsNew:
        {
            OnAction(DM_SOFTKEY_MTSW_MARK_AS_NEW);
        }
        break;
    case MessageBox_MarkAsHeard:
        {
            OnAction(DM_SOFTKEY_MTSW_MARK_AS_HEARD);
        }
        break;
    case MessageBox_AddToLocal:
        {
            OnAction(DM_SOFTKEY_MTSW_ADD_TO_LOCAL);
        }
        break;
    case MessageBox_AddToNetwork:
        {
            OnAction(DM_SOFTKEY_MTSW_ADD_TO_NETWORK);
        }
        break;
    case MessageBox_Delete:
        {
            OnAction(DM_SOFTKEY_CLICK_DELETE);
        }
        break;
    default:
        break;
    }
}

void CDlgMTSWVoicemail::OnReceiveMsg(const msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case MTSW_MESSAGE_LOGIN:
        {
            SETTINGUI_INFO("OnReceiveMsg MTSW_MESSAGE_LOGIN");
            //登录成功重新下载列表
            MTSWVM_LoadVoiceMailList();
        }
        break;
    case MTSW_MESSAGE_AUTH_ERR:
        {
            NS_TA::TaskActionErrorType eErrCode = (NS_TA::TaskActionErrorType)objMessage.lParam;
            MessageBox_RemoveAllMessageBox(MessageBox_Destory);
            SETTINGUI_INFO("OnReceiveMsg MTSW_MESSAGE_AUTH_ERR(%d)", objMessage.lParam);

            if ((eErrCode == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
                    || eErrCode == NS_TA::TA_HTTP_ERR403)
            {
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_COMMPORTAL_LOGIN_FAILED), MESSAGEBOX_OkCancel,
                                            -1
                                            , "", "", MSG_ID_LOAD_FAIL);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOGIN_FAILED), MESSAGEBOX_OkCancel, -1
                                            , "", "", MSG_ID_LOAD_FAIL);
            }
        }
        break;
    case MTSW_MESSAGE_DOWNLOADED:
        {
            SETTINGUI_INFO("OnReceiveMsg MTSW_MESSAGE_DOWNLOADED");
            //关闭弹框
            MessageBox_RemoveAllMessageBox(MessageBox_Destory);
            PlayVoiceMail(objMessage.wParam);
        }
        break;
    case MTSW_MESSAGE_DOWNLOADFAILED:
        {
            SETTINGUI_INFO("OnReceiveMsg MTSW_MESSAGE_DOWNLOADFAILED");
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500);
        }
        break;
    case MTSW_MESSAGE_VMLIST_RESULT:
        {
            SETTINGUI_INFO("OnReceiveMsg MTSW_MESSAGE_VMLIST_RESULT [%d]", objMessage.lParam);
            MTSWVM_SetDownLoading(false);
            //关闭弹框
            MessageBox_RemoveAllMessageBox(MessageBox_Destory);

            if (objMessage.lParam == 1)
            {
                _MainWND.SetTimerLock(false);
                UpdateWnd();
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500
                                            , "", "", MSG_ID_LOAD_FAIL);
            }
        }
        break;
    default:
        {

        }
        break;
    }
}

void CDlgMTSWVoicemail::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    int iMsgID = pMessageBox->GetID();
    MESSAGEBOX_RESULT eResault = pMessageBox->GetResult();

    switch (iMsgID)
    {
    case MSG_ID_LOAD_VOICEMAIL_LIST:
        {
            if (MessageBox_Cancel == eResault)
            {
                MTSWVM_CancelDownload();
                emit BtnBackClick();
            }
        }
    case MSG_ID_DELETE_MESSAGE:
        {
            if (MessageBox_OK == eResault)
            {
                int iCurrentIndex = MTSWVM_GetItemIndex();
                int iTotal = MTSWVM_GetTotalCount() - 1;
                //删除的是最后一条则焦点上移
                if (iCurrentIndex == iTotal)
                {
                    m_pListDelegate->SetCurrentIndex(iCurrentIndex - 1);
                }
                MTSWVM_Delete();
                UpdateWnd();
            }
        }
        break;
    case MSG_ID_DELETE_ALL_MESSAGE:
        {
            if (MessageBox_OK == eResault)
            {
                StopVoiceMail(m_iPlayIndex, true);

                MTSWVM_DeleteAll();
                UpdateWnd();
            }
        }
        break;
    case MSG_ID_LOAD_VOICEMAIL:
        {
            if (MessageBox_Cancel == eResault)
            {
                _MainWND.SetTimerLock(false);

                MTSWVM_SetPlayLoading(false);

                MTSWVM_Stop();
            }
            else if (MessageBox_Destory == eResault)
            {
                MTSWVM_SetPlayLoading(false);
            }
        }
        break;
    case MSG_ID_LOAD_FAIL:
        {
            if (MessageBox_OK == eResault)
            {
                MessageBox_ShowMTSWCommportalBox(this, MSG_ID_COMMPORTAL);
            }
            //下载失败,退出界面
            else if (MessageBox_Cancel == eResault)
            {
                emit BtnBackClick();
            }
        }
        break;
    case MSG_ID_OPTION:
        {
            DealWithMore(eResault);
        }
        break;
    case MSG_ID_COMMPORTAL:
        {
            if (MessageBox_OK == eResault)
            {
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                            MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOAD_VOICEMAIL_LIST);
            }
            else if (MessageBox_Cancel == eResault)
            {
                emit BtnBackClick();
            }
        }
        break;
    default:
        break;
    }
}

bool CDlgMTSWVoicemail::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                        QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = 0;
    iMax = 15;
    return true;
}
#endif
