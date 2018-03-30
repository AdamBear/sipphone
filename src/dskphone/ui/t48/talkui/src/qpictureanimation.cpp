#include "qpictureanimation.h"
#include "imagemanager/modimagemanager.h"

// 动画最小间隔时间
#define MIN_ANIMATION_INTERVAL      10

qPicAnimationBase::qPicAnimationBase(QWidget * pParent /* = 0 */)
    : QWidget(pParent)
    , m_bStopByHide(false)
{
    m_timerPlay.setParent(this);
    connect(&m_timerPlay, SIGNAL(timeout()), this, SLOT(UpdateAnimation()));
    m_timerPlay.setInterval(1000);
}

qPicAnimationBase::~qPicAnimationBase()
{

}

//设置播放速度
void qPicAnimationBase::SetPlaySpeed(int nInterval)
{
    bool bActive = m_timerPlay.isActive();

    //设置指定播放速度
    m_timerPlay.setInterval(qMax(nInterval, MIN_ANIMATION_INTERVAL));

    // 由于setInterval会开启计时,还原为停止状态
    if (!bActive)
    {
        m_timerPlay.stop();
    }
}

//开始播放
void qPicAnimationBase::Play()
{
    if (!m_timerPlay.isActive())
    {
        //开启定时器
        m_timerPlay.start();
        update();
    }
}

//停止播放
void qPicAnimationBase::Stop()
{
    //停止定时器
    m_timerPlay.stop();
}

void qPicAnimationBase::UpdateAnimation()
{
    update();
}

void qPicAnimationBase::hideEvent(QHideEvent * pHideEvent)
{
    Stop();
    m_bStopByHide = true;
}

void qPicAnimationBase::showEvent(QShowEvent * pShowEvent)
{
    if (m_bStopByHide)
    {
        Play();
        m_bStopByHide = false;
    }
}

//////////////////////////////////////////////////////////////////////////

qMultiPicAnimation::qMultiPicAnimation(QWidget * pParent/* = 0*/)
    : qPicAnimationBase(pParent)
    , m_iPicIndex(0)
{

}

qMultiPicAnimation::~qMultiPicAnimation()
{

}

void qMultiPicAnimation::AddPicture(const QString & strPicPath)
{
    QPixmap objPixmap = THEME_GET_BMP(strPicPath.toUtf8().data());
    if (!objPixmap.isNull())
    {
        m_vecPixmap.push_back(objPixmap);
    }
}

void qMultiPicAnimation::SetPicture(const VecPicPath & vecPicPath)
{
    m_vecPixmap.clear();

    for (int i = 0; i < vecPicPath.size(); ++i)
    {
        AddPicture(vecPicPath[i]);
    }
}

void qMultiPicAnimation::UpdateAnimation()
{
    CountPicIndex();
    update();
}

void qMultiPicAnimation::CountPicIndex()
{
    if (++m_iPicIndex >= m_vecPixmap.size()
            || m_iPicIndex < 0)
    {
        m_iPicIndex = 0;
    }
}

void qMultiPicAnimation::paintEvent(QPaintEvent * pEvent)
{
    if (m_iPicIndex >= m_vecPixmap.size() || m_iPicIndex < 0)
    {
        return ;
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_vecPixmap[m_iPicIndex]);
}

//////////////////////////////////////////////////////////////////////////
#define  MIN_LOOP_ANIMATION_RANGE 2     // 动画控件最小的Range

qLoopPicAnimation::qLoopPicAnimation(QWidget * pParent/* = 0*/)
    : qPicAnimationBase(pParent)
    , m_eDiection(DIRECTION_STOP)
    , m_strPicPath("")
    , m_iRange(MIN_LOOP_ANIMATION_RANGE)
    , m_iStep(0)
    , m_iX(0)
    , m_iY(0)
{

}

qLoopPicAnimation::~qLoopPicAnimation()
{

}

void qLoopPicAnimation::SetPicture(const QString & strPicPath)
{
    if (m_strPicPath != strPicPath)
    {
        m_strPicPath = strPicPath;
        m_DispalyPixmap = THEME_GET_BMP(m_strPicPath.toUtf8().data());
        update();
    }
}

void qLoopPicAnimation::SetAnimationDirection(DIRECTION eType)
{
    m_eDiection = eType;
}

void qLoopPicAnimation::SetRange(int iRange)
{
    m_iRange = qMax(iRange, MIN_LOOP_ANIMATION_RANGE);
    m_iStep  = 0;
}

void qLoopPicAnimation::UpdateAnimation()
{
    if (++m_iStep >= m_iRange
            || m_iStep < 0)
    {
        m_iStep = 0;
    }

    QSize sizePic = m_DispalyPixmap.size();

    // 计算图片的位置坐标
    switch (m_eDiection)
    {
    case DIRECTION_LEFT:
        {
            m_iX = sizePic.width() / m_iRange * (m_iRange - m_iStep);
            m_iY = 0;
        }
        break;
    case DIRECTION_RIGHT:
        {
            m_iX = sizePic.width() / m_iRange * m_iStep;
            m_iY = 0;
        }
        break;
    case DIRECTION_UP:
        {
            m_iX = 0;
            m_iY = sizePic.height() / m_iRange * (m_iRange - m_iStep);
        }
        break;
    case DIRECTION_DOWN:
        {
            m_iX = 0;
            m_iY = sizePic.height() / m_iRange * m_iStep;
        }
        break;
    default:
        {

        }
        break;
    }

    update();
}

void qLoopPicAnimation::paintEvent(QPaintEvent * pEvent)
{
    QPainter painter(this);
    QSize sizePic = m_DispalyPixmap.size();

    if (m_eDiection == DIRECTION_STOP)
    {
        painter.drawPixmap(0, 0, m_DispalyPixmap);
    }
    else
    {
        painter.drawPixmap(QPointF(0, 0),
                           m_DispalyPixmap,
                           QRectF(IsHorizontal() ? sizePic.width() - m_iX : 0,
                                  IsHorizontal() ? 0 : sizePic.height() - m_iY,
                                  sizePic.width(),
                                  sizePic.height()));

        painter.drawPixmap(QPointF(m_iX, m_iY), m_DispalyPixmap, QRectF(0, 0, sizePic.width() - m_iX,
                           sizePic.height() - m_iY));
    }
}
