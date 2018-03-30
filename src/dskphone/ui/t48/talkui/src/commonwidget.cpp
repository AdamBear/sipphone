//mod-this(Talk-UI)
#include "commonwidget.h"

//mod-globalInputHook
#include "keymap.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"

//mod-configParser
#include "configparser/modconfigparser.h"

#include <configiddefine.h>
#include "uikernel/qwidgetutility.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

//mod-ImageManager
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/quipushbutton.h"

#include "customtheme/modcustomtheme.h"
#include "qtcommon/xmlhelper.h"

//DTMF条自动隐藏时间
#define DTMFLINE_AUTO_HIDE_TIME     5000
#define DTMF_DELAY_TIME             400             //延时加密时间400毫秒

/////////////////////////////////////////////////////////////////////
// CItemLabel
CItemLabel::CItemLabel(QWidget * parent /* = 0 */)
    : QLabel(parent)
{
    //设置Label文字颜色为白色
    SetWgtFontColor(this, FONT_TYPE_TEXT);

    this->setFocusPolicy(Qt::StrongFocus);
    this->setObjectName(LABEL_INFO);
}

CItemLabel::~CItemLabel()
{
    //
}

void CItemLabel::SetContent(const QString & strText,
                            const yl::string & strIcon /* = */ /*, const yl::string &strPhoto = ""*/)
{
    SetLabText(strText);
    SetLabIcon(strIcon);

    //设置完成，主动更新
    update();
}

void CItemLabel::ClearContent()
{
    SetLabText("");
    SetLabIcon("");

    update();
}

void CItemLabel::SetLabText(const QString & strText, bool bMake/* = false*/)
{
    if (bMake)
    {
        m_strText = strText + ":";
    }
    else
    {
        m_strText = strText;
    }
    setText(m_strText);
    update();
}

void CItemLabel::SetLabIcon(const yl::string & strIcon /* = */)
{
    m_strIcon = strIcon;

    update();
}

void CItemLabel::paintEvent(QPaintEvent * pEvent)
{
    //
    QStylePainter painter(this);

    QRect rcPixmap;
    QPixmap pmPhoto = THEME_GET_BMP(m_strIcon.c_str());

    //绘制图标
    int nLeft = 7;
    if (!pmPhoto.isNull())
    {
        rcPixmap.setTop(height() > pmPhoto.height()
                        ? (height() - pmPhoto.height()) >> 1    : 0);
        //!!!!!
        //rcPixmap.setTop(rcPixmap.top() + 3);
        rcPixmap.setLeft(nLeft);
        rcPixmap.setHeight(pmPhoto.height());
        rcPixmap.setWidth(pmPhoto.width());

        //
        painter.drawPixmap(rcPixmap, pmPhoto);

        nLeft = rcPixmap.right() + 4;
    }

    //绘制文字
    painter.drawItemText(QRect(nLeft, 0, this->width() - nLeft,  height()),
                         Qt::AlignLeft | Qt::AlignVCenter, this->palette(), isEnabled(), m_strText);
}

////////////////////////////////////////////////////////////////////


/************************************************************************/
/*                        about class QDtmfLine                         */
/************************************************************************/
QDtmfLine::QDtmfLine(QWidget * parent)
    : QLineEdit(parent)
    , m_bIsPassword(false)
    , m_bDelayToStar(false)
    , m_pTimerAutoHide(NULL)
    , m_pTimerDelayToStar(NULL)
{
    InitWidget();
    SetDtmfLineVisible(false);
}

QDtmfLine::~QDtmfLine()
{
    if (m_pTimerAutoHide != NULL)
    {
        m_pTimerAutoHide->stop();
    }
}

