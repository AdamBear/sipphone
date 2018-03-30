#include "talklogic_inc.h"

#ifdef IF_BT_SUPPORT_PHONE
CBTFinishedRoutine::CBTFinishedRoutine()
{
}


CBTFinishedRoutine::~CBTFinishedRoutine()
{
}

void CBTFinishedRoutine::AfterRoutineCreate()
{
    int iHour = 0;
    int iMin = 0;
    int iSec = 0;

    // 如果不能格式化出通话时长则说明是呼叫失败信息
    bool bError = (sscanf(m_pCallInfo->m_errorInfo.strError.c_str(), "%2d:%2d:%2d", &iHour, &iMin, &iSec) != 3);
    m_pCallInfo->m_bFinishedError = bError;

    // 如果格式化没出错,则是通话时长,需要保存
    if (!bError)
    {
        m_pCallInfo->SetCallDuration(iHour * 3600 + iMin * 60 + iSec);
    }

    // 进到callfinished界面就把通话停掉 bugid:17308
    StopTalk();
}

#endif // IF_BT_SUPPORT_PHONE
