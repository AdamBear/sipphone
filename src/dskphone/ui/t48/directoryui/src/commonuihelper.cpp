#include <QtGui>
#include "commonuihelper.h"
#include "directorymgr.h"
#include "commondirctrl.h"
#include "settingdelegate.h"
#include "callloglistitem.h"
#include "contactupdatelisttitle.h"
#include "translateiddefine.h"
#include "bsftpwdmgr.h"
#include "baseui/framelist.h"
#include "baseui/iconmenulistitem.h"
#include "baseui/t4xpicpath.h"
#include "messagebox/modmessagebox.h"
#include "qtcommon/qmisc.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "uikernel/languagehelper.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#if IF_FEATURE_GBBOOK
#include "groupdelegate.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactcontroler.h"
#endif


CCommonUIHelper::CCommonUIHelper(int nType)
    : CDirUIHelper(nType)
{
}

CCommonUIHelper::~CCommonUIHelper()
{
}

CCommonUIHelper * CCommonUIHelper::GetUIHelper(CDirUIHelperPtr & pUIHelper)
{
    if (pUIHelper.IsEmpty())
    {
        return NULL;
    }

    int nType = pUIHelper->GetType();

    if (DIRECTORY_TYPE_LOCAL != nType && DIRECTORY_TYPE_REMOTE != nType
            && DIRECTORY_TYPE_LDAP != nType && DIRECTORY_TYPE_BROADSOFT != nType
            && CALL_LOG_TYPE_LOCAL != nType && CALL_LOG_TYPE_BROADSOFT != nType
            && DIRECTORY_TYPE_REMOTE_SEARCH != nType && DIRECTORY_TYPE_BLUETOOTH != nType
#if IF_BUG_PLCMDIR
            && DIRECTORY_TYPE_PLCM != nType
#endif
#if IF_FEATURE_GBBOOK
            && DIRECTORY_TYPE_GENBAND != nType
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
            && CALL_LOG_TYPE_MTSW != nType
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
            && DIRECTORY_TYPE_METASWICTH != nType
#endif
       )
    {
        return NULL;
    }

    return (CCommonUIHelper *)pUIHelper.Get();
}

void CCommonUIHelper::TransBsftFACText(yl::string & strValue, QString & strText)
{
    if (strValue.find(kszReplaceCallParkCode) != yl::string::npos
            || strValue.find(kszReplaceCallRetriveCode) != yl::string::npos
            || strValue.find(kszReplaceGroupParkCode) != yl::string::npos
            || strValue.find(kszReplaceGPickUpCode) != yl::string::npos
            || strValue.find(kszReplaceDPickUpCode) != yl::string::npos
            || strValue.find(kszReplaceVoiceMailCode) != yl::string::npos
            || strValue.find(kszReplaceBargeInCode) != yl::string::npos
            || strValue.find(kszReplaceCallPullCode) != yl::string::npos
#if IF_BUG_23548
            || strValue.find(kszReplaceCallBackCode) != yl::string::npos
#endif
       )
    {
        yl::string::size_type nPos = strValue.find(':');

        if (yl::string::npos != nPos)
        {
            yl::string strFACValue = strValue.substr(0, nPos);
            yl::string strTemp = strValue.substr(nPos);

            strText = LANG_TRANSLATE(strFACValue.c_str()) + toQString(strTemp, true);
        }
        else
        {
            strText = LANG_TRANSLATE(strValue.c_str());
        }
    }
    else
    {
        strText = toQString(strValue, true);
    }
}

bool CCommonUIHelper::FillData(ContactListData & listData)
{
    DIRUI_INFO("CCommonUIHelper::FillData(TotalCount:%d GetCount:%d Index:%d)",
               listData.m_nTotalCount, listData.m_nCountToGet, listData.m_nIndexFrom);

    if (NULL == m_pFrameList)
    {
        DIRUI_ERR("FillData when m_pFrameList is null");
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonControler = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonControler)
    {
        DIRUI_ERR("CCommonUIHelper::FillData fail when pCommonControler is null");
        return false;
    }

    int nUIState = pCommonControler->GetState();

    RefreshListItemAmount(nUIState, listData.m_nIndexFrom, listData.m_nTotalCount);

    int nSize = listData.m_listId.size();

    if (0 == nSize)
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

        if (!pDelegate.IsEmpty())
        {
            pDelegate->SetPageDataIndex(0);
        }

        m_pFrameList->ClearContent(true);
        return true;
    }

    int nItemIndex = 0;
    int i = 0;

    for (; i < nSize; ++i)
    {
        int & nId = listData.m_listId[i];

        SmartPtr pDetail = pCommonControler->GetDetail(nId);

        if (pDetail.IsEmpty())
        {
            DIRUI_ERR("GetDetail(DirUIType:%d Id:%d) return empty detail!", pCommonControler->GetType(), nId);
            continue;
        }

        if (FillListItem(nId, nItemIndex, (listData.m_nIndexFrom + i), listData.m_nTotalCount, pDetail))
        {
            ++nItemIndex;
        }
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();

    return true;
}

int CCommonUIHelper::GetAllContactAmount()
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);

    if (NULL == pCommonDirCtrl)
    {
        return 0;
    }

    return pCommonDirCtrl->GetContactAmount(pCommonDirCtrl->GetGroupId());
}

void CCommonUIHelper::OnAttachFrameList()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty() || NULL == m_pFrameList)
    {
        return;
    }

    int nState = pDelegate->GetState();

    if (DIR_UI_STATE_SEARCH == nState)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
        return;
    }

    CDirUIHelper::OnAttachFrameList();
}

bool CCommonUIHelper::RefreshGroupMenuItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        const QString & strName)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    if (0 == nItemIndex)
    {
        if (!m_pFrameList->IsCanReuseItem(CIconMenuListItem::GetIconMenuListItemType()))
        {
            m_pFrameList->ClearContent();
        }

        m_pFrameList->SetTotalDataCount(nDataCount);
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CDirectoryListDelegate * pListDelegate = pDelegate.Get();

    if (NULL == pListDelegate || pListDelegate != m_pFrameList->GetFrameDelegate())
    {
        return false;
    }

    bool bAdd = false;
    CIconMenuListItemPtr pItem = (CIconMenuListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

    if (NULL == pItem)
    {
        pItem = new CIconMenuListItem;

        if (NULL == pItem)
        {
            return false;
        }

        bAdd = true;
    }

    pItem->ClearContent();

    pItem->SetAction(pListDelegate);
    pItem->SetId(nId);
    pItem->SetDataIndex(nDataIndex);
    pItem->SetIconPath(PIC_CONTACTS_GROUP_ICON);
    pItem->SetMenu(strName);

    if (bAdd && !m_pFrameList->AddItem(pItem))
    {
        pItem->Release();
        return false;
    }

    return true;
}

bool CCommonUIHelper::RefreshCallLogItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
        int nCallType, bool bHaveName, const QString & strName, const yl::string & strNumber,
        const yl::string & strBornTick, const yl::string & strHead)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);
    CDirectoryListDelegate * pListDelegate = pDelegate.Get();

    if (NULL == pListDelegate || pListDelegate != m_pFrameList->GetFrameDelegate())
    {
        return false;
    }

    if (0 == nItemIndex)
    {
        if (!m_pFrameList->IsCanReuseItem(CCallLogListItem::GetCallLogListItemType()))
        {
            m_pFrameList->ClearContent();
        }

        m_pFrameList->SetTotalDataCount(nDataCount);
        pListDelegate->SetPageDataIndex(nDataIndex);
    }

    bool bAdd = false;
    CCallLogListItemPtr pItem = (CCallLogListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

    if (NULL == pItem)
    {
        pItem = new CCallLogListItem;

        if (NULL == pItem)
        {
            return false;
        }
        pItem->setObjectName(QString("Item%1").arg(nItemIndex));
        bAdd = true;
    }

    pItem->ClearContent();

    pItem->SetAction(pListDelegate);
    pItem->SetDataIndex(nDataIndex);

    int nDisplayType = configParser_GetConfigInt(kszCalllogShowNumEnable);
    if (2 == nDisplayType)
    {
        pItem->SetTitle(strName);
        pItem->SetSubTitle(strNumber);
        pItem->setShowSubTitle(bHaveName);
        pItem->setShowInfo(true);
        pItem->SetInfo(toQString(strBornTick));
    }
    else if (1 == nDisplayType)
    {
        pItem->SetTitle(strNumber);
        pItem->SetSubTitle(toQString(strBornTick));
        pItem->setShowSubTitle(true);
        pItem->setShowInfo(false);
    }
    else
    {
        pItem->SetTitle(strName);
        pItem->SetSubTitle(toQString(strBornTick));
        pItem->setShowSubTitle(true);
        pItem->setShowInfo(false);
    }

    pItem->SetIcon(strHead);
    pItem->SetId(nId);

    QColor colorTitle;
    bool bResult = GetTitleColor(colorTitle, nCallType);
    if (bResult)
    {
        pItem->SetTitleColor(colorTitle);
    }

    pItem->SetCallType(nCallType);

    if (NULL != pSettingDelegate)
    {
        pItem->SetEdit(true);
        pItem->SetChecked(pSettingDelegate->IsSelect(nId));
    }
    else
    {
        pItem->SetEdit(false);

        CDirectoryControlerPtr pControler = pListDelegate->GetControler();
        bool bReadOnly = true;

        if (!pControler.IsEmpty())
        {
            bReadOnly = pControler->IsReadOnly();
        }

        pItem->SetDetail(!bReadOnly);
    }

    if (bAdd && !m_pFrameList->AddItem(pItem))
    {
        pItem->Release();

        return false;
    }

    return true;
}

