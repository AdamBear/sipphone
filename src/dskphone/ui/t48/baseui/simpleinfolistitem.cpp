#include <QtGui>
#include "uikernel/ilistitemdelegate.h"
#include "simpleinfolistitem.h"
#include "imagemanager/modimagemanager.h"
#include "fontcolor.h"

namespace
{
// 默认高度
#define DEFAULT_SIMPLE_INFO_ITEM_HEIGHT 55

// 头像位置信息
#define DEFAULT_INFO_POS_ICON_LEFT 0
#define DEFALUT_INFO_POS_ICON_TOP 7
#define DEFAULT_INFO_POS_ICON_WIDTH 41
#define DEFAULT_INFO_POS_ICON_HEIGHT 41

// 文本位置信息
#define DEFALUT_INFO_POS_TEXT_LEFT_MARGIN 10
#define DEFAULT_INFO_POS_TITLE_TOP 8
#define DEFAULT_INFO_POS_TITLE_HEIGHT 22
#define DEFAULT_INFO_POS_SUBTITLE_BOTTOM 48
#define DEFAULT_INFO_POS_SUBTITLE_HEIGHT 18
#define DEFAULT_INFO_POS_INFO_WIDTH 100
#define DEFAULT_INFO_POS_INFO_HEIGHT 55

#define DEFAULT_INFO_TEXT_FONT_SIZE_TITLE 16
#define DEFAULT_INFO_TEXT_FONT_SIZE_SUB_TITLE 14
#define DEFAULT_INFO_TEXT_FONT_SIZE_INFO 14

#define DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR QColor(0, 0, 0)
}

bool CSimpleInfoListItem::s_bFontInit = false;
QFont CSimpleInfoListItem::s_fontTitle;
QFont CSimpleInfoListItem::s_fontSubTitle;
QFont CSimpleInfoListItem::s_fontInfo;

CSimpleInfoListItem::CSimpleInfoListItem(QWidget * parent/* = NULL*/,
        IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_SIMPLE_INFO, parent, pAction)
    , m_strIconPath("")
    , m_rcInfo(QRect())
    , m_rcTitle(QRect())
    , m_rcSubTitle(QRect())
    , m_rcIcon(QRect())
    , m_bDefineTitleColor(false)
    , m_TitleColor(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR)
    , m_bShowSubTitle(false)
    , m_bShowInfo(false)
    , m_bContactPicVisible(true)
{
    setObjectName("CSimpleInfoListItem");

    setGeometry(0, 0, 0, DEFAULT_SIMPLE_INFO_ITEM_HEIGHT);

    if (!s_bFontInit)
    {
        s_bFontInit = true;
        s_fontTitle = font();
        s_fontSubTitle = font();
        s_fontInfo = font();

        SetTitleFontSize(DEFAULT_INFO_TEXT_FONT_SIZE_TITLE, QFont::Bold, true);
        SetSubTitleFontSize(DEFAULT_INFO_TEXT_FONT_SIZE_SUB_TITLE);
        SetInfoFontSize(DEFAULT_INFO_TEXT_FONT_SIZE_INFO);
    }
}

CSimpleInfoListItem::~CSimpleInfoListItem(void)
{
}

void CSimpleInfoListItem::SetTitleFontSize(int nFontSize, int nFontWeight/* = QFont::Normal*/,
        bool bBold/* = false*/)
{
    s_fontTitle.setPointSize(nFontSize);
    s_fontTitle.setWeight(nFontWeight);
    s_fontTitle.setBold(bBold);
}

void CSimpleInfoListItem::SetSubTitleFontSize(int nFontSize, int nFontWeight/* = QFont::Normal*/,
        bool bBold/* = false*/)
{
    s_fontSubTitle.setPointSize(nFontSize);
    s_fontSubTitle.setWeight(nFontWeight);
    s_fontSubTitle.setBold(bBold);
}

void CSimpleInfoListItem::SetInfoFontSize(int nFontSize, int nFontWeight/* = QFont::Normal*/,
        bool bBold/* = false*/)
{
    s_fontInfo.setPointSize(nFontSize);
    s_fontInfo.setWeight(nFontWeight);
    s_fontInfo.setBold(bBold);
}

void CSimpleInfoListItem::ClearContent()
{
    m_strTitle = QLatin1String("");
    m_strSubTitle = QLatin1String("");
    m_strInfo = QLatin1String("");
    m_strIconPath = "";

    m_bDefineTitleColor = false;
    m_TitleColor = DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR;
    m_bShowSubTitle = false;
    m_bShowInfo = false;

    CListItem::ClearContent();
}

void CSimpleInfoListItem::SetTitle(const yl::string & strTitle)
{
    m_strTitle = QString(strTitle.c_str());
}

void CSimpleInfoListItem::SetTitle(const QString & strTitle)
{
    m_strTitle = strTitle;
}

void CSimpleInfoListItem::SetSubTitle(const yl::string & strSubTitle)
{
    m_strSubTitle = QString(strSubTitle.c_str());
}

void CSimpleInfoListItem::SetSubTitle(const QString & strSubTitle)
{
    m_strSubTitle = strSubTitle;
}

void CSimpleInfoListItem::SetInfo(const yl::string & strInfo)
{
    m_strInfo = QString(strInfo.c_str());
}

void CSimpleInfoListItem::SetInfo(const QString & strInfo)
{
    m_strInfo = strInfo;
}

void CSimpleInfoListItem::SetIcon(const yl::string & strIconPath)
{
    m_strIconPath = strIconPath;
}

void CSimpleInfoListItem::SetTitleColor(QColor color)
{
    m_bDefineTitleColor = true;
    m_TitleColor = color;
}

void CSimpleInfoListItem::setShowSubTitle(bool bShow)
{
    m_bShowSubTitle = bShow;
}

void CSimpleInfoListItem::setShowInfo(bool bShow)
{
    m_bShowInfo = bShow;
}

void CSimpleInfoListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QRect rcItem = rect();
    QPoint ptBegin(rcItem.left(), rcItem.top());

    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        ptBegin.setX(ptBegin.x() + nLeftMargin);
    }

    PaintDefault(stylePainter, ptBegin);
}

void CSimpleInfoListItem::PaintDefault(QStylePainter & stylePainter, const QPoint & ptBegin,
                                       int nInfoWidth/* = 0*/, const char * pszDefaultIcon/* = NULL*/)
{
    QPoint ptIcon(ptBegin.x() + DEFAULT_INFO_POS_ICON_LEFT, ptBegin.y());

    PaintIconInfo(stylePainter, ptIcon, nInfoWidth, pszDefaultIcon);
}

void CSimpleInfoListItem::PaintDefault(QStylePainter & stylePainter, QPixmap & pmIcon,
                                       const QPoint & ptBegin, int nInfoWidth/* = 0*/)
{
    QRect rcIcon(ptBegin.x() + DEFAULT_INFO_POS_ICON_LEFT, ptBegin.y() + DEFALUT_INFO_POS_ICON_TOP,
                 DEFAULT_INFO_POS_ICON_WIDTH, DEFAULT_INFO_POS_ICON_HEIGHT);

    stylePainter.drawPixmap(rcIcon, pmIcon);

    QPoint ptInfo(rcIcon.right(), ptBegin.y());

    if (0 == nInfoWidth)
    {
        QRect rcItem = rect();

        nInfoWidth = rcItem.right() - ptInfo.x();
    }
    else
    {
        nInfoWidth = nInfoWidth - ptInfo.x();
    }

    PaintInfo(stylePainter, ptInfo, nInfoWidth);
}

void CSimpleInfoListItem::PaintIconInfo(QStylePainter & stylePainter, const QPoint & ptBegin,
                                        int nInfoWidth/* = 0*/, const char * pszDefaultIcon/* = NULL*/)
{
    QPoint ptIcon(ptBegin.x(), ptBegin.y() + DEFALUT_INFO_POS_ICON_TOP);

    PaintIcon(stylePainter, ptIcon, pszDefaultIcon);

    QPoint ptInfo(ptBegin.x() + DEFAULT_INFO_POS_ICON_WIDTH, ptBegin.y());
    if (!m_bContactPicVisible)
    {
        ptInfo.setX(ptBegin.x());
    }

    if (0 == nInfoWidth)
    {
        QRect rcItem = rect();

        nInfoWidth = rcItem.right() - ptBegin.x();
    }
    else
    {
        nInfoWidth = nInfoWidth - ptBegin.x();
    }

    PaintInfo(stylePainter, ptInfo, nInfoWidth);
}

void CSimpleInfoListItem::PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon,
                                    const char * pszDefaultIcon/* = NULL*/)
{
    QRect rcIcon(ptIcon.x(), ptIcon.y(), DEFAULT_INFO_POS_ICON_WIDTH, DEFAULT_INFO_POS_ICON_HEIGHT);

    PaintIcon(stylePainter, rcIcon, pszDefaultIcon);
}

void CSimpleInfoListItem::PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                                    const char * pszDefaultIcon/* = NULL*/)
{
    m_rcIcon = rcIcon;

    if (!m_strIconPath.empty())
    {
        QPixmap pmIcon = THEME_GET_BMP(m_strIconPath.c_str());

        if (!pmIcon.isNull())
        {
            stylePainter.drawPixmap(rcIcon, pmIcon);
            return;
        }

        QPixmap pmTmpIcon = STOCK_GET_BMP(m_strIconPath.c_str());

        if (!pmTmpIcon.isNull())
        {
            stylePainter.drawPixmap(rcIcon, pmTmpIcon);
            return;
        }
    }

    if (NULL != pszDefaultIcon)
    {
        QPixmap pmDefault = THEME_GET_BMP(pszDefaultIcon);

        if (!pmDefault.isNull())
        {
            stylePainter.drawPixmap(rcIcon, pmDefault);
        }

        return;
    }
}

void CSimpleInfoListItem::PaintInfo(QStylePainter & stylePainter, const QPoint & ptText,
                                    int nRemainWidth)
{
    nRemainWidth -= DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;

    int nLeft = ptText.x() + DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;
    int nTitleTop = ptText.y() + DEFAULT_INFO_POS_TITLE_TOP;
    int nTitleHeight = DEFAULT_INFO_POS_TITLE_HEIGHT;
    if (!m_bShowSubTitle)
    {
        nTitleTop = ptText.y();
        nTitleHeight = DEFAULT_SIMPLE_INFO_ITEM_HEIGHT;
    }
    int nTitleWidth = nRemainWidth;
    if (m_bShowInfo)
    {
        nTitleWidth -= DEFAULT_INFO_POS_INFO_WIDTH;
    }

    if (!m_bContactPicVisible)
    {
        nLeft -= DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;
    }
    QRect rcTitle(nLeft, nTitleTop, nTitleWidth, nTitleHeight);

    int nSubTitleTop = ptText.y() + DEFAULT_INFO_POS_SUBTITLE_BOTTOM - DEFAULT_INFO_POS_SUBTITLE_HEIGHT;
    int nSubTitleWidth = nRemainWidth;
    if (m_bShowInfo)
    {
        nSubTitleWidth -= DEFAULT_INFO_POS_INFO_WIDTH;
    }
    QRect rcSubTitle(nLeft, nSubTitleTop, nSubTitleWidth, DEFAULT_INFO_POS_SUBTITLE_HEIGHT);

    int nInfoTop = ptText.y();
    int nInfoLeft = nLeft + nTitleWidth + DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;
    QRect rcInfo(nInfoLeft, nInfoTop, DEFAULT_INFO_POS_INFO_WIDTH, DEFAULT_INFO_POS_INFO_HEIGHT);

    PaintInfo(stylePainter, rcTitle, rcSubTitle, rcInfo);
}

void CSimpleInfoListItem::PaintInfo(QStylePainter & stylePainter, const QRect & rcTitle,
                                    const QRect & rcSubTitle, const QRect & rcInfo,
                                    int nTitleAlign/* = Qt::AlignTop*/, int nInfoAlgin/* = Qt::AlignBottom*/)
{
    bool bEnable = isEnabled();
    const QPalette & txtPalette = palette();

    m_rcTitle = rcTitle;
    m_rcSubTitle = rcSubTitle;
    m_rcInfo = rcInfo;

    if (m_bDefineTitleColor)
    {
        stylePainter.setPen(m_TitleColor);
    }
    // 选中或按下设置为白色字体
    else if (IsDown())
    {
        stylePainter.setPen(DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR);
    }
    else
    {
        stylePainter.setPen(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR);
    }

    if (!m_strTitle.isEmpty())
    {
        stylePainter.setFont(s_fontTitle);

        if (!m_bShowSubTitle)
        {
            nTitleAlign = Qt::AlignVCenter;
        }
        //stylePainter.drawItemText(rcTitle, (Qt::AlignLeft | nTitleAlign), txtPalette, bEnable, m_strTitle);
        m_colorTextDelegate.SetIsNeedCutOff(true);
        m_colorTextDelegate.SetVAlign(Qt::AlignLeft | nTitleAlign);
        m_colorTextDelegate.PaintHighLightText(stylePainter, m_strTitle, rcTitle, HIGH_LIGHT_COLOR);
    }

    if (m_bShowSubTitle)
    {
        // 选中或按下设置为白色字体
        if (IsDown())
        {
            stylePainter.setPen(DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR);
        }
        else
        {
            stylePainter.setPen(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR);
        }

        if (!m_strSubTitle.isEmpty())
        {
            stylePainter.setFont(s_fontSubTitle);
            /*stylePainter.drawItemText(rcSubTitle, (Qt::AlignLeft | nTitleAlign), txtPalette, bEnable,
                                      m_strSubTitle);*/
            m_colorTextDelegate.SetIsNeedCutOff(true);
            m_colorTextDelegate.SetVAlign(Qt::AlignLeft | nTitleAlign);
            m_colorTextDelegate.PaintHighLightText(stylePainter, m_strSubTitle, rcSubTitle, HIGH_LIGHT_COLOR);
        }
    }

    if (m_bShowInfo)
    {
        // 选中或按下设置为白色字体
        if (IsDown())
        {
            stylePainter.setPen(DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR);
        }
        else
        {
            stylePainter.setPen(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR);
        }

        if (!m_strInfo.isEmpty())
        {
            stylePainter.setFont(s_fontInfo);
            stylePainter.drawItemText(rcInfo, (Qt::AlignLeft | Qt::AlignVCenter), txtPalette, bEnable,
                                      m_strInfo);
            //m_colorTextDelegate.SetIsNeedCutOff(true);
            //m_colorTextDelegate.SetVAlign(Qt::AlignLeft | Qt::AlignVCenter);
            //m_colorTextDelegate.PaintHighLightText(stylePainter, m_strInfo, rcInfo, HIGH_LIGHT_COLOR);
        }
    }
}
