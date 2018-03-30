#include "detaillistitembase.h"
#include "xwindow/xviewrenderer.h"
#include "baseuicommon.h"
#include "modbaseui.h"


CDetailListItemBase::CDetailListItemBase(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_DETAIL, pAction)
    , m_pTitle(NULL)
    , m_bShowIndex(false)
    , m_bShowColon(true)
{
}

CDetailListItemBase::~CDetailListItemBase()
{
}

int CDetailListItemBase::GetDetailListItemBaseType()
{
    return LIST_ITEM_TYPE_DETAIL;
}

void CDetailListItemBase::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pTitle = static_cast<xTextView *>(getViewById(BASE_UI_ID_TEXT_TITLE));
}

void CDetailListItemBase::Release()
{
    if (NULL != m_pTitle)
    {
        m_pTitle->SetText("");
    }

    xListViewItem::Release();
}

bool CDetailListItemBase::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pTitle)
    {
        if (bFocusIn)
        {
            m_pTitle->EnableScroll(true);
            m_pTitle->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pTitle->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    UpdateTextView(bFocusIn);
#endif

    return xListViewItem::onFocusEvent(bFocusIn);
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CDetailListItemBase::SetItemFocus(bool bFocus)
{
    xListViewItem::SetItemFocus(bFocus);

    UpdateTextView(bFocus);
}
#endif

int CDetailListItemBase::GetDetailSubType() const
{
    return (m_nType & LIST_ITEM_TYPE_DETAIL_MARK);
}

void CDetailListItemBase::SetTitle(const yl::string & strTitle)
{
    if (NULL == m_pTitle)
    {
        return;
    }

    if (strTitle.empty())
    {
        m_pTitle->SetText("");

        return;
    }

    yl::string strShowTitle = BaseUI_GetRealShowText(strTitle, m_bShowIndex, GetDataIndex() + 1);


    if (m_bShowColon)
    {
        strShowTitle = strShowTitle + ":";
    }

    m_pTitle->SetText(strShowTitle);
}

void CDetailListItemBase::SetShowIndex(bool bShowIndex)
{
    m_bShowIndex = bShowIndex;
}

void CDetailListItemBase::SetShowColon(bool bShowColon)
{
    m_bShowColon = bShowColon;
}

void CDetailListItemBase::SetValue(const yl::string & strValue)
{
}

yl::string CDetailListItemBase::GetValue() const
{
    return yl::string("");
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CDetailListItemBase::UpdateTextView(bool bFocus)
{
    UpdateTextViewColor(m_pTitle, bFocus);
}

void CDetailListItemBase::UpdateTextViewColor(xTextView* pTextView, bool bFocus)
{
    if (NULL == pTextView || !pTextView->isVisible())
    {
        return;
    }

    if (bFocus)
    {
        pTextView->SetTextColor(m_colorFocus);
    }
    else
    {
        pTextView->SetTextColor(m_colorNormal);
    }
}
#endif
