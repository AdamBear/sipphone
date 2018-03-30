#ifndef _MOD_COMMON_UNITS_H_
#define _MOD_COMMON_UNITS_H_

#include "commonunits_def.h"
#include "contacts/directory/include/directorystructure.h"
#include <yllist.h>
#include <ylstring.h>
#include "debugfunc.h"

#if IF_BUG_30625
#include "netmsgtypedefine.h"
#endif

/*******************************************************************
** 函数描述:   初始化各公用单元
** 参数：      [in] args1：arg description
** 返回:       无
********************************************************************/
void commonUnits_Init();

/*******************************************************************
** 函数描述:     获取默认铃声路径
** 参数：        无
** 返回:          无
********************************************************************/
VOID commonUnits_GetDefaultRing(yl::string & strRingPath);


yl::string commonUnit_SearchRingPath(const yl::string & strRingFileName,
                                     const yl::string & strDefaultFileName = "Ring1.wav");

/*******************************************************************
** 函数描述:     是否*、#呼出键
** 参数：        按键值
** 返回:      TRUE:是, FALSE:否
********************************************************************/
BOOL commonUnits_IsSendDigitKey(int iKey);

/*******************************************************************
** 函数描述:     发送模拟按键
** 参数：        按键值
** 返回:      1:成功, 0:失败
********************************************************************/
int commonUnits_SendSimulateKey(UINT uMessage, int iKey, bool bExp = false);

// 获取路径的真实路径.
// | strPath | [in out] 是传入的路径, 以及转换后的路径. 可以为完全路径,
// 也可以为隐藏的路径.
// | eType | [in] 是文件类型.
// 返回替换后的路径.
yl::string commonUnits_Hide2Real(yl::string & strPath, PHFileType eType);

// 获取路径的隐藏路径.
// | strPath | [in out] 是传入的路径, 以及转换后的路径. 可以为完全路径,
// 也可以为隐藏的路径.
// | eType | [in] 是文件类型.
// 返回隐藏路径.
yl::string commonUnits_Real2Hide(yl::string & strPath, PHFileType eType);

/*******************************************************************
** 函数描述:    恢复出厂
** 参数：        ResetOptionType 恢复类型
** 返回:
** 输入:
********************************************************************/
void commonUnits_ResetToFatory(int eOption = RESET_OPTION_ALL);

/*******************************************************************
** 函数描述:    判断是否是测试模式前缀
** 参数：
** 返回:      TRUE:是测试模式前缀,FALSE:不是测试模式前缀
********************************************************************/
bool commonUnits_IsTestModePrefix(yl::string strNumber);

/*******************************************************************
** 函数描述:    获取测试模式类型
** 参数：
** 返回:      测试模式类型
********************************************************************/
int commonUnits_GetTestModeType(yl::string strNumber);

/*******************************************************************
** 函数描述:    重启
** 参数：        延迟时间
** 返回:      无
********************************************************************/
void commonUnits_Reboot(int iDelayTime = 4);

/*******************************************************************
** 函数描述:    启用异常时的BackTrace
** 参数：
** 返回:      无
********************************************************************/
void commonUnits_EnableMiniDump();

/*******************************************************************
** 函数描述: 设置恢复模式状态
** 参数：无
** 返回：无
*/
Factory_State commUnits_GetResetFactoryState();

/*******************************************************************
** 函数描述: 设置是否能够进入恢复模式
** 参数：无
** 返回：无
*/
bool commUnits_SetResetFactoryEnable(bool bEnable);

/*******************************************************************
** 函数描述: 获取userenv 字符串
** 参数：strFlag flag字段
** 返回：Value
*/
yl::string commUnits_GetUserEnvString(const yl::string & strFlag);

/*******************************************************************
** 函数描述:    对输入的URL地址进行编码，参照javasript的encodeURI规则
** 参数：        [in] strURL:    待编码的URL地址,传引用
** 返回:      无
********************************************************************/
void commonUnits_EncodeURI(yl::string & strURL);

/*******************************************************************
** 函数描述:    对输入的URL地址进行编码
** 参数：        [in] strURL:    待编码的URL地址,传引用
** 返回:      无
** 不会转换的字符：0-9,abc...xyz,ABC...XYZ, 斜杠，冒号，小数点
********************************************************************/

void commonUnits_EncodeAutopURI(yl::string & strURL);

// 辅助函数
// 从lpCfgContent中查找lpKey,形式lpKey=value&,
//以整型值的形式取出value,如果成功取得value值,则返回TRUE,否则返回FALSE
bool commonUnits_GetIntValue(LPCSTR lpCfgContent, LPCSTR lpKey, int & iOutValue);

// 从lpCfgContent中查找lpKey,形式lpKey=value&,
//以字符串形式取出value,如果lpKey存在,则返回TRUE,否则返回FALSE,不关心value是否为空
bool commonUnits_GetStringValue(LPCSTR lpCfgContent, LPCSTR lpKey, LPSTR lpOut);

/*******************************************************************
** 函数描述:    对号码URI进行解析
** 参数：        [in] strURI:  呼出的URI
                [out] strNumber: 获取到的号码
                [out] strServer: 获取到的服务器
** 返回:      int 协议类型（当前无用）
********************************************************************/
void commonUnits_ParserDialURI(const yl::string & strURL, yl::string & strNumber,
                               yl::string & strServer);

// 根据不同的机型,获取不同的配置文件名
yl::string commonUnits_GetConfigFileByPhone();

#if IF_FEATURE_AUTOP_IME
//根据配置项的值获取对应IME输入法
yl::string commonUnits_GetIMEByType(const yl::string strIMEType);
#endif

bool commonUnits_SetIMEType(yl::string strIMEType, const yl::string strIME);

#if IF_BUG_30625
bool commonUnits_GetWanThirdDNS(yl::string & strResult, IPAddrType addrType/* = IAT_IPv4 */);
#endif

#if IF_BUG_25367
// 输入为10个纯数字,则显示的格式为xx xxxx xxxx
void commonUnits_FormatNumber(yl::string & strText);
#endif

bool commonUnits_System(LPCSTR lpCmdString);

// 兼容旧版本的softkey layout
bool commonUnits_GetBackupSoftkey(yl::string & strFileName, yl::string & strStatus);

bool commonUnits_IsSoftkeyEnable(yl::string & strFileName, yl::string & strStatus);

//判断字符串是否为正整数(0也合法)
bool commonUnits_PositiveInteger(yl::string & strInput);

#if IF_FEATURE_PAGING
//判断是否是合法的paging channel
bool commonUnits_IsValidPagingChannel(yl::string & strChannel);
#endif

void commonUnits_SetSystemMaintenance(bool bEnable);

yl::string commonUnits_PwAesDataEncrypt(yl::string & strSrcData, const char * lpKey);
yl::string commonUnits_PwAesDataDecrypt(yl::string & strSrcData, char * lpKey);

//判断是否为系统维护状态
bool commonUnits_IsSystemMaintenanceStatus();

//判断机型是否支持
bool commonUint_IsDevSupport(LPCSTR lpDevName);

//转换通配符
yl::string commUnit_ReplaceWildcard(yl::string & strIn);

//mac 地址是否符合进入测试模式
bool commonUnit_IsMatchTestMode(const yl::string & strMac = "");

#ifdef IF_SUPPORT_VIDEO
// 是否允许音视频转换
bool commonUnits_EnableAVSwitch();
#endif //#ifdef IF_SUPPORT_VIDEO

// 设置FWD配置信息
int commonUnits_SetForwardCfg(const char * cszConfig);

// index 下标从0开始
bool commonUnits_GetCustomLabel(yl::string & result, const yl::string & strLabelSet,
                                const yl::string & strDefLabel, int index);

// 删除初始化信息文件
void comminUnits_DeleteInitInfofile();

// 追加填写初始化信息
void comminUnits_WriteInitInfo(const char * pszInitInfo);

AMR_LICENSE_RESULT comminUnits_GetAmrLicenseResult();

yl::string commonUnits_Time2String(time_t tTime);

#ifdef IF_FEATURE_DIAGNOSE
yl::string commonUnits_GetUpTime();

bool commonUnits_GetMemInfo(int & nTotal, int & nFree, int & nBuff, int & nCached);
#endif //IF_FEATURE_DIAGNOSE

// 是否为组播IP
bool commonUnits_BeMulticastIP(const yl::string & strIPPort);

/**
 * @brief 增加Log 回调.
 *
 * @author Song
 * @date 2017/12/21
 *
 * @param pFunc The function.
 *
 * @return True if it succeeds, false if it fails.
 */

bool commonUnits_AddSysLogFunc(FuncSetSysLog pFunc);

/**
 * @brief 移除Log回调
 *
 * @author Song
 * @date 2017/12/21
 *
 * @param pFunc The function.
 *
 * @return True if it succeeds, false if it fails.
 */

bool commonUnits_RemoveSysLogFunc(FuncSetSysLog pFunc);

/**
 * @brief 初始化Log 模块.
 *
 * @author Song
 * @date 2017/12/21
 *
 * @return An int.
 */

void commonUnits_InitLog(int argc, char * argv[]);

int commonUnits_GetLogLevel(SYSTEM_LOG_TYPE eType = LOG_AUTO_MODE);
DSK_PHONE_TYPE commonUnits_GetDskPhoneType();

/**
 * @brief 修改话机用户密码
 * @return true/false
 */
bool commonUnits_SetUserPSWD(const char* szUser, const char* szPwd);

#endif // _MOD_COMMON_UNITS_H_
