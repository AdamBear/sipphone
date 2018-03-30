#include "textmenuitem.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define DEFAULT_TEXT_MENU_COLOR_NORMAL QColor(0, 0, 0)
#define DEFAULT_TEXT_MENU_COLOR_PRESS QColor(255, 255, 255)
}

CTextMenuItem::CTextMenuItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_TEXT_MENU, parent, pAction)
    , m_nMenuId(0)
    , m_bShowIndex(false)
    , m_bChangeColorOnPress(true)
    , m_strText("")
    , m_strClickAction("")
    , m_clrNormal(DEFAULT_TEXT_MENU_COLOR_NORMAL)
    , m_clrSelect(DEFAULT_TEXT_MENU_COLOR_PRESS)
    , m_clrPress(DEFAULT_TEXT_MENU_COLOR_PRESS)
#ifdef IF_ENABLE_TESTABILITY
    , m_rcText(QRect())
    , ichoose(0)
#endif
{
    Relayout();

    setObjectName("CTextMenuItem");
}

CTextMenuItem::~CTextMenuItem(void)
{
}

void CTextMenuItem::ClearContent()
{
    m_strText = "";
    m_strClickAction = "";
    CListItem::ClearContent();
}

void CTextMenuItem::SetId(int nMenuId)
{
    m_nMenuId = nMenuId;
}

int CTextMenuItem::GetId() const
{
    return m_nMenuId;
}

void CTextMenuItem::SetMenu(const QString & strMenu)
{
    m_strText = strMenu;
}

void CTextMenuItem::SetMenu(const yl::string & strMenu)
{
    m_strText = QString(strMenu.c_str());
}

void CTextMenuItem::SetClickAction(const QString & strClickAction)
{
    m_strClickAction = strClickAction;
}

QString CTextMenuItem::GetClickAction()
{
    return m_strClickAction;
}

void CTextMenuItem::SetTextColor(QColor & clrNormal, QColor & clrSelect)
{
    m_clrNormal = clrNormal;
    m_clrSelect = clrSelect;
}

void CTextMenuItem::SetChangeColorOnPress(bool bChangeOnPress)
{
    m_bChangeColorOnPress = bChangeOnPress;
}

void CTextMenuItem::SetPressColor(QColor & clrPress)
{
    m_clrPress = clrPress;
}

QColor & CTextMenuItem::GetTextPaintColor()
{
    if (m_bChangeColorOnPress && IsDown())
    {
#ifdef IF_ENABLE_TESTABILITY
        ichoose = 1;
#endif
        return m_clrPress;
    }
    else
    {
        if (IsFocus())
        {
#ifdef IF_ENABLE_TESTABILITY
            ichoose = 1;
#endif
            return m_clrSelect;
        }
        else
        {
#ifdef IF_ENABLE_TESTABILITY
            ichoose = 0;
#endif
            return m_clrNormal;
        }
    }
}

void CTextMenuItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    QRect rcItem = rect();
    QRect rectText = rect();

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        rectText.setX(rcItem.x() + nLeftMargin);
        rectText.setWidth(rcItem.width() - nLeftMargin - nRightMargin);
    }

    QString strTextToDraw = m_strText;
    if (m_bShowIndex)
    {
        strTextToDraw = QString::number(GetDataIndex() + 1) + QString(". ") + m_strText;
    }

    stylePainter.setPen(GetTextPaintColor());
    stylePainter.drawItemText(rectText, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                              strTextToDraw);
#ifdef IF_ENABLE_TESTABILITY
    m_rcText = rectText;
    m_strDraw = strTextToDraw;
#endif
}

void CTextMenuItem::SetShowIndex(bool bShowIndex)
{
    if (bShowIndex != m_bShowIndex)
    {
        m_bShowIndex = bShowIndex;
        if (isVisible())
        {
            update();
        }
    }
}

bool CTextMenuItem::IsShowIndex()
{
    return m_bShowIndex;
}

#ifdef IF_ENABLE_TESTABILITY
QString CTextMenuItem::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcText.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcText.width();
    testInfo.height = m_rcText.height();
    testInfo.choose = ichoose;
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strDraw);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif
