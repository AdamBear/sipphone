#include "dlgusbphoto.h"

#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "xwindow/xWindow.h"
#include "settingui/include/pageiddefine.h"
#include "imagecache/include/modimagecache.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/modbaseui.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "settingui/include/modsettingui.h"

#include "settinguilogic/src/displaycontroller.h"
#include "record/include/modrecord.h"
#include "record/include/recordmessagedefine.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace
{
#define  DLG_USB_PHOTO_XML          "setting/dlgusbphoto.xml"
#define  PIC_PHOTO                  "picPhoto"
enum
{
    MSG_ID_REPLACE = 160,
    MSG_ID_MODIFY_CONFIG,
    MSG_ID_READING_FILE,
};
}

CBaseDialog* CDlgUsbPhoto::CreateInstance()
{
    CDlgUsbPhoto* pDlg = new CDlgUsbPhoto;
    if (NULL != pDlg)
    {
        pDlg->loadContent(DLG_USB_PHOTO_XML);
    }
    return pDlg;
}

CDlgUsbPhoto::CDlgUsbPhoto()
    : m_pImgPhoto(NULL)
    , m_nPhotoIndex(-1)
{
    etl_RegisterMsgHandle(RECORD_SET_DEFAULT_WALLPAPER, RECORD_SET_SCREENSAVER,
                          &CDlgUsbPhoto::OnRecordMsg);
    etl_RegisterMsgHandle(MSG_IMAGE_MESSAGE_LOAD_FINISH, MSG_IMAGE_MESSAGE_LOAD_FINISH,
                          &CDlgUsbPhoto::OnRecordMsg);
}

CDlgUsbPhoto::~CDlgUsbPhoto()
{
    etl_UnregisterMsgHandle(RECORD_SET_DEFAULT_WALLPAPER, RECORD_SET_SCREENSAVER,
                            &CDlgUsbPhoto::OnRecordMsg);
    etl_UnregisterMsgHandle(MSG_IMAGE_MESSAGE_LOAD_FINISH, MSG_IMAGE_MESSAGE_LOAD_FINISH,
                            &CDlgUsbPhoto::OnRecordMsg);
    Image_ClearImage(IMG_CLASSIFY_USB_PHOTO);
}

void CDlgUsbPhoto::loadChildrenElements(xml_node& node)
{
    CDlgSettingUI::loadChildrenElements(node);
    m_pImgPhoto = static_cast<xImageView*>(getViewById(PIC_PHOTO));
}

void CDlgUsbPhoto::Init()
{

}

void CDlgUsbPhoto::RefreshUI(bool bReLoadData /*= false*/)
{
    if (NULL == m_pImgPhoto)
    {
        return;
    }

    SetCommonSoftkey();
    RefreshTitle(m_stPhotoInfo.strName);

    yl::string strPhotoPath = m_stPhotoInfo.strPath + m_stPhotoInfo.strName;

    chSize szImage;
    if (!xPixmap::GetImageSize(strPhotoPath, szImage))
    {
        return;
    }

    if (0 >= szImage.cx || 0 >= szImage.cy)
    {
        g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE),
                                               TRID_LOAD_IMAGE_FAILURE,
                                               SETTING_MESSAGEBOX_NOTE, "", "",
                                               SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
    }
    else if (szImage.cx * szImage.cy > MAX_PICTURE_RESOLUTION_SIZE)
    {
        g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE),
                                               TRID_PIC_SET_RST_FILE_TOO_LARGE,
                                               SETTING_MESSAGEBOX_NOTE, "", "",
                                               SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
    }
    else
    {
        xPixmap pix;

        bool bSuccess = Image_GetImage(pix, strPhotoPath, IMG_CLASSIFY_USB_PHOTO);
        if (!pix.isNull())
        {
#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
            m_pImgPhoto->setPixmap(pix, true);
#else
            m_pImgPhoto->setPixmap(pix);
#endif
        }
        else
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE),
                                                   _LANG(TRID_USB_READING_FILE),
                                                   SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                                   MSG_ID_READING_FILE, NULL, this);
        }
    }
}

bool CDlgUsbPhoto::onKeyPressedEvent(int key)
{
    switch (key)
    {
    case PHONE_KEY_LEFT:
    case PHONE_KEY_UP:
        {
            OnPreviousImage();
            return true;
        }
        break;
    case PHONE_KEY_RIGHT:
    case PHONE_KEY_DOWN:
        {
            OnNextImage();
            return true;
        }
        break;
    default:
        break;
    }

    return CDlgSettingUI::onKeyPressedEvent(key);
}