//初始化控件
void QDtmfLine::InitWidget()
{
    //设置输入框相关属性
    this->setObjectName("DtmfLine");
    this->setFocusPolicy(Qt::NoFocus);
    qWidgetUtility::setFontStyle(this, 30, -1);

    //连接信号槽
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(OnEditChanged(const QString &)));

    //设置可输入值的内容
    QRegExpValidator objRegExp(QRegExp("[0-9A-D\\*#]*"), this);
    this->setValidator(&objRegExp);

    //初始化定时器
    if (m_pTimerAutoHide == NULL)
    {
        m_pTimerAutoHide = new QTimer(this);
        if (NULL != m_pTimerAutoHide)
        {
            connect(m_pTimerAutoHide, SIGNAL(timeout()), this, SLOT(OnAutoHideTimeOut()));
        }
    }

    if (m_pTimerDelayToStar == NULL)
    {
        m_pTimerDelayToStar = new QTimer(this);
        if (NULL != m_pTimerDelayToStar)
        {
            connect(m_pTimerDelayToStar, SIGNAL(timeout()), this, SLOT(OnSetToStar()));
        }
    }
}

//输入内容改变槽函数
void QDtmfLine::OnEditChanged(const QString & strEdit)
{
    // 当输入内容改变的时候,重置隐藏计时器
    StartAutoHideTimer();
}

void QDtmfLine::StartAutoHideTimer()
{
    // 数据的有效性判断
    if (m_pTimerAutoHide != NULL)
    {
        // 计算自动隐藏的时间
        int iAutoHideTime = DTMFLINE_AUTO_HIDE_TIME;

        // 启动计时器
        m_pTimerAutoHide->start(iAutoHideTime);
    }
}

void QDtmfLine::OnAutoHideTimeOut()
{
    // 停止计时器
    if (NULL != m_pTimerAutoHide)
    {
        m_pTimerAutoHide->stop();
    }

    SetDtmfLineVisible(false);
}

void QDtmfLine::OnSetToStar()
{
    TALKUI_INFO("QDtmfLine::OnSetToStar1");
    if (!m_bIsPassword)
    {
        return;
    }

    TALKUI_INFO("QDtmfLine::OnSetToStar2");
    if (m_pTimerDelayToStar != NULL)
    {
        m_pTimerDelayToStar->stop();
    }

    TALKUI_INFO("QDtmfLine::OnSetToStar3");
    //将当前字符全部设置为*
    int nLength = this->text().length();
    if (nLength > 0)
    {
        yl::string strNewText(nLength, '*');
        this->setText(toQString(strNewText.c_str()));
    }
}

//设置DTMF栏可见性
void QDtmfLine::SetDtmfLineVisible(bool bVisible, bool bRefresh/* = false*/)
{
    if (bVisible == isVisible())
    {
        //可见性一致，不设置
        return;
    }

    if (bVisible)
    {
        //在每次显示时都读取配置，判断其输入模式
        m_bIsPassword = (configParser_GetConfigInt(kszEnableHideDTMF) == 1);
        m_bDelayToStar = (configParser_GetConfigInt(kszEnableDTMFDelay) == 1);

        TALKUI_INFO("m_bIsPassword(%d)", m_bIsPassword);
        TALKUI_INFO("m_bDelayToStar(%d)", m_bDelayToStar);

        raise();

        show();

        repaint();

        //设置焦点
        this->setFocus();

        if (!bRefresh)
        {
            //启动隐藏定时器
            StartAutoHideTimer();
        }

        emit visibleChanged(true);
    }
    else
    {
        hide();
        OnAutoHideTimeOut();
        emit visibleChanged(false);
    }
}

