#include "../ETLLib/ETLLib.hpp"
#include "automatakr.h"
#include "koreanime.h"
#include "interfacedefine.h"
#include "config.h"
#include "features-define.h"
#include <commonapi/stringhelperapi.h>


#ifdef IF_FEATURE_KOREANIME

yl::wstring g_strIngWord = L"";
yl::wstring g_strCompleteWord = L"";

// 全局静态的韩文输入法对象
static CAutomataKR s_objKoreanIme;

// 全局静态的韩文输入按键缓存
static yl::string s_csKeyBuffer;

// 全局静态指示当前是否处于输入特殊转义字符状态
static bool s_bDiyDot;

static char KOREANCODE[40][10] = \
{
    "4", "444", "5", "6", "666", "7", \
    "8", "9", "999", "*", "**", \
    "0", "#", "###", "##", "44", \
    "66", "99", "00", "12", "121", \
    "122", "1221", "21", "211", "221", \
    "2211", "23", "2312", "23121", "231", \
    "223", "32", "3221", "32211", "321", \
    "322", "3", "31", "1"   \
};

int GetKoreanCode(LPCSTR szKeyBuffer)
{
    for (int i = 0; i < 40; i++)
    {
        if (strcmp(KOREANCODE[i], szKeyBuffer) == 0)
        {
            return i;
        }
    }

    return -1;
}


void korean_Start()
{
    s_objKoreanIme.Clear();
    s_csKeyBuffer.clear();
    g_strIngWord.clear();
    g_strCompleteWord.clear();
    s_bDiyDot = false;
}

void korean_KeyIn(char chKey)
{
    if (chKey < 0)
    {
        s_objKoreanIme.SetKeyCode(chKey);
        return;
    }

    yl::string strKeyBuffer = commonAPI_FormatString("%s%c", s_csKeyBuffer.c_str(), chKey);

    //查表获取对应字符
    int iCodeIndex = GetKoreanCode(strKeyBuffer.c_str());

    if (iCodeIndex >= 0)
    {
        s_csKeyBuffer = strKeyBuffer;

        if (s_bDiyDot)
        {
            s_bDiyDot = false;
        }
        else
        {
            if (s_objKoreanIme.ingWord != 0)
            {
                s_objKoreanIme.SetKeyCode(KEY_CODE_BACKSPACE);
            }
        }

        s_objKoreanIme.SetKeyCode(iCodeIndex);
    }
    else
    {
        if (!s_bDiyDot)
        {
            s_csKeyBuffer.clear();
        }
        else
        {
            if (chKey > '3')
            {
                s_csKeyBuffer.clear();
                s_bDiyDot = false;
            }
        }

        s_csKeyBuffer = commonAPI_FormatString("%s%c", s_csKeyBuffer.c_str(), chKey);

        if (chKey == '2')
        {
            s_bDiyDot = true;

            int iLen = s_csKeyBuffer.size();

            s_csKeyBuffer = s_csKeyBuffer.substr(0, 2 - (iLen % 2));
        }
        else
        {
            int iCode = GetKoreanCode(s_csKeyBuffer.c_str());

            if (iCode >= 0)
            {
                s_objKoreanIme.SetKeyCode(iCode);
            }
        }
    }

    g_strIngWord.clear();
    if (s_objKoreanIme.ingWord != 0)
    {
        g_strIngWord += s_objKoreanIme.ingWord;
    }

    if (s_bDiyDot)
    {
        if (s_csKeyBuffer.size() == 1)
        {
            g_strIngWord += '.';
        }
        else if (s_csKeyBuffer.size() == 2)
        {
            g_strIngWord += ':';
        }
    }

    g_strCompleteWord.clear();
    g_strCompleteWord = s_objKoreanIme.completeText;
    if (s_objKoreanIme.completeText.size() > 0)
    {
        s_objKoreanIme.completeText.clear();
    }
}
#else // #ifdef IF_FEATURE_KOREANIME
void korean_Start() { }
void korean_KeyIn(char chKey) { }
#endif // #ifdef IF_FEATURE_KOREANIME
