#include "editcheckmethod.h"


// 指定IP是否是IPV6的IP
BOOL netIsValidIP_ipv6(LPCSTR lpszIpAddress)
{
    if (lpszIpAddress == NULL || strlen(lpszIpAddress) == 0)
    {
        return FALSE;
    }
#ifdef WIN32
    if (strchr(lpszIpAddress, ':') != NULL
            && strncmp("fe80", lpszIpAddress, 4) != 0
            && strncmp("ff", lpszIpAddress, 2) != 0)
    {
        return TRUE;
    }
    return FALSE;
#else
    if (strchr(lpszIpAddress, ':') == NULL
            || strncmp("fe80", lpszIpAddress, 4) == 0
            || strncmp("ff", lpszIpAddress, 2) == 0)
    {
        return FALSE;
    }
    chStringA strIP = lpszIpAddress;
    struct in6_addr tmpaddr;
    return (inet_pton(AF_INET6, (char *)(LPCTSTR)strIP.c_str(), &tmpaddr) > 0);
#endif
}

// 判断是否是正确的IPV6 gateway
BOOL netIsValidGateWay_ipv6(LPCSTR lpszIpAddress)
{
    if (lpszIpAddress == NULL || strlen(lpszIpAddress) == 0)
    {
        return FALSE;
    }
#ifdef WIN32
    return TRUE;
#else
    chStringA strIP = lpszIpAddress;
    struct in6_addr tmpaddr;
    return (inet_pton(AF_INET6, (char *)(LPCTSTR)strIP.c_str(), &tmpaddr) > 0);
#endif
}

bool IsValidIP(const yl::string & strValue)
{
    return (TRUE == netIsValidIP(strValue.c_str()));
}

bool IsValidIPv4(const yl::string & strValue)
{
    return (TRUE == netIsValidIPv4(strValue.c_str()));
}

bool IsValidIPv6(const yl::string & strValue)
{
    return (TRUE == netIsValidIPV6(strValue.c_str()));
}

bool IsValidIPv4NetMask(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;

    strTrimValue.trim_both(" ");

    if (IsValidIPv4(strTrimValue.c_str()))
    {
        UINT ip = 0, b[4];
        if (sscanf(strTrimValue.c_str(), "%u.%u.%u.%u", &b[3], &b[2], &b[1], &b[0]) == 4)
        {
            for (int i = 0; i < 4; i++)
            {
                ip += b[i] << (i << 3);
            }
            ip = ~ip + 1;
            if ((ip & (ip - 1)) == 0)// if 2^n
            {
                return true;
            }
        }
    }
    return false;
}

bool IsValidIPv6GateWay(const yl::string & strValue)
{
    return (TRUE == netIsValidIPV6GateWay(strValue.c_str()));
}

bool isInRange(const yl::string & strValue, int min, int max)
{
    if (!strValue.empty())
    {
        yl::string strTrimValue = strValue;

        strTrimValue.trim_both(" ");

        int iLength = strTrimValue.length();
        int nValue = 0;
        for (int i = 0; i < iLength; i++)
        {
            if (!chIsDigit(strTrimValue[i]))
            {
                return false;
            }
            nValue *= 10;
            int nNum = strTrimValue[i] - '0';
            nValue += nNum;
        }

        return nValue >= min && nValue <= max;
    }
    return false;
}

bool IsValidIPv6Prefix(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;
    if (strValue != strTrimValue.trim_both(" "))
    {
        return false;
    }

    //0-128
    return isInRange(strValue, 0, 128);
}

bool IsValidPort(const yl::string & strValue)
{
    return isInRange(strValue, 0, 65535);
}

bool IsValidDTMF(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;

    strTrimValue.trim_both(" ");

    //只接受 数字,'*','#'
    LPCSTR sz = strTrimValue.c_str();
    while (*sz != '\0')
    {
        if (!chIsDigit(*sz) && *sz != '*' && *sz != '#')
        {
            return false;
        }
        sz++;
    }
    return true;
}

bool IsValidNumber(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;

    strTrimValue.trim_both(" ");

    //只接受 数字
    LPCSTR sz = strTrimValue.c_str();
    while (*sz != '\0')
    {
        if (!chIsDigit(*sz))
        {
            return false;
        }
        sz++;
    }
    return true;
}

