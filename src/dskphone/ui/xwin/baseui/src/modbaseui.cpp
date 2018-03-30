#include "modbaseui.h"
#include "xwindow/xWindowManager.h"
#include "listviewitemfactory.h"
#include "xdatetimeview.h"
#include "t2xstyle.h"
#include "itemfocusstyle.h"
#include "comboboxlistitem.h"
#include "detaillistitem.h"
#include "editlistitem.h"
#include "twolineeditlistitem.h"
#include "menulistitem.h"
#include "textstatusitem.h"
#include "radiolistitem.h"
#include "selectitem.h"
#include "customdetaillistitem.h"
#include "messagebox/messageboxmanager.h"
#include "imeedit/ximeedit.h"
#include "inputmethod.h"
#include "baseui/include/fullscreenmessagebox.h"
#include <baseui/include/baseuidefine.h>
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "language/input/include/modInput.h"

#include "talkui/src/searchwidgetitem.h"
#include "talkui/src/confmanageritem.h"
#include "talkui/src/talknormalitem.h"
#include "xwindow/xtableviewstyle.h"
#include "imagecache/include/modimagecache.h"
#if IS_COLOR
#include "baseui/include/t2xpicpath.h"
#endif


#if IS_COLOR
namespace
{
#define T29_BACKGROUND_COLOR xColor(5, 70, 92)
#define T5X_BACKGROUND_COLOR xColor(255, 255, 255)
}
#endif

void ReleaseListViewItem(xListViewItemPtr pItem)
{
    g_ItemFactory.ReleaseItem(pItem);
}

void BaseUI_Init()
{
#if IS_COLOR
    BaseUI_ImageInit();
#endif

    xEditTextView::SetFunInstallIME(&IME_InstallIME);
    CImeTarget::SetCreateInputerFun(&IME_CreateInputer);
    CImeTarget::SetReleaseInputerFun(&IME_ReleaseInputer);

    xListViewItem::SetFunFreeItem(&ReleaseListViewItem);
    xDateTimeView::RegisterDataTimeView();

    InitCommonStyle();
#if !IS_COLOR
    InitT2xStyle();
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    CItemImageBGPainter::Init();
    xListViewStyle::SetDefaultItemBGPainter("CItemImageBGPainter");
    COptionMenuItemBG::Init();
    CDialSearchItemBG::Init();
#endif

#if IF_TABLE_VIEW_ENABLE
    xTableViewStyle::RegisterStyle();
#endif

    g_ItemFactory.RegisterItem(CComboBoxListItem::GetComboBoxListItemType(),
                               &CComboBoxListItem::CreateComboBoxListItem);
    g_ItemFactory.RegisterItem(CDetailListItem::GetDetailListItemType(),
                               &CDetailListItem::CreateDetailListItem);
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
    g_ItemFactory.RegisterItem(CCustomDetailListItem::GetCustomDetailListItemType(),
                               &CCustomDetailListItem::CreateCustomDetailListItem);
#endif
    g_ItemFactory.RegisterItem(CEditListItem::GetEditListItemType(),
                               &CEditListItem::CreateEditListItem);
    g_ItemFactory.RegisterItem(CMenuListItem::GetMenuItemType(), &CMenuListItem::CreateMenuListItem);
    g_ItemFactory.RegisterItem(CTextStatusItem::GetStatusItemType(), &CTextStatusItem::CreateStatusItem);
    g_ItemFactory.RegisterItem(CRadioListItem::GetRadioItemType(),
                               &CRadioListItem::CreateRadioListItem);
    g_ItemFactory.RegisterItem(CSelectItem::GetSelectItemType(), &CSelectItem::CreateSelectItem);
    g_ItemFactory.RegisterItem(CTwoLineEditListItem::GetTwoLineEditListItemType(), &CTwoLineEditListItem::CreateTwoLineEditListItem);

    MessageBox_RegisterMsgBox((MESSAGEBOX_TYPE)MESSAGEBOX_FULLSCREEN,
                              &CFullScreenMessageBox::CreateFullScreenMsgBox);
    MessageBox_RegisterMsgBox((MESSAGEBOX_TYPE)MESSAGEBOX_SELECTBOX,
                              &CSelectMessageBox::CreateSelectMsgBox);

    g_ItemFactory.RegisterItem(talkui::CSearchWidgetItem::GetSearchItemType(),
                               &talkui::CSearchWidgetItem::CreateSearchItem);
    g_ItemFactory.RegisterItem(talkui::CConfManagerItem::GetConfManagerItemType(),
                               &talkui::CConfManagerItem::CreateConfManagerItem);
    g_ItemFactory.RegisterItem(talkui::CTalkNormalItem::GetNormalItemType(),
                               &talkui::CTalkNormalItem::CreateNormalItem);

    g_WorkStation.SetFunIsRTLText(&Lang_IsRTL);
    g_WorkStation.SetFunIsRTLChar(&Lang_IsRTL);
    g_WorkStation.SetFunGetRTLShowText(&RTL_GetRealString);
    g_WorkStation.SetFunGetRTLShowWText(&Lang_Format);

}

