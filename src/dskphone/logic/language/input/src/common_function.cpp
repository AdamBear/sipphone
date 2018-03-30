#include "common_function.h"
#include "modInput.h"
#include "dsklog/log.h"
#include "language/input/include/langattrmanager.h"
#include "translateiddefine.h"

using namespace yl;

// 翻转时需要跳过的字符 处理类似 11-22-33 （empty）类似翻译
static inline bool IsSkipReverse(WCHAR wch)
{
    // 標點和空白字符
    return iswpunct(wch) || iswspace(wch);
}

static const char s_Pair_Sharp[][2] =
{
    // 目前话机使用到的成对字符
    { '(', ')'},
//  { '{', '}'},
    { '<', '>'},
//  { '[', ']'},
};

// 是否是一个成对字符中的一个
static bool IsOneOfPairsSymbols(WCHAR wch)
{
    for (int i = 0; i < sizeof(s_Pair_Sharp) / sizeof(s_Pair_Sharp[0]); ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            if (wch == s_Pair_Sharp[i][j])
            {
                return true;
            }
        }
    }

    return false;
}

// 将字符转换成成对字符中的另一个
static WCHAR ReserverPairsSymbols(WCHAR wch)
{
    for (int i = 0; i < sizeof(s_Pair_Sharp) / sizeof(s_Pair_Sharp[0]); ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            if (wch == s_Pair_Sharp[i][j])
            {
                return s_Pair_Sharp[i][j == 0 ? 1 : 0];
            }
        }
    }

    return wch;
}

