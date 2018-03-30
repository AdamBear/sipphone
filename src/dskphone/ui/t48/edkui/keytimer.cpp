#include "keytimer.h"
#include "edkui/modedkui.h"
#include "edk/include/modedk.h"

#define  EDK_VOLUMNKEY_TIME     2000

IMPLEMENT_GETINSTANCE(CKeyTimer)

CKeyTimer::CKeyTimer(QWidget * parent/* = 0*/)
    : QObject(parent)
{
    m_pTimer = new QTimer(this);

    if (NULL != m_pTimer)
    {
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(KeyTimeOut()));
    }
}

CKeyTimer::~CKeyTimer()
{
    if (m_pTimer != NULL)
    {
        m_pTimer->stop();
    }
}

//开启定时器
void CKeyTimer::StartKeyTimer()
{
    if (m_pTimer != NULL)
    {
        m_pTimer->start(EDK_VOLUMNKEY_TIME);
    }
}

//停止定时器
void CKeyTimer::StopKeyTimer()
{
    if (NULL != m_pTimer && m_pTimer->isActive())
    {
        m_pTimer->stop();
    }
}

bool CKeyTimer::IsKeyTimerActive() const
{
    if (NULL == m_pTimer)
    {
        return false;
    }

    return m_pTimer->isActive();
}

void CKeyTimer::KeyTimeOut()
{
    EdkUI_SetHintMode(!EDK_IsTipModeEnable());

    StopKeyTimer();
}