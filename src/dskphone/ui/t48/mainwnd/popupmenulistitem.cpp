#include <QtGui>
#include "popupmenulistitem.h"

#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"

#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define POPUP_MENU_ITEM_POS_HEIGHT 50

#define POPUP_MENU_ITEM_POS_ICON_LEFT 6
#define POPUP_MENU_ITEM_POS_ICON_TOP 12
#define POPUP_MENU_ITEM_POS_ICON_WIDTH 27
#define POPUP_MENU_ITEM_POS_ICON_HEIGHT 25

#define POPUP_MENU_ITEM_POS_TEXT_LEFT 39
#define POPUP_MENU_ITEM_POS_TEXT_TOP 0

#if IF_BUG_26252 || IF_FEATURE_STATION_NAME
#define POPUP_MENU_ITEM_POS_TEXT_WIDTH 184
#else
#define POPUP_MENU_ITEM_POS_TEXT_WIDTH 113
#endif

#define POPUP_MENU_ITEM_POS_TEXT_HEIGHT 50

#define POPUP_MENU_ITEM_POS_TEXT_FONT_SIZE 18
#define DEFAULT_POPUP_MENU_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define DEFAULT_POPUP_MENU_NORMAL_TEXT_COLOR QColor(0, 0, 0)
}

CPopupMenuListItem::CPopupMenuListItem(QWidget * parent/* = NULL*/,
                                       IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_POPUP_MENU, parent, pAction)
    , m_rcIcon(POPUP_MENU_ITEM_POS_ICON_LEFT, POPUP_MENU_ITEM_POS_ICON_TOP,
               POPUP_MENU_ITEM_POS_ICON_WIDTH, POPUP_MENU_ITEM_POS_ICON_HEIGHT)
    , m_rcText(POPUP_MENU_ITEM_POS_TEXT_LEFT, POPUP_MENU_ITEM_POS_TEXT_TOP,
               POPUP_MENU_ITEM_POS_TEXT_WIDTH, POPUP_MENU_ITEM_POS_TEXT_HEIGHT)
{
    setObjectName("CPopupMenuListItem");

    setGeometry(0, 0, 0, POPUP_MENU_ITEM_POS_HEIGHT);

    qWidgetUtility::setFontStyle(this, POPUP_MENU_ITEM_POS_TEXT_FONT_SIZE);
}

CPopupMenuListItem::~CPopupMenuListItem(void)
{
}

int CPopupMenuListItem::GetPopupMenuListItemType()
{
    return LIST_ITEM_TYPE_POPUP_MENU;
}

void CPopupMenuListItem::SetMenuIcon(const char * pszIcon, const char * pszSelIcon)
{
    if (NULL != pszIcon)
    {
        m_strIcon = pszIcon;
    }

    if (NULL != pszSelIcon)
    {
        m_strSelIcon = pszSelIcon;
    }
}

void CPopupMenuListItem::SetMenuText(const QString & strText)
{
    m_strText = strText;
}

void CPopupMenuListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintMenu(stylePainter);
}

void CPopupMenuListItem::PaintMenu(QStylePainter & stylePainter)
{
    // 画按下效果
    if (NULL != m_pDelegate)
    {
        PaintDownBg(stylePainter);
    }

    QPixmap pmIcon;

    // 选中或按下设置为白色字体
    if (IsDown() || IsFocus())
    {
        stylePainter.setPen(DEFAULT_POPUP_MENU_SELECT_TEXT_COLOR);

        pmIcon = THEME_GET_BMP(m_strSelIcon.c_str());
    }
    else
    {
        stylePainter.setPen(DEFAULT_POPUP_MENU_NORMAL_TEXT_COLOR);

        pmIcon = THEME_GET_BMP(m_strIcon.c_str());
    }

    stylePainter.drawPixmap(m_rcIcon, pmIcon);

    stylePainter.drawItemText(m_rcText, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                              m_strText);
}

#ifdef IF_ENABLE_TESTABILITY
QString CPopupMenuListItem::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcIcon.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIcon.width();
    testInfo.height = m_rcIcon.height();
    testInfo.id = LABEL_ICON;
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    ptAbsolute = mapToGlobal(m_rcText.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcText.width();
    testInfo.height = m_rcText.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strText);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif