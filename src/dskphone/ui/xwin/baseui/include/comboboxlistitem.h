#ifndef __COMBOBOX_LIST_ITEM_H__
#define __COMBOBOX_LIST_ITEM_H__

#include "detaillistitembase.h"

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX      = 0x00030000,
};

class xComboBox;
class xComboBoxHandler;

class CComboBoxListItem : public CDetailListItemBase, public xSlotHandler
{
public:
    explicit CComboBoxListItem(IListViewItemAction * pAction = NULL);
    virtual ~CComboBoxListItem();

    static int GetComboBoxListItemType();
    static CComboBoxListItem * GetComboBoxListItem(xListViewItem * pItem);
    static xListViewItem * CreateComboBoxListItem();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual void Release();

    virtual void SetItemFocus(bool bFocus);

    virtual bool onFocusEvent(bool bFocusIn);

public:
    virtual yl::string GetValue() const;

    xComboBox * GetComboBox()
    {
        return m_pComboBox;
    }
    // 添加选项
    void AddValue(const yl::string & strValue, int nData);
    void AddValue(const yl::string & strValue, const yl::string & strData);

    void ClearValue();

    // 选中itemData对应的ComboBox选项
    void SetItemData(int nData, int nDefaultIndex = 0);
    void SetItemData(const yl::string & strData, int nDefaultIndex = 0);

    // 选中相应的项
    void SetCurrentIndex(int nIndex);

    int GetItemData() const;
    yl::string GetItemStringData() const;

    void SwitchNextValue();
    void SwitchPreValue();

    void OnSoftKeyPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    void SetupValueChangedSlot(xSlotHandler * pHandler, slotMethod pMethod);
    void RemoveValueChangedSlot(xSlotHandler * pHandler, slotMethod pMethod);

    void SetComboBoxStyle(yl::string strStyle);

    void SetSwitchSoftKey(bool bSwitch);

    void SetComboBoxDisable(bool bDisable);

    void SetIsSetupSoftkeyByItem(bool bSetupSoftkeyByItem);

#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
    virtual bool IsShowFocusBG();
#endif

protected:
    virtual void SetupSoftkey();
    void RemoteSoftkey();

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
#endif

protected:
    xComboBox * m_pComboBox;
    bool m_bSwitchSoftKey;
    bool m_bSetupSoftkeyByItem;
};
typedef CComboBoxListItem * CComboBoxListItemPtr;

#endif
