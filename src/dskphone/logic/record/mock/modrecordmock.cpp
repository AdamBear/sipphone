#include "modrecordmock.h"
#include "modrecord.h"

MockRecord * g_pMockRecord = NULL;

bool modRecord_IsRecording(RECORD_TYPE eRecordType = RECORD_NONE)
{
    return g_pMockRecord ? g_pMockRecord->IsRecording(eRecordType) : false;
}

bool modRecord_IsPlaying(RECORD_TYPE eRecordType = RECORD_NONE)
{
    return g_pMockRecord ? g_pMockRecord->IsPlaying(eRecordType) : false;
}
