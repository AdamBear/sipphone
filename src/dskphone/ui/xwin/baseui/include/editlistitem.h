#ifndef __EDIT_LIST_ITEM_H__
#define __EDIT_LIST_ITEM_H__

#include "detaillistitembase.h"
#include <imeedit/imeeditdefine.h>

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_EDIT      = 0x00020000,
};


class xEditTextView;

class CEditListItem : public CDetailListItemBase
{
public:
    explicit CEditListItem(IListViewItemAction * pAction = NULL);
    virtual ~CEditListItem();

    static int GetEditListItemType();
    static CEditListItem * GetEditListItem(xListViewItem * pItem);
    static xListViewItem * CreateEditListItem();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual void Release();

public:
    virtual void SetValue(const yl::string & strValue);

    virtual yl::string GetValue() const;
    virtual bool onFocusEvent(bool bFocusIn);

    void EndIME();

    void InstallIME(const char * pszSupportIME, const char * pszIME);
    void UnInstallIME();

    void ConnectIMEChanged(xSlotHandler * pHandler, slotMethod pMethod);
    void DisconnectIMEChanged(xSlotHandler * pHandler, slotMethod pMethod);

    void SetPassword(bool bPassword);
    void SetSuddenPassword(bool bSuddenPassword);
    void SetDeleteEnable(bool bEnable, int nSoftkeyIndex = -1);
    void SetMaxLength(int nMaxLength);
    int GetMaxLength();

    void ConnectValueChanged(xSlotHandler* pHandler, slotMethod pMethod);
    void DisconnectValueChanged(xSlotHandler* pHandler, slotMethod pMethod);

    // 返回所有的输入
    void ReturnAllInput();
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    // 设置内容为空时提示信息
    void SetHintText(const yl::string & strHint);
#endif
    //设置是否可编辑
    void SetEditEnable(bool bEnable);

    xEditTextView * GetEditText();

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
    virtual bool IsShowFocusBG();
#endif

protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
#endif

protected:
    xEditTextView   *   m_pEdit;
};

typedef CEditListItem * CEditListItemPtr;

#endif
