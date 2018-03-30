#ifndef __chSTRING_H__
#define __chSTRING_H__
#include "ylstl/ylstring.h"
#include <ylstl/ylvector.h>
#include <ylstl/ylstringalgorithm.h>

#ifndef __ETL_H__
#error _chString.h only can be include in ETL.h
#endif
namespace ETL_StringHelper
{
extern const char szNULL[4];

#define LPCTYPE         const TYPE*
#define LPTYPE          TYPE*

EXTERN_DECLARE(BOOL chIsDigit(int ch));
EXTERN_DECLARE(BOOL chIsWhiteSpace(int ch));
EXTERN_DECLARE(BOOL chIsAlpha(int ch));
EXTERN_DECLARE(int chUpper(int ch));
EXTERN_DECLARE(int chLower(int ch));
EXTERN_DECLARE(int chReverse(int ch));
EXTERN_DECLARE(LPCSTR chUTF8Inc(LPCSTR szText));
EXTERN_DECLARE(LPCSTR chUTF8Dec(LPCSTR szHead, LPCSTR szThis));
EXTERN_DECLARE(WCHAR chUTF8Word(LPCSTR & lpszTextA));

EXTERN_DECLARE_CPP(BOOL chIsLeadbyte(LPCSTR szThis));
EXTERN_DECLARE_CPP(BOOL chIsLeadbyte(LPCWSTR szThis));

EXTERN_DECLARE_CPP(BOOL chIsTrailbyte(LPCSTR szHead, LPCSTR szThis));
EXTERN_DECLARE_CPP(BOOL chIsTrailbyte(LPCWSTR szHead, LPCWSTR szThis));


template<class TYPE>
inline int chWord(LPCTYPE szText)
{
    chASSERT(szText != NULL);
    if (chIsLeadbyte(szText))
    {
        return chMAKEWORD(szText[0], szText[1]);
    }
    return szText[0];
}

template<class TYPE>
inline LPCTYPE chStrinc(LPCTYPE szThis)
{
    chASSERT(szThis != NULL);
    if (*szThis == '\0')
    {
        return szThis;
    }
    if (chIsLeadbyte(szThis))
    {
        return szThis + 2;
    }
    return szThis + 1;
}

template<class TYPE>
inline LPCTYPE chStrdec(LPCTYPE szHead, LPCTYPE szThis)
{
    chASSERT(szThis != NULL && szHead != NULL && szThis >= szHead);
    if (szThis <= szHead)
    {
        return NULL;
    }
    if (chIsTrailbyte(szHead, szThis - 1))
    {
        return chMAX(szHead, szThis - 2);
    }
    return szThis - 1;
}

template<class TYPE>
inline int chStrlen(LPCTYPE lpszText)
{
    chASSERT(lpszText != NULL);
    return sizeof(TYPE) == 1 ? (int)strlen((LPCSTR)lpszText) : (int)wcslen((LPCWSTR)lpszText);
}

template<class TYPE>
inline LPTYPE chStrcpy(LPTYPE lpszDes, LPCTYPE lpszSrc)
{
    chASSERT(lpszSrc != NULL && lpszDes != NULL);
    LPTYPE lpResult = lpszDes;
    if (lpszDes != lpszSrc)
    {
        lpResult = sizeof(TYPE) == 1 ? (LPTYPE)strcpy((LPSTR)lpszDes,
                   (LPCSTR)lpszSrc) : (LPTYPE)wcscpy((LPWSTR)lpszDes, (LPCWSTR)lpszSrc);
    }
    return lpResult;
}

template<class TYPE>
inline LPTYPE chStrncpy(LPTYPE lpszDes, LPCTYPE lpszSrc, int nCount)
{
    chASSERT(lpszSrc != NULL && lpszDes != NULL && nCount >= 0);
    LPTYPE lpResult = lpszDes;
    if (lpszDes != lpszSrc && nCount > 0)
    {
        lpResult = sizeof(TYPE) == 1 ? (LPTYPE)strncpy((LPSTR)lpszDes, (LPCSTR)lpszSrc,
                   nCount) : (LPTYPE)wcsncpy((LPWSTR)lpszDes, (LPCWSTR)lpszSrc, nCount);
        lpszDes[nCount] = '\0';
    }
    return lpResult;
}

template<class TYPE>
inline LPTYPE chStrcat(LPTYPE lpszDes, LPCTYPE lpszSrc)
{
    chASSERT(lpszSrc != NULL && lpszDes != NULL);
    return sizeof(TYPE) == 1 ? (LPTYPE)strcat((LPSTR)lpszDes,
            (LPCSTR)lpszSrc) : (LPTYPE)wcscat((LPWSTR)lpszDes, (LPCWSTR)lpszSrc);
}

template<class TYPE>
inline LPCTYPE chSkipWhiteSpace(LPCTYPE lpszText)
{
    while (chIsWhiteSpace(*lpszText))
    {
        lpszText ++;
    }
    return lpszText;
}

template<class TYPE>
inline LPCTYPE chResverSkipWhiteSpace(LPCTYPE lpszBegin, LPCTYPE lpszEnding)
{
    chASSERT(lpszBegin != NULL && lpszEnding >= lpszBegin);
    for (LPCTYPE lpszText = lpszEnding; lpszText > lpszBegin; lpszText --)
    {
        if (!chIsWhiteSpace(*(lpszText - 1)))
        {
            return lpszText;
        }
    }
    return lpszBegin;
}

template<class TYPE, bool bMatchCase>
class uCTextHelper
{
public:
    static int chStrcmp(LPCTYPE s1, LPCTYPE s2)
    {
        chASSERT(s1 != NULL && s2 != NULL);
        if (s1 == s2)
        {
            return 0;
        }
        if (bMatchCase)
        {
            return sizeof(TYPE) == 1 ? strcmp((LPCSTR)s1, (LPCSTR)s2) : wcscmp((LPCWSTR)s1, (LPCWSTR)s2);
        }
        else
        {
            return sizeof(TYPE) == 1 ? strcasecmp((LPCSTR)s1, (LPCSTR)s2) : wcscasecmp((LPCWSTR)s1,
                    (LPCWSTR)s2);
        }
    }

