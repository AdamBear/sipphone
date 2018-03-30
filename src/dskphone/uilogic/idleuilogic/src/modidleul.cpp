#include "modidleul.h"
#include "uilogichelper.h"
#include "uilogicmanager.h"
#include "uimanager/moduimanager.h"
#include "pstnidlehandle.h"
#include "adapterbox/include/modadapterbox.h"
#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#endif

void IdleUL_Init()
{

}

void IdleUL_RegisterHandle(int iType, CBaseIdleHandle * pHandle)
{
    _UiIdleLogicHelper.RegisterHandle(iType, pHandle);
}

// 在idle下弹出指定信息的框，主要用于Idle逻辑层的的弹出框控制
bool IdleUL_PopupBox(const PopupBoxInfo & popData)
{
    return _UiIdleLogicManager.CreateMessageBox(popData);
}

// 退出idle被监听的触发器（如退出指定的TIPS或MESSAGEBOX），主要用于Idle逻辑层的弹出框控制
bool IdleUL_ExitPopupBox(int nID/* = MSGBOX_ID_NOTE*/)
{
    return _UiIdleLogicManager.RemoveMessagebox(nID);
}

bool IdleUL_UpdateAccountInfo(const yl::string & strName, ICON_TYPE eIcon)
{
    return _UiIdleLogicManager.UpdateAccountInfo(strName, eIcon);
}

bool IdleUL_UpdateSoftkey()
{
    LIST_IDLE_STATUS_ITEM statusList;
    return _UiIdleLogicManager.IdleLogicSoftkeyCallBack(statusList);
}

void IdleUL_UpdateStatus()
{
    if (AdapterBox_IsInIdle())
    {
        _UiIdleLogicManager.UpdateStatus();
    }
}

void IdleUL_ResetKeypadLock()
{
    if (AdapterBox_IsInIdle())
    {
        _UiIdleLogicManager.ResetKeypadLock();
    }
}

#ifdef IF_FEATURE_PSTN
bool IdleUL_GetPstnIdList(VEC_ID & vecID)
{
    CBaseIdleHandle * pBaseIdleHandle = NULL;
    pBaseIdleHandle = GET_IDLE_HANDLE(PS_STATE_PSTN_ACCOUNT_NOTE);
    CPstnHandle * pPstnHandle = static_cast<CPstnHandle *>(pBaseIdleHandle);
    if (NULL == pPstnHandle)
    {
        return false;
    }

    vecID = pPstnHandle->GetPstnIdList();
    return true;
}

void IdleUL_ClearPstnIdList()
{
    CBaseIdleHandle* pBaseIdleHandle = NULL;
    pBaseIdleHandle = GET_IDLE_HANDLE(PS_STATE_PSTN_ACCOUNT_NOTE);
    CPstnHandle* pPstnHandle = static_cast<CPstnHandle*>(pBaseIdleHandle);
    if (NULL == pPstnHandle)
    {
        return;
    }
    pPstnHandle->ClearPstnIdList();
}

#endif

void IdleUL_UpdateXMLBrowser()
{
    if (AdapterBox_IsInIdle())
    {
        _UiIdleLogicManager.IdleLogicUpdateCustomNotify();
    }
}

void IdleUL_EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType)
{
    if (AdapterBox_IsInIdle())
    {
        _UiIdleLogicManager.EnterPageByType(eEnterPageType);
    }
}

int IdleUL_GetDsskeyIDByKeyCode(int nKey)
{
    return _UiIdleLogicManager.GetDsskeyIDByKeyCode(nKey);
}

#if IF_FEATURE_METASWITCH_VOICEMAIL
bool IdleUL_EnterMTSWVoiceMail()
{
    if (!MTSWVM_IsMessagesListEnable())
    {
        return false;
    }

    if (!MTSW_IsLogined())
    {
        IdleUL_EnterPageByType(IEPT_MTSW_VOICE_MAIL_LOGIN);
    }
    else
    {
        MTSWVM_SetDownLoading(true);
        MTSWVM_SetFromIndex(0);
        IdleUL_EnterPageByType(IEPT_MTSW_VOICE_MAIL_LIST);
    }

    return true;
}
#endif

#if IF_SUPPORT_CENTER_ACD_STATUS
void IdleUL_UpdateCenterNote(const yl::string& strNote)
{
    if (!AdapterBox_IsInIdle())
    {
        return;
    }

    return _UiIdleLogicManager.UpdateCenterNote(strNote);
}
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
void IdleUL_UpdateExtraNote(const yl::string& strNote)
{
    if (!AdapterBox_IsInIdle())
    {
        return;
    }

    return _UiIdleLogicManager.UpdateExtraNote(strNote);
}
#endif

void IdleUL_UpdateACDQueueStatus()
{
    if (AdapterBox_IsInIdle())
    {
        _UiIdleLogicManager.IdleLogicUpdateCustomNotify();
    }
}
