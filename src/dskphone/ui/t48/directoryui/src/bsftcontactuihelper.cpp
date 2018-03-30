#include <QtGui>
#include "bsftcontactuihelper.h"
#include "bsftcontactcontroler.h"
#include "dirctrlfactory.h"
#include "directorymgr.h"
#include "settingdelegate.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "contactdetailtitle.h"
#include "contactlisttitle.h"
#include "contactupdatelisttitle.h"
#include "moddirectoryui.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/detaillistitem.h"
#include "baseui/editlistitem.h"
#include "baseui/framelist.h"
#include "baseui/frametiptitle.h"
#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

namespace
{
enum
{
    BS_DETAIL_ITEM_INDEX_FIRST_NAME     = 0,
    BS_DETAIL_ITEM_INDEX_LAST_NAME,
    BS_DETAIL_ITEM_INDEX_NUMBER,
    BS_DETAIL_ITEM_INDEX_EXTENSION,
    BS_DETAIL_ITEM_INDEX_MOBILE,
    BS_DETAIL_ITEM_INDEX_DEPARTMENT,
    BS_DETAIL_ITEM_INDEX_EMAIL,
    BS_DETAIL_ITEM_INDEX_GROUP,
    BS_DETAIL_ITEM_INDEX_MAX,
};
#define BROADSOFT_SUPER_SEARCH_ITEM_COUNT 6
#define BROADSOFT_GROUP_NAME_MAX_LENGTH 205
}


CBSFTContactUIHelper::CBSFTContactUIHelper()
    : CCommonUIHelper(DIRECTORY_TYPE_BROADSOFT)
    , m_pDetailLayout(NULL)
    , m_pAdditionLayout(NULL)
    , m_pSearchLayout(NULL)
    , m_bDetailEditing(false)
{
}

CBSFTContactUIHelper::~CBSFTContactUIHelper()
{
}

CBSFTContactUIHelper * CBSFTContactUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_BROADSOFT != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CBSFTContactUIHelper *)pUIHelper.Get();
}

void CBSFTContactUIHelper::GetGroupDisplayName(int nGroupId, const yl::string & strGroupName,
        QString & strDisplayName)
{
    if (kszAllContactId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_ALL_CONTACTS);
        return;
    }

    if (!strGroupName.empty())
    {
        strDisplayName = toQString(strGroupName, true);
        return;
    }

    if (kszPersonalId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_PERSONAL);
    }
    else if (kszGroupId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_GROUP);
    }
    else if (kszEnterPriseId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_ENTERPRISE);
    }
    else if (kszGroupCommonId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_GROUP_COMMON);
    }
    else if (kszEnterPriseCommonId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_ENTERPRISE_COMMON);
    }
    else if (kszCustomId == nGroupId)
    {
        strDisplayName = LANG_TRANSLATE(TRID_CUSTOM_CONTACT);
    }
}

void CBSFTContactUIHelper::OnPreciseSearchResult()
{

}

void CBSFTContactUIHelper::SetAdditionLayout(CLocalDetailLayoutPtr pLayout)
{
    DIRUI_INFO("CBSFTContactUIHelper::SetAdditionLayout(%p)", pLayout);

    m_pAdditionLayout = pLayout;

    if (NULL == m_pAdditionLayout)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_BROADSOFT);
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    yl::string strName;
    yl::string strNumber;

    GetAttributeByName(0, ATTR_NAME, strName);
    GetAttributeByName(0, ATTR_NUMBER, strNumber);

    CEditListItemPtr pNameItem = m_pAdditionLayout->GetNameEditItem();

    if (NULL != pNameItem)
    {
        pNameItem->SetValue(toQString(strName, true));
    }

    CEditListItemPtr pNumberItem = m_pAdditionLayout->GetOfficeNoEditItem();

    if (NULL != pNumberItem)
    {
        pNumberItem->SetValue(toQString(strNumber, true));
    }
}

void CBSFTContactUIHelper::SetDetailLayout(CBroadSoftDetailLayoutPtr pDetailLayout)
{
    m_pDetailLayout = pDetailLayout;

    if (NULL == m_pDetailLayout)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    CEditListItemPtr pGroupEdit = m_pDetailLayout->GetGroupEditItem();

    if (NULL != pGroupEdit)
    {
        QString strGroupDisplayName;
        yl::string strGroupName;

        pBSFTContact->GetGroupName(kszPersonalId, strGroupName);
        GetGroupDisplayName(kszPersonalId, strGroupName, strGroupDisplayName);
        strGroupDisplayName = LANG_PARSE(strGroupDisplayName);

        QFont f;
        f.setPointSize(16);

        QString strGroupDisplayName_Cut = CutOffText(strGroupDisplayName, QFontMetrics(f), BROADSOFT_GROUP_NAME_MAX_LENGTH);
        if (strGroupDisplayName_Cut.length() != strGroupDisplayName.length())
        {
            pGroupEdit->SetValue(strGroupDisplayName_Cut);
        }
        else
        {
            pGroupEdit->SetValue(strGroupDisplayName);
        }
    }
}

void CBSFTContactUIHelper::SetSearchLayout(CBroadSoftSearchLayoutPtr pSearchLayout)
{
    m_pSearchLayout = pSearchLayout;

    if (NULL == m_pSearchLayout)
    {
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    CComboBoxListItemPtr pGroupItem = m_pSearchLayout->GetGroupItem();

    if (NULL == pGroupItem)
    {
        return;
    }

    pGroupItem->ClearValue();

    // 获取组名列表数据成功, 加入可选择的组
    int nGroupId = pBSFTContact->GetGroupId();
    VEC_CONTACT_ID vecId;

    pBSFTContact->GetAllGroup(vecId);

    YLVector<yl::string> vecGroupName;
    int nSize = vecId.size();
    int nIndex = 0;

    for (int i = 0; i < nSize; ++i)
    {
        int nId = vecId[i];

        if (nId == nGroupId)
        {
            nIndex = i;
        }

        SmartPtr pDetail = pBSFTContact->GetDetail(nId);

        if (pDetail.IsEmpty())
        {
            continue;
        }

        QString strDisplayName;

        GetGroupDisplayName(nId, pDetail->GetDisplayName(), strDisplayName);

        pGroupItem->AddValue(strDisplayName, QVariant::fromValue<int>(nId));
    }

    pGroupItem->SetCurrentIndex(nIndex);
}

CBroadSoftSearchLayoutPtr CBSFTContactUIHelper::GetSearchLayout()
{
    return m_pSearchLayout;
}

void CBSFTContactUIHelper::SetInputFocus(const char * pszAttr)
{
    if (NULL == pszAttr)
    {
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_NAME))
    {
        CEditListItem * pNameItem = NULL;

        if (NULL != m_pAdditionLayout)
        {
            pNameItem = m_pAdditionLayout->GetNameEditItem();
        }
        else if (NULL != m_pDetailLayout)
        {
            pNameItem = m_pDetailLayout->GetNameEditItem();
        }

        if (NULL != pNameItem)
        {
            setCurrentPageForItem(pNameItem);
            pNameItem->FocusItem(true);
        }
        return;
    }

    if (0 == strcmp(pszAttr, ATTR_NUMBER))
    {
        CEditListItem * pNumberItem = NULL;

        if (NULL != m_pAdditionLayout)
        {
            pNumberItem = m_pAdditionLayout->GetOfficeNoEditItem();
        }
        else if (NULL != m_pDetailLayout)
        {
            pNumberItem = m_pDetailLayout->GetNumberEditItem();
        }

        if (NULL != pNumberItem)
        {
            setCurrentPageForItem(pNumberItem);
            pNumberItem->FocusItem(true);
        }
    }
}

void CBSFTContactUIHelper::OnDetailCtrlFocus(bool bFocus)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact || !pBSFTContact->IsDetailState())
    {
        return;
    }

    if (bFocus == m_bDetailEditing)
    {
        return;
    }

    m_bDetailEditing = bFocus;
    InitDetailTitle();
}

void CBSFTContactUIHelper::InitSettingTitle()
{
    DIRUI_INFO("CBSFTContactUIHelper::InitSettingTitle");

    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (pControler.IsEmpty())
    {
        return;
    }

    bool bAdd = true;
    CContactListTitlePtr pContactListTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactListTitle::IsContactListTitle(pTitle))
    {
        bAdd = false;
        pContactListTitle = (CContactListTitlePtr)pTitle;
    }
    else
    {
        pContactListTitle = new CContactListTitle;
        if (NULL == pContactListTitle)
        {
            return;
        }
    }

    pContactListTitle->SetAction(pDelegate.Get());
    pContactListTitle->SetBtnCount(1);

    int nGroupId = pControler->GetGroupId();

    if (kszPersonalId == nGroupId)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
    }
    else if (kszAllContactId == nGroupId)
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));
    }
    else
    {
        pContactListTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));
    }

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pContactListTitle);
    }
    else
    {
        pContactListTitle->RefreshTitle();
    }
}

void CBSFTContactUIHelper::InitDetailTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    bool bAdd = true;
    CContactDetailTitlePtr pDetailTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactDetailTitle::IsContactDetailTitle(pTitle))
    {
        bAdd = false;
        pDetailTitle = (CContactDetailTitlePtr)pTitle;
    }
    else
    {
        pDetailTitle = new CContactDetailTitle;
        if (NULL == pDetailTitle)
        {
            return;
        }
    }

    if (!CBSFTContactControler::IsExitPersonal())
    {
        pDetailTitle->SetBtnCount(1);
        pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
    }
    else
    {
        if (m_bDetailEditing)
        {
            pDetailTitle->SetBtnCount(1);
        }
        else
        {
            pDetailTitle->SetBtnCount(2);
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DIAL_CONTACT, LANG_TRANSLATE(TRID_DIAL));
        }

        if (pBSFTContact->IsPersonal(pBSFTContact->GetContactId()))
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_DEL_CONTACT, LANG_TRANSLATE(TRID_DELETE));
        }
        else
        {
            pDetailTitle->AddButton(LIST_TITLE_ACTION_MOVE_CONTACT, LANG_TRANSLATE(TRID_ADD_PERSONAL));
        }
    }

    pDetailTitle->SetAction(pDelegate.Get());

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pDetailTitle);
    }
    else
    {
        pDetailTitle->RefreshTitle();
    }
}

int CBSFTContactUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        if (LIST_TITLE_ACTION_CONTACT_UPDATE == nTitleAction)
        {
            return DIR_ACTION_BSFT_UPDATE;
        }

        return DIR_ACTION_NONE;
    }

    switch (nTitleAction)
    {
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_BSFT_BATCH_DEL_PERSONAL;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_BSFT_DEL_PERSONAL;
            }
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            if (DIR_UI_STATE_SETTING == nUIState)
            {
                return DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL;
            }
            else if (DIR_UI_STATE_DETAIL == nUIState)
            {
                return DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL;
            }
        }
        break;
    default:
        break;
    }

    return CDirUIHelper::GetActionByTitle(nUIState, nTitleAction);
}

void CBSFTContactUIHelper::InitPreciseSearchTitle()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    bool bAdd = true;
    CFrameTipTitlePtr pTipTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CFrameTipTitle::IsTipTitle(pTitle))
    {
        bAdd = false;
        pTipTitle = (CFrameTipTitlePtr)pTitle;
    }
    else
    {
        pTipTitle = new CFrameTipTitle;
        if (NULL == pTipTitle)
        {
            return;
        }
    }

    pTipTitle->SetTip(LANG_TRANSLATE(TRID_PLEASE_ENTER_FILTER_CONDITIONS));

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pTipTitle);
    }
    else
    {
        pTipTitle->RefreshTitle();
    }
}

void CBSFTContactUIHelper::CheckSettingTitle()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return;
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    InitSettingTitle();

    yl::string strGroupName;

    pBSFTContact->GetGroupName(pBSFTContact->GetGroupId(), strGroupName);

    SetUpdateTitleGroupName(toQString(strGroupName, true));

    int nTotalCount = GetAllContactAmount();

    RefreshUpdateTitle(nTotalCount);
}

bool CBSFTContactUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (DIR_UI_STATE_BROWSER == nUIState || (DIR_UI_STATE_SETTING == nUIState && IsShowUpateTitile()))
    {
        m_pFrameList->SetTotalDataCount(nTotalCount);
        RefreshUpdateTitle(GetAllContactAmount());
        return true;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == nUIState)
    {
        nUIState = DIR_UI_STATE_SEARCH;
    }

    return CDirUIHelper::RefreshListItemAmount(nUIState, nFromIndex, nTotalCount);
}

void CBSFTContactUIHelper::OnAttachFrameList()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    int nState = pDelegate->GetState();

    if (DIR_UI_STATE_BROWSER == nState)
    {
        InitUpdateTitle(configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_SETTING == nState)
    {
        InitSettingTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_DETAIL == nState)
    {
        InitDetailTitle();
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_DETAIL_ITEM_COUNT);
        return;
    }

    if (DIR_UI_STATE_PRECISE_SEARCH == nState)
    {
        CDirectoryControlerPtr pControler = pDelegate->GetControler();
        CBSFTContactControler * pBSFTControler = CBSFTContactControler::GetControler(pControler);

        // 从详情退回高级搜索界面时FrameList保持原来的设置
        if (NULL != pBSFTControler && pBSFTControler->IsPreciseSearchResult())
        {
            return;
        }

        InitPreciseSearchTitle();
        m_pFrameList->SetOnlyTitleLine(true);
        m_pFrameList->SetMaxPageItemCount(BROADSOFT_SUPER_SEARCH_ITEM_COUNT);
        m_pFrameList->SetItemClickBG("");
        return;
    }

    CCommonUIHelper::OnAttachFrameList();
}