// Bidi 算法的简易实现，只试用话机简单翻译
void YLMiniBidiAlgorithm(yl::wstring & strW)
{
    /*
    一般步驟 示例：" 1. לעדכן כעת?"
    1. 以序号冒号结尾，先格式化冒号
    2. 三段处理，分割以查找目標反轉字符 " 1. "、 "לעדכן כעת"、 "?"
    3. 中間字符完全翻轉 希伯来语之后的第一串数字和希伯来语一并处理
    4. 糾正成對出現的符號
    5. 重组
    */
    /*  13 个 while 醉了,
    一共需要四次完整扫描，分别是，字符截取、两次反转（2次扫描）、纠正符号一次扫描 */


    yl::string strTemp = yl::to_utf8(strW);

    const int iMaxIndex = strW.size() - 1;
    if (iMaxIndex < 1 || !Lang_IsRTL(strW))
    {
        return;
    }

    int iLeftIndex = 0;
    int iRightIndex = iMaxIndex;
    WCHAR wch = L':';

    // 1. 冒號可能是作為序號添加的，需要先做特殊處理
    if (strW[iMaxIndex] == wch)
    {
        // 如果冒号前，还有非RTL字符，冒号不需要纠正
        int irPos = iMaxIndex - 1;
        while (irPos > 0 && (IsSkipReverse(strW[irPos]) || iswdigit(strW[irPos])))
        {
            --irPos;
        }

        if (Lang_IsRTL(strW[irPos]))
        {
            int iIndex = 0;

            //跳过开始的空格
            while (iIndex < iMaxIndex && iswspace(strW[iIndex]))
            {
                ++iIndex;
            }

            // 检测数字
            int iStartPos = iIndex;
            while (iIndex < iMaxIndex && iswdigit(strW[iIndex]))
            {
                ++iIndex;
            }

            // 有數字,且作為序號
            if (iStartPos != iIndex && strW[iIndex] == '.')
            {
                ++iIndex;

                // 跳过数字之后的空格
                iStartPos = iIndex;
                while (iIndex < iMaxIndex && iswspace(strW[iIndex]))
                {
                    ++iIndex;
                }

                if (iIndex == iStartPos && iIndex < iMaxIndex)
                {
                    ++iIndex;
                }

                // 重新调整冒号位置
                strW.erase(iMaxIndex);
                strW.insert(iIndex, 1, wch);
            }

            iLeftIndex = iIndex;
        }
    }

    // 2. 分段处理，取出中间含RTL字符的那一段
    // 查找中间的RTL字符
    int iLastIndex = iLeftIndex;
    while (iLeftIndex <= iMaxIndex && !Lang_IsRTL(strW[iLeftIndex]))
    {
        ++iLeftIndex;
    }

    while (iLeftIndex > iLastIndex && iLeftIndex > 0 && iswdigit(strW[iLeftIndex - 1]))
    {
        --iLeftIndex;
    }

    // RTL字符右侧起点
    while (iRightIndex > iLeftIndex && !Lang_IsRTL(strW[iRightIndex]))
    {
        --iRightIndex;
    }

    // 希伯来语之后的（中间可以有标点和空格）数字，和希伯来语一起处理
    if (iRightIndex < iMaxIndex)
    {
        // http://ts.yealink.com/Bug.php?BugID=38853
        if (IsSkipReverse(strW[iRightIndex + 1]) || iswdigit(strW[iRightIndex + 1]))
        {
            // 多减了一個
            ++iRightIndex;
        }

        int iIndex = iRightIndex;
        while (iIndex <= iMaxIndex && IsSkipReverse(strW[iIndex]))
        {
            ++iIndex;
        }

        int iPosBeforeDigit = iIndex;
        while (iPosBeforeDigit <= iMaxIndex && (iswdigit(strW[iPosBeforeDigit])
                                                || iswpunct(strW[iPosBeforeDigit])))
        {
            ++iPosBeforeDigit;
        }

        // 重新调整右边界
        if (iPosBeforeDigit != iIndex)
        {
            iRightIndex = iPosBeforeDigit;
        }
    }

    bool bTailFmt = iRightIndex < iMaxIndex;
    bool bHeadFmt = iLeftIndex > 0;

    // 字符串切割
    yl::wstring strPrefixW;
    yl::wstring strContentW;
    yl::wstring strPostW;

    if (bHeadFmt)
    {
        strPrefixW = strW.substr(0, iLeftIndex);

        if (bTailFmt)
        {
            strContentW = strW.substr(iLeftIndex, iRightIndex - iLeftIndex);
            strPostW = strW.substr(iRightIndex);
        }
        else
        {
            strContentW = strW.substr(iLeftIndex);
        }
    }
    else
    {
        if (bTailFmt)
        {
            strContentW = strW.substr(0, iRightIndex);
            strPostW = strW.substr(iRightIndex);
        }
        else
        {
            strContentW = strW;
        }
    }

    const int iLength = strContentW.length();
    if (iLength < 2)
    {
        return;
    }

    // 3. 开始两次反转, 非标点字符就位
    // 右侧是否是不需要反转的字符
    int iRightPos = iLength - 1;
    while (iRightPos > 0 && IsSkipReverse(strContentW[iRightPos]))
    {
        --iRightPos;
    }

    // 进行两次反转，实现特定字符的反转 如 “希1 希2 空格 a b”
    ReverseString(&strContentW[0], iRightPos + 1);
    // 第一次全部Reverse之后为“b a 空格 希2 希1”

    int iStart = -1;
    int iIndex = 0;
    while (iIndex <= iRightPos)
    {
        // 再对非希伯来语倒置一次 "a b 空格 希2 希1"
        if (Lang_IsRTL(strContentW[iIndex])
                || IsSkipReverse(strContentW[iIndex]))
        {
            bool bReverse = true;
            LPCWSTR lpTimer = L"-:/.";
            if (yl::find(lpTimer, lpTimer + wcslen(lpTimer), strContentW[iIndex]) != NULL)
            {
                if (iIndex > 0 && iswdigit(strContentW[iIndex - 1])
                        && iIndex < strContentW.length() - 1 && iswdigit(strContentW[iIndex + 1]))
                {
                    bReverse = false;
                }
            }

            if (bReverse && iStart != -1)
            {
                //连续的LTR字符串结束，则按开始位置到结束位置的长度进行倒置
                ReverseString(&strContentW[iStart], iIndex - iStart);
                iStart = -1;
            }
        }
        else
        {
            if (iStart == -1)
            {
                //记录LTR字符串开始的位置
                iStart = iIndex;
            }
            else if (iIndex == iRightPos)
            {
                //如果已到字符串结尾，且最后一个字符是LTR，且前面有连续的LTR字符
                ReverseString(&strContentW[iStart], iRightPos - iStart + 1);
                break;
            }
        }

        ++iIndex;
    }

    // 4. 纠正成对字符
    // 去掉了嵌套的处理，话机上一般不会出现嵌套的情况，简化处理
    iIndex = 0;
    // 查找RTL起始位置
    while (iIndex < iLength && !Lang_IsRTL(strContentW[iIndex]))
    {
        ++iIndex;
    }

    bool bFindRtl = false;
    while (iIndex < iLength)
    {
        WCHAR & wch = strContentW[iIndex];
        ++iIndex;

        if (Lang_IsRTL(wch))
        {
            bFindRtl = true;
            continue;
        }

        // 未考虑多重嵌套的问题
        bool bInPairs = IsOneOfPairsSymbols(wch);
        if (IsSkipReverse(wch) && !bInPairs)
        {
            continue;
        }
        else if (bInPairs && bFindRtl)
        {
            int iNewIndex = iIndex;
            bool bNextRTL = false;

            // 成对字符之后紧接着RTL或数字才需要纠正
            while (iNewIndex < iLength)
            {
                WCHAR wNextCh = strContentW[iNewIndex];
                ++iNewIndex;

                if (Lang_IsRTL(wNextCh)
                        || iswdigit(wNextCh))
                {
                    bNextRTL = true;
                    break;
                }
                else
                {
                    if (IsSkipReverse(wNextCh))
                    {
                        continue;
                    }

                    break;
                }
            }

            if (bNextRTL)
            {
                wch = ReserverPairsSymbols(wch);
            }
        }
        else
        {
            bFindRtl = false;
        }
    }

    // 对显示的优化，这一项不做，以客户提供的翻译为准

    // 5. 重组形成新的格式化字符串
    strW = strPrefixW + strContentW + strPostW;
}

