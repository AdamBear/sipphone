#include "inputbutton.h"
#include "qtcommon/qmisc.h"

// 字体信息
#define ENTERBTN_TEXT_FONT_SIZE          26
#define ENTERBTN_TEXT_FONT_SIZE_PRESS    26

CInputButton::CInputButton(QWidget* v_parent):
    QObject(v_parent),
    m_pWidget(NULL),
    m_qstrNormalText(""),
    m_qstrPressText(""),
    m_qColorPressText(Qt::black),
    m_qColorNormalText(Qt::black),
#ifndef IF_LYNC
    m_strNormalcon(""),
    m_strPressIcon("")
#else
    m_qstrPressIcon("")
#endif
    , m_rctButton(QRect())
    , m_strNormalIconPath("")
    , m_nBtnAction(0)
    , m_nIconAlign(0)
    , m_bInputButtonDown(false)
    , m_bIsResponePress(true)
{
    //设置一个默认的背景色
//  QPalette ple                    = this->palette();
//  QBrush brush                    = ple.background();
    m_qColorNormalBackGround        = QColor(37, 37, 37);
    m_qColorPressBackGround         = QColor(255, 255, 255);

//  grabGesture(Qt::TapAndHoldGesture);
}

CInputButton::~CInputButton(void)
{
}

void CInputButton::SetNormalText(const QString &qstrText)
{
    m_qstrNormalText            = qstrText;
    m_qstrPressText             = qstrText;
}

void CInputButton::SetPressText(const QString &qstrText)
{
    m_qstrPressText             = qstrText;
}

void CInputButton::setNormalTextColor(const QColor& clr)
{
    //m_clrText = clr;
    m_qColorNormalText          = clr;
}

void CInputButton::SetPressTextColor(const QColor& clr)
{

    // 设置属性和本身属性一样时不进行设置
    if (clr != m_qColorPressText)
    {
        m_qColorPressText = clr;
    }
}

void CInputButton::SetNormalBackGround(const QColor& clr)
{
    //设置透明可以把北京颜色设置为QColor(0, 0, 0, 0)

    m_qColorNormalBackGround    = clr;

    m_qColorPressBackGround     = clr;
}

void CInputButton::SetPressBackGround(const QColor& clr)
{
    // 设置属性和本身属性一样时不进行设置
    if (clr != m_qColorPressBackGround)
    {
        m_qColorPressBackGround = clr;
    }
}

#ifndef IF_LYNC
void CInputButton::SetPressIcon(QString qstrPressIcon)
{
    m_strPressIcon = qstrPressIcon;
}

void CInputButton::SetNormalIcon(QString qstrNormalIcon)
{
    m_strNormalcon = qstrNormalIcon;
}

#else
void CInputButton::SetPressIcon(QString qstrPressIcon)
{
    m_qstrPressIcon                 = qstrPressIcon;

    if (m_qstrPressIcon.isNull() || m_qstrPressIcon.isEmpty())
    {
        //设置为空
        m_pmPressIcon               = m_pmNull;
        return ;
    }

    m_pmPressIcon.load(m_qstrPressIcon.toUtf8().data());
}

void CInputButton::SetPressIconQpixmap(QPixmap pixmap)
{
    m_pmPressIcon = pixmap;
}
#endif

