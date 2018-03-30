#include "longpressinputmanager.h"
#include "inputmethod.h"
#include "uimanager/moduimanager.h"
#include "uimanager/basemainwnd.h"

CLongPressInputManager::CLongPressInputManager(void)
{
    m_iCurrKeyCode = 0;
    m_iDestKeyCode = 0;
    m_pObj = NULL;
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnLongPress()));
}

CLongPressInputManager::~CLongPressInputManager(void)
{
    m_pObj = NULL;
    m_timer.stop();
}

CLongPressInputManager& CLongPressInputManager::GetInstance()
{
    static CLongPressInputManager LongPressInputManager;
    return LongPressInputManager;
}

void CLongPressInputManager::startLongPress(QObject* pObj, int iSrcKeyCode, int iDestKeyCode, int iTime)
{
    if (NULL == pObj)
    {
        return;
    }
    m_timer.stop();
    m_iCurrKeyCode = iSrcKeyCode;
    m_iDestKeyCode = iDestKeyCode;
    m_pObj = pObj;
    m_timer.start(iTime);
}

void CLongPressInputManager::stopLongPress(int iKeyCode)
{
    if (m_iCurrKeyCode == iKeyCode)
    {
        m_timer.stop();
    }
}

void CLongPressInputManager::OnLongPress()
{
    m_timer.stop();
    // 长按0，变成+号
    if (NULL != m_pObj)
    {
        QWidget* p = UIManager_GetMainWnd()->focusWidget();
        if (NULL != p)
        {
            qOnBackspaceClicked(p);
            QKeyEvent evtKey(QEvent::KeyPress, m_iDestKeyCode, Qt::NoModifier);
            qPassEventToEdit(m_pObj, &evtKey);
        }
        m_pObj = NULL;
    }
}

bool CLongPressInputManager::IsOnWork()
{
    return m_timer.isActive();
}
