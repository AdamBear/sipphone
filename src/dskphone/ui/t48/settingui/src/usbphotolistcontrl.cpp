#include "usbphotolistcontrl.h"
#include "dlgusbbrowsephoto.h"
#include "settingui/src/settingitemfactory.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "dsklog/log.h"
#include "dlgusbphotodetail.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "setting/src/displaycontroller.h"
#include "uicommon/messagebox/optionmessagebox.h"
#include "actiondefinecommon.h"
#include "uicommon/messagebox/selectmessagebox.h"
#include "imagemanager/modimagemanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "record/include/recordmessagedefine.h"

#define MSG_ID_DELETE_FOCUS_RECORD  29281
#define MSG_ID_DELETE_ALL_RECORD    29282
#define MSG_ID_AFTER_DELETE_NOTE    29283
#define MSG_ID_CHANGE_PHOTO         29284
#define MSG_ID_SET_AS               29285
#define MSG_ID_ON_SET_AS            29286
#define MSG_ID_REPLACE              29287
#define MSG_ID_ON_REPLACE              29288

LRESULT OnWallPaperSetMessage(msgObject & refMessage);

LRESULT OnWallPaperSetMessage(msgObject & refMessage)
{
    CDlgUSBPhotoDetail * pUSBPhotoDetail = qobject_cast<CDlgUSBPhotoDetail *>
                                           (UIManager_GetPrivateDlg(
                                                   DLG_CDlgUSBPhotoDetail));
    if (NULL != pUSBPhotoDetail && pUSBPhotoDetail->IsTopPage())
    {

        switch (refMessage.message)
        {
        case RECORD_SET_DEFAULT_WALLPAPER:
        case RECORD_SET_SCREENSAVER:
        {
            MessageBox_RemoveAllMessageBox(MessageBox_Destory);
            switch (refMessage.wParam)
            {
            case PIC_SET_RST_SUCCESS:
            {
                switch (g_pUSBPhotoContrl->m_eUSBPicSetType)
                {
                case USB_PIC_SET_TYPE_WALLPAPER:
                {
                    MessageBox_ShowCommonMsgBox(NULL,
                                                LANG_TRANSLATE(TRID_SET_AS_WALLPAPER_SUCCESS),
                                                MESSAGEBOX_NOTE, SHORT_TIME);
                }
                break;

                case USB_PIC_SET_TYPE_SCRRENSAVER:
                {
                    MessageBox_ShowCommonMsgBox(NULL,
                                                LANG_TRANSLATE(TRID_SET_AS_SCREENCAPTURE_SUCCESS),
                                                MESSAGEBOX_NOTE, SHORT_TIME);
                }
                break;

                case USB_PIC_SET_TYPE_DSSKEYWALLPAPER:
                {
                    MessageBox_ShowCommonMsgBox(NULL,
                                                LANG_TRANSLATE(TRID_SET_AS_DSSKEYWALLPAPER_SUCCESS),
                                                MESSAGEBOX_NOTE, SHORT_TIME);
                }
                break;
                default:
                    break;
                }
            }
            break;
            //源文件不存在
            case PIC_SET_RST_FILE_NOT_EXIST:
            {
                MessageBox_ShowCommonMsgBox(NULL,
                                            LANG_TRANSLATE(TRID_SET_RST_FILE_NOT_EXIST), MESSAGEBOX_NOTE, SHORT_TIME);
            }
            break;
            //源文件太大
            case PIC_SET_RST_FILE_TOO_LARGE:
            {
                MessageBox_ShowCommonMsgBox(NULL,
                                            LANG_TRANSLATE(TRID_PIC_SET_RST_FILE_TOO_LARGE), MESSAGEBOX_NOTE, SHORT_TIME);
            }
            break;
            //分区容量不足
            case PIC_SET_RST_PARTITION_NOT_ENOUGH:
            {
                MessageBox_ShowCommonMsgBox(NULL,
                                            LANG_TRANSLATE(TRID_SET_RST_PARTITION_NOT_ENOUGH), MESSAGEBOX_NOTE, SHORT_TIME);
            }
            break;
            default:
            {
                MessageBox_ShowCommonMsgBox(NULL,
                                            LANG_TRANSLATE(TRID_SET_RST_ERROR), MESSAGEBOX_NOTE, SHORT_TIME);
            }
            break;
            }
        }
        break;
        default:
        {
            //
        }
        break;
        }

    }
    return 0;
}

USBPhotoListContrl::USBPhotoListContrl()
    : m_iCurrentIndex(0)
    , m_strWallPaperPath("")
    , m_eUSBPicSetType(USB_PIC_SET_TYPE_WALLPAPER)
    , m_pUSBSelector(NULL)
    , m_strWallPaperName("")
{
    m_pUSBSelector = new CUSBPhotoSelector;
    etl_RegisterMsgHandle(RECORD_SET_DEFAULT_WALLPAPER, RECORD_SET_SCREENSAVER, OnWallPaperSetMessage);
}

