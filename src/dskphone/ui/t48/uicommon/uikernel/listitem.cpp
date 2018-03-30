#include <QtGui>
#include "ilistitemdelegate.h"
#include "listitem.h"
#include "imagemanager/modimagemanager.h"
#include "framelistbase.h"


namespace
{
#define LIST_ITEM_FOCUS_BG_LEFT 11
#define LIST_ITEM_FOCUS_BG_TOP 0
#define LIST_ITEM_FOCUS_BG_WIDTH 453
#define LIST_ITEM_FOCUS_BG_HEIGHT 38
}


CListItem::CListItem(int nType, QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : QWidget(parent)
    , m_nType(nType)
    , m_pDelegate(NULL)
    , m_pAction(pAction)
    , m_nDataIndex(INVALID_ITEM_INDEX)
    , m_bFocus(false)
    , m_bDown(false)
{
    setObjectName("CListItem");
}

CListItem::~CListItem()
{

}

void CListItem::SetDelegate(IListItemDelegate * pDelegate)
{
    m_pDelegate = pDelegate;
}

void CListItem::SetAction(IListItemAction * pAction)
{
    m_pAction = pAction;
}

void CListItem::Release()
{
    SetAction(NULL);
    setParent(NULL);
    hide();
    deleteLater();
}

void CListItem::ClearContent()
{
    int nDataIndex = m_nDataIndex;
    bool bFocus = m_bFocus;

    m_nDataIndex = INVALID_ITEM_INDEX;
    m_bFocus = false;
    m_bDown = false;

    if (NULL != m_pDelegate)
    {
        if (nDataIndex != m_nDataIndex)
        {
            m_pDelegate->OnDataIndexChange(this);
        }

        if (bFocus != m_bFocus)
        {
            m_pDelegate->OnItemFocusChange(this, bFocus);
        }
    }

    RefreshUI();
}

int CListItem::GetType() const
{
    return m_nType;
}

int CListItem::GetListType() const
{
    return (m_nType & LIST_ITEM_TYPE_MARK);
}

int CListItem::GetDataIndex() const
{
    return m_nDataIndex;
}

void CListItem::SetDataIndex(int nIndex)
{
    m_nDataIndex = nIndex;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->OnDataIndexChange(this);
    }
}

bool CListItem::IsFocus() const
{
    return m_bFocus;
}

void CListItem::SelectItem(bool bSelect)
{
    FocusItem(bSelect, LIST_ITEM_ACTION_SELECT);
}

void CListItem::FocusItem(bool bFocus, int nAction/* = LIST_ITEM_ACTION_FOCUS*/)
{
    bool bPreFocus = m_bFocus;

    SetFocus(bFocus);

    if (bPreFocus != m_bFocus)
    {
        RefreshUI();
    }

    if (m_bFocus)
    {
        if (NULL != m_pDelegate)
        {
            m_pDelegate->OnItemFocusChange(this, bPreFocus);
        }

        DoAction(nAction);
    }
}

void CListItem::SetFocus(bool bFocus)
{
    m_bFocus = bFocus;
}

bool CListItem::IsDown() const
{
    return m_bDown;
}

void CListItem::mousePressEvent(QMouseEvent * pEvent)
{
    ProcessMousePress(pEvent);
}

bool CListItem::ProcessMousePress(QMouseEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    QRect rcItem = rect();
    const QPoint & pt = pEvent->pos();

    if (rcItem.contains(pt))
    {
        if (!m_bDown)
        {
            m_bDown = true;

            RefreshUI();
        }
    }
    else if (m_bDown)
    {
        m_bDown = false;

        RefreshUI();
    }

    return m_bDown;
}

void CListItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    ProcessMouseRelease(pEvent);
}

bool CListItem::ProcessMouseRelease(QMouseEvent * pEvent, bool bReSelect/* = false*/)
{
    if (NULL == pEvent)
    {
        return false;
    }

    bool bDown = m_bDown;

    m_bDown = false;

    QRect rcItem = rect();
    const QPoint & pt = pEvent->pos();

    if (bDown && rcItem.contains(pt))
    {
        if (bReSelect && m_bFocus)
        {
            m_bFocus = false;
        }

        SelectItem(true);
        return true;
    }

    if (bDown)
    {
        RefreshUI();
    }

    return false;
}

void CListItem::moveEvent(QMoveEvent * pEvent)
{
    Relayout();
}

void CListItem::resizeEvent(QResizeEvent * pEvent)
{
    Relayout();
}

void CListItem::Relayout()
{

}

void CListItem::RefreshUI()
{
    if (!isVisible())
    {
        return;
    }

    QWidget * pParent = (QWidget *)parent();

    if (NULL != pParent)
    {
        pParent->update();
    }
    else
    {
        update();
    }
}

void CListItem::DoAction(int nAction)
{
    if (NULL == m_pAction || !isVisible())
    {
        return;
    }

    CFrameListBase * pFrameList = dynamic_cast<CFrameListBase *>(parent());

    if (NULL == pFrameList)
    {
        return;
    }

    m_pAction->OnItemAction(pFrameList, this, nAction);
}

void CListItem::PaintDownBg(QStylePainter & stylePainter, const char * pszDownBg/* = NULL*/,
                            const QRect & rcBg/* = QRect(0,0,0,0)*/)
{
    // 默认图片改成由FrameList实现(设计有改的话，再换回来)
    if (!m_bDown || NULL == pszDownBg)
    {
        return;
    }

    if (NULL == pszDownBg)
    {
        if (NULL == m_pDelegate)
        {
            return;
        }

        pszDownBg = m_pDelegate->GetClickBg(GetDataIndex());

        if (NULL == pszDownBg || 0 == strcmp(pszDownBg, ""))
        {
            return;
        }
    }

    QRect rc;

    if (rcBg.isEmpty())
    {
        rc = rect();
    }
    else
    {
        rc = rcBg;
    }

    QPixmap pmBg = THEME_GET_BMP(pszDownBg);

    stylePainter.drawPixmap(rc, pmBg);
}

void CListItem::PaintFocusBg(QStylePainter & stylePainter, const char * pszFocusBg/* = NULL*/,
                             const QRect & rcBg/* = QRect(0,0,0,0)*/)
{
    if (!m_bFocus)
    {
        return;
    }

    if (NULL == pszFocusBg)
    {
        if (NULL == m_pDelegate)
        {
            return;
        }

        pszFocusBg = m_pDelegate->GetFocusBg(GetDataIndex());

        if (NULL == pszFocusBg || 0 == strcmp(pszFocusBg, ""))
        {
            return;
        }
    }

    QRect rc(LIST_ITEM_FOCUS_BG_LEFT, LIST_ITEM_FOCUS_BG_TOP, LIST_ITEM_FOCUS_BG_WIDTH,
             LIST_ITEM_FOCUS_BG_HEIGHT);

    if (!rcBg.isEmpty())
    {
        rc = rcBg;
    }

    QPixmap pmBg = THEME_GET_BMP(pszFocusBg);

    stylePainter.drawPixmap(rc, pmBg);
}
