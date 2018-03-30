
#ifndef __CONTACT_LIST_ITEM_H__
#define __CONTACT_LIST_ITEM_H__

#include "uikernel/buttonwrapper.h"
#include "uikernel/ilistitemaction.h"
#include "baseui/simpleinfolistitem.h"



enum
{
    LIST_ITEM_TYPE_CONTACT_BASE     = 0x00010000,
};

class QPoint;
class QMouseEvent;
class QStylePainter;
class CContactBaseItem : public CSimpleInfoListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();

    QRect m_rcBtnDetail;
    QRect m_rcCheckBox;
#endif
public:
    explicit CContactBaseItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CContactBaseItem(void);

    static int GetContactBaseItemType();

// CListItem
public:
    // 清除内容
    virtual void ClearContent();

public:
    // 设置可否编辑
    void SetEdit(bool bEdit);

    // 设置是否选中
    void SetChecked(bool bChecked, bool bRefresh = false);

    // 设置Id
    void SetId(int nId);

    // 设置Detail是否可见
    void SetDetail(bool bVisible);

    // 设置是否有联系人头像
    void SetContactPicVisible(bool bVisible);

    // 是否可编辑
    bool IsEdit() const;

    // 是否选中
    bool IsChecked() const;

    // 是否有Detail按钮
    bool IsDetail() const;

    // 获得数据Id
    int GetId() const;



protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);
    virtual void paintEvent(QPaintEvent * pEvent);

    virtual void Relayout();

    // 画默认联系人
    void PaintContact(QStylePainter & stylePainter, QPoint & ptContact);

    // 画可选框
    void PaintCheckBox(QStylePainter & stylePainter, QPoint & ptBegin);

    // 画Detail
    void PaintDetailButton(QStylePainter & stylePainter, int nRight);

protected:
    // 是否可以编辑
    bool m_bEditing;
    // 编辑时是否选中
    bool m_bChecked;
    // 是否有Detail按钮
    bool m_bDetail;
    // Detail按钮
    CButtonWrapper m_btnDetail;

    // ID
    int m_nId;


};
typedef CContactBaseItem * CContactBaseItemPtr;

#endif
