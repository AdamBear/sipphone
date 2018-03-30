#pragma once
#include <QObject>


class CLongPressInputManager : public QObject
{
    Q_OBJECT
public:
    CLongPressInputManager(void);
    ~CLongPressInputManager(void);

public:
    static CLongPressInputManager& GetInstance();

    void startLongPress(QObject* pObj, int iSrcKeyCode, int iDestKeyCode, int iTime);

    void stopLongPress(int iKeyCode);

    bool IsOnWork();
private slots:
    void OnLongPress();

private:
    int         m_iCurrKeyCode;
    int         m_iDestKeyCode;
    QTimer      m_timer;
    QObject*    m_pObj;
};

#define g_LongPressInputManager CLongPressInputManager::GetInstance()