bool IsValidAES(const yl::string & strValue)
{
    LPCSTR sz = strValue.c_str();

    while (*sz != '\0')
    {
        if (*sz == '(' || *sz == ')' || *sz == '>' || *sz == '<'
                || *sz == '|' || *sz == '\'' || *sz == '&' || *sz == ' ' || *sz == '!')
        {
            return false;
        }
        sz++;
    }
    return true;
}

bool IsContentEmpty(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;

    strTrimValue.trim_both(" ");

    return strTrimValue.empty();
}

/*
国际域名格式:
域名由各国文字的特定字符集、英文字母、数字及“-”(即连字符或减号)任意组合而成, 但开头及结尾均不能含有“-”。
域名中字母不分大小写。域名最长可达67个字节
*/
bool IsValidURLServer(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;

    strTrimValue.trim_both(" ");

    // 如果全为数字和点号，则必然为非法的URL
    LPCSTR sz = strTrimValue.c_str();
    bool bIpFormat = true;
    bool bUrlChars = true;
    while (*sz != '\0')
    {
        if (chIsDigit(*sz) || chIsAlpha(*sz) || *sz == '.' || *sz == '-')
        {
            if (bIpFormat && !chIsDigit(*sz) && *sz != '.')
            {
                bIpFormat = false;
            }
        }
        else
        {
            bUrlChars = false;
            break;
        }
        sz++;
    }

    if (bIpFormat || !bUrlChars || strTrimValue.empty() || strTrimValue.length() > 67
            || strTrimValue[0] == '-' || strTrimValue[strTrimValue.length() - 1] == '-'
            || ACC::chFind(strTrimValue.c_str(), "--") != NULL
            || ACC::chFind(strTrimValue.c_str(), " ") != NULL)
    {
        return false;
    }
    return true;
}

bool IsValidServer(const yl::string & strValue)
{
    return true;
}

bool IsValidServerName(const yl::string & strValue)
{
    yl::string strTrimValue = strValue;

    strTrimValue.trim_both(" ");

    // SIP Server:首个字符不能为":",：这个符号会引起被转接方显示错误
    if (strTrimValue.length() > 0 && ':' == strTrimValue.at(0))
    {
        return false;
    }

    return true;
}

bool IsValidAccountName(const yl::string & strValue)
{
    LPCSTR sz = strValue.c_str();
    //Account相关的名称不能含有 !&:;[]?%
    while (*sz != '\0')
    {
        if (*sz == '!' || *sz == '&' || *sz == ':' || *sz == ';'
                || *sz == '[' || *sz == ']' || *sz == '?'  || *sz == '%')
        {
            return false;
        }
        sz++;
    }
    return true;
}

bool IsValidPassword(const yl::string & strValue)
{
    LPCSTR sz = strValue.c_str();
    //帐号密码:不能含有冒号
    while (*sz != '\0')
    {
        if (*sz == ':')
        {
            return false;
        }
        sz++;
    }
    return true;
}

bool IsValidQos(const yl::string & strValue)
{
    //0-63
    return isInRange(strValue, 0, 63);
}

bool IsValidVideoMTU(const yl::string & strValue)
{
    //1000-1500
    return isInRange(strValue, 1000, 1500);
}

bool IsValidVlanVID(const yl::string & strValue)
{
    //1-4094
    return isInRange(strValue, 1, 4094);
}

bool IsValidPacketInterval(const yl::string & strValue)
{
    //1-3600s
    return isInRange(strValue, 1, 3600);
}

bool IsValidServerExpires(const yl::string & strValue)
{
    //0~2147483647
    return isInRange(strValue, 0, 2147483647);
}

bool IsValidUdpPort(const yl::string & strValue)
{
    //1~65535
    return isInRange(strValue, 1024, 65535);
}

bool IsValidConferencePwd(const yl::string & strValue)
{
    if (!strValue.empty())
    {
        yl::string strTrimValue = strValue;

        strTrimValue.trim_both(" ");

        int iLength = strTrimValue.length();
        for (int i = 0; i < iLength; i++)
        {
            if (!chIsDigit(strTrimValue[i]))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}
