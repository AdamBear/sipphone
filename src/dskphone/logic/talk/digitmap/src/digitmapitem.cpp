#include "digitmap_inc.h"

#ifdef IF_FEATURE_DIGIT_MAP

#define dm_error_str    " fatal error"

//#define IF_EXTEND_FORMATE

#if 0000000000 // 如果正则表达式库没有这个接口
#include "deelx.h"

static int regExp_IsMatchedExactEx(const yl::string & strPattern, const yl::string & strSource,
                                   int & iStartPos, int & iEndPos)
{
    static CRegexp regexp;

    regexp.m_builder.Clear();
    regexp.Compile(strPattern.c_str());
    MatchResult result = regexp.Match(strSource.c_str());

    iStartPos = result.GetStart();
    iEndPos = result.GetEnd();

    return (result.IsMatched() > 0 && iStartPos >= 0 && iEndPos > iStartPos) ? 0 : -1;
}
#endif

bool DigitMapItem::MatchInfo(const yl::string & strSource, const yl::string & strPattern,
                             int & iStartPos, int & iEndPos)
{
    // 两个为空时，认为匹配上，正则表达式接口认为不匹配，需做特殊处理
    if (strSource.empty() && strPattern.empty())
    {
        iStartPos = iEndPos = 0;
        return true;
    }

    return regExp_IsMatchedExactEx(strPattern.c_str(), strSource.c_str(), iStartPos, iEndPos) == 0;
}

bool DigitMapItem::isMatched(const yl::string & strSource, const yl::string & strPattern)
{
    if (strSource.empty() && strPattern.empty())
    {
        return true;
    }

    return regExp_IsMatchedExact(strPattern.c_str(), strSource.c_str()) == 0;
}

int DigitMapItem::regMatchedLength(const yl::string & strSource, const yl::string & strPattern)
{
    int iStartPos, iEndPos;
    return MatchInfo(strSource, strPattern, iStartPos, iEndPos) ? iEndPos - iStartPos : 0;
}

UINT DigitMapItem::isSemiMatched(const yl::string & strSource) const
{
    if (strSource.empty())
    {
        return GetCommonFlag(strSource, DM_MATCH_RESULT_NONE);
    }

    // 不判断完全匹配，减少一次匹配，完全匹配应该在单例中优先判断

    // 可能发生匹配的正则表达式范围
    yl::string strReg = GetRegByCount(strSource.length());

    bool bPosConfirm;
    int iCount = GetMinRegCount(strReg, bPosConfirm);

    // 位置固定的处理，相对简单
    if (bPosConfirm)
    {
        // 正则表达式字符个数比输入的少，根据一一匹配原则，肯定不会发生匹配
        if (iCount < strSource.length())
        {
            return DM_MATCH_RESULT_NONE;
        }
        // 位置固定，直接返回
        if (isMatched(strSource, strReg))
        {
            return GetCommonFlag(strSource, DM_MATCH_RESULT_SEMI);
        }
    }
    // 包含*位置不固定的处理分支
    else
    {
        iCount = GetRegElementCount(m_strRegMatch);
        int iLowerIndex = 0;
        int iMaxLen = strSource.length() *
                      2;  // warring:最大只能是2倍，当出现.的时候才可能出现
        int iUpperIndex = chMIN(iMaxLen, iCount);
        int iSourceLength = strSource.length();

        // 二分法搜索位置
        while (iLowerIndex <= iUpperIndex)
        {
            int iMidIndex = (iUpperIndex + iLowerIndex) / 2;
            yl::string strRegTmp = GetRegByCount(iMidIndex);
            int iLength = regMatchedLength(strSource, strRegTmp);

            // 最后一次匹配
            if (iLowerIndex == iUpperIndex)
            {
                return (iLength == iSourceLength) ? GetCommonFlag(strSource,
                        DM_MATCH_RESULT_SEMI) : DM_MATCH_RESULT_NONE;
            }

            // 完全匹配上了
            if (iLength == iSourceLength)
            {
                return GetCommonFlag(strSource, DM_MATCH_RESULT_SEMI);
            }
            else if (iLength == 0)
            {
                iUpperIndex = iMidIndex - 1;
            }
            else if (iLength < iSourceLength)
            {
                iLowerIndex = iMidIndex + 1;
            }
        }
    }

    return DM_MATCH_RESULT_NONE;
}

