#ifndef __MODULE_APPLICATION_H__
#define __MODULE_APPLICATION_H__

#include <ylstring.h>
#include "entranceguardmessagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"

void application_Init();

#ifdef IF_FEATURE_ENTRANCE_GUARD

// 服务器对来电响铃阶段sip info类型的DTMF的支持程度
enum EDL_COMPATIBLE
{
    EDLC_NONE   = 0,    // 服务器不支持，话机也不兼容
    EDLC_SERVER = 1,    // 服务器支持
    EDLC_PHONE  = 2,    // 服务器不支持，但是话机要兼容
    EDLC_PHONE_SIP = 3, // 服务器不支持，但是话机要兼容，且只支持sip info 方式
};

// 在门禁逻辑层创建一个Task
bool application_CreateDoorLineTask(int iTaskId, const yl::string & strDtmf,
                                    const yl::string & strUrl);

// 对应Session或其sub session是否是门禁来电
bool application_IsDoorLineCall(int iSessionId = INVALID_SESSION_ID);

// 开关是否开启
bool application_IsDoorLineEnable();

// 获取DTMF标志
yl::string application_GetDTMFString(int iSessionId);

// 获取当前激活的任务
int application_GetCurrentTaskId();

// 设置自动应答，自动应答后该路通话自动挂断
void application_SetAutoAnswer(int iSessionId, bool bAuto = false);

// 是否自动应答
bool application_IsAutoAnswer(int iSessionId);

// 获取服务器对sip info的支持情况
EDL_COMPATIBLE application_GetEDLCType();

// 需要自动解hold
bool application_IsNeedAutoUnHold();

#endif

#endif  // __MODULE_APPLICATION_H__