bool QDtmfLine::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            if (!isVisible())
            {
                //不可见时直接返回，由其他进行处理
                return false;
            }

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_1:
            case PHONE_KEY_2:
            case PHONE_KEY_3:
            case PHONE_KEY_4:
            case PHONE_KEY_5:
            case PHONE_KEY_6:
            case PHONE_KEY_7:
            case PHONE_KEY_8:
            case PHONE_KEY_9:
            case PHONE_KEY_0:
            case PHONE_KEY_POUND:
            case PHONE_KEY_STAR:
                {
                    TALKUI_INFO("QDtmfLine1");
                    //DTMF按键控制
                    //这里需映射按键!!!!
                    QString strDigit(nKey);
                    if (!strDigit.isEmpty())
                    {
                        TALKUI_INFO("QDtmfLine2");
                        //根据是否加密确定输入内容
                        if (m_bIsPassword)
                        {
                            TALKUI_INFO("QDtmfLine3");
                            if (m_bDelayToStar)
                            {
                                TALKUI_INFO("QDtmfLine4");
                                if (m_pTimerDelayToStar != NULL)
                                {
                                    TALKUI_INFO("QDtmfLine5");
                                    m_pTimerDelayToStar->start(DTMF_DELAY_TIME);
                                }
                            }
                            else
                            {
                                strDigit = toQString("*");
                            }
                        }
                        this->insert(strDigit);
                    }
                    return true;
                }
                break;
            case PHONE_KEY_A:
            case PHONE_KEY_B:
            case PHONE_KEY_C:
            case PHONE_KEY_D:
#if IF_FEATURE_DTMF_FLASH
            case PHONE_KEY_E:
#endif
                {
                    //DTMF按键控制
                    //这里需映射按键!!!!
                    QString strLetter(nKey);
                    if (!strLetter.isEmpty())
                    {
                        if (m_bIsPassword)
                        {
                            if (m_bDelayToStar)
                            {
                                if (m_pTimerDelayToStar != NULL)
                                {
                                    m_pTimerDelayToStar->start(DTMF_DELAY_TIME);
                                }
                            }
                            else
                            {
                                strLetter = toQString("*");
                            }
                        }
                        this->insert(strLetter);
                    }
                    return true;
                }
                break;
            default:
                {
                    // 当DTMF条存在的时候,按其他按键需要关闭
                    SetDtmfLineVisible(false);
                }
                break;
            }
        }
        break;
    case QEvent::MouseButtonPress:
        {
            //若DTMF条显示且点击事件非产生在DTMF条上，则隐藏
            if (!isChildOf(pObject, this))
            {
                //非点击在自身子控件，则隐藏显示
                SetDtmfLineVisible(false);
            }
        }
        break;
    default:
        break;
    }

    return false;
}

///////////////////////////////////CConnectAnimation///////////////////////////////////////
#define DEFAULT_PLAY_INTERVAL 1000
//!!!!!, 控件需要重写
CConnectAnimation::CConnectAnimation(EAnimationType eAnim, QWidget * pParent /* = 0 */)
    : QWidget(pParent)
    , m_nShowIdx(0)
    , m_nInterval(0)
{
    InitData(eAnim);
}

CConnectAnimation::~CConnectAnimation()
{
    Stop();
}

void CConnectAnimation::InitData(EAnimationType eAnim)
{
    if (eAnim == eAT_Callout)
    {
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_ONE));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_TWO));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_THREE));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_FOUR));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_FIVE));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_SIX));
    }
    else if (eAnim == eAT_Callin)
    {
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_SEVEN));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_EIGHT));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_NINE));
    }
    else if (eAnim == eAT_TalkCallin)
    {
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_TALKING_COMMING_1));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_TALKING_COMMING_2));
        m_vecPixmap.push_back(loadPixmap(PIC_ANIMATION_TALKING_COMMING_3));
    }

    connect(&m_timerPlay, SIGNAL(timeout()), this, SLOT(UpdateAnimation()));
    m_nInterval = DEFAULT_PLAY_INTERVAL;
}

//设置播放速度
void CConnectAnimation::SetPlaySpeed(int nInterval)
{
    //设置指定播放速度
    m_nInterval = nInterval;
    //如果正在播放，那么需要先停止定时器
    //再用新设定速度开启定时器
    if (m_timerPlay.isActive())
    {
        m_timerPlay.stop();
        m_timerPlay.start(m_nInterval);
    }
}

