#include "timerpieceview.h"

#include <ETLLib.hpp>
#include <etlmsghandler/modetlmsghandler.h>
#include "baseuicommon.h"
#include "commonapi/stringhelperapi.h"
#include "devicelib/networkserver.h"
#include "xwindow/modviewdump.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xTimerPieceView);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_VIEW_DUMP_BEGIN(xTimerPieceView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(format, m_nFormat, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(maxtick, m_lMaxTick, VIEW_DUMP_DATA_TYPE_LONG)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xTimerPieceView, xTextView)
IMPLEMENT_VIEW_DUMP_PROPERTY(format)
IMPLEMENT_VIEW_DUMP_PROPERTY(maxtick)
IMPLEMENT_VIEW_DUMP_END(xTimerPieceView)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
xTimerPieceView::xTimerPieceView()
    : m_nFormat(TIMER_PIECE_FORMAT_ALL)
    , m_tmBornTime(0)
    , m_lBornDuration(0)
    , m_lDuration(0)
    , m_lMaxTick(0)
{
    REGISTER_VIEW_DUMP_CLASS(xTimerPieceView)
}

xTimerPieceView::~xTimerPieceView()
{
    StopTimer();
}

void xTimerPieceView::loadAttributes(xml_node & node)
{
    xTextView::loadAttributes(node);

    PopupAndroidInt(node, BASE_UI_ATTR_FORMAT, m_nFormat);
}

yl::string xTimerPieceView::FormatHMS(long lTimer)
{
    long lHour = lTimer / 3600;
    long lMinute = (lTimer / 60) % 60 ;
    long lSecond = lTimer % 60;

    return commonAPI_FormatString("%.2d:%.2d:%.2d", lHour, lMinute, lSecond);
}

yl::string xTimerPieceView::FormatMS(long lTimer)
{
    long lMinute = lTimer / 60 ;
    long lSecond = lTimer % 60;

    return commonAPI_FormatString("%.2d:%.2d", lMinute, lSecond);
}

void xTimerPieceView::SetFormat(int nFormat)
{
    m_nFormat = nFormat;
}

void xTimerPieceView::SetMaxTick(long lMaxTick)
{
    m_lMaxTick = lMaxTick;
}

void xTimerPieceView::SetStartDuration(long lDuration)
{
    m_lBornDuration = lDuration;
}

void xTimerPieceView::ResetTimer()
{
    StopTimer();

    m_lDuration = 0;
    m_lBornDuration = 0;

    UpdateTimerText();
}

bool xTimerPieceView::IsTimerStart()
{
    return m_timer.IsTimerRuning();
}

long xTimerPieceView::GetMaxTick()
{
    return m_lMaxTick;
}

long xTimerPieceView::GetDurationTime()
{
    return m_lBornDuration + m_lDuration;
}

void xTimerPieceView::StartTimer()
{
    StopTimer();

    m_timer.SetTimer(1000, TIMER_METHOD(this, xTimerPieceView::OnTimer));

    m_tmBornTime = netGetLocalTime();
    m_lDuration = 0;

    UpdateTimerText();
}

void xTimerPieceView::UpdateDuration()
{
    if (m_tmBornTime <= 0)
    {
        return;
    }

    m_lDuration = difftime(netGetLocalTime(), m_tmBornTime);

    if (m_lMaxTick > 0 && GetDurationTime() > m_lMaxTick)
    {
        m_lDuration = m_lMaxTick - m_lBornDuration;
    }
}

void xTimerPieceView::PauseTimer()
{
    m_timer.KillTimer();

    UpdateDuration();

    m_lBornDuration += m_lDuration;
    m_lDuration = 0;
    m_tmBornTime = 0;

    UpdateTimerText();
}

void xTimerPieceView::ResumeTimer()
{
    m_timer.SetTimer(1000, TIMER_METHOD(this, xTimerPieceView::OnTimer));

    m_tmBornTime = netGetLocalTime();

    UpdateTimerText();
}

void xTimerPieceView::StopTimer()
{
    if (!m_timer.IsTimerRuning())
    {
        return;
    }

    m_timer.KillTimer();

    if (0 != m_tmBornTime)
    {
        UpdateDuration();

        m_lDuration += m_lBornDuration;
        m_lBornDuration = 0;
        m_tmBornTime = 0;

        UpdateTimerText();
    }
}

void xTimerPieceView::OnAppearSlot(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                   LPVOID pData)
{
    if (0 == wParam)
    {
        StopTimer();
    }
}

void xTimerPieceView::OnTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    UpdateDuration();

    UpdateTimerText();

    if (m_lMaxTick > 0 && GetDurationTime() >= m_lMaxTick)
    {
        StopTimer();
    }
}

void xTimerPieceView::UpdateTimerText()
{
    yl::string strTime;

    long lDuration = GetDurationTime();
    long lHour = lDuration / 3600;

    if (TIMER_PIECE_FORMAT_HMS == m_nFormat
            || (TIMER_PIECE_FORMAT_HMS == (TIMER_PIECE_FORMAT_HMS & m_nFormat)
                && 0 != lHour))
    {
        strTime = FormatHMS(lDuration);
    }
    else
    {
        strTime = FormatMS(lDuration);
    }

    SetText(strTime);
}
