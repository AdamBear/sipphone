#include <QtGui>
#include "frameselecttitle.h"
#include "framelistaction.h"
#include "t4xpicpath.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define FRAME_SELECT_TITLE_HEIGHT 55

#define FRAME_SELECT_TITLE_POS_CHECK_BOX_LEFT 6
#define FRAME_SELECT_TITLE_POS_CHECK_BOX_TOP 1
#define FRAME_SELECT_TITLE_POS_CHECK_BOX_WIDTH 42
#define FRAME_SELECT_TITLE_POS_CHECK_BOX_HEIGHT 53

#define FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_LEFT 16
#define FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_TOP 16
#define FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_WIDTH 22
#define FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_HEIGHT 23

#define FRAME_SELECT_TITLE_POS_TEXT_LEFT 0
}

CFrameSelectTitle::CFrameSelectTitle(CFrameList * pFrameList/* = NULL*/,
                                     IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_SELECT_TITLE, pFrameList, pAction)
    , m_btnCheckBox(this)
    , m_nTotalAmount(0)
    , m_nSelectAmount(0)
    , m_bChecked(false)
#if IF_FEATURE_GBBOOK
    , m_bShowChecked(true)
#endif
{
    setGeometry(0, 0, 0, FRAME_SELECT_TITLE_HEIGHT);

    m_btnCheckBox.SetRect(FRAME_SELECT_TITLE_POS_CHECK_BOX_LEFT, FRAME_SELECT_TITLE_POS_CHECK_BOX_TOP,
                          FRAME_SELECT_TITLE_POS_CHECK_BOX_WIDTH, FRAME_SELECT_TITLE_POS_CHECK_BOX_HEIGHT);

    setObjectName("CFrameSelectTitle");
}

CFrameSelectTitle::~CFrameSelectTitle(void)
{
}

bool CFrameSelectTitle::IsSelectTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_SELECT_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CFrameSelectTitle::SetTotalAmount(int nTotalAmount, bool bRefresh/* = false*/)
{
    m_nTotalAmount = nTotalAmount;

    if (bRefresh)
    {
        update(rect());
    }
}

void CFrameSelectTitle::SetSelectAmount(int nSelectAmount, bool bRefresh/* = false*/)
{
    m_nSelectAmount = nSelectAmount;

    if (0 != m_nSelectAmount && nSelectAmount == m_nTotalAmount)
    {
        m_bChecked = true;
    }
    else
    {
        m_bChecked = false;
    }

    if (bRefresh)
    {
        update(rect());
    }
}

void CFrameSelectTitle::SetCheck(bool bCheck)
{
    bool bUpdate = false;

    if (m_bChecked != bCheck)
    {
        bUpdate = true;
    }

    if (bUpdate)
    {
        update(rect());
    }
}

bool CFrameSelectTitle::IsCheck() const
{
    return m_bChecked;
}

void CFrameSelectTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnCheckBox.ProcessMousePress(pEvent);
}

void CFrameSelectTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (m_btnCheckBox.ProcessMouseRelease(pEvent))
    {
        m_bChecked = !m_bChecked;
        DoAction(LIST_TITLE_ACTION_CHECK_STATE);
    }
}

void CFrameSelectTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintCheckBox(stylePainter);
}

void CFrameSelectTitle::PaintCheckBox(QStylePainter & stylePainter)
{
#if IF_FEATURE_GBBOOK
    if (!m_bShowChecked)
    {
        return;
    }
#endif


    QRect rcIcon(FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_LEFT, FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_TOP,
                 FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_WIDTH, FRAME_SELECT_TITLE_POS_CHECK_BOX_ICON_HEIGHT);

    if (m_bChecked)
    {
        m_btnCheckBox.PaintButton(stylePainter, PIC_CHECK_BOX_SELECT, PIC_CHECK_BOX_SELECT, NULL, rcIcon);
    }
    else
    {
        m_btnCheckBox.PaintButton(stylePainter, PIC_CHECK_BOX_NORMAL, PIC_CHECK_BOX_NORMAL, NULL, rcIcon);
    }

    QRect rcTitle = rect();
    QRect rcCheckBox = m_btnCheckBox.GetRect();
    int nLeft = rcCheckBox.right() + FRAME_SELECT_TITLE_POS_TEXT_LEFT;
    QRect rcText(nLeft, rcTitle.top(), rcTitle.width() - nLeft, rcTitle.height());

    QString strText = LANG_TRANSLATE(TRID_SELECT_ALL);
    char szBuffer[64];

    snprintf(szBuffer, sizeof(szBuffer), " (%d/%d)", m_nSelectAmount, m_nTotalAmount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';
    strText += szBuffer;

    stylePainter.drawItemText(rcText, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                              strText);
#ifdef IF_ENABLE_TESTABILITY
    m_rcText = rcText;
    m_strText = strText;
#endif
}

#if IF_FEATURE_GBBOOK
void CFrameSelectTitle::SetCheckable(bool bCheckable)
{
    m_bShowChecked = bCheckable;

    Relayout();
}

void CFrameSelectTitle::Relayout()
{
    if (!m_bShowChecked)
    {
        m_btnCheckBox.SetRect(0, 0, 0, 0);
        return;
    }

    m_btnCheckBox.SetRect(FRAME_SELECT_TITLE_POS_CHECK_BOX_LEFT, FRAME_SELECT_TITLE_POS_CHECK_BOX_TOP,
                          FRAME_SELECT_TITLE_POS_CHECK_BOX_WIDTH, FRAME_SELECT_TITLE_POS_CHECK_BOX_HEIGHT);
}
#endif

#ifdef IF_ENABLE_TESTABILITY
QString CFrameSelectTitle::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcText.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcText.width();
    testInfo.height = m_rcText.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strText);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    AddBtnNode(nodeRoot, m_btnCheckBox, BTN_CHECK);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif