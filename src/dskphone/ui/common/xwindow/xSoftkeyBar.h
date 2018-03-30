#ifndef __X_SOFTKEY_BAR_H__
#define __X_SOFTKEY_BAR_H__

#include <ylstring.h>
#include "xWindow.h"

XWINDOW_EXPORT xView& currentSoftKeyBar();

//////////////////////////////////////////////////////////////////////////////////////////////
// xHotKeyObject
enum
{
    HOT_KEY_OBJECT_TYPE_NONE        = 0,
    HOT_KEY_OBJECT_TYPE_SLOT        = 0x01000000,
#if IF_VIEW_USER_ACTION
    HOT_KEY_OBJECT_TYPE_ACTION      = 0x02000000,
#endif
    HOT_KEY_OBJECT_TYPE_MASK        = 0xff000000,
};

class xHotKeyObject
{
public:
    explicit xHotKeyObject(int nObjectType = HOT_KEY_OBJECT_TYPE_NONE);
    virtual ~xHotKeyObject();

public:
    virtual void emitHotKey(bool bLongPressed)          = 0;
    virtual bool isEnableLongPress()                    = 0;
    virtual void UpdateHotKeyBtn(xButton* pBtn);

    void SetTitle(const yl::string& strTitle)
    {
        m_strTitle = strTitle;
    }
    void SetType(int nType)
    {
        m_nType = nType;
    }
    void SetIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }

    const yl::string& GetTitle() const
    {
        return m_strTitle;
    }
    int GetObjectType() const
    {
        return m_nObjectType;
    }
    int GetType() const
    {
        return m_nType;
    }
    int GetIndex() const
    {
        return m_nIndex;
    }

protected:
    yl::string          m_strTitle;
    int                 m_nObjectType;
    int                 m_nType;        // softkey业务类型
    int                 m_nIndex;       // softkey按键索引值
};

//////////////////////////////////////////////////////////////////////////////////////////////
// xSlotHotKey
class xSlotHotKey : public xHotKeyObject
{
public:
    xSlotHotKey();
    virtual ~xSlotHotKey();

    static xSlotHotKey* GetSlotHotKey(xHotKeyObject* pHotKey);

public:
    virtual void emitHotKey(bool bLongPressed);
    virtual bool isEnableLongPress();

    void SetupPressSlot(xSlotHandler* pReceiver, slotMethod slot);
    void SetupLongPressSlot(xSlotHandler* pReceiver, slotMethod slot);

protected:
    xSlotHandler*       m_pReceiver;
    slotMethod          m_slot;
    xSlotHandler*       m_pLongPressReceiver;
    slotMethod          m_slotLongPress;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// xSlotHotKey
#if IF_VIEW_USER_ACTION
class xActionHotKey : public xHotKeyObject
{
public:
    xActionHotKey();
    virtual ~xActionHotKey();

    static xActionHotKey* GetActionHotKey(xHotKeyObject* pHotKey);

public:
    virtual void emitHotKey(bool bLongPressed);
    virtual bool isEnableLongPress();

    void SetAction(const yl::string& strAction)
    {
        m_strAction = strAction;
    }
    void SetLongPressAction(const yl::string& strAction)
    {
        m_strActionLongPressed = strAction;
    }

    const yl::string& GetAction() const
    {
        return m_strAction;
    }
    const yl::string& GetLongPressAction() const
    {
        return m_strActionLongPressed;
    }

protected:
    yl::string          m_strAction;
    yl::string          m_strActionLongPressed;
};
#endif

XWINDOW_EXPORT void setupViewSoftKey(xView* pView, int nIndex, const yl::string& strTitle, int nType,
                                     xSlotHandler* receiver, slotMethod slot,
                                     xSlotHandler* receiverLongPress = NULL,
                                     slotMethod slotLongPress = NULL_METHOD);
XWINDOW_EXPORT bool setupViewSoftKey(xView* pView, xSlotHotKey* pHotKey);
XWINDOW_EXPORT void setSoftKeyBarPhoneKey(int nPhoneKey);
XWINDOW_EXPORT void removeViewSoftKey(xView* pView, int nIndex);
XWINDOW_EXPORT void clearViewSoftKey(xView* pView);
XWINDOW_EXPORT void resetSoftkeyPageIndex();

#endif // __X_SOFTKEY_BAR_H__
