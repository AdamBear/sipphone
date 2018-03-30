#include "input_include.h"

#define MIN_COUT_FOR 15


/*******************************************************华丽的分割线********************************************************/

/* 阿拉伯语编码范围 */
static const UINT s_imeArabic[] =
{
    0x0600, 0x06FF, 0xFB50, 0xFDFF, 0xFE70, 0xFEFF, 0
};

/* 各语言的特有字符，默认只包含小写字符 */
#ifdef IF_SUPOURT_SPITE_LANGUAGE_FAMILY
/* 来源 http://www.dawaiyu.com/  */

/* 法语特殊字符
 * áäčďéíĺľňóôŕšťúýž */
static const UINT s_imeFranch[] =
{
    0x00E0, 0x00E2, 0x00E7, 0x00E9, 0x00E8,
    0x00EA, 0x00EB, 0x00EE, 0x00EF, 0x00F4,
    0x0153, 0x00F9, 0x00FB, 0x0000
};

/* 德语特殊字符
 * äöü */
static const UINT s_imeGerman[] =
{
    0x00E4, 0x00F6, 0x00FC, 0x0000
};

/* 意大利特殊字符
 * àéèíìóòúù */
static const UINT s_imeItalian[] =
{
    0x00E0, 0x00E9, 0x00E8, 0x00ED, 0x00EC,
    0x00F3, 0x00F2, 0x00FA, 0x00F9, 0x0000
};

/* 西班牙特殊字符
 * ñáçéíóúü */
static const UINT s_imeSpanish[] =
{
    0x00F1, 0x00E1, 0x00E7, 0x00E9, 0x00ED,
    0x00F3, 0x00FA, 0x00FC, 0x0000
};

/* 葡萄牙特殊字符
 * àáã âúü çô */
static const UINT s_imePortuguese[] =
{
    0x00E0, 0x00E1, 0x00E3, 0x00A0, 0x00E2,
    0x00FA, 0x00FC, 0x00A0, 0x00E7, 0x00F4, 0x0000
};

/* 波兰特殊字符
 * ąćęłńóśźż */
static const UINT s_imePolish[] =
{
    0x0105, 0x0107, 0x0119, 0x0142, 0x0144,
    0x00F3, 0x015B, 0x017A, 0x017C, 0x0000
};

/* 土耳其特殊字符
 * çğıiöşüə */
static const UINT s_imeTurkish[] =
{
    0x00E7, 0x011F, 0x0131, 0x00F6, 0x015F,
    0x00FC, 0x0259, 0x0000
};

/* 捷克语特殊字符
 * áčďéěíňóřšťúůýž */
static const UINT s_imeCzech[] =
{
    0x00E1, 0x010D, 0x010F, 0x00E9, 0x011B,
    0x00ED, 0x0148, 0x00F3, 0x0159, 0x0161,
    0x0165, 0x00FA, 0x016F, 0x00FD, 0x017E, 0x0000
};

/* 瑞典特殊字符
 * åæø */
static const UINT s_imeSwedish[] =
{
    0x00E5, 0x00E4, 0x00F6, 0x0000
};

/* 丹麦、挪威特殊字符
 * åæø */
static const UINT s_imeDanish[] =
{
    0x00E5, 0x00E6, 0x00F8, 0x0000
};

/* 斯洛伐克语
 * áäčďéíĺľňóôŕšťúýž */
static const UINT s_imeSlovak[] =
{
    0x00E1, 0x00E4, 0x010D, 0x010F, 0x00E9,
    0x00ED, 0x013A, 0x013E, 0x0148, 0x00F3,
    0x00F4, 0x0155, 0x0161, 0x0165, 0x00FA,
    0x00FD, 0x017E, 0x0000
};

/* 匈牙利
 * áéíóöőúüű */
static const UINT s_imeHungarian[] =
{
    0x00E1, 0x00E9, 0x00ED, 0x00F3, 0x00F6,
    0x0151, 0x00FA, 0x00FC, 0x0171, 0x0000
};

/* 罗马尼亚
 * ăâîşţ */
