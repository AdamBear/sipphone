#ifndef __MOD_FEATURE_H__
#define __MOD_FEATURE_H__

#include "featurecommon.h"
#include "common/common_data_define.h"
#include "contacts/directory/include/directoryenumtypes.h"

/*******************************************************************
** 函数描述:   初始化
** 参数：      无
** 返回:       无
********************************************************************/
void feature_Init();

// 发送开启特征码
BOOL feature_SendOnCode(int iLineId, CCodeInfo & refCode);

// 发送关闭特征码
BOOL feature_SendOffCode(int iLineId, CCodeInfo & refCode);

// 跳过上电后的首次Code发送. 首次账号注册上时要避免发送
bool feature_IsSkipCodeSend(int iType, int iLineId);


/**
 * 判断DND同步功能是否开启
 *
 * @return  DND同步功能是否开启
 */
bool feature_IsDNDSync(int iAccountID = -1);

/**
 * 判断FWD同步功能是否开启
 *
 * @return  DND同步功能是否开启
 */
bool feature_IsForwardSync(int iAccountID = -1);

/**
 * 设置BoradSoft Feature Key同步功能是否开启
 *
 * @param   bEnable FeatureKey同步功能是否开启
 */
void feature_SetFeatureKeySync(bool bEnable, int iAccountID = -1);

/**
 * 设置DND同步功能是否开启
 *
 * @param   bEnable DND同步功能是否开启
 */
void feature_SetDNDSync(bool bEnable, int iAccountID = -1);

/**
 * 设置FWD同步功能是否开启
 *
 * @param   bEnable FWD同步功能是否开启
 */
void feature_SetForwardSync(bool bEnable, int iAccountID = -1);

/**
 * 判断启用DND同步功能时，本地是否需要处理DND（拒接）
 *
 * @param   iAccount    账号ID
 *
 * @return  本地是否需要处理DND
 */
bool feature_IsDNDLocalProcess(int iAccount);

/**
 * 判断启用FWD同步功能时，本地是否需要处理FWD（转移）
 *
 * @param   iAccount    账号ID
 *
 * @return  本地是否需要处理FWD
 */
bool feature_IsForwardLocalProcess(int iAccount);



#if IF_FEATURE_PAGING
//////////////////////////////////////////////////////////////////////////////////////////////
// 获取Paging List
bool feature_MutliPaingGetList(PagingListVec & pagingList);

// 根据配置项项数获取相应数据
const PagingGroupInfo * feature_MutliPaingGetGroupInfo(int iConfigId);

// 修改某个Paging数据
bool feature_MutliPaingSaveGroupInfo(const PagingGroupInfo & groupInfo);

// 删除某个Paging数据
bool feature_MutliPaingDeleteGroupInfo(int iConfigId);

// 删除所有配置
bool feature_MutliPaingDeleteAll();

// 注册/注销回调函数，主要用于通知UI刷新
void feature_MutliPaingCallBack(PagingListCallBack pCallBack, bool bRegister = true);
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// 是否启用三级权限
bool feature_UserModeIsOpen();

// 获取当前用户使用的权限
USER_TYPE feature_UserModeGetCurrType(int iUIID = DEF_UIIID);

USER_TYPE feature_UserModeGetTypeByName(const yl::string & strName);

// 改变话机的使用权限
void feature_UserModeSetCurrType(USER_TYPE eType, int iUIID = DEF_UIIID);

// 根据权限获取用户名：用于密码匹配 / 显示
// 返回空 表示传入的权限类型不合法 或 该权限不开放
yl::string feature_UserModeGetUserName(USER_TYPE eType);

// 获取默认的显示名
yl::string feature_UserModeGetDefaultUserName(USER_TYPE eType);

// 判断配置项是否有显示权限
// | [in] strItemAction| 黑白屏：strID  彩屏：ClickAction
bool feature_UserModeConfigItemShow(const yl::string & strItemAction, int iUIID = DEF_UIIID);

// 判断配置界面是否只读
// | [in] strItemAction| 配置界面的Action
bool feature_UserModeConfigItemReadOnly(const yl::string & strItemAction, int iUIID = DEF_UIIID);

// #ifdef 1//_DECT
/*******************************************************************
** 函数名:     feature_UserModeGetData
** 函数描述:   三级权限数据
**             [out] refData:               三级权限列表
**             [in] iUIID:              手柄ID
** 返回:
*******************************************************************/
bool feature_UserModeGetData(UserModeListData & refData, int iUIID = DEF_UIIID);

/*******************************************************************
** 函数名:     feature_UserModeGetCnt
** 函数描述:   三级权限数量
**             [in] iUIID:              手柄ID
** 返回:
*******************************************************************/
int  feature_UserModeGetCnt(int iUIID = DEF_UIIID);
// #endif //_DECT

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取显示的RTP配置项列表
const RTPConfigureList * feature_RTPConfigGetEnableList();

#endif // __MOD_FEATURE_H__
