#include <QtGui>
#include "treeitem.h"
#include "uikernel/ilistitemdelegate.h"
#include "imagemanager/modimagemanager.h"

namespace
{
#define DEFAULT_LEVEL_CONTENT_SPACE 0
#define DEFAULT_TREE_ARROW_LEFT 5
#define DEFAULT_TREE_ARROW_TOP 15
#define DEFAULT_TREE_ARROW_WIDTH 16
#define DEFAULT_TREE_ARROW_HEIGHT 16
}

CTreeItem::CTreeItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_TREE, parent, pAction)
    , m_bHadChildren(false)
    , m_bExpand(false)
    , m_ucLevel(TREE_ITEM_LEVEL_NONE)
    , m_nId(0)
    , m_nContextLeft(0)
{
    setObjectName("CTreeItem");
}

CTreeItem::~CTreeItem()
{

}

void CTreeItem::SetDelegate(IListItemDelegate * pDelegate)
{
    CListItem::SetDelegate(pDelegate);

    SetLevel(m_ucLevel);
}

void CTreeItem::ClearContact()
{
    m_bHadChildren = false;
    m_bExpand = false;
    m_ucLevel = TREE_ITEM_LEVEL_NONE;
    m_nContextLeft = 0;
    m_nId = 0;

    CListItem::ClearContent();
}

void CTreeItem::SetHadChildren(bool bHadChildren)
{
    m_bHadChildren = bHadChildren;
}

void CTreeItem::SetExpand(bool bExpand)
{
    m_bExpand = bExpand;
}

void CTreeItem::SetLevel(unsigned char ucLevel)
{
    m_ucLevel = ucLevel;

    m_nContextLeft = DEFAULT_LEVEL_CONTENT_SPACE * (ucLevel - (unsigned char)(TREE_ITEM_LEVEL_FIRST));

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        m_nContextLeft += nLeftMargin;
    }
}

void CTreeItem::SetId(int nId)
{
    m_nId = nId;
}

bool CTreeItem::IsHadChildren() const
{
    return m_bHadChildren;
}

bool CTreeItem::IsExpand() const
{
    return m_bExpand;
}

unsigned char CTreeItem::GetLevel() const
{
    return m_ucLevel;
}

int CTreeItem::GetId() const
{
    return m_nId;
}

void CTreeItem::PaintExpand(QStylePainter & stylePainter, QPoint & ptBegin, const char * pszExand,
                            const char * pszCurl)
{
    int nLeft = ptBegin.x() + DEFAULT_TREE_ARROW_LEFT;

    if (!m_bHadChildren)
    {
        ptBegin.setX(nLeft + DEFAULT_TREE_ARROW_WIDTH);
        return;
    }

    QRect rcIcon(nLeft, ptBegin.y() + DEFAULT_TREE_ARROW_TOP, DEFAULT_TREE_ARROW_WIDTH,
                 DEFAULT_TREE_ARROW_HEIGHT);

    if (m_bExpand)
    {
        if (NULL != pszExand)
        {
            QPixmap pm = THEME_GET_BMP(pszExand);

            if (!pm.isNull())
            {
                stylePainter.drawPixmap(rcIcon, pm);
            }
        }
    }
    else if (NULL != pszCurl)
    {
        QPixmap pm = THEME_GET_BMP(pszCurl);

        if (!pm.isNull())
        {
            stylePainter.drawPixmap(rcIcon, pm);
        }
    }

    ptBegin.setX(nLeft + DEFAULT_TREE_ARROW_WIDTH);
}

void CTreeItem::PaintRawIcon(QStylePainter & stylePainter, QPoint & ptBegin, const char * pszIcon)
{
    int nLeft = ptBegin.x() + DEFAULT_TREE_ARROW_LEFT;

    QRect rcIcon(nLeft, ptBegin.y() + DEFAULT_TREE_ARROW_TOP, DEFAULT_TREE_ARROW_WIDTH,
                 DEFAULT_TREE_ARROW_HEIGHT);

    if (NULL != pszIcon)
    {
        QPixmap pm = THEME_GET_BMP(pszIcon);

        if (!pm.isNull())
        {
            stylePainter.drawPixmap(rcIcon, pm);
        }
    }

    ptBegin.setX(nLeft + DEFAULT_TREE_ARROW_WIDTH);
}
