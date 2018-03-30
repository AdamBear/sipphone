#ifndef __TIMER_PIECE_VIEW_H__
#define __TIMER_PIECE_VIEW_H__

#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"


class xTimerPieceView : public xTextView, public xTimerHandler
{
    DECLARE_VIEW_DUMP_CLASS(xTimerPieceView)

public:
    xTimerPieceView();
    virtual ~xTimerPieceView();

    enum
    {
        TIMER_PIECE_FORMAT_MS           = 1,
        TIMER_PIECE_FORMAT_HMS          = 2,
        TIMER_PIECE_FORMAT_ALL          = 3,
    };

    static yl::string FormatHMS(long lTimer);
    static yl::string FormatMS(long lTimer);

public:
    virtual void loadAttributes(xml_node & node);

    void SetFormat(INT nFormat);
    void SetMaxTick(long lMaxTick);
    void SetStartDuration(long lDuration);
    void ResetTimer();

    bool IsTimerStart();
    long GetMaxTick();
    long GetDurationTime();

    void StartTimer();
    void PauseTimer();
    void ResumeTimer();
    void StopTimer();

    void UpdateTimerText();

    void OnAppearSlot(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    yl::string tick2Time(long lTimeDuration);

    void OnTimer(xThreadTimer* timer, LPVOID pExtraData);
    void UpdateDuration();

protected:
    int             m_nFormat;
    time_t          m_tmBornTime;
    long            m_lBornDuration;
    long            m_lDuration;
    long            m_lMaxTick;
    xThreadTimer    m_timer;
};

#endif
