#ifndef __COMMON_WIDGET_H__
#define __COMMON_WIDGET_H__

#include <QtGui>
#include <ylstring.h>

class qUIPushButton;

class CItemLabel : public QLabel
{
    Q_OBJECT

public:
    CItemLabel(QWidget * parent = NULL);

    ~CItemLabel();

public:
    //设置内容
    virtual void SetContent(const QString & strText,
                            const yl::string & strIcon = ""/*/, const yl::string &strPhoto = ""*/);

    //设置文字
    void SetLabText(const QString & strText, bool bMake = false);

    //设置图标
    void SetLabIcon(const yl::string & strIcon = "");

    QString GetText()
    {
        return m_strText;
    }

    virtual void ClearContent();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

protected:
    yl::string      m_strIcon;      //图标

    QString         m_strText;      //显示文字
};

//DTMF控件条
class QDtmfLine : public QLineEdit
{
    Q_OBJECT

public:
    explicit QDtmfLine(QWidget * parent = 0);
    ~QDtmfLine();

public:
    //设置DTMF栏可见性
    void SetDtmfLineVisible(bool bVisible, bool bRefresh = false);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected:
    // 重新启动定时器
    void StartAutoHideTimer();

private:
    //初始化控件
    void InitWidget();

signals:
    //可见状态改变信号
    void visibleChanged(bool bVisible);

private slots:
    //输入号码框改变时作出的响应操作
    void OnEditChanged(const QString & strEdit);

    //自动隐藏定时器到达
    void OnAutoHideTimeOut();

    //延时加密
    void OnSetToStar();

private:
    //自动隐藏定时器
    QTimer   *  m_pTimerAutoHide;

    //是否以密码模式显示
    bool        m_bIsPassword;

    //如果以密码模式显示，是否延时加密
    bool        m_bDelayToStar;

    //自动加密的定时器
    QTimer   *  m_pTimerDelayToStar;
};


#define MAX_CONNECT_ICON_COUNT  6

class CConnectAnimation : public QWidget
{
    Q_OBJECT

public:
    enum EAnimationType
    {
        eAT_Callout,
        eAT_Callin,
        eAT_TalkCallin
    };

public:
    CConnectAnimation(EAnimationType eAnim, QWidget * pParent = 0);
    virtual ~CConnectAnimation();

public:
    //设置播放速度
    void SetPlaySpeed(int nInterval);
    //开始播放
    void Play();
    //停止播放
    void Stop();

    QPixmap loadPixmap(const char * path);

private slots:
    void UpdateAnimation();

protected:
    void InitData(EAnimationType eAnim);
    void paintEvent(QPaintEvent * pEvent);

private:
    //定义播放时使用的定时器
    QTimer m_timerPlay;
    // 显示的图标索引
    int m_nShowIdx;
    std::vector<QPixmap> m_vecPixmap;
    //定时器间隔时间
    int m_nInterval;
};


class IconLabel: public QLabel
{
    Q_OBJECT
public:
    IconLabel(QWidget * parent = 0);
    ~IconLabel();

    // 设置Icon的大小
    void SetIconSize(const QSize & sizeIcon);

    //设置图标
    void SetTipsIcon(const QString & strIcon);

    //设置文本
    void SetTipsText(const QString & strTipText);

    //设置提示内容
    void SetTips(const QString & strIcon, const QString & strText,
                 Qt::AlignmentFlag eFlag = Qt::AlignCenter);
private:
    void paintEvent(QPaintEvent * paintEvent);
    void GetLayoutInfo(QRect & iconRect, QRect & textRect);

private:
    //记录图标路径
    QString m_strIcon;

    //记录设置文本内容
    QString m_strTipsText;

    // Icon的大小
    QSize   m_sizeIcon;
};

//////////////////////////////////////////////////////////////////////////
class qHoldFrame : public QFrame
{
    Q_OBJECT

public:
    explicit qHoldFrame(QWidget * parent = 0);
    ~qHoldFrame();

    void SetTipInfo(const QString & strText,
                    const QString & strIcon);

protected:
    void resizeEvent(QResizeEvent * pEvent);

signals:
    // 点击HoldBtn的时候发射信号
    void ClickHoldBtn();

private:
    qUIPushButton  * m_pHoldBtn;
    QLabel           m_labText;
};

#endif //__COMMON_WIDGET_H__
