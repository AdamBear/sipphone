#include "cbasehelper.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"

bool CBaseHelper::m_bNotifXSIChange = false;

XSIAvailableMap CBaseHelper::m_map;
XSIAvailableMap CBaseHelper::m_mapAuthSuc;

//帐号消息回调函数
static LRESULT OnXsiAvailable(msgObject & objMessage)
{
    if (objMessage.message == CONFIG_MSG_BCAST_CHANGED)
    {
        XSI_INFO("OnXsiAvailable wparam[%d], lparam[%d]",
                 objMessage.wParam, objMessage.lParam);

        switch (objMessage.wParam)
        {
        case ST_XSI_CONFIGE:
            {
                CBaseHelper::SetXsiAvailable(true, objMessage.lParam);
            }
            break;
        case ST_XSI_TYPE:
            {
                CBaseHelper::SetXsiAllAccountAvailable(true);
            }
            break;
        default:
            break;
        }
    }

    return 1;
}

void XSI_BaseHelp_Init()
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnXsiAvailable);
}

CBaseHelper::CBaseHelper()
{
}

CBaseHelper::~CBaseHelper()
{
}

void CBaseHelper::SetXSINotif(bool bOn)
{
    m_bNotifXSIChange = bOn;
    if (bOn)
    {
        //修改密码之后直接先开启XSI可用标志
        //暂时只需通知账号0
        SetXsiAvailable(true);
    }
}

void CBaseHelper::ProcessCallback(NS_TA::CTaskAction * pTaskAction, int nLineId,
                                  XSI_SEND_TYPE eSend/* = XSI_UNKNOW_SEND*/)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    bool isAuthFail = (pTaskAction->GetTaskActionErrorType() == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED);

    bool suc = pTaskAction->IsOperationSucceed();

    XSIAvailableMap::iterator iter = m_map.find(nLineId);

    if (iter == m_map.end())
    {
        m_map[nLineId] = false;

        iter = m_map.find(nLineId);
    }

    if (iter == m_map.end())
    {
        return;
    }

    if (!isAuthFail && !iter->second)
    {
        iter->second = true;
    }
    else if (isAuthFail && iter->second)
    {
        iter->second = false;
    }

    // 回调成功才设置 不能复用之前的xsi鉴权 之前的在UI点击时候就设置成功
    XSIAvailableMap::iterator iterAuth = m_mapAuthSuc.find(nLineId);
    if (iterAuth == m_mapAuthSuc.end())
    {
        m_mapAuthSuc.put(nLineId, iter->second);
    }
    else
    {
        iterAuth->second = iter->second;
    }

    if (m_bNotifXSIChange)
    {
        if (suc)
        {
            etl_NotifyAppEx(false, CONFIG_MSG_BCAST_CHANGED, ST_XSI_CONFIGE, nLineId, sizeof(eSend), &eSend);
        }
        SetXSINotif(false);
    }
}

bool CBaseHelper::SetXsiAvailable(bool bXsiAvailable, int nLine/* = 0*/)
{
    XSIAvailableMap::iterator iter = m_map.find(nLine);

    if (iter != m_map.end())
    {
        iter->second = bXsiAvailable;
        return true;
    }
    return false;
}

void CBaseHelper::SetXsiAllAccountAvailable(bool bXsiAvailable)
{
    for (XSIAvailableMap::iterator iter = m_map.begin(); iter != m_map.end(); iter++)
    {
        iter->second = bXsiAvailable;
    }
}

XSI_STATUS CBaseHelper::GetXSIStatus(int nLineId)
{
    bool bXsiAvailable = true;

    XSIAvailableMap::iterator iter = m_map.find(nLineId);

    if (iter != m_map.end())
    {
        bXsiAvailable = iter->second;
    }

    if (!bXsiAvailable)
    {
        if (acc_IsAccountAvailable(nLineId)
                && configParser_GetConfigInt(kszAllowSipAuth) == 1)
        {
            return XSI_SIPACCOUNT_NOT_AVAILABLE;
        }

        return XSI_AUTH_ERROR;
    }
    else
    {
        return XSI_SUCCESS;
    }
}

bool CBaseHelper::IsXsiAvailable(int nLineId/* = 0*/)
{
    XSIAvailableMap::iterator iter = m_map.find(nLineId);
    if (iter == m_map.end())
    {
        return false;
    }

    bool bXsiAvailable = iter->second;
    // 鉴权之后才有鉴权信息
    XSIAvailableMap::iterator iterAuth = m_mapAuthSuc.find(nLineId);
    if (iterAuth != m_mapAuthSuc.end())
    {
        bXsiAvailable = iter->second && iterAuth->second;
    }

    return bXsiAvailable;
}

//获得子节点的数据 string型
bool GetXmlChildText(const xml_node & xmlRoot, const yl::string & strChildName, yl::string & strOut)
{
    //获得子节点
    xml_node xmlChild = xmlRoot.child(strChildName.c_str());
    if (xmlChild.empty())
    {
        return false;
    }

    strOut = xmlChild.text().as_string();
    return true;
}

//获得子节点的数据 bool型
bool GetXmlChildText(const xml_node & xmlRoot, const yl::string & strChildName, bool & bOut)
{
    //获得子节点
    xml_node xmlChild = xmlRoot.child(strChildName.c_str());
    if (xmlChild.empty())
    {
        return false;
    }
    bOut = xmlChild.text().as_bool();
    return true;
}

//增加一个带有Text的子节点
bool PutXmlChildText(xml_node & xmlRoot, const yl::string & strChildName, const yl::string & strIn)
{
    //获得子节点
    xml_node xmlChild = xmlRoot.append_child(strChildName.c_str());
    xmlChild.append_child(node_pcdata).set_value((const char *)strIn.c_str());
    return true;
}

//增加一个带有Text的子节点
bool PutXmlChildBool(xml_node & xmlRoot, const yl::string & strChildName, bool bIn)
{
    return PutXmlChildText(xmlRoot, strChildName, bIn ? "true" : "false");
}