    static int chStrncmp(LPCTYPE s1, LPCTYPE s2, int nCount)
    {
        chASSERT(s1 != NULL && s2 != NULL && nCount >= 0);
        if (s1 == s2 || nCount == 0)
        {
            return 0;
        }
        if (bMatchCase)
        {
            return sizeof(TYPE) == 1 ? strncmp((LPCSTR)s1, (LPCSTR)s2, nCount) : wcsncmp((LPCWSTR)s1,
                    (LPCWSTR)s2, nCount);
        }
        else
        {
            return sizeof(TYPE) == 1 ? strncasecmp((LPCSTR)s1, (LPCSTR)s2, nCount) : wcsncasecmp((LPCWSTR)s1,
                    (LPCWSTR)s2, nCount);
        }
    }

    // try "0123" at "0123456789" get TRUE
    // try "1234" at "0123456789" get FALSE
    // try "" at "0123456789" get FALSE
    // try "" at "" get TRUE
    static BOOL chIsBeginText(LPCTYPE lpszText, LPCTYPE lpszSubText)
    {
        chASSERT(lpszText != NULL && !textEMPTY(lpszSubText));
        if (lpszText == lpszSubText)
        {
            return TRUE;
        }
        if (*lpszSubText == '\0')
        {
            return *lpszText == '\0';
        }
        int nLen = chStrlen(lpszSubText);
        return chStrncmp(lpszText, lpszSubText, nLen) == 0;
    }

    // find '5' in "0123456789" get "56789"
    // find '5' in "01234_6789" get NULL
    // find '\0' in "01234_6789" get ""
    static LPCTYPE chFind(LPCTYPE lpszText, int ch)
    {
        chASSERT(lpszText != NULL && ch != '\0');
        int ch2 = bMatchCase ? ch : chUpper(ch);
        while (TRUE)
        {
            if (bMatchCase ? ch2 == chWord(lpszText) : ch2 == chUpper(chWord(lpszText)))
            {
                return lpszText;
            }
            if (*lpszText == '\0')
            {
                return NULL;
            }
            lpszText = chStrinc(lpszText);
        }
        return NULL;
    }

