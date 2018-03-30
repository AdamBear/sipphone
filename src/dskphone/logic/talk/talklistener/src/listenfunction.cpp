#include "talklistener_inc.h"

yl::string GetNameFromContact(const yl::string& strNumber, CALL_TYPE eCallType /*= CT_UNKNOW*/)
{
    if (strNumber.empty())
    {
        return "";
    }

    // 先从本地联系人查找
    yl::string strName;
    if (eCallType == CT_INCOMING
            && configParser_GetConfigInt(kszIncomeCallDisplayName) != 0)
    {
        Dir_GetCotactNameByNumber(strNumber, strName, LOCAL_DIR);
    }
    else
    {
        Dir_GetCotactNameByNumber(strNumber, strName);
    }

    return strName;
}

// 格式化sip信息
yl::string FormatSIPNumber(const yl::string& strNum, const yl::string& strServer)
{
    yl::string strSIPName;
    yl::string strServerNameFromURI;
    commonUnits_ParserDialURI(strNum, strSIPName, strServerNameFromURI);

    yl::string strRetVal = "";
    if (!strSIPName.empty() && !strServerNameFromURI.empty())
    {
        yl::string strTmpServer = strServerNameFromURI;
        string_replace(strTmpServer, "*", ".");

        yl::string strTmpNumber = strSIPName;
        RemoveInvalidChar(strTmpNumber);

        strRetVal = "sip:" + strTmpNumber + "@" + strTmpServer;
    }
    else if (!strServer.empty())
    {
        yl::string strTmpNumber = strNum;
        RemoveInvalidChar(strTmpNumber);
        strRetVal = "sip:" + strTmpNumber + "@" + strServer;
    }
    else
    {
        return strNum;
    }

    return strRetVal;
}
