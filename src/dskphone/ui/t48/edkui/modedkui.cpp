#include "modedkui.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "edkuimanager.h"
#include "uicommon/uimanager/moduimanager.h"
#include "edk/include/modedk.h"
#include "eventawait.h"
#include "edkui/keytimer.h"
#include "configparser/modconfigparser.h"
#include "uicommon/globalinputhook/modinputhook.h"
#include <configiddefine.h>

// #################### 内部使用函数 ####################

// 函数描述 : 打开长按音量+计时器
void Key_StarTimer();

// 函数描述 ：关闭长按音量+计时器
void Key_StopTimer();

// 函数描述 : 判断Edk长按音量键计时器是否开启
bool Key_IsTimerEnable();

void EDKUI_RegisterKeyTimerHook()
{
    inputHook_RegisterKeyPadHookProc(EDKUI_KeyTimerHook, HOOK_MODULE_KEYTIMER);
}

void EDKUI_UnRegisterKeyTimerHook()
{
    inputHook_UnRegisterKeyPadHookProc(EDKUI_KeyTimerHook);
}

static LRESULT OnEdkUIMessage(msgObject & objMessage)
{
    if (objMessage.message != CONFIG_MSG_BCAST_CHANGED)
    {
        return FALSE;
    }

    switch (objMessage.wParam)
    {
    case ST_EDK_SWITCH:
        {
            if (EDKUI_IsEdkSwitchEnable())
            {
                EDKUI_RegisterKeyTimerHook();
            }
            else
            {
                EDKUI_UnRegisterKeyTimerHook();
            }
        }
        break;
    default:
        break;
    }

    return FALSE;
}

// #################### 公共函数 ####################

bool EDKUI_IsEdkSwitchEnable()
{
    return Edk_IsEnable() && configParser_GetConfigInt(kszEdkTipMode) == 1;
}

void EDKUI_Init()
{
    if (EDKUI_IsEdkSwitchEnable())
    {
        EDKUI_RegisterKeyTimerHook();
    }

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnEdkUIMessage);
}


bool EdkUI_DoUIAction(const yl::string & strUIAction, int nIndex/* = 0*/)
{
    EDK_INFO("EdkUI_DoUIAction:%s", strUIAction.c_str());
    return _EDKUIManager.DoUIAction(strUIAction, nIndex);
}

bool EdkUI_DoSoftkeyAction(const yl::string & strHint)
{
    EDK_INFO("EdkUI_DoSoftkeyAction:%s", strHint.c_str());
    return _EDKUIManager.DoSoftkeyAction(strHint);
}

void EdkUI_SetHintMode(bool bShowMode)
{
    if (EDK_IsTipModeEnable() == bShowMode)
    {
        return;
    }

    if (EDK_SetTipMode(bShowMode))
    {
        _EDKUIManager.RefreshUI2Hint(UIManager_GetTopWindow());
    }
}

yl::string EDKUI_GetHintByDirectoryType(int nDirType)
{
    return _EDKUIManager.GetHintByIndex(nDirType);
}

yl::string EDKUI_GetHintNameBySoftkeyAction(yl::string strAction)
{
    return _EDKUIManager.GetHintNameBySoftkeyAction(strAction);
}

yl::string EDKUI_GetHintNameByUIAction(yl::string strAction)
{
    return _EDKUIManager.GetHintNameByUIAction(strAction);
}

yl::string EDKUI_GetCurPageAction()
{
    return _EDKUIManager.GetCurPageHint();
}

void EDKUI_WaitEventProcesses(const KeyPressCallBackData & callBackData)
{
    CEDKEventWait * pEventWait = new CEDKEventWait(callBackData);
    pEventWait->deleteLater();
}

bool EDKUI_SetFuncAllowEnter(yl::string strHint, FunIsAllowEnterUI pFunc)
{
    return _EDKUIOpenLogic->SetFuncAllowEnterUI(strHint, pFunc);
}

bool EDKUI_GetIsAllowEnter(yl::string strHint)
{
    return _EDKUIOpenLogic->IsAllowEnterUI(strHint);
}

void Key_StarTimer()
{
    _KeyTimer.StartKeyTimer();
}

void Key_StopTimer()
{
    _KeyTimer.StopKeyTimer();
}

bool Key_IsTimerEnable()
{
    return _KeyTimer.IsKeyTimerActive();
}

bool EDKUI_KeyTimerHook(int nKeyCode, bool bPress)
{
    if (nKeyCode == PHONE_KEY_VOLUME_INCREASE)
    {
        if (bPress)
        {
            Key_StarTimer();
        }
        else if (Key_IsTimerEnable())
        {
            Key_StopTimer();
        }
        else
        {
            return true;
        }
    }

    return  false;
}
