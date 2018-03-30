#include "dlgcustomphoto.h"
#ifdef PHOTO_SHOW
#include "localcontactcontroler.h"
#include "localcontactuihelper.h"

#include <resource/modresource.h>
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

#include "xwindow/xListView.h"
#include "xwindow/xValueHandler.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/comboboxlistitem.h"
#include "commonunits/commonunits_def.h"
#include "dsskey/include/moddsskey.h"
#include "dsskeyui/moddsskeyui.h"
#include "uimanager/moduimanager.h"
#include "moddirectoryui.h"
#include "directorymgr.h"
#include "imagecache/include/modimagecache.h"

namespace
{
const char* DIR_UI_ID_PHOTO = "imgPhoto";

#define CUSTOM_PHOTO_DLG_LISTVIEW_ITEM_COUNT        1
#define CUSTOM_PHOTO_DLG_PHOTO_ITEM_DATA_INDEX      0
#define CONTACT_PHOTO_FILE_SYS_DEFAULT "default_contact_image.png"
}

CDlgCustomPhoto::CDlgCustomPhoto()
    : m_pImgView(NULL)
    , m_pPhotoItem(NULL)
{
}

CDlgCustomPhoto::~CDlgCustomPhoto()
{

}

CBaseDialog* CDlgCustomPhoto::CreateInstance()
{
    CDlgCustomPhoto* pDlgCustomPhoto = new CDlgCustomPhoto;

    if (NULL != pDlgCustomPhoto)
    {
        pDlgCustomPhoto->loadContent(DIR_UI_LAYOUT_FILE_CUSTOM_PHOTO);
    }

    return pDlgCustomPhoto;
}

void CDlgCustomPhoto::Init()
{
    InitPhotoItem();

    xListView* pListView = CDlgDirectoryBase::GetListView();

    if (NULL == pListView || NULL == m_pPhotoItem)
    {
        return;
    }

    pListView->SetAdapter(this);

    m_pPhotoItem->SetupValueChangedSlot(SLOT_METHOD(this, CDlgCustomPhoto::OnCurrentIndexChange));

    IDirUIHelperPtr pDirUIHelper = g_DirectoryMgr.GetActiveUIHelper();

    CLocalContactUIHelper* pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pDirUIHelper);

    if (NULL != pLocalUIHelper)
    {
        yl::string strPhoto;
        pLocalUIHelper->GetDetailLayoutPhoto(strPhoto);

        res_toRealPath(strPhoto, PHFILE_TYPE_CONTACT_IMAGE);
        m_pPhotoItem->SetItemData(strPhoto);
        OnCurrentIndexChange(NULL, 0, 0, 0, NULL);
    }
}

void CDlgCustomPhoto::Uninit()
{
    CDlgDirectoryBase::Uninit();

    if (NULL == m_pPhotoItem)
    {
        return;
    }

    m_pPhotoItem->RemoveValueChangedSlot(SLOT_METHOD(this, CDlgCustomPhoto::OnCurrentIndexChange));

    m_pPhotoItem->Release();
    m_pPhotoItem = NULL;
}

void CDlgCustomPhoto::UpdateWnd(bool bUIReturn/* = false*/)
{
    RefreshTitle();

    RefreshSoftkey();

    RefreshListView();
}

void CDlgCustomPhoto::InitListViewLayout(xListView* pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetItemAttach();

    pListView->SetTotalDataCount(CUSTOM_PHOTO_DLG_LISTVIEW_ITEM_COUNT);
}

xListViewItem* CDlgCustomPhoto::PrepareItem(xListView* pListView, int nDataIndex,
        xListViewItem* pItem)
{
    if (NULL != m_pPhotoItem)
    {
        m_pPhotoItem->SetAction(this);
    }

    return m_pPhotoItem;
}

bool CDlgCustomPhoto::RefreshItem(xListView* pListView, xListViewItem* pItem)
{
    return true;
}

void CDlgCustomPhoto::loadChildrenElements(xml_node& node)
{
    CDlgDirectoryBase::loadChildrenElements(node);

    m_pImgView = (xImageView*)getViewById(DIR_UI_ID_PHOTO);
}

bool CDlgCustomPhoto::onKeyPressedEvent(int nKeyCode)
{
    if (PHONE_KEY_OK == nKeyCode)
    {
        FinishPhotoChoose();
        return true;
    }
    else if (PHONE_KEY_CANCEL == nKeyCode)
    {
        UIManager_PutPrivateDlg(this);
        return true;
    }
    else if (nKeyCode >= PHONE_KEY_LINE1 && nKeyCode <= PHONE_KEY_LINE10)
    {
        int nDsskeyId = dsskeyUI_GetDsskeyIDByKeycode(nKeyCode);
        DssKey_Type dsskeyType = dsskey_GetDsskeyType(nDsskeyId);

        if (DT_LINE != dsskeyType && DM_IsDirDssKey(dsskeyType))
        {
            return true;
        }
    }

    xListView* pListView = CDlgDirectoryBase::GetListView();

    if (NULL != pListView && pListView->onKeyPressedEvent(nKeyCode))
    {
        return true;
    }

    return false;
}

bool CDlgCustomPhoto::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    if (ST_CANCEL == iType)
    {
        UIManager_PutPrivateDlg(this);
        return true;
    }
    else if (ST_OK == iType)
    {
        FinishPhotoChoose();
        return true;
    }
    else if (ST_SWITCH == iType && NULL != m_pPhotoItem)
    {
        m_pPhotoItem->SwitchNextValue();
    }

    return false;
}

void CDlgCustomPhoto::RefreshSoftkey()
{
    CArraySoftKeyBarData vecSoftKey;

    for (int i = 0; i < SOFTKEY_NUM; i++)
    {
        CSoftKeyBarData objSoftkeyData;

        objSoftkeyData.m_iType = ST_EMPTY;

        vecSoftKey.push_back(objSoftkeyData);
    }

    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_CANCEL);
    vecSoftKey[0].m_iType = ST_CANCEL;

    vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SWITCH);
    vecSoftKey[2].m_iType = ST_SWITCH;

    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
    vecSoftKey[3].m_iType = ST_OK;

    SetSoftKey(vecSoftKey);
}

void CDlgCustomPhoto::RefreshTitle()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl* pCommonCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonCtrl)
    {
        return;
    }

    yl::string strDisplayName = _LANG(TRID_ADD_CONTACT);

    SmartPtr pDetail = pCommonCtrl->GetCurrentDetail();
    if (!pDetail.IsEmpty())
    {
        strDisplayName = pDetail->GetDisplayName();
    }

    SetTitle(strDisplayName);
    SetTitlePageInfo("");
}

void CDlgCustomPhoto::InitPhotoItem()
{
    xListViewItem* pItem = g_ItemFactory.ApplyItem(CComboBoxListItem::GetComboBoxListItemType());
    m_pPhotoItem = CComboBoxListItem::GetComboBoxListItem(pItem);

    if (NULL == m_pPhotoItem)
    {
        return;
    }

    m_pPhotoItem->SetDataIndex(CUSTOM_PHOTO_DLG_PHOTO_ITEM_DATA_INDEX);

    m_pPhotoItem->SetShowIndex(true);
    m_pPhotoItem->SetTitle(_LANG(TRID_PHOTO));

    typedef YLSmartPtr<BaseResItem> CPhotoDataPtr;
    typedef YLList<CPhotoDataPtr> LIST_PHOTO;
    yl::string strResType;
    LIST_PHOTO lsPhoto;

    if (Res_GetResType(DIR_RES_TYPE_PHOTO, strResType)
            && 0 == res_getResItemsByResType(lsPhoto, strResType.c_str(), true))
    {
        LIST_PHOTO::iterator iter = lsPhoto.begin();

        for (; iter != lsPhoto.end(); iter++)
        {
            CPhotoDataPtr & pItem = *iter;

            if (pItem.IsEmpty())
            {
                continue;
            }

            if (CONTACT_PHOTO_FILE_SYS_DEFAULT == pItem->resItemDispLabel)
            {
                pItem->resItemDispLabel = _LANG(TRID_PHOTO_CUSTOM);
            }

            m_pPhotoItem->AddValue(pItem->resItemDispLabel, pItem->resPath);
        }
    }
}

void CDlgCustomPhoto::FinishPhotoChoose()
{
    IDirUIHelperPtr pUIHelper = g_DirectoryMgr.GetActiveUIHelper();
    CLocalContactUIHelper* pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pUIHelper);

    if (NULL != pLocalUIHelper && NULL != m_pPhotoItem)
    {
        yl::string strPhoto = m_pPhotoItem->GetValue();
        res_toHidePath(strPhoto, PHFILE_TYPE_CONTACT_IMAGE);

        pLocalUIHelper->SetDetailLayoutPhoto(strPhoto);
    }

    UIManager_PutPrivateDlg(this);
}

void CDlgCustomPhoto::OnCurrentIndexChange(xSignal* sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    if (NULL == m_pPhotoItem || NULL == m_pImgView)
    {
        return;
    }

    yl::string strPhotoPath = m_pPhotoItem->GetValue();

    xPixmap pmImage;
    Image_GetImage(pmImage, strPhotoPath, IMG_CLASSIFY_CUSTOM_PHOTO);
    m_pImgView->setPixmap(pmImage);
}

#endif
