#include "dsskey_inc.h"


void CFunctionManager::CreateTips(int iTipsID, const yl::string & strTips)
{
    IF_LOG_TRACE("id=[%d] value=[%s]", iTipsID, strTips.c_str());
    CDsskeyPair * pParis = GetPairByIndex(iTipsID);
    if (pParis == NULL)
    {
        m_vecTips.push_back(CDsskeyPair(iTipsID, strTips));
    }
    else
    {
        pParis->strValue = strTips;
    }
}

bool CFunctionManager::CreateFuncData(yl::string & strID, LPCSTR lpType, yl::string & strValue,
                                      int iTipsID)
{
    if (strID.empty() || strValue.empty())
    {
        return false;
    }

    static const mapItem s_func_info_arr[] =
    {
        { DA_LOAD,      XML_ATTR_FUNC_LOAD      },
        { DA_SAVE,      XML_ATTR_FUNC_SAVE      },
        { DA_STATIC,    XML_ATTR_FUNC_STATIC    },
        { DA_DYNAMIC,   XML_ATTR_FUNC_DYNAMIC   },
        { DA_REGEXP,    XML_ATTR_FUNC_REGEXP    },
    };

    int iAttr = DA_NONE;
    for (int i = 0; i < sizeof(s_func_info_arr) / sizeof(s_func_info_arr[0]); ++i)
    {
        if (strstr(lpType, s_func_info_arr[i].strName) != NULL)
        {
            iAttr |= s_func_info_arr[i].iKey;
        }
    }

    if (iAttr == DA_NONE)
    {
        return false;
    }

    CFunDetails * pFunData = GetFunDataByID(strID);
    if (pFunData == NULL)
    {
        CFunDetails funData(iAttr);
        funData.strID = strID;
        funData.strFunction = strValue;
        funData.iTipsID = iTipsID;
        m_vecFunData.push_back(funData);
    }
    else
    {
        pFunData->strFunction = strValue;
        pFunData->iTipsID = iTipsID;
    }

    IF_LOG_TRACE("id=[%s] value=[%s] attr=[%d] tips=[%d]", strID.c_str(), strValue.c_str(), iAttr,
                 iTipsID);
    return true;
}

bool CFunctionManager::ParseFunction(const yl::string & strFunction)
{
    if (strFunction.empty())
    {
        return false;
    }

    yl::string::size_type nLeftPos = strFunction.find("(");
    yl::string::size_type nRightPos = strFunction.rfind(")");

    if (nLeftPos != yl::string::npos
            && nRightPos != yl::string::npos
            && nRightPos > nLeftPos)
    {
        yl::string strName = strFunction.substr(0, nLeftPos);
        yl::string strParam = strFunction.substr(nLeftPos + 1, nRightPos - nLeftPos - 1);

        trim_both(strName);
        trim_both(strParam);

        for (vecFunctionImp::iterator it = m_vecFunImp.begin(); it != m_vecFunImp.end(); ++it)
        {
            if (strName == (*it)->GetFlag())
            {
                return (*it)->OnCalled(strParam);
            }
        }
    }

    return strFunction == XML_BOOL_TRUE;
}

void CFunctionManager::UnInit()
{
    m_vecFunData.clear();
    m_vecTips.clear();
}

CFunctionManager::CFunctionManager() : m_iLastTipsID(INVALID_INDEX)
{
    // 读取整数
    m_vecFunImp.push_back(new CCfgGetIntValueFunction);
    // 获取PHONE TYPE
    m_vecFunImp.push_back(new CCfgGetPhoneTypeFunction);
#ifdef IF_EXTERN_FUNCTION
    // 读取字符
    m_vecFunImp.push_back(new CCfgGetStringValueFunction);
    // 账号个数
    m_vecFunImp.push_back(new CCfgGetAccountNumFunction);
    // 逻辑或
    m_vecFunImp.push_back(new CCfgLogicORFunction);
#endif
    // 逻辑与
    m_vecFunImp.push_back(new CCfgLogicAndFunction);
    // 逻辑非
    m_vecFunImp.push_back(new CCfgLogicNotFunction);
}

bool CFunctionManager::OnSaveAction(const yl::string & strID, const yl::string & strValue /*= ""*/)
{
    return OnMultipleParse(strID, strValue, true);
}

bool CFunctionManager::OnLoadAction(const yl::string & strID)
{
    return OnMultipleParse(strID, strID, false);
}

yl::string CFunctionManager::GetErrorString()
{
    CDsskeyPair * pTipsPair = GetPairByIndex(m_iLastTipsID);
    return pTipsPair ? pTipsPair->strValue : yl::string();
}

