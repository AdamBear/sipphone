#include <QtGui>
#include "detaillistitembase.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
// 列表框高度
#define DEFAULT_DETAIL_LIST_ITEM_HEIGHT 43

// 标题位置信息
#define DEFAULT_DETAIL_LIST_ITEM_TITLE_LEFT 6
#define DEFAULT_DETAIL_LIST_ITEM_TITLE_TOP 0
#define DEFAULT_DETAIL_LIST_ITEM_TITLE_WEIGHT 135
#define DEFAULT_DETAIL_LIST_ITEM_TITLE_HEIGHT 43
}

CDetailListItemBase::CDetailListItemBase(QWidget * parent/* = NULL*/,
        IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_DETAIL, parent, pAction)
    , m_rcTitle(DEFAULT_DETAIL_LIST_ITEM_TITLE_LEFT, DEFAULT_DETAIL_LIST_ITEM_TITLE_TOP,
                DEFAULT_DETAIL_LIST_ITEM_TITLE_WEIGHT, DEFAULT_DETAIL_LIST_ITEM_TITLE_HEIGHT)
    , m_strTitle(QLatin1String(""))
    , m_colTitle(Qt::black)
{
    setObjectName("CDetailListItemBase");
    setGeometry(0, 0, 0, DEFAULT_DETAIL_LIST_ITEM_HEIGHT);
}

CDetailListItemBase::~CDetailListItemBase(void)
{
}

int CDetailListItemBase::GetDetailListItemBaseType()
{
    return LIST_ITEM_TYPE_DETAIL;
}

void CDetailListItemBase::SetItemHeight(int nHeight)
{
    QRect rcItem = geometry();

    rcItem.setHeight(nHeight);
    setGeometry(rcItem);
}

void CDetailListItemBase::SetTitleRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_rcTitle = QRect(nLeft, nTop, nWidth, nHeight);
}

void CDetailListItemBase::SetTitleRect(const QRect & rcTitle)
{
    m_rcTitle = rcTitle;
}

void CDetailListItemBase::SetTitle(const char * pszTitle, bool bAddColon/* = true*/)
{
    if (NULL == pszTitle)
    {
        m_strTitle = QLatin1String("");
    }
    else
    {
        SetTitle(QString(pszTitle), bAddColon);
    }
}

void CDetailListItemBase::SetTitle(const QString & strTitle, bool bAddColon/* = true*/)
{
    m_strTitle = strTitle;

    if (bAddColon)
    {
        m_strTitle += QLatin1String(":");
    }
}

void CDetailListItemBase::SetValue(const char * pszValue)
{
    QString strValue = QLatin1String("");

    if (NULL != pszValue)
    {
        strValue = pszValue;
    }

    SetValue(strValue);
}

void CDetailListItemBase::SetValue(const QString & strValue)
{
}

QString CDetailListItemBase::GetValue() const
{
    return QLatin1String("");
}

int CDetailListItemBase::GetDetailSubType() const
{
    return (m_nType & LIST_ITEM_TYPE_DETAIL_MARK);
}

int CDetailListItemBase::GetDefaultTitleLeft() const
{
    return DEFAULT_DETAIL_LIST_ITEM_TITLE_LEFT;
}

int CDetailListItemBase::GetDefaultTitleWidth() const
{
    return DEFAULT_DETAIL_LIST_ITEM_TITLE_WEIGHT;
}

void CDetailListItemBase::SetDefaultTitlePos()
{
    m_rcTitle.setRect(DEFAULT_DETAIL_LIST_ITEM_TITLE_LEFT, DEFAULT_DETAIL_LIST_ITEM_TITLE_TOP,
                      DEFAULT_DETAIL_LIST_ITEM_TITLE_WEIGHT, DEFAULT_DETAIL_LIST_ITEM_TITLE_HEIGHT);
}

void CDetailListItemBase::SetTitleColor(QColor color)
{
    m_colTitle = color;
}

// 画标题
void CDetailListItemBase::PaintTitle(QStylePainter & stylePainter)
{
    stylePainter.setPen(IsDown() ? m_colTitle : Qt::black);
    stylePainter.drawItemText(m_rcTitle, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(), m_strTitle);
}

#ifdef IF_ENABLE_TESTABILITY
QString CDetailListItemBase::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    QString strTitle = m_strTitle + QLatin1String(":");
    ptAbsolute = mapToGlobal(m_rcTitle.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcTitle.width();
    testInfo.height = m_rcTitle.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(strTitle);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif