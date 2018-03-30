#include "longpressbutton.h"

CLongPressButton::CLongPressButton(QWidget* v_parent)
    : CInputButton(v_parent)
{
    //定时器先设置
    m_nTimerID      = 0;
    // http://10.3.5.199/Bug.php?BugID=68596 概率性信号顺序出错，先不使用此方法实现长按功能
    //connect(this, SIGNAL(pressed()), this, SLOT(OnLongPressBtnPressed()));
    //connect(this, SIGNAL(released()), this, SLOT(OnLongPressBtnReleased()));
}

CLongPressButton::~CLongPressButton(void)
{
}

bool CLongPressButton::InputButtonMousePress(const QPoint& mousePressPoint)
{
    if (CInputButton::InputButtonMousePress(mousePressPoint))
    {
        OnLongPressBtnPressed();
        return true;
    }
    return false;
}

bool CLongPressButton::InputButtonMouseRelease(const QPoint& mouseReleasePoint)
{
    if (CInputButton::InputButtonMouseRelease(mouseReleasePoint))
    {
        OnLongPressBtnReleased();
        return true;
    }
    return false;
}

void CLongPressButton::OnLongPressBtnPressed()
{
    startTimer(180);
}

void CLongPressButton::OnLongPressBtnReleased()
{
    stopTimer();
}

void CLongPressButton::startTimer(int iMSecond)
{
    // 先停止计时器
    stopTimer();
    // 开启计时并记录ID
    m_nTimerID = QObject::startTimer(iMSecond);
}

void CLongPressButton::stopTimer()
{
    // 判断计时器是否激活
    if (m_nTimerID != 0)
    {
        QObject::killTimer(m_nTimerID);
        m_nTimerID = 0;
    }
}

void CLongPressButton::timerEvent(QTimerEvent* pEvent)
{
    // 停止计时器
    stopTimer();

    emit SignalLongPress(m_nBtnAction);

//  if(this->isHidden())
//  {
//      return ;
//  }
//
//  if(!this->isVisible())
//  {
//      return ;
//  }

    //没有收到停止信号 继续往下执行
    startTimer(50);

}

void CLongPressButton::ResetInputButtonDown()
{
    CInputButton::ResetInputButtonDown();
    stopTimer();
}
