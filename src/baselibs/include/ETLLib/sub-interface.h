#ifndef __SUB_INTERFACE_H__
#define __SUB_INTERFACE_H__

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化ETL Log.
 *
 * @author Song
 * @date 2018/1/22
 *
 * @param mode      The mode.
 * @param call_back The call back.
 *
 * @return An int.
 */

int etlInitLog(int mode, etllib_log_callback call_back);

int etlSetlogLevel(int mode, int level);

DEFINE_HANDLE(HINIFILE);

BOOL etlGetProfileString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                         LPSTR lpszBuffer, LPCSTR lpszDefaultValue);
BOOL etlGetProfileStringEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                           LPSTR lpszBuffer, LPCSTR lpszDefaultValue, CONFIG_LEVEL_TYPE eType);


int etlGetProfileInt(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                     int nDefaultValue);
int etlGetProfileIntEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                       int nDefaultValue, CONFIG_LEVEL_TYPE eType);

double etlGetProfileFloat(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                          double lfDefaultValue);
double etlGetProfileFloatEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                            double lfDefaultValue, CONFIG_LEVEL_TYPE eType);

void etlSetProfileString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                         LPCSTR lpszValue);
void etlSetProfileStringEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                           LPCSTR lpszValue, CONFIG_LEVEL_TYPE eType);

void etlSetProfileInt(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                      int nValue);
void etlSetProfileIntEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                        int nValue, CONFIG_LEVEL_TYPE eType);

void etlSetProfileFloat(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                        double lfValue);
void etlSetProfileFloatEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                          double lfValue, CONFIG_LEVEL_TYPE eType);


// registry object
DEFINE_HANDLE(HREGISTRY);
typedef HREGISTRY       HSECTION;

HREGISTRY registryOpen(LPCSTR lpszFileName, LPCSTR lpszSectionName);
BOOL registryFlush(HREGISTRY hRegistry);

//强制同步写文件
BOOL registrySyncFlush(HREGISTRY hRegistry);

//刷新一下注册表
BOOL registryFlushConfig(HREGISTRY hRegistry);

BOOL registryGetString(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, LPSTR lpszBuffer,
                       LPCSTR lpszDefaultValue);

BOOL registryGetStringEx(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, LPSTR lpszBuffer,
                         LPCSTR lpszDefaultValue, CONFIG_LEVEL_TYPE eType);

BOOL registrySetString(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, LPCSTR lpszValue);
BOOL registrySetStringEx(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, LPCSTR lpszValue,
                         CONFIG_LEVEL_TYPE eType);

int registryGetInt(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, int nDefaultValue);
int registryGetIntEx(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, int nDefaultValue,
                     CONFIG_LEVEL_TYPE eType);
BOOL registrySetInt(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, int nValue);
BOOL registrySetIntEx(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, int nValue,
                      CONFIG_LEVEL_TYPE eType);

double registryGetFloat(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, double fDefaultValue);
double registryGetFloatEx(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, double fDefaultValue,
                          CONFIG_LEVEL_TYPE eType);
BOOL registrySetFloat(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, double fValue);
BOOL registrySetFloatEx(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, double fValue,
                        CONFIG_LEVEL_TYPE eType);

// 更新配置项的类型
BOOL registryUpdateConfig(HREGISTRY hRegistrySection, LPCSTR lpszKeyName,
                          CONFIG_LEVEL_TYPE eNewType);

//清除配置项
BOOL registryRemoveConfig(HREGISTRY hRegistrySection, LPCSTR lpszKeyName, CONFIG_LEVEL_TYPE eType,
                          LPSTR lpszBuffer, LPCSTR lpszDefaultValue);

// 获取当前配置生效级别
CONFIG_LEVEL_TYPE registryGetCurrentLevel(HREGISTRY hRegistrySection, LPCSTR lpszKeyName,
        CONFIG_LEVEL_TYPE eDefaultType);

void registryUpdateProtectStatus(BOOL bOpen);

