#ifndef __MOCK_CONFIG_PARSER_H__
#define __MOCK_CONFIG_PARSER_H__

#include <gmock/gmock.h>

#include <modconfigparser.h>
#include <configiddefine.h>

class MockConfigParser
{
public:
    MOCK_METHOD2(GetConfigInt, int(const char * lpszConfigName,
                                   CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD3(SetConfigInt, bool(const char * lpszConfigName, int nValue,
                                    CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD2(GetConfigString, yl::string(const char * lpszConfigName,
                 CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD3(SetConfigString, bool(const char * lpszConfigName, const char * lpszValue,
                                       CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD3(GetCfgItemIntValue, int(const char * lpszConfigName, int itemIndex,
                                         CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD4(SetCfgItemIntValue, bool(const char * lpszConfigName, int index,
                                          int value, CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD3(GetCfgItemStringValue, yl::string(const char * lpszConfigName, int itemIndex,
                 CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD4(SetCfgItemStringValue, bool(const char * lpszConfigName, int index,
                 const char * cszValue, CONFIG_LEVEL_TYPE eType));
    MOCK_METHOD1(GetDefaultValueString, yl::string(const char * lpszConfigName));
};

void EXPECT_CALL_GetConfigInt(const char * cszConfigName, int iReturn);
void EXPECT_CALL_SetConfigInt(const char * cszConfigName, int iValue);
void EXPECT_CALL_GetConfigString(const char * cszConfigName, const char * cszReturn);
void EXPECT_CALL_SetConfigString(const char * cszConfigName, const char * cszValue);
void EXPECT_CALL_GetCfgItemIntValue(const char * cszConfigName, int iIndex, int iReturn);
void EXPECT_CALL_SetCfgItemIntValue(const char * cszConfigName, int iIndex, int iValue);
void EXPECT_CALL_GetCfgItemStringValue(const char * cszConfigName, int iIndex,
                                       const char * cszReturn);
void EXPECT_CALL_SetCfgItemStringValue(const char * cszConfigName, int iIndex,
                                       const char * cszValue);
void EXPECT_CALL_GetDefaultValueString(const char * cszConfigName, const char * cszReturn);

extern MockConfigParser * g_pMockConfigParser;

#endif // !__MOCK_CONFIG_PARSER_H__
