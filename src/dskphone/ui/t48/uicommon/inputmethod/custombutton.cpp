#include "custombutton.h"
#include "datastruct.h"
#include "qtcommon/qmisc.h"

namespace
{
//长按上限时间
#define CUSTOM_BUTTON_LONG_PRESS_TIME       600
//长按连续发信号时间
#define CUSTOM_BUTTON_LONG_PRESS_EMIT_TIME  50

//按钮圆角宽度
#define CUSTOM_BUTTON_FILLET_WIDTH              10
//按钮圆角高度
#define CUSTOM_BUTTON_FILLET_HEIGHT             10

#define CUSTOM_BUTTON_NORMAL_COLOR      QColor(94, 136, 145)
#define CUSTOM_BUTTON_LIGHT_COLOR               QColor(174, 192, 199)
#define CUSTOM_BUTTON_DEEP_COLOR                QColor(48, 84, 98)
#define CUSTOM_BUTTON_PRESS_COLOR               QColor(11, 59, 80, 102)


#define PIC_INPUTMETHOD_PATH PIC_COMMON_PATH "inputmethod/"

#define CUSTOM_BUTTON_FILLET_NORMAL_LT      PIC_INPUTMETHOD_PATH "btn_fillet_normal_lt.png"
#define CUSTOM_BUTTON_FILLET_NORMAL_LB      PIC_INPUTMETHOD_PATH "btn_fillet_normal_lb.png"
#define CUSTOM_BUTTON_FILLET_NORMAL_RT  PIC_INPUTMETHOD_PATH "btn_fillet_normal_rt.png"
#define CUSTOM_BUTTON_FILLET_NORMAL_RB  PIC_INPUTMETHOD_PATH "btn_fillet_normal_rb.png"

#define CUSTOM_BUTTON_FILLET_LIGHT_LT           PIC_INPUTMETHOD_PATH "btn_fillet_light_lt.png"
#define CUSTOM_BUTTON_FILLET_LIGHT_LB           PIC_INPUTMETHOD_PATH "btn_fillet_light_lb.png"
#define CUSTOM_BUTTON_FILLET_LIGHT_RT           PIC_INPUTMETHOD_PATH "btn_fillet_light_rt.png"
#define CUSTOM_BUTTON_FILLET_LIGHT_RB           PIC_INPUTMETHOD_PATH "btn_fillet_light_rb.png"

#define CUSTOM_BUTTON_FILLET_DEEP_LT            PIC_INPUTMETHOD_PATH "btn_fillet_deep_lt.png"
#define CUSTOM_BUTTON_FILLET_DEEP_LB            PIC_INPUTMETHOD_PATH "btn_fillet_deep_lb.png"
#define CUSTOM_BUTTON_FILLET_DEEP_RT            PIC_INPUTMETHOD_PATH "btn_fillet_deep_rt.png"
#define CUSTOM_BUTTON_FILLET_DEEP_RB            PIC_INPUTMETHOD_PATH "btn_fillet_deep_rb.png"

#define CUSTOM_BUTTON_FILLET_PRESS_LT           PIC_INPUTMETHOD_PATH "btn_fillet_press_lt.png"
#define CUSTOM_BUTTON_FILLET_PRESS_LB           PIC_INPUTMETHOD_PATH "btn_fillet_press_lb.png"
#define CUSTOM_BUTTON_FILLET_PRESS_RT           PIC_INPUTMETHOD_PATH "btn_fillet_press_rt.png"
#define CUSTOM_BUTTON_FILLET_PRESS_RB           PIC_INPUTMETHOD_PATH "btn_fillet_press_rb.png"

}

CCustomButton::CCustomButton(QWidget* v_parent)
    : CLongPressButton(v_parent)
    , m_bLongPress(true)
    , m_bStopWhenEmit(false)
    , m_bIsFunctionKey(false)
    , m_eBGType(BUTTON_BG_TYPE_ERROR)
    , m_strButtonIcon("")
    , m_bIsEnterKey(false)
    , m_bIsImeKey(false)
    , m_bIsShiftKey(false)
    , m_qstrHintText("")
{

}

CCustomButton::~CCustomButton()
{

}

