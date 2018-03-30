#ifndef Q_DATETIME_LABEL
#define Q_DATETIME_LABEL
#include <QLabel>
#include "setting/include/common.h"

// 前向声明
class QTimer;

// 日期和时间的数据结构
class CDataAndTime
{
public:
    QString strYear;
    QString strMonth;
    QString strDay;
    QString strHour;
    QString strMin;
};

// 重命名回调函数指针
typedef CDataAndTime(*TimeCallBackFun)();

class qDateTimeLabel : public QLabel
{
    Q_OBJECT

public:
    explicit qDateTimeLabel(QWidget * pParent);
    ~qDateTimeLabel();

    void SetTimeFmtType(int iDataFmt, int iTimeFmt);// 设置显示时间格式的类型
    int GetDateFmt();                               // 返回日期格式的类型
    int GetTimeFmt();                               // 返回时间格式的类型
    void SetRefreshInterVal(int
                            iMinute);           // 设置时间刷新的间隔时间，单位为分钟
    void SetGetTimeFun(TimeCallBackFun pGetTimeFun);// 设置获得服务器时间的调用函数

private:
    bool GetLocalTime(QString & strTime);  // 获取本地的时间
    void InitData();                       // 初始化数据
    void SetTimeFmtFromType();             // 根据时间格式的类型，设置时间格式
    QString GetTime();                     // 获得时间
    void RestartTimer();                   // 重启计时器，获得焦点的时候重新调用
    QString TimeDateToString(const CDataAndTime &
                             objDataAndTime); // 根据时间数据结构返回时间字符串

private:
    int               m_iDataFmt;
    int               m_iTimeFmt;
    QTimer      *     m_pTimer;             // 控制刷新时间的计时器
    QString           m_strTimeFormat;      // 显示时间的格式
    int               m_iRefreshInterval;   // 刷新时间的间隔
    bool              m_bIsPointShow;       //小时和分钟之前的":"是否显示，形成闪烁
    TimeCallBackFun   m_pGetTimeFun;        // 获得服务器时间字符串的函数指针

private slots:
    void RefreshDisplay(); // 刷新Lab上的内容
};

#endif // Q_TIME_LAB
