#ifndef __X_COMBOBOX_H__
#define __X_COMBOBOX_H__

#include "xValueHandler.h"
#include "xwindow/xWindow.h"


#if IF_COMBOBOX_PICKUP_ENABLE
class xComboBox;
class xComboBoxPickupView
{
public:
    xComboBoxPickupView() {} ;
    virtual ~xComboBoxPickupView() {};

public:
    virtual void OnComboBoxDisappear(xComboBox* pComboBox)          = 0;
};
#endif

class XWINDOW_EXPORT xComboBox : public xTextView
{
    friend class xComboBoxHandler;

public:
    xComboBox();
    virtual ~xComboBox();

    static void SetGlobalStyle(const yl::string& strStyle);
    bool xComboBox::onKeyPressedEvent(int nKeyCode);

public:
    virtual void loadChildrenElements(xml_node& node);

    virtual void loadAttributes(xml_node& node);

    virtual void showValue(const yl::string& strValue);

    xComboBoxHandler* GetComboBoxHandle() const;

    bool AddItem(const yl::string& strText, const yl::string& strValue);
    void Clear();

    void SetValue(const yl::string& strValue);

    void SetCurrentIndex(int nIndex);

    int GetItemCount() const;
    yl::string GetValue() const;

#if IF_COMBOBOX_PICKUP_ENABLE

    void SetPickupTitle(const yl::string& strTitle);
    yl::string GetPickupTitle() const;

    void SetPickupView(xComboBoxPickupView* pPickupView);
    xComboBoxPickupView* GetPickupView() const;

    // 显示或隐藏信号槽
    void OnAppearSlot(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    // 关闭ComboBox信号槽
    void OnCloseSlot(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    void OnDisappear();
#endif

#if IF_GESTURE_ENABLE
    bool onGestureEvent(xGestureEvent* pEvent);
#endif

protected:
    static yl::string s_strGlobalStyle;

#if IF_COMBOBOX_PICKUP_ENABLE
    xComboBoxPickupView* m_pPickupView;

    yl::string m_strPickupTitle;
#endif
};

#endif
