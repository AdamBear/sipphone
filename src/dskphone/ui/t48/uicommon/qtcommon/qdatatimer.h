#ifndef QDATATIMER_H
#define QDATATIMER_H

#include <QTimer>
#include <QVariant>

class QDataTimer : public QTimer
{
    Q_OBJECT

public:
    explicit QDataTimer(QObject * parent = 0);
    ~QDataTimer();

    static void singleShot(int msec, QObject * receiver, const char * member, const QVariant & value);
};

class QDataSingleShotTimer : public QObject
{
    Q_OBJECT

public:
    int timerId;

public:
    ~QDataSingleShotTimer();
    QDataSingleShotTimer(int msec, QObject * r, const char * m, const QVariant & value);

signals:
    void timeout(const QVariant & value);

protected:
    void timerEvent(QTimerEvent *);

private:
    QVariant m_timerValue;
};

#endif //QDATATIMER_H
