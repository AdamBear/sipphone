#include "ylmousedriver.h"

#include <qtimer.h>
#include <qsocketnotifier.h>
#ifndef WIN32
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qscreen_qws.h>
#endif
//#include <private/qcore_unix_p.h> // overrides QT_OPEN
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <qfile.h>
#include <qtextstream.h>
#include <winimp/winimp.h>

// 读取触点数据最大个数，包含3个EV_ABS（x/y/z）、1个EV_KEY、1个EV_SYN
#define     MAX_READDATA_ONCE           5
// 调试信息环境开关变量，控制加载校准文件
static int debug_env = -1;
// 控制触屏点信息调试信息宏开关
// #define MOUSE_DEBUG_INFO

#ifdef MOUSE_DEBUG_INFO
#define DEBUG_LOG(...)      printTime(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

void printTime(const char * lpszFmt, ...)
{
    struct timeval tData;
    gettimeofday(&tData, NULL);

    va_list argList;
    va_start(argList, lpszFmt);
    fprintf(stdout, "Time:(%ld.%ld)\t", (tData.tv_sec % 1000) * 1000 + tData.tv_usec / 1000,
            tData.tv_usec % 1000);
    vfprintf(stdout, lpszFmt, argList);
    fprintf(stdout, "\n");
    va_end(argList);
}



//#define SUPPORT_TOUCHSCREEN_DRIVER      1

// #define SAMPLECOUNT 5

typedef struct MPoint
{
    int    x;
    int    y;
    int    z;
} MyPoint;


class QYealinkTouchScreenHandlerPrivate : public QObject
{
    Q_OBJECT

public:
    QYealinkTouchScreenHandlerPrivate(QYealinkInputMouseHandler * h, const QString & device);
    ~QYealinkTouchScreenHandlerPrivate();

    void suspend();
    void resume();
private:
    int     mouseFD;
    MyPoint CursorPos;
    int     Param[7];
    int     PrevXPos;
    int     PrevYPos;

    bool    pressed;

    void    handleTSData();
    QYealinkInputMouseHandler * handler;
    QSocketNotifier * mouseNotifier;

private slots:
    void readMouseData();
    void sendRelease();

};

QYealinkTouchScreenHandlerPrivate::QYealinkTouchScreenHandlerPrivate(QYealinkInputMouseHandler * h,
        const QString & device)
{
    mouseFD = -1;
    PrevXPos = 0;
    PrevYPos = 0;
    pressed = FALSE;

    handler = h;
    QString mouseDev = device;

    //********** Add by cdx at 2008-12-12 14:55 Begin ***************************
    //********** 对不一样的QT分辨率实行不一样的映射关系和校正参数********************
    int G_Screen_Width = 800;
    int G_Screen_Height = 480;
    int DefaultParam[7] = {13962, 114, -2429380, 255, 8679, -2645884, 65536};

    if (G_Screen_Width == 720)
    {
        if (G_Screen_Height == 480)
        {
            DefaultParam[0] =  11814     ;
            DefaultParam[1] =  -95       ;
            DefaultParam[2] = -791520    ;
            DefaultParam[3] = 14         ;
            DefaultParam[4] = 8427       ;
            DefaultParam[5] = -2249240   ;
            DefaultParam[6] = 65536      ;
        }
        else if (G_Screen_Height == 576)
        {
            DefaultParam[0] =  11556     ;
            DefaultParam[1] =  41        ;
            DefaultParam[2] = -85060     ;
            DefaultParam[3] = 60         ;
            DefaultParam[4] = 10184      ;
            DefaultParam[5] = -2094424   ;
            DefaultParam[6] = 65536      ;
        }
    }
    else if (G_Screen_Width == 800)
    {
        if (G_Screen_Height == 480)
        {
            DefaultParam[0] = 13962;
            DefaultParam[1] = 114;
            DefaultParam[2] = -2429380;
            DefaultParam[3] = 255;
            DefaultParam[4] = 8679;
            DefaultParam[5] = -2645884;
            DefaultParam[6] = 65536;
        }
    }
    //********** Add by linym at 2011-4-26 14:55 End ***************************
    // 获取POINTERCAL_FILE环境变量指定的校准文件路径
    QString calFile;
    calFile = QString::fromLocal8Bit(qgetenv("POINTERCAL_FILE"));

    // 检测调试信息开关环境变量
    if (debug_env == -1)
    {
        debug_env = qgetenv("QT_DEBUG_PLUGINS").toInt();
    }
    // 默认校准文件路径
    if (calFile.isEmpty())
    {
        calFile = QLatin1String("/etc/pointercal");
    }
    // 调试信息开关打开，则输出调试信息
    if (debug_env == 1)
    {
        DEBUG_LOG("calFile is %s.\n", calFile.toUtf8().data());
    }
    QFile file(calFile);
    bool bSuccess = false;
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream t(&file);
        if (debug_env == 1)
        {
            DEBUG_LOG("pointercal open success.\n");
        }
        t >> Param[0] >> Param[1] >> Param[2] >> Param[3] >> Param[4] >> Param[5] >> Param[6];
        bSuccess = true;
        if (Param[6] == 0 || t.status() != QTextStream::Ok)
        {
            qDebug("Corrupt calibration data");
            bSuccess = false;
        }
    }
    // 读取校准文件不成功，使用默认值
    if (bSuccess == false)
    {
        if (debug_env == 1)
        {
            DEBUG_LOG("Use default values.\n");
        }
        memcpy(Param, DefaultParam, sizeof(int) * 7);
    }

    if (mouseDev.isEmpty())
    {
        mouseDev = QString::fromLatin1("/dev/input/TouchScreen");
    }
    // 打开输入设备
    if ((mouseFD = open(mouseDev.toLatin1().data(), O_RDONLY)) < 0)
    {
        qWarning("Cannot open %s (%s)", mouseDev.toLatin1().data(), strerror(errno));
        return ;
    }
    else
    {
        etlSleep(1000);
    }

    mouseNotifier = new QSocketNotifier(mouseFD, QSocketNotifier::Read,
                                        this);
    connect(mouseNotifier, SIGNAL(activated(int)), this, SLOT(readMouseData()));

}