void BaseUI_UnInit()
{
    g_WorkStation.SetFunIsRTLText(NULL);
    g_WorkStation.SetFunIsRTLChar(NULL);
    g_WorkStation.SetFunGetRTLShowText(NULL);
    g_WorkStation.SetFunGetRTLShowWText(NULL);

    UnInitCommonStyle();
#if !IS_COLOR
    UnInitT2xStyle();
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    CItemImageBGPainter::UnInit();
    COptionMenuItemBG::UnInit();
    CDialSearchItemBG::UnInit();
#endif
}

void BaseUI_ShowOptionMenu(CMsgBoxCallBackAction * pAction, VEC_OPTION_ITEM_DATA & vecOption,
                           int nID)
{
    MSGBOX_INFO stMsgInfo;

    stMsgInfo.pAction = pAction;
    stMsgInfo.eType = (MESSAGEBOX_TYPE)MESSAGEBOX_SELECTBOX;
    stMsgInfo.iID = nID;

    SELECTMSGBOXINFO objSelectBoxInfo;
    objSelectBoxInfo.m_vecSelectMsgBoxItem = vecOption;

    stMsgInfo.extraData = &objSelectBoxInfo;
    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

#if IS_COLOR
void BaseUI_ImageInit()
{
#if IF_SUPPORT_WALLPAPER
    CImageClassifier* pWallpaperCacheClassifier = new CImageClassifier;
    if (NULL != pWallpaperCacheClassifier)
    {
        pWallpaperCacheClassifier->SetImageClassifyType(IMG_CLASSIFY_WALLPAPER);
        pWallpaperCacheClassifier->SetCacheType(IMG_CACHE_TYPE_MANUAL);

        if (!Image_RegisterClassifier(pWallpaperCacheClassifier))
        {
            delete pWallpaperCacheClassifier;
        }
    }
#endif

#ifdef PHOTO_SHOW
    CImageClassifier* pCustomPhotoClassifier = new CImageClassifier;
    if (NULL != pCustomPhotoClassifier)
    {
        pCustomPhotoClassifier->SetImageClassifyType(IMG_CLASSIFY_CUSTOM_PHOTO);
        pCustomPhotoClassifier->SetCacheType(IMG_CACHE_TYPE_LRU);

        if (!Image_RegisterClassifier(pCustomPhotoClassifier))
        {
            delete pCustomPhotoClassifier;
        }
    }
#endif

#ifdef IF_SUPPORT_USB_PHOTO
    CImageClassifier* pUSBPhotoClassifier = new CImageClassifier;
    if (NULL != pUSBPhotoClassifier)
    {
        pUSBPhotoClassifier->SetImageClassifyType(IMG_CLASSIFY_USB_PHOTO);
        pUSBPhotoClassifier->SetCacheType(IMG_CACHE_TYPE_LRU);
        pUSBPhotoClassifier->SetAsyncLoad(true);

        if (!Image_RegisterClassifier(pUSBPhotoClassifier))
        {
            delete pUSBPhotoClassifier;
        }
    }
#endif
}

void BaseUI_GetNormalDialogBG(xBrush& bursh)
{
    int nPhoneType = devicelib_GetPhoneType();

    if (DEVICELIB_IS_T46(nPhoneType))
    {
        xPixmap bmBG;

        if (g_WorkStation.GetImageByPath(PIC_DEFAULT_THEME_BG, bmBG))
        {
            bursh.setPixmap(bmBG);
        }
    }
    else if (PT_T52 == nPhoneType || PT_T54S == nPhoneType)
    {
        bursh.setColor(T5X_BACKGROUND_COLOR);
    }
    else if (PT_T29 == nPhoneType)
    {
        bursh.setColor(T29_BACKGROUND_COLOR);
    }
}
#endif

yl::string BaseUI_GetRealShowText(const yl::string& strText, bool bShowIndex, int nIndex)
{
    yl::string strShowText;

#if IF_ITEM_SUPPORT_CUSTOM_TEXT_EFFECT
    strShowText = " ";

    if (bShowIndex)
    {
        char szBuffer[32];

        _snprintf(szBuffer, sizeof(szBuffer), "%d.  ", nIndex);
        szBuffer[sizeof(szBuffer) - 1] = '\0';

        strShowText += szBuffer;
    }

    strShowText += strText;
#else
    if (bShowIndex)
    {
        char szBuffer[32];

        _snprintf(szBuffer, sizeof(szBuffer), "%d. ", nIndex);
        szBuffer[sizeof(szBuffer) - 1] = '\0';

        strShowText = szBuffer;
    }

    strShowText += strText;
#endif

    return strShowText;
}