bool CFunctionManager::OnMultipleParse(const yl::string & strID, const yl::string & strValue,
                                       bool bSaveData)
{
    /* 此函数有诸多限制：匹配结果进行与操作， 正则表达式，静态函数
    只能是全部是正则表达式或者全部是静态函数，两者不能混杂*/

    if (strID.empty())
    {
        return true;
    }

    m_iLastTipsID = INVALID_INDEX;

    // 多个函数与语义
    if (strID.find(",") != yl::string::npos)
    {
        vecString vecList;
        ParseString(vecList, strID, ", ");

        for (vecString::iterator it = vecList.begin(); it != vecList.end(); ++it)
        {
            if (OnSingleParse(*it, strValue, bSaveData))
            {
                continue;
            }

            return false;
        }

        return true;
    }

    return OnSingleParse(strID, strValue, bSaveData);
}

bool CFunctionManager::CreateFuctionResult()
{
    IF_TimeTrace;
    bool bRet = false;
    for (vecFunDetails::iterator it = m_vecFunData.begin(); it != m_vecFunData.end(); ++it)
    {
        CFunDetails & FunData = *it;

        if (FunData.iAttr & DA_STATIC)
        {
            bool bResult = ParseFunction(FunData.strFunction);
            if (bResult != FunData.bResult)
            {
                FunData.bResult = bResult;
                bRet = true;
                IF_LOG_TRACE("fun=[%s] result=[%d]", FunData.strFunction.c_str(), FunData.bResult);
            }
        }
    }

    return bRet;
}

bool CFunctionManager::OnSingleParse(const yl::string & strID, const yl::string & strValue,
                                     bool bSaveData)
{
    yl::string strFunctionID = strID;
    trim_both(strFunctionID);

    IF_LOG_TRACE("[%s][%s] save=[%d]", strFunctionID.c_str(), strValue.c_str(), bSaveData);
    // 去掉$标志
    if (strFunctionID.find('$') == 0)
    {
        strFunctionID.erase(0, 1);
    }

    // 简单类型直接返回
    if (STR_CMP_PLUS(strFunctionID.c_str(), XML_BOOL_TRUE))
    {
        return true;
    }
    else if (STR_CMP_PLUS(strFunctionID.c_str(), XML_BOOL_FALSE))
    {
        return false;
    }

    CFunDetails * pFunData = GetFunDataByID(strFunctionID);
    if (pFunData != NULL)
    {
        const CFunDetails & funData = *pFunData;
        int iAttr = funData.iAttr;

        if (bSaveData && (iAttr & DA_REGEXP))
        {
            if (regExp_IsMatchedExact(funData.strFunction.c_str(), strValue.c_str()) == 0)
            {
                return true;
            }

            m_iLastTipsID = funData.iTipsID;
            return false;
        }
        else if (iAttr & DA_STATIC)
        {
            bool bSave = (bSaveData && (iAttr & DA_SAVE));
            bool bLoad = (!bSaveData && (iAttr & DA_LOAD));

            if (bSave || bLoad)
            {
                if (funData.bResult)
                {
                    return true;
                }

                m_iLastTipsID = funData.iTipsID;
                return false;
            }
        }
    }

    return true;
}

CDsskeyPair * CFunctionManager::GetPairByIndex(int iIndex)
{
    for (VecCandidateValue::iterator it = m_vecTips.begin(); it != m_vecTips.end(); ++it)
    {
        if (iIndex == it->iIndex)
        {
            return &(*it);
        }
    }

    return NULL;
}

CFunDetails * CFunctionManager::GetFunDataByID(const yl::string & strID)
{
    for (vecFunDetails::iterator it = m_vecFunData.begin(); it != m_vecFunData.end(); ++it)
    {
        if (strID == it->strID)
        {
            return &(*it);
        }
    }

    return NULL;
}

bool CFunctionManager::SaveRegexp(LPCSTR lpFileName)
{
    if (lpFileName == NULL)
    {
        return false;
    }

    using namespace pugi;
    xml_document doc;

    for (vecFunDetails::iterator it = m_vecFunData.begin(); it != m_vecFunData.end(); ++it)
    {
        const CFunDetails & FunData = *it;
        if (FunData.iAttr & DA_REGEXP)
        {
            xml_node nodeItem = doc.append_child(XML_SUB_NODE_ITEM);
            nodeItem.append_attribute(XML_ATTR_ID, FunData.strID.c_str());
            nodeItem.append_attribute(XML_ATTR_VALUE, FunData.strFunction.c_str());

            CDsskeyPair * pTipsPairs = GetPairByIndex(FunData.iTipsID);
            if (pTipsPairs != NULL)
            {
                nodeItem.append_attribute(XML_ATTR_TIPS, pTipsPairs->strValue.c_str());
            }
        }
    }

    return doc.save_file(lpFileName);
}