void CBSFTContactUIHelper::OnGroupChange()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

    if (NULL == pBSFTContact)
    {
        return;
    }

    int nGroupId = pBSFTContact->GetGroupId();

    DIRUI_INFO("CBSFTContactUIHelper::OnGroupChange() GroupId:%d Setting:%d",
               nGroupId, (pBSFTContact->IsSettingState()));

    if (pBSFTContact->IsSettingState())
    {
        if (kszCustomId != nGroupId)
        {
            CheckSettingTitle();
            return;
        }

        if (!IsShowUpateTitile())
        {
            InitUpdateTitle(configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1);
        }
    }
    else if (!IsShowUpateTitile())
    {
        return;
    }

    yl::string strGroupName;

    if (kszAllContactId != nGroupId && !pBSFTContact->GetGroupName(nGroupId, strGroupName))
    {
        DIRUI_ERR("CBSFTContactControler::GetGroupName(%d) fail in OnGroupChange", nGroupId);
    }

    QString strDisplayName;

    GetGroupDisplayName(nGroupId, strGroupName, strDisplayName);

    SetUpdateTitleGroupName(strDisplayName, true);


}

bool CBSFTContactUIHelper::FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount)
{
    DIRUI_INFO("CRemoteUIHelper::FillDetail(Id:%d Detail:%p DataIndex:%d DataCount:%d) FrameList:%p",
               nId, pDetail.Get(), nDataIndex, nDataCount, m_pFrameList);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    switch (pDetail->GetType())
    {
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            return FillGroupDetail(nDataIndex, nDataCount, (ContactBroadSoftGroup *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_PERSONAL:
        {
            return FillPersonalDetail((ContactBroadSoftPersonal *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            return FillCommonDetail((ContactBroadSoftCommon *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        {
            return FillEnterpriseDetail(nDataIndex, nDataCount, (ContactBroadSoftEnterprise *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        {
            return FillCustomDetail(nDataIndex, nDataCount, (ContactBroadSoftCustom *)pDetail.Get());
        }
        break;
    default:
        {
            DIRUI_ERR("CBSFTContactUIHelper::FillDetail fail, Type=%d", pDetail->GetType());
        }
        break;
    }

    return false;
}

bool CBSFTContactUIHelper::FillPersonalDetail(ContactBroadSoftPersonal * pContact)
{
    if (NULL == pContact || NULL == m_pFrameList || NULL == m_pDetailLayout)
    {
        return false;
    }

    bool bPersonal = m_pFrameList->IsItemAttach();

    m_pFrameList->ClearContent();
    m_pFrameList->SetTotalDataCount(3);

    m_pFrameList->SetItemAttach();

    if (!bPersonal)
    {
        InitDetailTitle();
    }

    CEditListItemPtr pGroupEdit = m_pDetailLayout->GetGroupEditItem();

    if (NULL != pGroupEdit)
    {
        pGroupEdit->SetAction(NULL);
        pGroupEdit->SetDataIndex(0);
        m_pFrameList->AddItem(pGroupEdit);
    }

    CEditListItemPtr pNameEdit = m_pDetailLayout->GetNameEditItem();

    if (NULL != pNameEdit)
    {
        pNameEdit->SetAction(NULL);
        pNameEdit->SetDataIndex(1);
        pNameEdit->SetValue(toQString(pContact->GetDisplayName(), true));
        m_pFrameList->AddItem(pNameEdit);
    }

    CEditListItemPtr pNumberEdit = m_pDetailLayout->GetNumberEditItem();

    if (NULL != pNumberEdit)
    {
        yl::string strNumber;

        pContact->GetNumberByName(kszBsftNumber, strNumber);
        pNumberEdit->SetAction(NULL);
        pNumberEdit->SetDataIndex(2);
        pNumberEdit->SetValue(toQString(strNumber, true));
        m_pFrameList->AddItem(pNumberEdit);
    }

    m_pFrameList->update();

    return true;
}

bool CBSFTContactUIHelper::FillCommonDetail(ContactBroadSoftCommon * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = TRID_NAME;
    stAttr.m_strValue = pContact->GetDisplayName();
    vecAttr.push_back(stAttr);

    stAttr.m_strName = TRID_NUMBER;
    pContact->GetNumberByName(kszBsftNumber, stAttr.m_strValue);
    vecAttr.push_back(stAttr);

    return FillNormalDetail(0, 2, vecAttr);
}

bool CBSFTContactUIHelper::FillEnterpriseDetail(int nDataIndex, int nDataCount,
        ContactBroadSoftEnterprise * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;
    int nTotalAmount = BS_DETAIL_ITEM_INDEX_EMAIL + 1;
    int nTmpDataIndex = nDataIndex;

    for (int i = 0; i < nDataCount && nTmpDataIndex < nTotalAmount; ++i, ++nTmpDataIndex)
    {
        switch (nTmpDataIndex)
        {
        case BS_DETAIL_ITEM_INDEX_FIRST_NAME:
            {
                stAttr.m_strName = TRID_FIRST_NAME;
                stAttr.m_strValue = pContact->m_strFirstName;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_LAST_NAME:
            {
                stAttr.m_strName = TRID_LAST_NAME;
                stAttr.m_strValue = pContact->m_strLastName;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_NUMBER:
            {
                stAttr.m_strName = TRID_NUMBER;

                if (!pContact->GetNumberByName(kszBsftNumber, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_EXTENSION:
            {
                stAttr.m_strName = TRID_EXTENSION_NUMBER;

                if (!pContact->GetNumberByName(kszBsftExtension, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_MOBILE:
            {
                stAttr.m_strName = TRID_MOBILE_NUMBER;

                if (!pContact->GetNumberByName(kszBsftMobile, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_DEPARTMENT:
            {
                stAttr.m_strName = TRID_DEPARTMENT;
                stAttr.m_strValue = pContact->m_strDepartment;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_EMAIL:
            {
                stAttr.m_strName = TRID_EMAIL;
                stAttr.m_strValue = pContact->m_strEmail;
                vecAttr.push_back(stAttr);
            }
            break;
        default:
            break;
        }
    }

    return FillNormalDetail(nDataIndex, nTotalAmount, vecAttr);
}

bool CBSFTContactUIHelper::FillCustomDetail(int nDataIndex, int nDataCount,
        ContactBroadSoftCustom * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;
    int nTotalAmount = BS_DETAIL_ITEM_INDEX_EMAIL + 1;
    int nTmpDataIndex = nDataIndex;

    for (int i = 0; i < nDataCount && nTmpDataIndex < nTotalAmount; ++i, ++nTmpDataIndex)
    {
        switch (nTmpDataIndex)
        {
        case BS_DETAIL_ITEM_INDEX_FIRST_NAME:
            {
                stAttr.m_strName = TRID_FIRST_NAME;
                stAttr.m_strValue = pContact->m_strFirstName;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_LAST_NAME:
            {
                stAttr.m_strName = TRID_LAST_NAME;
                stAttr.m_strValue = pContact->m_strLastName;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_NUMBER:
            {
                stAttr.m_strName = TRID_NUMBER;

                if (!pContact->GetNumberByName(kszBsftNumber, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_EXTENSION:
            {
                stAttr.m_strName = TRID_EXTENSION_NUMBER;

                if (!pContact->GetNumberByName(kszBsftExtension, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_MOBILE:
            {
                stAttr.m_strName = TRID_MOBILE_NUMBER;

                if (!pContact->GetNumberByName(kszBsftMobile, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_DEPARTMENT:
            {
                stAttr.m_strName = TRID_DEPARTMENT;
                stAttr.m_strValue = pContact->m_strDepartment;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_EMAIL:
            {
                stAttr.m_strName = TRID_EMAIL;
                stAttr.m_strValue = pContact->m_strEmail;
                vecAttr.push_back(stAttr);
            }
            break;
        default:
            break;
        }
    }

    return FillNormalDetail(nDataIndex, nTotalAmount, vecAttr);
}

bool CBSFTContactUIHelper::FillGroupDetail(int nDataIndex, int nDataCount,
        ContactBroadSoftGroup * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    VEC_ATTRIBUTE vecAttr;
    ST_CONTACT_ATTRIBUTE stAttr;
    int nTotalAmount = BS_DETAIL_ITEM_INDEX_EMAIL + 1;
    int nTmpDataIndex = nDataIndex;

    for (int i = 0; i < nDataCount && nTmpDataIndex < nTotalAmount; ++i, ++nTmpDataIndex)
    {
        switch (nTmpDataIndex)
        {
        case BS_DETAIL_ITEM_INDEX_FIRST_NAME:
            {
                stAttr.m_strName = TRID_FIRST_NAME;
                stAttr.m_strValue = pContact->m_strFirstName;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_LAST_NAME:
            {
                stAttr.m_strName = TRID_LAST_NAME;
                stAttr.m_strValue = pContact->m_strLastName;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_NUMBER:
            {
                stAttr.m_strName = TRID_NUMBER;

                if (!pContact->GetNumberByName(kszBsftNumber, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_EXTENSION:
            {
                stAttr.m_strName = TRID_EXTENSION_NUMBER;

                if (!pContact->GetNumberByName(kszBsftExtension, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_MOBILE:
            {
                stAttr.m_strName = TRID_MOBILE_NUMBER;

                if (!pContact->GetNumberByName(kszBsftMobile, stAttr.m_strValue))
                {
                    stAttr.m_strValue = "";
                }

                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_DEPARTMENT:
            {
                stAttr.m_strName = TRID_DEPARTMENT;
                stAttr.m_strValue = pContact->m_strDepartment;
                vecAttr.push_back(stAttr);
            }
            break;
        case BS_DETAIL_ITEM_INDEX_EMAIL:
            {
                stAttr.m_strName = TRID_EMAIL;
                stAttr.m_strValue = pContact->m_strEmail;
                vecAttr.push_back(stAttr);
            }
            break;
        default:
            break;
        }
    }

    return FillNormalDetail(nDataIndex, nTotalAmount, vecAttr);
}

bool CBSFTContactUIHelper::FillNormalDetail(int nDataIndex, int nTotalCount,
        VEC_ATTRIBUTE & vecAttr)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    bool bPersonal = m_pFrameList->IsItemAttach();

    if (bPersonal || !m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
    {
        m_pFrameList->ClearContent();
    }

    if (bPersonal)
    {
        InitDetailTitle();
    }

    int nMaxPageCount = m_pFrameList->GetMaxPageItemCount();
    int nSize = vecAttr.size();
    int nItemIndex = 0;

    if (nMaxPageCount > nSize)
    {
        nMaxPageCount = nSize;
    }

    for (; nItemIndex < nSize; ++nItemIndex)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = vecAttr[nItemIndex];
        QString strTitle = LANG_TRANSLATE(stAttr.m_strName.c_str());
        QString strValue = toQString(stAttr.m_strValue, true);

        RefreshDetailItem(nItemIndex, (nDataIndex + nItemIndex), nTotalCount, strTitle, strValue);
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CBSFTContactUIHelper::FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                                        SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    int nDetailType = pDetail->GetType();
    // 分段下载特殊处理，此时的pDetail是随便生成的，需要特定判断。
    if (nId == kszViewMoreId)
    {
        nDetailType = DIR_TYPE_BROADSOFT_ITEM_COMMON;
    }

    if (DIR_TYPE_GROUP == nDetailType || DIR_TYPE_BROADSOFT_MENU == nDetailType)
    {
        QString strDisplayName;

        GetGroupDisplayName(nId, pDetail->GetDisplayName(), strDisplayName);

        return RefreshGroupMenuItem(nId, nItemIndex, nDataIndex, nDataCount, strDisplayName);
    }

    switch (nDetailType)
    {
    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        {
            return FillGroupItem(nId, nItemIndex, nDataIndex, nDataCount,
                                 (ContactBroadSoftGroup *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_PERSONAL:
        {
            return FillPersonalItem(nId, nItemIndex, nDataIndex, nDataCount,
                                    (ContactBroadSoftPersonal *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        {
            return FillCommonItem(nId, nItemIndex, nDataIndex, nDataCount,
                                  (ContactBroadSoftCommon *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        {
            return FillEnterpriseItem(nId, nItemIndex, nDataIndex, nDataCount,
                                      (ContactBroadSoftEnterprise *)pDetail.Get());
        }
        break;
    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        {
            return FillCustomItem(nId, nItemIndex, nDataIndex, nDataCount,
                                  (ContactBroadSoftCustom *)pDetail.Get());
        }
        break;
    default:
        break;
    }

    DIRUI_ERR("CBSFTContactUIHelper::FillListItem(Id:%d ItemIndex:%d DataIndex:%d DataCount:%d Detail:%p) fail",
              nId, nItemIndex, nDataIndex, nDataCount, pDetail.Get());
    return false;
}

bool CBSFTContactUIHelper::FillCommonItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        ContactBroadSoftCommon * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum;
    //分段下载跳过，因此时pContact是虚假的
    if (nId != kszViewMoreId)
    {
        pContact->GetNumberByName(kszBsftNumber, strNum);
    }

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CBSFTContactUIHelper::FillPersonalItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        ContactBroadSoftPersonal * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum;

    pContact->GetNumberByName(kszBsftNumber, strNum);

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CBSFTContactUIHelper::FillEnterpriseItem(int nId, int nItemIndex, int nDataIndex,
        int nDataCount, ContactBroadSoftEnterprise * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum = "";
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    if (NULL != pBSFTContact)
    {
        if (pBSFTContact->IsSearchState())
        {
            strNum = pContact->GetSearchShowNumber(pBSFTContact->GetSearchKey());
        }
    }

    if (strNum.empty())
    {
        if (!pContact->GetNumberByName(kszBsftExtension, strNum) || strNum.empty())
        {
            if (!pContact->GetNumberByName(kszBsftNumber, strNum) || strNum.empty())
            {
                pContact->GetNumberByName(kszBsftMobile, strNum);
            }
        }
    }

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CBSFTContactUIHelper::FillCustomItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        ContactBroadSoftCustom * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    if (NULL != pBSFTContact)
    {
        if (pBSFTContact->IsSearchState())
        {
            strNum = pContact->GetSearchShowNumber(pBSFTContact->GetSearchKey());
        }
    }

    if (strNum.empty())
    {
        if (!pContact->GetNumberByName(kszBsftExtension, strNum) || strNum.empty())
        {
            if (!pContact->GetNumberByName(kszBsftNumber, strNum) || strNum.empty())
            {
                pContact->GetNumberByName(kszBsftMobile, strNum);
            }
        }
    }

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

bool CBSFTContactUIHelper::FillGroupItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        ContactBroadSoftGroup * pContact)
{
    if (NULL == pContact)
    {
        return false;
    }

    yl::string strNum = "";

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    if (NULL != pBSFTContact)
    {
        if (pBSFTContact->IsSearchState())
        {
            strNum = pContact->GetSearchShowNumber(pBSFTContact->GetSearchKey());
        }
    }

    if (strNum.empty())
    {
        if (!pContact->GetNumberByName(kszBsftExtension, strNum) || strNum.empty())
        {
            if (!pContact->GetNumberByName(kszBsftNumber, strNum) || strNum.empty())
            {
                pContact->GetNumberByName(kszBsftMobile, strNum);
            }
        }
    }

    yl::string strHead = "";

    return RefreshContactItem(nItemIndex, nDataIndex, nDataCount, nId, pContact->GetDisplayName(),
                              strNum, strHead);
}

void CBSFTContactUIHelper::CheckPreciseSearchUI()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    m_pFrameList->SetOnlyTitleLine(false);
    m_pFrameList->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP, PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                 NULL);
}

bool CBSFTContactUIHelper::LoadBSFTPreciseSearchData(int nDataIndex, int nDataCount)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    DIRUI_INFO("LoadBSFTPreciseSearchData(Index:%d Count:%d)", nDataIndex, nDataCount);

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        DIRUI_ERR("CBSFTContactUIHelper::LoadBSFTPreciseSearchData() fail when delegate is empty");
        return false;
    }

    InitPreciseSearchTitle();

    m_pFrameList->SetMaxPageItemCount(BROADSOFT_SUPER_SEARCH_ITEM_COUNT);

    if (NULL == m_pSearchLayout)
    {
        DIRUI_ERR("SearchLayout is empty when LoadBSFTPreciseSearchData");
        return false;
    }

    m_pFrameList->ClearContent();
    m_pFrameList->SetItemAttach();
    m_pFrameList->SetTotalDataCount(BS_SEARCH_ITEM_INDEX_MAX);

    for (int i = 0;
            i < BROADSOFT_SUPER_SEARCH_ITEM_COUNT && nDataIndex < BS_SEARCH_ITEM_INDEX_MAX;
            ++i, ++nDataIndex)
    {
        CDetailListItemBasePtr pItem = m_pSearchLayout->GetDetailListItem(nDataIndex);

        if (NULL == pItem)
        {
            continue;
        }

        if (0 == i)
        {
            pItem->SetDataIndex(INVALID_ITEM_INDEX);
        }

        pItem->SetAction(pDelegate.Get());
        m_pFrameList->AddItem(pItem);
        pItem->SetDataIndex(nDataIndex);
    }

    m_pFrameList->update();

    return true;
}

bool CBSFTContactUIHelper::GetBSFTPersonalData(ContactBroadSoftPersonal & itemData)
{
    CEditListItemPtr pNameItem = NULL;
    CEditListItemPtr pNumberItem = NULL;

    if (NULL != m_pAdditionLayout)
    {
        pNameItem = m_pAdditionLayout->GetNameEditItem();
        pNumberItem = m_pAdditionLayout->GetOfficeNoEditItem();
    }
    else if (NULL != m_pDetailLayout)
    {
        pNameItem = m_pDetailLayout->GetNameEditItem();
        pNumberItem = m_pDetailLayout->GetNumberEditItem();
    }

    if (NULL == pNameItem || NULL == pNumberItem)
    {
        return false;
    }

    pNameItem->ReturnAllInput();
    pNumberItem->ReturnAllInput();

    itemData.m_strDisplayName = pNameItem->GetValue().trimmed().toUtf8().data();

    yl::string strNumber = pNumberItem->GetValue().trimmed().toUtf8().data();

    itemData.SetNumber(strNumber);

    return true;
}

bool CBSFTContactUIHelper::GetBSFTPreciseSearchData(int & nGroupId,
        ContactBroadSoftItemData & itemData)
{
    if (NULL == m_pSearchLayout)
    {
        return false;
    }

    m_pSearchLayout->ReturnAllEdit();

    CEditListItemPtr pFirstNameItem = m_pSearchLayout->GetFirstNameItem();
    yl::string strFirstName;

    if (NULL != pFirstNameItem)
    {
        strFirstName = pFirstNameItem->GetValue().toUtf8().data();
    }

    CEditListItemPtr pLastNameItem = m_pSearchLayout->GetLastNameItem();
    yl::string strLastName;

    if (NULL != pLastNameItem)
    {
        strLastName = pLastNameItem->GetValue().toUtf8().data();
    }

    itemData.m_strDisplayName = strFirstName + "," + strLastName;

    CEditListItemPtr pNumberItem = m_pSearchLayout->GetNumberItem();

    if (NULL != pNumberItem)
    {
        itemData.m_strNumber = pNumberItem->GetValue().toUtf8().data();
    }

    CEditListItemPtr pExtensionItem = m_pSearchLayout->GetExtensionItem();

    if (NULL != pExtensionItem)
    {
        itemData.m_strExtension = pExtensionItem->GetValue().toUtf8().data();
    }

    CEditListItemPtr pMobileItem = m_pSearchLayout->GetMobileItem();

    if (NULL != pMobileItem)
    {
        itemData.m_strMobile = pMobileItem->GetValue().toUtf8().data();
    }

    CEditListItemPtr pEMailItem = m_pSearchLayout->GetEmailItem();

    if (NULL != pEMailItem)
    {
        itemData.m_strEmail = pEMailItem->GetValue().toUtf8().data();
    }

    CEditListItemPtr pDepartmentItem = m_pSearchLayout->GetDepartmentItem();

    if (NULL != pDepartmentItem)
    {
        itemData.m_strDepartment = pDepartmentItem->GetValue().toUtf8().data();
    }

    CComboBoxListItemPtr pGroupItem = m_pSearchLayout->GetGroupItem();

    if (NULL != pGroupItem)
    {
        nGroupId = pGroupItem->GetItemData();
    }

    char szBuffer[32];

    snprintf(szBuffer, sizeof(szBuffer), "%d", nGroupId);
    szBuffer[sizeof(szBuffer) - 1] = '\0';
    itemData.m_strGroupId = szBuffer;

    return true;
}


void CBSFTContactUIHelper::RefreshSearchTitle(int nTotalCount)
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_BROADSOFT);
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    int nHasNextPage = 0;
    if (NULL != pBSFTContact)
    {

        nHasNextPage = (BsftDir_HasNextPage(pBSFTContact->GetLogicListGroupId()) ? 1 : 0);
        nTotalCount -= nHasNextPage;
    }
    CDirUIHelper::RefreshSearchTitle(nTotalCount);
}

void CBSFTContactUIHelper::RefreshUpdateTitle(int nContactAmount, bool bRefresh /*= false*/)
{
    CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(DIRECTORY_TYPE_BROADSOFT);
    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
    int nHasNextPage = 0;
    if (NULL != pBSFTContact)
    {
        if (kszCustomId == pBSFTContact->GetLogicListGroupId())
        {
            ContactListData listData;
            listData.m_nIndexFrom = 0;
            listData.m_nCountToGet = 0;
            if (Dir_GetContactListByGroupId(&listData, kszCustomId, (DIR_TYPE)GetType()))
            {
                int n_TotalHasNext = 0;
                for (int i = 0; i < listData.m_listId.size(); i++)
                {
                    int & nId = listData.m_listId[i];
                    n_TotalHasNext += (BsftDir_HasNextPage(nId) ? 1 : 0);
                }
                nContactAmount -= n_TotalHasNext;
            }
        }
        else
        {
            nHasNextPage = BsftDir_HasNextPage(pBSFTContact->GetLogicListGroupId()) ? 1 : 0;
            nContactAmount -= nHasNextPage;
        }
    }
    CCommonUIHelper::RefreshUpdateTitle(nContactAmount, bRefresh);
}
