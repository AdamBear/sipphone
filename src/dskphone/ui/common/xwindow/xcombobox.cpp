#include "xcombobox.h"
#include "keymap.h"
#include "xWindowManager.h"
#include "commonapi/stringhelperapi.h"
#include "xwindow-kernel/include/xsignal.h"
#include "modviewdump.h"


EXPORT_VIEW_CLASS(xComboBox);

yl::string xComboBox::s_strGlobalStyle = "";

xComboBox::xComboBox()
#if IF_COMBOBOX_PICKUP_ENABLE
    : m_pPickupView(NULL)
    , m_strPickupTitle("")
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xComboBox)

#if IF_COMBOBOX_PICKUP_ENABLE
    SLOT_CONNECT(this, signalAppear, this, xComboBox::OnAppearSlot);
    SLOT_CONNECT(this, signalClosed, this, xComboBox::OnCloseSlot);
#endif
#if IF_GESTURE_ENABLE
    grabGesture(GESTURE_TYPE_TAP);
#endif
}

xComboBox::~xComboBox()
{
#if IF_COMBOBOX_PICKUP_ENABLE
    SLOT_DISCONNECT(this, signalAppear, this, xComboBox::OnAppearSlot);
    SLOT_DISCONNECT(this, signalClosed, this, xComboBox::OnCloseSlot);
#endif
#if IF_GESTURE_ENABLE
    clearGesture();
#endif
}

void xComboBox::SetGlobalStyle(const yl::string& strStyle)
{
    s_strGlobalStyle = strStyle;
}

xComboBoxHandler* xComboBox::GetComboBoxHandle() const
{
    return xComboBoxHandler::GetHandle(m_pHandler);
}

void xComboBox::loadChildrenElements(xml_node& node)
{
    xView::loadChildrenElements(node);

    if (NULL == m_pHandler)
    {
        xComboBoxHandler* pHandle = new xComboBoxHandler;

        if (NULL == pHandle)
        {
            return;
        }

        xPresentGroup objGroup;

        objGroup.m_strGroupName = m_strId;
        objGroup.m_listPresentViews.push_back(this);
        pHandle->m_listPresentGroup.push_back(objGroup);

        m_pHandler = pHandle;
    }
}

void xComboBox::loadAttributes(xml_node& node)
{
    xTextView::loadAttributes(node);

    if (NULL != m_pStyle || s_strGlobalStyle.empty())
    {
        return;
    }

    m_pStyle = xStyle::getObjectByName(s_strGlobalStyle);
}

void xComboBox::showValue(const yl::string& strValue)
{
    SetText(strValue);
}

bool xComboBox::AddItem(const yl::string& strText, const yl::string& strValue)
{
    xComboBoxHandler* pHandle = GetComboBoxHandle();

    if (NULL == pHandle || 0 == pHandle->m_listPresentGroup.size())
    {
        return false;
    }

    xPresentGroup& objGroup = pHandle->m_listPresentGroup.back();
    LIST_VALUE_PRESENT& listValuePresent = objGroup.m_listValuePresent;

    bool bEmpty = (0 == listValuePresent.size());

    ST_VALUE_PRESENT stValuePresent;

    stValuePresent.strValue = strValue;
    stValuePresent.strPresent = strText;

    listValuePresent.push_back(stValuePresent);

    if (bEmpty)
    {
        pHandle->m_strValue = strValue;
        showValue(strText);
    }

    return true;
}

void xComboBox::Clear()
{
    xComboBoxHandler* pHandle = GetComboBoxHandle();

    if (NULL == pHandle)
    {
        return;
    }

    pHandle->m_strValue = "";
    showValue(pHandle->m_strValue);

    if (0 == pHandle->m_listPresentGroup.size())
    {
        return;
    }

    xPresentGroup& objGroup = pHandle->m_listPresentGroup.back();

    objGroup.m_listValuePresent.clear();
}

void xComboBox::SetValue(const yl::string& strValue)
{
    xComboBoxHandler* pHandle = GetComboBoxHandle();

    if (NULL == pHandle || 0 == pHandle->m_listPresentGroup.size())
    {
        return;
    }

    xPresentGroup& objGroup = pHandle->m_listPresentGroup.back();
    yl::string strPresent;

    if (!objGroup.GetPresentByValue(strValue, strPresent))
    {
        return;
    }

    pHandle->m_strValue = strValue;
    showValue(strPresent);
}

void xComboBox::SetCurrentIndex(int nIndex)
{
    xComboBoxHandler* pHandle = GetComboBoxHandle();

    if (NULL == pHandle || 0 == pHandle->m_listPresentGroup.size())
    {
        return;
    }

    xPresentGroup& objGroup = pHandle->m_listPresentGroup.back();
    yl::string strValue;
    yl::string strPresent;

    if (objGroup.GetValuePresentByIndex(nIndex, strValue, strPresent))
    {
        pHandle->m_strValue = strValue;
        showValue(strPresent);
    }
}

int xComboBox::GetItemCount() const
{
    xComboBoxHandler* pHandle = GetComboBoxHandle();

    if (NULL == pHandle || 0 == pHandle->m_listPresentGroup.size())
    {
        return 0;
    }

    xPresentGroup& objGroup = pHandle->m_listPresentGroup.back();

    return objGroup.m_listValuePresent.size();
}

yl::string xComboBox::GetValue() const
{
    xComboBoxHandler* pHandle = GetComboBoxHandle();

    if (NULL != pHandle)
    {
        return pHandle->getValue();
    }

    return "";
}

#if IF_COMBOBOX_PICKUP_ENABLE

void xComboBox::SetPickupTitle(const yl::string& strTitle)
{
    m_strPickupTitle = strTitle;
}

yl::string xComboBox::GetPickupTitle() const
{
    return m_strPickupTitle;
}

void xComboBox::SetPickupView(xComboBoxPickupView* pPickupView)
{
    m_pPickupView = pPickupView;
}

xComboBoxPickupView* xComboBox::GetPickupView() const
{
    return m_pPickupView;
}

void xComboBox::OnAppearSlot(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (0 != wParam)
    {
        return;
    }

    OnDisappear();
}

void xComboBox::OnCloseSlot(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    OnDisappear();
}

void xComboBox::OnDisappear()
{
    if (NULL == m_pPickupView)
    {
        return;
    }

    xComboBoxPickupView* pPickupView = m_pPickupView;

    m_pPickupView = NULL;

    pPickupView->OnComboBoxDisappear(this);
}
#endif

bool xComboBox::onKeyPressedEvent(int nKeyCode)
{
    xComboBoxHandler * pComboBoxHandler = xComboBoxHandler::GetHandle(m_pHandler);
    if (NULL != pComboBoxHandler)
    {
        if (PHONE_KEY_RIGHT == nKeyCode)
        {
            pComboBoxHandler->SwitchValue(this, true);
            return true;
        }
        else if (PHONE_KEY_LEFT == nKeyCode)
        {
            pComboBoxHandler->SwitchValue(this, false);
            return true;
        }
    }

    return xTextView::onKeyPressedEvent(nKeyCode);
}

#if IF_GESTURE_ENABLE
bool xComboBox::onGestureEvent(xGestureEvent* pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    xGesturePtr pGesture = pEvent->GetGestureByType(GESTURE_TYPE_TAP);
    xTapGesture* pTapGesture = xTapGesture::GetTapGesture(pGesture.Get());

    if (NULL != pTapGesture && GESTURE_STATE_FINISH == pTapGesture->GetState())
    {
        onClick();
        signalClicked.emitSignal(0, 0, 0, NULL);
        return true;
    }

    return false;
}
#endif
