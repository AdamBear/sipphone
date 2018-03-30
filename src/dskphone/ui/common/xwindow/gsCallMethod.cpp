#include "ETLLib/ETLLib.hpp"
#include "gsCallMethod.h"
#include "commonapi/stringhelperapi.h"

#if IF_VIEW_USER_ACTION
static LPCSTR PeekMethodName(LPCSTR lpszText, yl::string& strMethodName)
{
    LPCSTR lpszWordBegin = NULL;

    for (; *lpszText != '\0'; ++lpszText)
    {
        char cText = *lpszText;
        bool bIsWordHead = false;

        if (isalpha(cText) || '_' == cText)
        {
            bIsWordHead = true;
        }

        if (NULL == lpszWordBegin)
        {
            if (bIsWordHead)
            {
                lpszWordBegin = lpszText;
            }
        }
        else if (!bIsWordHead && !isdigit(*lpszText))
        {
            strMethodName = yl::string(lpszWordBegin, lpszText);

            return lpszText;
        }
    }

    return NULL;
}

extern ST_ROUTINE_METHOD s_RoutineTable[];
static ST_ROUTINE_METHOD* GetNextRoutine(ST_ROUTINE_METHOD* pRoutine)
{
    ++pRoutine;

    if (NULL == pRoutine->pRoutineMethod)
    {
        if (NULL == pRoutine->lpszParamList)
        {
            // all end
            return NULL;
        }

        return (ST_ROUTINE_METHOD*)pRoutine->lpszParamList;
    }

    return pRoutine;
}

