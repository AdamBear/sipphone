#include "dlgvisualvoicemaillist.h"

#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "include/uicommon.h"
#include "account/include/modaccount.h"

#include "modvoicemailuicommon.h"
#include "voice/include/modvoice.h"
#include "uicommon/globalinputhook/modinputhook.h"

namespace
{
#define Pageaction_Opensubpage_VisualVoiceMailDetail        "visual_voicemail_detail"
#define Pageaction_Opensubpage_VisualVoicemailOption    "visual_voicemail_option"
#define Pageaction_Opensubpage_VisualVoiceMailPlay      "visual_voicemail_play"

#define Pageaction_Opensubpage_SetVoiceMail                 "set_voice_mail"

}

#ifdef _T48
bool VVMPlay_KeypadHookCallbackFun(int nKeyCode, bool bPress)
{
    if (!voice_IsChannelKey(nKeyCode))
    {
        return false;
    }

    FILEPLAYER_STATE eStatus = g_VVMController.GetCurrentPlayState();
    if (FILEPLAYER_STATE_PLAY != eStatus
            && FILEPLAYER_STATE_PAUSE != eStatus)
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
#endif

CDlgVisualVoiceMailList::CDlgVisualVoiceMailList(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pVisualVoiceMailListDelegate(NULL)
#ifdef _T48
    , m_bRefreshOnReshow(false)
#endif
{
    InitPageData();
    InitController();
}

CDlgVisualVoiceMailList::~CDlgVisualVoiceMailList()
{

}

void CDlgVisualVoiceMailList::BeforeSetSubPage(void * pData)
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::BeforeSetSubPage");

    g_VVMController.SetListDialogExist(true);

    g_VVMController.SetCurrentAccount(pData);

#ifdef _T48
    inputHook_RegisterKeyPadHookProc(VVMPlay_KeypadHookCallbackFun, HOOK_MODULE_VVM);
#endif

    int nAccountNumber = g_VVMController.GetCurrentAccount();
    yl::string strName = "";
    if (acc_IsAccountAvailable(nAccountNumber))
    {
        //label > displayname > registerName
        strName = acc_GetLabel(nAccountNumber);
        if (strName.empty())
        {
            strName = acc_GetDisplayName(nAccountNumber);
        }
        if (strName.empty())
        {
            strName = acc_GetRegisterName(nAccountNumber);
        }
    }

    if (strName.empty())
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_VOICEMAIL_LIST));
    }
    else
    {
        SetWindowTitle(toQString(strName));
    }
}

void CDlgVisualVoiceMailList::Uninit()
{
#ifdef _T48
    OnAction(FILEPLAYER_ACTION_STOPONLY);
#endif

    g_VVMController.SetListDialogExist(false);

#ifdef _T48
    inputHook_UnRegisterKeyPadHookProc(VVMPlay_KeypadHookCallbackFun);
#endif
}

void CDlgVisualVoiceMailList::showEvent(QShowEvent * e)
{
    g_VVMController.OnVVMDialogShow();
}

bool CDlgVisualVoiceMailList::LoadPageData()
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::LoadPageData");

    this->setFocus();
    return true;
}

//界面初始化
void CDlgVisualVoiceMailList::InitPageData()
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::InitPageData");

#ifdef _T48
    m_pVisualVoiceMailListDelegate = new CVisualVoiceMailListDelegate();
#else
    m_pVisualVoiceMailListDelegate = new CVisualVoiceMailListDelegate(m_pGeneralFrame);
#endif

    if (m_pGeneralFrame != NULL && NULL != m_pVisualVoiceMailListDelegate)
    {
        m_pVisualVoiceMailListDelegate->setParent(this);
        m_pVisualVoiceMailListDelegate->SetItemPat(this);
#ifdef _T48
        m_pVisualVoiceMailListDelegate->BindFramelist(m_pGeneralFrame);
#endif
        m_pVisualVoiceMailListDelegate->InitSettingListStyle();
        m_pVisualVoiceMailListDelegate->SetItemParent(this);
        m_pVisualVoiceMailListDelegate->PreCreateItem();
        m_pListDelegate = m_pVisualVoiceMailListDelegate;
    }

    QObject::connect(m_pVisualVoiceMailListDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));
}

