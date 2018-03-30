#include "ETLLib/ETLLib.hpp"

#include <ylvector.h>
#include "keymap.h"
#include "xWindow.h"
#include "xWindowManager.h"
#include "xSoftkeyBar.h"
#if IF_VIEW_USER_ACTION
#include "gsCallMethod.h"
#endif
#include "translateiddefine.h"

namespace
{
#define SOFTKEY_BTN_ID_PREFIX       "softkey"
}

//////////////////////////////////////////////////////////////////////////////////////////////
xHotKeyObject::xHotKeyObject(int nObjectType/* = HOT_KEY_OBJECT_TYPE_NONE*/)
    : m_nObjectType(nObjectType)
    , m_nType(0)
    , m_nIndex(-1)
{

}

xHotKeyObject::~xHotKeyObject()
{

}

void xHotKeyObject::UpdateHotKeyBtn(xButton* pBtn)
{
    if (NULL != pBtn)
    {
        pBtn->Clear();
        pBtn->SetText(m_strTitle);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
xSlotHotKey::xSlotHotKey()
    : xHotKeyObject(HOT_KEY_OBJECT_TYPE_SLOT)
    , m_pReceiver(NULL)
    , m_slot(NULL_METHOD)
    , m_pLongPressReceiver(NULL)
    , m_slotLongPress(NULL_METHOD)
{
}

xSlotHotKey::~xSlotHotKey()
{

}

xSlotHotKey* xSlotHotKey::GetSlotHotKey(xHotKeyObject* pHotKey)
{
    if (NULL != pHotKey && HOT_KEY_OBJECT_TYPE_SLOT != (HOT_KEY_OBJECT_TYPE_SLOT & pHotKey->GetObjectType()))
    {
        return static_cast<xSlotHotKey*>(pHotKey);
    }

    return NULL;
}

void xSlotHotKey::SetupPressSlot(xSlotHandler* pReceiver, slotMethod slot)
{
    m_pReceiver = pReceiver;
    m_slot = slot;
}

void xSlotHotKey::SetupLongPressSlot(xSlotHandler* pReceiver, slotMethod slot)
{
    m_pLongPressReceiver = pReceiver;
    m_slotLongPress = slot;
}

void xSlotHotKey::emitHotKey(bool bLongPressed)
{
    if (bLongPressed)
    {
        if (m_pLongPressReceiver != NULL && m_slotLongPress != NULL_METHOD)
        {
            ((m_pLongPressReceiver)->*(m_slotLongPress))(NULL, m_nType, m_nIndex, 0, this);
        }
    }
    else
    {
        if (m_pReceiver != NULL && m_slot != NULL_METHOD)
        {
            ((m_pReceiver)->*(m_slot))(NULL, m_nType, m_nIndex, 0, this);
        }
    }
}

bool xSlotHotKey::isEnableLongPress()
{
    return m_pLongPressReceiver != NULL && m_slotLongPress != NULL_METHOD;
}

//////////////////////////////////////////////////////////////////////////////////////////////
#if IF_VIEW_USER_ACTION
xActionHotKey::xActionHotKey()
    : xHotKeyObject(HOT_KEY_OBJECT_TYPE_ACTION)
{

}

xActionHotKey::~xActionHotKey()
{

}

xActionHotKey* xActionHotKey::GetActionHotKey(xHotKeyObject* pHotKey)
{
    if (NULL != pHotKey && HOT_KEY_OBJECT_TYPE_ACTION == (HOT_KEY_OBJECT_TYPE_ACTION & pHotKey->GetObjectType()))
    {
        return static_cast<xActionHotKey*>(pHotKey);
    }

    return NULL;
}

void xActionHotKey::emitHotKey(bool bLongPressed)
{
    call_text_method(bLongPressed ? m_strActionLongPressed.c_str() : m_strAction.c_str());
}

bool xActionHotKey::isEnableLongPress()
{
    return !m_strActionLongPressed.empty();
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// xSoftKeys
static yl::string keySoftkeys = "softkeys";
typedef YLSmartPtr<xHotKeyObject>       typeHotKeyFrame;
typedef YLList<typeHotKeyFrame>     typeHotKeys;
class xSoftKeys : public ExtraData
{
public:
    xSoftKeys();
    virtual ~xSoftKeys();

    static xSoftKeys* GetViewSoftKeys(xView* pView);
    static xSoftKeys* GainViewSoftKeys(xView* pView);

public:
    bool AddHotKey(xHotKeyObject* pHotKey);

#if IF_VIEW_USER_ACTION
    xActionHotKey* addKey(int nIndex, const yl::string& strTitle, int nType,
                          const yl::string& strAction, const yl::string& strActionL);
#endif

    bool removeKey(int nIndex);

    bool ClearKeys();

    typeHotKeyFrame keyAt(int nIndex);
    int GetMaxSoftKeyIndex();

protected:
    typeHotKeys m_listHotKeys;
};

xSoftKeys::xSoftKeys()
{

}

xSoftKeys::~xSoftKeys()
{

}

xSoftKeys* xSoftKeys::GetViewSoftKeys(xView* pView)
{
    if (NULL == pView)
    {
        return NULL;
    }

    ViewDataFrame pDataFrame = pView->getProperty(keySoftkeys);

    return static_cast<xSoftKeys*>(pDataFrame.Get());
}

xSoftKeys* xSoftKeys::GainViewSoftKeys(xView* pView)
{
    if (NULL == pView)
    {
        return NULL;
    }

    xSoftKeys* pSoftKeys = GetViewSoftKeys(pView);

    if (NULL != pSoftKeys)
    {
        return pSoftKeys;
    }

    pSoftKeys = new xSoftKeys;

    if (NULL != pSoftKeys)
    {
        ViewDataFrame pDataFrame(pSoftKeys);

        pView->setProperty(keySoftkeys, pDataFrame);
    }

    return pSoftKeys;
}

bool xSoftKeys::AddHotKey(xHotKeyObject* pHotKey)
{
    if (NULL == pHotKey)
    {
        return false;
    }

    int nIndex = pHotKey->GetIndex();

    chASSERT(nIndex >= 0);

    typeHotKeys::iterator iter = m_listHotKeys.begin();

    for (; iter != m_listHotKeys.end(); ++iter)
    {
        typeHotKeyFrame& hotKey = *iter;
        xHotKeyObject* pOriHotKey = hotKey.Get();

        if (NULL == pOriHotKey)
        {
            continue;
        }

        int nOriIndex = pOriHotKey->GetIndex();

        if (nOriIndex == nIndex)
        {
            if (pOriHotKey != pHotKey)
            {
                hotKey = typeHotKeyFrame(pHotKey);
            }

            return true;
        }
        else if (nOriIndex > nIndex)
        {
            break;
        }
    }

    typeHotKeyFrame pHotKeyFrame(pHotKey);

    if (iter != m_listHotKeys.end())
    {
        m_listHotKeys.insert(iter, pHotKeyFrame);
    }
    else
    {
        m_listHotKeys.push_back(pHotKeyFrame);
    }

    return true;
}

#if IF_VIEW_USER_ACTION
xActionHotKey* xSoftKeys::addKey(int nIndex, const yl::string& strTitle, int nType,
                                 const yl::string& strAction, const yl::string& strActionL)
{
    chASSERT(nIndex >= 0);

    typeHotKeyFrame pHotKey = keyAt(nIndex);
    xActionHotKey* pActionHotKey = xActionHotKey::GetActionHotKey(pHotKey.Get());
    bool bAdd = false;

    if (NULL == pActionHotKey)
    {
        if (pHotKey.IsEmpty())
        {
            removeKey(nIndex);
        }

        pActionHotKey = new xActionHotKey;
        bAdd = true;

        if (NULL == pActionHotKey)
        {
            return NULL;
        }
    }

    pActionHotKey->SetType(nType);
    pActionHotKey->SetIndex(nIndex);
    pActionHotKey->SetTitle(strTitle);
    pActionHotKey->SetAction(strAction);
    pActionHotKey->SetLongPressAction(strActionL);

    if (bAdd)
    {
        AddHotKey(pActionHotKey);
    }

    return pActionHotKey;
}
#endif

bool xSoftKeys::removeKey(int nIndex)
{
    typeHotKeys::iterator iter = m_listHotKeys.begin();

    for (; iter != m_listHotKeys.end(); ++iter)
    {
        typeHotKeyFrame& pHotKeyFrame = *iter;
        xHotKeyObject* pHotKey = pHotKeyFrame.Get();

        if (NULL != pHotKey && pHotKey->GetIndex() == nIndex)
        {
            m_listHotKeys.erase(iter);

            return true;
        }
    }

    return false;
}

bool xSoftKeys::ClearKeys()
{
    if (m_listHotKeys.size() > 0)
    {
        m_listHotKeys.clear();

        return true;
    }

    return false;
}

typeHotKeyFrame xSoftKeys::keyAt(int nIndex)
{
    typeHotKeys::iterator iter = m_listHotKeys.begin();

    for (; iter != m_listHotKeys.end(); ++iter)
    {
        typeHotKeyFrame& pHotKeyFrame = *iter;
        xHotKeyObject* pHotKey = pHotKeyFrame.Get();

        if (NULL != pHotKey && pHotKey->GetIndex() == nIndex)
        {
            return pHotKeyFrame;
        }
    }

    return typeHotKeyFrame(NULL);
}

int xSoftKeys::GetMaxSoftKeyIndex()
{
    if (m_listHotKeys.size() <= 0)
    {
        return -1;
    }

    typeHotKeyFrame& pHotKeyFrame = m_listHotKeys.back();
    xHotKeyObject* pHotKey = pHotKeyFrame.Get();

    if (NULL == pHotKey)
    {
        chASSERT(0);
        return -1;
    }

    return pHotKey->GetIndex();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// xSoftKeyBar
class xSoftKeyBar : public xMessageHandler, public xLinearLayout, public xSlotHandler
{
public:
    void updateSoftKeys();
    void setFirstBtnKey(int nPhoneKey);
    void setBtnKeyOverlapMode(bool bOverlap);
    void ResetPageIndex();

    void OnViewSoftKeyChanged(xView* pView);

private:
    int getMaxSoftKeyIndex(xView* pRootView, xView* pFocusView);
    typeHotKeyFrame getHotKeyObject(xView* pRootView, xView* pFocusView, int nIndex);

    void slotActiveFocusEvent(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
    {
        if (!m_bUpdatePending)
        {
            m_bUpdatePending = true;
        }

        updateSoftKeys();
    }

    void slotKeyEvent(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
    {
        if (1 == lParam)
        {
            // press
            emitHotkeySignal(getKeyIndex(wParam), sender, false);
        }
    }

    void slotKeyLongPressEvent(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
    {
        if (lParam != 0)
        {
            emitHotkeySignal(getKeyIndex(wParam), sender, true);
        }
    }

    int getButtonIndex(xSignal* pSender);
    int getKeyIndex(int key);

    void emitHotkeySignal(int iItemIndex, xSignal* sender, bool bLongPressed);

    void slotMore(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

#if IF_VIEW_USER_ACTION
    void slotCustomElement(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
#endif
public:
    xSoftKeyBar();
    ~xSoftKeyBar();

    static xSoftKeyBar& getSoftKeyBar();

private:
    typedef struct
    {
        typeHotKeyFrame pHotKey;
        xButton* pBtn;
    } typeKeyButton;
    typedef YLVector<typeKeyButton> VEC_HOTKEY_BTN;

private:
    xView*                                      m_pActiveView;
    int                                         m_iPageIndex;
    VEC_HOTKEY_BTN                              m_arrHotKeys;
    typeHotKeyFrame                             m_pHotKeyMore;
    int                                         m_nFirstBtnKey;
    bool                                        m_bUpdatePending;
};

#define g_SoftKeyBar xSoftKeyBar::getSoftKeyBar()

xSoftKeyBar& xSoftKeyBar::getSoftKeyBar()
{
#if IF_MULTI_XKERNEL_ENABLE
    MULTI_KERNEL_INSTANCE(xSoftKeyBar)
#else
    SINGLE_KERNEL_INSTANCE(xSoftKeyBar)
#endif
}

xSoftKeyBar::xSoftKeyBar()
    : m_nFirstBtnKey(PHONE_KEY_FN1)
    , m_bUpdatePending(true)
{
    xSlotHotKey* pHotKeyMore = new xSlotHotKey;

    if (NULL != pHotKeyMore)
    {
        pHotKeyMore->SetTitle(g_WorkStation.Lang(TRID_MORE));
        pHotKeyMore->SetType(-1);
        pHotKeyMore->SetupPressSlot(this, static_cast<slotMethod>(&xSoftKeyBar::slotMore));

        m_pHotKeyMore = typeHotKeyFrame(pHotKeyMore);
    }

#if IF_VIEW_USER_ACTION
    SLOT_CONNECT(m_pHostStation, signalCustomElement, this, xSoftKeyBar::slotCustomElement);
#endif

    loadContent("softkey-bar.xml");
    m_pWindow = m_pHostStation->createWindow(this, TML_USER + 1);

    m_iPageIndex = 0;
    m_pActiveView = NULL;

    for (xViewIterator it = child_head(); it.hasData(); ++it)
    {
        xView* pChildView = static_cast<xView*>(it.operator ->());
        if (NULL == pChildView)
        {
            continue;
        }

        yl::string strViewID = pChildView->m_strId;
        if (strViewID.find(SOFTKEY_BTN_ID_PREFIX) == yl::string::npos)
        {
            continue;
        }

#if IF_XWINDOW_RTTI_CHECK
        xButton* btn = dynamic_cast<xButton*>(pChildView);
#else
        xButton* btn = static_cast<xButton*>(pChildView);
#endif

        chASSERT(btn != NULL);
        //SLOT_CONNECT(btn, signalClicked, this, xSoftKeyBar::slotButtonClick);

        typeKeyButton objKeyBtn;

        objKeyBtn.pBtn = btn;
        objKeyBtn.pHotKey = typeHotKeyFrame(NULL);

        m_arrHotKeys.push_back(objKeyBtn);
    }

    SLOT_CONNECT(&ownerStation(), signalKeyEvent, this, xSoftKeyBar::slotKeyEvent);
    SLOT_CONNECT(&ownerStation(), signalLongKeyEvent, this, xSoftKeyBar::slotKeyLongPressEvent);

    SLOT_CONNECT(&ownerStation(), signalFocusEvent, this, xSoftKeyBar::slotActiveFocusEvent);
    SLOT_CONNECT(&ownerStation(), signalPendTillFocus, this, xSoftKeyBar::slotActiveFocusEvent);

    updateSoftKeys();
}

xSoftKeyBar::~xSoftKeyBar()
{
}

void xSoftKeyBar::setFirstBtnKey(int nPhoneKey)
{
    m_nFirstBtnKey = nPhoneKey;
}

void xSoftKeyBar::setBtnKeyOverlapMode(bool bOverlap)
{
    for (xViewIterator it = child_head(); it.hasData(); ++it)
    {
#if IF_XWINDOW_RTTI_CHECK
        xButton* btn = dynamic_cast<xButton*>(it.operator ->());
#else
        xButton* btn = static_cast<xButton*>(it.operator ->());
#endif

        if (btn)
        {
            btn->setOverlapMode(bOverlap);
        }
    }
}

void xSoftKeyBar::ResetPageIndex()
{
    m_iPageIndex = 0;
}

void xSoftKeyBar::OnViewSoftKeyChanged(xView* pView)
{
    if (m_bUpdatePending)
    {
        return;
    }

#if IF_SOFTKEY_BAR_RECURSIVE_VIEW
    if (NULL != pView && pView->getRootView()->isActiveWindow())
#else
    if (NULL != pView && (pView->isActiveWindow() || pView->hasFocus()))
#endif
    {
        m_bUpdatePending = true;
        g_WorkStation.PostPendTillFocus();
    }
}

int xSoftKeyBar::getMaxSoftKeyIndex(xView* pRootView, xView* pFocusView)
{
    int nMaxSoftKeyIndex = 0;

#if IF_SOFTKEY_BAR_RECURSIVE_VIEW
    chASSERT(pRootView != NULL);

    xView* pLeafView = pFocusView;

    for (xView *pEndView = pRootView->parent(); pLeafView != pEndView; pLeafView = pLeafView->parent())
    {
        chASSERT(pLeafView != NULL);

        xSoftKeys* pSoftKeys = xSoftKeys::GetViewSoftKeys(pLeafView);

        if (NULL == pSoftKeys)
        {
            continue;
        }

        int nSoftKeyIndex = pSoftKeys->GetMaxSoftKeyIndex();

        if (nSoftKeyIndex > nMaxSoftKeyIndex)
        {
            nMaxSoftKeyIndex = nSoftKeyIndex;
        }
    }
#else
    xSoftKeys* pRootSoftKeys = xSoftKeys::GetViewSoftKeys(pRootView);

    if (NULL != pRootSoftKeys)
    {
        nMaxSoftKeyIndex = pRootSoftKeys->GetMaxSoftKeyIndex();
    }

    xSoftKeys* pFocusSoftKeys = xSoftKeys::GetViewSoftKeys(pFocusView);

    if (NULL != pFocusSoftKeys)
    {
        int nMaxFocusKeyIndex = pFocusSoftKeys->GetMaxSoftKeyIndex();

        if (nMaxFocusKeyIndex > nMaxSoftKeyIndex)
        {
            nMaxSoftKeyIndex = nMaxFocusKeyIndex;
        }
    }
#endif

    return nMaxSoftKeyIndex;
}

typeHotKeyFrame xSoftKeyBar::getHotKeyObject(xView* pRootView, xView* pFocusView, int nIndex)
{
#if IF_SOFTKEY_BAR_RECURSIVE_VIEW
    chASSERT(pRootView != NULL);

    for (xView *pEndView = pRootView->parent(); pLeafView != pEndView; pLeafView = pLeafView->parent())
    {
        chASSERT(pLeafView != NULL);
        xSoftKeys* pSoftkeys = xSoftKeys::GetViewSoftKeys(pLeafView);

        if (NULL != pSoftkeys)
        {
            typeHotKeyFrame pHotKeyFrame = pSoftkeys->keyAt(iIndex);

            if (!pHotKeyFrame.IsEmpty())
            {
                return pHotKeyFrame;
            }
        }
    }
#else
    xSoftKeys* pFocusSoftKeys = xSoftKeys::GetViewSoftKeys(pFocusView);

    if (NULL != pFocusSoftKeys)
    {
        typeHotKeyFrame pHotKeyFrame = pFocusSoftKeys->keyAt(nIndex);

        if (!pHotKeyFrame.IsEmpty())
        {
            return pHotKeyFrame;
        }
    }

    xSoftKeys* pRootSoftKeys = xSoftKeys::GetViewSoftKeys(pRootView);

    if (NULL != pRootSoftKeys)
    {
        typeHotKeyFrame pHotKeyFrame = pRootSoftKeys->keyAt(nIndex);

        if (!pHotKeyFrame.IsEmpty())
        {
            return pHotKeyFrame;
        }
    }
#endif

    return typeHotKeyFrame(NULL);
}

int xSoftKeyBar::getButtonIndex(xSignal* pSender)
{
    int nCount = m_arrHotKeys.size();

    for (int i = 0; i < nCount; ++i)
    {
        typeKeyButton& objKeyBtn = m_arrHotKeys.at(i);

        if (&(objKeyBtn.pBtn->signalClicked) == pSender)
        {
            return i;
        }
    }

    return -1;
}

int xSoftKeyBar::getKeyIndex(int key)
{
    int nKeyIndex = key - m_nFirstBtnKey;

    if (nKeyIndex < 0 || nKeyIndex >= m_arrHotKeys.size())
    {
        return -1;
    }

    return nKeyIndex;
}

void xSoftKeyBar::updateSoftKeys()
{
    if (!m_bUpdatePending)
    {
        return;
    }

    m_bUpdatePending = false;

    xView* pActiveView = ownerStation().getActiveWindow();

    if (m_pActiveView != pActiveView)
    {
        m_pActiveView = pActiveView;
        m_iPageIndex = 0;
    }

    if (NULL == pActiveView)
    {
        for (int i = 0; i < m_arrHotKeys.size(); ++i)
        {
            m_arrHotKeys[i].pHotKey = typeHotKeyFrame(NULL);
            m_arrHotKeys[i].pBtn->Clear();
        }
    }
    else
    {
        xView* pFocusView = ownerStation().getFocusView();
        int nMaxSoftKey = getMaxSoftKeyIndex(pActiveView, pFocusView) + 1;
        int nSoftKeys = m_arrHotKeys.size();
        int iLastKey = nSoftKeys - 1;
        int nPageOffset = 0;

        if (nMaxSoftKey > nSoftKeys && !m_pHotKeyMore.IsEmpty())
        {
            // has more button
            typeHotKeyFrame& pHotKey = m_arrHotKeys[iLastKey].pHotKey;

            if (pHotKey.Get() != m_pHotKeyMore.Get())
            {
                m_arrHotKeys[iLastKey].pHotKey = m_pHotKeyMore;
            }

            xButton* pBtn = m_arrHotKeys[iLastKey].pBtn;

            m_pHotKeyMore->UpdateHotKeyBtn(pBtn);

            nSoftKeys -= 1;

            nPageOffset = m_iPageIndex * iLastKey;
            if (nPageOffset >= nMaxSoftKey)
            {
                m_iPageIndex = 0;
                nPageOffset = 0;
            }
        }
        else if (m_iPageIndex > 0)
        {
            m_iPageIndex = 0;
        }

        for (int i = 0; i < nSoftKeys; ++i)
        {
            typeHotKeyFrame pHotKeyFrame = getHotKeyObject(pActiveView, pFocusView, nPageOffset + i);
            typeHotKeyFrame pOldHotKeyFrame = m_arrHotKeys[i].pHotKey;

            if (pHotKeyFrame.Get() == pOldHotKeyFrame.Get())
            {
                continue;
            }

            m_arrHotKeys[i].pHotKey = pHotKeyFrame;

            xButton* pBtn = m_arrHotKeys[i].pBtn;

            if (NULL == pBtn)
            {
                continue;
            }

            if (pHotKeyFrame.IsEmpty())
            {
                pBtn->Clear();
            }
            else
            {
                pHotKeyFrame->UpdateHotKeyBtn(pBtn);
            }
        }
    }
}

void xSoftKeyBar::emitHotkeySignal(int iItemIndex, xSignal* /*sender*/, bool bLongPressed)
{
    xView* pActiveView = ownerStation().getActiveWindow();

    if (m_pActiveView != pActiveView)
    {
        return;
    }

    int nHotKeysCount = m_arrHotKeys.size();

    if (iItemIndex >= 0 && iItemIndex < nHotKeysCount && m_pActiveView != NULL)
    {
        typeHotKeyFrame& pHotKey = m_arrHotKeys[iItemIndex].pHotKey;

        if (!pHotKey.IsEmpty())
        {
            pHotKey->emitHotKey(bLongPressed);
        }
    }
}

void xSoftKeyBar::slotMore(xSignal* sender, WPARAM wParam, LPARAM lParam,
                           int nDataBytes, LPVOID pData)
{
    if (NULL != m_pActiveView)
    {
        ++m_iPageIndex;

        m_bUpdatePending = true;

        updateSoftKeys();
    }
}

#if IF_VIEW_USER_ACTION
void xSoftKeyBar::slotCustomElement(xSignal* sender, WPARAM wParam, LPARAM lParam,
                                    int nDataBytes, LPVOID pData)
{
    xView* pView = (xView*)wParam;
    xml_node& node = *(xml_node*)lParam;
    yl::string strTagName = node.name();

    if (strTagName == keySoftkeys)
    {
        xml_node nodeChild = node.first_child();
        xSoftKeys* pSoftKeys = xSoftKeys::GainViewSoftKeys(pView);

        if (NULL == pSoftKeys)
        {
            return;
        }

        while (!nodeChild.empty())
        {
            yl::string strChildName = nodeChild.name();
            int nIndex = atoi(strChildName.c_str());
            yl::string strTitle = nodeChild.attribute(XWINDOW_NAME_TITLE).as_string();
            yl::string strOnClick = nodeChild.attribute(XWINDOW_NAME_ON_CLICK).as_string();
            yl::string strOnLongClick = nodeChild.attribute(XWINDOW_NAME_ON_LONG_CLICK).as_string();

            pSoftKeys->addKey(nIndex, strTitle, 0, strOnClick, strOnLongClick);

            nodeChild = nodeChild.next_sibling();
        }

        g_SoftKeyBar.OnViewSoftKeyChanged(pView);
    }
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// public interface
void resetSoftkeyPageIndex()
{
    g_SoftKeyBar.ResetPageIndex();
}

xView& currentSoftKeyBar()
{
    return xSoftKeyBar::getSoftKeyBar();
}

void setupViewSoftKey(xView* pView, int nIndex, const yl::string& strTitle, int nType,
                      xSlotHandler* receiver, slotMethod slot,
                      xSlotHandler* receiverLongPress,
                      slotMethod slotLongPressed)
{
    xSoftKeys* pSoftkeys = xSoftKeys::GainViewSoftKeys(pView);

    if (NULL == pSoftkeys)
    {
        return;
    }

    typeHotKeyFrame hotKey = pSoftkeys->keyAt(nIndex);
    xSlotHotKey* pSlotHotKey = xSlotHotKey::GetSlotHotKey(hotKey.Get());
    bool bAdd = false;

    if (NULL == pSlotHotKey)
    {
        pSlotHotKey = new xSlotHotKey;
        bAdd = true;

        if (NULL == pSlotHotKey)
        {
            return;
        }
    }

    pSlotHotKey->SetType(nType);
    pSlotHotKey->SetIndex(nIndex);
    pSlotHotKey->SetTitle(strTitle);
    pSlotHotKey->SetupPressSlot(receiver, slot);
    pSlotHotKey->SetupLongPressSlot(receiverLongPress, slotLongPressed);

    if (bAdd)
    {
        pSoftkeys->AddHotKey(pSlotHotKey);
    }

    g_SoftKeyBar.OnViewSoftKeyChanged(pView);
}

bool setupViewSoftKey(xView* pView, xSlotHotKey* pHotKey)
{
    if (NULL == pView || NULL == pHotKey)
    {
        return false;
    }

    xSoftKeys* pSoftkeys = xSoftKeys::GainViewSoftKeys(pView);

    if (NULL != pSoftkeys && pSoftkeys->AddHotKey(pHotKey))
    {
        g_SoftKeyBar.OnViewSoftKeyChanged(pView);

        return true;
    }

    return false;
}

void setSoftKeyBarPhoneKey(int nPhoneKey)
{
    g_SoftKeyBar.setFirstBtnKey(nPhoneKey);
}

void removeViewSoftKey(xView* pView, int nIndex)
{
    xSoftKeys* pSoftKeys = xSoftKeys::GetViewSoftKeys(pView);

    if (NULL != pSoftKeys && pSoftKeys->removeKey(nIndex))
    {
        g_SoftKeyBar.OnViewSoftKeyChanged(pView);
    }
}

void clearViewSoftKey(xView* pView)
{
    xSoftKeys* pSoftKeys = xSoftKeys::GetViewSoftKeys(pView);

    if (NULL != pSoftKeys && pSoftKeys->ClearKeys())
    {
        g_SoftKeyBar.OnViewSoftKeyChanged(pView);
    }
}
