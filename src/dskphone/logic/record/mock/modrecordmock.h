#ifndef __MOD_RECORD_MOCK_H__
#define __MOD_RECORD_MOCK_H__

#include <gmock/gmock.h>

#include "common.h"

class MockRecord
{
public:
    MOCK_METHOD1(IsRecording, bool(RECORD_TYPE eRecordType));
    MOCK_METHOD1(IsPlaying, bool(RECORD_TYPE eRecordType));
};

extern MockRecord * g_pMockRecord;

#endif // __MOD_RECORD_MOCK_H__
