#include "commonfunc.h"
#include <ylstl/ylstringalgorithm.h>

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include <ylhashmap.h>
#include <stdio.h>
#include "dsskeyui/include/dsskeyuicommondefine.h"

#define  MAX_TEMP_LENGTH        64

typedef YLHashMap<int, yl::string> map_DT_String;

static map_DT_String strMapDT;

/////////////for Dsskey Log Type String////////////////

void AddString(int eDKType, const char * pszString)
{
    if (NULL != pszString)
    {
        yl::string strContent = pszString;
        strMapDT.put(eDKType, strContent);
    }
}

void DsskeyTypeString_Init()
{
    strMapDT.clear();
    AddString(DT_NA, "DT_NA");
    AddString(DT_CONF, "DT_CONF");
    AddString(DT_FWD, "DT_FWD");
    AddString(DT_TRAN, "DT_TRAN");
    AddString(DT_HOLD, "DT_HOLD");
    AddString(DT_DND, "DT_DND");
    AddString(DT_REDIAL, "DT_REDIAL");
    AddString(DT_CALLRETURN, "DT_CALLRETURN");
    AddString(DT_SMS, "DT_SMS");
    AddString(DT_PICKUP, "DT_PICKUP");
    AddString(DT_CALLPARK, "DT_CALLPARK");
    AddString(DT_CUSTOM, "DT_CUSTOM");
    AddString(DT_VOICEMAIL, "DT_VOICEMAIL");
    AddString(DT_SPEEDDIAL, "DT_SPEEDDIAL");
    AddString(DT_INTERCOM, "DT_INTERCOM");
    AddString(DT_LINE, "DT_LINE");
    AddString(DT_BLF, "DT_BLF");
#ifdef IF_FEATURE_GENBAND
    AddString(DT_BLFGROUP, "DT_BLFGROUP");
#endif
    AddString(DT_URL, "DT_URL");
    AddString(DT_GROUPLISTEN, "DT_GROUPLISTEN");
    AddString(DT_PUBLICHOLD, "DT_PUBLICHOLD");
    AddString(DT_PRIVATEHOLD, "DT_PRIVATEHOLD");
    AddString(DT_BLA, "DT_BLA");
    AddString(DT_XMLGROUP, "DT_XMLGROUP");
    AddString(DT_GROUPPICKUP, "DT_GROUPPICKUP");
    AddString(DT_PAGING, "DT_PAGING");
    AddString(DT_RECORD, "DT_RECORD");
    AddString(DT_VOICERECORD, "DT_VOICERECORD");
    AddString(DT_XMLBROWSER, "DT_XMLBROWSER");
    AddString(DT_IDLE_CALLLIST, "DT_IDLE_CALLLIST");
    AddString(DT_IDLE_CONTACTS, "DT_IDLE_CONTACTS");
    AddString(DT_IDLE_MENU, "DT_IDLE_MENU");
    AddString(DT_IDLE_SWITCHACCOUNT, "DT_IDLE_SWITCHACCOUNT");
    AddString(DT_IDLE_NEWSMS, "DT_IDLE_NEWSMS");
    AddString(DT_IDLE_STATUS, "DT_IDLE_STATUS");
    AddString(DT_HOTDESKING, "DT_HOTDESKING");
    AddString(DT_URLRECORD, "DT_URLRECORD");
    AddString(DT_BUTTON, "DT_BUTTON");
    AddString(DT_EXP_SWITCH, "DT_EXP_SWITCH");
    AddString(DT_LDAP, "DT_LDAP");
    AddString(DT_BLFLIST, "DT_BLFLIST");
    AddString(DT_PREFIX, "DT_PREFIX");
    AddString(DT_ZERO, "DT_ZERO");
    AddString(DT_ACD, "DT_ACD");
    AddString(DT_LOCALDIR, "DT_LOCALDIR");
    AddString(DT_BSDIR, "DT_BSDIR");
    AddString(DT_LOCALGROUP, "DT_LOCALGROUP");
    AddString(DT_BSGROUP, "DT_BSGROUP");
    AddString(DT_IDLE_REMOTE, "DT_IDLE_REMOTE");
    AddString(DT_GET_BALANCE, "DT_GET_BALANCE");
    AddString(DT_TIPTEL_BUTTON, "DT_TIPTEL_BUTTON");

    AddString(DT_KEYPAD_LOCK, "DT_KEYPAD_LOCK");
    AddString(DT_IDLE_SWITCHACCOUNT_UP, "DT_IDLE_SWITCHACCOUNT_UP");
    AddString(DT_IDLE_SWITCHACCOUNT_DOWN, "DT_IDLE_SWITCHACCOUNT_DOWN");
    AddString(DT_IDLE_NETWORKCALLLOG, "DT_IDLE_NETWORKCALLLOG");
    AddString(DT_NETWORKCALLLOG_SINGLE, "DT_NETWORKCALLLOG_SINGLE");
    AddString(DT_MEET_ME_CONFERENCE, "DT_MEET_ME_CONFERENCE");

    AddString(DT_BSFT_RETRIEVE_PARK, "DT_BSFT_RETRIEVE_PARK");
    AddString(DT_HOTELING, "DT_HOTELING");
    AddString(DT_ACD_TRACE, "DT_ACD_TRACE");
    AddString(DT_DISPCODE, "DT_DISPCODE");
    AddString(DT_ESCALATE, "DT_ESCALATE");
    AddString(DT_FAVORITES, "DT_FAVORITES");

    AddString(DT_NETWORK_FAVORITE, "DT_NETWORK_FAVORITE");
    AddString(DT_UC_FAVORITE, "DT_UC_FAVORITE");
    AddString(DT_BUDDIES, "DT_BUDDIES");
    AddString(DT_MY_STATUS, "DT_MY_STATUS");
    AddString(DT_PAGING_LIST, "DT_PAGING_LIST");
    AddString(DT_CUSTOM_KEY, "DT_CUSTOM_KEY");
#if IF_BUG_14856
    AddString(DT_MISSED_CALL, "DT_MISSED_CALL");
#endif
#if IF_BUG_PLCMDIR
    AddString(DT_PLCM_PHONEBOOK, "DT_PLCM_PHONEBOOK");
#endif
#if IF_BUG_31595
    AddString(DT_XMLBROWSER_INFO, "DT_XMLBROWSER_INFO");
#endif
    AddString(DT_LAST, "DT_LAST");
}

