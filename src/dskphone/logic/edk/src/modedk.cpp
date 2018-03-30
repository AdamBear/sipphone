#include "modedk.h"
#include "edkmanager.h"
#include "edkdatamanager.h"
#include "edkactionmanager.h"
#include "keyguard/include/modkeyguard.h"

#if IF_FEATURE_EDK
void Edk_Init()
{
    _EDKManager.Init();
}

bool Edk_OnEdkSoftKeyPress(EDKSOFTKEYTYPE eType)
{
    KEY_GUARD_TYPE eLockType = keyGuard_GetKeyGuardType();
    if (keyGuard_IsLock()
            && (eLockType == KG_ALL_KEYS
                || eLockType == KG_FUN_KEY))
    {
        return false;
    }
    return _EDKManager.OnEdkSoftKeyProess(eType);
}

yl::string Edk_GetEdkSoftKeyLabel(EDKSOFTKEYTYPE eType)
{
    yl::string strLabel;

    _EDKManager.GetSoftkeyLabelByType(eType, strLabel);

    return strLabel;
}

yl::string Edk_GetEdkSoftKeyID(EDKSOFTKEYTYPE eType)
{
    yl::string strID;

    _EDKDataMgr.GetSoftkeyID(eType, strID);

    return strID;
}

EDKSOFTKEYTYPE Edk_GetEdkSoftKeyType(const yl::string & strSoftkeyID)
{
    return _EDKDataMgr.GetSoftkeyTypeByID(strSoftkeyID);
}

void Edk_GetEdkSoftKeyList(YLList<SoftKey_Data> & listSoftKey)
{
    return _EDKManager.GetEdkSoftKeyList(listSoftKey);
}
bool Edk_IsEnable()
{
    return _EDKDataMgr.IsEnable();
}

yl::string Edk_GetDsskeyData(int iDsskeyID, EDKACTIONTYPE eType)
{
    return _EDKManager.GetDsskeyLabelFromAction(iDsskeyID, eType);
}

bool EDK_IsTipModeEnable()
{
    return _EDKManager.IsTipModEnable();
}

bool EDK_SetTipMode(bool bEnable)
{
    return _EDKManager.SetTipMode(bEnable);
}

// 获取prompt数据
bool Edk_GetPromptByIndex(int iIndex, edk_prompt_data_t & promptData)
{
    return _EDKDataMgr.GetPromptByIndex(iIndex, promptData);
}

bool EDK_DoAction(const yl::string & strAction, int iDsskeyId/* = -1*/,
                  EDKTRIGGERTYPE eTriggerType/* = EDK_TRIGGER*/)
{
    return _EDKManager.DoAction(strAction, iDsskeyId, eTriggerType);
}

bool EDK_GetEdkCtrlDsskeyDispData(int iDsskeyId, EdkCtrlDsskeyDispData & objDisplayData)
{
    return _EdkActionManager.GetEdkCtrlDsskeyDispData(iDsskeyId, objDisplayData);
}

bool EDK_DoAction(int iActionId)
{
    return _EdkActionManager.DoAction(iActionId);
}

#else
void Edk_Init()
{
}

bool Edk_OnEdkSoftKeyPress(EDKSOFTKEYTYPE eType)
{
    return false;
}

yl::string  Edk_GetEdkSoftKeyLabel(EDKSOFTKEYTYPE eType)
{
    return "";
}

yl::string Edk_GetEdkSoftKeyID(EDKSOFTKEYTYPE eType)
{
    return "";
}

EDKSOFTKEYTYPE Edk_GetEdkSoftKeyType(const yl::string & strSoftkeyID)
{
    return EDK_SOFTKEY_NON;
}

void Edk_AddEdkSoftKey()
{
}

bool Edk_IsEnable()
{
    return false;
}

bool Edk_GetPromptByIndex(int iIndex, edk_prompt_data_t & promptData)
{
    return false;
}

void Edk_GetEdkSoftKeyList(YLList<SoftKey_Data> & listSoftKey)
{
}

yl::string Edk_GetDsskeyData(int iDsskeyID, EDKACTIONTYPE eType)
{
    return "";
}

bool EDK_IsTipModeEnable()
{
    return false;
}

bool EDK_SetTipMode(bool bEnable)
{
    return false;
}

bool EDK_DoAction(const yl::string & strAction, int iDsskeyId/* = -1*/,
                  EDKTRIGGERTYPE eTriggerType/* = EDK_TRIGGER*/)
{
    return false;
}

bool EDK_DoAction(int iActionId)
{
    return false;
}

bool EDK_GetEdkCtrlDsskeyDispData(int iDsskeyId, EdkCtrlDsskeyDispData & objDisplayData)
{
    return false;
}

#endif
