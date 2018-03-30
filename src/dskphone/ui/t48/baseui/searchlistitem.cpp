#include <QtGui>
#include "searchlistitem.h"
#include "imagemanager/modimagemanager.h"

CSearchListItem::CSearchListItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CDetailListItem(parent, pAction)
{
    setObjectName("CSearchListItem");

    m_nType |= LIST_ITEM_SUB_TYPE_SEARCH_ITEM;
}

CSearchListItem::~CSearchListItem(void)
{
}

int CSearchListItem::GetSearchListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_SEARCH_ITEM);
}

void CSearchListItem::SetHeadPortrait(const QString & strPic)
{
    m_strHeadPortraitPic = strPic;

    if (!pathFileExist(m_strHeadPortraitPic.toUtf8().data()))
    {
        m_strHeadPortraitPic = "";
    }
}

void CSearchListItem::SetHeadPortraitRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_rcHeadPortrait = QRect(nLeft, nTop, nWidth, nHeight);
}

void CSearchListItem::SetHeadPortraitRect(const QRect & rcValue)
{
    m_rcHeadPortrait = rcValue;
}

void CSearchListItem::SetStatePicutre(const QString & strPic)
{
    m_strStatusPic = strPic;
}

void CSearchListItem::SetStatePicRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_rcStatePic = QRect(nLeft, nTop, nWidth, nHeight);
}

void CSearchListItem::SetStatePicRect(const QRect & rcValue)
{
    m_rcStatePic = rcValue;
}

void CSearchListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintDownBg(stylePainter);

    PaintHeadPortrait(stylePainter);

    PaintStatePicutre(stylePainter);

    PaintTitle(stylePainter);

    PaintValue(stylePainter);
}

void CSearchListItem::PaintHeadPortrait(QStylePainter & stylePainter)
{
    QPixmap pm;
    if (!m_strHeadPortraitPic.isEmpty())
    {
        pm = STOCK_GET_BMP(m_strHeadPortraitPic.toUtf8().data());
    }
    else
    {
        pm = THEME_GET_BMP(PIC_CONTACTS_IMAGE_DEFAULT);
    }

    stylePainter.drawPixmap(m_rcHeadPortrait, pm.scaled(m_rcHeadPortrait.width(), m_rcHeadPortrait.height()));
}

void CSearchListItem::PaintStatePicutre(QStylePainter & stylePainter)
{
    if (m_strStatusPic.isEmpty())
    {
        return ;
    }

    QPixmap pm = THEME_GET_BMP(m_strStatusPic.toUtf8().data());
    stylePainter.drawPixmap(m_rcStatePic, pm);
}