static const UINT s_imeRomanian[] =
{
    0x0103, 0x00E2, 0x00EE, 0x015F, 0x0163, 0x0000
};

/* 克罗地亚
 * čćđšž */
static const UINT s_imeCroatian[] =
{
    0x010D, 0x0107, 0x0111, 0x0161, 0x017E, 0x0000
};

#endif

/*******************************************************忧郁的分割线********************************************************/

/* 语言属性 */
const CLangAttr s_imeAttr[] =
{
    { "ascii",      0,          IA_SECTION | IA_CASE | IA_DATE_BASIC | IA_IME, { 0X0000, 0X0080 }               },              /* ascii 英语 */
#ifdef IF_SUPOURT_SPITE_LANGUAGE_FAMILY
    { "franch",     "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeFranch     ,0}},         /* 法语 */
    { "german",     "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeGerman     ,0}},         /* 德语 */
    { "italian",    "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeItalian    ,0}},         /* 意大利 */
    { "spanish",    "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeSpanish    ,0}},         /* 西班牙 */
    { "portuguese", "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imePortuguese ,0}},         /* 西班牙 */
    { "polish",     "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imePolish     ,0}},         /* 波兰 */
    { "turkish",    "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeTurkish    ,0}},         /* 土耳其 */
    { "czech",      "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeCzech      ,0}},         /* 捷克 */
    { "swedish",    "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeSwedish    ,0}},         /* 瑞典 */
    { "danish",     "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeDanish     ,0}},         /* 丹麦 */
    { "slovak",     "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeSlovak     ,0}},         /* 斯洛伐克 */
    { "hungarian",  "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeHungarian  ,0}},         /* 斯洛伐克 */
    { "romanian",   "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeRomanian   ,0}},         /* 罗马尼亚 */
    { "croatian",   "latin",    IA_FEATURE_CODE | IA_CASE | IA_DATE_BASIC | IA_IME, { (const UINT)s_imeCroatian   ,0}},         /* 克罗地亚 */
#endif
    { "latin",      0,          IA_SECTION | IA_CASE | IA_DATE_BASIC | IA_IME, { 0X0081, 0X00FF }               },              /* 拉丁语系 法德意...,待细分 */
    { "latin",      0,          IA_SECTION | IA_CASE | IA_DATE_BASIC | IA_IME, { 0X0100, 0X023F } },              /* 拉丁语系 土 */
    { "cyrillic",   0,          IA_SECTION | IA_CASE | IA_DATE_BASIC | IA_IME, { 0X0400, 0X052F } },              /* 斯拉夫 俄语 */
    { "hebrew",     0,          IA_SECTION | IA_DATE_BASIC | IA_RTL  | IA_IME, { 0X05D0, 0X05F4 } },              /* 希伯来 */
    { "arabic",     0,          IA_ULLTI_SECTION | IA_DATE_BASIC | IA_RTL | IA_LIGATURES, { (const UINT)s_imeArabic, 0 }      },  /* 阿拉伯语 */
    { "syriac",     0,          IA_SECTION | IA_DATE_BASIC | IA_RTL  | IA_LIGATURES, { 0X0700, 0X074F } },        /* 叙利亚 */
    { "bengali",    0,          IA_SECTION | IA_DATE_CHINESE, { 0X0980, 0X09FF } },                               /* 缅甸文 */
    { "thai",       0,          IA_SECTION | IA_DATE_CHINESE, { 0X0E00, 0X0E7F } },                               /* 泰语 */
    { "korean",     0,          IA_SECTION | IA_DATE_CHINESE | IA_IME_BUILDIN, { 0XAC00, 0XD7AF } },              /* 谚文 韩语 */
    { "mongolian",  0,          IA_SECTION | IA_DATE_CHINESE, { 0X1800, 0X18AF } },                               /* 蒙古文 */
    { "latin",      0,          IA_SECTION | IA_CASE | IA_DATE_BASIC, { 0X1E00, 0X1EFF } },                       /* 拉丁语系 土耳其 */
    { "katakana",   0,          IA_SECTION | IA_DATE_CHINESE, { 0X3040, 0X30FF } },                               /* 片假 日语 */
    { "chinese",    0,          IA_SECTION | IA_DATE_CHINESE, { 0X4E00, 0X9FFF } },                               /* cjk  简体、繁体 */
};

/*******************************************************哀怨的分割线********************************************************/

#define ATTR_TAB_SIZE (sizeof(s_imeAttr)/sizeof(s_imeAttr[0]))

void CLangAttrManager::CalRefCount(const yl::string & strCode)
{
    yl::wstring strValueW = to_utf16(strCode);
    int istrLen = strValueW.length();
    /* 低于3个字符不计入统计 */
    if (istrLen < 3)
    {
        return;
    }

    /* 0、1、3、6、10、15、21、28 ... */
    int iInr = 0;
    for (int j = 0; j < istrLen; ++iInr, j += iInr)
    {
        for (int i = 0; i < ATTR_TAB_SIZE; ++i)
        {
            if (IsInRang(strValueW[j], i))
            {
                ++(m_pRefCount[i]);
            }
        }
    }
}

CLangAttrManager::CLangAttrManager() : m_pRefCount(0), m_iFocus(0)
{
    m_pRefCount = new UINT[ATTR_TAB_SIZE];
    memset(m_pRefCount, 0, ATTR_TAB_SIZE * sizeof(UINT));
}

CLangAttrManager::~CLangAttrManager()
{
    delete [] m_pRefCount;
}

void CLangAttrManager::Reload()
{
    m_iFocus = 0;
    memset(m_pRefCount, 0, ATTR_TAB_SIZE * sizeof(UINT));
}

bool CLangAttrManager::CreatDefaultLocation()
{
    /* 恢复默认 */
    m_iFocus = 0;

    struct CRefCount
    {
        int  iIndex;
        UINT uRefCount;
    };

    /* 找出排在前三个的编码,序号和引用数 */
    const int iFindSize = 3;
    CRefCount uArrCount[iFindSize];
    memset(uArrCount, 0, sizeof(CRefCount) * iFindSize);

    for (int i = 0; i < ATTR_TAB_SIZE; ++i)
    {
        for (int j = 0; j < iFindSize; ++j)
        {
            if (m_pRefCount[i] > uArrCount[j].uRefCount)
            {
                /* 后移元素 */
                for (int k = iFindSize - 1; k > j; --k)
                {
                    uArrCount[k] = uArrCount[k - 1];
                }

                uArrCount[j].uRefCount = m_pRefCount[i];
                uArrCount[j].iIndex = i;

                /* 直接跳出里层循环 */
                break;
            }
        }
    }

#ifdef IF_TEST_MODE
    for (int i = 0; i < ATTR_TAB_SIZE; ++i)
    {
        APP_INFO("CreatDefaultLocation index=[%d] name=[%s] count=[%d]", i, s_imeAttr[i].strName,
                 m_pRefCount[i]);
    }
    for (int i = 0; i < iFindSize; ++i)
    {
        APP_INFO("CreatDefaultLocation for=[%d] index [%d]=[%d] name=[%s]",
                 i, uArrCount[i].iIndex, uArrCount[i].uRefCount, s_imeAttr[uArrCount[i].iIndex].strName);
    }
#endif

    static LPCSTR const Ime_Section_Latin = "latin";
    static LPCSTR const Ime_Section_Ascii = "ascii";

    /* 有编码依赖的  例如：Ascii > Latin > Franch  （法语本身属于Latin字符集，同时包含大量Ascii） */
    LPCSTR CImeLocation[][2] =
    {
        { Ime_Section_Ascii, Ime_Section_Latin }
    };

    for (int i = 0; i < sizeof(CImeLocation) / sizeof(CImeLocation[0]); ++i)
    {
        /* 第一多 */
        if (uArrCount[0].uRefCount > MIN_COUT_FOR
                && strcmp(s_imeAttr[uArrCount[0].iIndex].strName, CImeLocation[i][0]) != 0
                && strcmp(s_imeAttr[uArrCount[0].iIndex].strName, CImeLocation[i][1]) != 0)
        {
            m_iFocus = uArrCount[0].iIndex;
        }
        else if (uArrCount[1].uRefCount > MIN_COUT_FOR
                 && strcmp(s_imeAttr[uArrCount[1].iIndex].strName, CImeLocation[i][0]) != 0
                 && strcmp(s_imeAttr[uArrCount[1].iIndex].strName, CImeLocation[i][1]) != 0)
        {
            m_iFocus = uArrCount[1].iIndex;
        }
        else if (uArrCount[2].uRefCount > MIN_COUT_FOR)
        {
            m_iFocus = uArrCount[2].iIndex;
        }
        else
        {
            /* 取第二多的 */
            if (uArrCount[1].uRefCount > MIN_COUT_FOR
                    && strcmp(s_imeAttr[uArrCount[0].iIndex].strName, CImeLocation[i][1]) == 0)
            {
                m_iFocus = uArrCount[1].iIndex;
            }
            else if (uArrCount[0].uRefCount > MIN_COUT_FOR)
            {
                m_iFocus = uArrCount[0].iIndex;
            }
        }

        if (m_iFocus > 0)
        {
            break;
        }
    }

    APP_INFO("CreatDefaultLocation current index=[%d] name=[%s]", m_iFocus,
             s_imeAttr[m_iFocus].strName);
    return true;
}

/*
bool CLangAttrManager::IsRtl(const yl::wstring& strW)
{
    for (int j = 0; j < ATTR_TAB_SIZE; ++j)
    {
        if (s_imeAttr[j].iAttr & IA_RTL)
        {
            for(int i = 0; i < strW.length(); ++i)
            {
                if (IsInRang((WCHAR)strW[i], j))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool CLangAttrManager::IsRtl(WCHAR wch)
{
    for (int j = 0; j < ATTR_TAB_SIZE; ++j)
    {
        if (s_imeAttr[j].iAttr & IA_RTL)
        {
            if (IsInRang(wch, j))
            {
                return true;
            }
        }
    }

    return false;
}

bool CLangAttrManager::IsLigatures(const yl::wstring& strW)
{
    if (strW.length() < 2)
    {
        return false;
    }

    for (int j = 0; j < ATTR_TAB_SIZE; ++j)
    {
        if (s_imeAttr[j].iAttr & IA_LIGATURES)
        {
            for(int i = 0; i < strW.length(); ++i)
            {
                if (IsInRang(strW[i], j))
                {
                    return true;
                }
            }
        }
    }

    return false;
}*/

bool CLangAttrManager::IsInRang(UINT iValue, int iIndex/* = -1*/)
{
    if (iIndex < 0)
    {
        iIndex = m_iFocus;
    }

    if (s_imeAttr[iIndex].iAttr & IA_SECTION)
    {
        return iValue >= s_imeAttr[iIndex].Rang[0] && iValue <= s_imeAttr[iIndex].Rang[1];
    }
#ifdef IF_SUPOURT_SPITE_LANGUAGE_FAMILY
    else if (s_imeAttr[iIndex].iAttr & IA_FEATURE_CODE)
    {
        const UINT * pRang = (const UINT *)s_imeAttr[iIndex].Rang[0];
        for (int i = 0; pRang[i] != 0; ++i)
        {
            if (iValue == pRang[i])
            {
                return true;
            }
        }
    }
#endif
    else if (s_imeAttr[iIndex].iAttr & IA_ULLTI_SECTION)
    {
        UINT iFrom = 0;
        UINT iTo = 0;

        const UINT * pRang = (const UINT *)s_imeAttr[iIndex].Rang[0];
        for (int i = 0; pRang[i] != 0; ++i)
        {
            if (iFrom == 0 && i % 2 == 0)
            {
                iFrom = pRang[i];
            }
            else if (iTo == 0 && i % 2 == 1)
            {
                iTo = pRang[i];
            }

            if (iFrom != 0 && iTo != 0)
            {
                if (iValue >= iFrom && iValue <= iTo)
                {
                    return true;
                }

                iFrom = iTo = 0;
            }
        }
    }

    return false;
}



