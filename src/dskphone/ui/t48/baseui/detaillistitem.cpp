#include <QtGui>
#include "detaillistitem.h"
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define DETAIL_LIST_ITEM_TITLE_WEIGHT 132

// Detail的默认位置信息
#define DEFAULT_DETAIL_LIST_ITEM_VALUE_LEFT 138
#define DEFAULT_DETAIL_LIST_ITEM_VALUE_TOP 3
#define DEFAULT_DETAIL_LIST_ITEM_VALUE_WIDTH 215
#define DEFAULT_DETAIL_LIST_ITEM_VALUE_HEIGHT 36
#define DEFAULT_DETAIL_LIST_ITEM_FONT_SIZE 16
}

CDetailListItem::CDetailListItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CDetailListItemBase(parent, pAction)
    , m_rcValue(DEFAULT_DETAIL_LIST_ITEM_VALUE_LEFT, DEFAULT_DETAIL_LIST_ITEM_VALUE_TOP,
                DEFAULT_DETAIL_LIST_ITEM_VALUE_WIDTH, DEFAULT_DETAIL_LIST_ITEM_VALUE_HEIGHT)
    , m_strValue(QLatin1String(""))
    , m_nFontSize(16)
    , m_colValue(Qt::black)
{

    setObjectName("CDetailListItem");

    m_nType |= LIST_ITEM_SUB_TYPE_DETAIL_ITEM;

    m_rcTitle.setWidth(DETAIL_LIST_ITEM_TITLE_WEIGHT);

    qWidgetUtility::setFontStyle(this, DEFAULT_DETAIL_LIST_ITEM_FONT_SIZE);
}

CDetailListItem::~CDetailListItem(void)
{
}

int CDetailListItem::GetDetailListItemType()
{
    return (LIST_ITEM_TYPE_DETAIL | LIST_ITEM_SUB_TYPE_DETAIL_ITEM);
}

void CDetailListItem::SetValue(const QString & strValue)
{
    m_strValue = strValue;
}

QString CDetailListItem::GetValue() const
{
    return m_strValue;
}

void CDetailListItem::SetValuePos(int nLeft)
{
    QRect rcItem = rect();

    m_rcValue.setLeft(nLeft);
    m_rcValue.setTop(rcItem.top());
    m_rcValue.setWidth(rcItem.width() - nLeft);
    m_rcValue.setHeight(rcItem.height());
}

void CDetailListItem::SetValueRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_rcValue = QRect(nLeft, nTop, nWidth, nHeight);
}

void CDetailListItem::SetValueRect(const QRect & rcValue)
{
    m_rcValue = rcValue;
}

// 设置字体大小
void CDetailListItem::SetValueFontSize(int nSize)
{
    m_nFontSize = nSize;
}

void CDetailListItem::SetValueColor(QColor color)
{
    m_colValue = color;
}

void CDetailListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintDownBg(stylePainter);

    PaintTitle(stylePainter);

    PaintValue(stylePainter);
}

void CDetailListItem::PaintValue(QStylePainter & stylePainter)
{
    QFont font;
    font.setPointSize(m_nFontSize);
    stylePainter.setFont(font);
    stylePainter.setPen(IsDown() ? m_colValue : Qt::black);

    stylePainter.drawItemText(m_rcValue, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(), m_strValue);
}

#ifdef IF_ENABLE_TESTABILITY
QString CDetailListItem::GetTestInfo()
{
    QString strTestinfo = CDetailListItemBase::GetTestInfo();
    xml_document doc;
    doc.load(strTestinfo.toLatin1().constData());
    if (doc.empty())
    {
        return strTestinfo;
    }
    xml_node nodeRoot = doc.child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    // checkbox
    ptAbsolute = mapToGlobal(m_rcValue.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcValue.width();
    testInfo.height = m_rcValue.height();
    testInfo.id = LABEL_INFO;
    testInfo.text = fromQString(m_strValue);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif