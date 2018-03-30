#include "searchwidgetitem.h"
#include "talkuicommonfunction.h"
#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"

namespace talkui
{

///////////////////////// CSearchItemData /////////////////////////////
CSearchItemData::CSearchItemData()
{
    Reset();
}

void CSearchItemData::Reset()
{
    m_strDisplayName = "";
    m_strNumber = "";
    m_strContactImage = "";
    m_eContactStatus = _PRESENCE_TYPE_NONE;
    m_bHold = false;
    m_iCallID = -1;
}

void CSearchItemData::GetSearchItemDataFromDirectorySearchData(DirectorySearchData &
        objDirectorySearchData)
{
    // http://10.2.1.199/Bug.php?BugID=79353
    //  m_strDisplayName = objDirectorySearchData.m_strName;
    //  m_strNumber = objDirectorySearchData.m_strNumber;

    //  m_strDisplayName = objDirectorySearchData.GetDisplayName();
    m_strDisplayName = objDirectorySearchData.m_strName;
    m_strNumber = objDirectorySearchData.GetNumber();

    m_strContactImage = objDirectorySearchData.m_strPic;

    m_eContactStatus = objDirectorySearchData.m_eStatus;

    m_bHold = false;
    m_iCallID = -1;
}


bool IsSearchItemDataEqual(const CSearchItemData & objSearchItemData1,
                           const CSearchItemData & objSearchItemData2)
{
    if (objSearchItemData1.m_strDisplayName != objSearchItemData2.m_strDisplayName)
    {
        return false;
    }

    if (objSearchItemData1.m_strNumber != objSearchItemData2.m_strNumber)
    {
        return false;
    }

    if (objSearchItemData1.m_strContactImage != objSearchItemData2.m_strContactImage)
    {
        return false;
    }

    if (objSearchItemData1.m_eContactStatus != objSearchItemData2.m_eContactStatus)
    {
        return false;
    }

    if (objSearchItemData1.m_bHold != objSearchItemData2.m_bHold)
    {
        return false;
    }

    if (objSearchItemData1.m_iCallID != objSearchItemData2.m_iCallID)
    {
        return false;
    }

    return true;
}



///////////////////////// CSearchWidgetItem /////////////////////////////
CSearchWidgetItem::CSearchWidgetItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_TALK_SEARCH, pAction)
    , m_pName(NULL)
    , m_pNumber(NULL)
{

}

CSearchWidgetItem::~CSearchWidgetItem()
{

}

int CSearchWidgetItem::GetSearchItemType()
{
    return LIST_ITEM_TYPE_TALK_SEARCH;
}

CSearchWidgetItem * CSearchWidgetItem::GetSearchItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_TALK_SEARCH != pItem->GetType())
    {
        return NULL;
    }

    return (CSearchWidgetItem *)pItem;
}

xListViewItem * CSearchWidgetItem::CreateSearchItem()
{
    CSearchWidgetItem * pListItem = new CSearchWidgetItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(TALKUI_SEARCH_ITEM_XML_PATH);
    }

    return pListItem;
}

void CSearchWidgetItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pName = static_cast<xTextView *>(getViewById(TALKUI_DIAL_SEARCH_ITEM_NAME));
    m_pNumber = static_cast<xTextView *>(getViewById(TALKUI_DIAL_SEARCH_ITEM_NUMBER));
}

//获取焦点item的数据, 有焦点数据返回true， 没有焦点数据返回false
void CSearchWidgetItem::GetContactData(yl::string & strDisplayName, yl::string & strNumber,
                                       int & iCallID)
{
    strDisplayName = m_objSearchItemData.m_strDisplayName;

    strNumber = m_objSearchItemData.m_strNumber;

    iCallID = m_objSearchItemData.m_iCallID;
}

void CSearchWidgetItem::GetSearchTableItemData(CSearchItemData & objSearchItemData)
{
    objSearchItemData = m_objSearchItemData;
}

//设置数据
void CSearchWidgetItem::SetSearchTableItemData(const CSearchItemData & objSearchItemData)
{
    //先重置一下数据
    ResetAllInfo();

    m_objSearchItemData = objSearchItemData;

    //设置联系人名称和号码
    SetNameAndNumber(m_objSearchItemData.m_strDisplayName, m_objSearchItemData.m_strNumber);
}

//设置名称和号码  这个要一块设置因为名称和号码相同是要只显示名称的
void CSearchWidgetItem::SetNameAndNumber(const yl::string & strName, const yl::string & strNumber)
{
    yl::string strSetName = strName;
    yl::string strSetNumber = strNumber;

    if (m_objSearchItemData.m_bHold)
    {
        strSetName = _UILOGIC_LANG_TRAN(TRID_HOLD) + " " + strSetName;
    }

#if IF_TALKUI_CONTACT_PHOTO
    if (strSetName.empty())
    {
        strSetName = strSetNumber;
    }

    //设置名称
    if (NULL != m_pName)
    {
        m_pName->show(xView::SHOW_CMD_BRING_TO_TOP);
        m_pName->SetText(strSetName);
    }

    //设置号码
    if (NULL != m_pNumber)
    {
        if (strSetNumber.empty())
        {
            m_pNumber->hide(true);
        }
        else if (strSetName == strSetNumber)
        {
            m_pNumber->hide(true);
        }
        else
        {
            m_pNumber->show(xView::SHOW_CMD_BRING_TO_TOP);
            m_pNumber->SetText(strSetNumber);
        }
    }
#else
    if (strSetName.empty())
    {
        strSetName = strSetNumber;
    }
    else if (!strSetNumber.empty() && strSetName != strSetNumber)
    {
        strSetName += "(";
        strSetName += strSetNumber;
        strSetName += ")";
    }

    //设置名称
    if (NULL != m_pName)
    {
        m_pName->show(xView::SHOW_CMD_BRING_TO_TOP);
        m_pName->SetText(strSetName);
    }
#endif
}

//重置所有信息
void CSearchWidgetItem::ResetAllInfo()
{
    m_objSearchItemData.Reset();

    SetNameAndNumber("", "");

    //隐藏三个控件
    SetNameShow(false);
    SetNumberShow(false);
}

void CSearchWidgetItem::SetNameShow(bool bShow, bool bGone)
{
    if (NULL != m_pName)
    {
        if (bShow)
        {
            m_pName->show(xView::SHOW_CMD_BRING_TO_TOP);
        }
        else
        {
            m_pName->hide(bGone);
        }
    }
}

void CSearchWidgetItem::SetNumberShow(bool bShow, bool bGone)
{
    if (NULL != m_pNumber)
    {
        if (bShow)
        {
            m_pNumber->show(xView::SHOW_CMD_BRING_TO_TOP);
        }
        else
        {
            m_pNumber->hide(bGone);
        }
    }
}
}

