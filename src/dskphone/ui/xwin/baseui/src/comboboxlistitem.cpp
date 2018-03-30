#include "comboboxlistitem.h"
#include "keymap.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "xwindow/xcombobox.h"
#include "xwindow/xSoftkeyBar.h"
#include "xwindow/xValueHandler.h"
#include "common/common_data_define.h"
#include "baseuicommon.h"
#include "t2xpicpath.h"
#include "t2xwidget.h"
#include <baseuidefine.h>
#include "modbaseui.h"

namespace
{
#define COMBOBOX_LIST_ITEM_ST_LEFT_INDEX 1
#define COMBOBOX_LIST_ITEM_ST_RIGHT_INDEX 2
#define COMBOBOX_LIST_ITEM_ST_SWITCH_INDEX 2
}

CComboBoxListItem::CComboBoxListItem(IListViewItemAction * pAction/* = NULL*/)
    : CDetailListItemBase(pAction)
    , m_pComboBox(NULL)
#if IF_COMBOBOX_ITEM_DEFAULT_SWITCH
    , m_bSwitchSoftKey(true)
#else
    , m_bSwitchSoftKey(false)
#endif
    , m_bSetupSoftkeyByItem(true)
{
    m_nType |= LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX;
}

CComboBoxListItem::~CComboBoxListItem()
{
}

int CComboBoxListItem::GetComboBoxListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX);
}

CComboBoxListItem * CComboBoxListItem::GetComboBoxListItem(xListViewItem * pItem)
{
    if (NULL == pItem || GetComboBoxListItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CComboBoxListItem *)pItem;
}

xListViewItem * CComboBoxListItem::CreateComboBoxListItem()
{
    CComboBoxListItem * pListItem = new CComboBoxListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(BASE_UI_LAYOUT_FILE_COMBOBOX_LIST_ITEM);
    }

    return pListItem;
}

void CComboBoxListItem::loadChildrenElements(xml_node & node)
{
    CDetailListItemBase::loadChildrenElements(node);

    m_pComboBox = static_cast<xComboBox *>(getViewById(BASE_UI_ID_COMBO_VALUE));

    if (NULL != m_pComboBox)
    {
        m_pComboBox->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
    }
}

void CComboBoxListItem::SetupValueChangedSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    xComboBoxHandler * pCmbHandler = m_pComboBox->GetComboBoxHandle();

    if (NULL == pCmbHandler)
    {
        return;
    }

    pCmbHandler->signalValueChanged.connect(pHandler, pMethod);
}

void CComboBoxListItem::RemoveValueChangedSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    xComboBoxHandler * pCmbHandler = m_pComboBox->GetComboBoxHandle();

    if (NULL == pCmbHandler)
    {
        return;
    }

    pCmbHandler->signalValueChanged.disconnect(pHandler, pMethod);
}

void CComboBoxListItem::Release()
{
    RemoteSoftkey();

    if (NULL != m_pComboBox)
    {
        m_pComboBox->Clear();
    }

    CDetailListItemBase::Release();
}

void CComboBoxListItem::SetItemFocus(bool bFocus)
{
    CDetailListItemBase::SetItemFocus(bFocus);

    if (NULL != m_pComboBox)
    {
        if (bFocus)
        {
            m_pComboBox->setFocus();

            SetupSoftkey();
        }
        else
        {
            RemoteSoftkey();
        }
    }
}

bool CComboBoxListItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pComboBox)
    {
        if (bFocusIn)
        {
            m_pComboBox->EnableScroll(true);
            m_pComboBox->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pComboBox->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

    return CDetailListItemBase::onFocusEvent(bFocusIn);
}

yl::string CComboBoxListItem::GetValue() const
{
    return GetItemStringData();
}

void CComboBoxListItem::AddValue(const yl::string & strValue, int nData)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    char szBuffer[32];

    _snprintf(szBuffer, sizeof(szBuffer), "%d", nData);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    m_pComboBox->AddItem(strValue, szBuffer);
}

void CComboBoxListItem::AddValue(const yl::string & strValue, const yl::string & strData)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    m_pComboBox->AddItem(strValue, strData);
}

void CComboBoxListItem::ClearValue()
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox->Clear();
    }
}

void CComboBoxListItem::SetItemData(int nData, int nDefaultIndex/* = 0*/)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    char szBuffer[32];

    _snprintf(szBuffer, sizeof(szBuffer), "%d", nData);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    SetItemData(szBuffer, nDefaultIndex);
}

void CComboBoxListItem::SetItemData(const yl::string & strData, int nDefaultIndex/* = 0*/)
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    m_pComboBox->SetValue(strData);

    yl::string strValue = m_pComboBox->GetValue();

    // 没有相应的Data, 则选中默认序号
    if (strValue.empty() || strValue != strData)
    {
        m_pComboBox->SetCurrentIndex(nDefaultIndex);
    }
}

void CComboBoxListItem::SetCurrentIndex(int nIndex)
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox->SetCurrentIndex(nIndex);
    }
}

int CComboBoxListItem::GetItemData() const
{
    if (NULL == m_pComboBox)
    {
        return 0;
    }

    yl::string strValue = m_pComboBox->GetValue();

    if (strValue.empty())
    {
        return 0;
    }

    return atoi(strValue.c_str());
}

yl::string CComboBoxListItem::GetItemStringData() const
{
    if (NULL == m_pComboBox)
    {
        return yl::string("");
    }

    return m_pComboBox->GetValue();
}

void CComboBoxListItem::SwitchNextValue()
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    xComboBoxHandler * pComboBoxHandler = xComboBoxHandler::GetHandle(m_pComboBox->m_pHandler);

    if (NULL != pComboBoxHandler)
    {
        pComboBoxHandler->SwitchValue(m_pComboBox, true);
    }
}

void CComboBoxListItem::SwitchPreValue()
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    xComboBoxHandler * pComboBoxHandler = xComboBoxHandler::GetHandle(m_pComboBox->m_pHandler);

    if (NULL != pComboBoxHandler)
    {
        pComboBoxHandler->SwitchValue(m_pComboBox, false);
    }
}

void CComboBoxListItem::SetupSoftkey()
{
    if (NULL == m_pComboBox || !m_bSetupSoftkeyByItem || m_pComboBox->GetItemCount() <= 1
            || !m_pComboBox->IsEnable())
    {
        return;
    }

    if (m_bSwitchSoftKey)
    {
        setupViewSoftKey(m_pComboBox, COMBOBOX_LIST_ITEM_ST_SWITCH_INDEX, _LANG(TRID_SWITCH), ST_SWITCH,
                         SLOT_METHOD(this, CComboBoxListItem::OnSoftKeyPress));
        return;
    }

    CIconHotKey * pLeftKey = new CIconHotKey;
    CIconHotKey * pRightKey = new CIconHotKey;

    if (NULL == pLeftKey || NULL == pRightKey)
    {
        if (NULL != pLeftKey)
        {
            delete pLeftKey;
            pLeftKey = NULL;
        }

        if (NULL != pRightKey)
        {
            delete pRightKey;
            pRightKey = NULL;
        }

        return;
    }

    pLeftKey->SetType(ST_LEFT);
    pLeftKey->SetIndex(COMBOBOX_LIST_ITEM_ST_LEFT_INDEX);
    pLeftKey->SetupPressSlot(this, static_cast<slotMethod>(&CComboBoxListItem::OnSoftKeyPress));
    pLeftKey->SetIcon(PIC_LEFT_KEY);

    pRightKey->SetType(ST_RIGHT);
    pRightKey->SetIndex(COMBOBOX_LIST_ITEM_ST_RIGHT_INDEX);
    pRightKey->SetupPressSlot(this, static_cast<slotMethod>(&CComboBoxListItem::OnSoftKeyPress));
    pRightKey->SetIcon(PIC_RIGHT_KEY);

    if (!setupViewSoftKey(m_pComboBox, pLeftKey))
    {
        delete pLeftKey;
        pLeftKey = NULL;
    }

    if (!setupViewSoftKey(m_pComboBox, pRightKey))
    {
        delete pRightKey;
        pRightKey = NULL;
    }
}

void CComboBoxListItem::RemoteSoftkey()
{
    if (NULL == m_pComboBox)
    {
        return;
    }

    if (m_bSwitchSoftKey)
    {
        removeViewSoftKey(this, COMBOBOX_LIST_ITEM_ST_SWITCH_INDEX);
    }
    else
    {
        removeViewSoftKey(this, COMBOBOX_LIST_ITEM_ST_LEFT_INDEX);
        removeViewSoftKey(this, COMBOBOX_LIST_ITEM_ST_RIGHT_INDEX);
    }
}

void CComboBoxListItem::OnSoftKeyPress(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                       int nDataBytes, LPVOID pData)
{
    if (ST_SWITCH == wParam || ST_RIGHT == wParam)
    {
        SwitchNextValue();
    }
    else if (ST_LEFT == wParam)
    {
        SwitchPreValue();
    }
}

void CComboBoxListItem::SetComboBoxStyle(yl::string strStyle)
{
    if (m_pComboBox != NULL)
    {
        m_pComboBox->setStyle(strStyle);
    }
}

void CComboBoxListItem::SetSwitchSoftKey(bool bSwitch)
{
#if IF_COMBOBOX_ITEM_DEFAULT_SWITCH
    bSwitch = true;
#endif

    if (m_bSwitchSoftKey == bSwitch)
    {
        return;
    }

    RemoteSoftkey();

    m_bSwitchSoftKey = bSwitch;

    if (IsItemFocus())
    {
        SetupSoftkey();
    }
}

void CComboBoxListItem::SetComboBoxDisable(bool bDisable)
{
    if (NULL != m_pComboBox)
    {
        m_pComboBox->setDisable(bDisable);
        setDisable(false);
    }
}

void CComboBoxListItem::SetIsSetupSoftkeyByItem(bool bSetupSoftkeyByItem)
{
    m_bSetupSoftkeyByItem = bSetupSoftkeyByItem;
}

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
bool CComboBoxListItem::IsShowFocusBG()
{
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
    return false;
#endif

    return xListViewItem::IsShowFocusBG();
}
#endif

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CComboBoxListItem::UpdateTextView(bool bFocus)
{
    CDetailListItemBase::UpdateTextView(bFocus);

    if (NULL == m_pComboBox || !m_pComboBox->isVisible())
    {
        return;
    }

    if (!m_pComboBox->IsEnable())
    {
        m_pComboBox->SetTextColor(m_colorDisable);
    }
    else if (bFocus)
    {
        m_pComboBox->SetTextColor(m_colorFocus);
    }
    else
    {
        m_pComboBox->SetTextColor(m_colorNormal);
    }
}
#endif
