#include "deletelater.h"
#include "ETLLib.hpp"

bool app::CBaseDeleteLater::OnTimeOut(msgObject& msg)
{
    // timer error should delete too when async to delete
    if (msg.wParam == TIMER_ID(m_uInterval)
            || IsError())
    {
        destory();
        return true;
    }

    return false;
}