USBPhotoListContrl::~USBPhotoListContrl()
{
    m_iCurrentIndex = 0;
    if (m_pUSBSelector != NULL)
    {
        delete m_pUSBSelector;
        m_pUSBSelector = NULL;
    }
    etl_UnregisterMsgHandle(RECORD_SET_DEFAULT_WALLPAPER, RECORD_SET_SCREENSAVER, OnWallPaperSetMessage);
}

USBPhotoListContrl * USBPhotoListContrl::CreateInstance()
{
    static USBPhotoListContrl * m_pTomyself = new USBPhotoListContrl();
    return m_pTomyself;
}

void USBPhotoListContrl::SetCurrentIndex(int iIndex)
{
    m_iCurrentIndex = iIndex;
}

void USBPhotoListContrl::DeleteCurrentIndex(bool bDeleteAll/* = false*/)
{
    if (!bDeleteAll)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_USB_IF_DELETE_PHOTO), MESSAGEBOX_OkCancel, -1,
                                    "", "", MSG_ID_DELETE_FOCUS_RECORD);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_PHOTO_DELETE_ALL), MESSAGEBOX_OkCancel, -1,
                                    "", "", MSG_ID_DELETE_ALL_RECORD);
    }
}


void USBPhotoListContrl::ReloadPhotoList()
{
    // 获取录音文件列表
    m_vecRecordList.clear();
    YLList<RecordFileInfo> pListFileInfo;
    modRecord_GetResourceList(pListFileInfo, RECORD_PICTURE);
    YLList<RecordFileInfo>::ListIterator iter = pListFileInfo.begin();
    for (; iter != pListFileInfo.end(); ++iter)
    {
        m_vecRecordList.push_back(*iter);
    }
}


void USBPhotoListContrl::OnChangePhoto(int iIndex)
{
    SetCurrentIndex(iIndex);
    CDlgUSBPhotoDetail * pUSBPhotoDetail = qobject_cast<CDlgUSBPhotoDetail *>
                                           (UIManager_GetPrivateDlg(
                                                   DLG_CDlgUSBPhotoDetail));
    if (NULL != pUSBPhotoDetail && pUSBPhotoDetail->IsTopPage())
    {

        QPixmap pixCurrent;
        //是否存在内存
        if (Image_IsExistInLRUCache(pUSBPhotoDetail->GetCurrentPicture().c_str(), pixCurrent))
        {
            pUSBPhotoDetail->OnLoadPicture();
            return;
        }
    }
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_USB_READING_FILE),
                                MESSAGEBOX_NOTE, 500, "", "", MSG_ID_CHANGE_PHOTO);
}


