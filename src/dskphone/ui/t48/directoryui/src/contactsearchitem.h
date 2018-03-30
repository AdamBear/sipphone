
#ifndef __CONTACT_SEARCH_ITEM_H__
#define __CONTACT_SEARCH_ITEM_H__

#include "uikernel/listitem.h"
#include "inputmethod/inputmethod.h"

enum
{
    LIST_ITEM_TYPE_CONTACT_SEARCH           = 0x06000000,
};

class QLineEdit;
class CContactSearchItem : public CListItem
{
    Q_OBJECT

public:
    CContactSearchItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CContactSearchItem(void);

    static CContactSearchItem* GetContactSearchItem(CListItem* pItem);
public:
    void SetSearchType(int nSearchType);
    void SetTitle(const QString & strTitle);
    void SetKey(const QString & strKey);
    void SetSearch(bool bSearch);
    // 设置Search按钮是否可见
    void SetBtnSearch(bool bVisible);

    int GetSearchType() const;
    QString GetKey() const;

    QLineEdit * GetEdit() const;

    // 设置选中状态
    virtual void SetFocus(bool bFocus);

    void DelayShowKeyboard();
    void InstallKeyboard(INPUT_DIALOG_STYLE eStyle);

Q_SIGNALS:
    void editFocus(bool bFocus);

public:
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    void InitSearchEdit();
    void ReleaseSearchEdit();

private:
    int m_nSearchType;

    bool m_bDelayShowKeyboard;

    QLineEdit * m_pEdit;
    QString m_strTitle;
    // 是否有Search按钮
    bool m_bSearch;
    // Detail按钮
    CButtonWrapper m_btnSearch;
};
typedef CContactSearchItem * CContactSearchItemPtr;

#endif
