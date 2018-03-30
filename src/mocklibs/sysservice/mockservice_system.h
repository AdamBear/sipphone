#ifndef __MOCK_MK_SERVICE_H__
#define __MOCK_MK_SERVICE_H__

#include <gmock/gmock.h>

#include <cstring>
#include <service_system.h>


class MockSysService
{
public:
    MOCK_METHOD1(GetSysTime, int(sys_Time_t * time));
};

extern MockSysService * g_pMockSysService;

ACTION_TEMPLATE(SetSysTimeType, HAS_1_TEMPLATE_PARAMS(unsigned, uIndex), AND_2_VALUE_PARAMS(uGMTTime, lTimeZoneOffset))
{
    time_t tLocalTime = uGMTTime + lTimeZoneOffset;
    struct tm * pTm = gmtime(&tLocalTime);
    std::memcpy(&(std::get<uIndex>(args)->sys_tm), pTm, sizeof(struct tm));
    std::get<uIndex>(args)->sys_tm.tm_gmtoff = lTimeZoneOffset;
}

#define EXPECT_GET_SYS_TIME(gmt_time, timezone_offset) \
        EXPECT_CALL(*g_pMockSysService, GetSysTime(testing::_))\
            .WillRepeatedly(testing::DoAll(SetSysTimeType<0>(gmt_time, timezone_offset), testing::Return(0)))

#endif // !__MOCK_MK_SERVICE_H__
