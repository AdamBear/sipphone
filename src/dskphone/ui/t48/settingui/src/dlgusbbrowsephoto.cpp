#include "dlgusbbrowsephoto.h"
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
#include "usbphotolistcontrl.h"
#include "imagemanager/modimagemanager.h"

namespace
{
#define ACTION_DELETE               "delete"
#define ACTION_DELETEALL            "deleteAll"
#define ACTION_BACK                 "Back"
#define ACTION_SWITCH_PAGE          "SwitchPage"
#define LOAD_MIN_TIME               500
#define MSG_ID_READING_FILE         29280
}



static LRESULT USBBP_ReceiveingMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnUSBBrowsePhoto 0x[%x] 0x[%x]",
                   objMessage.message, objMessage.wParam);

    CDlgUSBBrowsePhoto * pUSBBrowsePhoto = qobject_cast<CDlgUSBBrowsePhoto *>
                                           (UIManager_GetPrivateDlg(
                                                   DLG_CDlgUSBBrowsePhoto));
    if (NULL == pUSBBrowsePhoto || !pUSBBrowsePhoto->IsTopPage())
    {
        return 0;
    }

    pUSBBrowsePhoto->OnReceiveMsg(objMessage);
    return 0;
}



CDlgUSBBrowsePhoto::CDlgUSBBrowsePhoto(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_timerLoad(this)
    , m_bLoadMinTimeOut(false)
    , m_pUSBPhotoListDelegate(NULL)
{

    InitPageData();

}

CDlgUSBBrowsePhoto::~CDlgUSBBrowsePhoto()
{
    if (m_timerLoad.isActive())
    {
        m_timerLoad.stop();
    }

    backlight_OpenBacklightEvent(true);

}


void CDlgUSBBrowsePhoto::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgUSBBrowsePhoto::OnShow()
{
    m_bReloadPhotoList = true;
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_USB_READING_FILE), MESSAGEBOX_Cancel, -1, "",
                                "", MSG_ID_READING_FILE);
    m_bLoadMinTimeOut = false;
    m_timerLoad.start(LOAD_MIN_TIME);
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                QEventLoop::ExcludeSocketNotifiers);
}

bool CDlgUSBBrowsePhoto::LoadPageData()
{
    this->setFocus();
    return true;
}

//界面初始化
void CDlgUSBBrowsePhoto::InitPageData()
{
    m_pUSBPhotoListDelegate = new USBPhotoListDelegate();

    if (m_pGeneralFrame != NULL && NULL != m_pUSBPhotoListDelegate)
    {
        m_pUSBPhotoListDelegate->setParent(this);
        m_pUSBPhotoListDelegate->SetItemPat(this);
        m_pUSBPhotoListDelegate->BindFramelist(m_pGeneralFrame);
        m_pUSBPhotoListDelegate->InitSettingListStyle();
        m_pUSBPhotoListDelegate->SetItemParent(this);
        m_pUSBPhotoListDelegate->PreCreateItem();
        m_pListDelegate = m_pUSBPhotoListDelegate;
    }
    QObject::connect(m_pUSBPhotoListDelegate, SIGNAL(listItemAction(int, QString)), this,
                     SLOT(OnListItemAction(int, QString)));
    QObject::connect(&m_timerLoad, SIGNAL(timeout()), this, SLOT(OnLoadTimeOut()));
    QObject::connect(m_pUSBPhotoListDelegate, SIGNAL(ItemDeleteAction(CListItem *)), this,
                     SLOT(OnItemDeleteAction(CListItem *)));

}

void CDlgUSBBrowsePhoto::OnLoadPhotoList()
{
    ReloadPhotoList();
    m_bReloadPhotoList = false;
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);
    UpdateWnd();
}

void CDlgUSBBrowsePhoto::OnItemDeleteAction(CListItem * pItem)
{

    OnAction(ACTION_DELETE);
}

void CDlgUSBBrowsePhoto::ReloadPhotoList()
{
    // 获取录音文件列表
    g_pUSBPhotoContrl->m_vecRecordList.clear();
    YLList<RecordFileInfo> pListFileInfo;
    modRecord_GetResourceList(pListFileInfo, RECORD_PICTURE);
    YLList<RecordFileInfo>::ListIterator iter = pListFileInfo.begin();
    for (; iter != pListFileInfo.end(); ++iter)
    {
        g_pUSBPhotoContrl->m_vecRecordList.push_back(*iter);
    }
}

