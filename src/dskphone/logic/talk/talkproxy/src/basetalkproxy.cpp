#include "basetalkproxy.h"

CBaseTalkASyncProxy::CBaseTalkASyncProxy(int iSessionID) : m_iSessionID(iSessionID)
{
    SetCallBack(MAKE_TIMER_CALL_BACK(CBaseTalkASyncProxy::CallBack));
}
