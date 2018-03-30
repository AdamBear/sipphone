//add by kjf, 2011-04-20
#include "qdatatimer.h"
#include <QAbstractEventDispatcher>

QDataSingleShotTimer::QDataSingleShotTimer(int msec, QObject * receiver, const char * member,
        const QVariant & value)
    : QObject(QAbstractEventDispatcher::instance())
{
    connect(this, SIGNAL(timeout(const QVariant &)), receiver, member);
    m_timerValue = value;
    timerId = startTimer(msec);
}

QDataSingleShotTimer::~QDataSingleShotTimer()
{
    if (timerId > 0)
    {
        killTimer(timerId);
    }
}

class QDPtrAccessor : public QObject
{
public:
    QObjectData * d() const
    {
        return d_ptr.data();
    }
};

void qDeleteInEventHandler(QObject * o)
{
    if (!o)
    {
        return;
    }
    ((QDPtrAccessor *) o)->d()->inEventHandler = false;
    delete o;
}

void QDataSingleShotTimer::timerEvent(QTimerEvent *)
{
    // need to kill the timer _before_ we emit timeout() in case the
    // slot connected to timeout calls processEvents()
    if (timerId > 0)
    {
        killTimer(timerId);
    }
    timerId = -1;
    emit timeout(m_timerValue);

    // we would like to use delete later here, but it feels like a
    // waste to post a new event to handle this event, so we just unset the flag
    // and explicitly delete...
    qDeleteInEventHandler(this);
}

QDataTimer::QDataTimer(QObject * parent)
    : QTimer(parent)
{

}

QDataTimer::~QDataTimer()
{

}

void QDataTimer::singleShot(int msec, QObject * receiver, const char * member,
                            const QVariant & value)
{
    if (receiver && member)
    {
        (void) new QDataSingleShotTimer(msec, receiver, member, value);
    }
}