void CDlgVisualVoiceMailList::InitController()
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::InitController");

    connect(this, SIGNAL(DialogAction(QString)), &g_VVMController, SLOT(OnPlayerAction(QString)));
    connect(this, SIGNAL(DialogAction(QString, QVariant)), &g_VVMController,
            SLOT(OnPlayerAction(QString, QVariant)));
    connect(this, SIGNAL(NormallyExit()), &g_VVMController, SLOT(OnFilePlayFinish()));

    connect(&g_VVMController, SIGNAL(ListDialogOpenPage(VVM_DIALOG_TYPE)), this,
            SLOT(OnListDialogOpenPage(VVM_DIALOG_TYPE)));
    connect(&g_VVMController, SIGNAL(DialogUpdate(int)), this, SLOT(OnDialogListUpdate(int)));
    connect(&g_VVMController, SIGNAL(DialogListExit()), this, SLOT(OnDialogListExit()));

#ifdef _T48
    connect(&g_VVMController, SIGNAL(DialogRefreshSoftkey()), this, SLOT(OnDialogRefreshSoftkey()));
    connect(&g_VVMController, SIGNAL(PlayStateChange()), this, SLOT(OnDialogRefreshSoftkey()));
#endif
}

bool CDlgVisualVoiceMailList::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::GetSoftkeyData");

    if (objSoftData.size() < SOFTKEY_NUM)
    {
        return true;
    }

    if (m_pGeneralFrame == NULL
            || m_pGeneralFrame->GetFocusItemIndex() == INVALID_ITEM_INDEX)
    {
        objSoftData[1].m_strSoftkeyAction = "";
        objSoftData[1].m_strSoftkeyTitle = "";
        objSoftData[2].m_strSoftkeyAction = "";
        objSoftData[2].m_strSoftkeyTitle = "";
        objSoftData[3].m_strSoftkeyAction = "";
        objSoftData[3].m_strSoftkeyTitle = "";
        return true;
    }

#ifdef _T48
    objSoftData[1].m_strSoftkeyAction = FILEPLAYER_ACTION_CALL_VOICEMAIL;
    objSoftData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_VOICEMAIL_CALL_VOICEMAIL);
    objSoftData[1].m_strIconPath = PIC_SOFTKEY_CALLINFO;

    FILEPLAYER_STATE eStatus = g_VVMController.GetCurrentPlayState();
    if (FILEPLAYER_STATE_PLAY == eStatus
            || FILEPLAYER_STATE_PAUSE == eStatus)
    {
        objSoftData[2].m_strSoftkeyAction = FILEPLAYER_ACTION_DELETE;
        objSoftData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
        objSoftData[2].m_strIconPath = PIC_SOFTKEY_REMOVE;

        int iCurrentIndex = m_pGeneralFrame->GetFocusItemDataIndex();

        if (!VoiceMailUI_IsPrivacyVoiceMail(iCurrentIndex))
        {
            objSoftData[3].m_strSoftkeyAction = FILEPLAYER_ACTION_CALL_BACK;
            objSoftData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_VOICEMAIL_OPTION_CALL_BACK);
            objSoftData[3].m_strIconPath = PIC_SOFTKEY_NEWCALL;
        }
        else
        {
            objSoftData[3].m_strSoftkeyAction = "";
            objSoftData[3].m_strSoftkeyTitle = "";
            objSoftData[3].m_strIconPath = "";
        }
    }
    else
    {
        objSoftData[2].m_strSoftkeyAction = "";
        objSoftData[2].m_strSoftkeyTitle = "";
        objSoftData[2].m_strIconPath = "";

        objSoftData[3].m_strSoftkeyAction = "";
        objSoftData[3].m_strSoftkeyTitle = "";
        objSoftData[3].m_strIconPath = "";
    }

#else
    objSoftData[1].m_strSoftkeyAction = FILEPLAYER_ACTION_DELETE;
    objSoftData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);

    objSoftData[2].m_strSoftkeyAction = FILEPLAYER_ACTION_OPTION;
    objSoftData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_OPTION);

    objSoftData[3].m_strSoftkeyAction = FILEPLAYER_ACTION_ENTER_PLAY;
    objSoftData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_PLAY);
#endif

    return true;
}

bool CDlgVisualVoiceMailList::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::OnAction, strAction = %s", strAction.toUtf8().data());

    if (ACTION_QUIT == strAction)
    {
        emit BtnBackClick();
        return true;
    }

