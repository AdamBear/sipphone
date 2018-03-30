#include <QtGui>
#include "popupmenulisttitle.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#if IF_BUG_26252 || IF_FEATURE_STATION_NAME
#define POPUP_MENU_TITLE_POS_WIDTH 223
#else
#define POPUP_MENU_TITLE_POS_WIDTH 152
#endif

#define POPUP_MENU_TITLE_POS_HEIGHT 43

#define POPUP_MENU_TITLE_POS_ICON_LEFT 8
#define POPUP_MENU_TITLE_POS_ICON_TOP 9
#define POPUP_MENU_TITLE_POS_ICON_WIDTH 27
#define POPUP_MENU_TITLE_POS_ICON_HEIGHT 25

#define POPUP_MENU_TITLE_POS_TEXT_LEFT 41
#define POPUP_MENU_TITLE_POS_TEXT_TOP 0
#if IF_BUG_26252 || IF_FEATURE_STATION_NAME
#define POPUP_MENU_TITLE_POS_TEXT_WIDTH 182
#else
#define POPUP_MENU_TITLE_POS_TEXT_WIDTH 111
#endif
#define POPUP_MENU_TITLE_POS_TEXT_HEIGHT 43

#define POPUP_MENU_TITLE_POS_TEXT_FONT_SIZE 18
#define DEFAULT_POPUP_MENU_SELECT_TEXT_COLOR QColor(255, 255, 255)
}

CPopupMenuListTitle::CPopupMenuListTitle(CFrameList * pFrameList/* = NULL*/,
        IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_POPUP_MENU, pFrameList, pAction)
#ifdef IF_ENABLE_TESTABILITY
    , m_rcText(QRect())
    , m_rcIcon(QRect())
#endif
{
    setGeometry(0, 0, POPUP_MENU_TITLE_POS_WIDTH, POPUP_MENU_TITLE_POS_HEIGHT);
    qWidgetUtility::setFontStyle(this, POPUP_MENU_TITLE_POS_TEXT_FONT_SIZE);

    QPalette objPal = palette();

    objPal.setColor(QPalette::WindowText, DEFAULT_POPUP_MENU_SELECT_TEXT_COLOR);
    setPalette(objPal);

    setObjectName("CPopupMenuListTitle");
}

CPopupMenuListTitle::~CPopupMenuListTitle(void)
{

}

bool CPopupMenuListTitle::IsPopupMenuTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_POPUP_MENU == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CPopupMenuListTitle::SetTitleText(const QString & strText)
{
    m_strText = strText;
}

void CPopupMenuListTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QRect rcIcon(POPUP_MENU_TITLE_POS_ICON_LEFT, POPUP_MENU_TITLE_POS_ICON_TOP,
                 POPUP_MENU_TITLE_POS_ICON_WIDTH, POPUP_MENU_TITLE_POS_ICON_HEIGHT);
    QPixmap pmIcon = THEME_GET_BMP(PIC_DSS_LINE_REGISTERED);

    stylePainter.drawPixmap(rcIcon, pmIcon);

    QRect rcText(POPUP_MENU_TITLE_POS_TEXT_LEFT, POPUP_MENU_TITLE_POS_TEXT_TOP,
                 POPUP_MENU_TITLE_POS_TEXT_WIDTH, POPUP_MENU_TITLE_POS_TEXT_HEIGHT);

    stylePainter.drawItemText(rcText, (Qt::AlignLeft | Qt::AlignVCenter), palette(), isEnabled(),
                              m_strText);
#ifdef IF_ENABLE_TESTABILITY
    m_rcText = rcText;
    m_rcIcon = rcIcon;
#endif
}

#ifdef IF_ENABLE_TESTABILITY
QString CPopupMenuListTitle::GetTestInfo()
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