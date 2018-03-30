#include "rtpstatusitem.h"
#include <QStylePainter>
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define TALK_RTP_ITEM_FONT_SIZE 16
#define TALK_RTP_ITEM_FONT_SIZE_MIN 12
#define TALK_RTP_ITEM_FONT_SIZE_MAX 20
#define TALK_RTP_ITEM_LEFT_MARGIN 10
#define TAG_VALUE_SPACING   20
const QColor ITME_DEFAULT_COLOR(Qt::black);
#define TAG_RATIO (0.45)
}


CRTPStatusItem::CRTPStatusItem(QWidget * parent/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_RTP, parent, NULL)
    , m_fontColor(ITME_DEFAULT_COLOR)
#ifdef IF_ENABLE_TESTABILITY
    , m_rcTag(QRect())
    , m_rcValue(QRect())
#endif
{
    setObjectName("CRTPStatusItem");
    qWidgetUtility::setFontStyle(this, TALK_RTP_ITEM_FONT_SIZE);
}

CRTPStatusItem::~CRTPStatusItem()
{
}

void CRTPStatusItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    stylePainter.setPen(m_fontColor);

    QRect rectTag = this->rect();
    rectTag.setLeft(rectTag.left() + TALK_RTP_ITEM_LEFT_MARGIN);
    rectTag.setWidth((this->width() - TAG_VALUE_SPACING) * TAG_RATIO - TALK_RTP_ITEM_LEFT_MARGIN);
#ifdef IF_ENABLE_TESTABILITY
    m_rcTag = rectTag;
#endif
    //draw tag
    stylePainter.drawItemText(rectTag, Qt::AlignLeft | Qt::AlignVCenter, this->palette(), true,
                              m_strTag);

    QRect rectValue = this->rect();
    rectValue.setLeft(rectTag.right() + TAG_VALUE_SPACING);
    rectValue.setWidth(this->width() - rectValue.left());
#ifdef IF_ENABLE_TESTABILITY
    m_rcValue = rectValue;
#endif
    //draw value
    stylePainter.drawItemText(rectValue, Qt::AlignLeft | Qt::AlignVCenter, this->palette(), true,
                              m_strValue);
}

void CRTPStatusItem::SetFontSize(int iFontSize)
{
    if (IsFontSizeValid(iFontSize))
    {
        qWidgetUtility::setFontStyle(this, iFontSize);
    }
}

void CRTPStatusItem::SetFontColor(QColor clr)
{
    if (clr != m_fontColor)
    {
        m_fontColor = clr;
        if (this->isVisible())
        {
            update();
        }
    }
}

bool CRTPStatusItem::IsFontSizeValid(int iFontSize)
{
    if (TALK_RTP_ITEM_FONT_SIZE_MIN <= iFontSize
            && TALK_RTP_ITEM_FONT_SIZE_MAX >= iFontSize)
    {
        return true;
    }

    return false;
}

#ifdef IF_ENABLE_TESTABILITY
QString CRTPStatusItem::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcTag.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcTag.width();
    testInfo.height = m_rcTag.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strTag);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    ptAbsolute = mapToGlobal(m_rcValue.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcValue.width();
    testInfo.height = m_rcValue.height();
    testInfo.id = LABEL_INFO;
    testInfo.text = fromQString(m_strValue);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);


    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
