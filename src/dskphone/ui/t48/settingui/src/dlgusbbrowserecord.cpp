#include "dlgusbbrowserecord.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "actiondefinecommon.h"
#include "messagebox/modmessagebox.h"
#include "record/include/modrecord.h"
#include "baseui/t4xpicpath.h"
#include "lamp/backlight/include/modbacklight.h"
#include "uicommon/uimanager/moduimanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "record/include/recordmessagedefine.h"
#include "voice/include/modvoice.h"
#include "uicommon/globalinputhook/modinputhook.h"

namespace
{
#define ACTION_DELETE               "delete"
#define ACTION_DELETEALL            "deleteAll"
#define ACTION_BACK                 "Back"
#define ACTION_SWITCH_PAGE          "SwitchPage"
#define LOAD_MIN_TIME               500
#define MSG_ID_READING_FILE         29280
#define MSG_ID_DELETE_FOCUS_RECORD  29281
#define MSG_ID_DELETE_ALL_RECORD    29282
}

bool USBPlayRecord_KeypadHookCallbackFun(int nKeyCode, bool bPress)
{
    if (!voice_IsChannelKey(nKeyCode))
    {
        return false;
    }

    // 不在顶层窗口不处理声道键
    CDlgUSBBrowseRecord * pUSBBrowseRecord = qobject_cast<CDlgUSBBrowseRecord *>
            (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowseRecord));
    if (NULL == pUSBBrowseRecord
            || !pUSBBrowseRecord->IsTopPage())
    {
        return false;
    }

    if (!modRecord_IsPlaying(RECORD_AUDIO)
            && !modRecord_IsPlayPause(RECORD_AUDIO))
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

static LRESULT USBBR_ReceiveingMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnUSBBrowseRecord 0x[%x] 0x[%x]",
                   objMessage.message, objMessage.wParam);

    CDlgUSBBrowseRecord * pUSBBrowseRecord = qobject_cast<CDlgUSBBrowseRecord *>
            (UIManager_GetPrivateDlg(
                 DLG_CDlgUSBBrowseRecord));
    if (NULL == pUSBBrowseRecord || !pUSBBrowseRecord->IsTopPage())
    {
        return 0;
    }

    pUSBBrowseRecord->OnReceiveMsg(objMessage);
    return 0;
}


void OnPlayRecordTopWndChange(CBaseDialog * pDlgPreTop, CBaseDialog * pDlgNewTop)
{
    CDlgUSBBrowseRecord * pUSBBrowseRecord = qobject_cast<CDlgUSBBrowseRecord *>
            (UIManager_GetPrivateDlg(DLG_CDlgUSBBrowseRecord));
    if (NULL == pUSBBrowseRecord)
    {
        return ;
    }

    if (pUSBBrowseRecord->IsTopPage())
    {
        // 重新刷新数据界面
        pUSBBrowseRecord->OnShow();
        pUSBBrowseRecord->PageChange(false);
    }
    else
    {
        if (!modRecord_IsPlaying(RECORD_AUDIO)
                && !modRecord_IsPlayPause(RECORD_AUDIO))
        {
            pUSBBrowseRecord->OnAction(ACTION_BACK);
        }
        else
        {
            pUSBBrowseRecord->PageChange(true);
        }
    }
}


CDlgUSBBrowseRecord::CDlgUSBBrowseRecord(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_timerLoad(this)
    , m_bLoadMinTimeOut(false)
    , m_pUSBRecordListDelegate(NULL)
{

    InitPageData();

    inputHook_RegisterKeyPadHookProc(USBPlayRecord_KeypadHookCallbackFun, HOOK_MODULE_VVM);
}

CDlgUSBBrowseRecord::~CDlgUSBBrowseRecord()
{
    if (m_timerLoad.isActive())
    {
        m_timerLoad.stop();
    }

    backlight_OpenBacklightEvent(true);
    UIManager_UnregisterTopWndChangeCallBack(&OnPlayRecordTopWndChange);

    inputHook_UnRegisterKeyPadHookProc(USBPlayRecord_KeypadHookCallbackFun);
}


void CDlgUSBBrowseRecord::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgUSBBrowseRecord::OnShow()
{
    m_bReloadRecordList = true;
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_USB_READING_FILE), MESSAGEBOX_Cancel, -1, "",
                                "", MSG_ID_READING_FILE);
    m_bLoadMinTimeOut = false;
    m_timerLoad.start(LOAD_MIN_TIME);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                QEventLoop::ExcludeSocketNotifiers);
}

bool CDlgUSBBrowseRecord::LoadPageData()
{
    this->setFocus();
    return true;
}

