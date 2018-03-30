#ifndef __MOD_IDLEUL_H__
#define __MOD_IDLEUL_H__

#include "baseidlehandle.h"
#include "common/common_data_define.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "ylstl/ylstring.h"
#include "ylstl/ylvector.h"

void IdleUL_Init();

void IdleUL_RegisterHandle(int iType, CBaseIdleHandle * pHandle);

// 在idle下弹出指定信息的框，主要用于Idle逻辑层的的弹出框控制
bool IdleUL_PopupBox(const PopupBoxInfo & popData);

// 退出idle被监听的触发器（如退出指定的TIPS或MESSAGEBOX），主要用于Idle逻辑层的弹出框控制
bool IdleUL_ExitPopupBox(int nID = MSGBOX_ID_NOTE);

bool IdleUL_UpdateAccountInfo(const yl::string & strName, ICON_TYPE eIcon);

bool IdleUL_UpdateSoftkey();

void IdleUL_UpdateStatus();

void IdleUL_ResetKeypadLock();

#ifdef IF_FEATURE_PSTN
bool IdleUL_GetPstnIdList(VEC_ID& vecID);

void IdleUL_ClearPstnIdList();
#endif

void IdleUL_UpdateXMLBrowser();

void IdleUL_EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType);

int IdleUL_GetDsskeyIDByKeyCode(int nKey);

#if IF_FEATURE_METASWITCH_VOICEMAIL
bool IdleUL_EnterMTSWVoiceMail();
#endif

#if IF_SUPPORT_CENTER_ACD_STATUS
void IdleUL_UpdateCenterNote(const yl::string& strNote);
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
void IdleUL_UpdateExtraNote(const yl::string& strNote);
#endif

void IdleUL_UpdateACDQueueStatus();

#endif // __MOD_IDLEUL_H__