LRESULT CDlgUsbPhoto::OnRecordMsg(msgObject& msg)
{
    if (RECORD_SET_DEFAULT_WALLPAPER != msg.message
            && RECORD_SET_DEFAULT_WALLPAPER != msg.message
            && RECORD_SET_DEFAULT_WALLPAPER != msg.message
            && MSG_IMAGE_MESSAGE_LOAD_FINISH != msg.message)
    {
        return FALSE;
    }

    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    if (MSG_IMAGE_MESSAGE_LOAD_FINISH == msg.message)
    {
        if (1 == msg.wParam && IMG_CLASSIFY_USB_PHOTO == msg.lParam)
        {
            g_SetUICallBackFun.RefreshUI(false);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    switch (msg.wParam)
    {
    //成功
    case PIC_SET_RST_SUCCESS:
        {
            yl::string strNote;
            switch (msg.message)
            {
            case RECORD_SET_DEFAULT_WALLPAPER:
                {
                    strNote = TRID_SET_AS_WALLPAPER_SUCCESS;
                }
                break;
            case RECORD_SET_EXP_WALLPAPER:
                {
                    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_EXP_WALLPAPER, 0);
                    strNote = TRID_SET_AS_EXPWALLPAPER_SUCCESS;
                }
                break;
            case RECORD_SET_SCREENSAVER:
                {
                    strNote = TRID_SET_AS_SCREENCAPTURE_SUCCESS;
                }
                break;
            default:
                break;
            }
            //设置壁纸、拓展台壁纸、屏保成功后清除缓存那张图片  防止与缓存文件同名读取失败
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), strNote,
                                                   SETTING_MESSAGEBOX_NOTE, "", "",
                                                   SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        break;
    //源文件不存在
    case PIC_SET_RST_FILE_NOT_EXIST:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), _LANG(TRID_SET_RST_FILE_NOT_EXIST),
                                                   SETTING_MESSAGEBOX_NOTE, "", "",
                                                   SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        break;
    //源文件太大
    case PIC_SET_RST_FILE_TOO_LARGE:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), _LANG(TRID_PIC_SET_RST_FILE_TOO_LARGE),
                                                   SETTING_MESSAGEBOX_NOTE, "", "",
                                                   SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        break;
    //分区容量不足
    case PIC_SET_RST_PARTITION_NOT_ENOUGH:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), _LANG(TRID_SET_RST_PARTITION_NOT_ENOUGH),
                                                   SETTING_MESSAGEBOX_NOTE, "", "",
                                                   SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        break;
    default:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), _LANG(TRID_SET_RST_ERROR),
                                                   SETTING_MESSAGEBOX_NOTE, "", "",
                                                   SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME);
        }
        break;
    }

    return TRUE;
}

bool CDlgUsbPhoto::OnSoftkeyAction(int iType, bool bLongPress)
{
    switch (iType)
    {
    case ST_PREV:
        {
            OnPreviousImage();
        }
        break;
    case ST_NEXT:
        {
            OnNextImage();
        }
        break;
    case ST_OPTION:
        {
            ShowOption();
        }
        break;
    default:
        break;
    }

    return true;
}

void CDlgUsbPhoto::InitExtraData(void* pData)
{
    if (NULL == pData)
    {
        return;
    }
    m_nPhotoIndex = *((int*)pData);
    LoadPhotoInfo();
}

void CDlgUsbPhoto::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox || MESSAGEBOX_SELECTBOX != pMessageBox->GetType()
            || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    int nType = pMessageBox->GetType();
    if (MESSAGEBOX_SELECTBOX == nType)
    {
        CSelectMessageBox* pOptionMsgBox = (CSelectMessageBox*)pMessageBox;
        int nAction = pOptionMsgBox->GetFcousAction();

        if (FileIsExist(nAction))
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), _LANG(TRID_FILE_IS_EXIST_REPLACE),
                                                   SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                   MSG_ID_REPLACE, (void*)(&nAction), this);
        }
        else
        {
            OnSetAction(nAction);
        }

        return;
    }

    int nID = pMessageBox->GetID();
    void* pExtraData = pMessageBox->GetExtraData();
    if (NULL == pExtraData || MSG_ID_REPLACE != nID)
    {
        return;
    }

    int nAction = *((int*)pExtraData);
    OnSetAction(nAction);
}

void CDlgUsbPhoto::ShowOption()
{
    VEC_OPTION_ITEM_DATA vecOption;
    ST_OPTION_ITEM_DATA stOption;

    stOption.m_nAction = OPTION_SET_AS_WALLPAPER;
    stOption.m_strTitle = _LANG(TRID_SET_AS_WALLPAPER);
    vecOption.push_back(stOption);

    stOption.m_nAction = OPTION_SET_AS_SCREENSAVER;
    stOption.m_strTitle = _LANG(TRID_SET_AS_SCREENCAPTURE);
    vecOption.push_back(stOption);

    /*获取已连接拓展台个数的接口*/
    if (exp_GetExpCounts(exp_GetExpTypeByIndex(0)) > 0)
    {
        stOption.m_nAction = OPTION_SET_AS_EXPWALLPAPER;
        stOption.m_strTitle = _LANG(TRID_SET_AS_EXPWALLPAPER);
        vecOption.push_back(stOption);
    }

    BaseUI_ShowOptionMenu(this, vecOption);
}