//开始播放
void CConnectAnimation::Play()
{
    if (!m_timerPlay.isActive())
    {
        //开启定时器
        m_timerPlay.start(m_nInterval);
        m_nShowIdx = 1;
        update();
    }
}

QPixmap CConnectAnimation::loadPixmap(const char * path)
{
    return THEME_GET_BMP(path);
}
//停止播放
void CConnectAnimation::Stop()
{
    //停止定时器
    m_timerPlay.stop();
}

void CConnectAnimation::UpdateAnimation()
{
    if (++m_nShowIdx >= m_vecPixmap.size())
    {
        m_nShowIdx = 0;
    }

    update();
}

void CConnectAnimation::paintEvent(QPaintEvent * pEvent)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_vecPixmap[m_nShowIdx]);
}

/************************************************************************/
/*                         about IconLabel                              */
/************************************************************************/
IconLabel::IconLabel(QWidget * parent/* = 0*/)
    :   QLabel(parent)
    ,   m_sizeIcon(0, 0)
{
    m_strIcon = "";
    m_strTipsText = "";
    //设置文字颜色
    SetWgtFontColor(this, FONT_TYPE_TALK_TEXT);
}

IconLabel::~IconLabel()
{

}

void IconLabel::SetIconSize(const QSize & sizeIcon)
{
    m_sizeIcon = sizeIcon;
}

void IconLabel::SetTipsIcon(const QString & strIcon)
{
    m_strIcon = strIcon;
    update();
}

void IconLabel::SetTipsText(const QString & strTipText)
{
    m_strTipsText = strTipText;
    setText(strTipText);
    update();
}

void IconLabel::GetLayoutInfo(QRect & iconRect, QRect & textRect)
{
    //IconLabel总长度
    int nSumLenght = width();
    //IconLabel高度
    int nSumHeight = height();

    //图标
    int nIconX = 0;
    int nIconY = 0;
    int nIconLenght = m_sizeIcon.width();
    int nIconHeight = m_sizeIcon.height();

    //文字显示区域初始值
    //字符布局需要占据的长度
    //一个字符等于占据空间为13个像素
    int nTextX = 0;
    int nTextY = 0;
    int nTextLenght = 0;
    int nTextHeight = 0;
    //图标与文本间隔
    int nInterval = 2;

#ifdef _T4X
    nTextLenght = nSumLenght;
    nTextHeight = m_strIcon.isEmpty() ? 30 : 20;// 没有图标则扩大文字的显示区域
    nIconY = (nSumHeight - m_sizeIcon.height() - nInterval - textRect.height()) >> 1;
    nTextY = nIconY + nInterval + nIconHeight;
#ifdef _T48
    // 修复FarMute 图标显示的文字没居中
    if (m_sizeIcon.isEmpty())
    {
        TALKUI_INFO("m_sizeIcon: x is %d, y is %d.", m_sizeIcon.width(), m_sizeIcon.height());
        nTextY = 0;
        nTextHeight = nSumHeight;
    }
#endif
#else
    nTextLenght = m_strTipsText.size() * 13;
    nTextHeight = nSumHeight;
    //图标总共需要占据空间长度为图标长度与间隔之和
    int nIconNeedLenght = nIconLenght + nInterval;
    nIconY = (nSumHeight - m_sizeIcon.height()) >> 1;
    nTextY = 0;
#endif

    //根据类型计算布局位置
    switch (alignment() & Qt::AlignHorizontal_Mask)
    {
    case Qt::AlignLeft:
        {
            nIconX = 0;
            nTextX = 0;
        }
        break;
    case Qt::AlignRight:
        {
#ifdef _T4X
            nIconX = nSumLenght - nIconLenght;
            nTextX = nSumLenght - nTextLenght;
#else
            if (nSumLenght > (nTextLenght + nIconNeedLenght))
            {
                nIconX = nSumLenght - (nTextLenght + nIconNeedLenght);
            }
            else
            {
                nIconX = 0;
            }
#endif
        }
        break;
    case Qt::AlignHCenter:
    default:
        {
#ifdef _T4X
            nIconX = (nSumLenght - nIconLenght) >> 1;
            nTextX = (nSumLenght - nTextLenght) >> 1;
#else
            if (nSumLenght > (nTextLenght + nIconNeedLenght))
            {
                nIconX = nSumLenght / 2 - (nTextLenght + nIconNeedLenght) / 2;
            }
            else
            {
                nIconX = 0;
            }
#endif
        }
        break;
    }

    //计算非T4X文本布局坐标
#ifndef _T4X
    nTextX = nIconX + nIconNeedLenght;
#endif

    iconRect = QRect(nIconX, nIconY, nIconLenght, nIconHeight);
    textRect = QRect(nTextX, nTextY, nTextLenght, nTextHeight);
}

