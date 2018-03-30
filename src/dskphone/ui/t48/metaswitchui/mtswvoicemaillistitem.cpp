#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "mtswvoicemaillistitem.h"
#include "baseui/fontcolor.h"
#include "interfacedefine.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/ilistitemdelegate.h"
#include "baseui/t4xpicpath.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"

namespace
{
#define DEFAULT_COLUMN_SAPCING 8
#define DEFAULT_ICON_LABEL_SAPCING 8
#define DISPLAY_MARGIN_BETWENN 1

#define SETTING_WIGET_DEFAUT_X 300

//左侧图标位置
#define LEFT_ICON_HEIGHT 30
#define LEFT_ICON_WIDTH 30
//显示标签位置
#define DISPLAY_LABEL_TOP   6
#define DISPLAY_LABEL_HEIGHT    20
//通话时长位置
#define DISPLAY_DURATION_TOP    30
#define DISPLAY_DURATION_HEIGHT 20
//时长显示
#define DISPLAY_TIME_LEFT   340
#define DISPLAY_TIME_WIDTH  100
//进度条位置
#define PROGRESS_BAR_WIDTH  371//366
#define PROGRESS_BAR_HIGHT  20//4
#define PROGRESS_BAR_TOP    7//18
//标志label宽度
#define DISPLAY_URGENT_WIDTH    16
// More位置信息
// More按钮离右侧的偏移位置
#define DEFAULT_POS_BTN_MORE_X_OFFSET 72
#define DEFAULT_POS_BTN_MORE_TOP 1
#define DEFAULT_POS_BTN_MORE_WIDTH 72
#define DEFAULT_POS_BTN_MORE_HEIGHT 53
// More图标离右侧的偏移位置
#define DEFAULT_POS_ICON_MORE_X_OFFSET 51
#define DEFAULT_POS_ICON_MORE_TOP 13
#define DEFAULT_POS_ICON_MORE_WIDTH 30
#define DEFAULT_POS_ICON_MORE_HEIGHT 30

const QColor SETTING_ITEM_READ_COLOR = Qt::black;
const QColor SETTING_ITEM_UNREAD_COLOR = Qt::blue;
const QColor SETTING_MESSAGE_ITEM_NORMAL_COLOR = Qt::black;
const QColor SETTING_MESSAGE_ITEM_URGENT_COLOR = Qt::red;
}

CVoiceMailListItem::CVoiceMailListItem(QWidget * parent, QString strIcon, QString strLabel,
                                       QString strAction /*= ""*/, bool bAddIcon/* = false*/
                                       , int iDataIndex/* = -1*/, bool bAddInex/* = true*/, bool bWithStatus/* = false*/)
    : CSettingItem(parent)
    , m_btnMore(this)
    , m_ProgressBar(this)
    , m_bAddIcon(bAddIcon)
    , m_bWithStatus(bWithStatus)
    , m_bRead(false)
    , m_bPlayStatus(false)
    , m_bShowUrgent(false)
    , m_pPlayTimer(NULL)
{
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    // 设置是否有选中效果
    SetChangeStyleOnSelect(true);
#endif
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);
#endif

    m_bAddIndex = bAddInex;
    m_pWgt = NULL;
    m_pLabDate = NULL;
    m_pLabTime = NULL;
    m_pLabUrgent = NULL;

    if (m_bAddIcon)
    {
        yl::string strIconPath = strIcon.toUtf8().data();
        m_iconMap = THEME_GET_BMP(strIconPath.c_str());
    }
    SetLabText(strLabel);
    m_strAction = strAction;
    SetDataIndex(iDataIndex);
    GenerateLabelText();

    Init();
}

