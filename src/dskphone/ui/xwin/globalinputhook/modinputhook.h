#ifndef __MODULE_INPUTHOOK_H__
#define __MODULE_INPUTHOOK_H__

#include "inputhook_include.h"

/************************************************************************/
/* 接口       ： hook_EnableHook()                                       */
/* 功能       ： 启用输入事件钩子                                            */
/* 参数说明 ：                                                         */
/*      1.eHookType：启用钩子类型    ，默认值为所有输入事件都进行勾取    */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_EnableHook(HOOK_TYPE eHookType = HT_ALL);

/************************************************************************/
/* 接口       ： hook_DisableHook()                                      */
/* 功能       ： 禁用输入事件钩子                                            */
/* 参数说明 ：                                                         */
/*      1.eHookType：禁用钩子类型    ，默认值为所有输入事件都禁止勾取            */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_DisableHook(HOOK_TYPE eHookType = HT_ALL);

/************************************************************************/
/* 接口       ： inputHook_SendKey()                                     */
/* 功能       ： 发送按键                                                    */
/* 参数说明 ：                                                         */
/*      [in]1.nKeyCode：所发送的按键值                                  */
/*      [in]2.bPress：按键是否按下                                       */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_SendKey(int nKeyCode, bool bPress);

/************************************************************************/
/* 接口       ： inputHook_RegisterKeyPadHookProc()                      */
/* 功能       ： 注册按键处理回调函数                                      */
/* 参数说明 ：                                                         */
/*      [in]1.keypadHook：相关的回调函数指针                          */
/*      [in]2.priority：优先级，应在inputhook_include.h中的enum HOOK_MODULE进行设置，若使用默认值则排在最后且与注册顺序有关    */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_RegisterKeyPadHookProc(KEYPAD_HOOK_PROC keypadHook, HOOK_MODULE priority);

/************************************************************************/
/* 接口       ： inputHook_UnRegisterKeyPadHookProc()                        */
/* 功能       ： 注销按键处理回调函数                                      */
/* 参数说明 ：                                                         */
/*      [in]1.keypadHook：相关的回调函数指针                          */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE priority);

/************************************************************************/
/* 接口       ： inputHook_RegisterMouseHookProc()                       */
/* 功能       ： 注册鼠标处理回调函数                                      */
/* 参数说明 ：                                                         */
/*      [in]1.mouseHook：相关的回调函数指针                               */
/*      [in]2.priority：优先级，应在inputhook_include.h中的enum HOOK_MODULE进行设置，若使用默认值则排在最后且与注册顺序有关    */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_RegisterMouseHookProc(MOUSE_HOOK_PROC mouseHook, HOOK_MODULE priority);

/************************************************************************/
/* 接口       ： inputHook_UnRegisterMouseHookProc()                     */
/* 功能       ： 注销鼠标处理回调函数                                      */
/* 参数说明 ：                                                         */
/*      [in]1.mouseHook：相关的回调函数指针                               */
/* 返回值    ： 无                                                     */
/************************************************************************/
void inputHook_UnRegisterMouseHookProc(HOOK_MODULE priority);

bool inputHook_ProcessKeyInput(int nKey, bool bPressed);
bool inputHook_ProcessMouseInput(int nKey, bool bPressed);

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
int inputHook_GetCurrentRawKey();
void inputHook_SetRawKey(int nKey);
#endif

#endif //__MODULE_INPUTHOOK_H__
