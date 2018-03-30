#ifndef _DSSKEY_MSG_PROCESSOR_HEADER_
#define _DSSKEY_MSG_PROCESSOR_HEADER_
#include <ETLLib.hpp>

//消息注册
void RegisterMsgHandle();

//解注册操作
void UnRegisterMsgHandle();

//消息处理
LRESULT OnMsgProcess(msgObject & msg);

#endif
