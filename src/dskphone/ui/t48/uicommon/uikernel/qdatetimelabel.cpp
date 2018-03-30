#include "qdatetimelabel.h"
#include <QDateTime>
#include <QTimer>
#include <QEvent>
//#include "define/common.h"
#include <ETLLib/ETLLib.hpp>
#include "devicelib/networkserver.h"
#include "qtcommon/qmisc.h"

qDateTimeLabel::qDateTimeLabel(QWidget * pParent)
    : QLabel(pParent)
    , m_pTimer(NULL)
    , m_strTimeFormat("")
    , m_iRefreshInterval(1)
    , m_pGetTimeFun(NULL)
    , m_iDataFmt(0)
    , m_iTimeFmt(0)
    , m_bIsPointShow(true)
{
    // 初始化数据
    InitData();
}

qDateTimeLabel::~qDateTimeLabel()
{

}

void qDateTimeLabel::InitData()
{
    setObjectName("qDateTimeLabel");

    // 根据初始化的时间格式类型设置时间格式
    SetTimeFmtFromType();

    // 初始化计时器
    m_pTimer = new QTimer(this);
    if (m_pTimer != NULL)
    {
        // 连接计时器时间到触发的事件
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(RefreshDisplay()));

        // 对刷新的时间进行修正
        // 刷新时间最快为1s
        if (m_iRefreshInterval < 1)
        {
            m_iRefreshInterval = 1;
        }
        RefreshDisplay();
        RestartTimer();
    }
}

void qDateTimeLabel::SetTimeFmtType(int iDataFmt, int iTimeFmt)
{
    // 入口参数判断
    if (m_iDataFmt == iDataFmt && m_iTimeFmt == iTimeFmt)
    {
        return ;
    }

    // 设置时间格式的类型
    m_iDataFmt = iDataFmt;
    m_iTimeFmt = iTimeFmt;
    SetTimeFmtFromType();

    // 重设时间格式后，刷新显示时间
    RefreshDisplay();
}

int qDateTimeLabel::GetDateFmt()
{
    return m_iDataFmt;
}

int qDateTimeLabel::GetTimeFmt()
{
    return m_iTimeFmt;
}

void qDateTimeLabel::SetTimeFmtFromType()
{
    // 根据时间格式的类型，设置时间格式
    switch ((TimeFormatType)m_iTimeFmt)
    {
    case TIME_FORMAT_12HOUR:
        {
            if (m_bIsPointShow)
            {
                m_strTimeFormat = "hh:mm AP";
            }
            else
            {
                m_strTimeFormat = "hh mm AP";
            }
        }
        break;
    case TIME_FORMAT_24HOUR:
        {
            if (m_bIsPointShow)
            {
                m_strTimeFormat = "hh:mm";
            }
            else
            {
                m_strTimeFormat = "hh mm";
            }
        }
        break;
    default:
        break;
    }

    // 根据日期格式的类型，设置日期格式
    switch ((DateFormatType)m_iDataFmt)
    {
    case DATE_FORMAT_WWW_MMM_DD:
        {
            m_strTimeFormat += " ddd MMM dd";
        }
        break;
    case DATE_FORMAT_DD_MMM_YY:
        {
            m_strTimeFormat += " dd-MMM-yy";
        }
        break;
    case DATE_FORMAT_YYYY_MM_DD:
        {
            m_strTimeFormat += " yyyy-MM-dd";
        }
        break;
    case DATE_FORMAT_DD_MM_YYYY:
        {
            m_strTimeFormat += " dd/MM/yyyy";
        }
        break;
    case DATE_FORMAT_MM_DD_YY:
        {
            m_strTimeFormat += " MM/dd/yy";
        }
        break;
    case DATE_FORMAT_DD_MMM_YYYY:
        {
            m_strTimeFormat += " dd MMM yyyy";
        }
        break;
    case DATE_FORMAT_WWW_DD_MMM:
        {
            m_strTimeFormat += " ddd dd MMM";
        }
        break;
    default:
        break;
    }


}

void qDateTimeLabel::SetRefreshInterVal(int iMinute)
{
    if (m_iRefreshInterval != iMinute)
    {
        m_iRefreshInterval = iMinute;

        // 重设刷新时间后，先刷新时间
        RefreshDisplay();

        // 重设计时器
        RestartTimer();
    }
}

void qDateTimeLabel::SetGetTimeFun(TimeCallBackFun pGetTimeFun)
{
    // 设置获取时间的函数指针
    if (pGetTimeFun != NULL)
    {
        m_pGetTimeFun = pGetTimeFun;
    }
}

bool qDateTimeLabel::GetLocalTime(QString & strTime)
{
    // 若时间的显示格式为空，读取本地时间失败
    if (m_strTimeFormat.isEmpty())
    {
        return false;
    }

    // 获得系统的时间
    strTime = QDateTime::currentDateTime().toString(m_strTimeFormat);
    return true;
}

void qDateTimeLabel::RefreshDisplay()
{
    //标记改变
    m_bIsPointShow = !m_bIsPointShow;

    setText(GetTime());
}

QString qDateTimeLabel::GetTime()
{
    QString strTime;
    // 判断函数指针的有效性
    if (NULL != m_pGetTimeFun)
    {
        // 获得的时间
        strTime = TimeDateToString(m_pGetTimeFun());
        return strTime;
    }

    //刷新时间格式，实现闪烁
    SetTimeFmtFromType();

    // 若时间的显示格式为空，读取本地时间失败
    if (m_strTimeFormat.isEmpty())
    {
        return strTime;
    }

    return toQString(Timet2String(netGetLocalTime(), " ", false, m_bIsPointShow, false, true).c_str());
}

QString qDateTimeLabel::TimeDateToString(const CDataAndTime & objDataAndTime)
{
    QString strTime;
    // 根据日期格式的类型，设置日期格式
    switch ((DateFormatType)m_iDataFmt)
    {
    case DATE_FORMAT_WWW_MMM_DD:
        {
            strTime = "";
        }
        break;
    case DATE_FORMAT_DD_MMM_YY:
        {
            strTime = "";
        }
        break;
    default:
        break;
    }

    // 根据时间格式的类型，设置时间格式
    switch ((TimeFormatType)m_iTimeFmt)
    {
    case TIME_FORMAT_12HOUR:
        {
            strTime += "";
        }
        break;
    case TIME_FORMAT_24HOUR:
        {
            strTime += "";
        }
        break;
    default:
        break;
    }

    return strTime;
}

void qDateTimeLabel::RestartTimer()
{
    // 指针进行判空操作
    if (NULL == m_pTimer)
    {
        return;
    }

    // 开始计时
    m_pTimer->start(m_iRefreshInterval * 1000);
}
