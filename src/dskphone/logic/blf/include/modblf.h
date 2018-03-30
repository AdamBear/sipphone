#ifndef _BLF_MOD_BLF_H__
#define _BLF_MOD_BLF_H__

#include "blfcommon.h"

/*******************************************************************
** 函数描述:    初始化BLF模块
** 参数           NA
** 返回值:       NA
********************************************************************/
void blf_Init();

/*******************************************************************
** 函数描述:    获取Pickup显示的列表
** 参数           [out] listMenu: 显示列表
** 返回值:       是否获取成功
********************************************************************/
bool blf_GetPickupMenuList(YLList<yl::string> & listMenu);

#if IF_FEATURE_SHOW_BLF_CALLID
/*******************************************************************
** 函数描述:    获取BLF监控显示的列表
** 参数           [out] listMenu: 显示列表 listStatus: 状态列表
** 返回值:       是否获取成功
********************************************************************/
bool blf_GetBLFMenuList(int nDsskeyId, YLList<yl::string>& listMenu, YLList<int>& listStatus);

/*******************************************************************
** 函数描述:    显示BLF列表
** 参数           [out] nDsskeyId
** 返回值:       是否显示成功
********************************************************************/
bool blf_ShowBLFMenu(int nDsskeyId);
#endif

/*******************************************************************
** 函数描述:    通过在Pickup列表的位置找到对应的Dsskeyid
** 参数           [in] iIndex: 位置
** 返回值:       成功返回DsskeyiD, 失败返回-1
********************************************************************/
int blf_GetPickupDsskeyIdByIndex(int iIndex);

/*******************************************************************
** 函数描述:    通过在Pickup列表的位置找到详情信息的位置
** 参数           [in] iIndex: 位置
** 返回值:       成功返回详情信息的位置, 失败返回-1
********************************************************************/
int blf_GetDialogIndexIdByIndex(int iIndex);

/*******************************************************************
** 函数描述:    取消Pickup弹窗的显示
** 参数           NA
** 返回值:       NA
********************************************************************/
void blf_CancelPickupMenuShow();

/*******************************************************************
** 函数描述:    获取监控方的详情信息（供Pickup使用）
** 参数           [out] refData: 监控方详情信息
                [in] nDsskeyId: Dsskey唯一标识
                [in] eType: 所属的监控类型
                [in] index: 处于Pickup列表的位置..（默认0取最高优先级）
** 返回值:       获取成功返回true，否则false
********************************************************************/
bool blf_GetDialogInfoByDsskeyId(DialgInfo & refData, int nDsskeyId, eBLFMangaerType eType,
                                 int index = 0);

/*******************************************************************
** 函数描述:    是否支持自动Pickup方式
** 参数           [in] nDsskeyId: Dsskey唯一标识
                [in] index: 处于Pickup列表的位置..（默认0取最高优先级）
** 返回值:       获取成功返回true，否则false
********************************************************************/
bool blf_IsSupportAutopPickUpByDsskey(int nDsskeyId, int index = 0);

/*******************************************************************
** 函数描述:    获取BLFlist数据
** 参数           [out] refVector: 数据列表
** 返回值:       获取成功返回true，否则false
********************************************************************/
bool blf_GetBlfListData(YLVector<AccountMoitorData> & refVector);

/*******************************************************************
** 函数描述:    AutoSet自动配置完Dsskey, BLF模块需要重新绑定
** 参数           [in] iDsskeyId 数据列表
                [in] iAccountId 账号id
                [in] strKey 监控方的唯一标识
** 返回值:       获取成功返回true，否则false
********************************************************************/
void blf_SetDsskeyId(int iDsskeyId, int iAccountId, const yl::string & strKey);

/*******************************************************************
** 函数描述:    请求自定义的操作类型
** 参数           [in] eDssStatus 监控方状态
                [in] eSessState 目前通话的状态
** 返回值:       获取成功返回true，否则false
********************************************************************/
eBLFOperateType blf_GetCustomOperationType(Dsskey_Status eDssStatus, SESSION_STATE eSessState);

/*******************************************************************
** 函数描述:    请求自定义配置数据
** 参数           [in] eDssStatus 监控方状态
                [in] eSessState 目前通话的状态
** 返回值:       成功返回自定义的字符串，否则返回空
********************************************************************/
yl::string blf_GetCustomOperationValue(Dsskey_Status eDssStatus, SESSION_STATE eSessState);

/*******************************************************************
** 函数描述:    刷新BLF&BLFlist的状态
** 参数           [in] lstDsskey 刷新的BLF&BLFList Dsskey集合
** 返回值:       无
********************************************************************/
void blf_RefreshDsskey(YLList<int> & lstFlushDsskey);

bool blf_RefreshDsskey(const int iDsskeyId, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);


//////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************
** 函数描述:    获取Pickup显示的内容（供内部使用）
** 参数           [out] listMenuData: 显示列表
                [out] listDsskeyID: Dsskey列表
                [in] strKey: 监控方的唯一标识
                [in] eType: 所属的监控类型
** 返回值:       NA
********************************************************************/
void blf_GetPickUpShowContentByKey(YLList<yl::string> & listMenuData, YLVector<int> & listDsskeyID,
                                   const yl::string & strKey, eBLFMangaerType eType);


void blf_UnBindDsskeyId(int iDsskeyId, int iAccountId);

void blf_PlayVoice();

void blf_StopVoice();

#if IF_BUG_32812
// 通过账号ID获取相应的监控列表
// iLineID 账号ID
// list Monitor 监控列表输出
// 存在监控列表返回true，不存在则返回false
bool blf_GetBLFListMonitorList(const int iLineID, YLList<yl::string> & listMonitor);

bool blf_IsAccountHaveBLFListData(const int iLineID);

bool blf_GetBLFListAutoSetDataBykey(const int iDsskeyId, autoset_dsskeydata_t & tAutoSetDssKeyData);
#endif

void blf_ProcessTalkFoucsChange();

bool blf_IsBlfIdleRing();

bool blf_StopBlfIdleRing();

#endif