//界面初始化
void CDlgUSBBrowseRecord::InitPageData()
{
    m_pUSBRecordListDelegate = new USBRecordListDelegate();

    if (m_pGeneralFrame != NULL && NULL != m_pUSBRecordListDelegate)
    {
        m_pUSBRecordListDelegate->setParent(this);
        m_pUSBRecordListDelegate->SetItemPat(this);
        m_pUSBRecordListDelegate->BindFramelist(m_pGeneralFrame);
        m_pUSBRecordListDelegate->InitSettingListStyle();
        m_pUSBRecordListDelegate->SetItemParent(this);
        m_pUSBRecordListDelegate->PreCreateItem();
        m_pListDelegate = m_pUSBRecordListDelegate;
    }
    QObject::connect(m_pUSBRecordListDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));
    QObject::connect(&m_timerLoad, SIGNAL(timeout()), this, SLOT(OnLoadTimeOut()));
    QObject::connect(m_pUSBRecordListDelegate, SIGNAL(ItemDeleteAction(CListItem *)), this,
                     SLOT(OnItemDeleteAction(CListItem *)));

    SetRejectReturnToIdle();

    UIManager_RegisterTopWndChangeCallBack(&OnPlayRecordTopWndChange);
}

void CDlgUSBBrowseRecord::OnLoadRecordList()
{
    ReloadRecordList();
    m_bReloadRecordList = false;
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);
    UpdateWnd();
}

void CDlgUSBBrowseRecord::OnItemDeleteAction(CListItem * pItem)
{
    if (NULL != pItem)
    {
        m_pListDelegate->SetCurrentIndex(pItem->GetDataIndex());
    }

    OnAction(ACTION_DELETE);
}

void CDlgUSBBrowseRecord::ReloadRecordList()
{
    // 获取录音文件列表
    m_vecRecordList.clear();
    YLList<RecordFileInfo> pListFileInfo;
    modRecord_GetResourceList(pListFileInfo, RECORD_AUDIO);
    YLList<RecordFileInfo>::ListIterator iter = pListFileInfo.begin();
    for (; iter != pListFileInfo.end(); ++iter)
    {
        m_vecRecordList.push_back(*iter);
    }
}

bool CDlgUSBBrowseRecord::LoadRecordList(int nDataIndex, int nDataCount)
{
    if (m_bReloadRecordList)
    {
        return true;
    }

    if (nDataIndex < 0 || nDataCount < 1)
    {
        return false;
    }

    m_vecWidgetPair.clear();
    for (int i = nDataIndex; i < (nDataIndex + nDataCount) && i < m_vecRecordList.size(); i++)
    {
        QWidgetPair qPair;

        yl::string strDisplayName = m_vecRecordList[i].strName;
        qPair.first = toQString(strDisplayName);

        qPair.strAuthorId = modRecord_TransSizeToString(m_vecRecordList[i].lSize);

        yl::string strFile = m_vecRecordList[i].strPath + m_vecRecordList[i].strName;
        qPair.third = toQString(strFile);

        qPair.iIndex = i;
        m_vecWidgetPair.push_back(qPair);
    }

    if (NULL != m_pListDelegate && m_pListDelegate->inherits("USBRecordListDelegate"))
    {
        USBRecordListDelegate * pUSBRecordListDelegate = static_cast<USBRecordListDelegate *>
                (m_pListDelegate);
        pUSBRecordListDelegate->SetItemsData(m_vecWidgetPair, m_vecRecordList.size());
    }
    return true;
}

bool CDlgUSBBrowseRecord::PorcessHomeBtnClick()
{
    modRecord_PlayStop(RECORD_AUDIO, "");
    emit BtnBackClick();
    UIManager_ReturnToIdle(RETURN_IDLE_TYPE_HOME_PRESS);
    return true;
}

void CDlgUSBBrowseRecord::PlayStatusChange(bool bPlay)
{
    //处于播放状态背光灯长亮,不自动退出界面
    if (bPlay)
    {
        backlight_OpenBacklightEvent(false);
    }
    else
    {
        backlight_OpenBacklightEvent(true);
    }

    emit RefreshSoftkey();
}

void CDlgUSBBrowseRecord::PlayFail()
{
    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USB_RECORDING_NOT_ALLOW_PLAY_RECORD),
                                MESSAGEBOX_NOTE, 2000);
}

void CDlgUSBBrowseRecord::PageChange(bool bHide)
{
    if (NULL != m_pListDelegate && m_pListDelegate->inherits("USBRecordListDelegate"))
    {
        USBRecordListDelegate * pUSBRecordListDelegate = static_cast<USBRecordListDelegate *>
                (m_pListDelegate);
        pUSBRecordListDelegate->PageChange(bHide);
    }
}

bool CDlgUSBBrowseRecord::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
#if 0//产品部要求去掉Delete All
    if (m_pGeneralFrame == NULL
            || m_vecRecordList.size() == 0
            || modRecord_IsPlaying(RECORD_AUDIO)
            || modRecord_IsPlayPause(RECORD_AUDIO))
    {
        objSoftData[0].m_strSoftkeyTitle = "";
        objSoftData[0].m_strSoftkeyAction = "";
        objSoftData[0].m_strIconPath = "";
    }
    else
    {
        objSoftData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
        objSoftData[0].m_strSoftkeyAction = ACTION_DELETEALL;
        objSoftData[0].m_strIconPath = PIC_SOFTKEY_REMOVE;
    }
