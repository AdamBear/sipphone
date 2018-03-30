#ifndef _TALKUILOGIC_SOFTKEY_H
#define _TALKUILOGIC_SOFTKEY_H

#include "talk/talklogic/include/uiandlogic_common.h"
#include "talkuilogiccommon.h"
#include "talkuilogicactiondefine.h"

//invite
#define TALK_UI_ACTION_INVITE "invite"
#define TALK_UI_ACTION_MEMBER "member"
#define TALK_UI_ACTION_SECURITY "Security"


yl::string ToMakeLower(yl::string str);

//根据字符串 获取softkey 类型  用于自定义softkey
SoftKey_TYPE GetTypeOfKey(const yl::string & strType);

//获取录制的softkey
SoftKey_TYPE GetRecordSoftkey();

//通过softkeytype 获取 softkey 的title
yl::string GetSoftkeyTitleBySoftkeyType(SoftKey_TYPE softkeyType);


#endif
