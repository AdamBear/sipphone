#ifndef __X_VALUE_HANDLE_H__
#define __X_VALUE_HANDLE_H__

#include <yllist.h>
#include <ylstring.h>
#include "xwindow-kernel/include/xsignal.h"
#include "xwindow-kernel/include/xmessagehandler.h"
#include "xViewHandler.h"

class xView;
class xTextView;

//////////////////////////////////////////////////////////////////////////
typedef struct
{
    yl::string strValue;
    yl::string strPresent;
} ST_VALUE_PRESENT;

typedef YLList<ST_VALUE_PRESENT>                    LIST_VALUE_PRESENT;
typedef YLList<xView*>                              LIST_VIEW;

class XWINDOW_EXPORT xPresentGroup
{
public:
    bool operator == (const xPresentGroup&)
    {
        return false;
    }

    bool GetPresentByValue(const yl::string& strValue, yl::string& strPresent);
    bool GetValueByPresent(const yl::string& strPresent, yl::string& strValue);
    bool GetValuePresentByIndex(int nIndex, yl::string& strValue, yl::string& strPresent);
    int GetValueIndex(const yl::string& strValue);
    void DeleteValue(const yl::string& strValue);

public:
    yl::string                  m_strGroupName;
    LIST_VALUE_PRESENT          m_listValuePresent;
    LIST_VIEW                   m_listPresentViews;
};
typedef YLList<xPresentGroup>                               typePresentGroupList;

class XWINDOW_EXPORT xValueHandler : public xMessageHandler, public xViewHandler
{
    friend class xWorkStation;
public:
    virtual const yl::string& getValue() const;
    virtual bool setValue(const yl::string& value);

public:
    virtual void loadChildrenElements(xml_node& node);
    virtual void loadAttributes(xml_node& node);
protected:
    void asyncUpdateValueToViews();
    BOOL onUpdateValueToViews(msgObject& objMessage);
protected:
    void fetchPresentViewsToGroup(xView* pView, xPresentGroup& objGroup);
    void fetchPresentViews();
public:
    explicit xValueHandler(int nType = VIEW_HANDLE_TYPE_VALUE);

    static xValueHandler* GetHandle(xViewHandler* pHandle);

public:
    yl::string                  m_strValue;
    xSignal                 signalValueChanged;
    bool                        m_bAsyncUpdate;
    typePresentGroupList        m_listPresentGroup;
};

enum
{
    VALUE_HANDLE_TYPE_MASK              = 0xFFFF0000,
    VALUE_HANDLE_TYPE_SUB_MASK          = 0x00FF0000,
    VALUE_HANDLE_TYPE_CHECK_BOX         = VIEW_HANDLE_TYPE_VALUE | 0x00010000,
    VALUE_HANDLE_TYPE_SLIDER            = VIEW_HANDLE_TYPE_VALUE | 0x00020000,
    VALUE_HANDLE_TYPE_COMBO_BOX_PICKUP  = VIEW_HANDLE_TYPE_VALUE | 0x00030000,
    VALUE_HANDLE_TYPE_COMBO_BOX         = VIEW_HANDLE_TYPE_VALUE | 0x00040000,
    VALUE_HANDLE_TYPE_EDIT_BOX          = VIEW_HANDLE_TYPE_VALUE | 0x00050000,
};

class xComboBox;
class XWINDOW_EXPORT xComboBoxHandler : public xValueHandler
{
    friend class xComboBox;

public:
    explicit xComboBoxHandler(int nType = VALUE_HANDLE_TYPE_COMBO_BOX);
    virtual ~xComboBoxHandler();

    static xComboBoxHandler* GetHandle(xViewHandler* pHandle);

public:
    bool SwitchValue(xComboBox* pComboBox, bool bNext);

protected:
    bool SwitchToNextValue(LIST_VALUE_PRESENT& listValuePresent);
    bool SwitchToPreValue(LIST_VALUE_PRESENT& listValuePresent);
};

#endif // __X_VALUE_HANDLE_H__