static bool Compare(yl::string & strL, yl::string & strR)
{
    return strcasecmp(strL.c_str(), strR.c_str()) < 0;
}

void SortStringList(YLList<yl::string> & liststring)
{
    Sort(liststring, Compare);
}

LPCSTR PeekContentFromQuotaString(LPCSTR lpszParamText, yl::string & strContent)
{
    assert(lpszParamText[-1] == '"');
    strContent.clear();
    for (; *lpszParamText != '\0'; lpszParamText++)
    {
        if (*lpszParamText == '"')
        {
            return lpszParamText + 1;
        }
        if (*lpszParamText == '\\')
        {
            lpszParamText++;
            switch (*lpszParamText)
            {
            case 'n':
                strContent += '\n';
                break;
            case 'r':
                strContent += '\r';
                break;
            case 't':
                strContent += '\t';
                break;
            case 'v':
                strContent += '\v';
                break;
            case 'f':
                strContent += '\f';
                break;
            default:
                strContent += *lpszParamText;
            }
        }
        else
        {
            strContent += *lpszParamText;
        }
    }

    return lpszParamText;
}

int PeekPhraseByCharTokens(LPCSTR & lpszText, yl::string & strText, LPCSTR lpszToken)
{
    if (textEMPTY(lpszText))
    {
        return -1;
    }

    LPCSTR lpszWordBegin = SkipWhiteSpace(lpszText);
    if (*lpszWordBegin == '\0')
    {
        lpszText = lpszWordBegin;
        return -1;
    }

    LPCSTR lpszWordEnding = lpszWordBegin - 1;
    for (LPCSTR lpszWord = lpszWordBegin;; lpszWord = (*lpszWord == '\0') ? lpszWord : lpszWord + 1)
    {
        LPCSTR lpszTestToken;
        for (lpszTestToken = lpszToken; *lpszTestToken != '\0'; lpszTestToken++)
        {
            if (*lpszWord == *lpszTestToken)
            {
                strText = strText.assign(lpszWordBegin, lpszWordEnding - lpszWordBegin + 1);
                lpszText = *lpszTestToken == '\0' ? lpszWord : lpszWord + 1;
                return lpszTestToken - lpszToken;
            }
        }
        if (*lpszWord == '\0')
        {
            strText = strText.assign(lpszWordBegin, lpszWordEnding - lpszWordBegin + 1);
            lpszText = lpszWord;
            return lpszTestToken - lpszToken;
        }

        // doesn't match now save the last word position
        if (!IsWhiteSpace(*lpszWord))
        {
            lpszWordEnding = lpszWord;
        }
    }

    assert(false);
    return -1;
}

void ReverseString(WCHAR * wchStart, int iLength)
{
    if (wchStart == NULL || iLength == 1)
    {
        return;
    }

    WCHAR wchTmp;
    for (int i = 0; i < iLength / 2; ++i)
    {
        wchTmp = wchStart[i];
        wchStart[i] = wchStart[iLength - 1 - i];
        wchStart[iLength - 1 - i] = wchTmp;
    }
}

int StrToKey(const yl::string & str, const mapItem mapArr[], int iSize)
{
    for (int i = 0; i < iSize; ++i)
    {
        if (str == mapArr[i].strName)
        {
            return mapArr[i].iKey;
        }
    }

    return -1;
}

