#ifndef _MOD_BW_EXECUTIVE_ASSISTANT_H__
#define _MOD_BW_EXECUTIVE_ASSISTANT_H__

#include "config.h"
#include "features-define.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

#include "broadsoft/executiveassistant/include/execommon.h"

/*******************************************************************
** 函数名:       BW_ExeAssis_SetUserType
** 函数描述:    设置Executive Assistant用户类型
**              [in] eType      用户身份
**              [in] iLineID    账号
**              [in] nodeRoot   功能键同步携带的xml根节点
** 返回:      none
*******************************************************************/
void BWExeAssis_SetUserType(EEA_TYPE eType, int iLineID, const CExeAssisInspector * pInfo);

/*******************************************************************
** 函数名:       BWExeAssis_CallPush
** 函数描述:    CallPush实现
**              [in] iCallId    本地通话的CallID
**              [in] iLineID    账号
** 返回:      XSI接口是否成功
*******************************************************************/
bool BWExeAssis_CallPush(int iCallId, int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_CallIntiation
** 函数描述:    CallIntiation实现
**              [in] strTarget  需要拨打的目标号码
**              [in] iLineID    账号
** 返回:      XSI接口是否成功
*******************************************************************/
bool BWExeAssis_CallIntiation(const yl::string & strTarget, int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_GetAccountCountByType
** 函数描述:    获取eType对应的身份的账号个数
**              [in] eType   身份标识
** 返回:      账号个数
*******************************************************************/
int  BWExeAssis_GetAccountCountByType(EEA_TYPE eType);

/*******************************************************************
** 函数名:       BWExeAssis_GetFirstUsableAccountByType
** 函数描述:    获取eType对应的身份的第一个账号
**              [in] eType   身份标识
** 返回:      账号ID
*******************************************************************/
int BWExeAssis_GetFirstUsableAccountByType(EEA_TYPE eType = EET_ALL);

/*******************************************************************
** 函数名:       BWExeAssis_QueryAction
** 函数描述:    通过XSI向服务器查询数据
**              [in] eAction   对应的操作类型
**              [in] iLineID   账号
** 返回:      XSI接口是否成功
*******************************************************************/
bool BWExeAssis_QueryAction(ExeAssis_Action eAction, int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_GetData
** 函数描述:    获取逻辑层缓存在本地的数据
**              [in] pInfo      指针，获取到的数据的存放地址
**              [in] iLineID    账号
** 返回:      是否获取成功
*******************************************************************/
bool BWExeAssis_GetData(CExeAssisInspector * pInfo, int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_SetData
** 函数描述:    获取逻辑层缓存在本地的数据
**              [in] pInfo      指针，需要设置的数据的存放地址
**              [in] iLineID    账号
** 返回:      是否设置成功
*******************************************************************/
bool BWExeAssis_SetData(const CExeAssisInspector * pInfo, int iLineID,
                        ExeAssis_Action eAction = EAA_AutoSelect);

/*******************************************************************
** 函数名:       BWExeAssis_SetLineId
** 函数描述:    设置UI当前操作的账号ID
**              [in] iLineID    账号
** 返回:      void
*******************************************************************/
void BWExeAssis_SetLineId(int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_GetLineId
** 函数描述:    获取UI当前操作的账号ID
**
** 返回:      [in]    账号
*******************************************************************/
int BWExeAssis_GetLineId();

/*******************************************************************
** 函数名:       BWExeAssis_SetCallInitiationNumber
** 函数描述:    设置CallInitiation 对应Executive的号码
**              [in] strNumber  对应Executive的号码
**              [in] iLineID    账号
** 返回:      是否设置成功
*******************************************************************/
bool BWExeAssis_SetCallInitiationNumber(const yl::string & strNumber, int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_GetExecutiveListCount
** 函数描述:    获取对应账号的Executive个数
**              [in] iLineID    账号
** 返回:      Executive个数
*******************************************************************/
int BWExeAssis_GetExecutiveListCount(int iLineID);



/*******************************************************************
** 函数名:       BWExeAssis_IsExecutive
** 函数描述:    对应账号是否是Executive身份
**              [in] iLineID    账号
** 返回:      是\否
*******************************************************************/
bool BWExeAssis_IsExecutive(int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_IsAssistant
** 函数描述:    对应账号是否是Assistant身份
**              [in] iLineID    账号
** 返回:      是\否
*******************************************************************/
bool BWExeAssis_IsAssistant(int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_IsNeedShowAssistantDetailsSetting
** 函数描述:    对应账号是否是Assistant身份
**              [in] iLineID    账号
** 返回:      是\否
*******************************************************************/
bool BWExeAssis_IsNeedShowAssistantDetailsSetting(int iLineID = -1);

/*******************************************************************
** 函数名:       BWExeAssis_IsCallInitiationEnable
** 函数描述:    对应账号是否开启CallInitiation功能
**              [in] iLineID    账号
** 返回:      是\否
*******************************************************************/
bool BWExeAssis_IsCallInitiationEnable(int iLineID);


/*******************************************************************
** 函数名:       BWExeAssis_IsExecutiveNumber
** 函数描述:    是否是iLineID对应账号的Executive的号码
**              [string] strNumber  号码
**              [in] iLineID        账号
** 返回:      是\否
*******************************************************************/
bool BWExeAssis_IsExecutiveNumber(const yl::string & strNumber, int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_IsSilentMonitorEnable
** 函数描述:    对应账号是否开启SilentMonitor功能
**              [in] iLineID    账号
** 返回:      是\否
*******************************************************************/
bool BWExeAssis_IsSilentMonitorEnable(int iLineID);

/*******************************************************************
** 函数名:       BWExeAssis_CancelAllRequest
** 函数描述:    清空所有正在运行中的XSI任务
**              void
** 返回:      void
*******************************************************************/
void BWExeAssis_CancelAllRequest();

#endif
#endif //_MOD_BW_EXECUTIVE_ASSISTANT_H_
