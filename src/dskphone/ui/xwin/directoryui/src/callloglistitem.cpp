#include "callloglistitem.h"
#include "modcalllogui.h"
#include "directoryuicommon.h"

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
#include "interfacedefine.h"

namespace
{
#define DEFAULT_NAME_COLOR_RED_T54 xColor(0xff, 0x0a, 0x0a)
#define DEFAULT_NAME_COLOR_RED_T46 xColor(0xe2, 0x16, 0x13)
#define DEFAULT_NAME_COLOR_RED_T29 xColor(0xff, 0x27, 0x27)
#define DEFAULT_NAME_COLOR_RED_T29_FOCUS xColor(0xf6, 0x00, 0x0)
}
#endif

CCallLogListItem::CCallLogListItem(IListViewItemAction * pAction/* = NULL*/)
    : CContactListItem(pAction)
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    , m_bMissCall(false)
#endif
{
    m_nType |= LIST_ITEM_SUB_TYPE_CALL_LOG;
}

CCallLogListItem::~CCallLogListItem()
{
}

int CCallLogListItem::GetCallLogListItemType()
{
    return (LIST_ITEM_TYPE_CONTACT_BASE | LIST_ITEM_SUB_TYPE_CALL_LOG);
}

CCallLogListItem * CCallLogListItem::GetCallLogListItem(xListViewItem * pItem)
{
    if (NULL == pItem || pItem->GetType() != GetCallLogListItemType())
    {
        return NULL;
    }

    return static_cast<CCallLogListItem *>(pItem);
}

xListViewItem * CCallLogListItem::CreateCallLogListItem()
{
    CCallLogListItem * pListItem = new CCallLogListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(DIR_UI_LAYOUT_FILE_CALLLOG_ITEM);
    }

    return pListItem;
}

void CCallLogListItem::SetCallType(int nType)
{
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    m_bMissCall = (CT_MISSCALL == nType);
#endif

    yl::string strIcon = GetCallTypeIcon(nType);

    CContactListItem::SetIcon(strIcon);
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CCallLogListItem::UpdateTextView(bool bFocus)
{
    UpdateTextViewColor(m_pInfo, bFocus);

    if (m_bMissCall)
    {
        UpdateMissCallNameColor(bFocus);
    }
    else
    {
        UpdateTextViewColor(m_pName, bFocus);
    }
}

void CCallLogListItem::UpdateMissCallNameColor(bool bFocus)
{
    if (!m_bMissCall || NULL == m_pName)
    {
        return;
    }

    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (DEVICELIB_IS_T46(ePhoneType))
    {
        m_pName->SetTextColor(DEFAULT_NAME_COLOR_RED_T46);
    }
    else if (PT_T52 == ePhoneType || PT_T54S == ePhoneType)
    {
        m_pName->SetTextColor(DEFAULT_NAME_COLOR_RED_T54);
    }
    else if (PT_T29 == ePhoneType)
    {
        xColor colName = bFocus ? DEFAULT_NAME_COLOR_RED_T29_FOCUS : DEFAULT_NAME_COLOR_RED_T29;
        m_pName->SetTextColor(colName);
    }
}
#endif