#ifdef _T48
    if (FILEPLAYER_ACTION_ENTER_PLAY == strAction)
    {
        int iCurrentIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        m_pListDelegate->SetCurrentIndex(iCurrentIndex);
        emit DialogAction(strAction, QVariant::fromValue<int>(iCurrentIndex));
    }
    else
    {
        emit DialogAction(strAction);
    }
#else
    int iCurrentIndex = m_pGeneralFrame->GetFocusItemDataIndex();
    m_pListDelegate->SetCurrentIndex(iCurrentIndex);
    SETTINGUI_INFO("CDlgVisualVoiceMailList::OnAction, CurrentIndex = %d", iCurrentIndex);

    emit DialogAction(strAction, QVariant::fromValue<int>(iCurrentIndex));
#endif
    return false;
}

void CDlgVisualVoiceMailList::OnListDialogOpenPage(VVM_DIALOG_TYPE eDialogPage)
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::OnListDialogOpenPage");

    yl::string strPage = "";
    if (VVM_DIALOG_TYPE_DETAIL == eDialogPage)
    {
        strPage = Pageaction_Opensubpage_VisualVoiceMailDetail;
    }
    else if (VVM_DIALOG_TYPE_OPTION == eDialogPage)
    {
        strPage = Pageaction_Opensubpage_VisualVoicemailOption;
    }
    else if (VVM_DIALOG_TYPE_PLAY == eDialogPage)
    {
        strPage = Pageaction_Opensubpage_VisualVoiceMailPlay;
    }
    else if (VVM_DIALOG_TYPE_SET == eDialogPage)
    {
        strPage = Pageaction_Opensubpage_SetVoiceMail;
    }
    else
    {
        SETTINGUI_INFO("CDlgVisualVoiceMailList::OnListDialogOpenPage, strPage error");
        return;
    }

    SETTINGUI_INFO("CDlgVisualVoiceMailList::OnListDialogOpenPage, strPage = %s", strPage.c_str());

    SubMenuData subData(strPage);
    if (VVM_DIALOG_TYPE_SET == eDialogPage)
    {
        subData.iFirstSelectItem = g_VVMController.GetCurrentAccount();
    }
    emit openpage(subData);
}

void CDlgVisualVoiceMailList::OnDialogListUpdate(int nFocusIndex)
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::OnDialogListUpdate, nFocusIndex = %d, IsTopPage[%d]",
                   nFocusIndex, IsTopPage());

    m_pListDelegate->SetCurrentIndex(nFocusIndex);

    if (IsTopPage())
    {
        UpdateWnd();
    }
    else
    {
        m_bRefreshOnReshow = true;
    }
}

void CDlgVisualVoiceMailList::OnDialogListExit()
{
    SETTINGUI_INFO("CDlgVisualVoiceMailList::OnDialogListExit");
    emit BtnBackClick();
}

#ifdef _T48
bool CDlgVisualVoiceMailList::PorcessBackBtnClick()
{
    FILEPLAYER_STATE eStatus = g_VVMController.GetCurrentPlayState();
    if (FILEPLAYER_STATE_PLAY == eStatus
            || FILEPLAYER_STATE_PAUSE == eStatus)
    {
        emit NormallyExit();
    }

    return false;
}

bool CDlgVisualVoiceMailList::PorcessHomeBtnClick()
{
    FILEPLAYER_STATE eStatus = g_VVMController.GetCurrentPlayState();
    if (FILEPLAYER_STATE_PLAY == eStatus
            || FILEPLAYER_STATE_PAUSE == eStatus)
    {
        emit NormallyExit();
    }

    emit BtnBackClick();
    UIManager_ReturnToIdle(true);
    return true;
}