bool CCustomButton::InputButtonMousePress(const QPoint& mousePressPoint)
{
    if (CInputButton::InputButtonMousePress(mousePressPoint))
    {
        if (m_bLongPress)
        {
            OnLongPressBtnPressed();
        }
        return true;
    }
    return false;
}

bool CCustomButton::InputButtonMove(const QPoint& mousePressPoint)
{
    if (m_rctGlobalButton.contains(mousePressPoint))
    {
        return true;
    }
    m_bInputButtonDown = false;
    if (m_bLongPress)
    {
        stopTimer();
    }
    return false;
}

bool CCustomButton::InputButtonMouseRelease(const QPoint& mouseReleasePoint)
{
    if (CInputButton::InputButtonMouseRelease(mouseReleasePoint))
    {
        if (m_bLongPress)
        {
            OnLongPressBtnReleased();
        }
        return true;
    }
    return false;
}


void CCustomButton::OnLongPressBtnPressed()
{
    startTimer(CUSTOM_BUTTON_LONG_PRESS_TIME);
}

void CCustomButton::timerEvent(QTimerEvent* pEvent)
{
    if (NULL == pEvent
            || pEvent->timerId() != m_nTimerID)
    {
        return;
    }

    // 停止计时器
    stopTimer();

    if (m_bInputButtonDown)
    {
        if (m_bStopWhenEmit)
        {
            emit SignalLongPressToBox(true);
        }
        else
        {
            emit SignalLongPress(m_nBtnAction);

            //没有收到停止信号 继续往下执行
            startTimer(CUSTOM_BUTTON_LONG_PRESS_EMIT_TIME);
        }
    }
}

void CCustomButton::paintInputButton(QStylePainter& stylePainter)
{
#ifndef IF_LYNC
    if (IfButtonAvailable())
    {
        CInputButton::paintInputButton(stylePainter);
    }
#else
    if (m_bIsEnterKey || m_bIsImeKey)
    {
        CInputButton::paintInputButton(stylePainter);
    }

    if (m_bInputButtonDown && IfButtonAvailable())
    {
        CCustomButton::paintButtonPress(stylePainter);
    }
#endif
}

bool CCustomButton::IfButtonAvailable()
{
    //功能键总是有效
    if (m_bIsFunctionKey
            //如果不是功能键，要有字符，否则无效
            || "" != m_qstrNormalText)
    {
        return true;
    }
    return false;
}

void CCustomButton::paintButtonPress(QStylePainter& stylePainter)
{
    if (!m_bInputButtonDown)
    {
        return;
    }

    //如果按钮大小小于圆角的两倍，直接使用矩形
    if (m_rctButton.width() < CUSTOM_BUTTON_FILLET_WIDTH + CUSTOM_BUTTON_FILLET_WIDTH
            || m_rctButton.height() < CUSTOM_BUTTON_FILLET_HEIGHT + CUSTOM_BUTTON_FILLET_HEIGHT)
    {
        stylePainter.setPen(CUSTOM_BUTTON_PRESS_COLOR);
        stylePainter.setBrush(CUSTOM_BUTTON_PRESS_COLOR);
        stylePainter.drawRoundedRect(m_rctButton, CUSTOM_BUTTON_FILLET_WIDTH, CUSTOM_BUTTON_FILLET_HEIGHT);

        return;
    }

    //分别画四个圆角
    stylePainter.drawPixmap(
        m_rctButton.x()
        , m_rctButton.y()
        , THEME_GET_BMP(GetFilletPixByType(BUTTON_BG_TYPE_PRESS, BUTTON_ANGLE_TYPE_LEFT_TOP).toUtf8().data()));

    stylePainter.drawPixmap(
        m_rctButton.x()
        , m_rctButton.y() + m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT
        , THEME_GET_BMP(GetFilletPixByType(BUTTON_BG_TYPE_PRESS, BUTTON_ANGLE_TYPE_LEFT_BOTTOM).toUtf8().data()));

    stylePainter.drawPixmap(
        m_rctButton.x() + m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH
        , m_rctButton.y()
        , THEME_GET_BMP(GetFilletPixByType(BUTTON_BG_TYPE_PRESS, BUTTON_ANGLE_TYPE_RIGHT_TOP).toUtf8().data()));

    stylePainter.drawPixmap(
        m_rctButton.x() + m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH
        , m_rctButton.y() + m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT
        , THEME_GET_BMP(GetFilletPixByType(BUTTON_BG_TYPE_PRESS, BUTTON_ANGLE_TYPE_RIGHT_BOTTOM).toUtf8().data()));

    //画剩下的上中下三部分
    QRect rctTop(m_rctButton.x() + CUSTOM_BUTTON_FILLET_WIDTH
                 , m_rctButton.y()
                 , m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH - CUSTOM_BUTTON_FILLET_WIDTH
                 , CUSTOM_BUTTON_FILLET_HEIGHT);

    QRect rctMiddle(m_rctButton.x()
                    , m_rctButton.y() + CUSTOM_BUTTON_FILLET_HEIGHT
                    , m_rctButton.width()
                    , m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT - CUSTOM_BUTTON_FILLET_HEIGHT);

    QRect rctBottom(m_rctButton.x() + CUSTOM_BUTTON_FILLET_WIDTH
                    , m_rctButton.y() + m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT
                    , m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH - CUSTOM_BUTTON_FILLET_WIDTH
                    , CUSTOM_BUTTON_FILLET_HEIGHT);

    stylePainter.fillRect(rctTop, CUSTOM_BUTTON_PRESS_COLOR);
    stylePainter.fillRect(rctMiddle, CUSTOM_BUTTON_PRESS_COLOR);
    stylePainter.fillRect(rctBottom, CUSTOM_BUTTON_PRESS_COLOR);
}