CVoiceMailListItem::CVoiceMailListItem(QWidget * pParent, QWidgetPair & pair)
    : CSettingItem(pParent, pair.first, pair.second, pair.third, pair.iIndex, pair.bAddIndex, false)
    , m_btnMore(this)
    , m_ProgressBar(this)
    , m_bAddIcon(pair.bAddIcon)
    , m_bWithStatus(pair.bWithStatus)
    , m_bRead(false)
    , m_bPlayStatus(false)
    , m_bShowUrgent(false)
    , m_pPlayTimer(NULL)
{
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    // 设置是否有选中效果
    SetChangeStyleOnSelect(false);
#endif
#ifdef  QT_TOUCH_SUPPORT
    // 设置点击效果
    SetChangeStyleOnPress(true);

    SetClickBg(PIC_GENERAL_LIST_ITEM_CLICK_BG);
#endif

    if (m_bAddIcon)
    {
        yl::string strIconPath = pair.strIcon;
        m_iconMap = THEME_GET_BMP(strIconPath.c_str());
    }

    Init();
}

void CVoiceMailListItem::Init()
{
    ResetTime();

    //初始化计时器
    m_pPlayTimer = new QTimer(this);
    if (NULL != m_pPlayTimer)
    {
        //连接超时处理函数
        connect(m_pPlayTimer, SIGNAL(timeout()), this, SLOT(OnPlayTimeOut()));
    }

    m_pLabDate = new QLabel(this);

    if (NULL != m_pLabDate)
    {
        QPalette palLabDisp = m_pLabDate->palette();
        palLabDisp.setColor(QPalette::Text, SETTING_MESSAGE_ITEM_NORMAL_COLOR);
        m_pLabDate->setPalette(palLabDisp);
    }

    m_pLabTime = new QLabel(this);

    if (NULL != m_pLabTime)
    {
        QPalette palLabDisp = m_pLabTime->palette();
        palLabDisp.setColor(QPalette::Text, SETTING_MESSAGE_ITEM_NORMAL_COLOR);
        m_pLabTime->setPalette(palLabDisp);
    }

    m_pLabUrgent = new QLabel(this);

    if (NULL != m_pLabUrgent)
    {
        QPalette palLabDisp = m_pLabUrgent->palette();
        palLabDisp.setColor(QPalette::Text, SETTING_MESSAGE_ITEM_URGENT_COLOR);
        palLabDisp.setBrush(QPalette::Foreground, QBrush(SETTING_MESSAGE_ITEM_URGENT_COLOR));
        m_pLabUrgent->setPalette(palLabDisp);
        m_pLabUrgent->setText(" ! ");
    }

    m_ProgressBar.setParent(this);
    m_ProgressBar.SetImage(PIC_USB_AUDIO_PROCESSBAR_EMPTY, PIC_USB_AUDIO_PROCESSBAR_FULL,
                           PIC_USB_AUDIO_PROGRESSBAR_CURPOS);
    //设置焦点代理
    m_ProgressBar.setFocusProxy(this);
    //设置步长
    m_ProgressBar.SetStep(1);
    m_ProgressBar.hide();
    m_ProgressBar.SetReadOnly(true);
}

void CVoiceMailListItem::ClearContent()
{
    m_btnMore.ResetDown();

    CSettingItem::ClearContent();
}

void CVoiceMailListItem::AddIcon(LPCSTR strIconPath)
{
    if (strcmp(strIconPath, "") == 0)
    {
        return;
    }

    m_iconMap = THEME_GET_BMP(strIconPath);
    if (m_iconMap.isNull())
    {
        return;
    }

    m_bAddIcon = true;
}

CVoiceMailListItem::~CVoiceMailListItem()
{
    //
}

void CVoiceMailListItem::ApplyNomalStyle()
{
    QPalette pal;

    const QColor colorPress = SETTING_MESSAGE_ITEM_NORMAL_COLOR;

    if (NULL != m_pWgt)
    {
        pal = m_pWgt->palette();
        pal.setBrush(QPalette::Text, QBrush(colorPress));
        pal.setBrush(QPalette::WindowText, QBrush(colorPress));
        m_pWgt->setPalette(pal);
    }

    QColor color = SETTING_ITEM_UNREAD_COLOR;
    if (m_bRead)
    {
        color = SETTING_ITEM_READ_COLOR;
    }

    if (NULL != m_pLabDisplay)
    {
        pal = m_pLabDisplay->palette();
        pal.setBrush(QPalette::Foreground, QBrush(color));
        m_pLabDisplay->setPalette(pal);
    }
}

void CVoiceMailListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
//  PaintFocusBg(stylePainter);
#ifdef  QT_TOUCH_SUPPORT
    PaintDownBg(stylePainter, m_strClickBg.empty() ? NULL : m_strClickBg.c_str());
#endif
    ApplyStyleByState(GetState());

    int nLeftBegin = 0;
    int nRightEnd = 0;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }
    if (m_bAddIcon && !m_iconMap.isNull())
    {
        QSize iconSize = m_iconMap.size();
        stylePainter.drawPixmap(nLeftBegin, (rect().height() - LEFT_ICON_HEIGHT) / 2
                                , iconSize.width() < LEFT_ICON_WIDTH ? iconSize.width() : LEFT_ICON_WIDTH
                                , iconSize.height() < LEFT_ICON_HEIGHT ? iconSize.height() : LEFT_ICON_HEIGHT, m_iconMap);
    }

    QRect rcItem = rect();
    int nbtnEnterLeft = rcItem.right() - DEFAULT_POS_ICON_MORE_X_OFFSET;
    int nTop = rcItem.top() + DEFAULT_POS_ICON_MORE_TOP;
    QRect rcBtnMore(nbtnEnterLeft, nTop, DEFAULT_POS_ICON_MORE_WIDTH, DEFAULT_POS_ICON_MORE_HEIGHT);
    if (m_bPlayStatus)
    {
        m_btnMore.PaintButton(stylePainter, PIC_USB_BROWSE_ITEM_STOP, PIC_USB_BROWSE_ITEM_STOP_PRESS, NULL,
                              rcBtnMore);
    }
    else
    {
        m_btnMore.PaintButton(stylePainter, PIC_BLUE_MORE, PIC_BLUE_MORE_PRESS, NULL, rcBtnMore);
    }
}

void CVoiceMailListItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!m_btnMore.ProcessMousePress(pEvent))
    {
        if (!ProcessMousePress(pEvent))
        {
            return;
        }
    }

    pEvent->accept();
}

void CVoiceMailListItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    bool bMoreDown = false;
    bool bMoreClick = false;

    bMoreDown = m_btnMore.IsDown();
    bMoreClick = m_btnMore.ProcessMouseRelease(pEvent);

    if (!bMoreClick)
    {
        if (bMoreDown)
        {
            m_btnMore.ResetDown();
        }

        if (!ProcessMouseRelease(pEvent, true))
        {
            return;
        }
    }

    if (bMoreClick)
    {
        if (m_bPlayStatus)
        {
            ResetTime();
            StopPlayTimer();
            DoAction(MTSW_VOICE_MAIL_STOP);
        }
        else
        {
            DoAction(MTSW_VOICE_MAIL_MORE);
        }
    }
}

