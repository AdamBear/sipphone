
#ifndef __COMBOBOX_LIST_ITEM_H__
#define __COMBOBOX_LIST_ITEM_H__

#include <ylstring.h>
#include "detaillistitembase.h"
#include "uikernel/qcomboboxex.h"

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_COMBOBOX      = 0x00030000,
};

class CComboBoxListItem : public CDetailListItemBase
{
    Q_OBJECT

public:
    explicit CComboBoxListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CComboBoxListItem(void);

    static int GetComboBoxListItemType();
    static CComboBoxListItem* GetComboBoxListItem(CDetailListItemBase* pItem);
public:
    // 设置ComboBox区域
    void SetComboBoxRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetComboBoxRect(const QRect & rcComboBox);

    virtual void SetFocus(bool bFocus);

    void ClearValue();

    // 添加选项
    void AddValue(const char * pszValue, const QVariant & auserData);
    void AddValue(const QString & strValue, const QVariant & auserData);

    // 设置ComboBox文字
    virtual void SetValue(const QString & strValue);

    // 选中itemData对应的ComboBox选项
    void SetItemData(const yl::string & strData, int nDefaultIndex = 0);
    void SetItemData(int nData, int nDefaultIndex = 0);

    bool IsHadItemData(const yl::string & strData);

    // 选中相应的项
    void SetCurrentIndex(int nIndex);

    // 获取ComboBox文字
    virtual QString GetValue() const;

    qComboBoxEx * GetComboBox() const;

    yl::string GetItemStringData() const;
    int GetItemData() const;

    void SetTitleWidth(int nTitleWidth);

    void InstallComboBoxFilter(QObject * pObject);

    void SetLayout(bool bFrameLayout);

    bool IsOnlyShowText() const;

    void CheckOnlyShowText(bool bOnlyText);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void paintEvent(QPaintEvent * pEvent);

    virtual void Relayout();
    void LayoutByFrameList();

protected:
    qComboBoxEx * m_pComboBox;
    bool m_bFrameLayout;
    bool m_bOnlyShowText;
    yl::string m_strBg;
};
typedef CComboBoxListItem * CComboBoxListItemPtr;

#endif
