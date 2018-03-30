#include "baseui/editlistitem.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/framelist.h"
#include "localdetaillayout.h"

#include "dlgdirnew.h"

#include "contactgroupdelegate.h"
#include "contactphotoselector.h"
#include "directorycommon.h"
#include "translateiddefine.h"
#include "directorymgr.h"
#include "dirctrlfactory.h"
#include "diruihelperfactory.h"
#include "delegatefactory.h"
#include "moddirectoryui.h"
#include "bsftcontactuihelper.h"
#include "localcontactuihelper.h"
#include "bsftcontactcontroler.h"
#include "localcontactcontroler.h"
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactcontroler.h"
#include "mtswcontactuihelper.h"
#include "mtswdetaillayout.h"
#endif

#include "baseui/t4xpicpath.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "uicommon.h"
#include "imagemanager/modimagemanager.h"
#include "uimanager/rectdefine.h"
#include "mainwnd/mainwnd.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/languagehelper.h"
#include "account/include/modaccount.h"
#include "inputmethod/inputmethod.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "uikernel/qcomboboxutility.h"
#include "commonunits/commonunits_def.h"
#include <resource/modresource.h>

#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#include "genbandcontactuihelper.h"
#include "genbandpabdetaillayout.h"
#endif

namespace
{
//favorite时图像框位置信息
#define DLG_DIR_NEW_POS_IMAGE_FAVORITE_CONTACT_LEFT 142
// 图像框位置信息
#define DLG_DIR_NEW_POS_IMAGE_CONTACT_LEFT 170
#define DLG_DIR_NEW_POS_IMAGE_CONTACT_TOP 10
#define DLG_DIR_NEW_POS_IMAGE_CONTACT_WEIGHT 108
#define DLG_DIR_NEW_POS_IMAGE_CONTACT_HEIGHT 108
// 列表项位置信息
#define DLG_DIR_NEW_POS_LIST_ITEM_HEIGHT 40

#define DLG_DIR_NEW_ITEM_OFFSET 4

// 字体大小
#define DLG_DIR_NEW_FONT_SIZE 16

#define DLG_DIR_NEW_MAX_PAGE_ITEM_COUNT 8

const QRect RECT_DIR_NEW_LIST_NORMAL(286, 8, 356, 340);
const QRect RECT_DIR_CONTACT_ADD(251, 8, 414, 332);
const QRect RECT_DIR_CONTACT_IMAGE(118, 10, 108, 108);
}

CDlgDirNew::CDlgDirNew(QWidget * parent/* = NULL*/)
    : CBaseDialog(parent)
    , m_btnImg(this)
    , m_pListItemLayout(NULL)
    , m_pFrameList(NULL)
    , m_pComboBoxGroup(NULL)
    , m_pComboBoxRing(NULL)
    , m_pComboBoxPhoto(NULL)
    , m_strImage("")
    , m_nAddGroupId(0)
    , m_nContactType(DIRECTORY_TYPE_NONE)
    , m_nFocusIndex(INVALID_ITEM_INDEX)
    , m_bCopyContact(false)
    , m_ring(this)
    , m_bMutiPage(false)
    , m_nListGroupId(0)
{
    SetWindowType(eWT_Directory);
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_DIRECTORY);
    SetSoftkeyType(STYLE_TYPE_DIVIDE);
    SetWindowTitle(LANG_TRANSLATE(TRID_NEW_CONTACT));

    qWidgetUtility::setFontStyle(this, DLG_DIR_NEW_FONT_SIZE, QFont::Bold);

    m_pFrameList = new CFrameList(this);

    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetDelegate(this);
        m_pFrameList->SetRect(RECT_DIR_NEW_LIST_NORMAL);
        m_pFrameList->SetMaxPageItemCount(DLG_DIR_NEW_MAX_PAGE_ITEM_COUNT);

        m_pFrameList->SetItemSpace(0);

        m_pFrameList->SetItemMargin(0, 0, 0);

        m_pFrameList->show();
        m_pFrameList->raise();
    }
}

CDlgDirNew::~CDlgDirNew(void)
{
    ReleaseLayout();
}


void CDlgDirNew::ReleaseLayout()
{
    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.FindDirUIHelper(m_nContactType);
    qBlockKeyboard(true);
    DIRUI_INFO("CDlgDirNew::ReleaseLayout Contact Type [%d]", m_nContactType);
    switch (m_nContactType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            CLocalContactUIHelper * pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pUIHelper);

            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetDetailLayout(NULL);
            }
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

            if (NULL != pBSFTUIHelper)
            {
                pBSFTUIHelper->SetAdditionLayout(NULL);
            }
        }
        break;
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pUIHelper);

            if (NULL != pMTSWUIHelper)
            {
                pMTSWUIHelper->SetAdditionLayout(NULL);
            }
        }
        break;
#endif
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            CGenbandContactUIHelper * pGenbandUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);
            if (NULL != pGenbandUIHelper)
            {
                pGenbandUIHelper->SetDetailLayout(NULL);
            }
        }
#endif
    default:
        break;
    }

    if (NULL != m_pListItemLayout)
    {
        m_pListItemLayout->ReleaseAllItem();

        delete m_pListItemLayout;
        m_pListItemLayout = NULL;
    }

    m_pComboBoxGroup = NULL;
    m_pComboBoxRing = NULL;
    m_pComboBoxPhoto = NULL;
    qBlockKeyboard(false);
}

void CDlgDirNew::InitLocalLayout(bool bIsDsskeyAdd/* =false*/)
{
    if (NULL != m_pListItemLayout)
    {
        ReleaseLayout();
    }

    bool bCopyContact = false;

#if IF_FEATURE_METASWITCH_DIRECTORY
    if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        CMTSWDetailLayoutPtr pMTSWLayout = new CMTSWDetailLayout(this, true, m_bCopyContact);

        if (NULL == pMTSWLayout)
        {
            return;
        }

        m_pListItemLayout = pMTSWLayout;
        pMTSWLayout->Init();

        m_bMutiPage = pMTSWLayout->GetItemCount() > DLG_DIR_NEW_MAX_PAGE_ITEM_COUNT;
    }
    else
#endif
#if IF_FEATURE_GBBOOK
        if (DIRECTORY_TYPE_GENBAND == m_nContactType)
        {
            CGenbandDetailLayoutPtr pGenbandLayout = new CGenbandDetailLayout(this, true, m_bCopyContact);
            if (NULL == pGenbandLayout)
            {
                return;
            }
            m_pListItemLayout = pGenbandLayout;
            pGenbandLayout->Init();

            m_bMutiPage = pGenbandLayout->GetItemCount() > DLG_DIR_NEW_MAX_PAGE_ITEM_COUNT;
        }
        else
#endif
        {
            if (m_bCopyContact && DIRECTORY_TYPE_LOCAL == m_nContactType)
            {
                bCopyContact = true;
            }

            CLocalDetailLayoutPtr pLocalLayout = new CLocalDetailLayout(this, true, bCopyContact);

            if (NULL == pLocalLayout)
            {
                return;
            }

#if IF_FEATURE_FAVORITE_DIR
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                pLocalLayout->SetIsFromDsskeyAdd(bIsDsskeyAdd);
            }
#endif

            m_pListItemLayout = pLocalLayout;

            pLocalLayout->Init();

            m_bMutiPage = pLocalLayout->GetItemCount() > DLG_DIR_NEW_MAX_PAGE_ITEM_COUNT;

            // Group ComboBox
            CComboBoxListItemPtr pGroupComboBoxItem = pLocalLayout->GetGroupComboBoxItem();

            if (NULL != pGroupComboBoxItem)
            {
                m_pComboBoxGroup = pGroupComboBoxItem->GetComboBox();
            }

            if (NULL != m_pComboBoxGroup)
            {
                connect(m_pComboBoxGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxGroupSelect(int)));
            }

            // Ring ComboBox
            CComboBoxListItemPtr pRingComboBoxItem = pLocalLayout->GetRingComboBoxItem();

            if (NULL != pRingComboBoxItem)
            {
                m_pComboBoxRing = pRingComboBoxItem->GetComboBox();
            }

            if (NULL != m_pComboBoxRing)
            {
                connect(m_pComboBoxRing, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxRingSelect(int)));
                connect(m_pComboBoxRing, SIGNAL(highlighted(int)), this, SLOT(OnComboBoxRingSelect(int)));
            }

            // Photo ComboBox
            CComboBoxListItemPtr pPhotoComboBoxItem = pLocalLayout->GetPhotoComboBoxItem();

            if (NULL != pPhotoComboBoxItem)
            {
                m_pComboBoxPhoto = pPhotoComboBoxItem->GetComboBox();
            }

            if (NULL != m_pComboBoxPhoto)
            {
                connect(m_pComboBoxPhoto, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxPhotoSelect(int)));
                connect(m_pComboBoxPhoto, SIGNAL(highlighted(int)), this, SLOT(OnComboBoxPhotoSelect(int)));
            }
        }
}

void CDlgDirNew::ReLayoutListItem()
{
    if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        SetBSFTItemStyle();
        RelayoutBSFTListItem();
        m_strImage = PIC_CONTACTS_IMAGE_DEFAULT;
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        // do null
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        // do null
    }
#endif
    else if (kszBlackListGroupId == m_nAddGroupId)
    {
        SetLocalItemStyle();
        m_strImage = PIC_CONTACTS_IMAGE_DEFAULT;
    }
    else
    {
        SetLocalItemStyle();
        RelayoutLocalListItem();
    }

    update(m_btnImg.GetRect());
}

void CDlgDirNew::SetLocalItemStyle()
{
    if (NULL == m_pListItemLayout)
    {
        return;
    }

    CLocalDetailLayoutPtr pLocalLayout = NULL;
    if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        pLocalLayout = CLocalDetailLayoutPtr(m_pListItemLayout);
    }

    if (NULL == pLocalLayout)
    {
        return;
    }

    CEditListItemPtr pNameEditItem = pLocalLayout->GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->SetMaxLength(MAX_LEN_DISPLAY_NAME);
    }

    if (pLocalLayout->IsCopyLayout())
    {
        CComboBoxListItemPtr pOfficeNoItem = pLocalLayout->GetOfficeNoComboBoxItem();

        if (NULL != pOfficeNoItem)
        {
            pOfficeNoItem->SetTitle(LANG_TRANSLATE(TRID_OFFICE_NUMBER));
        }
    }
    else
    {
        CEditListItemPtr pOfficeNoItem = pLocalLayout->GetOfficeNoEditItem();

        if (NULL != pOfficeNoItem)
        {
            pOfficeNoItem->SetTitle(LANG_TRANSLATE(TRID_OFFICE_NUMBER));
            pOfficeNoItem->SetMaxLength(MAX_LEN_PHONE_NUMBER_LONG);
        }
    }
}

void CDlgDirNew::SetBSFTItemStyle()
{
    if (NULL == m_pListItemLayout)
    {
        return;
    }

    CLocalDetailLayoutPtr pLocalLayout = NULL;
    if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        pLocalLayout = CLocalDetailLayoutPtr(m_pListItemLayout);
    }

    if (pLocalLayout == NULL)
    {
        return;
    }

    CEditListItemPtr pNameEditItem = pLocalLayout->GetNameEditItem();

    if (NULL != pNameEditItem)
    {
        pNameEditItem->SetMaxLength(MAX_LEN_BSFT_NAME);
    }

    if (pLocalLayout->IsCopyLayout())
    {
        CComboBoxListItemPtr pOfficeNoItem = pLocalLayout->GetOfficeNoComboBoxItem();

        if (NULL != pOfficeNoItem)
        {
            pOfficeNoItem->SetTitle(LANG_TRANSLATE(TRID_NUMBER));
        }
    }
    else
    {
        CEditListItemPtr pOfficeNoItem = pLocalLayout->GetOfficeNoEditItem();

        if (NULL != pOfficeNoItem)
        {
            pOfficeNoItem->SetTitle(LANG_TRANSLATE(TRID_NUMBER));
            pOfficeNoItem->SetMaxLength(MAX_LEN_BSFT_NUMBER);
        }
    }
}

void CDlgDirNew::RelayoutLocalListItem()
{
    if (NULL == m_pListItemLayout)
    {
        DIRUI_INFO("CDlgDirNew::RelayoutLocalListItem Layout Error");
        return;
    }

    if (NULL != m_pComboBoxPhoto)
    {
        int nIndex = m_pComboBoxPhoto->currentIndex();
        yl::string strFilePath = m_pComboBoxPhoto->itemData(nIndex).value<yl::string>();

        m_strImage = strFilePath;
    }
    else
    {
        m_strImage = PIC_CONTACTS_IMAGE_DEFAULT;
    }
}

void CDlgDirNew::RelayoutBSFTListItem()
{
    if (NULL == m_pListItemLayout)
    {
        return;
    }

    CLocalDetailLayoutPtr pLocalLayout = NULL;
    if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        pLocalLayout = CLocalDetailLayoutPtr(m_pListItemLayout);
    }

    if (NULL == pLocalLayout)
    {
        return;
    }

    CDetailListItemBasePtr arrItem[5] = { pLocalLayout->GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_MOBILE_NO),
                                          pLocalLayout->GetDetailListItem(LOCAL_CONTACT_DETAIL_ITEM_INDEX_OTHER_NO),
                                          pLocalLayout->GetAccountComboBoxItem(),
                                          pLocalLayout->GetRingComboBoxItem(),
                                          pLocalLayout->GetPhotoComboBoxItem()
                                        };

    for (int i = 0; i < 5; ++i)
    {
        CDetailListItemBasePtr pItem = arrItem[i];

        if (NULL == pItem)
        {
            continue;
        }

        pItem->hide();
    }


    m_strImage = PIC_CONTACTS_IMAGE_DEFAULT;
}

void CDlgDirNew::SetContactType(int nContactType)
{
    if (m_nContactType == nContactType)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(m_nContactType);

    if (!pControler.IsEmpty())
    {
        pControler->UnInit();
    }

    m_nContactType = nContactType;

    if (DIRECTORY_TYPE_NONE == m_nContactType)
    {
        return;
    }

    pControler = g_DirCtrlFactory.GetControler(m_nContactType);

    if (pControler.IsEmpty())
    {
        return;
    }

    CDirUIHelperPtr pUIHelper = g_DirUIHelperFactory.GetDirUIHelper(m_nContactType);

    if (pUIHelper.IsEmpty())
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate(NULL);

    pControler->Init(pDelegate, pUIHelper);
    pUIHelper->Init();
    pUIHelper->SetFrameList(NULL);
    pUIHelper->SetFocusInvalidFrameList(m_pFrameList);

    switch (nContactType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            CLocalContactUIHelper * pLocalUIHelper = CLocalContactUIHelper::GetUIHelper(pUIHelper);

            if (NULL != pLocalUIHelper)
            {
                pLocalUIHelper->SetDetailLayout((CLocalDetailLayoutPtr)m_pListItemLayout);
            }
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            CBSFTContactUIHelper * pBSFTUIHelper = CBSFTContactUIHelper::GetUIHelper(pUIHelper);

            if (NULL != pBSFTUIHelper)
            {
                pBSFTUIHelper->SetAdditionLayout((CLocalDetailLayoutPtr)m_pListItemLayout);
            }
        }
        break;
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            CMTSWContactUIHelper * pMTSWUIHelper = CMTSWContactUIHelper::GetUIHelper(pUIHelper);
            if (NULL != pMTSWUIHelper)
            {
                pMTSWUIHelper->SetAdditionLayout((CMTSWDetailLayoutPtr)m_pListItemLayout);
            }
        }
        break;
#endif
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            // 设置焦点时候需要用到framelist
            pUIHelper->SetFrameList(m_pFrameList);
            CGenbandContactUIHelper * pGBUIHelper = CGenbandContactUIHelper::GetUIHelper(pUIHelper);
            if (NULL != pGBUIHelper)
            {
                pGBUIHelper->SetDetailLayout((CGenbandDetailLayoutPtr)m_pListItemLayout);
            }
        }
        break;
#endif
    default:
        break;
    }
}

void CDlgDirNew::InitGroup()
{
    if (NULL == m_pComboBoxGroup)
    {
        return;
    }

    m_pComboBoxGroup->clear();

    CGroupDelegatePtr pGroupDelegate = g_DirDelegateFactory.GetGroupDelegate(DIR_MOD_TYPE_CONTACT);
    CContactGroupDelegate * pContactDelegate = CContactGroupDelegate::GetGroupDelegate(pGroupDelegate);

    if (NULL == pContactDelegate)
    {
        return;
    }

    VEC_CONTACT_GROUP_NODE vecGroupNode;

    pContactDelegate->GetAdditiveGroup(vecGroupNode, m_bCopyContact, m_nContactType);

    int nContactType = m_nContactType;
    int nGroupId = m_nAddGroupId;
    int nSelIndex = 0;

    VEC_CONTACT_GROUP_NODE::iterator iter = vecGroupNode.begin();

    for (int i = 0; iter != vecGroupNode.end(); ++iter, ++i)
    {
        CContactGroupNodePtr pGroupNode = *iter;

        if (NULL == pGroupNode)
        {
            continue;
        }

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                && pGroupNode->GetGroupId() == kszFavoriteGroupId)
        {
            continue;
        }
#endif

        int nGroupTreeId = pGroupNode->GetId();

        // 添加群组项时，m_nContactType、m_nAddGroupId会变掉，所以不能用m_nContactType、m_nAddGroupId判断
        if (pGroupNode->GetType() == nContactType && pGroupNode->GetGroupId() == nGroupId)
        {
            nSelIndex = i;
        }

        m_pComboBoxGroup->addItem(toQString(pGroupNode->GetName(), true),
                                  QVariant::fromValue(nGroupTreeId));
    }

    m_pComboBoxGroup->setCurrentIndex(nSelIndex);

    CLocalDetailLayoutPtr pLocalLayout;
    if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        pLocalLayout = CLocalDetailLayoutPtr(m_pListItemLayout);
    }

    if (m_bCopyContact && NULL != pLocalLayout)
    {
        CComboBoxListItemPtr pGroupComboBoxItem = pLocalLayout->GetGroupComboBoxItem();

        if (NULL != pGroupComboBoxItem)
        {
            pGroupComboBoxItem->CheckOnlyShowText(true);
        }
    }
}

void CDlgDirNew::Init(bool bCopyContact, int nContactType, int nDefaultGroupId,
                      bool bIsDsskeyAdd /*= false*/)
{
    DIRUI_INFO("CDlgDirNew::Init(Copy=%d, Type=%d, GroupId=%d, bIsDsskeyAdd=%d)",
               (bCopyContact ? 1 : 0), nContactType, nDefaultGroupId, bIsDsskeyAdd);

    m_bCopyContact = bCopyContact;
    m_nContactType = nContactType;

    InitLocalLayout(bIsDsskeyAdd);

    m_nContactType = DIRECTORY_TYPE_NONE;
    SetContactType(nContactType);

    m_nAddGroupId = nDefaultGroupId;

    InitGroup();

    ReLayoutListItem();

}

int CDlgDirNew::GetDirType()
{
    return m_nContactType;
}

void CDlgDirNew::SetPhotoPath(yl::string & strFilePath)
{
    if (NULL == m_pComboBoxPhoto)
    {
        return;
    }

    yl::string strName;

    if (!Res_GetNameByPath(DIR_RES_TYPE_PHOTO, strFilePath, strName))
    {
        res_toHidePath(strFilePath, PHFILE_TYPE_CONTACT_IMAGE);

        if (CONTACT_PHOTO_DEFAULT_CONTACT_IMAGE != strFilePath)
        {
            return;
        }

        strName = CONTACT_PHOTO_FILE_DEFAULT;
    }

    QString strUtf8Name = LANG_TRANSLATE(DM_GetContactImageName(strName).c_str());

    qComboBoxUtility::SetComboBoxSelectText(m_pComboBoxPhoto, strUtf8Name);
}

CEditListItem * CDlgDirNew::GetNameEditItem()
{
    if (NULL == m_pListItemLayout)
    {
        return NULL;
    }

    if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        CLocalDetailLayoutPtr pLoaclLayout = (CLocalDetailLayoutPtr)m_pListItemLayout;
        if (pLoaclLayout != NULL)
        {
            return pLoaclLayout->GetNameEditItem();
        }
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_MTSW_CONTACT)
    {
        CMTSWDetailLayoutPtr pMtswLayout = (CMTSWDetailLayoutPtr)m_pListItemLayout;
        if (pMtswLayout != NULL)
        {
            return pMtswLayout->GetFirstNameItem();
        }
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_GENBAND)
    {
        CGenbandDetailLayoutPtr pGBLayout = (CGenbandDetailLayoutPtr)m_pListItemLayout;
        if (pGBLayout != NULL)
        {
            return pGBLayout->GetNameEditItem();
        }
    }
#endif

    return NULL;
}

bool CDlgDirNew::GetRingPath(yl::string & strRingPath, int nIndex)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget || pFocusWidget != m_pComboBoxRing)
    {
        return false;
    }

    // 如果不在当前界面，不播放铃声
    if (!this->isVisible())
    {
        return false;
    }

    //如果在第一项，也就是Auto项，不播放铃声
    if (nIndex <= 0)
    {
        return false;
    }

    strRingPath = m_pComboBoxRing->itemData(nIndex).value<yl::string>();

    return true;
}

void CDlgDirNew::Init()
{
    m_nFocusIndex = INVALID_ITEM_INDEX;
    m_ring.StopRingPlay();

    m_strImage = PIC_CONTACTS_IMAGE_DEFAULT;

    int nLeft = DLG_DIR_NEW_POS_IMAGE_CONTACT_LEFT;
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        nLeft = DLG_DIR_NEW_POS_IMAGE_FAVORITE_CONTACT_LEFT;
    }
#endif

    m_btnImg.SetRect(nLeft, DLG_DIR_NEW_POS_IMAGE_CONTACT_TOP,
                     DLG_DIR_NEW_POS_IMAGE_CONTACT_WEIGHT, DLG_DIR_NEW_POS_IMAGE_CONTACT_HEIGHT);

    if (NULL != m_pFrameList)
    {
#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            m_pFrameList->SetRect(RECT_DIR_CONTACT_ADD);
        }
        else
#endif
        {
            m_pFrameList->SetRect(RECT_DIR_NEW_LIST_NORMAL);
        }
        m_pFrameList->SetDelegate(this);
    }
}

void CDlgDirNew::Uninit()
{
    qBlockKeyboard(true);
    m_nFocusIndex = INVALID_ITEM_INDEX;

    if (NULL != m_pFrameList)
    {
        m_pFrameList->ClearContent();
        m_pFrameList->SetDelegate(NULL);
    }

    ReleaseLayout();

    SetContactType(DIRECTORY_TYPE_NONE);

    m_ring.StopRingPlay();
    qBlockKeyboard(false);
}

void CDlgDirNew::UpdateWnd()
{

    if (NULL != m_pFrameList)
    {
        m_pFrameList->RefreshData(0);
    }

    CEditListItemPtr pNameItem = GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetFocus(true);
    }

}

void CDlgDirNew::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    MESSAGEBOX_TYPE eMsgBoxType = pMessageBox->GetType();
    MESSAGEBOX_RESULT eResult = pMessageBox->GetResult();
    int nAction = pMessageBox->GetID();

    switch (eMsgBoxType)
    {
    case MESSAGEBOX_SPECIAL:
        {
            if (MessageBox_Discard == eResult)
            {
                if (DIR_UI_ACTION_COMMON_BACK == nAction)
                {
                    UIManager_PutPrivateDlg(this);
                }
                else if (DIR_UI_ACTION_COMMON_HOME == nAction)
                {
                    CBaseDialog::OnHomeButtonClick();
                }
            }
            else if (MessageBox_OK == eResult)
            {
                OnBtnSaveClick();
            }
        }
        break;
    case MESSAGEBOX_IMAGE_SELECT:
        {
            if (MessageBox_OK != eResult)
            {
                break;
            }

            CContactPhotoSelector * pPhotoSelector = g_DirectoryMgr.GetPhotoSelector();

            if (NULL == pPhotoSelector)
            {
                break;
            }

            yl::string strPhotoPath = pPhotoSelector->GetPhoto();

            SetPhotoPath(strPhotoPath);
        }
        break;
    default:
        break;
    }
}

bool CDlgDirNew::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    if (MessageBox_GetMessageBoxCount() > 0)
    {
        return false;
    }

    //处理铃声事件
    if (m_ring.IsRinging())
    {
        QWidget * pFocusWidget = focusWidget();

        if (pFocusWidget != m_pComboBoxRing)
        {
            m_ring.StopRingPlay();
        }
    }

    //判断事件类型
    switch (pEvent->type())
    {
    case QEvent::Show:
        {
            if (OnShowEventFilter(pObj, pEvent))
            {
                return true;
            }
        }
        break;
    //焦点集中事件
    case QEvent::FocusIn:
        {
            if (OnFocusInEventFilter(pObj, (QFocusEvent *)pEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::FocusOut:
        {
            OnCtrlFocusChanged();
        }
        break;
    //键盘输入事件
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_CANCEL:
                {
                    QWidget * pFocusWidget = focusWidget();

                    if (NULL != pFocusWidget && pFocusWidget->inherits("QComboBox")
                            && pFocusWidget != pObj)
                    {
                        QComboBox * pComboBox = (QComboBox *)pFocusWidget;

                        pComboBox->hidePopup();
                        return true;
                    }

                    OnBtnBackClick();
                    return true;
                }
                break;
            case PHONE_KEY_OK:
                {
                    bool bFilter = false;

                    if (OnEnterPressEventFilter(pObj, pEvent, bFilter))
                    {
                        return bFilter;
                    }
                }
                break;
            case PHONE_KEY_LEFT:
                {
                    if (qComboBoxUtility::OnLeftPressEventFilter(pObj, pEvent, focusWidget()))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_RIGHT:
                {
                    if (qComboBoxUtility::OnRightPressEventFilter(pObj, pEvent, focusWidget()))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_UP:
                {
                    if (OnUpPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_DOWN:
                {
                    if (OnDownPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_VOLUME_INCREASE:
            case PHONE_KEY_VOLUME_DECREASE:
                {
                    if (m_ring.IsRinging())
                    {
                        m_ring.AdjustVolume(PHONE_KEY_VOLUME_INCREASE == nKey);
                    }

                    return false;
                }
                break;
            default:
                break;
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            if (OnMousePressEventFilter(pObj, pEvent))
            {
                return true;
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            if (pObj->inherits("QLineEdit"))
            {
                AdjustFrameList(false);
            }
        }
        break;
    default:
        break;
    }

    return CBaseDialog::eventFilter(pObj, pEvent);
}

bool CDlgDirNew::OnShowEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QLineEdit"))
    {
        _MainWND.DisplayIMESoftkey(pFocusWidget);
    }

    return false;
}

bool CDlgDirNew::OnFocusInEventFilter(QObject * pObj, QFocusEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pEvent && Qt::MouseFocusReason != pEvent->reason() && NULL != pObj)
    {
        pEvent->ignore();
        return true;
    }

    if (pFocusWidget == m_pComboBoxRing)
    {
        OnPlayRingInFocus();
    }
    else if (pFocusWidget == m_pComboBoxPhoto)
    {
        OnComboBoxPhotoInFocus();
    }

    if (FullKeyboardEnabled() && NULL != m_pListItemLayout)
    {
        int nFocusIndex = m_nFocusIndex;
        m_nFocusIndex = m_pListItemLayout->GetFocusIndex();

        if (nFocusIndex != m_nFocusIndex)
        {
            CDetailListItemBase* pFocusItem = m_pListItemLayout->GetDetailListItem(m_nFocusIndex);
            CDetailListItemBase* pLastFocusItem = m_pListItemLayout->GetDetailListItem(nFocusIndex);
            CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pFocusItem);
            CEditListItemPtr pLastEditItem = CEditListItem::GetEditListItem(pLastFocusItem);

            if (NULL == pEditItem && NULL != pLastEditItem)
            {
                qSetIsShowWnd(pLastEditItem->GetLineEdit(), false);
            }
        }
    }

    OnCtrlFocusChanged();

    return false;
}

bool CDlgDirNew::OnEnterPressEventFilter(QObject * pObj, QEvent * pEvent, bool & bFilter)
{
    if (NULL == m_pListItemLayout || NULL == m_pFrameList)
    {
        return false;
    }

    bool bSave = false;
    QWidget* pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget->inherits("QComboBox"))
        {
            if (pFocusWidget == pObj)
            {
                bSave = true;
            }
            else
            {
                bFilter = false;
                return true;
            }
        }
        else if (pFocusWidget->inherits("QLineEdit"))
        {
            bSave = true;
        }
    }

    if (bSave)
    {
        OnBtnSaveClick();
    }

    bFilter = true;
    return true;
}

bool CDlgDirNew::OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QComboBox"))
    {
        // 有下拉框时，左键无效，也不做其他处理
        if (pFocusWidget == pObj)
        {
            qComboBoxUtility::SwitchItem((QComboBox *)pFocusWidget, true);
        }

        return true;
    }

    return false;
}

bool CDlgDirNew::OnRightPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget && pFocusWidget->inherits("QComboBox"))
    {
        // 有下拉框时，右键无效，也不做其他处理
        if (pFocusWidget == pObj)
        {
            qComboBoxUtility::SwitchItem((QComboBox *)pFocusWidget, false);
        }

        return true;
    }

    return false;
}

bool CDlgDirNew::OnUpPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    bool bSwitchFocus = false;
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget->inherits("QLineEdit") || (pFocusWidget->inherits("QComboBox")
                && pFocusWidget == pObj))
        {
            bSwitchFocus = true;
        }
    }

    if (bSwitchFocus && NULL != m_pListItemLayout)
    {
        if (m_pListItemLayout->IsFirstItemFocus())
        {
            if (NULL != m_pFrameList)
            {
                m_pFrameList->OnNotifyScrollPrevious();
            }

            m_pListItemLayout->SetLastItemFocus();
        }
        else
        {
            m_pListItemLayout->SwitchFocusToPre();
        }

        SetKeyboardEnterType();

        AdjustFrameList(false);

        return true;
    }

    return false;
}

bool CDlgDirNew::OnDownPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    bool bSwitchFocus = false;
    QWidget * pFocusWidget = focusWidget();

    if (NULL != pFocusWidget)
    {
        if (pFocusWidget->inherits("QLineEdit") || (pFocusWidget->inherits("QComboBox")
                && pFocusWidget == pObj))
        {
            bSwitchFocus = true;
        }
    }

    if (bSwitchFocus && NULL != m_pListItemLayout)
    {
        if (m_pListItemLayout->IsLastItemFocus())
        {
            if (NULL != m_pFrameList)
            {
                m_pFrameList->OnNotifyScrollNext();
            }
            m_pListItemLayout->SetFirstItemFocus();
        }
        else
        {
            m_pListItemLayout->SwitchFocusToNext();
        }

        SetKeyboardEnterType();

        AdjustFrameList(false);

        return true;
    }

    return false;
}

bool CDlgDirNew::OnMousePressEventFilter(QObject * pObj, QEvent * pEvent)
{
    QWidget * pFocusWidget = focusWidget();

    SetKeyboardEnterType();

    if (NULL != pFocusWidget && NULL != pObj && pObj != pFocusWidget
            && (!pObj->inherits("qButtonItem")) && pFocusWidget->inherits("QComboBox"))
    {
        QComboBox * pComboBox = (QComboBox *)pFocusWidget;
        QAbstractItemView * pView = pComboBox->view();
        QScrollBar * pScrollBar = NULL;

        if (NULL != pView)
        {
            pScrollBar = pView->verticalScrollBar();
        }

        // 弹出下拉框时，点击其他地方
        if (NULL != pView && pView->isVisible() && pView != pObj && pScrollBar != pObj
                && pObj->parent() != pView)
        {
            return true;
        }
    }

    return false;
}

void CDlgDirNew::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnImg.ProcessMousePress(pEvent);
}

void CDlgDirNew::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (m_btnImg.ProcessMouseRelease(pEvent))
    {
        OnChooseImage();
    }
}

void CDlgDirNew::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QPixmap pmImage;

    if (!m_strImage.empty())
    {
        pmImage = THEME_GET_BMP(m_strImage.c_str());

        if (pmImage.isNull())
        {
            pmImage = STOCK_GET_BMP(m_strImage.c_str());
        }
    }

    if (pmImage.isNull())
    {
        pmImage = THEME_GET_BMP(PIC_CONTACTS_IMAGE_DEFAULT);
    }

    m_btnImg.PaintButton(stylePainter, pmImage);
}

void CDlgDirNew::SetKeyboardEnterType()
{
    if (NULL != m_pFrameList)
    {
        CDetailListItemBase *item = m_pListItemLayout->GetDetailListItem(m_pListItemLayout->GetFocusIndex());
        int nCurIndex = m_pFrameList->GetIndexByItem(item);
        if (nCurIndex != INVALID_ITEM_INDEX)
        {
            DIRUI_INFO(" ====== nCurIndex %d total count :%d====== ", nCurIndex, m_pFrameList->GetTotalItemCount());
            if (nCurIndex == m_pListItemLayout->GetItemCount() - 1)
            {
                qSetEnterType(ENTER_TYPE_DONE);
            }
            else
            {
                qSetEnterType(m_pListItemLayout->IsLastEditItem() ? ENTER_TYPE_DONE : ENTER_TYPE_NEXT);
            }
        }

    }
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgDirNew::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_GENERAL_DLG_NORMAL_BACKGROUND);
}

bool CDlgDirNew::OnAction(const QString & strAction)
{
    bool bProcess = true;

    if (strAction == ACTION_BACK)
    {
        OnBtnBackClick();
    }
    else if (strAction == IME_ACTION)
    {
        OnBtnIMEClick();
    }
    else if (strAction == IME_DEL_ACTION)
    {
        OnBtnDeleteClick();
    }
    else if (strAction == TRID_SAVE)
    {
        OnBtnSaveClick();
    }
    else
    {
        bProcess = false;
    }

    return bProcess;
}

void CDlgDirNew::OnChooseImage()
{
    if (DIRECTORY_TYPE_LOCAL != m_nContactType || kszBlackListGroupId == m_nAddGroupId)
    {
        return;
    }

    CContactPhotoSelector * pPhotoSelector = g_DirectoryMgr.GetPhotoSelector();

    if (NULL == pPhotoSelector)
    {
        return;
    }

    pPhotoSelector->Init(m_strImage);

    // hide keyboard
    if (FullKeyboardEnabled() && qGetIsShowWnd())
    {
        qSetHideKeyboard();
    }

    MessageBox_ShowImageSelectBox(this, pPhotoSelector->GetRowCount(), pPhotoSelector);
}

void CDlgDirNew::OnCtrlFocusChanged()
{
    QWidget * pFocusWidget = focusWidget();

    if (pFocusWidget != m_pComboBoxRing && (m_ring.IsRinging() || m_ring.IsBeginPlay()))
    {
        m_ring.StopRingPlay();
    }

    InitSoftKeyData();

    if (NULL == pFocusWidget)
    {
        _MainWND.DrawSoftkey(this);
        return;
    }

    if (pFocusWidget->inherits("QLineEdit"))
    {
        m_SoftkeyData[0].m_strSoftkeyAction = IME_ACTION;

        m_SoftkeyData[1].m_strSoftkeyAction = IME_DEL_ACTION;
        m_SoftkeyData[1].m_strIconPath = PIC_SOFTKEY_DELETE;

        m_SoftkeyData[2].m_strSoftkeyAction = TRID_SAVE;
        m_SoftkeyData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
        m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_SAVE;

        _MainWND.DrawSoftkey(this);
        _MainWND.DisplayIMESoftkey(pFocusWidget);
    }
    else if (pFocusWidget->inherits("QComboBox"))
    {
        m_SoftkeyData[0].m_strSoftkeyAction = TRID_SAVE;
        m_SoftkeyData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
        m_SoftkeyData[0].m_strIconPath = PIC_SOFTKEY_SAVE;

        _MainWND.DrawSoftkey(this);
    }
    else
    {
        _MainWND.DrawSoftkey(this);
    }

    qSetEnterType(m_pListItemLayout->IsLastEditItem() ? ENTER_TYPE_DONE : ENTER_TYPE_NEXT);
}

void CDlgDirNew::OnPlayRingInFocus()
{
    if (NULL == m_pComboBoxRing)
    {
        m_ring.StopRingPlay();
        return;
    }

    int nIndex = m_pComboBoxRing->currentIndex();
    yl::string strRingPath;

    if (!GetRingPath(strRingPath, nIndex))
    {
        m_ring.StopRingPlay();
        return;
    }

    m_ring.StartRingPlay(strRingPath, TIME_DELAY_FOCUS_RING);
}

void CDlgDirNew::OnComboBoxPhotoInFocus()
{
    if (NULL == m_pComboBoxPhoto)
    {
        return;
    }

    OnComboBoxPhotoSelect(m_pComboBoxPhoto->currentIndex());
}

void CDlgDirNew::OnComboBoxGroupSelect(int nIndex)
{

    CLocalDetailLayoutPtr pLocalLayout;
    if (m_pListItemLayout != NULL && m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        pLocalLayout = CLocalDetailLayoutPtr(m_pListItemLayout);
    }

    CGroupDelegatePtr pGroupDelegate = g_DirDelegateFactory.GetGroupDelegate(DIR_MOD_TYPE_CONTACT);
    CContactGroupDelegate * pContactDelegate = CContactGroupDelegate::GetGroupDelegate(pGroupDelegate);

    if (NULL == pContactDelegate)
    {
        return;
    }

    int nGroupTreeId = m_pComboBoxGroup->itemData(nIndex).value<int>();
    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)pContactDelegate->GetTreeDataNode(
                                          nGroupTreeId);

    if (NULL == pGroupNode)
    {
        return;
    }

    int nContactType = m_nContactType;
    int nGroupId = m_nAddGroupId;

    SetContactType(pGroupNode->GetType());

    m_nAddGroupId = pGroupNode->GetGroupId();

    if (nContactType != m_nContactType || (DIRECTORY_TYPE_LOCAL == m_nContactType
                                           && (kszBlackListGroupId == nGroupId || kszBlackListGroupId == m_nAddGroupId)))
    {
        ReLayoutListItem();

        if (NULL != m_pFrameList)
        {
            m_pFrameList->RefreshData(0);
            m_pFrameList->FocusItemByIndex(0);
        }
    }

    AdjustFrameList(true);
}

void CDlgDirNew::OnComboBoxRingSelect(int nIndex)
{
    yl::string strRingPath;

    if (!GetRingPath(strRingPath, nIndex))
    {
        m_ring.StopRingPlay();
        return;
    }

    m_ring.StartRingPlay(strRingPath, TIME_DELAY_PLAY_RING);
}

void CDlgDirNew::OnComboBoxPhotoSelect(int nIndex)
{
    if (NULL == m_pComboBoxPhoto)
    {
        return;
    }

    yl::string strFilePath = m_pComboBoxPhoto->itemData(nIndex).value<yl::string>();

    if (strFilePath == m_strImage)
    {
        return;
    }

    m_strImage = strFilePath;
    update(m_btnImg.GetRect());
}

bool CDlgDirNew::OnBtnSaveClick()
{
    m_ring.StopRingPlay();

    //设置处于预输入状态的字符也有效
    if (NULL == m_pListItemLayout)
    {
        return true;
    }

    m_pListItemLayout->ReturnAllEdit();

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(m_nContactType);

    if (DIRECTORY_TYPE_LOCAL == m_nContactType)
    {
        CLocalContactControler * pLocalControler = CLocalContactControler::GetControler(pControler);

        if (NULL != pLocalControler)
        {
            int nAction = DIR_ACTION_LOCAL_ADD_CONTACT;

            if (m_bCopyContact)
            {
                if (kszBlackListGroupId == m_nAddGroupId)
                {
                    nAction = DIR_ACTION_LOCAL_COPY_TO_BLACKLIST;
                }
                else
                {
                    nAction = DIR_ACTION_LOCAL_COPY_TO_CONTACTS;
                }
            }
            pLocalControler->OnAction(nAction, -1, knInvalidRId);
        }
    }
    else if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        CBSFTContactControler * pBSFTControler = CBSFTContactControler::GetControler(pControler);

        if (NULL != pBSFTControler)
        {
            int nAction = DIR_ACTION_BSFT_ADD_PERSONAL;

            if (m_bCopyContact)
            {
                nAction = DIR_ACTION_BSFT_ADD_TO_PERSONAL;
            }

            pBSFTControler->OnAction(nAction, -1, knInvalidRId);
        }
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == m_nContactType)
    {
        CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

        if (NULL != pMTSWContact)
        {
            int nAction = DIR_ACTION_MTSW_ADD_CONTACTLIST;

            pMTSWContact->OnAction(nAction, -1, knInvalidRId);
        }
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == m_nContactType)
    {
        CGenbandContactControler * pGBContact = CGenbandContactControler::GetControler(pControler);

        if (NULL != pGBContact)
        {
            int nAction = DIR_ACTION_GENBAND_ADD_PAB;

            pGBContact->OnAction(nAction, -1, knInvalidRId);
        }
    }
#endif
    SetKeyboardEnterType();
    return false;
}

void CDlgDirNew::OnBtnIMEClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    _MainWND.SwitchNextIMEName(pFocusWidget);

    if (NULL == m_pListItemLayout)
    {
        return;
    }

    if (m_pListItemLayout->GetType() == DETAIL_LIST_ITEM_LAYOUT_LOCAL)
    {
        CLocalDetailLayoutPtr pLocalLayout = (CLocalDetailLayoutPtr)m_pListItemLayout;
        if (pLocalLayout != NULL)
        {
            CEditListItemPtr pNameItem = pLocalLayout->GetNameEditItem();

            if (NULL != pNameItem && pFocusWidget == (QWidget *)pNameItem->GetLineEdit())
            {
                yl::string strIMEName = fromQString(qGetCurrentIMEName(pFocusWidget));

                if (!strIMEName.empty())
                {
                    configParser_SetConfigString(kszContactDefaultIME, strIMEName.c_str(), CONFIG_LEVEL_PHONE);
                }
            }
        }
    }
}

void CDlgDirNew::OnBtnDeleteClick()
{
    QWidget * pFocusWidget = focusWidget();

    if (NULL == pFocusWidget)
    {
        return;
    }

    if (pFocusWidget->inherits("QLineEdit") || pFocusWidget->inherits("QTextEdit"))
    {
        //点击删除时，调用此函数，对当前焦点的文本输入框的内容进行退格删除
        qOnBackspaceClicked(this);
    }
}

void CDlgDirNew::OnBtnBackClick()
{
    if (!ProcessInputs(DIR_UI_ACTION_COMMON_BACK))
    {
        UIManager_PutPrivateDlg(this);
    }
}

void CDlgDirNew::OnHomeButtonClick()
{
    if (!ProcessInputs(DIR_UI_ACTION_COMMON_HOME))
    {
        CBaseDialog::OnHomeButtonClick();
    }
}

bool CDlgDirNew::ProcessInputs(int nUIAction)
{
    if (NULL == m_pListItemLayout)
    {
        return false;
    }

    m_pListItemLayout->ReturnAllEdit();

    if (m_pListItemLayout->IsInputEmpty())
    {
        return false;
    }

    MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey skDiscard(LANG_TRANSLATE(TRID_DISCARD), MessageBox_Discard);
    MsgBoxSoftkey skSave(LANG_TRANSLATE(TRID_SAVE), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(skCancel);
    vecSoftKey.push_back(skDiscard);
    vecSoftKey.push_back(skSave);

    QString strNote = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_SAVE_THE_CONTACT_OR_DISCARD_IT);

    MessageBox_ShowCommonMsgBox(this, strNote, vecSoftKey, -1, nUIAction);

    return true;
}

int CDlgDirNew::GetGroupId()
{
    return m_nAddGroupId;
}


void CDlgDirNew::SetBeforeGroupId(int nBeforeGroupId)
{
    m_nListGroupId = nBeforeGroupId;
}

bool CDlgDirNew::OnLastItemNext()
{
    return false;
}

bool CDlgDirNew::OnFirstItemPrevious()
{
    return false;
}

void CDlgDirNew::OnCleanContent(CFrameListBase * pFrameList)
{
}

void CDlgDirNew::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CDlgDirNew::OnLoadData pFrameList : %p nDataIndex : %d nDataCount : %d ContactType : %d",
               pFrameList, nDataIndex, nDataCount, m_nContactType);

    if (NULL == m_pListItemLayout || NULL == pFrameList || NULL == m_pFrameList)
    {
        DIRUI_WARN("CDlgDirNew::OnLoadData Error");
        return;
    }
    qBlockKeyboard(true);
    m_pFrameList->DetachAllItem();

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nTotalItemCount = m_pListItemLayout->GetItemCount();

    int nTotalCount = nTotalItemCount;

    if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        nTotalCount = 3;
    }
    else if (DIRECTORY_TYPE_LOCAL == m_nContactType && kszBlackListGroupId == m_nAddGroupId
             && nTotalCount > 3)
    {
        nTotalCount -= 3;

#if IF_FEATURE_FAVORITE_DIR
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            nTotalCount -= 1;
        }
#endif
    }

    if (m_bMutiPage)
    {
        DIRUI_INFO("nMaxPageTotalCount : %d nTotalItemCount : %d", nMaxPageTotalCount, nTotalCount);
        QRect rtRect = RECT_DIR_NEW_LIST_NORMAL;

        if (nTotalCount > nMaxPageTotalCount)
        {
            rtRect = RECT_DIR_CONTACT_ADD;
            m_btnImg.SetRect(RECT_DIR_CONTACT_IMAGE);
        }
        else
        {
            m_btnImg.SetRect(DLG_DIR_NEW_POS_IMAGE_CONTACT_LEFT, DLG_DIR_NEW_POS_IMAGE_CONTACT_TOP,
                             DLG_DIR_NEW_POS_IMAGE_CONTACT_WEIGHT, DLG_DIR_NEW_POS_IMAGE_CONTACT_HEIGHT);
        }

        if (NULL != m_pFrameList)
        {
            m_pFrameList->SetRect(rtRect);
            m_pFrameList->SetDelegate(this);
        }
    }

    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(nTotalCount);
    //

    int i = 0;
    for (; i < nMaxPageTotalCount && nDataIndex < nTotalCount; ++i, ++nDataIndex)
    {
        CDetailListItemBasePtr pItem = m_pListItemLayout->GetDetailListItem(nDataIndex);

        if (NULL == pItem)
        {
            continue;
        }

        if (0 == i)
        {
            pItem->SetDataIndex(INVALID_ITEM_INDEX);
        }

        pItem->SetDataIndex(nDataIndex);

        pItem->SetItemHeight(DLG_DIR_NEW_POS_LIST_ITEM_HEIGHT);

        m_pFrameList->AddItem(pItem);
    }

    if (m_bMutiPage)
    {
        update(m_btnImg.GetRect());
    }
    qBlockKeyboard(false);
    m_pFrameList->SetItemFocusByIndex(0);
    m_pFrameList->update();
}

bool CDlgDirNew::IsFrameListAttach(CFrameListBase * pFrameList)
{
    if (NULL == pFrameList || NULL == m_pFrameList)
    {
        return false;
    }

    return true;
}

void CDlgDirNew::OnFrameListDetach(CFrameListBase * pFrameList)
{
}

void CDlgDirNew::OnFrameListAttach(CFrameListBase * pFrameList)
{
}

bool CDlgDirNew::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, QRect & rtVolume,
                                 yl::string & strBGPath) const
{
    if (NULL != m_pComboBoxRing && focusWidget() == m_pComboBoxRing)
    {
        int nIndex = m_pComboBoxRing->currentIndex();

        //如果在第一项，也就是Auto项，不播放铃声
        if (0 == nIndex)
        {
            return false;
        }

        iMin = 0;
        iMax = 15;
        return true;
    }

    return false;
}

void CDlgDirNew::OnKeyboardShow()
{
    SetKeyboardEnterType();
    AdjustFrameList(false);
}

void CDlgDirNew::OnKeyboardHide()
{
    AdjustFrameList(true);
}

void CDlgDirNew::AdjustFrameList(bool bReset)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nTotalItemCount = m_pListItemLayout->GetItemCount();

    int nTotalCount = nTotalItemCount;
    QRect rtRect = RECT_DIR_NEW_LIST_NORMAL;

    if (DIRECTORY_TYPE_BROADSOFT == m_nContactType)
    {
        nTotalCount = 3;
    }
    else if (DIRECTORY_TYPE_LOCAL == m_nContactType && kszBlackListGroupId == m_nAddGroupId
             && nTotalCount > 2)
    {
        nTotalCount -= 2;
    }

    if (m_bMutiPage)
    {
        if (nTotalCount > nMaxPageTotalCount)
        {
            rtRect = RECT_DIR_CONTACT_ADD;
        }
    }

    DirUI_AdjustFrameList(m_pFrameList
                          , bReset
                          , rtRect
                          , DLG_DIR_NEW_ITEM_OFFSET
                          , RECT_DIRECTORY.y());
}
