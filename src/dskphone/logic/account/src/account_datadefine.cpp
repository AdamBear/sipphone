#include "account_datadefine.h"


SipAccountType GetTypeByName(const yl::string & strName)
{
    SipAccountType eType = ACCOUNT_COMMON;
    if (strName == "zoom")
    {
        eType = ACCOUNT_ZOOM;
    }
    else if (strName == "pexip")
    {
        eType = ACCOUNT_PEXIP;
    }
    else if (strName == "mind")
    {
        eType = ACCOUNT_MIND;
    }
    else if (strName == "bluejeans")
    {
        eType = ACCOUNT_BLUEJEANS;
    }
    else if (strName == "custom")
    {
        eType = ACCOUNT_CUSTOM;
    }
    else if (strName == "yealink")
    {
        eType = ACCOUNT_YEALINK;
    }

    return eType;
}

CloudAccountLoginStatus GetStateByName(const yl::string & strName)
{
    CloudAccountLoginStatus eType = STATUS_LOGIN_NONE;
    if (strName == "needregister")
    {
        eType = STATUS_LOGIN_REQUEST;
    }
//  else if (strName == "registered")
//  {
//      eType = STATUS_LOGIN_SUCC;
//  }

    return eType;
}

const char * GetNameByState(CloudAccountLoginStatus eType)
{
    if (eType == STATUS_LOGIN_REQUEST)
    {
        return "needregister";
    }

    return "";
}

const char * CloudAccountInfo::GetNameByType(SipAccountType eType, bool bLabel)
{
    if (eType == ACCOUNT_ZOOM)
    {
        return bLabel ? "Zoom" : "zoom";
    }
    else if (eType == ACCOUNT_PEXIP)
    {
        return bLabel ? "Pexip" : "pexip";
    }
    else if (eType == ACCOUNT_MIND)
    {
        return bLabel ? "Mind" : "mind";
    }
    else if (eType == ACCOUNT_BLUEJEANS)
    {
        return bLabel ? "Bluejeans" : "bluejeans";
    }
    else if (eType == ACCOUNT_CUSTOM)
    {
        return bLabel ? "" : "custom";
    }
    else
    {
        return bLabel ? "" : "yealink";
    }

    return "";
}