bool CDlgUSBBrowsePhoto::LoadPhotoList(int nDataIndex, int nDataCount)
{
    if (m_bReloadPhotoList)
    {
        return true;
    }

    if (nDataIndex < 0 || nDataCount < 1)
    {
        return false;
    }

    m_vecWidgetPair.clear();
    for (int i = nDataIndex; i < (nDataIndex + nDataCount) && i < g_pUSBPhotoContrl->m_vecRecordList.size(); i++)
    {
        QWidgetPair qPair;

        yl::string strDisplayName = g_pUSBPhotoContrl->m_vecRecordList[i].strName;
        qPair.first = toQString(strDisplayName);

        qPair.strAuthorId = modRecord_TransSizeToString(g_pUSBPhotoContrl->m_vecRecordList[i].lSize);

        yl::string strFile = g_pUSBPhotoContrl->m_vecRecordList[i].strPath + g_pUSBPhotoContrl->m_vecRecordList[i].strName;
        qPair.third = toQString(strFile);

        qPair.iIndex = i;
        m_vecWidgetPair.push_back(qPair);
    }

    if (NULL != m_pListDelegate && m_pListDelegate->inherits("USBPhotoListDelegate"))
    {
        USBPhotoListDelegate * pUSBPhotoListDelegate = static_cast<USBPhotoListDelegate *>
                (m_pListDelegate);
        pUSBPhotoListDelegate->SetItemsData(m_vecWidgetPair, g_pUSBPhotoContrl->m_vecRecordList.size());
    }
    return true;
}


bool CDlgUSBBrowsePhoto::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
#if 0//产品部要求去掉Delete All
    if (m_pGeneralFrame == NULL
            || g_pUSBPhotoContrl->m_vecRecordList.size() == 0
            || modRecord_IsPlaying(RECORD_PICTURE)
            || modRecord_IsPlayPause(RECORD_PICTURE))
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

bool CDlgUSBBrowsePhoto::IsStatusChanged()
{
    return false;
}

bool CDlgUSBBrowsePhoto::SavePageData()
{
    return true;
}

bool CDlgUSBBrowsePhoto::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject
            || NULL == pEvent)
    {
        return false;
    }

    QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
    switch (pKeyEvent->key())
    {
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


    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgUSBBrowsePhoto::OnAction(const QString & strAction)
{
    SETTINGUI_INFO("CDlgUSBBrowsePhoto::OnAction(%s)  iCurrentIndex[%d]", strAction.toUtf8().data(),
                   m_pUSBPhotoListDelegate->GetCurrentIndex());
    if (strAction == ACTION_DELETE)
    {
        g_pUSBPhotoContrl->DeleteCurrentIndex();
    }
    else if (strAction == ACTION_DELETEALL)
    {
        g_pUSBPhotoContrl->DeleteCurrentIndex(true);
    }
    else if (strAction == ACTION_ENTER)
    {
        emit openpage("OpenSubPage(browse_usb_photo_detail)");
    }
    else
    {
        return false;
    }

    return true;
}

void CDlgUSBBrowsePhoto::OnLoadTimeOut()
{
    if (m_timerLoad.isActive())
    {
        m_timerLoad.stop();
    }
    m_bLoadMinTimeOut = true;
    if (!modRecord_IsLoadingFile(RECORD_PICTURE))
    {
        OnLoadPhotoList();
    }
}
void CDlgUSBBrowsePhoto::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case RECORD_STORAGE_LOAD_FILE_LIST:
        {
            if (objMessage.lParam == 1 && objMessage.wParam == RECORD_PICTURE)
            {
                if (m_bLoadMinTimeOut)
                {
                    OnLoadPhotoList();
                }
            }
        }
        break;
    default:
        break;
    }
}

void CDlgUSBBrowsePhoto::Init()
{
    etl_RegisterMsgHandle(RECORD_STORAGE_LOAD_FILE_LIST, RECORD_STORAGE_LOAD_FILE_LIST,
                          USBBP_ReceiveingMsg);
}


void CDlgUSBBrowsePhoto::UpdateWnd()
{
    if (NULL == m_pUSBPhotoListDelegate)
    {
        return;
    }
    m_pUSBPhotoListDelegate->UpdateList();
}

void CDlgUSBBrowsePhoto::Uninit()
{
    etl_UnregisterMsgHandle(RECORD_STORAGE_LOAD_FILE_LIST, RECORD_STORAGE_LOAD_FILE_LIST,
                            USBBP_ReceiveingMsg);
    //清空缓存
    Image_ClearLRUCache();
}
