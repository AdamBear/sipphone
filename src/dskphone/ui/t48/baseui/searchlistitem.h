
#ifndef __SEARCH_LIST_ITEM_H__
#define __SEARCH_LIST_ITEM_H__

#include "detaillistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_SEARCH_ITEM = 0x00040000,
};

class CSearchListItem : public CDetailListItem
{
    Q_OBJECT

public:
    CSearchListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CSearchListItem(void);

    static int GetSearchListItemType();

public:
    // 设置头像
    void SetHeadPortrait(const QString & strPic);

    // 设置头像的显示区域
    void SetHeadPortraitRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetHeadPortraitRect(const QRect & rcValue);

    // 设置状态图标
    void SetStatePicutre(const QString & strPic);

    // 设置图标的显示区域
    void SetStatePicRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetStatePicRect(const QRect & rcValue);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintHeadPortrait(QStylePainter & stylePainter);

    void PaintStatePicutre(QStylePainter & stylePainter);

protected:
    // 头像的大小
    QRect m_rcHeadPortrait;
    // 头像的路径
    QString m_strHeadPortraitPic;

    // 状态图标的大小
    QRect m_rcStatePic;
    // 状态图标的路径
    QString m_strStatusPic;
};

typedef CSearchListItem* CSearchListItemPtr;

#endif //__SEARCH_LIST_ITEM_H__