LPCSTR KeyToStr(int iKey, const mapItem mapArr[], int iSize)
{
    for (int i = 0; i < iSize; ++i)
    {
        if (iKey == mapArr[i].iKey)
        {
            return mapArr[i].strName;
        }
    }

    return NULL;
}

LPCSTR SkipWhiteSpace(LPCSTR lpszText)
{
    while (IsWhiteSpace(*lpszText))
    {
        lpszText++;
    }
    return lpszText;
}

bool IsWhiteSpace(int ch)
{
    return (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t');
}

/***********************************************************华丽的分割线**************************************************************/

#define SUN     TRID_SUNDAY_SHORT
#define MON     TRID_MONDAY_SHORT
#define TUE     TRID_TUESDAY_SHORT
#define WED     TRID_WEDNESDAY_SHORT
#define THU     TRID_TURSDAY_SHORT
#define FRI     TRID_FRIDAY_SHORT
#define SAT     TRID_SATURDAY_SHORT

#define SUNFULL TRID_SUNDAY
#define MONFULL TRID_MONDAY
#define TUEFULL TRID_TUESDAY
#define WEDFULL TRID_WEDNESDAY
#define THUFULL TRID_TURSDAY
#define FRIFULL TRID_FRIDAY
#define SATFULL TRID_SATURDAY

#define JAN     TRID_JANUARY_SHORT
#define FEB     TRID_FEBRUARY_SHORT
#define MAR     TRID_MARCH_SHORT
#define APR     TRID_APRIL_SHORT
#define MAY     TRID_MAY_SHORT
#define JUN     TRID_JUNE_SHORT
#define JUL     TRID_JULY_SHORT
#define AUG     TRID_AUGUST_SHORT
#define SEP     TRID_SEPTEMBER_SHORT
#define OCT     TRID_OCTOBER_SHORT
#define NOV     TRID_NOVEBER_SHORT
#define DEC     TRID_DECEMBER_SHORT

#define JAN_FULL        TRID_JANUARY
#define FEB_FULL        TRID_FEBRUARY
#define MAR_FULL        TRID_MARCH
#define APR_FULL        TRID_APRIL
#define MAY_FULL        TRID_MAY
#define JUN_FULL        TRID_JUNE
#define JUL_FULL        TRID_JULY
#define AUG_FULL        TRID_AUGUST
#define SEP_FULL        TRID_SEPTEMBER
#define OCT_FULL        TRID_OCTOBER
#define NOV_FULL        TRID_NOVEBER
#define DEC_FULL        TRID_DECEMBER

// 将数字转化为星期
LPCSTR GetWeeday(int nDay)
{
    switch (nDay)
    {
    case 0:
        return SUN;
    case 1:
        return MON;
    case 2:
        return TUE;
    case 3:
        return WED;
    case 4:
        return THU;
    case 5:
        return FRI;
    case 6:
        return SAT;
    default:
        return "";
    }
}

// 将数字转化为星期
LPCSTR GetWeedayFull(int nDay)
{
    switch (nDay)
    {
    case 0:
        return SUNFULL;
    case 1:
        return MONFULL;
    case 2:
        return TUEFULL;
    case 3:
        return WEDFULL;
    case 4:
        return THUFULL;
    case 5:
        return FRIFULL;
    case 6:
        return SATFULL;
    default:
        return "";
    }
}

//将整数月份转化为字母缩写
LPCSTR GetMonth(int month)
{
    switch (month)
    {
    case 1:
        return JAN;
    case 2:
        return FEB;
    case 3:
        return MAR;
    case 4:
        return APR;
    case 5:
        return MAY;
    case 6:
        return JUN;
    case 7:
        return JUL;
    case 8:
        return AUG;
    case 9:
        return SEP;
    case 10:
        return OCT;
    case 11:
        return NOV;
    case 12:
        return DEC;
    default:
        return "";
    }
}

//将整数月份转化为字母缩写
LPCSTR GetMonthFull(int month)
{
    switch (month)
    {
    case 1:
        return JAN_FULL;
    case 2:
        return FEB_FULL;
    case 3:
        return MAR_FULL;
    case 4:
        return APR_FULL;
    case 5:
        return MAY_FULL;
    case 6:
        return JUN_FULL;
    case 7:
        return JUL_FULL;
    case 8:
        return AUG_FULL;
    case 9:
        return SEP_FULL;
    case 10:
        return OCT_FULL;
    case 11:
        return NOV_FULL;
    case 12:
        return DEC_FULL;
    default:
        return "";
    }
}