bool CloudAccountInfo::ParseFromXMLNode(const xml_node & node)
{
    xml_attribute attrTmp = node.attribute(kszAttrUserName);
    if (attrTmp)
    {
        m_strUserName = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrPassWord);
    yl::string strInString = attrTmp.value();
    if (attrTmp)
    {
        if (!strInString.empty())
        {
            unsigned char lpOutPassword[160] = "";
            if (0 == pw_data_decrypt((const unsigned char *)strInString.c_str(), lpOutPassword))
            {
                m_strPassWord = commonAPI_FormatString("%s", lpOutPassword);
            }
        }
        else
        {
            m_strPassWord = strInString;
        }
    }

    attrTmp = node.attribute(kszAttrSavePassWord);
    if (attrTmp)
    {
        yl::string strTemp = attrTmp.value();
        m_bSavePass = strTemp == "1" ? true : false;
    }

    attrTmp = node.attribute(kszAttrType);
    if (attrTmp)
    {
        m_eType = GetTypeByName(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrState);
    if (attrTmp)
    {
        m_eStatus = GetStateByName(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrAccount);
    if (attrTmp)
    {
        m_nAccountId = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrRegistername);
    if (attrTmp)
    {
        m_strRegName = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrServer);
    if (attrTmp)
    {
        m_strServer = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrLabel);
    strInString = attrTmp.value();
    if (attrTmp
            && !strInString.empty())
    {
        m_strLabel = attrTmp.value();
    }
    else if (m_eType == ACCOUNT_ZOOM
             || m_eType == ACCOUNT_MIND
             || m_eType == ACCOUNT_BLUEJEANS)
    {
        m_strLabel = GetNameByType(m_eType, true);
    }

    attrTmp = node.attribute(kszAttrDtmfType);
    if (attrTmp)
    {
        m_nDtmfType = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrDtmfInfoType);
    if (attrTmp)
    {
        m_nDtmfInfoType = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrDtmfPayload);
    if (attrTmp)
    {
        m_nDtmfPayload = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrKeeplive);
    if (attrTmp)
    {
        m_nKeepAlive = atoi(attrTmp.value());
    }

    if (m_eType == ACCOUNT_YEALINK)
    {
        // Yealink强制写成2
        m_nTransport = 2;
    }
    else
    {
        attrTmp = node.attribute(kszAttrTransport);
        if (attrTmp)
        {
            m_nTransport = atoi(attrTmp.value());
        }
    }

    attrTmp = node.attribute(kszAttrSrtp);
    if (attrTmp)
    {
        m_nSrtp = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrPort);
    if (attrTmp)
    {
        m_nPort = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrBfcp);
    if (attrTmp)
    {
        m_nBfcp = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrFecc);
    if (attrTmp)
    {
        m_nFecc = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrExtvaddonce);
    if (attrTmp)
    {
        m_nExtvaddonce = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrExpires);
    if (attrTmp)
    {
        m_nExpires = atoi(attrTmp.value());
    }

    Print();

    return true;
}

bool CloudAccountInfo::ToXMLNode(xml_node & xmlNode)
{
    yl::string strTmp = GetNameByType(m_eType, false);

    xmlNode.append_attribute(kszAttrType, strTmp.c_str());

    // GUI写文件，肯定是经过GUI处理过
//      if (m_eStatus == STATUS_LOGIN_REQUEST)
//      {
//          xmlNode.append_attribute(kszAttrState, GetNameByState(m_eStatus));
//      }

#define kszMaxLen 32

    char szBuffer[kszMaxLen + 1] = {0};
    memset(szBuffer, 0, sizeof(szBuffer));
    snprintf(szBuffer, kszMaxLen, "%s", m_bSavePass ? "1" : "0");
    szBuffer[kszMaxLen] = '\0';
    xmlNode.append_attribute(kszAttrSavePassWord, szBuffer);

    if (m_nAccountId != -1)
    {
        strTmp = commonAPI_FormatString("%d", m_nAccountId);
        xmlNode.append_attribute(kszAttrAccount, strTmp.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrAccount, "");
    }

    xmlNode.append_attribute(kszAttrState, "");

    xmlNode.append_attribute(kszAttrLabel, m_strLabel.c_str());

    xmlNode.append_attribute(kszAttrUserName, m_strUserName.c_str());

    xmlNode.append_attribute(kszAttrRegistername, m_strRegName.c_str());


    if (m_bSavePass)
    {
        yl::string strOutString = "";
        unsigned char lpOutPassword[160] = "";
        if (0 == pw_data_encrypt((const unsigned char *)m_strPassWord.c_str(), lpOutPassword))
        {
            strOutString = commonAPI_FormatString("%s", lpOutPassword);
        }
        xmlNode.append_attribute(kszAttrPassWord, strOutString.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrPassWord, "");
    }

    xmlNode.append_attribute(kszAttrServer, m_strServer.c_str());

    if (m_nDtmfType != -1)
    {
        strTmp = commonAPI_FormatString("%d", m_nDtmfType);
        xmlNode.append_attribute(kszAttrDtmfType, strTmp.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrDtmfType, "");
    }

    if (m_nDtmfInfoType != -1)
    {
        strTmp = commonAPI_FormatString("%d", m_nDtmfInfoType);
        xmlNode.append_attribute(kszAttrDtmfInfoType, strTmp.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrDtmfInfoType, "");
    }

    if (m_nDtmfPayload != -1)
    {
        strTmp = commonAPI_FormatString("%d", m_nDtmfPayload);
        xmlNode.append_attribute(kszAttrDtmfPayload, strTmp.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrDtmfPayload, "");
    }

    if (m_nKeepAlive != -1)
    {
        strTmp = commonAPI_FormatString("%d", m_nKeepAlive);
        xmlNode.append_attribute(kszAttrKeeplive, strTmp.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrKeeplive, "");
    }

    if (m_nTransport != -1)
    {
        strTmp = commonAPI_FormatString("%d", m_nTransport);
        xmlNode.append_attribute(kszAttrTransport, strTmp.c_str());
    }
    else
    {
        xmlNode.append_attribute(kszAttrTransport, "");
    }

    strTmp = commonAPI_FormatString("%d", m_nSrtp);
    xmlNode.append_attribute(kszAttrSrtp, strTmp.c_str());

    strTmp = commonAPI_FormatString("%d", m_nPort);
    xmlNode.append_attribute(kszAttrPort, strTmp.c_str());

    strTmp = commonAPI_FormatString("%d", m_nBfcp);
    xmlNode.append_attribute(kszAttrBfcp, strTmp.c_str());

    strTmp = commonAPI_FormatString("%d", m_nFecc);
    xmlNode.append_attribute(kszAttrFecc, strTmp.c_str());

    strTmp = commonAPI_FormatString("%d", m_nExtvaddonce);
    xmlNode.append_attribute(kszAttrExtvaddonce, strTmp.c_str());

    strTmp = commonAPI_FormatString("%d", m_nExpires);
    xmlNode.append_attribute(kszAttrExpires, strTmp.c_str());

    Print();

    return true;
}

void CloudAccountInfo::Print()
{
    ACCOUNT_INFO("id[%d] type[%d] ser[%s] user[%s] label[%s] reg[%s]", m_nAccountId, m_eType,
                 m_strServer.c_str(),
                 m_strUserName.c_str(), m_strLabel.c_str(), m_strRegName.c_str());

    ACCOUNT_INFO("transport[%d] dtmftype[%d] dtmfinfo[%d] dtmfpayload[%d] keep[%d] srtp[%d] state[%d]",
                 m_nTransport, m_nDtmfType,
                 m_nDtmfInfoType, m_nDtmfPayload, m_nKeepAlive, m_nSrtp, m_eStatus);

    ACCOUNT_INFO("port[%d] bfcp[%d] fecc[%d] extadd[%d] expires[%d]", m_nPort, m_nBfcp, m_nFecc,
                 m_nExtvaddonce, m_nExpires);

    return;
}