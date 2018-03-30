#ifndef __MOD_COMMON_UNITS_MOCK_H__
#define __MOD_COMMON_UNITS_MOCK_H__

#include <gmock/gmock.h>

#include <ylstring.h>

// modcommonunits.h接口mock
class CommonUnitsMock
{
public:
    MOCK_METHOD3(SendSimulateKey, int(unsigned int, int, bool));
    MOCK_METHOD1(GetDefaultRing, void(yl::string & strRingPath));
};

// 全局mock实例指针
extern CommonUnitsMock * g_pCommonUnitsMock;

#endif