bool CCommonUIHelper::IsShowUpateTitile()
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactUpdateListTitle::IsContactUpdateListTitle(pTitle))
    {
        return true;
    }

    return false;
}

void CCommonUIHelper::InitUpdateTitle(bool bShowUpdateBtn/* = true*/)
{
    DIRUI_INFO("CCommonUIHelper::InitUpdateTitle()");

    if (NULL == m_pFrameList)
    {
        return;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    bool bAdd = true;
    CContactUpdateListTitlePtr pUpdateTitle = NULL;
    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactUpdateListTitle::IsContactUpdateListTitle(pTitle))
    {
        bAdd = false;
        pUpdateTitle = (CContactUpdateListTitlePtr)pTitle;
    }
    else
    {
        pUpdateTitle = new CContactUpdateListTitle;
        if (NULL == pUpdateTitle)
        {
            return;
        }
    }

    pUpdateTitle->SetAction(pDelegate.Get());
    pUpdateTitle->SetUpdatable(CDirectoryControler::IsDirCanEidt(DIR_MOD_TYPE_NONE) && bShowUpdateBtn);

    if (bAdd)
    {
        m_pFrameList->SetFrameListTitle(pUpdateTitle);
    }
    else
    {
        pUpdateTitle->RefreshTitle();
    }
}

void CCommonUIHelper::SetUpdateTitleGroupName(const QString & strGroupName,
        bool bRefresh/* = false*/)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CContactUpdateListTitlePtr pTitle = (CContactUpdateListTitlePtr)m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactUpdateListTitle::IsContactUpdateListTitle(pTitle))
    {
        pTitle->SetGroupName(strGroupName);

        if (bRefresh)
        {
            pTitle->RefreshTitle();
        }
    }
}

#if IF_FEATURE_GBBOOK

void CCommonUIHelper::SetShowUpdate(bool bShow, bool bRefresh/* = false*/)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CContactUpdateListTitlePtr pTitle = (CContactUpdateListTitlePtr)m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactUpdateListTitle::IsContactUpdateListTitle(pTitle))
    {
        pTitle->SetUpdatable(bShow);

        if (bRefresh)
        {
            pTitle->RefreshTitle();
        }
    }
}

#endif

