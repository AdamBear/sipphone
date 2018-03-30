
#ifndef __DETAIL_LIST_ITEM_H__
#define __DETAIL_LIST_ITEM_H__

#include "detaillistitembase.h"

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_ITEM      = 0x00010000,
};

class CDetailListItem : public CDetailListItemBase
{
public:
    explicit CDetailListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CDetailListItem(void);

    static int GetDetailListItemType();

public:
    // 设置Detail文字
    virtual void SetValue(const QString & strValue);

    // 获取Detail文字
    virtual QString GetValue() const;

    // 设置Detail的起始位置
    void SetValuePos(int nLeft);

    // 设置Detail的位置
    void SetValueRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetValueRect(const QRect & rcValue);

    // 设置字体大小
    void SetValueFontSize(int nSize);

    void SetValueColor(QColor color);

#ifdef IF_ENABLE_TESTABILITY
    virtual QString GetTestInfo();
#endif
protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintValue(QStylePainter & stylePainter);

protected:
    // 列表项当前值的大小
    QRect m_rcValue;

    // 列表项当前值
    QString m_strValue;

    // 字体大小
    int m_nFontSize;

    // 字体颜色
    QColor m_colValue;
};
typedef CDetailListItem * CDetailListItemPtr;

#endif
