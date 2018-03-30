#ifndef _MOD_LINE_DEVICE_H__
#define _MOD_LINE_DEVICE_H__

void lineDevice_Init();

// 外接设备插入拔出事件
bool lineDevice_Event(int iKeyEvent);

// 外接设备是否检测
// bCheckMobile是否也检测手机端
bool lineDevice_IsConnect(bool bCheckMobile = true);

#endif // _MOD_LINE_DEVICE_H__
