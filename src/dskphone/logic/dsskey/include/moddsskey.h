#ifndef _DSSKEY_MOD_HEDADER_
#define  _DSSKEY_MOD_HEDADER_

#include "dsskey_enumtypes.h"
#include "dsskeystructure.h"
#include <devicelib/phonedevice.h>
#include "dsskey_def.h"
#include <ETLLib/ETLLib.hpp>

struct CallOut_Param;
class CDSSkeyData;

/************************************************************************/
// 函数描述  ： Dsskey模块初始化
// 参数说明 ： 无
// 返回值 ： 无
/************************************************************************/
bool dsskey_Init();

//程序结束时释放资源
bool dsskey_UnInit();

/************************************************************************/
// 函数描述  ： 按键处理
// 参数说明 ：
//     [iDssKeyID] DssKey的ID
//     [iDssKeyIndex] Dsskey的index
// 返回值 ： 无
/************************************************************************/
bool dsskey_OnDsskeyClick(int iDssKeyID, int iDssKeyIndex = 0, bool bCheckNeedPassword = true);

/************************************************************************/
// 接口  ： dsskey_OnDssKeyLongClick(int iDssKeyID)
// 功能  ： Linekey按键长按的处理
// 参数说明 ： [iDssKeyID] DssKey的ID
// 返回值 ： 无
/************************************************************************/
bool dsskey_OnDssKeyLongClick(int iDssKeyID);


/************************************************************************/
// 函数描述  ： 更新dsskey状态
// 参数说明 ：
//     [iDssKeyID] DssKey的ID
//     [eStatus] DssKey的状态
// 返回值 ： 更新是否成功
/************************************************************************/
bool dsskey_UpdateStatus(int iDssKeyID, Dsskey_Status eStatus, Dsskey_Status eStart,
                         Dsskey_Status eEnd, bool bUpdate = true);

// 移除状态
bool dsskey_RemoveStatus(int iDsskeyID, Dsskey_Status eStart, Dsskey_Status eEnd,
                         bool bUpdate = true);

#ifdef IF_FEATURE_MULTIFWD
// 是否Linekey类型的按键
bool dsskey_IsLineKey(int iDssKeyID);
#endif

/**
 * @brief Dsskey is exponent module.
 *
 * @author Song
 * @date 2017/8/21
 *
 * @param eModuleType Type of the module.
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsskey_IsExpModule(DSSKEY_MODULE_TYPE eModuleType);

/**
 * @brief Dsskey is support module type.
 *
 * @author Song
 * @date 2017/8/21
 *
 * @param eModuleType Type of the module.
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsskey_IsSupportModuleType(DSSKEY_MODULE_TYPE eModuleType);

//===========================================================================
//获取DssKey属性的状态
bool dsskey_GetData(DSSKEY_MODULE_TYPE eType, int iBeginKey, int iNum,
                    YLList<DssKeyLogicData> & listData, int iEXPIndex = 0);

//获取DssKey属性的状态
bool dsskey_GetData(int nDsskeyID, DssKeyLogicData & sKeyData);

/*******************************************************************
** 函数描述:   更新DND状态
** 参数：      [in] bDndOn: DND开启状态
** 返回:       空
********************************************************************/
void dsskey_UpdateDNDStatus(bool bDndOn);

/*******************************************************************
** 函数描述:   更新FWD状态
** 参数：      [in] dsskeyID: DSSKey ID
** 参数：      [in] bFwdOn: FWD开启状态
** 返回:       空
********************************************************************/
void dsskey_UpdateFWDStatus(int dsskeyID, bool bFwdOn);

#ifdef IF_FEATURE_MULTIFWD
// 更新FWD状态
void dsskey_UpdateFWDStatus(yl::string & strTarget, bool bFwdOn);
#endif

/*******************************************************************
** 函数描述:   是否SCA类型的linekey
** 参数：      [in] iKeyIndex: linekey序号，0~x
** 返回:       TRUE: 是；FALSE: 否
********************************************************************/
bool dsskey_IsSCATypeLinekey(int dsskeyID);

/*******************************************************************
** 函数描述:   当DND或FWD状态有改变时更新Dsskey
** 参数：      [in] iAccount: 账号ID
** 返回:       TRUE: 更新成功；FALSE: 更新失败
********************************************************************/
bool dsskey_UpdateStatusByDNDOrFWDChanged(int iAccount = -1);

/*******************************************************************
** 函数描述:   设置linekey的callpark状态
** 参数：      [in] iAccount: 账号ID
               [in] bOn: 是否开启SCA CallPark
** 返回:       TRUE: 更新成功；FALSE: 更新失败
********************************************************************/
void dsskey_ProcessCallParkNotify(int iAccountID, bool bOn);