QYealinkTouchScreenHandlerPrivate::~QYealinkTouchScreenHandlerPrivate()
{
    if (mouseFD >= 0)
    {
        close(mouseFD);
    }
}

void QYealinkTouchScreenHandlerPrivate::suspend()
{
    if (mouseNotifier)
    {
        mouseNotifier->setEnabled(false);
    }
}

void QYealinkTouchScreenHandlerPrivate::resume()
{
    if (mouseNotifier)
    {
        mouseNotifier->setEnabled(true);
    }
}

#ifndef WIN32
// 鼠标点击位置
static QPoint & mousePos = QWSServer::mousePosition;
#else
static QPoint mousePos;
#endif
// 处理触摸屏输入的数据
void QYealinkTouchScreenHandlerPrivate::handleTSData()
{
    int xpos = 0;
    int ypos = 0;
    //若受力很轻，则认为抬起
    if (CursorPos.z != 0)
    {
        //将样本值转化为屏幕坐标值
        xpos = (Param[0] * CursorPos.x + Param[1] * CursorPos.y + Param[2]) / Param[6];
        ypos = (Param[3] * CursorPos.x + Param[4] * CursorPos.y + Param[5]) / Param[6];

        DEBUG_LOG("CursorPos.z != 0.\n");
        //若本次坐标与上次坐标相比变化很小，则认为光标没有移动，避免光标不断漂移情况
        if ((!pressed) || (pressed
                           && (abs(PrevXPos - xpos) + abs(PrevYPos - ypos) > 3)))
        {
            PrevXPos = xpos;
            PrevYPos = ypos;
            QPoint q;
            q.setX(xpos);
            q.setY(ypos);
            mousePos = q;
            handler->mouseChanged(mousePos, Qt::LeftButton);
            DEBUG_LOG("*******mouseChanged(Qt::LeftButton).x is %d, y is %d.\n", xpos, ypos);
            pressed = TRUE;
        }
        else
        {
        }
    }
    else
    {
        if (pressed)
        {
            // 认为鼠标抬起状态变化的处理
            handler->mouseChanged(mousePos, 0);

            static long mouseCount = 0;
            DEBUG_LOG("******mouseChanged(0) , mouse is [up].******mouseCount is %d ******.\n", mouseCount);
            mouseCount++;

            pressed = FALSE;
            PrevXPos = 0;
            PrevYPos = 0;
        }
    }
    return;
}

void QYealinkTouchScreenHandlerPrivate::sendRelease()
{
    static bool handled = false;
    if (!handled)
    {
        handleTSData();
        if (pressed)
        {
            handled = true;
        }
    }
    else
    {
        if (pressed)
        {
            handler->mouseChanged(mousePos, 0);
            pressed = FALSE;
            handled = false;
        }
        PrevXPos = 0;
        PrevYPos = 0;
    }
}