#endif
    objSoftData[3].m_strSoftkeyTitle = "";
    objSoftData[3].m_strSoftkeyAction = "";

    return true;
}

bool CDlgUSBBrowseRecord::IsStatusChanged()
{
    return false;
}

bool CDlgUSBBrowseRecord::SavePageData()
{
    return true;
}

bool CDlgUSBBrowseRecord::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
        QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = 0;
    iMax = 15;
    return true;
}

bool CDlgUSBBrowseRecord::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject
            || NULL == pEvent)
    {
        return false;
    }

    QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
    switch (pKeyEvent->key())
    {
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
    case PHONE_KEY_STAR:
    case PHONE_KEY_POUND:
        {
            return true;
        }
        break;
    default:
        break;
    }

    if (m_pUSBRecordListDelegate != NULL
            && m_pUSBRecordListDelegate->ProcessKeyEvent(pObject, pEvent))
    {
        return true;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgUSBBrowseRecord::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("CDlgUSBBrowseRecord::OnAction(%s)  iCurrentIndex[%d]", strAction.toUtf8().data(),
                   m_pListDelegate->GetCurrentIndex());
    bool bHandled = true;
    if (strAction == ACTION_DELETE)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_USB_IF_DELETE_AUDIO), MESSAGEBOX_OkCancel, -1,
                                    "", "", MSG_ID_DELETE_FOCUS_RECORD);
    }
    else if (strAction == ACTION_DELETEALL)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_RECORD_DELETE_ALL), MESSAGEBOX_OkCancel, -1,
                                    "", "", MSG_ID_DELETE_ALL_RECORD);
    }
    else if (strAction == ACTION_BACK)
    {
        modRecord_PlayStop(RECORD_AUDIO, "");
        emit BtnBackClick();
    }
    else if (strAction == ACTION_SWITCH_PAGE)
    {
        modRecord_PlayStop(RECORD_AUDIO, "");
        bHandled = false;
    }

    return bHandled;
}

void CDlgUSBBrowseRecord::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    int iMsgID = pMessageBox->GetID();
    MESSAGEBOX_RESULT eResault = pMessageBox->GetResult();

    switch (iMsgID)
    {
    case MSG_ID_DELETE_FOCUS_RECORD:
        {
            if (MessageBox_OK == eResault)
            {
                int iCurrentIndex = m_pListDelegate->GetCurrentIndex();
                RecordFileInfo RecordInfo;
                if (!modRecord_GetResourceByIndex(RecordInfo, iCurrentIndex, RECORD_AUDIO))
                {
                    return ;
                }

                int iLastIndex = m_vecRecordList.size() - 1;
                //删除的是最后一条则焦点上移
                if (iCurrentIndex == iLastIndex)
                {
                    m_pListDelegate->SetCurrentIndex(iCurrentIndex - 1);
                }

                yl::string strFile = RecordInfo.strPath + RecordInfo.strName;
                modRecord_DeleteResoucrce(RECORD_AUDIO, strFile.c_str());
                ReloadRecordList();
                UpdateWnd();
            }
        }
        break;
    case MSG_ID_DELETE_ALL_RECORD:
        {
            if (MessageBox_OK == eResault)
            {
                modRecord_DeleteAll(RECORD_AUDIO);
                m_vecRecordList.clear();
                m_pListDelegate->SetCurrentIndex(-1);
                UpdateWnd();
            }
        }
        break;
    default:
        break;
    }
}
void CDlgUSBBrowseRecord::OnLoadTimeOut()
{
    if (m_timerLoad.isActive())
    {
        m_timerLoad.stop();
    }
    m_bLoadMinTimeOut = true;
    if (!modRecord_IsLoadingFile(RECORD_AUDIO))
    {
        OnLoadRecordList();
    }
}
void CDlgUSBBrowseRecord::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case RECORD_STORAGE_LOAD_FILE_LIST:
        {
            if (objMessage.lParam == 1)
            {
                if (m_bLoadMinTimeOut)
                {
                    OnLoadRecordList();
                }
            }
        }
        break;
    default:
        break;
    }
}

void CDlgUSBBrowseRecord::Init()
{
    etl_RegisterMsgHandle(RECORD_STORAGE_LOAD_FILE_LIST, RECORD_STORAGE_LOAD_FILE_LIST,
                          USBBR_ReceiveingMsg);
}

void CDlgUSBBrowseRecord::Uninit()
{
    etl_UnregisterMsgHandle(RECORD_STORAGE_LOAD_FILE_LIST, RECORD_STORAGE_LOAD_FILE_LIST,
                            USBBR_ReceiveingMsg);
}
