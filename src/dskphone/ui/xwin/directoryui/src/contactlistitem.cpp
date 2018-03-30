#include "contactlistitem.h"
#include "xwindow/xWindow.h"
#include "directoryuicommon.h"
#include "baseui/include/t2xpicpath.h"
#include "devicelib/phonedevice.h"
#include "baseui/include/modbaseui.h"

namespace
{
#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
#define HIGHT_LIGHT_TEXT_COLOR_T46 xColor(0, 124, 230)
#define HIGHT_LIGHT_TEXT_COLOR_T29 xColor(236, 0, 0)
#define HIGHT_LIGHT_TEXT_COLOR_T5254 xColor(252, 64, 60)
#endif
}

CContactListItem::CContactListItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_CONTACT_BASE, pAction)
    , m_nId(0)
    , m_pIcon(NULL)
    , m_pName(NULL)
    , m_pInfo(NULL)
{
}

CContactListItem::~CContactListItem()
{
}

int CContactListItem::GetContactListItemType()
{
    return LIST_ITEM_TYPE_CONTACT_BASE;
}

CContactListItem * CContactListItem::GetContactListItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_CONTACT_BASE != pItem->GetType())
    {
        return NULL;
    }

    return static_cast<CContactListItem *>(pItem);
}

xListViewItem * CContactListItem::CreateContactListItem()
{
    CContactListItem * pListItem = new CContactListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(DIR_UI_LAYOUT_FILE_CONTACT_ITEM);
    }

    return pListItem;
}

void CContactListItem::onLoadChildElement(xml_node & nodeChild)
{
    xListViewItem::onLoadChildElement(nodeChild);

    m_pName = static_cast<xTextView *>(getViewById(DIR_UI_ID_TXT_NAME));
    m_pIcon = static_cast<xImageView *>(getViewById(DIR_UI_ID_PIC_ICON));
    m_pInfo = static_cast<xTextView *>(getViewById(DIR_UI_ID_TXT_INFO));
}

void CContactListItem::ClearContent()
{
    m_nId = 0;

#ifdef PHOTO_SHOW
    SetIcon("");
#endif
    SetName("");
    SetInfo("");

    xListViewItem::ClearContent();
}

bool CContactListItem::onFocusEvent(bool bFocusIn)
{
    if (bFocusIn)
    {
        if (NULL != m_pName)
        {
            m_pName->EnableScroll(true);
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }

        if (NULL != m_pInfo)
        {
            m_pInfo->EnableScroll(true);
            m_pInfo->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
    }
    else
    {
        if (NULL != m_pName)
        {
            m_pName->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }

        if (NULL != m_pInfo)
        {
            m_pInfo->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    UpdateTextView(bFocusIn);
#endif

    return xListViewItem::onFocusEvent(bFocusIn);
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CContactListItem::SetItemFocus(bool bFocus)
{
    xListViewItem::SetItemFocus(bFocus);

    UpdateTextView(bFocus);
}
#endif

void CContactListItem::SetId(int nId)
{
    m_nId = nId;
}

void CContactListItem::SetIcon(const yl::string & strIcon)
{
    if (NULL != m_pIcon)
    {
        m_pIcon->setPixmap(strIcon);
    }
}

void CContactListItem::SetName(const yl::string & strName)
{
    if (NULL != m_pName)
    {
        yl::string strRealName = BaseUI_GetRealShowText(strName, false, -1);
        m_pName->SetText(strRealName);
    }
}

void CContactListItem::SetInfo(const yl::string & strInfo)
{
    if (NULL != m_pInfo)
    {
        m_pInfo->SetText(strInfo);
    }
}

yl::string CContactListItem::GetInfo() const
{
    if (NULL != m_pInfo)
    {
        return m_pInfo->GetText();
    }

    return yl::string("");
}

int CContactListItem::GetId() const
{
    return m_nId;
}

void CContactListItem::SetPhoto(const yl::string & strPhoto)
{
    if (NULL == m_pIcon)
    {
        return;
    }

    if (strPhoto.empty())
    {
        m_pIcon->hide(true);
    }
    else
    {
        m_pIcon->setPixmap(strPhoto);
        m_pIcon->show();
    }
}

#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
void CContactListItem::SetHightLightText(const yl::string& strText)
{
    xColor colHightLight = GetHightLightTextColor();
    if (NULL != m_pName)
    {
        m_pName->setHighlightKeyword(strText);
        m_pName->SetTextHightLightColor(colHightLight);
    }

    if (NULL != m_pInfo)
    {
        m_pInfo->setHighlightKeyword(strText);
        m_pName->SetTextHightLightColor(colHightLight);
    }
}

xColor CContactListItem::GetHightLightTextColor()
{
    xColor colHightLight;
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (DEVICELIB_IS_T46(ePhoneType))
    {
        colHightLight = HIGHT_LIGHT_TEXT_COLOR_T46;
    }
    else if (PT_T29 == ePhoneType)
    {
        colHightLight = HIGHT_LIGHT_TEXT_COLOR_T29;
    }
    else if (PT_T52 == ePhoneType || PT_T54S == ePhoneType)
    {
        colHightLight = HIGHT_LIGHT_TEXT_COLOR_T5254;
    }

    return colHightLight;
}
#endif

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CContactListItem::UpdateTextView(bool bFocus)
{
    UpdateTextViewColor(m_pName, bFocus);
    UpdateTextViewColor(m_pInfo, bFocus);
}

void CContactListItem::UpdateTextViewColor(xTextView* pTextView, bool bFocus)
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
