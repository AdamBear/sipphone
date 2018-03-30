#include "modInput.h"
#include "input_include.h"
#include "translate/include/translate.h"
#include "translate/include/shapearabic.h"
#include "imelogic.h"


// 通过区间标识, 适用于 ABCD...XYZabcd...xyz 序列，大小写不紧挨着，但区间相对
struct CodeRangItem
{
    UINT iLowerFrom;
    UINT iLowerTo;
    UINT iUpperFrom;
    UINT iUpperTo;
};

// 通过个数标识，适用于 AABbCcDd...XxYyZz 序列，大小写紧挨着
struct CodeRangItemCount
{
    UINT    iFrom;
    UINT    iTo;
    short   iLowerSign;
    short   iUpper2Lower;
};

// unicode 字符属性参考如下
// http://www.unicode.org/charts/

static const CodeRangItem CodeRangArr[] =
{
//  { 0x0061, 0x007A, 0x0041, 0x005A}, // ASII
    { 0x00E0, 0x00F6, 0x00C0, 0x00D6}, // Latin-1 Supplement
    { 0x00F8, 0x00FE, 0x00D8, 0x00DE}, // Latin-1 Supplement
    { 0x0450, 0x045F, 0x0400, 0x040F}, // Cyrillic
    { 0x0430, 0x044F, 0x0410, 0x042F}, // Cyrillic
};

static const CodeRangItemCount CodeCountArr[] =
{
    { 0x0100, 0x0177, 1,  1}, // Latin Extended-A
    { 0x0179, 0x017E, 1,  1}, // Latin Extended-A
    { 0x01CD, 0x01DC, 1,  1}, // Latin Extended-B
    { 0x01DE, 0x01EF, 1,  1}, // Latin Extended-B
    { 0x01F8, 0x0233, 1,  1}, // Latin Extended-B
    { 0xA732, 0xA76F, 1,  1}, // Latin Extended-D
    { 0x1E00, 0x1E95, 0,  1}, // Latin Extended Additional
    { 0x1EA0, 0x1EFF, 1,  1}, // Latin Extended Additional
    { 0x0460, 0x0481, 1,  1}, // Cyrillic
    { 0x048A, 0x04BF, 1,  1}, // Cyrillic
    { 0x04C1, 0x04CE, 0,  1}, // Cyrillic
    { 0x04D0, 0x04FF, 1,  1}, // Cyrillic
};

UINT Ime_Lower2Upper(UINT uCode)
{
    for (int i = 0; i < sizeof(CodeRangArr) / sizeof(CodeRangArr[0]); ++i)
    {
        if (uCode >= CodeRangArr[i].iLowerFrom && uCode <= CodeRangArr[i].iLowerTo)
        {
            return CodeRangArr[i].iUpperFrom + uCode - CodeRangArr[i].iLowerFrom;
        }
    }

    for (int i = 0; i < sizeof(CodeCountArr) / sizeof(CodeCountArr[0]); ++i)
    {
        if (uCode >= CodeCountArr[i].iFrom && uCode <= CodeCountArr[i].iTo)
        {
            if (uCode % 2 == CodeCountArr[i].iLowerSign)
            {
                return uCode - CodeCountArr[i].iUpper2Lower;
            }
        }
    }

    // 该函数只实现了ASCII码的转换
    return towupper(uCode);
}

UINT Ime_Upper2Lower(UINT uCode)
{
    for (int i = 0; i < sizeof(CodeRangArr) / sizeof(CodeRangArr[0]); ++i)
    {
        const CodeRangItem & item = CodeRangArr[i];
        if (uCode >= item.iUpperFrom && uCode <= item.iUpperTo)
        {
            return item.iLowerFrom + uCode - item.iUpperFrom;
        }
    }

    for (int i = 0; i < sizeof(CodeCountArr) / sizeof(CodeCountArr[0]); ++i)
    {
        if (uCode >= CodeCountArr[i].iFrom && uCode <= CodeCountArr[i].iTo)
        {
            if (uCode %  2 != CodeCountArr[i].iLowerSign)
            {
                return uCode + CodeCountArr[i].iUpper2Lower;
            }
        }
    }

    // 该函数只实现了ASCII码的转换
    return towlower(uCode);
}

static bool inline IsLigatures(const yl::wstring & strValue)
{
    if (strValue.length() < 2)
    {
        return false;
    }

    for (int i = 0; i < strValue.size(); ++i)
    {
        if ((strValue[i] >= 0x0600 && strValue[i] <= 0x6FF)
                || (strValue[i] >= 0xFB50 && strValue[i] <= 0xFDFF)
                || (strValue[i] >= 0xFE70 && strValue[i] <= 0xFEFF))
        {
            return true;
        }
    }

    return false;
}

static bool inline IsRTL(WCHAR wch)
{
    return (wch >= 0x0590 && wch <= 0x05FF) || (wch >= 0x0600 && wch <= 0x06FF)
           || (wch >= 0xFB50 && wch <= 0xFDFF) || (wch >= 0xFE70 && wch <= 0xFEFF);
}

bool Lang_IsChineseStyleDate()
{
    return _LangAttrManager.GetLangAttr() & IA_DATE_CHINESE;
}

void Ime_Lang_Init()
{
    _ImeLogic;
    _LangAttrManager;
    _TransLogic;

    Lang_ReLoadLang();
}

yl::wstring Lang_Format(const yl::string & strValue)
{
    yl::wstring strTextW = yl::to_utf16(strValue);
    Lang_Format(strTextW);
    return strTextW;
}

yl::wstring Lang_Format(LPCWSTR lpwszText)
{
    yl::wstring strTextW = lpwszText;
    Lang_Format(strTextW);
    return strTextW;
}

void Lang_Format(yl::wstring & strTextW)
{
    if (IsLigatures(strTextW))
    {
        shapedStringImp(strTextW, false);
    }

    YLMiniBidiAlgorithm(strTextW);
}

void Lang_ReLoadLang()
{
    // 语言属性重新加载
    _LangAttrManager.Reload();
    // 翻译重新加载
    _TransLogic.Reload();
    // 生成新的输入法
    _ImeLogic.Reload();
}

bool Lang_IsRTL(const yl::string & strValue)
{
    yl::wstring strW = to_utf16(strValue);
    return Lang_IsRTL(strW);
}

bool Lang_IsRTL(const yl::wstring & strW)
{
    if (strW.length() <= 1)
    {
        return false;
    }

    for (int i = 0; i < strW.size(); ++i)
    {
        if (IsRTL(strW[i]))
        {
            return true;
        }
    }

    return false;
}

bool Lang_IsRTL(WCHAR wch)
{
    return IsRTL(wch);
}

#ifdef IF_FEATURE_KOREANIME
bool Ime_IsKorean(int iIndex)
{
    if (_ImeLogic.GetCustomImeCount() == 0)
    {
        return false;
    }

    // 非内置语言一定不是
    if (iIndex < 0 || !(_LangAttrManager.GetLangAttr() & IA_IME_BUILDIN))
    {
        return false;
    }

    // 随机检测一个编码
    if (_LangAttrManager.IsInRang(0XD000))
    {
        LPCTSTR lpName = _ImeLogic.Ime2Name(IT_STD_Custom, (UINT)iIndex);
        // 对于内置的输入法，也可能加入输入法系统中
        return strcasecmp(_LangAttrManager.GetName(), lpName) == 0;
    }

    return false;
}
#endif


bool Lang_IsJapanese()
{
    return strcmp(_LangAttrManager.GetName(), "katakana") == 0;
}

int Ime_GetCustomImeCount()
{
    return _ImeLogic.GetCustomImeCount();
}

