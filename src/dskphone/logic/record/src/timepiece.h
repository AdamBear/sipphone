#ifndef __TIMEPIECE_H__
#define __TIMEPIECE_H__

#include <ETLLib/ETLLib.hpp>

class TimePiece: public CMKProcessor
{
public:
    TimePiece();
    virtual ~TimePiece();

public:
    void StartTimer();
    void PauseTimer();
    void StopTimer();

    BOOL IsTimerStart();
    BOOL IsTimerPause();

    void GetTime(UINT & uHour, UINT & uMinute, UINT & uSecond);
    void SetTime(UINT uHour, UINT uMinute, UINT uSecond);

private:
    BOOL OnTimeOut(msgObject & objMessage);

protected:
    UINT64  m_uStartTime;
    UINT64  m_uTickTime;
    UINT64  m_uPauseTime;

    mkThreadTimer   m_RefreshTimer;
};

#endif // __BASE_RECORD_H__
