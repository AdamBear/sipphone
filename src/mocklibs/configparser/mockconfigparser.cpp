#include "mockconfigparser.h"

// 使用时不判断g_pMockConfigParser是否为空，let it crash
MockConfigParser * g_pMockConfigParser;

int configParser_GetConfigInt(const char * lpszConfigName,
                              CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->GetConfigInt(lpszConfigName, eType);
}

bool configParser_SetConfigInt(const char * lpszConfigName, int nValue,
                               CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->SetConfigInt(lpszConfigName, nValue, eType);
}

yl::string configParser_GetConfigString(const char * lpszConfigName,
                                        CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->GetConfigString(lpszConfigName, eType);
}

bool configParser_SetConfigString(const char * lpszConfigName, const yl::string & strValue,
                                  CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->SetConfigString(lpszConfigName, strValue.c_str(), eType);
}

int configParser_GetCfgItemIntValue(const char * lpszConfigName, int itemIndex,
                                    CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->GetCfgItemIntValue(lpszConfigName, itemIndex, eType);
}

bool configParser_SetCfgItemIntValue(const char * lpszConfigName, int index,
                                     int value, CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->SetCfgItemIntValue(lpszConfigName, index, value, eType);
}

yl::string configParser_GetCfgItemStringValue(const char * lpszConfigName, int itemIndex,
        CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->GetCfgItemStringValue(lpszConfigName, itemIndex, eType);
}

bool configParser_SetCfgItemStringValue(const char * lpszConfigName, int index,
                                        const yl::string & strValue, CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    return g_pMockConfigParser->SetCfgItemStringValue(lpszConfigName, index, strValue.c_str(), eType);
}

yl::string configParser_GetDefaultValueString(const char * lpszConfigName)
{
    return g_pMockConfigParser->GetDefaultValueString(lpszConfigName);
}

void EXPECT_CALL_GetConfigInt(const char * cszConfigName, int iReturn)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        GetConfigInt(testing::StrEq(cszConfigName), testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(iReturn));
}

void EXPECT_CALL_SetConfigInt(const char * cszConfigName, int iValue)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        SetConfigInt(testing::StrEq(cszConfigName), iValue, testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));
}

void EXPECT_CALL_GetConfigString(const char * cszConfigName, const char * cszReturn)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        GetConfigString(testing::StrEq(cszConfigName), testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(cszReturn));
}

void EXPECT_CALL_SetConfigString(const char * cszConfigName, const char * cszValue)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        SetConfigString(testing::StrEq(cszConfigName), testing::StrEq(cszValue), testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));
}

void EXPECT_CALL_GetCfgItemIntValue(const char * cszConfigName, int iIndex, int iReturn)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        GetCfgItemIntValue(testing::StrEq(cszConfigName), iIndex, testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(iReturn));
}

void EXPECT_CALL_SetCfgItemIntValue(const char * cszConfigName, int iIndex, int iValue)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        SetCfgItemIntValue(testing::StrEq(cszConfigName), iIndex, iValue, testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));
}

void EXPECT_CALL_GetCfgItemStringValue(const char * cszConfigName, int iIndex,
                                       const char * cszReturn)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        GetCfgItemStringValue(testing::StrEq(cszConfigName), iIndex, testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(cszReturn));
}

void EXPECT_CALL_SetCfgItemStringValue(const char * cszConfigName, int iIndex,
                                       const char * cszValue)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        SetCfgItemStringValue(testing::StrEq(cszConfigName), iIndex, testing::StrEq(cszValue), testing::_)
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));
}

void EXPECT_CALL_GetDefaultValueString(const char * cszConfigName, const char * cszReturn)
{
    EXPECT_CALL(
        *g_pMockConfigParser,
        GetDefaultValueString(testing::StrEq(cszConfigName))
    ).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(cszReturn));
}