void USBPhotoListContrl::SetAs(const yl::string & strWallPaperPath, const yl::string & strWallPaperName)
{
    if (NULL == m_pUSBSelector)
    {
        return;
    }

    m_strWallPaperPath = strWallPaperPath;
    m_strWallPaperName = strWallPaperName;


    QString strTitle = LANG_TRANSLATE(TRID_SET_AS);

    //更换弹出框
    MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey skOk(LANG_TRANSLATE(TRID_OK), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(skCancel);
    vecSoftKey.push_back(skOk);
    MessageBox_ShowSelectBox(this, vecSoftKey, SELECT_POP_BOX_TYPE_NORMAL, m_pUSBSelector, strTitle, MSG_ID_SET_AS);
}

int USBPhotoListContrl::GetCurrentIndex()
{
    return m_iCurrentIndex;
}

void USBPhotoListContrl::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    int iMsgID = pMessageBox->GetID();
    MESSAGEBOX_RESULT eResault = pMessageBox->GetResult();

    switch (iMsgID)
    {
    case MSG_ID_DELETE_FOCUS_RECORD:
    {
        if (MessageBox_OK == eResault)
        {
            int iCurrentIndex = GetCurrentIndex();
            RecordFileInfo RecordInfo;
            if (!modRecord_GetResourceByIndex(RecordInfo, iCurrentIndex, RECORD_PICTURE))
            {
                return;
            }

            int iLastIndex = m_vecRecordList.size() - 1;
            if (iCurrentIndex == iLastIndex)
            {
                SetCurrentIndex(iCurrentIndex - 1);
            }

            yl::string strFile = RecordInfo.strPath + RecordInfo.strName;
            if (modRecord_DeleteResoucrce(RECORD_PICTURE, strFile.c_str()))
            {
                ReloadPhotoList();
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DELETE_SUCCESSED),
                                            MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                            MSG_ID_AFTER_DELETE_NOTE);

            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_USB_DELETIE_FILE_FAIL),
                                            MESSAGEBOX_NOTE, SHORT_TIME);
            }

        }
    }
    break;
    case MSG_ID_DELETE_ALL_RECORD:
    {
        if (MessageBox_OK == eResault)
        {
            modRecord_DeleteAll(RECORD_PICTURE);
            m_vecRecordList.clear();
            SetCurrentIndex(-1);
            CDlgUSBBrowsePhoto * pUSBBrowsePhoto = qobject_cast<CDlgUSBBrowsePhoto *>
                                                   (UIManager_GetPrivateDlg(
                                                           DLG_CDlgUSBBrowsePhoto));
            if (NULL != pUSBBrowsePhoto && pUSBBrowsePhoto->IsTopPage())
            {
                pUSBBrowsePhoto->UpdateWnd();
            }
        }
    }
    break;
    case MSG_ID_AFTER_DELETE_NOTE:
    {
        CDlgUSBBrowsePhoto * pUSBBrowsePhoto = qobject_cast<CDlgUSBBrowsePhoto *>
                                               (UIManager_GetPrivateDlg(
                                                       DLG_CDlgUSBBrowsePhoto));
        CDlgUSBPhotoDetail * pUSBPhotoDetail = qobject_cast<CDlgUSBPhotoDetail *>
                                               (UIManager_GetPrivateDlg(
                                                       DLG_CDlgUSBPhotoDetail));
        if (NULL != pUSBBrowsePhoto && pUSBBrowsePhoto->IsTopPage())
        {
            pUSBBrowsePhoto->UpdateWnd();
        }
        if (NULL != pUSBPhotoDetail && pUSBPhotoDetail->IsTopPage())
        {
            pUSBPhotoDetail->LoadPageData();
        }
    }
    break;
    case MSG_ID_CHANGE_PHOTO:
    {
        CDlgUSBPhotoDetail * pUSBPhotoDetail = qobject_cast<CDlgUSBPhotoDetail *>
                                               (UIManager_GetPrivateDlg(
                                                       DLG_CDlgUSBPhotoDetail));
        if (NULL != pUSBPhotoDetail && pUSBPhotoDetail->IsTopPage())
        {
            pUSBPhotoDetail->OnLoadPicture();
        }
    }
    break;
    case MSG_ID_SET_AS:
    {
        if (pMessageBox->GetResult() == MessageBox_OK)
        {
            m_eUSBPicSetType = m_pUSBSelector->m_eSelIndex;
            if (!FileIsExist(m_eUSBPicSetType))
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_MODIFY_CONFIG),
                                            MESSAGEBOX_NOTE, 500, "", "", MSG_ID_ON_SET_AS);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_FILE_IS_EXIST_REPLACE),
                                            MESSAGEBOX_OkCancel, -1, "", "",
                                            MSG_ID_REPLACE);
            }
        }
    }
    break;
    case MSG_ID_ON_SET_AS:
    case MSG_ID_ON_REPLACE:
    {
        OnSetAs();
    }
    break;
    case MSG_ID_REPLACE:
    {
        if (pMessageBox->GetResult() == MessageBox_OK)
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_MODIFY_CONFIG),
                                        MESSAGEBOX_NOTE, 500, "", "", MSG_ID_ON_REPLACE);
        }
    }
    break;
    default:
        break;
    }
}

bool USBPhotoListContrl::FileIsExistInList(WallPaperListData & lstData, yl::string strName)
{
    //取得list迭代指针开始
    YLList<WallPaperData>::iterator iter =
        lstData.m_listPaperData.begin();
    //取得该list链表标识
    YLList<WallPaperData>::iterator listEnd =
        lstData.m_listPaperData.end();

    for (; iter != listEnd; iter++)
    {
        WallPaperData & refData = *iter;
        if (refData.m_strPaperDisplayName == strName)
        {
            return true;
        }
    }

    return false;
}

bool USBPhotoListContrl::FileIsExist(USB_PIC_SET_TYPE nSelec)
{
    bool bResult = false;
    WallPaperListData lstData;


    if (nSelec == USB_PIC_SET_TYPE_WALLPAPER)
    {
        g_pDisplayController->GetWallpaperListData(&lstData);
        if (FileIsExistInList(lstData, m_strWallPaperName.c_str()))
        {
            bResult = true;
        }
    }
    else if (nSelec == USB_PIC_SET_TYPE_SCRRENSAVER)
    {
        g_pDisplayController->GetScreenSaverListData(&lstData);
        if (FileIsExistInList(lstData, m_strWallPaperName.c_str()))
        {
            bResult = true;
        }
    }
    else if (nSelec == USB_PIC_SET_TYPE_DSSKEYWALLPAPER)
    {
        g_pDisplayController->GetWallpaperListData(&lstData);
        if (FileIsExistInList(lstData, m_strWallPaperName.c_str()))
        {
            bResult = true;
        }
    }

    return bResult;
}

void USBPhotoListContrl::OnSetAs()
{
    switch (m_eUSBPicSetType)
    {
    case USB_PIC_SET_TYPE_WALLPAPER:
    {

        modRecord_SetDefaultWallPaper(m_strWallPaperPath.c_str());
    }
    break;
    case USB_PIC_SET_TYPE_SCRRENSAVER:
    {
        modRecord_SetScreenSaver(m_strWallPaperPath.c_str());
    }
    break;
    case USB_PIC_SET_TYPE_DSSKEYWALLPAPER:
    {

        modRecord_SetDsskeyWallPaper(m_strWallPaperPath.c_str());

    }
    break;
    default:
        break;
    }
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_MODIFY_CONFIG),
                                MESSAGEBOX_NOTE);

}