static FUN_ROUTINE_METHOD FindMethByName(const yl::string& strMethodName)
{
    ST_ROUTINE_METHOD* pRoutine = s_RoutineTable;
    int nLoop = 0;

    while (NULL != pRoutine)
    {
        DEAD_LOOP_BREAK(nLoop);

        if (pRoutine->strKeyWord == strMethodName)
        {
            return pRoutine->pRoutineMethod;
        }

        pRoutine = GetNextRoutine(pRoutine);
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
jsParameter::jsParameter(LPCSTR& lpszText)
{
    lpszText = ParseFunctionText(NS_xWindow::SkipWhiteSpace(lpszText));
}

int jsParameter::GetCount() const
{
    return m_vecParam.size();
}

yl::string jsParameter::GetParam(int nIndex) const
{
    if (nIndex >= 0 && nIndex < m_vecParam.size())
    {
        return m_vecParam[nIndex];
    }

    return yl::string("");
}

LPCSTR jsParameter::ParseFunctionText(LPCSTR lpszParamText)
{
    if (NULL == lpszParamText)
    {
        return NULL;
    }

    chASSERT(lpszParamText[0] == '(');

    lpszParamText += 1;
    m_vecParam.clear();

    yl::string strCurrentParam;
    LPCSTR lpszParamBegin = NULL;
    int nLoop = 0;

    while (*lpszParamText != '\0')
    {
        DEAD_LOOP_BREAK(nLoop);

        lpszParamText = NS_xWindow::SkipWhiteSpace(lpszParamText);

        if (*lpszParamText == '"')
        {
            lpszParamBegin = lpszParamText + 1;

            yl::string strParam;

            lpszParamText = NS_xWindow::PeekContentFromQuotaString(lpszParamBegin, strParam);
            strCurrentParam += strParam;
        }
        else if (*lpszParamText == ',')
        {
            if (lpszParamBegin == NULL)
            {
                return NULL;
            }

            m_vecParam.push_back(strCurrentParam);
            strCurrentParam = "";
            lpszParamBegin = NULL;
            lpszParamText ++;
        }
        else if (*lpszParamText == ')')
        {
            if (lpszParamBegin != NULL)
            {
                m_vecParam.push_back(strCurrentParam);
                strCurrentParam = "";
                lpszParamBegin = NULL;
            }
            else if (0 != m_vecParam.size())
            {
                return NULL;
            }

            return lpszParamText + 1;
        }
        else
        {
            chASSERT(lpszParamBegin == NULL);
            int iIndex = NS_xWindow::PeekPhraseByCharTokens(lpszParamText, strCurrentParam, ",)");
            if (iIndex == -1 || iIndex >= 2)
            {
                return NULL;
            }
            m_vecParam.push_back(strCurrentParam);
            strCurrentParam = "";
            if (iIndex == 1)
            {
                return lpszParamText;
            }
        }
    }

    return NULL;
}

BOOL AppendRoutineTable(ST_ROUTINE_METHOD* pNewTable)
{
    static ST_ROUTINE_METHOD* pLastRoutine = s_RoutineTable;
    int nLoop = 0;

    for (ST_ROUTINE_METHOD* pRoutine = pLastRoutine; pRoutine != NULL; pRoutine = GetNextRoutine(pRoutine))
    {
        DEAD_LOOP_BREAK(nLoop);

        pLastRoutine = pRoutine;
    }

    pLastRoutine[1].lpszParamList = (LPCSTR)pNewTable;

    return TRUE;
}

yl::string dump_all_text_method()
{
    ST_ROUTINE_METHOD* pRoutine = s_RoutineTable;
    yl::string strResult;
    int iIndex = 1;
    int nLoop = 0;

    while (NULL != pRoutine)
    {
        DEAD_LOOP_BREAK(nLoop);

        strResult += commonAPI_FormatString("%.4d ", iIndex++);
        strResult += pRoutine->strKeyWord;
        strResult = strResult + "(" + pRoutine->lpszParamList + ")\n";
        pRoutine = GetNextRoutine(pRoutine);
    }

    return strResult;
}

static yl::string parse_text_method(LPCSTR& lpszScriptText)
{
    yl::string strResult;
    int nLoop = 0;

    if (NULL == lpszScriptText)
    {
        return strResult;
    }

    while (NULL != lpszScriptText)
    {
        DEAD_LOOP_BREAK(nLoop);

        yl::string strMethodName;
        LPCSTR lpszNextText = PeekMethodName(lpszScriptText, strMethodName);

        if (NULL == lpszNextText)
        {
            strResult += yl::string(lpszScriptText);
            break;
        }
        strResult.append(lpszScriptText, lpszNextText - lpszScriptText - strMethodName.GetLength());
        lpszScriptText = lpszNextText;

        FUN_ROUTINE_METHOD pThisMeth = NULL;
        if (NS_xWindow::SkipWhiteSpace(lpszScriptText)[0] == '(' && (pThisMeth = FindMethByName(strMethodName)) != NULL)
        {
            strResult += (*pThisMeth)(jsParameter(lpszScriptText));
        }
        else
        {
            strResult += strMethodName;
        }
    }

    return strResult;
}

yl::string call_text_method(LPCSTR lpszScriptText)
{
    if (NULL == lpszScriptText || '\0' == *(lpszScriptText))
    {
        return yl::string("");
    }

    return parse_text_method(lpszScriptText);
}

// "LPCSTR lpszText, LPCSTR fmt"
static yl::string sscanf(const jsParameter& objParams)
{
    chASSERT(2 == objParams.GetCount());

    yl::string strParam1 = objParams.GetParam(0);
    yl::string strParam2 = objParams.GetParam(1);
    char szBuf[1024];

    if (1 == sscanf(strParam1.c_str(), strParam2.c_str(), szBuf))
    {
        szBuf[sizeof(szBuf) - 1] = '\0';

        return yl::string(szBuf);
    }

    return yl::string("");
}

// "LPCSTR lpszFileName, LPCSTR fmt"
static yl::string fscanf(const jsParameter& objParams)
{
    chASSERT(2 == objParams.GetCount());
    yl::string strParam1 = objParams.GetParam(0);
    yl::string strParam2 = objParams.GetParam(1);
    char szBuf[1024] = "";

    FILE* pFile = fopen(strParam1.c_str(), "rb");

    if (NULL != pFile)
    {
        if (1 != fscanf(pFile, strParam2.c_str(), szBuf))
        {
            szBuf[0] = '\0';
        }

        fclose(pFile);
    }

    return yl::string(szBuf);
}

// "LPCSTR lpszFileName"
static yl::string mkfifo(const jsParameter& objParams)
{
    chASSERT(1 == objParams.GetCount());

#if LINUX_SYSTEM
    mkfifo(objParams.GetParam(0).c_str(), 0777);
#endif

    return yl::string("");
}

// "int nSeconds"
static yl::string sleep(const jsParameter& objParams)
{
    chASSERT(1 == objParams.GetCount());

    etlSleep(txtToi(objParams.GetParam(0).c_str()) * 1000);

    return yl::string("");
}

// "LPCSTR lpszCommand"
static yl::string system(const jsParameter& objParams)
{
    chASSERT(1 == objParams.GetCount());

    ::system(objParams.GetParam(0).c_str());

    return yl::string("");
}

#define DEF_ROUTINE(routine, param) { #routine, param, &routine}
ST_ROUTINE_METHOD s_RoutineTable[] =
{
    DEF_ROUTINE(sscanf,                     "LPCSTR lpszText, LPCSTR fmt"),
    DEF_ROUTINE(fscanf,                     "LPCSTR lpszFileName, LPCSTR fmt"),
    DEF_ROUTINE(mkfifo,                     "LPCSTR lpszFileName"),
    DEF_ROUTINE(sleep,                      "int nSeconds"),
    DEF_ROUTINE(system,                     "LPCSTR lpszCommand"),

    { "", NULL, NULL}
};
#endif
