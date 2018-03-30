#ifndef _Q_PICTURE_ANIMATION_H_
#define _Q_PICTURE_ANIMATION_H_

#include <QtGui>

// 图片播放基类
class qPicAnimationBase : public QWidget
{
    Q_OBJECT

public:
    qPicAnimationBase(QWidget * pParent = 0);
    virtual ~qPicAnimationBase();

public:
    //设置播放速度
    void SetPlaySpeed(int nInterval);

    //开始播放
    void Play();

    //停止播放
    void Stop();

protected slots:
    virtual void UpdateAnimation();

protected:
    virtual void paintEvent(QPaintEvent * pEvent) = 0;

    virtual void hideEvent(QHideEvent * pHideEvent);

    virtual void showEvent(QShowEvent * pShowEvent);

protected:
    QTimer m_timerPlay;  // 定义播放时使用的定时器
    bool   m_bStopByHide;// 是否因为Hide导致的停止

};

// 多图片播放类
// 目前只支持：定时按顺序播放图片
typedef QVector<QString>        VecPicPath;
typedef VecPicPath::iterator    IterVecPicPath;

class qMultiPicAnimation : public qPicAnimationBase
{
    Q_OBJECT

public:
    qMultiPicAnimation(QWidget * pParent = 0);
    virtual ~qMultiPicAnimation();

    // 在原来的基础上添加动画图片
    void AddPicture(const QString & strPicPath);

    // 重新设置所有的动画图片
    void SetPicture(const VecPicPath & vecPicPath);

protected slots:
    virtual void UpdateAnimation();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

private:
    void CountPicIndex();

private:
    int                     m_iPicIndex;    // 当前图片的index
    QVector<QPixmap>        m_vecPixmap;    // 图片容器
};


// 单张图片循环播放
class qLoopPicAnimation : public qPicAnimationBase
{
    Q_OBJECT

public:
    enum DIRECTION
    {
        DIRECTION_STOP,
        DIRECTION_LEFT,
        DIRECTION_RIGHT,
        DIRECTION_UP,
        DIRECTION_DOWN,
    };

public:
    qLoopPicAnimation(QWidget * pParent = 0);
    virtual ~qLoopPicAnimation();

    // 设置图片
    void SetPicture(const QString & strPicPath);

    // 设置图片移动的模式
    void SetAnimationDirection(DIRECTION eType);

    // 设置图片每次移动的距离
    // 例如设置3,则每次移动图片长宽1/3的距离
    void SetRange(int iRange);

protected slots:
    virtual void UpdateAnimation();

private:
    virtual void paintEvent(QPaintEvent * pEvent);

    // 判断控件是否水平移动
    bool IsHorizontal()
    {
        return (m_eDiection == DIRECTION_LEFT
                || m_eDiection == DIRECTION_RIGHT);
    }

private:
    QPixmap     m_DispalyPixmap;
    QString     m_strPicPath;
    DIRECTION   m_eDiection;

    int         m_iRange;
    int         m_iStep;
    int         m_iX;
    int         m_iY;
};

#endif
