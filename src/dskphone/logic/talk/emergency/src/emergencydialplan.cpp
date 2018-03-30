#include "emergency_inc.h"


#ifdef IF_FEATURE_EMERGENCY

static bool isValidEmergencyURI(const yl::string & strURI)
{
    // tel:+xxxxxxxxxx
    static const char * lpTelURI = "tel:(\\+)?(\\d)+";
    // xxxxxxxxxx
    static const char * lpDigit = "(\\d){10,25}";
    // sip:xxxxxxxxxx@127.0.0.1  sip:xxxxxxxxxx@yealink.com
    static const char * lpSipURI =
        "sip:(\\+)?(\\w)+@(((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)|\\w+([\\.-]?\\w+)*(\\.\\w{2,3})+)(:[0-9]{1,5})?";

    return regExp_IsMatchedExact(lpTelURI, strURI.c_str()) == 0
           || regExp_IsMatchedExact(lpDigit, strURI.c_str()) == 0
           || regExp_IsMatchedExact(lpSipURI, strURI.c_str()) == 0;
}

static yl::string GetNumInURI(const yl::string & strURI)
{
    if (strURI.empty())
    {
        return strURI;
    }

    yl::string strUser = strURI;
    if (strURI.find("@") == yl::string::npos)
    {
        if (Comm_IsValidIP(strUser.c_str()))
        {
            string_replace(strUser, "*", ".");
        }
        else
        {
            LPCSTR lpTelPrefix = "tel:";
            if (strURI.find(lpTelPrefix) == 0)
            {
                strUser = strURI.substr(strlen(lpTelPrefix));
            }
        }
    }
    else
    {
        char szNum[128] = {0};
        char szServer[128] = {0};

        if (strURI.find("sip:") == 0)
        {
            sscanf(strURI.c_str(), "sip:%[^@]@%s", szNum, szServer);
        }
        else
        {
            sscanf(strURI.c_str(), "%[^@]@%s", szNum, szServer);
        }

        strUser = szNum;
    }

    RemoveInvalidChar(strUser);

    // 跳过第一个+号
    if (strUser.size() > 1 && strUser[0] == '+')
    {
        return yl::string(&(strUser[1]));
    }

    return strUser;
}

CEmergencyMode::CEmergencyMode()
    : CBaseEmergency(EM_ADV)
{
}

bool CEmergencyMode::Reload(int iType, int lParam)
{
    m_vecEmergency.clear();

    if (configParser_GetConfigInt(kszEmengencySwtich) == 0)
    {
        return false;
    }

    yl::string strCfgSource = configParser_GetConfigString(kszAssertedIdSource);
    if (strcasecmp(strCfgSource.c_str(), "ELIN") == 0)
    {
        m_eAssertedIdSrc = EM_SRC_ELIN;
    }
    else if (strcasecmp(strCfgSource.c_str(), "CUSTOM") == 0)
    {
        m_eAssertedIdSrc = EM_SRC_CUSTOM;
    }
    else if (strcasecmp(strCfgSource.c_str(), "HELD") == 0)
    {
        m_eAssertedIdSrc = EM_SRC_HELD;
    }
    else
    {
        m_eAssertedIdSrc = EM_SRC_NONE;
    }

#define MAX_EMERGENCY_NUM  255

    for (int i = 0; i <= MAX_EMERGENCY_NUM; ++i)
    {
        yl::string strValue = GetNumInURI(configParser_GetCfgItemStringValue(kszEmergencyValue, i));

        if (strValue.empty())
        {
            continue;
        }

        m_vecEmergency.push_back(EmergencyItem(i, strValue));
    }

    Sort<EmergencyItem, EmergencyCmpFun>(m_vecEmergency, EmergencyItem::EmergencyCmp);

    return true;
}

