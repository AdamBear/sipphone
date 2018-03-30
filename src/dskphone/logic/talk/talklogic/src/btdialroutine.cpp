#include "talklogic_inc.h"

#ifdef IF_BT_SUPPORT_PHONE

CBTDialRoutine::CBTDialRoutine()
{
}


CBTDialRoutine::~CBTDialRoutine()
{
}

bool CBTDialRoutine::CallOut(const yl::string& strNumber, yl::string strName /*= ""*/,
                             int iCallOutTriggerMask /*= COT_DEFAULT*/, bool bAudioOnly /*= true*/)
{
    if (strNumber.empty())
    {
        return false;
    }

    return _BTMobileManager.ProcessCallOut(m_pCallSession, m_pCallInfo, strNumber, strName);
}
#endif // IF_BT_SUPPORT_PHONE