yl::string GetDsskeyTypeString(int type)
{
    char szTmp[MAX_TEMP_LENGTH] = {0};
    yl::string strTmp = "";
    map_DT_String::iterator it;
    it = strMapDT.find(type);

    if (it != strMapDT.end())
    {
        strTmp = it->second;
    }
    else
    {
        sprintf(szTmp, "Dsskey_Type[%d] ", type);
        // 防止越界
        szTmp[MAX_TEMP_LENGTH - 1] = '\0';
        strTmp = szTmp;
    }
    return strTmp;
}

/*
/////////////////for Hebrew//////////////////////
BOOL IsCharRTL(const WCHAR wchar)
{
    //Hebrew-希伯来语，满足从右向左显示的条件
    return (wchar >= 0x0590 && wchar <= 0x05FF);
    //return (wchar >= L'a' && wchar <= L'z'); //for test
}

BOOL IsStringContainRTLChar(const yl::wstring& strSrcW)
{
    for (int i=0; i<strSrcW.length(); i++)
    {
        if (IsCharRTL(strSrcW[i]))
        {
            return TRUE;
        }
    }

    return FALSE;
}

void ReverseString(WCHAR* wchStart, int iLength)
{
    if (wchStart == NULL || iLength == 1)
    {
        return;
    }

    WCHAR wchTmp;
    for (int i=0; i<iLength/2; i++)
    {
        wchTmp = wchStart[i];
        wchStart[i] = wchStart[iLength-1-i];
        wchStart[iLength-1-i] = wchTmp;
    }
}

#define IS_DIGIT_WCHAR(wChar) (((wChar)>= L'0')&&((wChar)<= L'9'))

// 希伯来语 从右到左倒转
void EXPReverseRTLWord(yl::wstring& strSrcW)
{
    if (!IsStringContainRTLChar(strSrcW))
    {
        return;
    }

    //如果字符串的前几位是表示序号，如 1. 2. ，则序号不翻转
    int i = 0;
    BOOL bHeadDigitChar = FALSE;
    BOOL bFindIt = FALSE;
    while (strSrcW[i] != L'\0')
    {
        if (!bHeadDigitChar)
        {
            if (IS_DIGIT_WCHAR(strSrcW[i]))
            {
                bHeadDigitChar = TRUE;
            }
            else if (strSrcW[i] != L' ')
            {
                // 如果起始字符不是数字也不是空格，则退出
                break;
            }
        }
        else //如果已经有数字开头
        {
            if (!IS_DIGIT_WCHAR(strSrcW[i]) && strSrcW[i] != L'.')
            {
                break;
            }
            else if (strSrcW[i] == L'.')
            {
                bFindIt = TRUE;
                break;
            }
        }

        i++;
    }

    yl::wstring strPrefixW = yl::to_utf16("");
    yl::wstring strContentW = yl::to_utf16("");

    if (bFindIt)
    {
        if (strSrcW[i+1] == L' ')
        {
            i++;
        }

        strPrefixW = strSrcW.substr(0, i+1);
        strContentW = strSrcW.substr(i+1, strSrcW.length()-i-1);
    }
    else
    {
        strPrefixW = (LPCWSTR)"\x00\x00\x00\x00";
        strContentW = strSrcW;
    }

    //////////////////
    int iLength = strContentW.length();
    ReverseString((WCHAR*)strContentW.c_str(), iLength);

    int iStart = -1;
    for (int i=0; i<iLength; i++)
    {
        // 空格不参与倒置如 “希1 希2 空格 a b”
        // 第一次全部Reverse之后为“b a 空格 希2 希1”
        // 再对非希伯来语倒置一次 "a b 空格 希2 希1"(此处的空格不能和a/b一起倒置)
        if (IsCharRTL(strContentW[i]) || strContentW[i] == L' ')
        {
            if (iStart != -1)
            {
                //连续的LTR字符串结束，则按开始位置到结束位置的长度进行倒置
                ReverseString((WCHAR*)(strContentW.c_str()+iStart), i-iStart);
                iStart = -1;
            }
        }
        else
        {
            if (iStart == -1)
            {
                //记录LTR字符串开始的位置
                iStart = i;
            }
            else if (i == iLength - 1)
            {
                //如果已到字符串结尾，且最后一个字符是LTR，且前面有连续的LTR字符
                ReverseString((WCHAR*)(strContentW.c_str()+iStart), iLength-iStart);
                iStart = -1;
            }
        }
    }

    strSrcW = strPrefixW + strContentW;
}*/

/////////////////////////////////////////////
QString TranslateDsskeyLabel(const char * lpszText, const bool bIsExpKey)
{
    if (bIsExpKey)
    {
#ifdef IF_FEATURE_TRANSLATE_EXP
        return toQString(LANG_EXP(lpszText));
#else
        return toQString(lpszText);
#endif
    }
    else
    {
        return LANG_TRANSLATE(lpszText);
    }
}


///////////////////////////////////////