//获取dsskey数量
int dsskey_GetDSSKeyAmount();

//设置DSSKey数据
/*******************************************************************
** 函数描述:    设置DSSKey数据
** 参数：        [in] dsskeyID: dsskey id
                [in] dsskeyLogicData: dsskey数据
                [in] DsskeySetFlags:写入标志，含义见枚举定义
** 返回:      设置结果
********************************************************************/
bool dsskey_SetDsskeyData(int dsskeyID, const DssKeyLogicData & dsskeyLogicData,
                          int iFlags = DSSKEY_SET_NORMAL);

//将内存数据保存到文件
bool dsskey_SaveModuleDataToFile(DSSKEY_MODULE_TYPE eModuleType, int nModuleIndex);

//获取dsskey状态链表
bool dsskey_GetDsskeyStatus(int dsskeyID, YLList<Dsskey_Status> & listOutput);

//获取DSSKey状态
Dsskey_Status dsskey_GetDsskeyStatus(int dsskeyID);

//设置dsskey状态
bool dsskey_SetDsskeyStatus(int dsskeyID, Dsskey_Status status);

#if IF_BUG_XMLBROWSER_EXP_LED
bool dsskey_SetDsskeyLightType(int dsskeyID, LIGHT_Type l_Type);
#endif

bool dsskey_SetDsskeyStatusByType(DssKey_Type type, Dsskey_Status status, bool bExpProcess = true);

//设置DT――LINE的类型的数据的line状态，仅对dt_line类型有效
bool dsskey_SetLineStatus(int dsskeyID, Dsskey_Status lineStatus);

//获取某一个模块所有的DSSKey数据
bool dsskey_GetAllDsskeyDataByModule(YLList<int> & listOutput, DSSKEY_MODULE_TYPE moduleType,
                                     int iModuleIndex = 0, DssKey_Type eType = DT_NONE);

//获取所有的dsskey
bool dsskey_getAllDsskeys(YLList<int> & listOutput);

//获取指定类型下所有的dsskey
bool dsskey_getAllDsskeys(YLList<int> & listOutput, DSSKEY_MODULE_TYPE moduleType);

//获取所有的dsskey
bool dsskey_getAllDsskeys(YLList<int> & listOutput, DssKey_Type eType, bool bNative = false);

// 获取第iPageNum页的Dssskey列表
bool dsskey_GetPageDsskeys(YLList<int> & listOutput, DSSKEY_MODULE_TYPE moduleType,
                           int iModuleIndex = 0, int iPageNum = 0);

// 获取分页数
int dsskey_GetPageCountByMoudle(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex = 0);

// 每页Dssskey是否是否平均分布
bool dsskey_IsAverageDistribution(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex = 0);

// 是否区间内的dsskey都为eType
bool dsskey_IsSectionAllSameType(int iFrom, int iTo, DssKey_Type eType, DSSKEY_MODULE_TYPE moduleType, int iModuleIndex = 0);

// 获取区间
int dsskey_GetPageInfoByMoudle(int & iStartIndex, int & iEndIndex, DSSKEY_MODULE_TYPE moduleType,
                               int iModuleIndex = 0, int iPageNum = 0);

int dsskey_GetMaxDsskeyNumPerPage();

bool dsskey_IsLinekeyHavePageDsskey(DssKey_Type eExceptType = DT_NA);

//获取账号可用的dsskey
void dsskey_GetDsskeysByAccID(YLList<int> & listOutput, int accountID);


//获取共有多少DSSKey管理模块
int dsskey_GetModulesAmount();

//获取指定数据的DSSKey类型
DssKey_Type dsskey_GetDsskeyType(int dsskeyID, int iFlag = DSS_ATTR_NONE);

//获取DSSKey模块链表
void dsskey_GetModulesList(YLList<int> & listOutput);

//重新加载指定模块的数据
bool dsskey_ReloadDSSKeyData(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex = 0);

//获取dsskey的值
yl::string  dsskey_GetDsskeyValue(int dsskeyID, int iFlag = DSS_ATTR_NONE);

//获取dsskey标签
yl::string  dsskey_GetDsskeyLabel(int dsskeyID, int iFlag = DSS_ATTR_NONE);

//获取dsskey扩展字段
yl::string  dsskey_GetDsskeyExtension(int dsskeyID, int iFlag = DSS_ATTR_NONE);

//设置dsskey的值
bool dsskey_SetDsskeyValue(int dsskeyID, const yl::string & strValue);

//设置dsskey的标签
bool dsskey_SetDsskeyLabel(int dsskeyID, const yl::string & strValue);

//设置dsskey的扩展字段
bool dsskey_SetDsskeyExtension(int dsskeyID, const yl::string & strValue);

//是否为绑定类型
bool dsskey_IsBindType(DssKey_Type dsskeyType);

//根据参数生成dsskeyID
int dsskey_GetDsskeyID(DSSKEY_MODULE_TYPE moduleType, int iModuleIndex, int keyIndex);

//刷新指定类型的dsskey数据
//重新加载
bool dsskey_RefressDsskeyByType(DssKey_Type dsskeyType);

//获取呼出号码
bool dsskey_GetCallOutNumber(int nDsskeyID, yl::string & strCallOutNumber, void * pExtData = NULL,
                             int nExtType = 0);

//获取dsskey关联的line字段
//DT_LINE类型若设为auto，则返回第一个可用的账号ID
int dsskey_GetDsskeyLine(int nDsskeyID, int iFlag = DSS_ATTR_NONE);

//判定是否是dsskey按键
bool dsskey_IsDsskey(int iKeyCode, bool bExp = false);


//定时器处理，由于已经在内部消息处理模块注册了定时器的处理函数
//该接口已经无用
bool dsskey_OnTimer(unsigned int uTimer) DECLARE_ATTR;

//定义UI回调函数
typedef bool (*dsskey_callback)(int dsskeyID, Dsskey_Status dsskeyStatus,
                                const DssKeyLogicData * logicData,  void * pExtraData);

//注册UI回调函数
bool dsskey_setUpdateFunCallback(dsskey_callback callback);

//获取programekey链表,
bool dsskey_getProgrameKeyList(YLList<int> & listOutput, int getSize);

//刷新关联的dsskeyUI
bool dsskey_refreshDSSKeyUI(int dsskeyID, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL,
                            bool bCheckData = true);

//设置Dsskey类型
bool dsskey_SetDsskeyType(int dsskeyID, DssKey_Type type);

//获取指定按键的dsskeyID
int dsskey_GetDsskeyInfoByKeyIndex(int keyIndex);

//dsskey和通话绑定后的处理
bool dsskey_OnBindProcess(int dsskeyID);

//dsskey和通话解绑后的处理
bool dsskey_OnUnbindProcess(int dsskeyID, bool bUpdate = true);

//根据类型获取默认标签
const yl::string & dsskey_GetDefaultLabel(DssKey_Type type);

//设置定时器，每个数据只允许设置一个定时器
bool dsskey_SetTimer(int dsskeyID, UINT timerInterval);

//停止定时器
bool dsskey_KillTimer(int dsskeyID);

//获取关联的计时器ID
int dsskey_GetRelatedTimerID(int dsskeyID);

//设置扩展字段
//
bool dsskey_SetDssekeyExtFiled(int dsskeyID, const yl::string & strFieldName,
                               const yl::string & strFieldValue);

//获取扩展字段的值
//false 获取失败
//true 获取成功
bool dsskey_GetDssekeyExtFiled(int dsskeyID, const yl::string & strFieldName,
                               yl::string & strFieldValue);

//获取dsskey所属类型
DSSKEY_MODULE_TYPE dsskey_GetModuleType(int dsskeyID);

//设置远程状态
//状态唯一
bool dsskey_SetRemoteStatus(int dsskeyID, char  remoteStatus);

//获取远程状态
bool dsskey_GetRemoteStatus(int dsskeyID, char & remoteStatus);

//更新指定模块的所有远程状态数据
bool dsskey_UpdateRemoteStatusByModule(DSSKEY_MODULE_TYPE moduleType, const char * lpStatus,
                                       int num, int moduleIndex = 0);

// 根据账号重置对方的状态
void  dsskey_ResetRemoteStatusByAccount(int iLineId);

//根据字符串获取对应的类型
DssKey_Type  dsskey_GetDssKeyType(const yl::string & strType);

//指定dsskey中是否存在对应的状态
bool dsskey_IsStatusExist(int dsskeyID, Dsskey_Status status);

//获取指定模块的路径
//yl::string dsskey_GetModulePath(DSSKEY_MODULE_TYPE moduleType, int moduleIndex = 0);

//dsskey是否为远程状态监控
bool dsskey_IsStatusCtrlByRemoteType(int dsskeyID);

//获取dsskey所在的模块位置
int dsskey_GetModuleIndex(int dsskeyID);


//获取dsskey所在的模块的按键偏移
int dsskey_GetKeyIndex(int dsskeyID);

int dsskey_GetDsskeyByKeyIndex(int index);

int dsskey_GetDsskeyIndexByID(int dsskeyID);

// 获取指定账号的dsskey个数
int dsskey_CountsDsskeyByAccID(int iAccountID, bool bNative = false);

// 获取dsskey对应的blaindex
bool dsskey_GetBLAIndexByDsskey(int & iIndex, int iDsskeyID, int iAccountID);

// 获取blaindex对应的dsskeyid
int dsskey_GetDsskeyByBLAIndex(int iIndex, int iAccountID);

//
bool dsskey_OnConfigChange(msgObject & msg);

bool dsskey_SaveDsskeyInfoToFile(const yl::string & strFilePath, int iFileType = DSS_FILE_TYPE_M7,
                                 DSSKEY_MODULE_TYPE eModuleType = DMT_NON, int iModuleIndex = 0);

bool dsskey_IsExpSupportSwitchKey(DSSKEY_MODULE_TYPE eExpType = DMT_NON);

/*
监听其他类型的Dsskey改变
eRecvType的Action会在eChangeType的Dsskey发生改变时收到通知
*/
bool dsskey_ListenOtherTypeChange(DssKey_Type eRecvType, DssKey_Type eChangeType);

//判断是否支持这种EXP
bool dsskey_IsSupportModuleType();

//重置Dsskey的状态
void dsskey_ResetDsskeyStatus(int iDsskeyID);

Dsskey_Status dsskey_MapStatusToDsskeyStatus(BLF_STATUS_CODE eStatus);

#if IF_BUG_31595
//显示XMLBrowser下载下来的数据
//数据格式:
//<?xml version="1.0" encoding="ISO-8859-1"?>
//  <YealinkIPPhoneStatus>
//  <MessageInDsskey>W:1 T:5</MessageInDsskey>
//  <MessageInDsskey>W:6 T:7</MessageInDsskey>
//</YealinkIPPhoneStatus>
//
//显示规则：
//按dsskey的顺序将上述MessageInDsskey节点的内容显示到类型为xml info的dsskey上
void dsskey_ShowXMLBrowserInfo(yl::string strXMLInfo);

int dsskey_GetDsskeyIDByKeyIndex(int index);
#endif

#if IF_FEATURE_SRTP
void dsskey_UpdateSRTPStatus();
#endif

#if IF_FEATRUE_PTT
//根据DsskeyID判断是否允许长按
bool dsskey_IsSupportLongPress(int iDsskeyID);
//Dsskey按键松开事件
bool dsskey_OnDsskeyRelease(int iDsskeyID);
//添加长按
bool dsskey_AddLongPress(int iDsskeyID);
//删除长按
bool dsskey_RemoveLongPress(int iDsskeyID);
//是否存在长按
bool dsskey_IsExistLongPress();
#endif

bool dsskey_IsValidDsskeyType(int iDsskeyID, DssKey_Type eDsskeyType);

void dsskey_GetDsskeysByType(YLList<int> & listOutput, DssKey_Type eType,
	bool bPositiveSequence = true);

/**
 * @brief 重置所有缓存.
 *
 * @author Song
 * @date 2017/9/5
 *
 * @param iDsskeyID (Optional) Identifier for the dsskey.
 *
 * @return True if it succeeds, false if it fails.
 */

bool dsskey_ReloadDSSKeyData(DssKey_Type eType = DT_NONE);

/**
 * @brief 将当前层级的dsskey 配置值重置为出厂值.
 *
 * @author Song
 * @date 2017/9/5
 *
 * @param eModuleType  Type of the module.
 * @param iModuleIndex (Optional) Zero-based index of the module index.
 */

void dsskey_ResetModuleDataToFactory(DSSKEY_MODULE_TYPE eModuleType, int iModuleIndex = 0);

// 刷新line类型的dsskey
void dsskey_UpdateLineStatus(int iDsskeyID);

//删除dsskey module
void dsskey_RemoveModule(DSSKEY_MODULE_TYPE eModuleType, int iEXPIndex = 0);

//dsskey module 是否存在
bool dsskey_IsDssKeyModuleExist(int iEXPIndex = 0, bool bReset = false);

//exp切页时，刷新dsskeyUI
void dsskey_RefressDsskeyUIDuetoSwitchExpPage(int iEXPIndex = -1, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);

//exp更新时，刷新dsskeyUI
void dsskey_RefressDsskeyUIDuetoExpUpdate(int iEXPIndex = -1, bool bColorExp = false);
#endif

