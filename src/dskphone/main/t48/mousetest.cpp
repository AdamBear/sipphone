#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QMouseEvent>
#include <QFile>
#include <string.h>
#include <QtGui/qmouse_qws.h>
#include <sys/inotify.h>
#include "mousetest.h"
#include <QtGui/QApplication>
#include <ETLLib.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <winimp/winimp.h>
#define MOUSE_DEBUGINFO
#ifdef  MOUSE_DEBUGINFO
#define DEBUGINFO(...) printTime(__VA_ARGS__)
#else
#define DEBUGINFO(...)
#endif

// 读取的文件
const char path[50] = TEMP_PATH "TouchTest";
// 每次读取文件数据的最大长度
#define MAX_READMOUSE_LEN   100
#define MAX_PARSE_NAME_LEN      5


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

enum MouseType
{
    MOUSE_LEFTBTN,          // 鼠标左键
    MOUSE_MIDBTN,           // 鼠标滚轴键
    MOUSE_RIGHTBTN,         // 鼠标右键
    MOUSE_MAXBTN
};


// 解析字符串
void ParseString(const char * szStr, const char * szContent, const char * szEnd, long * ret)
{
    char * p = NULL, *q = NULL;
    char * buf = NULL;
    buf = (char *)szStr;
    char temp[MAX_READMOUSE_LEN] = {0};
    p = strstr(buf, szContent);
    if (p != NULL)
    {
        p += strlen(szContent);
        q = strstr(p, szEnd);
        if (q != NULL)
        {
            memset(temp, 0, MAX_READMOUSE_LEN);
            memcpy(temp, p, q - p);
            *ret = atol(temp);
        }
    }
}

// 解析鼠标事件
void ParseMouse(const char * szStr, PMouseEvent pMouseEvent)
{
    char szX[MAX_PARSE_NAME_LEN] = "x:";
    char szY[MAX_PARSE_NAME_LEN] = "y:";
    char szZ[MAX_PARSE_NAME_LEN] = "z:";
    char szT[MAX_PARSE_NAME_LEN] = "T:";
    char szMouseType[MAX_PARSE_NAME_LEN] = "M:";
    char szBlank[MAX_PARSE_NAME_LEN] = "_";
    // X
    ParseString(szStr, szX, szBlank, &((*pMouseEvent).x));
    // Y
    ParseString(szStr, szY, szBlank, &((*pMouseEvent).y));
    // Z
    ParseString(szStr, szZ, szBlank, &((*pMouseEvent).z));
    // T
    ParseString(szStr, szT, szBlank, &((*pMouseEvent).t));
    // M
    ParseString(szStr, szMouseType, szBlank, &((*pMouseEvent).M));
}

MouseTestPrivate::MouseTestPrivate()
{
    PrevXPos = 0;       // 上一次的点x坐标
    PrevYPos = 0;       // 上一次的点y坐标
    pressed = false;        // 上一次的按下弹起状态
}

MouseTestPrivate::~MouseTestPrivate()
{
    if (mouseNotifier != NULL)
    {
        delete mouseNotifier;
        mouseNotifier = NULL;
    }
}
// 处理触摸屏输入的数据
void MouseTestPrivate::HandleTSData(PMouseEvent pEvent)
{
    int xpos = 0;
    int ypos = 0;
    static int iCnt = 0;
#ifndef WIN32
    QWSMouseHandler * handler = QWSServer::mouseHandler();
    if (NULL == handler)
    {
        iCnt++;
        DEBUGINFO("QWSServer::mouseHandler() is NULL, Count is %d.\n", iCnt);
        return;
    }
    //若受力很轻，则认为抬起
    if (pEvent->z != 0)
    {
        //将样本值转化为屏幕坐标值
        xpos = pEvent->x;
        ypos = pEvent->y;

        DEBUGINFO("CursorPos.z != 0.\n");
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
            pressed = TRUE;
            handler->mouseChanged(mousePos, Qt::LeftButton);
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
            DEBUGINFO("mouseChanged(0) , mouse is [up].mouseCount is %d.\n", mouseCount);
            mouseCount++;

            pressed = false;
            PrevXPos = 0;
            PrevYPos = 0;
        }
    }
#endif
}

void MouseTestPrivate::ReadMouseData()
{

    char buf[1024] = {0};
    MouseEvent mEvent;
    // 读取管道信息
    if (read(m_mouseFD, buf, 1024) != 0)
    {
        DEBUGINFO("Current MouseData is %s.\n", buf);
        ParseMouse(buf, &mEvent);
        DEBUGINFO("[ReadMouseData]:: mEvent.x is %d, mEvent.y is %d.\n", mEvent.x, mEvent.y);
        DEBUGINFO("mEvent.z is %d, mEvent.T is %d, mEvent.M is %d.\n", mEvent.z, mEvent.t, mEvent.M);
        HandleTSData(&mEvent);
    }

}


void MouseTestPrivate::InitMouseTest()
{

    int fd;
    if ((fd = access(path, F_OK)) == 0)
    {
        DEBUGINFO("[access] file %s exist!\n", path);
    }
    else
    {
        DEBUGINFO("[access] file %s not exist!!\n", path);
        int ret;
        ret = mkfifo(path, S_IFIFO | 0666);
        if (ret == 0)
        {
            // 成功建立命名管道
            DEBUGINFO("[fopen] create file %s succeed!!\n", path);
        }
        else
        {
            // 创建命名管道失败
            DEBUGINFO("[fopen] create file %s failed!!\n", path);
            DEBUGINFO("mkfifo %s Error. (%s)", path, strerror(errno));
        }
    }


    if (m_mouseDev.isEmpty())
    {
        m_mouseDev = QString::fromLatin1(TEMP_PATH "TouchTest");
    }
    DEBUGINFO("m_mouseDev is %s.\n", m_mouseDev.toUtf8().data());
    // 打开输入设备
    if ((m_mouseFD = open(m_mouseDev.toLatin1().data(), O_RDWR | O_NONBLOCK)) < 0)
    {
        qWarning("Cannot open %s (%s)", m_mouseDev.toLatin1().data(), strerror(errno));
        return ;
    }
    else
    {
        etlSleep(1000);
    }

    DEBUGINFO("connect mouseNotifier start.\n");
    mouseNotifier = new QSocketNotifier(m_mouseFD, QSocketNotifier::Read,
                                        this);
    connect(mouseNotifier, SIGNAL(activated(int)), this, SLOT(ReadMouseData()));
    DEBUGINFO("connect mouseNotifier end.\n");

}