BOOL regMonitorSection(HREGISTRY hRegistrySection);
void regUnmonitorSection(HREGISTRY hRegistrySection);
// save changed item to file
BOOL etlRegFlushAllFile();

BOOL etlUpdateProfileLevel(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                           CONFIG_LEVEL_TYPE eNewType);

// single registry item operator
BOOL etlGetRegistryString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                          LPSTR lpszBuffer, LPCSTR lpszDefaultValue);
BOOL etlGetRegistryStringEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                            LPSTR lpszBuffer, LPCSTR lpszDefaultValue, CONFIG_LEVEL_TYPE eType);

BOOL etlSetRegistryString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                          LPCSTR lpszValue, CONFIG_LEVEL_TYPE eType);

int etlGetRegistryInt(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                      int nDefaultValue);
int etlGetRegistryIntEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                        int nDefaultValue, CONFIG_LEVEL_TYPE eType);

BOOL etlSetRegistryInt(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                       int nValue, CONFIG_LEVEL_TYPE eType);

double etlGetRegistryFloat(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                           double lfDefaultValue);
double etlGetRegistryFloatEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                             double lfDefaultValue, CONFIG_LEVEL_TYPE eType);

BOOL etlSetRegistryFloat(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                         double lfValue, CONFIG_LEVEL_TYPE eType);
CONFIG_LEVEL_TYPE etlGetProfileCurrentLevel(LPCSTR lpszConfigFile, LPCSTR lpszSectionName,
        LPCSTR lpszKeyName, CONFIG_LEVEL_TYPE eDefaultValue);

BOOL etlRemoveProfileLevel(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                           CONFIG_LEVEL_TYPE eType);
// ini file object
HINIFILE profileOpen(LPCSTR lpszConfigFile);
void profileClose(HINIFILE hIniFile);
BOOL profileGetString(HINIFILE hIniFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                      LPSTR lpszBuffer, LPCSTR lpszDefaultValue);
int profileGetInt(HINIFILE hIniFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName, int nDefaultValue);

LPCSTR etlGetProfileTempString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                               LPCSTR lpszDefaultValue);
LPCSTR etlGetProfileTempStringEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                                 LPCSTR lpszDefaultValue, CONFIG_LEVEL_TYPE eType);
BOOL etlGetProfileStringLimitLen(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                                 LPSTR lpszBuffer, int nBufferLen, LPCSTR lpszDefaultValue);
BOOL etlGetProfileStringLimitLenEx(LPCSTR lpszConfigFile, LPCSTR lpszSectionName,
                                   LPCSTR lpszKeyName, LPSTR lpszBuffer, int nBufferLen, LPCSTR lpszDefaultValue,
                                   CONFIG_LEVEL_TYPE eType);

BOOL nameHelperLock();
LPVOID nameGetSystemObject(LPCSTR lpszObjName, int nSize, BOOL * pbFirstAlloc);
void nameHelperUnlock();


#ifdef  __cplusplus
}
#endif


#ifdef  __cplusplus

HREGISTRY regSection2HANDLE(LPVOID pSection);


void regSaveRegistryDocument(HREGISTRY hRegistry);

typedef int(*regFlushCallBack)(LPCSTR lpszFile, LPCSTR lpszSec, LPCSTR lpszKey, LPCSTR lpszValue,
                               CONFIG_LEVEL_TYPE eType);

void regSaveRegistryDocumentEx(HREGISTRY hRegistry, regFlushCallBack funcCallBack);

#if IF_USE_REGISTRY
void regSaveRegistryDocumentEx(regDocument * pDocument, regFlushCallBack funcCallBack);
void regSaveRegistryDocument(regDocument * pDocument);
regSection * regHANDLE2Section(HREGISTRY hSection);
#endif // IF_USE_REGISTRY

#if IF_EXPORT_SYMBOLS
#include "sub-interface.cpp"
#endif

#endif //__cplusplus

#endif //__SUB_INTERFACE_H__
