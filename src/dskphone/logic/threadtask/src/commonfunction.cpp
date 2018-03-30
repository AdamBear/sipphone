#include "commonfunction.h"
#include <ylstl/ylalgorithm.h>
#include <ylstl/ylstring.h>
//#include "CommonAPI/CommonAPI.h"

namespace NS_TA
{
NetworkProtocolType GetNetworkFileProtocolType(const yl::string & strNetworkFilePath)
{
    if (strNetworkFilePath.empty())
    {
        return NETWORK_PROTOCOL_NONE;
    }

    // 将输入字符转换为小写.
    yl::string strNetworkFilePathCpy = strNetworkFilePath;
    yl::transform(strNetworkFilePathCpy.begin(), strNetworkFilePathCpy.end(),
                  strNetworkFilePathCpy.begin(), tolower);

    NetworkProtocolType eRetType = NETWORK_PROTOCOL_NONE;
    // 判断远程文件的协议类型.
    if (strNetworkFilePathCpy.substr(0, 7) == "http://")
    {
        eRetType = NETWORK_HTTP;
    }
    else if (strNetworkFilePathCpy.substr(0, 8) == "https://")
    {
        eRetType = NETWORK_HTTPS;
    }
    else if (strNetworkFilePathCpy.substr(0, 6) == "ftp://")
    {
        eRetType = NETWORK_FTP;
    }
    else if (strNetworkFilePathCpy.substr(0, 7) == "tftp://")
    {
        eRetType = NETWORK_TFTP;
    }
    else
    {
        eRetType = NETWORK_PROTOCOL_NONE;
    }

    return eRetType;
}

yl::string EncodeHttp(const yl::string & strInput)
{
    struct stTemp
    {
        yl::string m_strSrc;  // 源字符串.
        yl::string m_strDest;  // 目标字符串.
    };

    static const stTemp stReplaceList[] =
    {
        {"%", "%25"},
        {" ", "%10"},
        {"\"", "%22"},
        {"<", "%3C"},
        {">", "%3E"},
        {"[", "%5B"},
        {"\\", "%5C"},
        {"]", "%5D"},
        {"^", "%5E"},
        {"`", "%60"},
        {"`", "%60"},
        {"{", "%7B"},
        {"|", "%7C"},
        {"}", "%7D"}
    };

    // 替换各个字符.
    for (size_t i = 0;
            i < sizeof(stReplaceList) / sizeof(stReplaceList[0]);
            ++i)
    {
        /*strInput = translateString(strInput,
            stReplaceList[i].m_strSrc, stReplaceList[i].m_strDest);*/
    }

    return strInput;
}

//!!!!!可添加至公用函数中
yl::string translateString(const yl::string & strInput,
                           const yl::string & strFrom, const yl::string & strTo)
{
    if (strFrom.empty() || strTo.empty())
    {
        return strInput;
    }

    yl::string strInputCpy = strInput;
    // Iterates to replace.
    size_t pos = strInputCpy.find(strFrom, 0);
    while (pos != yl::string::npos)
    {
        strInputCpy.replace(pos, strFrom.length(), strTo);
        pos = strInputCpy.find(strFrom, pos + strTo.length());
    }

    return strInputCpy;
}

bool ParserURLPath(const yl::string & strSrcPath, yl::string & strUser, yl::string & strPwd
                   , yl::string & strServer, yl::string & strRemoteFile
                   , yl::string & strPort, yl::string & strDefaultPort)
{
    if (strSrcPath.empty())
    {
        strUser = "";
        strPwd = "";
        strServer = "";
        strRemoteFile = "";
        strPort = "";

        return false;
    }

    yl::string strRemoteFileTmp = "";
    int nHdPos = strSrcPath.find("://");
    nHdPos = (nHdPos != yl::string::npos) ? nHdPos + 3 : 0;
    int nAtPos = strSrcPath.find("@", nHdPos);
    if (nAtPos != yl::string::npos)
    {
        // 查找 用户名:密码
        int nCommaPos = strSrcPath.find(":", nHdPos);
        if (nCommaPos != yl::string::npos)
        {
            strUser = strSrcPath.substr(nHdPos, nCommaPos - nHdPos);
            strPwd = strSrcPath.substr(nCommaPos + 1, nAtPos - nCommaPos - 1);
        }
    }
    strRemoteFileTmp = strSrcPath.substr((nAtPos == -1) ? (nHdPos) : (nAtPos + 1));
    if (!strRemoteFileTmp.empty())
    {
        int nIndex = strRemoteFileTmp.find_first_of("/");
        if (nIndex != yl::string::npos)
        {
            strServer = strRemoteFileTmp.substr(0, nIndex);
            strRemoteFile = strRemoteFileTmp.substr(nIndex + 1);
        }
        if (!strServer.empty())
        {
            nIndex = strServer.find(":");
            if (nIndex != yl::string::npos)
            {
                strPort = strServer.substr(nIndex + 1);
                strServer = strServer.substr(0, nIndex);
            }
            else
            {
                strPort = strDefaultPort;
            }
        }
    }
    return true;
}

}  // namespace NS_TA
