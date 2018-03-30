#include "wifi_inc.h"
#include <QStylePainter>

Q_DECLARE_METATYPE(WifiAddData);

namespace
{

//#define SELECT_TYPE_T49

//****************************bluetooth defiene begin
#ifdef SELECT_TYPE_T49
#define SETTING_ITEM_HEIGHT 98  //item的高度
#define SETTING_ITEM_LABEL_HEIGHT 58
#else
#define SETTING_ITEM_HEIGHT 47  //item的高度
#define SETTING_ITEM_LABEL_HEIGHT 24
#endif

#define DEFAULT_INFO_POS_ICON_LEFT              0
#ifdef SELECT_TYPE_T49
#define DEFAULT_INFO_POS_ICON_WIDTH             58
#define DEFAULT_INFO_POS_ICON_HEIGHT            58

#define DEFALUT_INFO_POS_TEXT_LEFT_MARGIN       22
#define DEFAULT_INFO_POS_NAME_TOP               21
#define DEFAULT_INFO_POS_NAME_HEIGHT            33
#define DEFAULT_INFO_POS_INFO_BOTTOM            58
#define DEFAULT_INFO_POS_INFO_HEIGHT            26

#define DEFAULT_INFO_WIDTH                      300
#else
#define DEFAULT_INFO_POS_ICON_WIDTH             30
#define DEFAULT_INFO_POS_ICON_HEIGHT            30

#define DEFALUT_INFO_POS_TEXT_LEFT_MARGIN       10
#define DEFAULT_INFO_POS_NAME_TOP               6
#define DEFAULT_INFO_POS_NAME_HEIGHT            30
#define DEFAULT_INFO_POS_INFO_BOTTOM            47
#define DEFAULT_INFO_POS_INFO_HEIGHT            26//30

#define DEFAULT_INFO_WIDTH                      200
#endif
#define DEFALUT_INFO_POS_ICON_TOP               ((SETTING_ITEM_HEIGHT - DEFAULT_INFO_POS_ICON_HEIGHT)>>1)


//wifi Detail
#define DEFAULT_DETAIL_POS_ICON_RIGHT_OFFSET    52
#ifdef SELECT_TYPE_T49
#define DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH   37
#define DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT  37
#else
#define DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH   30
#define DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT  30
#endif
#define DEFAULT_CONTACT_POS_ICON_DETAIL_TOP     ((SETTING_ITEM_HEIGHT - DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT)>>1)

// wifi note rect
#ifdef SELECT_TYPE_T9
const QRect RECT_WIFI_NOTE(475, 20, 270, SETTING_ITEM_LABEL_HEIGHT);
#else
const QRect RECT_WIFI_NOTE(265, 10, 200, 30);
#endif
// wifi device nums background rect
const QRect RECT_WIFI_DEVICE_NUM_BG(0, 0, 836, 86);

#ifdef SELECT_TYPE_T49
#define DEFAULT_INFO_TEXT_FONT_SIZE_NAME        28
#define DEFAULT_INFO_TEXT_FONT_SIZE_MAC         23
#else
#define DEFAULT_INFO_TEXT_FONT_SIZE_NAME        16
#define DEFAULT_INFO_TEXT_FONT_SIZE_MAC         13
#endif

#define DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR QColor(0, 0, 0)
//****************************bluetooth defiene end

// wifi search and statistics rect
const QRect RECT_WIFI_SEARCH_STATISTICS_NOTE(32, 14, 800, SETTING_ITEM_LABEL_HEIGHT);
// wifi search background rect
const QRect RECT_WIFI_SEARCH_BACKGROUND(2, 0, 836, 86);
// wifi device press background rect
const QRect RECT_WIFI_DEVICE_PRESS_BACKGROUND(2, 0, 836, 100);

// signal strong Icon
const char * g_pWifiIcon[] = {PIC_WIFI_SIGNAL_0, PIC_WIFI_SIGNAL_1, PIC_WIFI_SIGNAL_2, PIC_WIFI_SIGNAL_3, PIC_WIFI_SIGNAL_4};
#ifdef SELECT_TYPE_T49
const char * g_pWifiSuccessIcon[] = {PIC_WIFI_SIGNAL_0, PIC_WIFI_CONNECT_SIGNAL_1, PIC_WIFI_CONNECT_SIGNAL_2, PIC_WIFI_CONNECT_SIGNAL_3, PIC_WIFI_CONNECT_SIGNAL_4};
#else
const char * g_pWifiSuccessIcon[] = {PIC_WIFI_PRESS, PIC_WIFI_PRESS, PIC_WIFI_PRESS, PIC_WIFI_PRESS, PIC_WIFI_PRESS};
#endif

// 安全模式（是否有密码）
const char * strSecurity[2] = { "open", "secure", };
}

CSettingWifiItem::CSettingWifiItem(QWidget * parent, QString strAction, QString sDisName, int iSecu,
                                   int iID
                                   , std::string sNote, int iDataIdx /*= -1*/, bool bAddIdx /*= false*/, bool bAddNote /*= false*/)
    : CSettingItem(parent, "", NULL, strAction, iDataIdx, bAddIdx)
    , m_DetailBtn(this)
    , m_iLeftIcon(0)
    , m_iExistOrNot(0)
    , m_pLblNote(NULL)
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_WIFI;

    m_WifiData.strDisName = sDisName;
    m_WifiData.iSecurity = iSecu;
    m_WifiData.iNetID = iID;
    m_WifiData.strNote = sNote;
    m_WifiData.bShowNote = bAddNote;

    m_pLblNote = new NoteLabel(this);
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(INT_FOREVER);
        m_pLblNote->SetLabelText("");
    }
}

CSettingWifiItem::CSettingWifiItem(QWidget * parent, QWidgetPair & pair, bool bAddIdx /*= false*/)
    : CSettingItem(parent, pair.first, pair.second, pair.third, pair.iIndex, false)
    , m_DetailBtn(this)
    , m_iExistOrNot(0)
    , m_pLblNote(NULL)
{
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_nType |= LIST_ITEM_SUB_TYPE_SETTING_WIFI;

    m_WifiData = pair.appendVariantData.value<WifiAddData>();

    m_pLblNote = new NoteLabel(this);
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(m_WifiData.eType);
        m_pLblNote->SetLabelText(m_WifiData.strNote.c_str());
    }
}

CSettingWifiItem::~CSettingWifiItem(void)
{

}

void CSettingWifiItem::Relayout()
{
    if (NULL != m_pLblNote)
    {
        QPalette pal = m_pLblNote->palette();
        QRect rcNote(RECT_WIFI_NOTE);

        pal = m_pLblNote->palette();
        pal.setBrush(QPalette::Foreground, IsDown() ? (Qt::white) : (Qt::black));
        m_pLblNote->setPalette(pal);
        m_pLblNote->setGeometry(rcNote);
    }
}

void CSettingWifiItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
    ApplyStyleByState(GetState());
#endif

#ifdef _T49
    // paint background
    if (IsDown())
    {
        QPixmap pmIcon = THEME_GET_BMP(PIC_BLUE_CONNECT_CLICK);
        if (!pmIcon.isNull())
        {
            stylePainter.drawPixmap(RECT_WIFI_DEVICE_PRESS_BACKGROUND, pmIcon);
        }
    }
#endif

    QRect rcItem = rect();
    QPoint ptBegin(rcItem.left(), rcItem.top());

#ifdef FL_SUPPORT_CUSTOM_ITEM_STYLE
    if (NULL != m_pStyle)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pStyle->GetContentPos(nLeftMargin, nRightMargin);

        ptBegin.setX(ptBegin.x() + nLeftMargin);
    }
#endif
#ifndef _T49
    if (NULL != m_pDelegate)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);

        ptBegin.setX(ptBegin.x() + nLeftMargin);
    }
#endif

    PaintDefault(stylePainter, ptBegin);

    if (NULL != m_pLblNote)
    {
        QPalette pal = m_pLblNote->palette();
        QRect rcNote(RECT_WIFI_NOTE);
        pal = m_pLblNote->palette();
        pal.setBrush(QPalette::Foreground, IsDown() ? (Qt::white) : (Qt::black));
        m_pLblNote->setPalette(pal);
    }
}

void CSettingWifiItem::PaintDefault(QStylePainter & stylePainter, const QPoint & ptBegin,
                                    int nInfoWidth/* = 0*/, const char * pszDefaultIcon/* = NULL*/)
{
    QPoint ptIcon(ptBegin.x() + DEFAULT_INFO_POS_ICON_LEFT, ptBegin.y());

    PaintIconInfo(stylePainter, ptIcon, nInfoWidth, pszDefaultIcon);
}

void CSettingWifiItem::PaintDefault(QStylePainter & stylePainter, QPixmap & pmIcon,
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

void CSettingWifiItem::PaintIconInfo(QStylePainter & stylePainter, const QPoint & ptBegin,
                                     int nInfoWidth/* = 0*/, const char * pszDefaultIcon/* = NULL*/)
{
    QPoint ptIcon(ptBegin.x(), ptBegin.y() + DEFALUT_INFO_POS_ICON_TOP);

    bool bDrawLeftIcon = true;
    if (m_WifiData.iLeftIcon < 0 || m_WifiData.iLeftIcon > 4)
    {
        bDrawLeftIcon = false;
    }
    else
    {
        PaintIcon(stylePainter, ptIcon, pszDefaultIcon);
    }
    if (m_WifiData.bShowRightBtnIcon)
    {
        PaintDetailButton(stylePainter, this->rect().right());
    }

    QPoint ptInfo(ptBegin.x() + (bDrawLeftIcon ? DEFAULT_INFO_POS_ICON_WIDTH : 0), ptBegin.y());

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

void CSettingWifiItem::PaintDetailButton(QStylePainter & stylePainter, int nRight)
{
    QRect rcIcon(nRight - DEFAULT_DETAIL_POS_ICON_RIGHT_OFFSET, DEFAULT_CONTACT_POS_ICON_DETAIL_TOP,
                 DEFAULT_CONTACT_POS_ICON_DETAIL_WIDTH, DEFAULT_CONTACT_POS_ICON_DETAIL_HEIGHT);

    // Move Btn点击区域太小，再向左扩大30
    m_DetailBtn.SetRect(rcIcon.x() - 30, 0, rcIcon.x() + 60, this->height());
#ifdef _T49
    if (m_WifiData.bAutoPair)
#else
    if (IsDown())
#endif
    {
        m_DetailBtn.PaintButton(stylePainter, PIC_WIFI_MORE_WHITE, PIC_WIFI_MORE_PRESS, NULL, rcIcon);
    }
    else
    {
        m_DetailBtn.PaintButton(stylePainter, PIC_WIFI_MORE, PIC_WIFI_MORE_PRESS, NULL, rcIcon);
    }
}

void CSettingWifiItem::PaintIcon(QStylePainter & stylePainter, const QPoint & ptIcon,
                                 const char * pszDefaultIcon/* = NULL*/)
{
    QRect rcIcon(ptIcon.x(), ptIcon.y(), DEFAULT_INFO_POS_ICON_WIDTH, DEFAULT_INFO_POS_ICON_HEIGHT);

    PaintIcon(stylePainter, rcIcon, pszDefaultIcon);
}

void CSettingWifiItem::PaintIcon(QStylePainter & stylePainter, const QRect & rcIcon,
                                 const char * pszDefaultIcon/* = NULL*/)
{
#ifdef _T49
    QPixmap pmIcon = THEME_GET_BMP(m_WifiData.bConnectDevice ? g_pWifiSuccessIcon[m_WifiData.iLeftIcon]
                                   : g_pWifiIcon[m_WifiData.iLeftIcon]);
#else
    QPixmap pmIcon = THEME_GET_BMP(IsDown() ? g_pWifiSuccessIcon[m_WifiData.iLeftIcon] :
                                   g_pWifiIcon[m_WifiData.iLeftIcon]);
#endif
    if (!pmIcon.isNull())
    {
        stylePainter.drawPixmap(rcIcon, pmIcon);
        return ;
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

void CSettingWifiItem::PaintInfo(QStylePainter & stylePainter, const QPoint & ptText,
                                 int nRemainWidth)
{
    nRemainWidth -= DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;

    int nLeft = ptText.x() + DEFALUT_INFO_POS_TEXT_LEFT_MARGIN;
    int nTitleTop = ptText.y() + DEFAULT_INFO_POS_NAME_TOP;
    QRect rcTitle(nLeft, nTitleTop, DEFAULT_INFO_WIDTH, DEFAULT_INFO_POS_NAME_HEIGHT);

    int nInfoTop = ptText.y() + DEFAULT_INFO_POS_INFO_BOTTOM - DEFAULT_INFO_POS_INFO_HEIGHT;
    QRect rcInfo(nLeft, nInfoTop, DEFAULT_INFO_WIDTH, DEFAULT_INFO_POS_INFO_HEIGHT);

    QRect rcNote(265, 10, 200, 30);

    PaintInfo(stylePainter, rcTitle, rcInfo, rcNote);
}

void CSettingWifiItem::PaintInfo(QStylePainter & stylePainter, const QRect & rcTitle,
                                 const QRect & rcInfo, const QRect & rcNote,
                                 int nTitleAlign/* = Qt::AlignTop*/, int nInfoAlgin/* = Qt::AlignBottom*/,
                                 int nNoteAlgin/* = Qt::AlignLeft*/)
{
    bool bEnable = isEnabled();
    const QPalette & txtPalette = palette();
    QFont fontText = stylePainter.font();
    // 按下时设置反白
#ifndef SELECT_TYPE_T49
    // 按下时设置反白
    if (IsDown())
    {
        stylePainter.setPen(DEFAULT_SIMPLE_INFO_SELECT_TEXT_COLOR);
    }
    else
#endif
    {
        stylePainter.setPen(DEFAULT_SIMPLE_INFO_NORMAL_TEXT_COLOR);
    }

    // draw display name
    if (!m_WifiData.strDisName.isEmpty())
    {
        fontText.setPointSize(DEFAULT_INFO_TEXT_FONT_SIZE_NAME);
        fontText.setWeight(QFont::Bold);
        fontText.setBold(false);
        stylePainter.setFont(fontText);

        stylePainter.drawItemText(rcTitle, (Qt::AlignLeft | nTitleAlign), txtPalette, bEnable,
                                  m_WifiData.strDisName);
    }

    // draw security mode
    fontText.setPointSize(DEFAULT_INFO_TEXT_FONT_SIZE_MAC);
    fontText.setWeight(QFont::Light);
    fontText.setBold(false);
    stylePainter.setFont(fontText);
    int iSecureOrNot = m_WifiData.iSecurity != 0 ? 1 : 0;
    stylePainter.drawItemText(rcInfo, (Qt::AlignLeft | nInfoAlgin), txtPalette, bEnable,
                              strSecurity[iSecureOrNot]);
}

/*************************************** 处理点击 ***************************************/
void CSettingWifiItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!m_DetailBtn.ProcessMousePress(pEvent))
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }
    }

    pEvent->accept();
}

void CSettingWifiItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    bool bDetailDown = false;
    bool bDetailClick = false;

    bDetailDown = m_DetailBtn.IsDown();
    bDetailClick = m_DetailBtn.ProcessMouseRelease(pEvent);

    if (!bDetailClick && !ProcessMouseRelease(pEvent, true))
    {
        if (bDetailDown)
        {
            m_DetailBtn.ResetDown();
        }
        return;
    }

    if (bDetailClick)
    {
        DoAction(LIST_ITEM_WIFI_BTN_CLICK);
    }
}

int CSettingWifiItem::GetNetID()
{
    return m_WifiData.iNetID;
}

const QString CSettingWifiItem::GetDisName()
{
    return m_WifiData.strDisName;
}

void CSettingWifiItem::SetItemData(QWidgetPair qPair)
{
    SetClickAction(qPair.third);
    m_WifiData = qPair.appendVariantData.value<WifiAddData>();
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(m_WifiData.eType);
        m_pLblNote->SetLabelText(m_WifiData.strNote.c_str());
        m_pLblNote->show();
    }

    update();
}

void CSettingWifiItem::SetNoteDisplayAndType(const QString & strDisplay, Item_Note_Type eType)
{
    m_WifiData.strNote = strDisplay.toUtf8().data();
    m_WifiData.eType = eType;
    if (NULL != m_pLblNote)
    {
        m_pLblNote->SetLabelType(eType);
        m_pLblNote->SetLabelText(strDisplay);
        m_pLblNote->show();
    }
}

// set connected or not
void CSettingWifiItem::SetConnected(bool bConnected)
{
    m_WifiData.bConnectDevice = bConnected;
    update();
}

