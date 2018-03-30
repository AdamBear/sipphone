#include <QtGui>
#include "iconmenulistitem.h"
#include "uikernel/ilistitemdelegate.h"
#include "t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define DEFAULT_ICON_MENU_POS_ICON_LEFT 0
#define DEFAULT_ICON_MENU_POS_ICON_TOP 8
#define DEFAULT_ICON_MENU_POS_ICON_WIDTH 39
#define DEFAULT_ICON_MENU_POS_ICON_HEIGHT 39
#define DEFALUT_MENU_POS_LEFT_MARGIN 10
}

CIconMenuListItem::CIconMenuListItem(QWidget * parent/* = NULL*/,
                                     IListItemAction * pAction/* = NULL*/)
    : CMenuListItem(parent, pAction)
    , m_strIconPath("")
{
    m_nType |= LIST_ITEM_SUB_TYPE_ICON_MENU;
}

CIconMenuListItem::~CIconMenuListItem(void)
{
}

int CIconMenuListItem::GetIconMenuListItemType()
{
    return (LIST_ITEM_TYPE_MENU | LIST_ITEM_SUB_TYPE_ICON_MENU);
}

void CIconMenuListItem::ClearContent()
{
    m_strIconPath = "";

    CMenuListItem::ClearContent();
}

void CIconMenuListItem::SetIconPath(const yl::string & strPath)
{
    m_strIconPath = strPath;
}

void CIconMenuListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintDownBg(stylePainter);

    QRect rcItem = rect();
    QPoint ptBegin(rcItem.left(), rcItem.top());

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        ptBegin.setX(ptBegin.x() + nLeftMargin);
    }

    PaintIcon(stylePainter, ptBegin);

    int nMenuLeft = ptBegin.x() + DEFAULT_ICON_MENU_POS_ICON_LEFT + DEFAULT_ICON_MENU_POS_ICON_WIDTH;

    ptBegin.setX(nMenuLeft + DEFALUT_MENU_POS_LEFT_MARGIN);
    PaintMenu(stylePainter, ptBegin);

    PaintEnterBtn(stylePainter);
}

void CIconMenuListItem::PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon)
{
    QRect rcIcon(ptIcon.x() + DEFAULT_ICON_MENU_POS_ICON_LEFT,
                 ptIcon.y() + DEFAULT_ICON_MENU_POS_ICON_TOP,
                 DEFAULT_ICON_MENU_POS_ICON_WIDTH, DEFAULT_ICON_MENU_POS_ICON_HEIGHT);

    PaintIcon(stylePainter, rcIcon);
}

void CIconMenuListItem::PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon)
{
    if (m_strIconPath.empty())
    {
        return;
    }

    QPixmap pmIcon = THEME_GET_BMP(m_strIconPath.c_str());

    if (pmIcon.isNull())
    {
        return;
    }
#ifdef IF_ENABLE_TESTABILITY
    m_rcIcon = rcIcon;
#endif
    stylePainter.drawPixmap(rcIcon, pmIcon);
}

#ifdef IF_ENABLE_TESTABILITY
QString CIconMenuListItem::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    AddBtnNode(nodeRoot, m_btnEnter, BTN_DETAIL);

    ptAbsolute = mapToGlobal(m_rcTitle.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcTitle.width();
    testInfo.height = m_rcTitle.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strMenu);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    ptAbsolute = mapToGlobal(m_rcIcon.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIcon.width();
    testInfo.height = m_rcIcon.height();
    testInfo.id = LABEL_ICON;
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif