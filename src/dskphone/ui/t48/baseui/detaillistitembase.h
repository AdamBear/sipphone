
#ifndef __DETAIL_LIST_ITEM_BASE_H__
#define __DETAIL_LIST_ITEM_BASE_H__

#include <ylvector.h>
#include "uikernel/listitem.h"

class QString;
class QStylePainter;

enum
{
    LIST_ITEM_TYPE_DETAIL           = 0x05000000,
    LIST_ITEM_TYPE_DETAIL_MARK      = 0x00FF0000,
};

class CDetailListItemBase : public CListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();
#endif

public:
    explicit CDetailListItemBase(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CDetailListItemBase(void);

    static int GetDetailListItemBaseType();

public:
    // 设置高度
    void SetItemHeight(int nHeight);

    // 设置标题区域
    void SetTitleRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetTitleRect(const QRect & rcTitle);

    // 设置标题
    void SetTitle(const char * pszTitle, bool bAddColon = true);
    void SetTitle(const QString & strTitle, bool bAddColon = true);

    // 设置Detail文字
    void SetValue(const char * pszValue);
    virtual void SetValue(const QString & strValue);

    // 获取Detail文字
    virtual QString GetValue() const;

    int GetDetailSubType() const;

    int GetDefaultTitleLeft() const;
    int GetDefaultTitleWidth() const;

    void SetDefaultTitlePos();

    void SetTitleColor(QColor color);

protected:
    // 画标题
    void PaintTitle(QStylePainter & stylePainter);

protected:
    // 标题
    QRect m_rcTitle;
    // 标题名称
    QString m_strTitle;

    // 字体颜色
    QColor m_colTitle;
};
typedef CDetailListItemBase * CDetailListItemBasePtr;
typedef YLVector<CDetailListItemBase *> VEC_DETAIL_LIST_ITEM;

#endif
