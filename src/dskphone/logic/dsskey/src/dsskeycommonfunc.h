#ifndef _DSSKEY_COMMON_FUNCT_HEADER_
#define _DSSKEY_COMMON_FUNCT_HEADER_
#include "../include/dsskey_enumtypes.h"
#include "ylstl/ylstring.h"

class CDSSKeyAction;
//创建DSSKEy行为对象
CDSSKeyAction * CreateDsskeyAction(DssKey_Type dsskeyType);

int GetDsskeyDataID(DSSKEY_MODULE_TYPE moduleType, int iExpIndex, int dataIndex);

int GetModuleIDByModuleIndex(DSSKEY_MODULE_TYPE moduleType, int iExpIndex);

DSSKEY_MODULE_TYPE GetModuleType(int dsskeyID);

int GetModuleIndex(int dsskeyID);

int GetModuleID(int dsskeyID);

// 获取不变的ID
int GetUniDsskeyID(int dsskeyID);

//获取DSSKey按键在模块中的偏移
int GetDsskeyIndex(int dsskeyID);

//是否为扩展台
bool IsEXPModule(DSSKEY_MODULE_TYPE eModuleType);

bool IsValidStatus(Dsskey_Status status);

Dsskey_Status MapLineState(int iState, int lineID);

// 模拟发送按键消息
bool DsskeySendKeyMsg(int nKeyCode, bool bKeyDown = true);

int GetAbsoluteIndexByID(int nDsskeyID);
Dsskey_Status getStatusBegin(Dsskey_Status status);

// 判断该机型是否支持该类型
bool IsSupportModuleType(DSSKEY_MODULE_TYPE eType);

// 获取LineKey的数量
int GetLinekeyNumber();

#endif
