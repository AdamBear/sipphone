#include <QtGui>
#include "menulistitem.h"
#include "t4xpicpath.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define DEFAULT_MENU_ITEM_HEIGHT 55
#define DEFAULT_MENU_TEXT_POS_LEFT 0
#define DEFAULT_MENU_TEXT_POS_TOP 0
#define DEFAULT_MENU_TEXT_POS_HEIGHT 55

#define DEFAULT_MENU_BTN_ENTER_POS_RIGHT 15
#define DEFAULT_MENU_BTN_ENTER_POS_TOP 8
#define DEFAULT_MENU_BTN_ENTER_POS_WIDTH 39
#define DEFAULT_MENU_BTN_ENTER_POS_HEIGHT 39

#define DEFAULT_MENU_ICON_ENTER_POS_RIGHT 28
#define DEFAULT_MENU_ICON_ENTER_POS_TOP 19
#define DEFAULT_MENU_ICON_ENTER_POS_WIDTH 13
#define DEFAULT_MENU_ICON_ENTER_POS_HEIGHT 17

#define DEFAULT_MENU_ICON_FONT_SIZE 16
#define DEFAULT_MENU_ICON_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define DEFAULT_MENU_ICON_NORMAL_TEXT_COLOR QColor(0, 0, 0)
}

CMenuListItem::CMenuListItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_MENU, parent, pAction)
    , m_nMenuId(0)
    , m_btnEnter(this)
{

    setObjectName("CMenuListItem");

    setGeometry(0, 0, 0, DEFAULT_MENU_ITEM_HEIGHT);

    qWidgetUtility::setFontStyle(this, DEFAULT_MENU_ICON_FONT_SIZE);

    Relayout();
}

CMenuListItem::~CMenuListItem(void)
{
}

void CMenuListItem::ClearContent()
{
    m_strMenu = "";
    CListItem::ClearContent();
}

void CMenuListItem::SetId(int nMenuId)
{
    m_nMenuId = nMenuId;
}

void CMenuListItem::SetMenu(const QString & strMenu)
{
    m_strMenu = strMenu;
}

void CMenuListItem::SetMenu(const yl::string & strMenu)
{
    m_strMenu = QString(strMenu.c_str());
}

int CMenuListItem::GetId() const
{
    return m_nMenuId;
}

void CMenuListItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!ProcessMousePress(pEvent))
    {
        return;
    }

    m_btnEnter.ProcessMousePress(pEvent);
}

void CMenuListItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (ProcessMouseRelease(pEvent, true))
    {
        if (m_btnEnter.IsDown())
        {
            m_btnEnter.ResetDown();
        }
        return;
    }

    if (m_btnEnter.ProcessMouseRelease(pEvent))
    {
        if (IsFocus())
        {
            SetFocus(false);
        }

        SelectItem(true);
    }
}

void CMenuListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    QRect rcItem = rect();
    QPoint ptMenu(rcItem.left() + DEFAULT_MENU_TEXT_POS_LEFT, rcItem.top());

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        ptMenu.setX(ptMenu.x() + nLeftMargin);
    }

    PaintMenu(stylePainter, ptMenu);

    PaintEnterBtn(stylePainter);
}

void CMenuListItem::PaintMenu(QStylePainter & stylePainter, const QPoint & ptMenu)
{
    int nWidth = m_btnEnter.GetRect().left() - ptMenu.x();

    QRect rcMenu(ptMenu.x(), ptMenu.y() + DEFAULT_MENU_TEXT_POS_TOP, nWidth,
                 DEFAULT_MENU_TEXT_POS_HEIGHT);
#ifdef IF_ENABLE_TESTABILITY
    m_rcTitle = rcMenu;
#endif
    PaintMenu(stylePainter, rcMenu);
}

void CMenuListItem::PaintMenu(QStylePainter & stylePainter, const QRect & rcMenu)
{
    if (m_strMenu.isEmpty())
    {
        return;
    }

    // 选中或按下设置为白色字体
    if (IsDown())
    {
        stylePainter.setPen(DEFAULT_MENU_ICON_SELECT_TEXT_COLOR);
    }
    else
    {
        stylePainter.setPen(DEFAULT_MENU_ICON_NORMAL_TEXT_COLOR);
    }

    bool bEnable = isEnabled();
    const QPalette & txtPalette = palette();

    stylePainter.drawItemText(rcMenu, (Qt::AlignLeft | Qt::AlignVCenter), txtPalette, bEnable,
                              m_strMenu);
}

void CMenuListItem::PaintEnterBtn(QStylePainter & stylePainter)
{
    QRect rcItem = rect();
    int nLeft = rcItem.right() - DEFAULT_MENU_ICON_ENTER_POS_RIGHT - DEFAULT_MENU_ICON_ENTER_POS_WIDTH;
    int nTop = rcItem.top() + DEFAULT_MENU_ICON_ENTER_POS_TOP;
    QRect rcIcon(nLeft, nTop, DEFAULT_MENU_ICON_ENTER_POS_WIDTH, DEFAULT_MENU_ICON_ENTER_POS_HEIGHT);

    m_btnEnter.PaintButton(stylePainter, PIC_GENERAL_LIST_ITEM_ENTER, PIC_GENERAL_LIST_ITEM_ENTER,
                           PIC_GENERAL_LIST_ITEM_ENTER, rcIcon);
}

void CMenuListItem::Relayout()
{
    QRect rcItem = rect();
    int nLeft = rcItem.right() - DEFAULT_MENU_BTN_ENTER_POS_RIGHT - DEFAULT_MENU_BTN_ENTER_POS_WIDTH;
    int nTop = rcItem.top() + DEFAULT_MENU_BTN_ENTER_POS_TOP;
    QRect rcBtnEnter(nLeft, nTop, DEFAULT_MENU_BTN_ENTER_POS_WIDTH, DEFAULT_MENU_BTN_ENTER_POS_HEIGHT);

    m_btnEnter.SetRect(rcBtnEnter);
}
