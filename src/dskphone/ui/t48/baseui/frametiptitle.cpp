#include <QtGui>
#include "frametiptitle.h"
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define FRAME_TIP_TITLE_HEIGHT 55
#define FRAME_TIP_TITLE_POS_TEXT_LEFT 16
#define FRAME_TIP_TITLE_FONT_SIZE 16
}

CFrameTipTitle::CFrameTipTitle(CFrameList * pFrameList/* = NULL*/,
                               IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_TIP_TITLE, pFrameList, pAction)
    , m_strTip(QLatin1String(""))
#ifdef IF_ENABLE_TESTABILITY
    , m_rcTip(QRect())
#endif
{
    setGeometry(0, 0, 0, FRAME_TIP_TITLE_HEIGHT);

    qWidgetUtility::setFontStyle(this, FRAME_TIP_TITLE_FONT_SIZE);

    setObjectName("CFrameTipTitle");
}

CFrameTipTitle::~CFrameTipTitle(void)
{
}

bool CFrameTipTitle::IsTipTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_TIP_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CFrameTipTitle::SetTip(const yl::string & strTip)
{
    m_strTip = QString(strTip.c_str());
}

void CFrameTipTitle::SetTip(const QString & strTip)
{
    m_strTip = strTip;
}

void CFrameTipTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintTip(stylePainter);
}

void CFrameTipTitle::PaintTip(QStylePainter & stylePainter, int nRight/* = 0*/)
{
    QRect rcTitle = rect();
    int nTextWidth = rcTitle.width();

    if (0 != nRight)
    {
        nTextWidth = nRight - FRAME_TIP_TITLE_POS_TEXT_LEFT;
    }
    else
    {
        nTextWidth -= FRAME_TIP_TITLE_POS_TEXT_LEFT;
    }

    QRect rcText(rcTitle.left() + FRAME_TIP_TITLE_POS_TEXT_LEFT, rcTitle.top(), nTextWidth,
                 rcTitle.height());

#ifdef IF_ENABLE_TESTABILITY
    m_rcTip = rcText;
#endif

    stylePainter.drawItemText(rcText, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                              m_strTip);
}

#ifdef IF_ENABLE_TESTABILITY
QString CFrameTipTitle::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcTip.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcTip.width();
    testInfo.height = m_rcTip.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strTip);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif