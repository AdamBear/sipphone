#ifndef __MOD_BROADSOFT_SCA_H__
#define __MOD_BROADSOFT_SCA_H__

#include "scamanager_def.h"
#include "ETLLib.hpp"

// 初始化
void bsSCA_Init();

/*******************************************************************
** 函数描述:   尝试进行指定账号SCA状态的显示
** 参数：      [in] iAccoundID: 账号ID
** 返回:       是否成功显示
********************************************************************/
BOOL bsSCA_ShowSCAStatus(int iAccoundID);

/*******************************************************************
** 函数描述:   获取指定账号的SCA 呼入数据信息
** 参数：      [in] iAccoundID: 账号ID
**             [in] iScaIndex: 该账号分线的Index,从1开始计数
** 返回:       是否成功显示
********************************************************************/
SCA_Info * bsSCA_GetCallInData(int iAccoundID, int iScaIndex);

// 是否需要占线
BOOL bsSCA_BeNeedToSeizeLine(int iAccoundID, int iBLAIndex);


// 获取指账号状态的线路序号
int bsSCA_GetFirstIndexByStatus(BLF_STATUS_CODE eState, int iAccount, int iDsskeyIndex = 0);

// 更新灯状态
VOID bsSCA_UpdateLightInfo(int iAccoundID = -1);

// 获取指定账号首个空闲线路
int bsSCA_GetFirstFreeIndex(int iAccount);

// 获取指定账号响铃线路
int bsSCA_GetAlertingIndex(int iAccount);

// 获取账号线路的呼出信息
BOOL bsSCA_GetLineIndexCallInfo(int iAccoundID, int iScaIndex, yl::string & strUserName,
                                yl::string & strUri);

bool bsSCA_GetDsskeySessionByStatus(int iAccount, BLF_STATUS_CODE eState);

BLF_STATUS_CODE bsSCA_GetStatusByIndex(int iBlaIndex, int iAccountID);

int bsSCA_GetBLAIndex(int iDsskeyID);

// 获取dsskey对应的线路状态
BLF_STATUS_CODE bsSCA_GetStatusByDsskeyID(int iDsskeyID);

void bsSCA_FreeIndex(int iIndex, int iLine);

ListSCAInfo * bsSCA_GetListByAccount(int iAccountID);

int bsSCA_GetCurrntCallNumByAccount(int iAccountID, int nDsskeyIndex = 0);

BLF_STATUS_CODE bsSCA_GetStatus2UIShow(int iAccountId, int iIndex);

yl::string bsSCA_GetBLANum(int iAccountId);

#endif // __MOD_BROADSOFT_SCA_H__