void CVoiceMailListItem::Relayout()
{
    QRect rcItem = rect();

    int nLeftBegin = 0;
    int nRightEnd = 0;
    int nbtnEnterLeft = rcItem.right() - DEFAULT_POS_BTN_MORE_X_OFFSET;

    if (NULL != m_pDelegate)
    {
        m_pDelegate->GetContentPos(nLeftBegin, nRightEnd);
    }

    if (m_bAddIcon && (!m_iconMap.isNull()))
    {
        nLeftBegin += LEFT_ICON_WIDTH + DEFAULT_ICON_LABEL_SAPCING;
    }

    m_pLabDisplay->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    if (NULL != m_pWgt)
    {
        if (m_pLabDisplay)
        {
            if (m_bShowUrgent && !m_bPlayStatus)
            {
                m_pLabUrgent->setGeometry(nLeftBegin, DISPLAY_LABEL_TOP + DISPLAY_MARGIN_BETWENN
                                          , DISPLAY_URGENT_WIDTH, DISPLAY_LABEL_HEIGHT);
                m_pLabDisplay->setGeometry(nLeftBegin + DISPLAY_URGENT_WIDTH,
                                           DISPLAY_LABEL_TOP + DISPLAY_MARGIN_BETWENN
                                           , SETTING_WIGET_DEFAUT_X - DEFAULT_COLUMN_SAPCING - nLeftBegin, DISPLAY_LABEL_HEIGHT);

                m_pLabUrgent->show();
            }
            else
            {
                m_pLabUrgent->hide();
                m_pLabDisplay->setGeometry(nLeftBegin, DISPLAY_LABEL_TOP + DISPLAY_MARGIN_BETWENN
                                           , SETTING_WIGET_DEFAUT_X - DEFAULT_COLUMN_SAPCING - nLeftBegin, DISPLAY_LABEL_HEIGHT);
            }
        }

        if (m_pWgt->inherits("QLabel"))
        {
            ((QLabel *)m_pWgt)->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            if (m_bPlayStatus)
            {
                m_ProgressBar.setRange(0, Time2Second(((QLabel *)m_pWgt)->text()));
            }
        }

        QRect rcWgt(nLeftBegin, DISPLAY_DURATION_TOP - DISPLAY_MARGIN_BETWENN, DISPLAY_TIME_WIDTH
                    , DISPLAY_DURATION_HEIGHT);
        m_pWgt->setGeometry(rcWgt);

        if (m_pLabDate)
        {
            m_pLabDate->setGeometry(DISPLAY_TIME_LEFT, DISPLAY_LABEL_TOP + DISPLAY_MARGIN_BETWENN
                                    , DISPLAY_TIME_WIDTH, DISPLAY_LABEL_HEIGHT);
        }

        if (m_pLabTime)
        {
            m_pLabTime->setGeometry(DISPLAY_TIME_LEFT, DISPLAY_DURATION_TOP - DISPLAY_MARGIN_BETWENN,
                                    DISPLAY_TIME_WIDTH
                                    , DISPLAY_DURATION_HEIGHT);
        }

        if (m_bPlayStatus)
        {
            yl::string strPlayTime = MTSWVM_FormatDuration(m_nPlayTime);
            SetTime(strPlayTime.c_str());
            m_ProgressBar.setValue(m_nPlayTime);
            m_ProgressBar.setGeometry(nLeftBegin - 5
                                      , PROGRESS_BAR_TOP
                                      , PROGRESS_BAR_WIDTH
                                      , PROGRESS_BAR_HIGHT);

            m_pLabDisplay->hide();
            m_pLabDate->hide();
            m_ProgressBar.show();
        }
        else
        {
            m_pLabDisplay->show();
            m_pLabDate->show();
            m_ProgressBar.hide();
        }
    }
    else
    {
        if (m_pLabDisplay)
        {
            m_pLabDisplay->setGeometry(nLeftBegin, DISPLAY_LABEL_TOP
                                       , nbtnEnterLeft - DEFAULT_COLUMN_SAPCING - nLeftBegin, DISPLAY_LABEL_HEIGHT);
        }
    }

    QRect rcBtnMore(nbtnEnterLeft, DEFAULT_POS_BTN_MORE_TOP, DEFAULT_POS_BTN_MORE_WIDTH,
                    DEFAULT_POS_BTN_MORE_HEIGHT);
    m_btnMore.SetRect(rcBtnMore);
}

void CVoiceMailListItem::SetDate(QString strDate)
{
    m_pLabDate->setText(strDate);
}

void CVoiceMailListItem::SetTime(QString strTime)
{
    m_pLabTime->setText(strTime);
}


void CVoiceMailListItem::StartPlayTimer()
{
    if (NULL != m_pPlayTimer)
    {
        m_pPlayTimer->start(1000);
    }
}

void CVoiceMailListItem::StopPlayTimer()
{
    if (NULL != m_pPlayTimer)
    {
        m_pPlayTimer->stop();
    }
}

void CVoiceMailListItem::OnPlayTimeOut()
{
    m_nPlayTime++;

    if (m_nPlayTime > MTSWVM_GetDuration())
    {
        ResetTime();
        StopPlayTimer();
        DoAction(MTSW_VOICE_MAIL_FINISH);
        return;
    }

    if (!m_bPlayStatus)
    {
        return;
    }

    Relayout();
}


int CVoiceMailListItem::Time2Second(QString strTime)
{
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;
    sscanf((char *)strTime.toUtf8().data(), "%02d:%02d:%02d", &iHour, &iMinute, &iSecond);
    int iSecondTotal = iHour * 3600 + iMinute * 60 + iSecond;
    return iSecondTotal;
}
#endif