void IconLabel::paintEvent(QPaintEvent * paintEvent)
{
    //绘画准备
    QPainter painter(this);

    QPixmap iconPix = THEME_GET_BMP(m_strIcon.toUtf8().data());

    //解决会议中先held或者是FarHold再farmute，farmute提示文字没有居中问题
    //http://bugfree.yealink.com/Bug.php?BugID=116496
    //if (m_sizeIcon.isEmpty())
    {
        m_sizeIcon = iconPix.size();
    }

    //布局位置参数获取
    QRect iconRect, textRect;

    GetLayoutInfo(iconRect, textRect);

    //画上图标
    painter.drawPixmap(iconRect, iconPix);
    //画上文字
    painter.drawText(textRect, alignment() | Qt::AlignVCenter, LANG_PARSE(m_strTipsText));
}

//显示提示内容
void IconLabel::SetTips(const QString & strIcon, const QString & strText,
                        Qt::AlignmentFlag eFlag/* = Qt::AlignCenter*/)
{
    SetTipsIcon(strIcon);
    SetTipsText(strText);
    setAlignment(eFlag);
}

//////////////////////////////////////////////////////////////////////////
qHoldFrame::qHoldFrame(QWidget * parent)
    : m_pHoldBtn(NULL)
{
    m_pHoldBtn = new qUIPushButton(this);
    if (NULL != m_pHoldBtn)
    {
        m_pHoldBtn->setObjectName("HoldBtn");
        connect(m_pHoldBtn, SIGNAL(clicked()), this, SIGNAL(ClickHoldBtn()));
    }


    //QFont font;
    //font.setPointSize(22);
    //font.setWeight(75);
    //font.setBold(true);
    //m_labText.setFont(font);
    m_labText.setParent(this);
    m_labText.setObjectName("labText");
    //QPalette objQPalette = m_labText.palette();
    //objQPalette.setColor(QPalette::WindowText, Qt::white);
    m_labText.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    //m_labText.setPalette(objQPalette);

    CustomTheme_SetWidgetFontProperty(&m_labText, CUSTOM_FONT_ID_TALK_NORMAL_HOLDTIP, true,
                                      QFont::Bold);

    qWidgetUtility::SetBackGround(this, PIC_WIDGET_TALK_PAGE_MASK);
}

qHoldFrame::~qHoldFrame()
{

}

void qHoldFrame::SetTipInfo(const QString & strText,
                            const QString & strIcon)
{
    m_labText.setText(strText);

    if (NULL != m_pHoldBtn)
    {
        m_pHoldBtn->set3StatePic(strIcon
                                 , strIcon
                                 , strIcon);
    }

    //更新显示
    update();
}

void qHoldFrame::resizeEvent(QResizeEvent * pEvent)
{
    if (NULL != m_pHoldBtn)
    {
        m_pHoldBtn->setGeometry(210,
                                (this->height() - 20) / 2,
                                21, 23);
    }

    m_labText.setGeometry(233,
                          (this->height() - 28) / 2,
                          100, 28);
}