UINT DigitMapItem::RegFullMatch(const yl::string & strSource) const
{
    if (isFlag(DM_MATCH_PARSE_ERROR))
    {
        return DM_MATCH_PARSE_ERROR;
    }

    if (isMatched(strSource, m_strRegMatch))
    {
        UINT uAttr = GetCommonFlag(strSource, DM_MATCH_RESULT_FULL);
        return GetExtraAttr(uAttr);
    }

    return DM_MATCH_RESULT_NONE;
}

UINT DigitMapItem::RegSemiMatch(const yl::string & strSource) const
{
    if (isFlag(DM_MATCH_PARSE_ERROR))
    {
        return DM_MATCH_PARSE_ERROR;
    }

    UINT uAttr = isSemiMatched(strSource);
    if (uAttr & DM_MATCH_RESULT_SEMI)
    {
        return GetExtraAttr(uAttr);
    }

    return DM_MATCH_RESULT_NONE;
}

void DigitMapItem::Parse(const yl::string & strSource)
{
    // 先清空
    clear();

    m_strOrg = strSource;

    // 如： 12<T3>R2XR45R45<23X:45> 最终转化org为 12<2X:45>45<23X:45> 和 122X4523X 两个串

    // 默认不区分大小写，统一格式化为大写
    m_strOrg.to_upper();
    // 去掉所有空格
    for (int i = 0; i < m_strOrg.size();)
    {
        int iCount = 0;
        int iPos = i;

        // 去掉所有空格
        if (m_strOrg[i] == ' ')
        {
            iCount = 1;
        }
        // 限制呼出
        else if (m_strOrg[i] == '!')
        {
            iCount = 1;
            addFlag(DM_MATCH_ACTION_BLOCK);
        }
        // 时间、账号解析
        else if (m_strOrg[i] == 'T' || m_strOrg[i] == 'A')
        {
            iCount = 1;
            bool bTimer = m_strOrg[i] == 'T';
            addFlag(bTimer ? DM_MATCH_ATTR_TIMER : DM_MATCH_ACTION_SELECT_ACCOUNT);

            // <T3><A3>格式解析
            if (iPos > 0 && m_strOrg[iPos - 1] == '<')
            {
                int iIndex = iPos + 1;
                // 需要回退一个位置
                --i;

                yl::string::size_type iEndPos = m_strOrg.find('>', iIndex);
                if (iEndPos != yl::string::npos)
                {
                    int iTimerValueCount = iEndPos - iIndex;
                    yl::string strSubTimer = m_strOrg.substr(iIndex, iTimerValueCount);
                    if (strSubTimer.find(':') != yl::string::npos)
                    {
                        // 后面的应该是替换规则，不应该当作时间处理
                        ++i;
                    }
                    else
                    {
                        int iPeekValue = atoi(strSubTimer.c_str());
                        if (bTimer)
                        {
                            m_uTimer = (UINT)iPeekValue;
                        }
                        else
                        {
                            m_iLineID = --iPeekValue;
                        }

                        // 原始字符串不保存含T的部分
                        iCount = iTimerValueCount + 3;
                    }
                }
            }
        }

        if (iCount > 0)
        {
            m_strOrg.erase(i, iCount);
        }
        else
        {
            ++i;
        }
    }

    // 解析RRR替换 格式 R23R34R 可能出现多次，所以需要循环
    yl::string::size_type iPos = m_strOrg.find('R');
    int iIndex = 0;
    while (iPos != yl::string::npos)
    {
        // RRR格式是Polycom话机格式，要兼容，转化为Yealink定义的转化符<22:33>格式
        static const char s_rep_ch[3] = { '<', ':', '>' };
        m_strOrg[iPos] = s_rep_ch[iIndex++ % 3];

        iPos = m_strOrg.find('R', iPos + 1);
    }

    if (!CheckFormat())
    {
        addFlag(DM_MATCH_PARSE_ERROR);
        TALK_ERR("%s %s str=[%s] source=[%s]", __FUNCTION__, dm_error_str, m_strOrg.c_str(),
                 strSource.c_str());
        return;
    }

    // 开始解析用于匹配的字符
    m_strRegMatch = m_strOrg;

    // 去掉非正则表达式字符，尝试转化为正则表达式
    if (TrimReplaceFlag(m_strRegMatch))
    {
        addFlag(DM_MATCH_ATTR_REPLACE);
    }

    Trim2StdRegexp(m_strRegMatch);

    // ,号的处理，需单独一遍扫描
    yl::string::size_type nPos = m_strRegMatch.find_first_of(',');
    while (nPos != yl::string::npos)
    {
        addFlag(DM_MATCH_ACTION_DIAL_TONE);
        m_vecCommaPos.push_back(m_strRegMatch.substr(0, nPos));
        m_strRegMatch.erase(nPos, 1);
        nPos = m_strRegMatch.find_first_of(',', nPos);
    }

    // 原始字符串也需要去除标志
    m_strOrg = commonAPI_replaceSubStringByTag(m_strOrg, ",", "");

#if 0
    TALK_INFO("DigitMapItem::Parse src=[%s] str=[%s] reg=[%s] comma=[%d] attr=[%d]",
              strSource.c_str(), m_strOrg.c_str(), m_strRegMatch.c_str(), m_vecCommaPos.size(), m_uAttr);
#endif
    return;
}

bool DigitMapItem::CheckFormat()
{
    /***** 以下是基本的语法错误，影响继续解析  *****/
    /* <23:XX> */

    yl::string::size_type ilPos = m_strOrg.find('<');
    yl::string::size_type irPos = m_strOrg.find('>');
    while (ilPos != yl::string::npos || irPos != yl::string::npos)
    {
        if (ilPos == yl::string::npos || irPos == yl::string::npos || ilPos > irPos)
        {
            return false;
        }

        yl::string strMid = m_strOrg.substr(ilPos + 1, irPos - ilPos - 1);
        // 嵌套
        if (strMid.find_first_of("<") != yl::string::npos)
        {
            return false;
        }

        yl::string::size_type imPos = strMid.find(':');
        if (imPos != yl::string::npos)
        {
            yl::string strMidPost = strMid.substr(imPos + 1);
            LPCSTR lpstrError = "X.[]";

            // 非确定字符不合法
            if (strMidPost.find_first_of(lpstrError) != yl::string::npos)
            {
                return false;
            }
        }

        ilPos = m_strOrg.find('<', irPos + 1);
        irPos = m_strOrg.find('>', irPos + 1);
    }

    /***** 以下是容错，出现如下错误并不影响继续解析  *****/

    // 容错：连续的两个,转化为一个处理
    while (m_strOrg.find(",,") != yl::string::npos)
    {
        m_strOrg = commonAPI_replaceSubStringByTag(m_strOrg, ",,", ",");
    }
    // 容错：连续的两个.转化为一个处理
    while (m_strOrg.find("..") != yl::string::npos)
    {
        m_strOrg = commonAPI_replaceSubStringByTag(m_strOrg, "..", ".");
    }

    return true;
}

bool DigitMapItem::TrimReplaceFlag(yl::string & strReg)
{
    int iLength = strReg.length();
    // 11<22:33>44 格式化为 112244
    yl::string::size_type ilPos = strReg.find('<');
    while (ilPos != yl::string::npos)
    {
        yl::string::size_type irPos = strReg.find('>', ilPos + 1);
        if (irPos != yl::string::npos)
        {
            int iCount = irPos - ilPos - 1;

            yl::string strMid = strReg.substr(ilPos + 1, iCount);
            yl::string strPrefix = strReg.substr(0, ilPos);
            yl::string strPost = strReg.substr(irPos + 1);

            strReg.erase(ilPos, iCount + 2);

            yl::string::size_type iPos = strMid.find(':');
            if (iPos != yl::string::npos)
            {
                yl::string strSub = strMid.substr(0, iPos);
                // 重组三段式
                strReg = strPrefix + strSub + strPost;
            }
        }

        ilPos = strReg.find('<', ilPos);
    }

    return iLength != strReg.length();
}

bool DigitMapItem::Trim2StdRegexp(yl::string & strReg)
{
    // 转化为等价的正则表达式
    strReg = commonAPI_replaceSubStringByTag(strReg, "*", "\\*");
    strReg = commonAPI_replaceSubStringByTag(strReg, "X", "\\d");
    strReg = commonAPI_replaceSubStringByTag(strReg, "+", "\\+");

    // 为了和Polycom兼容，改'*'为'+'至少一次匹配，即‘+’
    // 出现在[X.]里的.不能转化为+处理
    int iBracketLevel = 0;
    for (int i = 0; i < strReg.length(); ++i)
    {
        if (strReg[i] == '[')
        {
            ++iBracketLevel;
        }
        else if (strReg[i] == ']')
        {
            --iBracketLevel;
        }

        if (iBracketLevel == 0 && strReg[i] == '.')
        {
            strReg[i] = '+';
        }
    }

    return true;
}

yl::string DigitMapItem::GetRegByCount(int iCount) const
{
    const yl::string & strReg = m_strRegMatch;

    int iLength = strReg.length();

    // 意外
    if (iCount >= iLength)
    {
        return strReg;
    }

    // 实际序号
    int iIndex = 0;
    // 个数序号
    int iWdIndex = 0;

    while (iIndex < iLength)
    {
        int iPos = iIndex++;

        if (strReg[iPos] == '\\')
        {
            continue;
        }
        else if (strReg[iPos] == '[')
        {
            yl::string::size_type iPosR = strReg.find(']', ++iPos);
            if (iPosR != yl::string::npos)
            {
                iIndex = ++iPosR;
            }
        }
#ifdef IF_EXTEND_FORMATE
        else if (strReg[iPos] == '(')
        {
            yl::string::size_type iPosR = strReg.find_first_of(')', iPos + 1);
            if (iPosR != yl::string::npos)
            {
                if (iPosR < strReg.length() - 1 && strReg[iPosR + 1] == '*')
                {
                    iIndex = iPosR + 2;
                }
                else
                {
                    iWdIndex += GetRegElementCount(strReg.substr(iPos, iPosR - iPos));
                    if (iWdIndex >= iCount)
                    {
                        break;
                    }

                    iIndex = ++iPosR;
                }
            }

            continue;
        }
#endif
        // 非转义字符当成一个整体处理，多重匹配字符 '*' '?' 这里不会用到
        else if (iPos > 0 && strReg[iPos - 1] != '\\')
        {
            if (strReg[iPos] == '*')
            {
                continue;
            }
            else if (strReg[iPos] == '+')
            {
                // 不需要处理，'+'当成一个独立字符处理
            }
        }

        ++iWdIndex;

        if (iWdIndex >= iCount)
        {
            break;
        }
    }

    return strReg.substr(0, iIndex);
}

//获取正则表达式代表的最小位数，bConfirm表示位数是否固定
int DigitMapItem::GetMinRegCount(const yl::string & strReg, bool & bConfirm)
{
    bConfirm = true;

    int iMinCount = 0;
    int iIndex = 0;

    while (iIndex < strReg.length())
    {
        int iPos = iIndex++;

        if (strReg[iPos] == '\\')
        {
            continue;
        }
        else if (strReg[iPos] == '[')
        {
            yl::string::size_type iPosR = strReg.find(']', ++iPos);
            if (iPosR != yl::string::npos)
            {
                iIndex = ++iPosR;
            }
        }
#ifdef IF_EXTEND_FORMATE
        else if (strReg[iPos] == '(')
        {
            yl::string::size_type iPosR = strReg.find_first_of(')', iPos + 1);
            if (iPosR != yl::string::npos)
            {
                if (iPosR < strReg.length() - 1 && strReg[iPosR + 1] == '*')
                {
                    bConfirm = false;
                    iIndex = iPosR + 2;
                }
                else
                {
                    bool bSubConfirm = true;
                    iMinCount += GetMinRegCount(strReg.substr(iPos, iPosR - iPos), bSubConfirm);
                    bConfirm &= bSubConfirm;
                    iIndex = ++iPosR;
                }
            }

            continue;
        }
#endif
        else if (iPos > 0 && strReg[iPos - 1] != '\\')
        {
            // *可以匹配0次
            if (strReg[iPos] == '*')
            {
                bConfirm = false;
                --iMinCount;
                continue;
            }
            // +至少匹配一次
            else if (strReg[iPos] == '+')
            {
                bConfirm = false;
                continue;
            }
        }

        ++iMinCount;
    }

    return iMinCount;
}

int DigitMapItem::GetRegElementCount(const yl::string & strReg)
{
    int iLength = strReg.length();
    int iIndex = 0;
    int iWdIndex = 0;

    while (iIndex < iLength)
    {
        int iPos = iIndex++;

        if (strReg[iPos] == '\\')
        {
            continue;
        }
        else if (strReg[iPos] == '[')
        {
            yl::string::size_type iPosR = strReg.find(']', ++iPos);
            if (iPosR != yl::string::npos)
            {
                iIndex = ++iPosR;
            }
        }
#ifdef IF_EXTEND_FORMATE
        else if (strReg[iPos] == '(')
        {
            yl::string::size_type iPosR = strReg.find_first_of(')', iPos + 1);
            if (iPosR != yl::string::npos)
            {
                if (iPosR < strReg.length() - 1 && strReg[iPosR + 1] == '*')
                {
                    iIndex = iPosR + 2;
                }
                else
                {
                    iWdIndex += GetRegElementCount(strReg.substr(iPos, iPosR - iPos));
                    iIndex = ++iPosR;
                }
            }

            continue;
        }
#endif
        // 非转义字符当成一个整体处理，多重匹配字符 '*' '?' 这里不会用到
        else if (iPos > 0 && strReg[iPos - 1] != '\\')
        {
            if (strReg[iPos] == '*')
            {
                continue;
            }
            else if (strReg[iPos] == '+')
            {
                //+当成一个字符
            }
        }

        ++iWdIndex;
    }

    return iWdIndex;
}

bool DigitMapItem::GetRegReplace(const yl::string & strHead, const yl::string & strMask,
                                 const yl::string & strDst, const yl::string & strPost, yl::string & strResult) const
{
    // 是否确定的个数
    bool bHeadPosConfirm, bMidPosConfirm, bPostPosConfirm;
    int iHeadCount = GetMinRegCount(strHead, bHeadPosConfirm);
    int iMidCount = GetMinRegCount(strMask, bMidPosConfirm);
    int iPostCount = GetMinRegCount(strPost, bPostPosConfirm);

    // 2的3次方，一共8种状态，分别处理，优化效率。只要有2个确定就可以直接替换
    // 如果是包含Post字段的2个以上字段不确定，确定最后一段位置时，需要进行整个字符串的扫描，而不仅仅是当前段
    // 2种：111|110
    bool bHandled = true;
    if (bHeadPosConfirm && bMidPosConfirm)
    {
        strResult.replace(iHeadCount, iMidCount, strDst);
    }
    // 1种：101
    else if (bHeadPosConfirm && bPostPosConfirm)
    {
        int iLength = strResult.length() - iHeadCount - iPostCount;
        strResult.replace(iHeadCount, iLength, strDst);
    }
    // 1种：011
    else if (bPostPosConfirm && bMidPosConfirm)
    {
        int iStartPos = strResult.length() - iMidCount - iPostCount;
        strResult.replace(iStartPos, iMidCount, strDst);
    }
    // 4种：100|001|010|000
    // 有两种以上不确定，性能差。理论上能够剥离出100|001|010三种情况，但这样反而增加实现的复杂度
    else
    {
        int iSourceLength = strResult.length();
        for (int i = iHeadCount; i <= iSourceLength; ++i)
        {
            for (int j = iMidCount; i + j <= iSourceLength; ++j)
            {
                if (isMatched(strResult.substr(0, i), strHead)
                        && isMatched(strResult.substr(i, j), strMask)
                        && isMatched(strResult.substr(i + j), strPost))
                {
                    strResult.replace(i, j, strDst);
                    return true;
                }
            }
        }

        bHandled = false;
    }

    return bHandled;
}

yl::string DigitMapItem::GetResult(const yl::string & strSource) const
{
    yl::string strResult = strSource;

    if (isFlag(DM_MATCH_ATTR_REPLACE) && !isFlag(DM_MATCH_PARSE_ERROR))
    {
        yl::string strOrg = m_strOrg;
        yl::string strPrefix;

        yl::string::size_type iPos = strOrg.find('>');
        while (iPos != yl::string::npos)
        {
            int iOffsetPos = 1;

            yl::string strFull = strOrg.substr(0, iPos);
            yl::string strPost = strOrg.substr(iPos + 1);

            // 分割
            yl::string::size_type iPrePos = strFull.find('<');
            if (iPrePos != yl::string::npos)
            {
                yl::string strHead = strFull.substr(0, iPrePos);
                yl::string strMid = strFull.substr(iPrePos + 1);

                yl::string::size_type iMidPos = strMid.find(':');
                if (iMidPos != yl::string::npos)
                {
                    yl::string strMask = strMid.substr(0, iMidPos);
                    yl::string strDst = strMid.substr(iMidPos + 1);

                    // 3个字符<:>
                    iOffsetPos = strDst.length() - strMask.length() - 3;

                    yl::string strPostTrimed = strPost;
                    TrimReplaceFlag(strPostTrimed);
                    Trim2StdRegexp(strPostTrimed);

                    Trim2StdRegexp(strMask);

                    // 从上一次位置开始转化为正则表达式，上次的部分其实已经转化过了，再次转有问题
                    yl::string strPre = strHead.substr(strPrefix.length());
                    Trim2StdRegexp(strPre);
                    strHead = strPrefix + strPre;
                    strPrefix = strHead;

                    if (GetRegReplace(strHead, strMask, strDst, strPostTrimed, strResult))
                    {
                        strOrg = strHead + strDst + strPost;
                    }
                    else
                    {
                        iOffsetPos = 1;
                    }
                }
            }

            iPos = strOrg.find('>', iPos + iOffsetPos);
        }
    }

    return strResult;
}

void DigitMapItem::clear()
{
    m_strOrg.clear();
    m_strRegMatch.clear();
    m_vecCommaPos.clear();
    m_uTimer = invalid_timer;
    clearFlag();
}

UINT DigitMapItem::GetTimer() const
{
    return !isFlag(DM_MATCH_PARSE_ERROR) && isFlag(DM_MATCH_ATTR_TIMER) ? m_uTimer : invalid_timer;
}

int DigitMapItem::GetLineID() const
{
    return !isFlag(DM_MATCH_PARSE_ERROR)
           && isFlag(DM_MATCH_ACTION_SELECT_ACCOUNT) ? m_iLineID : AUTO_SELECT_LINE_ID;
}

UINT DigitMapItem::GetExtraAttr(int uBaseAttr) const
{
    if (isFlag(DM_MATCH_PARSE_ERROR))
    {
        return DM_MATCH_PARSE_ERROR;
    }

    UINT uAttr = uBaseAttr;

    if (isFlag(DM_MATCH_ACTION_BLOCK) && (uBaseAttr & DM_MATCH_RESULT_FULL))
    {
        uAttr |= DM_MATCH_ACTION_BLOCK;
    }

    // 只要匹配上一定会触发呼出操作
    if (uAttr & DM_MATCH_RESULT_FULL)
    {
        uAttr |= DM_MATCH_ACTION_CALLOUT;

        if (isFlag(DM_MATCH_ACTION_SELECT_ACCOUNT))
        {
            uAttr |= DM_MATCH_ACTION_SELECT_ACCOUNT;
        }
    }

    if (uAttr & DM_MATCH_RESULT_SEMI)
    {
        uAttr |= DM_MATCH_ACTION_CALLOUT;
    }

    if (isFlag(DM_MATCH_ATTR_REPLACE))
    {
        uAttr |= DM_MATCH_ATTR_REPLACE;
    }

    if (isFlag(DM_MATCH_ATTR_TIMER))
    {
        uAttr |= DM_MATCH_ATTR_TIMER;
    }

    return uAttr;
}

UINT DigitMapItem::GetCommonFlag(const yl::string & strSource, UINT uBaseAttr) const
{
    if (isFlag(DM_MATCH_PARSE_ERROR))
    {
        return DM_MATCH_PARSE_ERROR;
    }

    UINT uAttr = uBaseAttr;

    if (isFlag(DM_MATCH_ACTION_DIAL_TONE))
    {
        // 可能有多个逗号, 数组中的序号是有序递增的
        for (int i = 0; i < m_vecCommaPos.size(); ++i)
        {
            const yl::string & strValue = m_vecCommaPos[i];
            if (isMatched(strSource, strValue))
            {
                uAttr |= DM_MATCH_ACTION_DIAL_TONE;

                // 特殊的状态类似 ",1," 的时候如果两者都为空，则应该视为匹配上,号
                if (strValue.empty() && strSource.empty())
                {
                    uAttr |= DM_MATCH_RESULT_SEMI;
                }

                break;
            }
        }
    }

    return uAttr;
}

#endif
