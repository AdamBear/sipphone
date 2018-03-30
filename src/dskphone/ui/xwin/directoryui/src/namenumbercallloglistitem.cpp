#if IS_COLOR
#include "namenumbercallloglistitem.h"
#include "devicelib/phonedevice.h"

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
namespace
{
#define DEFAULT_NUMBER_COLOR_T46 xColor(0x33, 0x33, 0x33)
#define DEFAULT_NUMBER_COLOR_T54 xColor(0x6a, 0x6a, 0x6a)
#define DEFAULT_NUMBER_COLOR_T52 xColor(0x88, 0x88, 0x88)
}
#endif

CNameNumberCallLogListItem::CNameNumberCallLogListItem(IListViewItemAction * pAction/* = NULL*/)
    : CCallLogListItem(pAction)
    , m_pTextNumber(NULL)
{
    m_nType |= LIST_ITEM_SUB_TYPE_CALL_LOG_NAME_NUMBER;
}

CNameNumberCallLogListItem::~CNameNumberCallLogListItem()
{
}

int CNameNumberCallLogListItem::GetNameNumberCallLogListItemType()
{
    return (LIST_ITEM_TYPE_CONTACT_BASE | LIST_ITEM_SUB_TYPE_CALL_LOG | LIST_ITEM_SUB_TYPE_CALL_LOG_NAME_NUMBER);
}

CNameNumberCallLogListItem * CNameNumberCallLogListItem::GetNameNumberCallLogListItem(xListViewItem * pItem)
{
    if (NULL == pItem || pItem->GetType() != GetNameNumberCallLogListItemType())
    {
        return NULL;
    }

    return static_cast<CNameNumberCallLogListItem *>(pItem);
}

xListViewItem * CNameNumberCallLogListItem::CreateNameNumberCallLogListItem()
{
    CNameNumberCallLogListItem * pListItem = new CNameNumberCallLogListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("directory/namenumbercalllogitem.xml");
    }

    return pListItem;
}

bool CNameNumberCallLogListItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pTextNumber)
    {
        if (bFocusIn)
        {
            m_pTextNumber->EnableScroll(true);
            m_pTextNumber->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pTextNumber->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return CCallLogListItem::onFocusEvent(bFocusIn);
}

void CNameNumberCallLogListItem::onLoadChildElement(xml_node & nodeChild)
{
    CCallLogListItem::onLoadChildElement(nodeChild);

    m_pTextNumber = static_cast<xTextView*>(getViewById("txtNumber"));
}

void CNameNumberCallLogListItem::SetNumber(const yl::string& strNumber)
{
    if (NULL == m_pTextNumber)
    {
        return;
    }

    m_pTextNumber->SetText(strNumber);
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CNameNumberCallLogListItem::UpdateTextView(bool bFocus)
{
    CCallLogListItem::UpdateTextView(bFocus);

    UpdateNumberTextColor(bFocus);
}

void CNameNumberCallLogListItem::UpdateNumberTextColor(bool bFocus)
{
    if (NULL == m_pTextNumber)
    {
        return;
    }

    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (DEVICELIB_IS_T46(ePhoneType))
    {
        m_pTextNumber->SetTextColor(DEFAULT_NUMBER_COLOR_T46);
    }
    else if (PT_T52 == ePhoneType)
    {
        m_pTextNumber->SetTextColor(DEFAULT_NUMBER_COLOR_T52);
    }
    else if (PT_T54S == ePhoneType)
    {
        m_pTextNumber->SetTextColor(DEFAULT_NUMBER_COLOR_T54);
    }
    else if (PT_T29 == ePhoneType)
    {
        UpdateTextViewColor(m_pTextNumber, bFocus);
    }
}
#endif

#endif