void CInputButton::paintInputButton(QStylePainter& stylePainter)
{
#ifndef IF_LYNC
    if (m_bInputButtonDown) //按下效果
    {
        stylePainter.fillRect(m_rctButton, m_qColorPressBackGround);

        if (!m_strPressIcon.isEmpty())
        {
            QPixmap pmIcon = THEME_GET_BMP(m_strPressIcon.toUtf8().data());
            if (!pmIcon.isNull())
            {
                stylePainter.drawPixmap(m_rctButton.x() + m_rctButton.width() / 2 - pmIcon.width() / 2,
                                        m_rctButton.y() + m_rctButton.height() / 2 - pmIcon.height() / 2, pmIcon);
            }
        }
        else if ("" != m_qstrPressText)
        {
            QFont fontName;
            QPen pen = stylePainter.pen();
            pen.setColor(m_qColorPressText);
            stylePainter.setPen(pen);
            fontName.setPointSize(ENTERBTN_TEXT_FONT_SIZE_PRESS);
            stylePainter.setFont(fontName);
            if (NULL != m_pWidget)
            {
                stylePainter.drawItemText(m_rctButton, Qt::AlignCenter, m_pWidget->palette(), true, m_qstrPressText);
            }
        }
    }
    else
    {
        stylePainter.fillRect(m_rctButton, m_qColorNormalBackGround);

        if (!m_strNormalcon.isEmpty())
        {
            QPixmap pmIcon = THEME_GET_BMP(m_strNormalcon.toUtf8().data());
            if (!pmIcon.isNull())
            {
                stylePainter.drawPixmap(m_rctButton.x() + m_rctButton.width() / 2 - pmIcon.width() / 2,
                                        m_rctButton.y() + m_rctButton.height() / 2 - pmIcon.height() / 2, pmIcon);
            }
        }
        else if ("" != m_qstrNormalText)
        {
            QFont fontName;
            QPen pen = stylePainter.pen();
            pen.setColor(m_qColorNormalText);
            stylePainter.setPen(pen);
            fontName.setPointSize(ENTERBTN_TEXT_FONT_SIZE);
            stylePainter.setFont(fontName);
            if (NULL != m_pWidget)
            {
                stylePainter.drawItemText(m_rctButton, Qt::AlignCenter, m_pWidget->palette(), true, m_qstrNormalText);
            }
        }
    }
#else
    if (m_bInputButtonDown) //按下效果
    {
        QPixmap pmIcon = m_pmPressIcon;

        if (!pmIcon.isNull())
        {
            stylePainter.drawPixmap(m_rctButton.x(),
                                    m_rctButton.y(), pmIcon);
        }
    }
    else
    {
        if (!m_pmNormalIcon.isNull())
        {
            stylePainter.drawPixmap(m_rctButton.x(),
                                    m_rctButton.y(), m_pmNormalIcon);
        }
    }


    if ("" != m_strNormalIconPath && !m_strNormalIconPath.isEmpty() && !m_strNormalIconPath.isNull())
    {
        yl::string strIconPath = fromQString(m_strNormalIconPath);
        QPixmap pmIcon = THEME_GET_BMP(m_strNormalIconPath.toUtf8().data());
        if (!pmIcon.isNull())
        {
            stylePainter.drawPixmap(m_rctButton.x() + m_rctButton.width() / 2 - pmIcon.width() / 2,
                                    m_rctButton.y() + m_rctButton.height() / 2 - pmIcon.height() / 2, pmIcon);
        }
    }
    else
    {
        if (m_bInputButtonDown) //按下效果
        {
            if ("" != m_qstrPressText)
            {
                QFont fontName;
                QPen pen = stylePainter.pen();
                pen.setColor(m_qColorPressText);
                stylePainter.setPen(pen);
                fontName.setPointSize(ENTERBTN_TEXT_FONT_SIZE_PRESS);
                stylePainter.setFont(fontName);
                if (NULL != m_pWidget)
                {
                    stylePainter.drawItemText(m_rctButton, Qt::AlignCenter, m_pWidget->palette(), true, m_qstrPressText);
                }
            }
        }
        else
        {
            if ("" != m_qstrNormalText)
            {
                QFont fontName;
                QPen pen = stylePainter.pen();
                pen.setColor(m_qColorNormalText);
                stylePainter.setPen(pen);
                fontName.setPointSize(ENTERBTN_TEXT_FONT_SIZE);
                stylePainter.setFont(fontName);
                if (NULL != m_pWidget)
                {
                    stylePainter.drawItemText(m_rctButton, Qt::AlignCenter, m_pWidget->palette(), true, m_qstrNormalText);
                }
            }
        }
    }
#endif

    return ;
}

bool CInputButton::InputButtonMousePress(const QPoint& mousePressPoint)
{
    if (m_rctGlobalButton.contains(mousePressPoint))
    {
        m_bInputButtonDown = true;
        return true;
    }
    return false;
}

bool CInputButton::InputButtonMouseRelease(const QPoint& mouseReleasePoint)
{
    if (m_rctGlobalButton.contains(mouseReleasePoint))
    {
        m_bInputButtonDown = false;
        return true;
    }
    return false;
}

void CInputButton::RepaintInputButton()
{
    if (NULL != m_pWidget)
    {
        m_pWidget->update(m_rctButton);
    }
}

void CInputButton::ResetInputButtonDown()
{
    m_bInputButtonDown = false;
}
