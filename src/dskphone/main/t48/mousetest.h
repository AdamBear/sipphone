#ifndef MOUSETSET
#define MOUSETSET

#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qscreen_qws.h>
#include <qsocketnotifier.h>
typedef struct tagMouseEvent
{
    long x;
    long y;
    long z;
    long t;
    long M;
    tagMouseEvent()
    {
        x = 0;
        y = 0;
        z = 0;
        t = 0;
        M = 0;
    }
} MouseEvent, *PMouseEvent;


class MouseTestPrivate : public QObject
{
    Q_OBJECT

public:
    MouseTestPrivate();
    ~MouseTestPrivate();
    void    InitMouseTest();
private:
    int     PrevXPos;       // 上一次的点x坐标
    int     PrevYPos;       // 上一次的点y坐标
    bool    pressed;        // 上一次的按下弹起状态
    QPoint  mousePos;       // 当前点位置
    int     m_mouseFD;      // 读取文件句柄
    QString m_mouseDev;     // 监测的文件名
    void    HandleTSData(PMouseEvent pEvent);
private:
    QSocketNotifier * mouseNotifier;
private slots:
    void    ReadMouseData();
};

#endif //MOUSETSET
