#include "digitmap_inc.h"

#ifdef IF_FEATURE_DIGIT_MAP

LRESULT CDigitMapManger::OnMessage(msgObject & msg)
{
    bool bHanled = true;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (msg.wParam == ST_DIGIT_MAP)
            {
                return _DigitMapManger.Reload();
            }
            else if (msg.wParam == ST_LOCALCALLLOG_REPLACERULE)
            {
                return _DigitMapManger.Init_CallLogDM();
            }
        }
        break;
    default:
        bHanled = false;
        break;
    }

    return bHanled;
}

CDigitMapManger::CDigitMapManger() : m_bInited(false)
{
    m_dmAction.clear();
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CDigitMapManger::OnMessage);
}

const CDigitMapManger::DMAction & CDigitMapManger::GetAction(const yl::string & strText,
        int iAccount/* = -1*/)
{
    if (!DigitMap_IsEnable(iAccount))
    {
        return m_dmAction;
    }

    // V81新需求，需先去掉非法字符再匹配
    yl::string strToMatch = strText;
    RemoveInvalidChar(strToMatch);
    DigitMapInfo & dmInfo = GetDMInfo(iAccount);
    DMAction & dmAction = dmInfo.dmActionResult;

    if (strToMatch != dmInfo.strLastMatch || !m_bInited)
    {
        bool bAccMod = IsAccountModeEnable(iAccount);
        // 如果对应账号没有配置，则结果将保存在Globle中
        dmAction.clear();

        DigitMapInfo & refInfoData = bAccMod ? dmInfo : m_DMGlobleInfo;

        GetAction(refInfoData, dmInfo, strToMatch);

        // 优先进行账号模式的匹配
        dmInfo.CreateDefaultAction(refInfoData);

        TALK_INFO("GetAction text=[%s] acc=[%d] mod=[%d] result=[%d] timer=[%d] fmt=[%s] line=[%d]",
                  strToMatch.c_str(), iAccount, bAccMod, dmAction.iResult, dmAction.iTimer, dmAction.strFmt.c_str(),
                  dmAction.iLineID);

        dmInfo.strLastMatch = strToMatch;
    }

    return dmAction;
}

void CDigitMapManger::GetAction(DigitMapInfo & dmInfo, DigitMapInfo & dmRefer, const yl::string & strText, bool bFullMatchOnly /* = false*/)
{
    VecDigitMap & vecDigitMap = dmInfo.vecDigitMap;

    // 全局模式的DMAction结构体里面不缓存数据
    if (vecDigitMap.size() == 0)
    {
        return;
    }

    // 结果保存在refer中
    DMAction & dmAction = dmRefer.dmActionResult;
    int iVecSize = vecDigitMap.size();

    LPCSTR lpDmItem = NULL;
    if (!dmAction.isFullMatched())
    {
        DMAction tmpAction;

        // 完全匹配优先级高
        for (int i = 0; i < iVecSize; ++i)
        {
            DigitMapItem & dmItem = vecDigitMap[i];

            UINT uAttr = dmItem.RegFullMatch(strText);
            if (DigitMapInfo::isFullMatched(uAttr))
            {
                dmAction.addFlag(uAttr);
                dmAction.CreateTimer(dmInfo, i);

                // 取时间距离最近的一条记录
                if (tmpAction > dmAction)
                {
                    lpDmItem = dmItem.GetOriginalString();
                    tmpAction = dmAction;
                    tmpAction.strFmt = dmItem.GetResult(strText);
                    tmpAction.iLineID = dmItem.GetLineID();
                }

                TALK_INFO("GetAction full matched org=[%s] result=[%d] timer=[%d]",
                          dmItem.GetOriginalString(), dmAction.iResult, dmAction.iTimer);

                dmAction.clear();
            }
        }

        dmAction = tmpAction;
    }

    if (bFullMatchOnly)
    {
        return;
    }

    if (!dmAction.isSemiMatched() && !dmAction.isFullMatched())
    {
        DMAction tmpAction;

        // 部分匹配优先级低
        for (int i = 0; i < iVecSize; ++i)
        {
            DigitMapItem & dmItem = vecDigitMap[i];

            UINT uAttr = dmItem.RegSemiMatch(strText);
            if (DigitMapInfo::isSemiMatched(uAttr))
            {
                dmAction.addFlag(uAttr);
                dmAction.CreateTimer(dmInfo, i);

                // 取时间距离最近的一条记录
                if (tmpAction > dmAction)
                {
                    lpDmItem = dmItem.GetOriginalString();
                    tmpAction = dmAction;
                }

                TALK_INFO("GetAction semi matched org=[%s] result=[%d] timer=[%d]",
                          dmItem.GetOriginalString(), dmAction.iResult, dmAction.iTimer);

                dmAction.clear();
            }
        }

        dmAction = tmpAction;
    }

    TALK_INFO("GetAction Mod full=[%d] semi=[%d] str=[%s]",
              dmAction.isFullMatched(), dmAction.isSemiMatched(), lpDmItem ? lpDmItem : "");
}

void CDigitMapManger::Init()
{
    int iAccountNum = acc_AccountNum();
    for (int i = 0; i < iAccountNum; ++i)
    {
        m_vecDMInfo.push_back(DigitMapInfo());
    }

    Reload();
}

void CDigitMapManger::Init(int iIndex)
{
    // -1 代表全局配置模式
    bool bGlobleInfo = iIndex < 0;
    if (!bGlobleInfo && !IsAccountModeEnable(iIndex))
    {
        return;
    }

    DigitMapInfo & dmTarget = GetDMInfo(iIndex);
    yl::string strDigitMap = GetDMCfgByAccount(kszDigitMap, iIndex);
    if (strDigitMap.empty())
    {
        return;
    }

    yl::string strNoMatchAction = GetDMCfgByAccount(kszDigitMapNoMatchAction, iIndex);
    int iAction = strNoMatchAction.empty() ? DigitMapInfo::DMConfigCache::NMA_UNKOWM : atoi(
                      strNoMatchAction.c_str());
    dmTarget.dmConfig.eNoMatchAction = (DigitMapInfo::NoMatchAction)iAction;

    yl::string strLongTimer = GetDMCfgByAccount(kszDigitMapLongTimer, iIndex);
    yl::string strShortTimer = GetDMCfgByAccount(kszDigitMapShortTimer, iIndex);

    TALK_INFO("Init index=[%d] long=[%s] short=[%s] action=[%d]", iIndex, strLongTimer.c_str(),
              strShortTimer.c_str(), iAction);

    YLVector<yl::string> listDigitMap;
    if (ParseString(listDigitMap, strDigitMap, "|"))
    {
        for (YLVector<yl::string>::iterator it = listDigitMap.begin(); it != listDigitMap.end(); ++it)
        {
            DigitMapItem dmItem;
            dmItem.Parse((*it));

            dmTarget.vecDigitMap.push_back(dmItem);
        }
    }

    listDigitMap.clear();
    if (ParseString(listDigitMap, strLongTimer, "|"))
    {
        for (YLVector<yl::string>::iterator it = listDigitMap.begin(); it != listDigitMap.end(); ++it)
        {
            dmTarget.vecLongTimer.push_back(atoi((*it).c_str()));
        }
    }

    listDigitMap.clear();
    if (ParseString(listDigitMap, strShortTimer, "|"))
    {
        for (YLVector<yl::string>::iterator it = listDigitMap.begin(); it != listDigitMap.end(); ++it)
        {
            dmTarget.vecShortTimer.push_back(atoi((*it).c_str()));
        }
    }
}

DigitMapInfo & CDigitMapManger::GetDMInfo(int iIndex /*= -1*/)
{
    if (iIndex >= 0 && iIndex < m_vecDMInfo.size())
    {
        return m_vecDMInfo[iIndex];
    }

    return m_DMGlobleInfo;
}

bool CDigitMapManger::Reload()
{
    m_bInited = false;
    // -1代表全局模式
    for (int i = -1; i < m_vecDMInfo.size(); ++i)
    {
        GetDMInfo(i).clear();
        Init(i);
        // 先预加载一遍，初始化各模式下缓存
        GetAction(yl::string(), i);
    }
    m_bInited = true;

    return true;
}

bool CDigitMapManger::IsAccountModeEnable(int iAccount)
{
    return DigitMap_IsEnable(iAccount);
}

bool CDigitMapManger::Init_CallLogDM()
{
    m_DMIncomingInfo.clear();

    yl::string strDigitMap = configParser_GetConfigString(kszLocalCalllogReplaceRule);
    if (strDigitMap.empty())
    {
        return false;
    }

    YLVector<yl::string> listDigitMap;
    if (ParseString(listDigitMap, strDigitMap, "|"))
    {
        for (YLVector<yl::string>::iterator it = listDigitMap.begin(); it != listDigitMap.end(); ++it)
        {
            DigitMapItem dmItem;
            dmItem.Parse((*it));
            m_DMIncomingInfo.vecDigitMap.push_back(dmItem);
            }
    }
    return true;
}


bool CDigitMapManger::ReplaceIncomingNumber(yl::string & strNumber)
{
    if (configParser_GetConfigString(kszLocalCalllogReplaceRule).empty())
    {
        return false;
    }
    yl::string strToMatch = strNumber;
    RemoveInvalidChar(strToMatch);
    DigitMapInfo& dmInfo = m_DMIncomingInfo;
    DMAction& dmAction = dmInfo.dmActionResult;

    if (strToMatch != dmInfo.strLastMatch)
    {
        GetAction(dmInfo, dmInfo, strToMatch, true);
        dmInfo.strLastMatch = strToMatch;
    }
    TALK_INFO(" ## dmAction.iResult =%d  dmAction.strFmt==%s  strNumber == %s ", dmAction.iResult, dmAction.strFmt.c_str(), strNumber.c_str());
    if ((dmAction.iResult & DM_MATCH_ATTR_REPLACE) && (dmAction.iResult & DM_MATCH_RESULT_FULL))
    {
        strNumber = dmAction.strFmt;
        //操作执行完要清标志，否则无法替换其他的规则号码
        dmAction.clear();
        //缓存的匹配字符需要清除
        dmInfo.strLastMatch.clear();
        return true;
    }
    return false;
}

#endif
