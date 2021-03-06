#ifndef MOD_EDK_UI_H
#define  MOD_EDK_UI_H
#include "common/common_data_define.h"
#include "openuilogicmanager.h"

/************************************************************************/
// 函数描述  ：Edk模块初始化函数
// 参数说明 ：
// 返回值 ： 无
/************************************************************************/
void EDKUI_Init();

/************************************************************************/
// 函数描述  ： EdkUI模块打开相应Menu
// 参数说明 ： [in] strUIAction:执行的Menu命令ID   [in] nIndex:Menu索引，暂时未使用，保留该参数
// 返回值 ： bool, 是否执行成功
/************************************************************************/
bool EdkUI_DoUIAction(const yl::string & strUIAction, int nIndex = 0);

/************************************************************************/
// 函数描述  ： EdkUI模块执行相应softkey操作
// 参数说明 ：[in] strHint:softkey命令ID
// 返回值 ： bool, 是否成功
/************************************************************************/
bool EdkUI_DoSoftkeyAction(const yl::string & strHint);

/************************************************************************/
// 函数描述  ： EdkUI模块设置提示模式
// 参数说明 ： 设置是否提示模式，并刷新UI
// 返回值 ： 无
/************************************************************************/
void EdkUI_SetHintMode(bool bShowMode);

/************************************************************************/
// 函数描述  ： EdkUI模块Directory模块通过Directory类型获取提示模式值
// 参数说明 ： [in] nDirType:Directory类型
// 返回值 ： [yl::string] 返回提示模式值
/************************************************************************/
yl::string EDKUI_GetHintByDirectoryType(int nDirType);

/************************************************************************/
// 函数描述  ： EdkUI模块通过softkeyAction获取提示命令
// 参数说明 ： [in] strAction:softkeyAction值
// 返回值 ： [yl::string] 返回提示模式值
/************************************************************************/
yl::string EDKUI_GetHintNameBySoftkeyAction(yl::string strAction);

/************************************************************************/
// 函数描述  ： EdkUI模块通过UIAction获取提示命令
// 参数说明 ： [in] strAction:UIAction值
// 返回值 ： [yl::string] 返回提示模式值
/************************************************************************/
yl::string EDKUI_GetHintNameByUIAction(yl::string strAction);

/************************************************************************/
// 函数描述  ： EdkUI模块获取顶层窗口的m_strHint值
// 参数说明 ：
// 返回值 ： [yl::string] 返回顶层窗口的m_strHint值
/************************************************************************/
yl::string EDKUI_GetCurPageAction();

/************************************************************************/
// 函数描述  ： 等待某个事件处理完成
// 参数说明 ：
// 返回值 ： 无
/************************************************************************/
void EDKUI_WaitEventProcesses(const KeyPressCallBackData & callBackData);

/************************************************************************/
// 函数描述  ： 为打开UI的命令设置权限判断函数
// 参数说明 ：  strHint：提示命令， pFunc：权限判断函数指针
// 返回值 ： 是否设置成功 如果映射中不存在这个命令则设置不成功
/************************************************************************/
bool EDKUI_SetFuncAllowEnter(yl::string strHint, FunIsAllowEnterUI pFunc);

/************************************************************************/
// 函数描述  ： 获取打开UI的命令权限
// 参数说明 ：  strHint：提示命令
// 返回值 ： 是否允许进入
/************************************************************************/
bool EDKUI_GetIsAllowEnter(yl::string strHint);

/************************************************************************/
// 函数描述  ： 硬按键定时器钩子函数
// 参数说明 ：  nKeyCode 硬按键keycode, bPress true则为press
// 返回值 ： 无
/************************************************************************/
bool EDKUI_KeyTimerHook(int nKeyCode, bool bPress);

bool EDKUI_IsEdkSwitchEnable();

#endif
