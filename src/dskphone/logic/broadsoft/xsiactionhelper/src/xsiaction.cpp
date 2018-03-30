#include "../include/xsiaction.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include <commonapi/stringhelperapi.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"

using namespace NS_TA;

#define BROADSOFT_XSICMD_PREFEX "%s:%d/com.broadsoft.xsi-actions/%s/user/%s/%s"

#define BROADSOFT_NEW_XSICMD_PREFEX "%s:%d/%s/%s/user/%s/%s"

#define BROADSOFT_DEFAULT_URL "com.broadsoft.xsi-actions"

const yl::string kszEmptyString = "";

XsiUrlMap XsiAction::m_urlMap;


bool XsiAction::Exec()
{
    if (!SetMethed())
    {
        SetTaskActionErrorType(TA_OTHER_ERROR);
        return false;
    }

    CNetworkFileAction::Exec();
    return true;
    //to do-syl
}

XsiAction::XsiAction(int nLineId, eXsiActionType eType, const yl::string & strExtUrl/* = ""*/,
                     const yl::string & strLocalFile/* = ""*/)
    : CNetworkFileAction("", "")

{
    m_eType = eType;

    m_nLineID = nLineId;

    m_bIsSipXsi = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

    yl::string strXSIUserId = configParser_GetCfgItemStringValue(kszAccountXSIUserID, nLineId);

    if (m_bIsSipXsi)
    {
        //userId为sip UserName  passwordc存的是userID：sip passward
        m_strUserId = configParser_GetCfgItemStringValue(kszAccountRegisterName, nLineId);

        yl::string strPassword = strXSIUserId;
        strPassword.append(":").append(configParser_GetCfgItemStringValue(kszAccountPassword, nLineId));

        m_strPassword = strPassword.c_str();
    }
    else
    {
        m_strUserId = strXSIUserId.c_str();
        m_strPassword = configParser_GetCfgItemStringValue(kszAccountXSIPassword, nLineId);
    }

    //获取服务器网址
    yl::string strtemp = configParser_GetCfgItemStringValue(kszAccountXSIHostAddr, nLineId);

    //获取服务器类型
    yl::string strServerType = configParser_GetCfgItemStringValue(kszAccountXSIServerType, nLineId);
    yl::string strHost = commonAPI_FormatString("%s://%s", strServerType.c_str(), strtemp.c_str());

    //获取端口

    int uPort = configParser_GetCfgItemIntValue(kszAccountXSIPort, nLineId);

    if (0 == uPort)
    {
        uPort = 80;
    }

    yl::string strVirsion = configParser_GetCfgItemStringValue(kszAccountXSIVersion, nLineId);

    if (strVirsion.empty())
    {
        strVirsion = "v2.0";
    }

    yl::string strCustomURL = configParser_GetCfgItemStringValue(kszAccountXSICustomURL, nLineId);

    if (strCustomURL.empty())
    {
        strCustomURL = BROADSOFT_DEFAULT_URL;
    }


    //加上端口
    //strtemp = commonAPI_FormatString(BROADSOFT_XSICMD_PREFEX, strHost.c_str(), uPort,strVirsion.c_str(),strXSIUserId.c_str(),GetAppUrlByType(eType).c_str());
    strtemp = commonAPI_FormatString(BROADSOFT_NEW_XSICMD_PREFEX, strHost.c_str(), uPort,
                                     strCustomURL.c_str(), strVirsion.c_str(), strXSIUserId.c_str(), GetAppUrlByType(eType).c_str());

    //设置服务器地址
    m_strFilePathRemote = strtemp;

    XSI_INFO("XSI INFO, RemoteURL[%s], LineId[%d]", m_strFilePathRemote.c_str(), nLineId);

    if (!strExtUrl.empty())
    {
        m_strFilePathRemote.append(strExtUrl);

        m_strExtenUrl = strExtUrl;
    }

    //设置本地文件
    SetFilePath(strLocalFile.empty() ? GetRandomFile() : strLocalFile);

    SetUsername(m_strUserId);

    SetPassword(m_strPassword);
    //设置XSI 模式
    SetXSIAuthMode(m_bIsSipXsi);

    // xsi方式才需要
    if (!m_bIsSipXsi
            && CBaseHelper::IsXsiAvailable(nLineId))
    {
        SetDownLoadMode(MODE_BASIC_XSI_AUTH);
    }

}

yl::string XsiAction::GetRandomFile()
{
    static unsigned int id = 0;

    yl::string strFilename ;

    strFilename = commonAPI_FormatString(TEMP_PATH "xsiaction_cmd%d", id++);

    if (id > 20)
    {
        id = 0;
    }
    return strFilename;
}

bool XsiAction::SetMethed()
{
    bool suc = true;
    if (m_eType > XSI_GET_BEGIN && m_eType < XSI_GET_END)
    {
        SetDelete(false);
        SetRead(true);  // 读取文件.
    }
    else if (m_eType > XSI_DELETE_BEGIN && m_eType < XSI_DELETE_END)
    {
        SetRead(false);
        SetDelete(true);
    }
    else if (m_eType > XSI_POST_BEGIN && m_eType < XSI_POST_END)
    {
        SetDelete(false);
        SetRead(false);  // 写文件.
        SetPut(false);
    }
    else if (m_eType > XSI_PUT_BEGIN && m_eType < XSI_PUT_END)
    {
        SetDelete(false);
        SetRead(false);  // 写文件.
        SetPut(true);
    }
    else
    {
        suc = false;
    }

    return suc;
}

void XsiAction::InsertMap(eXsiActionType eType, const yl::string & strAppUrl)
{
    m_urlMap[eType] = strAppUrl;
}

const yl::string & XsiAction::GetAppUrlByType(eXsiActionType eType)
{
    XsiUrlMap::iterator iter = m_urlMap.find(eType);

    if (iter != m_urlMap.end())
    {
        return iter->second;
    }
    return kszEmptyString;
}

void XsiAction::SetChangeData(bool bChangeData)
{
    m_bExeChangeData = bChangeData;
}
