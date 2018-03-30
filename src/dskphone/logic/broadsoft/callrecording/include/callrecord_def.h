#ifndef __CALLRECORD_DEF_H__
#define __CALLRECORD_DEF_H__

#include "feature/include/featurecommon.h"

enum CALL_RECORDING_MODE
{
    CRM_NEVER = 0,
    CRM_ALWAYS,
    CRM_ALWAYS_PAUSE_RESUME,
    CRM_DEMAND,
    CRM_DEMAND_USER_START,
};

enum CR_PAUSERESUME_METHOD
{
    CPM_NONE = 0,
    CPM_BEEP,
    CPM_ANNOUNCEMENT,
};

struct SCallRecord
{
    int nRecordMode;
    int nPauseResumeMethod;
    bool bPlayStartStopAnnouncement;
    bool bRecordVM;

    bool bPlayTone;
    yl::string strPlayToneTime;

    SCallRecord()
    {
        bPlayStartStopAnnouncement = false;
        bRecordVM = false;
        bPlayTone = false;
        strPlayToneTime = "";

        nRecordMode = 0;
        nPauseResumeMethod = 0;
    }
};

#endif // __CALLRECORD_DEF_H__