const QString CCustomButton::GetFilletPixByType(BUTTON_BG_TYPE eBGType, BUTTON_ANGLE_TYPE eAngleType)
{
    switch (eBGType)
    {
    case BUTTON_BG_TYPE_LIGHT:
        {
            if (BUTTON_ANGLE_TYPE_LEFT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_LIGHT_LT;
            }
            else if (BUTTON_ANGLE_TYPE_LEFT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_LIGHT_LB;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_LIGHT_RT;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_LIGHT_RB;
            }
        }
        break;
    case BUTTON_BG_TYPE_NORMAL:
        {
            if (BUTTON_ANGLE_TYPE_LEFT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_NORMAL_LT;
            }
            else if (BUTTON_ANGLE_TYPE_LEFT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_NORMAL_LB;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_NORMAL_RT;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_NORMAL_RB;
            }
        }
        break;
    case BUTTON_BG_TYPE_DEEP:
        {
            if (BUTTON_ANGLE_TYPE_LEFT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_DEEP_LT;
            }
            else if (BUTTON_ANGLE_TYPE_LEFT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_DEEP_LB;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_DEEP_RT;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_DEEP_RB;
            }
        }
        break;
    case BUTTON_BG_TYPE_PRESS:
        {
            if (BUTTON_ANGLE_TYPE_LEFT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_PRESS_LT;
            }
            else if (BUTTON_ANGLE_TYPE_LEFT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_PRESS_LB;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_TOP == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_PRESS_RT;
            }
            else if (BUTTON_ANGLE_TYPE_RIGHT_BOTTOM == eAngleType)
            {
                return CUSTOM_BUTTON_FILLET_PRESS_RB;
            }
        }
        break;
    default:
        {
            //
        }
        break;
    }

    return "";
}

QColor CCustomButton::GetColorByType(BUTTON_BG_TYPE eBGType)
{
    switch (eBGType)
    {
    case BUTTON_BG_TYPE_LIGHT:
        {
            return CUSTOM_BUTTON_LIGHT_COLOR;
        }
        break;
    case BUTTON_BG_TYPE_NORMAL:
        {
            return CUSTOM_BUTTON_NORMAL_COLOR;
        }
        break;
    case BUTTON_BG_TYPE_DEEP:
        {
            return CUSTOM_BUTTON_DEEP_COLOR;
        }
        break;
    case BUTTON_BG_TYPE_PRESS:
        {
            return CUSTOM_BUTTON_PRESS_COLOR;
        }
        break;
    default:
        {
            //
        }
        break;
    }

    return CUSTOM_BUTTON_NORMAL_COLOR;
}