struct CustomTPdata
{
    unsigned char status;
    unsigned short xpos;
    unsigned short ypos;

};

struct timeval tv_sub(const struct timeval * out, const struct timeval * in)
{
    struct timeval ret;
    ret.tv_sec = out->tv_sec - in->tv_sec;
    if ((ret.tv_usec = (out->tv_usec - in->tv_usec)) < 0)
    {
        -- ret.tv_sec;
        ret.tv_usec += 1000000;
    }

    return ret;
}

void QYealinkTouchScreenHandlerPrivate::readMouseData()
{
#ifndef WIN32
    if (!qt_screen)
    {
        return ;
    }
#endif

    int ret;

    struct input_event event;
    for (int i = 0; i < MAX_READDATA_ONCE; ++i)
    {
        ret = read(mouseFD, &event, sizeof(struct input_event));

        if (ret == sizeof(struct input_event))
        {
            if (event.type == EV_ABS)
            {
                DEBUG_LOG("event.type is EV_ABS.\n");
                //触摸屏的type为3
                if (event.code == ABS_X)
                {
                    //x，将值存入样本数组
                    CursorPos.x = event.value;

                    DEBUG_LOG("event.code is ABS_X, CursorPos.x is %d.\n", CursorPos.x);
                }
                else if (event.code == ABS_Y)
                {
                    //y，将值存入样本数组
                    CursorPos.y = event.value;
                    DEBUG_LOG("event.code is ABS_Y, CursorPos.y is %d.\n", CursorPos.y);
                }
                else if (event.code == ABS_Z
                         || event.code == ABS_PRESSURE)
                {
                    //z，将值存入样本数组
                    CursorPos.z = event.value;
                    DEBUG_LOG("event.code is %d (ABS_Z or ABS_PRESSURE), CursorPos.z is %d.\n", event.code,
                              CursorPos.z);
                }
            }
            else if (event.type == EV_SYN)
            {
                static struct timeval LastTime;
                struct timeval TimeInterval;
                struct timeval nowTime;
                long lInterval = 0;
                TimeInterval = tv_sub(&event.time, &LastTime);
                gettimeofday(&nowTime, NULL);
                nowTime = tv_sub(&nowTime, &event.time);
                if ((nowTime.tv_sec % 1000) * 1000 + nowTime.tv_usec / 1000 >= 1000 && !(CursorPos.z == 0
                        && pressed))
                {
                    //diff time large than 2s 去抖处理
                    return;
                }

                DEBUG_LOG("Event.time.tv_sec is %d, event.time.tv_usec is %d,\n", event.time.tv_sec,
                          event.time.tv_usec);
                DEBUG_LOG("TimeInterval.tv_sec is %d. TimeInterval.tv_usec is %d.\n", TimeInterval.tv_sec,
                          TimeInterval.tv_usec);
                DEBUG_LOG("Event.type is EV_SYN, CursorPos.z is %d, pressed is %d.\n", CursorPos.z, pressed);

                lInterval = (TimeInterval.tv_sec % 1000) * 1000 + TimeInterval.tv_usec / 1000;
                DEBUG_LOG("#############lInterval### is %d.\n", lInterval);
                // 0 < lInterval 是由于自定义时间时，有可能导致间隔时间为负数，导致按键无响应
                if ((0 <= lInterval && lInterval <= 70)  && !(CursorPos.z == 0 && pressed))
                {
                    // 最小点击2次间隔时间 去抖处理 间隔时间不低于70ms
                    return;
                }
                if (CursorPos.z == 0 && pressed)
                {
                    LastTime = event.time;
                }
                DEBUG_LOG("handleTSData().\n");
                handleTSData();
                // 读取到同步信号，说明当前输入点的信息已经全部获取
                break;
            }
        }

        if (ret < 0)
        {
            qDebug("Error %s", strerror(errno));
        }
    }

}

QYealinkInputMouseHandler::QYealinkInputMouseHandler(const QString & driver, const QString & device)
    : QWSCalibratedMouseHandler(driver, device)
{
    d = new QYealinkTouchScreenHandlerPrivate(this, device);
}

QYealinkInputMouseHandler::~QYealinkInputMouseHandler()
{
    delete d;
}

void QYealinkInputMouseHandler::suspend()
{
    d->suspend();
}

void QYealinkInputMouseHandler::resume()
{
    d->resume();
}

#include "ylmousedriver.moc"
