#ifndef QTIME_PIECE
#define QTIME_PIECE

#include <QLabel>
#include <ETLLib/ETLLib.hpp>

/********************************Time Piece********************************/
class QTimePiece : public QLabel
{
    Q_OBJECT
public:
    explicit QTimePiece(QWidget * pParent = 0, Qt::WindowFlags flag = 0);
    virtual ~QTimePiece();

public:
    void StartTimer();
    void PauseTimer();
    void StopTimer();

    BOOL IsTimerStart();
    BOOL IsTimerPause();

    void GetTime(UINT & uHour, UINT & uMinute, UINT & uSecond);
    void SetTime(UINT uHour, UINT uMinute, UINT uSecond);

public slots:
    void OnFlashTimer();

protected:
    UINT64  m_uStartTime;
    UINT64  m_uTickTime;
    UINT64  m_uPauseTime;

    QTimer * m_timerFlash;

protected:
    bool event(QEvent * event);
};

#endif //QTIME_PIECE
