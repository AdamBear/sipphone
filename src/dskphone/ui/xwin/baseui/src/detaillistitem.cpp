#include "detaillistitem.h"
#include "baseuicommon.h"


CDetailListItem::CDetailListItem(IListViewItemAction * pAction/* = NULL*/)
    : CDetailListItemBase(pAction)
    , m_pValue(NULL)
{
    m_nType |= LIST_ITEM_SUB_TYPE_DETAIL_ITEM;
}

CDetailListItem::~CDetailListItem()
{
}

int CDetailListItem::GetDetailListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_ITEM);
}

CDetailListItem * CDetailListItem::GetDetailListItem(xListViewItem * pItem)
{
    if (NULL == pItem || GetDetailListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CDetailListItem *)pItem;
}

xListViewItem * CDetailListItem::CreateDetailListItem()
{
    CDetailListItem * pDetailItem = new CDetailListItem;

    if (NULL != pDetailItem)
    {
        pDetailItem->loadContent(BASE_UI_LAYOUT_FILE_DETAIL_LIST_ITEM);
    }

    return pDetailItem;
}

void CDetailListItem::loadChildrenElements(xml_node & node)
{
    CDetailListItemBase::loadChildrenElements(node);

    m_pValue = (xTextView *)getViewById(BASE_UI_ID_TEXT_VALUE);
}

bool CDetailListItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pValue)
    {
        if (bFocusIn)
        {
            m_pValue->EnableScroll(true);
            m_pValue->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pValue->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return CDetailListItemBase::onFocusEvent(bFocusIn);
}

void CDetailListItem::SetValue(const yl::string & strValue)
{
    if (NULL != m_pValue)
    {
        m_pValue->SetText(strValue);
    }
}

yl::string CDetailListItem::GetValue() const
{
    if (NULL != m_pValue)
    {
        return m_pValue->GetText();
    }

    return yl::string("");
}

void CDetailListItem::SetItemData(const yl::string & strData)
{
    m_strData = strData;
}

void CDetailListItem::SetItemData(int nData)
{
    char szBuffer[32];

    _snprintf(szBuffer, sizeof(szBuffer), "%d", nData);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_strData = szBuffer;
}

const yl::string & CDetailListItem::GetItemStringData() const
{
    return m_strData;
}

int CDetailListItem::GetData() const
{
    return atoi(m_strData.c_str());
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CDetailListItem::UpdateTextView(bool bFocus)
{
    CDetailListItemBase::UpdateTextView(bFocus);

    UpdateTextViewColor(m_pValue, bFocus);
}
#endif