    // find "56" in "0123456789" get "56789"
    // find "55" in "0123456789" get NULL
    static LPCTYPE chFind(LPCTYPE lpszText, LPCTYPE lpszSubText)
    {
        chASSERT(lpszText != NULL && !textEMPTY(lpszSubText));
        if (lpszText == lpszSubText)
        {
            return lpszText;
        }

        int nLen = chStrlen(lpszSubText);
        while (*lpszText != '\0')
        {
            if (chStrncmp(lpszText, lpszSubText, nLen) == 0)
            {
                return lpszText;
            }
            lpszText = chStrinc(lpszText);
        }
        return NULL;
    }

    // find "56" in "0123456789" get "56789"
    // find "55" in "01234_6789" get NULL
    static LPCTYPE chFindOneOf(LPCTYPE lpszText, LPCTYPE lpszTarget)
    {
        chASSERT(lpszText != NULL && !textEMPTY(lpszTarget));
        if (lpszText == lpszTarget)
        {
            return lpszText;
        }

        while (*lpszText != '\0')
        {
            if (chFind(lpszTarget, chWord(lpszText)) != NULL)
            {
                return lpszText;
            }
            lpszText = chStrinc(lpszText);
        }
        return NULL;
    }

    // find "56" in "0123456789" get "6789"
    // find "55" in "01234_6789" get NULL
    static LPCTYPE chResverFindOneOf(LPCTYPE lpszBegin, LPCTYPE lpszEnding, LPCTYPE lpszTarget)
    {
        chASSERT(lpszBegin != NULL && lpszEnding > lpszBegin && !textEMPTY(lpszTarget)
                 && lpszBegin != lpszTarget);

        while (TRUE)
        {
            LPCTYPE lpszText = chStrdec(lpszBegin, lpszEnding);
            if (lpszText == NULL)
            {
                return NULL;
            }

            if (chFind(lpszTarget, chWord(lpszText)) != NULL)
            {
                return lpszText;
            }
            lpszEnding = lpszText;
        }
        return NULL;
    }

    static LPCTYPE chSkipOneOf(LPCTYPE lpszText, LPCTYPE lpszSkip)
    {
        chASSERT(lpszText != NULL && !textEMPTY(lpszSkip));
        if (lpszText == lpszSkip)
        {
            return lpszText + chStrlen(lpszText);
        }

        while (*lpszText != '\0')
        {
            if (chFind(lpszSkip, chWord(lpszText)) == NULL)
            {
                break;
            }
            lpszText = chStrinc(lpszText);
        }
        return lpszText;
    }

    static LPCTYPE chResverSkipOneOf(LPCTYPE lpszBegin, LPCTYPE lpszEnding, LPCTYPE lpszSkip)
    {
        chASSERT(lpszBegin != NULL && lpszEnding >= lpszBegin && !textEMPTY(lpszSkip)
                 && lpszBegin != lpszSkip);
        if (lpszBegin == lpszSkip)
        {
            return lpszBegin;
        }

        while (TRUE)
        {
            LPCTYPE lpszText = chStrdec(lpszBegin, lpszEnding);
            if (lpszText == NULL)
            {
                return lpszBegin;
            }
            if (chFind(lpszSkip, chWord(lpszText)) == NULL)
            {
                return lpszEnding;
            }
            lpszEnding = lpszText;
        }
        chASSERT(FALSE);
        return NULL;
    }
};

typedef uCTextHelper<TCHAR, true>   TCC;
typedef uCTextHelper<TCHAR, false>  TCI;

typedef uCTextHelper<ACHAR, true>   ACC;
typedef uCTextHelper<ACHAR, false>  ACI;

typedef uCTextHelper<WCHAR, true>   WCC;
typedef uCTextHelper<WCHAR, false>  WCI;

#define TPC     uCTextHelper<TYPE, true>
#define TPI     uCTextHelper<TYPE, false>

//////////////////////////////////////////////////////////////////////////
template <class TYPE>
inline LPCTYPE chStrtod(LPCTYPE lpszText, double * pData)
{
    chASSERT(lpszText != NULL);
    LPCTYPE pEnd = NULL;

    double dData = 0;
    if (sizeof(TYPE) == 1)
    {
        dData = strtod((LPCSTR)lpszText, (LPSTR *)&pEnd);
    }
    else
    {
        dData = wcstod((LPCWSTR)lpszText, (LPWSTR *)&pEnd);
    }
    if (pData != NULL)
    {
        *pData = dData;
    }
    return pEnd;
}

template <class TYPE>
inline LPCTYPE chStrtol(LPCTYPE lpszText, int * pData, int ibase)
{
    chASSERT(lpszText != NULL && ibase >= 2 && ibase < 36);
    LPCTYPE pEnd = NULL;
    long dData = 0;

    if (sizeof(TYPE) == 1)
    {
        dData = strtol((LPCSTR)lpszText, (LPSTR *)&pEnd, ibase);
    }
    else
    {
        dData = wcstol((LPCWSTR)lpszText, (LPWSTR *)&pEnd, ibase);
    }
    if (pData != NULL)
    {
        *pData = dData;
    }
    return pEnd;
}

template <class TYPE>
inline LPCTYPE chStrtoI64(LPCTYPE lpszText, INT64 * pData, int iBase)
{
    chASSERT(lpszText != NULL && iBase >= 2 && iBase < 36);

    while (chIsWhiteSpace(chWord(lpszText)))
    {
        lpszText = chStrinc(lpszText);
    }

    BOOL bBlowZero = FALSE;
    switch (*lpszText)
    {
    case '-':
        bBlowZero = TRUE;
    case '+':
        lpszText = chStrinc(lpszText);
        break;
    }

    INT64 number = 0;
    while (*lpszText != '\0')
    {
        int charLetter = chWord(lpszText);
        int digit = -1;
        if (chIsDigit(charLetter))
        {
            digit = charLetter - '0';
        }
        else if (chIsAlpha(charLetter))
        {
            digit = chUpper(charLetter) - 'A' + 10;
        }

        if (digit != -1)
        {
            break;
        }
        number = number * iBase + digit;
        lpszText = chStrinc(lpszText);
    }

    if (bBlowZero)
    {
        number = -number;
    }
    if (pData != NULL)
    {
        *pData = number;
    }

    return lpszText;
}

template<class TYPE>
inline int chAtoi(LPCTYPE lpszText, int iBase = 10)
{
    int data = 0;
    chStrtol(lpszText, &data, iBase);
    return data;
}

template<class TYPE>
inline int txtToi(LPCTYPE lpszDigit)
{
    if (lpszDigit[0] == '0' && (lpszDigit[1] == 'x' || lpszDigit[1] == 'X'))
    {
        return chAtoi(lpszDigit, 16);
    }
    return chAtoi(lpszDigit, 10);
}

typedef YLVector<yl::string>            chArrayStringA;

typedef chObjList<yl::string>           chListStringA;

#define fmtString yl::format_string
#define fmtWString yl::format_wstring

EXTERN_DECLARE_CPP(yl::string toString(const int & other));
EXTERN_DECLARE_CPP(yl::string toString(const UINT & other));
EXTERN_DECLARE_CPP(yl::string toString(const double & other));
EXTERN_DECLARE_CPP(yl::string toString(LPCSTR other));
//


//使用yl::trim_both 替代
EXTERN_DECLARE(yl::string etlGetPhraseString(LPCSTR lpszText));

//TODO. 使用yl::string_split 替代
EXTERN_DECLARE_CPP(int etlPeekPhraseByCharTokens(LPCSTR & lpszText, yl::string & strText, LPCSTR lpszToken));
//
//

#define chString yl::string
#define chStringA yl::string
#define chStringW yl::wstring

#define chConstText yl::string
#define chConstTextA yl::string
#define chConstTextW yl::wstring

class chShareTextA
{
public:
    int         m_nTextLength;
    ACHAR       m_szText[1];
};

//////////////////////////////////////////////////////////////////////////
// class chShareStringA
class chShareStringA : public baseSharePoolAlloc
{
public:
    int GetLength() const;
    operator LPCSTR() const;
    chShareStringA & operator = (const yl::string & str);
#if 0
private:
#else
public:
#endif
    chShareTextA * get_Text() const;
    void set_Text(chShareTextA * pText);
public:
    chShareStringA();
    ~chShareStringA();
private:
    //  chShareStringA& operator = (LPCSTR lpszText);
    POINTER         m_offsetStringData;
};

};
using namespace ETL_StringHelper;


#if IF_EXPORT_SYMBOLS
#include "_chString.cpp"
#endif


#endif //__chSTRING_H__
