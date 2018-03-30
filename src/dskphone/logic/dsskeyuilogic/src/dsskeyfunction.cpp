#include "dsskey_inc.h"

bool CCfgStaticFunctionBase::OnCalled(const yl::string & strValue)
{
    vecString veclist;
    if (ParseString(veclist, strValue, GetSeparator()))
    {
        for (vecString::iterator it = veclist.begin(); it != veclist.end(); ++it)
        {
            trim_both(*it);
        }

        return operator()(veclist);
    }

    return false;
}

bool CCfgGetIntValueFunction::LogicOperate(int iLeft, int iRight, const yl::string & strOpType)
{
    /* 默认使用等于号 */
    if (strOpType.empty() || strOpType == "==")
    {
        return iLeft == iRight;
    }
    else if (strOpType == "!=")
    {
        return iLeft != iRight;
    }
    else if (strOpType == ">=")
    {
        return iLeft >= iRight;
    }
    else if (strOpType == ">")
    {
        return iLeft > iRight;
    }
    else if (strOpType == "<=")
    {
        return iLeft <= iRight;
    }
    else if (strOpType == "<")
    {
        return iLeft < iRight;
    }

    return false;
}

bool CCfgGetIntValueFunction::operator()(vecString & veclist)
{
    int iSize = veclist.size();
    if (iSize > 1)
    {
        const yl::string & strCfgId = veclist[0];
        const yl::string & strValue = veclist[1];

        if (strCfgId.empty() || strValue.empty())
        {
            return false;
        }

        return GetCfgValue(strCfgId, strValue, iSize > 2 ? veclist[2] : "");
    }

    return false;
}

bool CCfgGetIntValueFunction::GetCfgValue(const yl::string & strFirstValue,
        const yl::string & strSecondValue, const yl::string & strThirdValue)
{
    int iCfgValue = configParser_GetConfigInt(strFirstValue.c_str());
    return LogicOperate(iCfgValue, atoi(strSecondValue.c_str()), strThirdValue);
}

bool CCfgGetPhoneTypeFunction::operator()(vecString & veclist)
{
    LPCSTR lpPhoneName = devicelib_GetPhoneName();
    for (vecString::iterator it = veclist.begin(); it != veclist.end(); ++it)
    {
        if (strcmp(lpPhoneName, it->c_str()) == 0)
        {
            return true;
        }
    }

    return false;
}

#ifdef IF_EXTERN_FUNCTION
bool CCfgGetStringValueFunction::GetCfgValue(const yl::string & strFirstValue,
        const yl::string & strSecondValue, const yl::string & strThirdValue)
{
    yl::string strCfgValue = configParser_GetConfigString(strFirstValue.c_str());
    return LogicOperate<yl::string>(strCfgValue, strSecondValue, strThirdValue);
}

bool CCfgGetAccountNumFunction::GetCfgValue(const yl::string & strFirstValue,
        const yl::string & strSecondValue, const yl::string & strThirdValue)
{
    return LogicOperate<int>(acc_AccountNum(), atoi(strFirstValue.c_str()), strSecondValue);
}

template<typename T>
bool CCfgGetStringValueFunction::LogicOperate(const T & tL, const T & tR,
        const yl::string & strOpType)
{
    /* 默认使用等于号 */
    if (strOpType.empty() || strOpType == "==")
    {
        return tL == tR;
    }
    else if (strOpType == "!=")
    {
        return tL != tR;
    }
    else if (strOpType == ">=")
    {
        return tL >= tR;
    }
    else if (strOpType == ">")
    {
        return tL > tR;
    }
    else if (strOpType == "<=")
    {
        return tL <= tR;
    }
    else if (strOpType == "<")
    {
        return tL < tR;
    }

    return false;
}

bool CCfgLogicORFunction::operator()(vecString & veclist)
{
    bool bRet = false;
    for (vecString::iterator it = veclist.begin(); it != veclist.end(); ++it)
    {
        bRet |= _DsskeyUIData.GetFunctionResult(*it);
    }

    return bRet;
}
#endif

bool CCfgLogicAndFunction::operator()(vecString & veclist)
{
    bool bRet = true;
    for (vecString::iterator it = veclist.begin(); it != veclist.end(); ++it)
    {
        bRet &= _DsskeyUIData.GetFunctionResult(*it);
    }

    return bRet;
}

bool CCfgLogicNotFunction::operator()(vecString & veclist)
{
    return !(CCfgLogicAndFunction::operator()(veclist));
}

int StrMap2Key(LPCSTR lpValue, const mapItem mapArr[], int iSize)
{
    if (lpValue == NULL)
    {
        return iSize > 0 ? mapArr[0].iKey : -1;
    }

    for (int i = 1; i < iSize; ++i)
    {
        if (STR_CMP_PLUS(lpValue, mapArr[i].strName))
        {
            return mapArr[i].iKey;
        }
    }

    return iSize > 0 ? mapArr[0].iKey : -1;
}

LPCSTR KeyMap2Str(int iKey, const mapItem mapArr[], int iSize)
{
    for (int i = 0; i < iSize; ++i)
    {
        if (iKey == mapArr[i].iKey)
        {
            return mapArr[i].strName;
        }
    }

    return iSize > 0 ? mapArr[0].strName : NULL;
}