bool CDlgVisualVoiceMailList::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    if (QEvent::KeyPress != pEvent->type()
            && QEvent::KeyRelease != pEvent->type())
    {
        return CDlgBaseSubPage::eventFilter(pObject, pEvent);
    }

    QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
    int nKey = pKeyEvent->key();
    bool bPress = (QEvent::KeyPress == pEvent->type());

    SETTINGUI_INFO("CDlgVisualVoiceMailList::eventFilter, Key = %d, Pressed = %d", nKey, bPress);

    if (PHONE_KEY_0 <= nKey && nKey <= PHONE_KEY_9)
    {
        return true;
    }
    else if (PHONE_KEY_LEFT == nKey
             || PHONE_KEY_UP == nKey
             || PHONE_KEY_RIGHT == nKey
             || PHONE_KEY_DOWN == nKey
             || PHONE_KEY_OK == nKey
             || PHONE_KEY_CANCEL == nKey
             || PHONE_KEY_VOLUME_DECREASE == nKey
             || PHONE_KEY_VOLUME_INCREASE == nKey)
    {
        if (ProcessFunctionkey(nKey, bPress))
        {
            return true;
        }
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgVisualVoiceMailList::ProcessFunctionkey(int nKeyCode, bool bPress)
{
    FILEPLAYER_STATE ePlayerState = g_VVMController.GetCurrentPlayState();
    bool bPlaying = (FILEPLAYER_STATE_PLAY == ePlayerState
                     || FILEPLAYER_STATE_PAUSE == ePlayerState);

    if (PHONE_KEY_LEFT == nKeyCode)
    {
        if (bPlaying)
        {
            if (bPress)
            {
                OnAction(FILEPLAYER_ACTION_SWIFT_PRE_START);
            }
            else
            {
                OnAction(FILEPLAYER_ACTION_SWIFT_PRE_OVER);
            }
        }
    }
    else if (PHONE_KEY_RIGHT == nKeyCode)
    {
        if (bPlaying)
        {
            if (bPress)
            {
                OnAction(FILEPLAYER_ACTION_SWIFT_NEXT_START);
            }
            else
            {
                OnAction(FILEPLAYER_ACTION_SWIFT_NEXT_OVER);
            }
        }
    }
    else if (PHONE_KEY_UP == nKeyCode)
    {
        if (bPress && NULL != m_pListDelegate)
        {
            m_pListDelegate->GoPrePage();
        }
    }
    else if (PHONE_KEY_DOWN == nKeyCode)
    {
        if (bPress && NULL != m_pListDelegate)
        {
            m_pListDelegate->GoNextPage();
        }
    }
    else if (PHONE_KEY_OK == nKeyCode)
    {
        if (bPlaying && bPress)
        {
            if (FILEPLAYER_STATE_PLAY == g_VVMController.GetCurrentPlayState())
            {
                OnAction(FILEPLAYER_ACTION_PAUSE);
            }
            else if (FILEPLAYER_STATE_PAUSE == g_VVMController.GetCurrentPlayState())
            {
                OnAction(FILEPLAYER_ACTION_RESUME);
            }
            else if (FILEPLAYER_STATE_STOP == g_VVMController.GetCurrentPlayState())
            {
                OnAction(FILEPLAYER_ACTION_ENTER_PLAY);
            }
        }
    }
    else if (PHONE_KEY_CANCEL == nKeyCode)
    {
        if (bPress)
        {
            emit NormallyExit();
            OnAction(ACTION_QUIT);
        }
    }
    else if (PHONE_KEY_VOLUME_DECREASE == nKeyCode ||
             PHONE_KEY_VOLUME_INCREASE == nKeyCode)
    {
        int iVolume = voice_GetVolume();
        iVolume = (PHONE_KEY_VOLUME_INCREASE == nKeyCode) ? iVolume + 1 : iVolume - 1;
        voice_SetVolume(iVolume, VT_AUTO, TRUE);
        return false;
    }

    return true;
}

void CDlgVisualVoiceMailList::OnDialogRefreshSoftkey()
{
    emit RefreshSoftkey();
}

bool CDlgVisualVoiceMailList::IsRejectReturnToIdle(RETURN_IDLE_TYPE
        eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    if (eReturnIdle != RETURN_IDLE_TYPE_TIME_OUT)
    {
        return false;
    }

    FILEPLAYER_STATE eStatus = g_VVMController.GetCurrentPlayState();
    if (FILEPLAYER_STATE_PLAY == eStatus
            || FILEPLAYER_STATE_PAUSE == eStatus)
    {
        return true;
    }

    return false;
}

void CDlgVisualVoiceMailList::OnReShow()
{
    if (m_bRefreshOnReshow)
    {
        UpdateWnd();
        m_bRefreshOnReshow = false;
    }
    else
    {
        CDlgBaseSubPage::OnReShow();
    }
}

bool CDlgVisualVoiceMailList::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
        QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = 0;
    iMax = 15;
    return true;
}
#endif
