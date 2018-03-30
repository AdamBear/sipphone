#include "digitmap_inc.h"

#ifdef IF_FEATURE_DIGIT_MAP

#define AUTO_QUIT_TIMER 61

#define DIGIT_MAP_CFG_PREFIX "Account"

int DigitMapInfo::DMAction::CreateTimer(const DigitMapInfo & dmInfo, int iIndex)
{
    const VecDigitMap & vecDMap = dmInfo.vecDigitMap;

    iTimer = invalid_timer;

    bool bBlock = isFlag(DM_MATCH_ACTION_BLOCK);
    bool bCallout = isFlag(DM_MATCH_ACTION_CALLOUT);
    bool bDelayCallout = isFlag(DM_MATCH_ACTION_NOACTION_CALLOUT);
    bool bSemiMatched = isFlag(DM_MATCH_RESULT_SEMI);
    bool bFullMatched = isFlag(DM_MATCH_RESULT_FULL);
    bool bHaveTimer = isFlag(DM_MATCH_ATTR_TIMER);

    // 非呼出不设置时间
    if (!bBlock && !bCallout && !bDelayCallout)
    {
        return iTimer;
    }

    // 越界，NoAction时符合这种情况，查不到时间源
    if (iIndex < 0 || iIndex >= vecDMap.size())
    {
        if (bBlock || bCallout)
        {
            iTimer = 1;
        }
        else if (bDelayCallout && dmInfo.vecLongTimer.size() > 0)
        {
            // 默认取第一个长时间
            iTimer = dmInfo.vecLongTimer[0] * 1000;
        }

        return iTimer;
    }

    // 规则不自带时间标志，直接立即呼出，通过设置1ms定时器实现
    if (!bHaveTimer && bFullMatched)
    {
        if (bBlock || bCallout)
        {
            return iTimer = 1;
        }
    }

    // 优先使用自带的时间
    if (bHaveTimer)
    {
        iTimer = vecDMap[iIndex].GetTimer();

        // 部分匹配模式下，如果出现呼出时间为0的情况，意味着不呼出，此时取最大时间可以满足此需求
        if (bSemiMatched && iTimer == 0)
        {
            return iTimer = invalid_timer;
        }
    }

    if (iTimer == invalid_timer || bSemiMatched || bDelayCallout)
    {
        // 部分匹配或者延时呼出时使用长超时时间
        const VecDigitMapTimer & vecTimer =
            (bSemiMatched || bDelayCallout) ? dmInfo.vecLongTimer : dmInfo.vecShortTimer;

        int ivecSize = vecTimer.size();
        if (iIndex >= 0 && ivecSize > 0)
        {
            iTimer = iIndex < ivecSize ? vecTimer[iIndex] : vecTimer[ivecSize - 1];

            // 部分匹配模式下，如果出现呼出时间为0的情况，意味着不呼出，此时取最大时间可以满足此需求
            if (bSemiMatched && iTimer == 0)
            {
                return iTimer = invalid_timer;
            }
        }
        else
        {
            iTimer = invalid_timer;
        }
    }

    if (iTimer == invalid_timer && bCallout)
    {
        // 所有时间都不可用的时候, 设置时间大于超时退出时间
        iTimer = AUTO_QUIT_TIMER;
    }

    // 0代表立即呼出
    if (iTimer == 0)
    {
        return iTimer = 1;
    }

    if (iTimer != invalid_timer)
    {
        // 转化为毫秒为单位
        iTimer *= 1000;
    }

    return iTimer;
}

bool DigitMapInfo::CreateDefaultAction(const DigitMapInfo & refInfoData)
{
    if (!dmActionResult.isValid())
    {
        UINT iResult = DM_MATCH_RESULT_NONE;
        switch (refInfoData.dmConfig.eNoMatchAction)
        {
        case DMConfigCache::NMA_CALLOUT:
            iResult = DM_MATCH_ACTION_CALLOUT;
            break;
        case DMConfigCache::NMA_BLOCK:
            iResult = (DM_MATCH_ACTION_BLOCK | DM_MATCH_ACTION_CALLOUT);
            break;
        case DMConfigCache::NMA_NOACTION:
            iResult = DM_MATCH_ACTION_NOACTION_CALLOUT;
            break;
        default:
            break;
        }

        TALK_INFO("CreateDefaultAction result=[%d]", iResult);

        if (IsAttrValid(iResult))
        {
            dmActionResult.addFlag(iResult);
            int iTimer = dmActionResult.CreateTimer(refInfoData, -1);
            return (iTimer != invalid_timer && iTimer > 0);
        }
    }

    return false;
}

bool GetDMSwitchByAccount(LPCSTR lpCfg, int iAccount)
{
    yl::string strValue = GetDMCfgByAccount(lpCfg, iAccount);
    return atoi(strValue.c_str());
}

yl::string GetDMCfgByAccount(LPCSTR lpCfg, int iAccount)
{
    if (iAccount >= 0
#ifdef IF_FEATURE_H323
            && acc_GetAccountProtocol(iAccount) != PROTOCOL_H323
#endif //#ifdef IF_FEATURE_H323
       )
    {
        yl::string strCfgID = commonAPI_FormatString(DIGIT_MAP_CFG_PREFIX "%s", lpCfg);
        yl::string strValue = configParser_GetCfgItemStringValue(strCfgID.c_str(), iAccount);
        if (!strValue.empty())
        {
            return strValue;
        }
    }

    return configParser_GetConfigString(lpCfg);
}

bool DigitMapInfo::DMAction::operator> (const DMAction & other)
{
    bool bThisSemi = isFlag(DM_MATCH_RESULT_SEMI);
    bool bThisFull = isFlag(DM_MATCH_RESULT_FULL);
    bool bOtherSemi = other.isFlag(DM_MATCH_RESULT_SEMI);
    bool bOtherFull = other.isFlag(DM_MATCH_RESULT_FULL);
    bool bThisDialTone = isFlag(DM_MATCH_ACTION_DIAL_TONE);
    bool bOtherDialTone = other.isFlag(DM_MATCH_ACTION_DIAL_TONE);
    bool bThisTimer = isFlag(DM_MATCH_ATTR_TIMER);
    bool bOtherTimer = other.isFlag(DM_MATCH_ATTR_TIMER);
    bool bThisBlock = isFlag(DM_MATCH_ACTION_BLOCK);
    bool bOtherBlock = other.isFlag(DM_MATCH_ACTION_BLOCK);

    // 容错处理
    if ((bThisSemi && bThisFull) || (bOtherSemi && bOtherFull)
            || isFlag(DM_MATCH_PARSE_ERROR) || other.isFlag(DM_MATCH_PARSE_ERROR))
    {
        TALK_ERR("DMAction fatal error [%d][%d][%d][%d]", bThisSemi, bThisFull, bOtherSemi, bOtherFull);
        return false;
    }

#define SELECT_OPTIMAL(bThis, bOther) \
    if (((bThis) || (bOther)) && !((bThis) && (bOther))) { return !(bThis); }

    if (bThisSemi && bOtherSemi)
    {
        // 部分匹配模式下，带,的优先（,优先级比T!高）
        SELECT_OPTIMAL(bThisDialTone, bOtherDialTone);
    }

    // 时间相同的情况下
    if (iTimer == other.iTimer)
    {
        // 带!的优先
        SELECT_OPTIMAL(bThisBlock, bOtherBlock);

        // 带T的优先
        SELECT_OPTIMAL(bThisTimer, bOtherTimer);
    }

    // 此处-1是最大的，所以优先级会最低
    return iTimer > other.iTimer;
}

#endif