void CDlgUsbPhoto::LoadPhotoInfo()
{
    modRecord_GetResourceByIndex(m_stPhotoInfo, m_nPhotoIndex, RECORD_PICTURE);
}

bool CDlgUsbPhoto::FileIsExist(int nAction)
{
    bool bResult = false;
    LIST_WALLPAPER_DATA listData;

    switch (nAction)
    {
    case OPTION_SET_AS_WALLPAPER:
        {
            g_pDisplayController->GetWallpaperListData(listData);

        }
        break;
    case OPTION_SET_AS_SCREENSAVER:
        {
            g_pDisplayController->GetScreenSaverListData(listData);
        }
        break;
    case OPTION_SET_AS_EXPWALLPAPER:
        {
            g_pDisplayController->GetExpWallpaperListData(listData);
        }
        break;
    default:
        break;
    }

    yl::string strPhotoPath = m_stPhotoInfo.strPath + m_stPhotoInfo.strName;
    if (FileIsExistInList(listData, strPhotoPath))
    {
        bResult = true;
    }

    return bResult;
}

bool CDlgUsbPhoto::FileIsExistInList(LIST_WALLPAPER_DATA& listData, const yl::string& strName)
{
    //取得list迭代指针开始
    LIST_WALLPAPER_DATA::const_iterator iter = listData.begin();
    //取得该list链表标识
    LIST_WALLPAPER_DATA::const_iterator listEnd = listData.end();

    for (; iter != listEnd; ++iter)
    {
        const ST_WallPaperData& refData = *iter;
        if (strName == refData.m_strPaperDisplayName)
        {
            return true;
        }
    }

    return false;
}

void CDlgUsbPhoto::OnSetAction(int nAction)
{
    // 提醒正在修改配置
    g_SetUICallBackFun.PopCommonMessageBox(kszPageIDUsbPhotoBrowser, _LANG(TRID_NOTE), _LANG(TRID_MODIFY_CONFIG),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                           MSG_ID_MODIFY_CONFIG, this);

    yl::string strPhotoPath = m_stPhotoInfo.strPath + m_stPhotoInfo.strName;
    switch (nAction)
    {
    case OPTION_SET_AS_WALLPAPER:
        {
            modRecord_SetDefaultWallPaper(strPhotoPath.c_str());
        }
        break;
    case OPTION_SET_AS_SCREENSAVER:
        {
            modRecord_SetScreenSaver(strPhotoPath.c_str());
        }
        break;
    case OPTION_SET_AS_EXPWALLPAPER:
        {
            modRecord_SetExpWallPaper(strPhotoPath.c_str());
        }
        break;
    default:
        break;
    }
}

void CDlgUsbPhoto::SetCommonSoftkey()
{
    CArraySoftKeyBarData vecSoftKey;
    CSoftKeyBarData objSoftkeyData;

    objSoftkeyData.m_iType = ST_BACK;
    objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_BACK);
    vecSoftKey.push_back(objSoftkeyData);


    //获取的图片的数量大于1则可以切换
    if (modRecord_GetRecordFileNum(RECORD_PICTURE) > 1)
    {
        objSoftkeyData.m_iType = ST_PREV;
        objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_PREVIOUS);
        vecSoftKey.push_back(objSoftkeyData);

        objSoftkeyData.m_iType = ST_NEXT;
        objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_NEXT);
        vecSoftKey.push_back(objSoftkeyData);

        objSoftkeyData.m_iType = ST_OPTION;
        objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_SET_AS);
        vecSoftKey.push_back(objSoftkeyData);
    }
    else
    {
        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";
        vecSoftKey.push_back(objSoftkeyData);

        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";
        vecSoftKey.push_back(objSoftkeyData);

        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";
        vecSoftKey.push_back(objSoftkeyData);
    }

    SetSoftKey(vecSoftKey);
}

void CDlgUsbPhoto::OnPreviousImage()
{
    OnSwitchImage(false);
}

void CDlgUsbPhoto::OnNextImage()
{
    OnSwitchImage(true);
}

void CDlgUsbPhoto::OnSwitchImage(bool bNext)
{
    int nTotal = modRecord_GetRecordFileNum(RECORD_PICTURE);
    SettingUI_GetSwitchIndex(nTotal, m_nPhotoIndex, bNext, m_nPhotoIndex);
    LoadPhotoInfo();
    g_SetUICallBackFun.RefreshUI(false);
}

#endif

