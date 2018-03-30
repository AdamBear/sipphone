#include "digitmap_inc.h"

#ifdef IF_FEATURE_DIGIT_MAP
void DigitMap_Init()
{
    _DigitMapManger.Init();
    _DigitMapManger.Init_CallLogDM();
}

bool DigitMap_GetFmtUri(yl::string & strNumber, int iAccount/* = -1*/)
{
    int iAttr = _DigitMapManger.GetAction(strNumber, iAccount).iResult;
    if ((iAttr & DM_MATCH_ATTR_REPLACE) && (iAttr & DM_MATCH_RESULT_FULL))
    {
        strNumber = _DigitMapManger.GetAction(strNumber, iAccount).strFmt;
        return true;
    }

    return false;
}

bool DigitMap_GetLineID(const yl::string & strNumber, int & iAccount)
{
    const DigitMapInfo::DMAction & result = _DigitMapManger.GetAction(strNumber, iAccount);
    if (result.iResult & DM_MATCH_ACTION_SELECT_ACCOUNT)
    {
        int iTmpAccount = result.iLineID;
        if (iTmpAccount == AUTO_SELECT_LINE_ID)
        {
            iTmpAccount = acc_GetDefaultAccount();
        }

        if (iTmpAccount != iAccount && acc_IsAccountIDValid(iTmpAccount))
        {
            TALK_INFO("DigitMap change account [%d]->[%d][%d]", iAccount, iTmpAccount, result.iLineID);
            iAccount = iTmpAccount;
            return true;
        }
    }

    return false;
}

UINT DigitMap_GetTimer(const yl::string & strNumber, int iAccount/* = -1*/)
{
    return _DigitMapManger.GetAction(strNumber, iAccount).iTimer;
}

bool DigitMap_IsInBlacklist(const yl::string & strNumber, int iAccount /*= -1*/)
{
    return _DigitMapManger.GetAction(strNumber, iAccount).iResult & DM_MATCH_ACTION_BLOCK;
}

bool DigitMap_IsDelayCallout(const yl::string & strNumber, int iAccount /*= -1*/)
{
    int iAction = _DigitMapManger.GetAction(strNumber, iAccount).iResult;
    // 禁止呼出的时候也有呼出的过程（流程和正常呼出一致），只是在真正呼出时被截断而已
    return (iAction & DM_MATCH_ACTION_CALLOUT) || (iAction & DM_MATCH_ACTION_NOACTION_CALLOUT);
}

bool DigitMap_PlayDialTone(const yl::string & strNumber, int iAccount /*= -1*/)
{
    return _DigitMapManger.GetAction(strNumber, iAccount).iResult & DM_MATCH_ACTION_DIAL_TONE;
}

bool DigitMap_IsEnable(int iAccount /*= -1*/)
{
    // 账号模式开关优先
    return GetDMSwitchByAccount(kszDigitMapSwitch, iAccount);
}

bool DigitMap_IsConfigEnable(LPCSTR lpConfigName, int iAccount /*= -1*/)
{
    // 账号模式开关优先
    return GetDMSwitchByAccount(kszDigitMapSwitch, iAccount)
           && GetDMSwitchByAccount(lpConfigName, iAccount);
}

bool DigitMap_GetFmtIncomingNumber(yl::string& strNumber)
{
    return _DigitMapManger.ReplaceIncomingNumber(strNumber);
}

#else

void DigitMap_Init()
{
}

bool DigitMap_GetFmtUri(yl::string & strNumber, int iAccount/* = -1*/)
{
    return false;
}

bool DigitMap_GetLineID(yl::string & strNumber, int & iAccount)
{
    return false;
}

UINT DigitMap_GetTimer(const yl::string & strNumber, int iAccount/* = -1*/)
{
    return invalid_timer;
}

bool DigitMap_IsInBlacklist(const yl::string & strNumber, int iAccount /*= -1*/)
{
    return false;
}

bool DigitMap_IsDelayCallout(const yl::string & strNumber, int iAccount /*= -1*/)
{
    return false;
}

bool DigitMap_PlayDialTone(const yl::string & strNumber, int iAccount /*= -1*/)
{
    return false;
}

bool DigitMap_IsEnable(int iAccount /*= -1*/)
{
    return false;
}

bool DigitMap_IsConfigEnable(LPCSTR lpConfigName, int iAccount /*= -1*/)
{
    return false;
}

bool DigitMap_GetFmtIncomingNumber(yl::string& strNumber)
{
    return false;
}

#endif
