#include <QtGui>
#include "statusbutton.h"

#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "qtcommon/qmisc.h"

namespace
{
#define STATUS_BUTTON_POS_WIDTH 42
// #define STATUS_BUTTON_POS_HEIGHT 43

#define STATUS_BUTTON_POS_ICON_LEFT 7
#define STATUS_BUTTON_POS_ICON_TOP 9
#define STATUS_BUTTON_POS_ICON_WIDTH 27
#define STATUS_BUTTON_POS_ICON_HEIGHT 25

#define STATUS_BUTTON_POS_TEXT_LEFT 23
#define STATUS_BUTTON_POS_TEXT_TOP 3
#define STATUS_BUTTON_POS_TEXT_WIDTH 18
#define STATUS_BUTTON_POS_TEXT_HEIGHT 18

#define FLASH_TIME_MSEC     500
}

CStatusButton::CStatusButton(QWidget * pWidget)
    :  QObject(pWidget)
    , CButtonWrapper(pWidget)
    , m_nId(0)
    , m_strHint("")
    , m_bIsFlashUp(true)
    , m_bPointDonw(false)
{
    m_vecstrIcon.clear();

    m_FlashTimer.setParent(this);
    connect(&m_FlashTimer, SIGNAL(timeout()), this, SLOT(OnFlashTimeOut()));
}

CStatusButton::~CStatusButton(void)
{
}

int CStatusButton::GetStatusBtnWidth()
{
    return STATUS_BUTTON_POS_WIDTH;
}

void CStatusButton::SetStatus(int nId, const yl::string & strIcon, const yl::string & strHint)
{
    m_nId = nId;
    m_vecstrIcon.push_back(strIcon);
    m_strHint = toQString(strHint.c_str());
}

void CStatusButton::SetStatus(int nId, vecIconString & vecstrIcon, const yl::string & strHint)
{
    m_nId = nId;
    m_vecstrIcon = vecstrIcon;
    m_strHint = toQString(strHint.c_str());

    m_nFlashIndex = 0;
}

int CStatusButton::GetId() const
{
    return m_nId;
}

void CStatusButton::PaintStatusButton(QStylePainter & stylePainter, const char * pszClick)
{
    if (m_bIsFlashUp)
    {
        PaintButton(stylePainter, "", pszClick);

        QRect rcIcon(m_rect.left() + STATUS_BUTTON_POS_ICON_LEFT, m_rect.top() + STATUS_BUTTON_POS_ICON_TOP,
                     STATUS_BUTTON_POS_ICON_WIDTH, STATUS_BUTTON_POS_ICON_HEIGHT);

        QPixmap pmIcon;
        if (1 == m_vecstrIcon.size())
        {
            pmIcon = THEME_GET_BMP(m_vecstrIcon[0].c_str());
        }
        else
        {
            pmIcon = THEME_GET_BMP(m_vecstrIcon[m_nFlashIndex].c_str());
        }

        if (pmIcon.isNull())
        {
            MAINWND_ERR("StatusButton Icon empty, IconPath=%s", m_vecstrIcon[0].c_str());
        }

        stylePainter.drawPixmap(rcIcon, pmIcon);

        if (m_strHint.isEmpty() || NULL == m_pWidget)
        {
            return;
        }

        QRect rcHint(m_rect.left() + STATUS_BUTTON_POS_TEXT_LEFT, m_rect.top() + STATUS_BUTTON_POS_TEXT_TOP,
                     STATUS_BUTTON_POS_TEXT_WIDTH, STATUS_BUTTON_POS_TEXT_HEIGHT);
        QPixmap pmTextBg = THEME_GET_BMP(PIC_ICON_IDLE_STATUS_TIP_BG);

        stylePainter.drawPixmap(rcHint, pmTextBg);
        stylePainter.drawItemText(rcHint, Qt::AlignCenter, m_pWidget->palette(), m_pWidget->isEnabled(),
                                  m_strHint);
    }
    else
    {
        PaintButton(stylePainter, "", "");
        QRect rcIcon(m_rect.left() + STATUS_BUTTON_POS_ICON_LEFT, m_rect.top() + STATUS_BUTTON_POS_ICON_TOP,
                     STATUS_BUTTON_POS_ICON_WIDTH, STATUS_BUTTON_POS_ICON_HEIGHT);
        QPixmap pmNone = THEME_GET_BMP("");

        stylePainter.drawPixmap(rcIcon, pmNone);

        if (m_strHint.isEmpty() || NULL == m_pWidget)
        {
            return;
        }

        QRect rcHint(m_rect.left() + STATUS_BUTTON_POS_TEXT_LEFT, m_rect.top() + STATUS_BUTTON_POS_TEXT_TOP,
                     STATUS_BUTTON_POS_TEXT_WIDTH, STATUS_BUTTON_POS_TEXT_HEIGHT);

        stylePainter.drawPixmap(rcHint, pmNone);
        stylePainter.drawItemText(rcHint, Qt::AlignCenter, m_pWidget->palette(), m_pWidget->isEnabled(),
                                  "");
    }
}

void CStatusButton::StartFlash()
{
    if (m_FlashTimer.isActive())
    {
        m_FlashTimer.stop();
    }

    m_FlashTimer.start(FLASH_TIME_MSEC);
}

void CStatusButton::StopFlash()
{
    if (m_FlashTimer.isActive())
    {
        m_FlashTimer.stop();
    }
}

void CStatusButton::OnFlashTimeOut()
{
    //在需要闪动的情况，如果图标列表只有一个图标，说明是闪烁
    if (m_vecstrIcon.size() == 1)
    {
        m_bIsFlashUp = !m_bIsFlashUp;
    }
    //如果有多于一个图标，则按顺序显示各图标
    else
    {
        ++m_nFlashIndex;
        if (m_nFlashIndex == m_vecstrIcon.size())
        {
            m_nFlashIndex = 0;
        }
    }

    emit NeedRepaint();
}

bool CStatusButton::ProcessMousePressPoint(QPoint & PressPoint)
{
    if (m_rect.contains(PressPoint))
    {
        if (!m_bPointDonw)
        {
            m_bPointDonw = true;

            RepaintButton();
        }
    }
    else if (m_bPointDonw)
    {
        m_bPointDonw = false;

        RepaintButton();
    }

    return m_bPointDonw;
}

bool CStatusButton::ProcessMouseReleasePoint(QPoint & PressPoint)
{
    bool bDown = m_bPointDonw;

    m_bPointDonw = false;

    if (m_rect.contains(PressPoint))
    {
        if (bDown)
        {
            RepaintButton();
        }

        return bDown;
    }

    if (bDown)
    {
        RepaintButton();
    }

    return false;
}
