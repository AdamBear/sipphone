#ifndef __MOD_CALLRECORDING_H__
#define __MOD_CALLRECORDING_H__

#include <ylstring.h>
#include "callrecord_def.h"

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool callRecording_StartRecord(int iCallId, int iAccountId, bool isNetworkConf = false);

bool callRecording_StopRecord(int iCallId, int iAccountId, bool isNetworkConf = false);

bool callRecording_PauseRecord(int iCallId, int iAccountId, bool isNetworkConf = false);

bool callRecording_ResumeRecord(int iCallId, int iAccountId, bool isNetworkConf = false);

void callRecord_SetRecordMode(int iAccountId, CALL_RECORDING_MODE eMode);

CALL_RECORDING_MODE callRecord_GetRecordMode(int iAccountId);

bool callRecord_IsCallRecordOn(int iAccountId);

bool callRecord_SetServerRecordMode(SCallRecord sRecordInfo, int iAccountId);

bool callRecord_QueryServerRecordMode(int iAccountId);

void callRecord_GetServerRecordMode(SCallRecord & RecordInfo, int iAccountId);

void callRecord_CancelRequest();

bool callRecord_IsSipRecordMode();
#endif

#endif
