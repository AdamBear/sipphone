#include "emergency_inc.h"


//////////////////////////////////////////////////////////////////////////
CWhitelistMode::CWhitelistMode()
    : CBaseEmergency(EM_WL_ALL)
{}

void CWhitelistMode::LoadEmergency()
{
    m_listEmergency.clear();
    yl::string strCfg = configParser_GetConfigString(kszEmergencyNumberList);
    ParseString(m_listEmergency, strCfg, ",");
    // 为了兼容原有版本，需要校验
    CheckValid(m_listEmergency);

    // 获取DND的紧急号码
    m_listDNDEmergency.clear();
    strCfg = configParser_GetConfigString(kszDNDEmergencyNumberList);
    ParseString(m_listDNDEmergency, strCfg, ",");

    m_listForwardEmergency.clear();
    strCfg = configParser_GetConfigString(kszForwardEmergencyNumberList);
    ParseString(m_listForwardEmergency, strCfg, ",");
}

void CWhitelistMode::CheckValid(ListEmergency & listData)
{
    for (ListEmergency::iterator it = listData.begin(); it != listData.end(); ++it)
    {
        yl::string & strNum = *it;
        strNum.trim_both();

        // 执行校验去掉非数字号码
        for (int i = 0; i < strNum.length();)
        {
            if (isdigit(strNum[i]))
            {
                ++i;
            }
            else
            {
                strNum.erase(i);
            }
        }
    }
}

bool CWhitelistMode::IsEmergencyImp(ListEmergency & listData, LPCSTR lpNum,
                                    bool IsCharCmp/* = false*/)
{
    if (textEMPTY(lpNum))
    {
        return false;
    }

    // Radiran，需要预先格式化非法字符
    yl::string strNumForMatch = lpNum;
    strNumForMatch.trim_both(" \r\t");

    for (ListEmergency::iterator it = listData.begin(); it != listData.end(); ++it)
    {
        const yl::string & strNum = *it;
        if (IsCharCmp)
        {
            if (strncmp(strNum.c_str(), strNumForMatch.c_str(), strNumForMatch.length()) == 0)
            {
                return true;
            }
        }
        else if (strNum == strNumForMatch)
        {
            return true;
        }
    }

    return false;
}

bool CWhitelistMode::IsEmergency(LPCSTR lpNum, bool IsCharCmp /*= false*/)
{
    return IsEmergencyImp(m_listEmergency, lpNum, IsCharCmp);
}

bool CWhitelistMode::IsEmergencyIncomingCall(LPCSTR lpNum)
{
    return configParser_GetConfigInt(kszDNDEmergencySwitch) != 0
           && IsEmergencyImp(m_listDNDEmergency, lpNum);
}

bool CWhitelistMode::IsForwardEmergency(LPCSTR lpNum)
{
    return configParser_GetConfigInt(kszForwardEmergencySwitch) != 0
           && IsEmergencyImp(m_listForwardEmergency, lpNum);
}

// 新增接口
bool CWhitelistMode::Reload(int iType, int lParam)
{
    LoadEmergency();
    return true;
}

void CWhitelistMode::Init()
{
    LoadEmergency();
}

bool CWhitelistMode::IsMatched(const char * cszTarget, int iType, CMatchedInfo * pInfo /*= NULL*/,
                               bool IsCharCmp /*= false*/)
{
    if (textEMPTY(cszTarget))
    {
        return false;
    }

    int iRet = EM_NONE;
    if (iType & EM_WL_DND)
    {
        if (IsEmergencyIncomingCall(cszTarget))
        {
            iRet |= EM_WL_DND;
        }
    }

    if (iType & EM_WL_FWD)
    {
        if (IsForwardEmergency(cszTarget))
        {
            iRet |= EM_WL_FWD;
        }
    }

    if (iType & EM_WL)
    {
        if (IsEmergency(cszTarget, IsCharCmp))
        {
            iRet |= EM_WL;
        }
    }

    if (pInfo != NULL)
    {
        pInfo->m_iLastMatchedMode |= iRet;
    }

    return iRet != EM_NONE;
}