const CEmergencyMode::EmergencyItem * CEmergencyMode::Find(const yl::string & strNum)
{
    int iLowerIndex = 0;
    int iUpperIndex = m_vecEmergency.size() - 1;

    // 二分法搜索位置
    while (iLowerIndex <= iUpperIndex)
    {
        int iMidIndex = (iUpperIndex + iLowerIndex) / 2;

        const EmergencyItem & tMidItem = m_vecEmergency[iMidIndex];

        if (tMidItem == strNum)
        {
            return &tMidItem;
        }

        if (tMidItem < strNum)
        {
            iLowerIndex = iMidIndex + 1;
        }
        else if (tMidItem > strNum)
        {
            iUpperIndex = iMidIndex - 1;
        }
    }

    return NULL;
}

const CEmergencyMode::EmergencyItem * CEmergencyMode::SemiFind(const yl::string & strNum)
{
    // 普通查找
    for (VecEmergency::iterator it = m_vecEmergency.begin(); it != m_vecEmergency.end(); ++it)
    {
        if (strncmp(it->strNum.c_str(), strNum.c_str(), strNum.length()) == 0)
        {
            return &(*it);
        }
    }

    return NULL;
}

bool CEmergencyMode::IsMatched(const char * cszTarget, int iType, CMatchedInfo * pInfo /*= NULL*/,
                               bool IsCharCmp /*= false*/)
{
    if (textEMPTY(cszTarget))
    {
        return false;
    }

    yl::string strValue = GetNumInURI(cszTarget);

    const EmergencyItem * pItem = IsCharCmp ? SemiFind(strValue) : Find(strValue);
    if (pItem != NULL && pInfo != NULL)
    {
        pInfo->m_iLastMatchedIndex = pItem->iIndex;
        pInfo->m_iLastMatchedMode |= GetType();

        return true;
    }

    return false;
}

void CEmergencyMode::Init()
{
    Reload(GetType(), 0);
}

LPCSTR CEmergencyMode::GetAssertedId()
{
    m_strAssertId.clear();
    switch (m_eAssertedIdSrc)
    {
    case EM_SRC_ELIN:
    case EM_SRC_CUSTOM:
        {
            // 创建 asserted_id
            yl::string strLLDPId;
            const size_t ibufsize = 128;
            char buf[ibufsize] = { 0 };

            // 应网络部要求第一个字节标识类型，ELIN对应的类型是3
            if (sys_get_lldp_location(buf, ibufsize) == 0 && buf[0] == 3)
            {
                strLLDPId = (char *)(&buf[1]);
            }

            yl::string strCustomId = configParser_GetConfigString(kszCustomAssertedId);

            TALK_INFO("CEmergencyMode::GetAssertedId lldap=[%s] custom=[%s]", strLLDPId.c_str(), strCustomId.c_str());

            if (!isValidEmergencyURI(strCustomId))
            {
                strCustomId.clear();
            }

            if ((m_eAssertedIdSrc == EM_SRC_ELIN && strLLDPId.empty())
                    || (m_eAssertedIdSrc == EM_SRC_CUSTOM && !strCustomId.empty()))
            {
                m_strAssertId = strCustomId;
            }
            else
            {
                m_strAssertId = strLLDPId;
            }

            if (!m_strAssertId.empty())
            {
                m_strAssertId = commonAPI_FormatString("P-Asserted-Identity:%s\r\n", m_strAssertId.c_str());
            }
        }
        break;
#if IF_FEATURE_EMERGENCY_HELD
    case EM_SRC_HELD:
        {
            m_strAssertId = EmergencyHELD_getLocation();
            if (!m_strAssertId.empty())
            {
                m_strAssertId = commonAPI_FormatString("Geolocation:%s\r\nGeolocation-Routing:yes\r\n", m_strAssertId.c_str());
            }
        }
        break;
#endif
    default:
        break;
    }

    TALK_INFO("CEmergencyMode::GetAssertedId AssertedIdSrc=[%d] assertid=[%s]", m_eAssertedIdSrc, m_strAssertId.c_str());

    return m_strAssertId.c_str();
}

#endif
