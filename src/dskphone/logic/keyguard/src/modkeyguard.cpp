#include "modkeyguard.h"
#include "keyguardmanager.h"

#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "commonapi/systemhelperapi.h"
#include "commonunits/commonunits_def.h"
#include <winimp/winimp.h>
//#include "commonunits/modcommonunits.h"
#include "keyguarddsskeyaction.h"


// 初始化
void keyGuard_Init()
{
    g_pKeyGuardManager;

    new CDssKeyKeyGuard();

}

// 键盘锁是否开启
bool keyGuard_IsEnable()
{
    return g_pKeyGuardManager->IsEnable();
}

// 键盘是否处于锁定状态
bool keyGuard_IsLock()
{
    return g_pKeyGuardManager->IsKeyPadLock();
}

// 执行上锁/解锁操作
void keyGuard_Lock(bool bLock)
{
    //上锁
    g_pKeyGuardManager->LockKeyPad(bLock);
}

// 能否进入特定的模块
bool keyGuard_IsEnableModuld(KEY_GUARD_MODULE eModule)
{
    return g_pKeyGuardManager->IsEnableModuld(eModule);
}

// 键盘锁处理相关的消息
bool keyGuard_ProcessMsg(msgObject & msg)
{
    return g_pKeyGuardManager->ProcessMsg(msg);
}

// 设置验证
void keyGuard_SetCheckUnlockPswFun(CheckUnlockPswFun pFun)
{
    g_pKeyGuardManager->SetCheckUnlockPswFun(pFun);
}

// 开启/关闭键盘锁
void keyGuard_Enable(bool bEnable)
{
    g_pKeyGuardManager->EnableKeyGuard(bEnable);
}

// 设置键盘锁类型
void keyGuard_SetKeyGuardType(KEY_GUARD_TYPE eKGType)
{
    g_pKeyGuardManager->SetKeyGuardType(eKGType);
}

// 设置非紧急号码的提示框函数
void keyGuard_SetEmergencyTipFun(PopupEmergencyNumNote pFun)
{
    g_pKeyGuardManager->SetEmergencyTip(pFun);
}

// 弹出紧急号码的提示框
void keyGuard_PopEmergencyTip()
{
    g_pKeyGuardManager->PopEmergencyTip();
}

// 设置键盘锁类型
KEY_GUARD_TYPE keyGuard_GetKeyGuardType()
{
    return g_pKeyGuardManager->GetKeyGuardType();
}

yl::string keyGuard_GetUnlockPIN()
{
    return configParser_GetConfigString(kszUnlockPIN);
}

extern yl::string MyEncryptedText(const char * lpszPlainText, const char * lpszSaltText);

bool keyGuard_CheckUnlockPin(const yl::string & strUnLockPin)
{
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {
        return (strUnLockPin == configParser_GetConfigString(kszUnlockPIN));
    }
    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {
        yl::string strOldPIN = configParser_GetConfigString(kszUnlockPIN);
        yl::string strInputPin = MyEncryptedText(strUnLockPin.c_str(), strOldPIN.c_str());

        return strInputPin == strOldPIN;
    }
    return false;
}

// 保存密码
bool keyGuard_SaveUnlockPin(const yl::string & strUnLockPin)
{
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {
        return configParser_SetConfigString(kszUnlockPIN, strUnLockPin.c_str(), CONFIG_LEVEL_PHONE);
    }
    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {
        yl::string strOldPIN = configParser_GetConfigString(kszUnlockPIN);
        yl::string strNewPin = MyEncryptedText(strUnLockPin.c_str(), strOldPIN.c_str());
        return configParser_SetConfigString(kszUnlockPIN, strNewPin.c_str(), CONFIG_LEVEL_PHONE);
    }
    return false;
}

PinType keyGuard_GetPinType(const yl::string & strPin)
{
    if (keyGuard_CheckUnlockPin(strPin))
    {
        return PIN_CORRECT_LOCK;
    }

    yl::string strUserName = configParser_GetConfigString(kszUserNameAdmin);

    // 检查是否是admin密码
    if (commonAPI_CheckPassword(strUserName.c_str(), strPin.c_str()))
    {
        return PIN_CORRECT_ADMIN;
    }

    return PIN_INCORRECT;
}

// 重置键盘锁状态
void keyGuard_ResetKeypadLock()
{
    g_pKeyGuardManager->ResetKeypadLockTimer();
}

static LPCSTR szSaltCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./";
yl::string MyEncryptedText(const char * lpszPlainText, const char * lpszSaltText)
{
    if (lpszPlainText == NULL)
    {
        lpszPlainText = "";
    }
    int nPasswordLen = chStrlen(lpszPlainText);
    int nCryptLoops = nPasswordLen == 0 ? 1 : (nPasswordLen - 1) / 8 + 1;

    yl::string strSalt = lpszSaltText;
    int nSaltLen = strSalt.length();
    int nShortenBytes = nCryptLoops * 13 - nSaltLen;
    while (nShortenBytes > 0)
    {
        if (nSaltLen != 0 && nSaltLen % 13 == 0)
        {
            strSalt += strSalt[0];
            strSalt += strSalt[1];
            nSaltLen += 2;
            nShortenBytes -= 2;
            continue;
        }
        strSalt += szSaltCharacters[rand() % 64];
        -- nShortenBytes;
        nSaltLen ++;
    }

    lpszSaltText = strSalt.c_str();

    yl::string strResult;
    while (nCryptLoops > 0)
    {
        strResult += crypt(lpszPlainText, lpszSaltText);
        lpszPlainText += 8;
        lpszSaltText += 13;
        -- nCryptLoops;
    }
    return yl::string(strResult.c_str());
}

void keyGuard_StopKeypadLockTimer()
{
    g_pKeyGuardManager->StopKeypadLockTimer();
}