void CCommonUIHelper::RefreshUpdateTitle(int nContactAmount, bool bRefresh/* = false*/)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CContactUpdateListTitlePtr pTitle = (CContactUpdateListTitlePtr)m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactUpdateListTitle::IsContactUpdateListTitle(pTitle))
    {
        pTitle->SetContactCount(nContactAmount);

        if (bRefresh)
        {
            pTitle->RefreshTitle();
        }
    }
}

int CCommonUIHelper::GetLisItemId(CListItem * pItem)
{
    if (NULL == pItem)
    {
        return knInvalidRId;
    }

    int nId = CDirUIHelper::GetLisItemId(pItem);

    if (knInvalidRId != nId)
    {
        return nId;
    }

    int nMenuItemType = CIconMenuListItem::GetIconMenuListItemType();
    int nItemType = pItem->GetType();

    if (nMenuItemType == (nMenuItemType & nItemType))
    {
        CIconMenuListItem * pMenuItem = (CIconMenuListItem *)pItem;

        return pMenuItem->GetId();
    }

    int nCallLogItemType = CCallLogListItem::GetCallLogListItemType();

    if (nCallLogItemType == (nCallLogItemType & nItemType))
    {
        CCallLogListItem * pCallLogItem = (CCallLogListItem *)pItem;

        return pCallLogItem->GetId();
    }

    return knInvalidRId;
}

void CCommonUIHelper::ExitBSFTPwdUI()
{
    int nDirType = _BsftPwdMgr.GetDirType();

    if (DIRECTORY_TYPE_NONE == nDirType)
    {
        return;
    }

    MessageBox_RemoveMessageBox(&g_DirectoryMgr, MessageBox_Cancel, nDirType);
}

#if IF_FEATURE_CALLLIST_SHOW_METHOD
void CCommonUIHelper::GetTextOfRecordToShow(yl::string & strText, const yl::string & strName,
        const yl::string & strNumber,
        const yl::string & strSipServer)
{
    if (strName.empty() && strNumber.empty())
    {
        return;
    }

    strText = "";
    int nDisplayMethod = configParser_GetConfigInt(kszCalllogShowNumEnable);
    yl::string strSeparator = "/";

    switch (nDisplayMethod)
    {
    case CL_SHOW_ONLY_NUMBER:
        {
            strText = strNumber.empty() ? strName : strNumber;
        }
        break;
    case CL_SHOW_NAME_AND_NUMBER:
        {
            if (strName.empty())
            {
                strText = strNumber;
            }
            else
            {
                strText = strNumber.empty() ? strName : (strName + strSeparator + strNumber);
            }
        }
        break;
    case CL_SHOW_NUMBER_AND_NAME:
        {
            if (strNumber.empty())
            {
                strText = strName;
            }
            else
            {
                strText = strName.empty() ? strNumber : (strNumber + strSeparator + strName);
            }
        }
        break;
    case CL_SHOW_FULL_CONTACT_INFO:
        {
            //local record full contact: "name" <sip:number@server>;
            //bsft record full contact: "name" <sip:number>;
            if (!strName.empty())
            {
                strText = "\"" + strName + "\" ";
            }
            strText += "<sip:" + strNumber;
            if (!strSipServer.empty())
            {
                strText += "@" + strSipServer;
            }
            strText += ">";
        }
        break;
    default:
        {
            strText = strName.empty() ? strNumber : strName;
        }
        break;
    }
}
#endif

bool CCommonUIHelper::GetTitleColor(QColor & color, int nCallType)
{
#if DIR_SUPPORT_MISSCALL_ALWAYS_RED
    if (CT_MISSCALL == nCallType)
    {
        color = Qt::red;
        return true;
    }
#endif

    return false;
}

void CCommonUIHelper::setCurrentPageForItem(CListItemPtr pItem)
{
    if (NULL != pItem
            && NULL != m_pFocusInvalidFrameList)
    {
        if (INVALID_ITEM_INDEX == m_pFocusInvalidFrameList->GetIndexByItem(pItem))
        {
            int nPageCount = m_pFocusInvalidFrameList->GetMaxPageItemCount();
            m_pFocusInvalidFrameList->SwitchPage(pItem->GetDataIndex() / nPageCount);
        }
    }
}

