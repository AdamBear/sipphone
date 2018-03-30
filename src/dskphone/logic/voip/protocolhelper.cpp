#include "protocolhelper.h"
#include "threadtask/include/modthreadtask.h"
#include "threadtask/include/networkfileaction.h"
#include "service_system.h"

#include "ETLLib.hpp"
#include "interfacedefine.h"
#include "devicelib/networkserver.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include <ylstring.h>
#include <ylstringalgorithm.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "commonfunctions.h"
#include "idlescreen/include/modidlescreen.h"

#define kszFileSavedName  TEMP_PATH "NatDynamic.xml"


CProtocolHelper::CProtocolHelper()
{
    m_nNatType = 0;
}

CProtocolHelper::~CProtocolHelper()
{

}

CProtocolHelper & CProtocolHelper::GetInstance()
{
    static CProtocolHelper objHelper;
    return objHelper;
}

bool CProtocolHelper::Init()
{
    // 初始化
    SingleMsgReg(SYS_MSG_NOTIFY_NETWORK_STATUS, CProtocolHelper::ProcessMsg);

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CProtocolHelper::ProcessMsg);

    // 重启必须重新去请求一次: 网络可能变更
    ProcessNatUpdate(true);
    return true;
}

void CProtocolHelper::ReqDynamicNat()
{
    COMMONUNITS_INFO("Req public ip[%d][%s]", m_nNatType, m_strPublicRequest.c_str());
    if (!netIsNetworkReady()
            || m_nNatType != 2)
    {
        return;
    }

    CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(kszFileSavedName,
                                   m_strPublicRequest.c_str());
    if (pAction != NULL)
    {
        pAction->SetRead(true);  // 读取文件.
        pAction->SetCallback(this);  // 回调函数.
        pAction->SetSync(false);  // 异步.

        bool bRet = TaskAction_ExecAction(pAction);

        if (!bRet)
        {
            delete pAction;
        }
    }

    // 其他情况, 则下载失败.
    return ;
}

void CProtocolHelper::ProcessNatUpdate(bool bForce/* = false*/)
{
    int nNatType = configParser_GetConfigInt(kszNetworkEnableStaticNAT);
    yl::string strUrl = configParser_GetConfigString(kszNetworkNATRequestAdds);

    if (nNatType == 0)
    {
        idleScreen_DropStatus(PS_STATE_NATWORK_NAT);
    }
    else
    {
        idleScreen_SetStatus(PS_STATE_NATWORK_NAT, "");
    }

    COMMONUNITS_INFO("Req nat add type[%d] url[%s]", nNatType, strUrl.c_str());
    if (m_nNatType != nNatType
            || m_strPublicRequest != strUrl
            || bForce)
    {
        m_nNatType = nNatType;
        m_strPublicRequest = strUrl;
        ReqDynamicNat();
    }

    return;
}

bool CProtocolHelper::ParseInfo(const yl::string & strPath, yl::string & strUrl)
{
    if (!pathFileExist(strPath.c_str()))
    {
        return false;
    }

    yl::string strSouce = yl::dump_file(strPath);

#if 0
    printf("-------[%s]\n", strSouce.c_str());
#endif //

    int nIndex1 = strSouce.find("is:");
    //第一个xml结束位置
    int nIndex2 = strSouce.find("</pre>");

    if (nIndex1 != strSouce.npos
            && nIndex2 != strSouce.npos)
    {
        strUrl = strSouce.substr(nIndex1 + 3, nIndex2 - nIndex1 - 3);
        strUrl.replace("\n", "");
        strUrl.replace("\r", "");
    }

    return false;
}

void CProtocolHelper::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    COMMONUNITS_INFO("Protocol Helper On TaskActionCallback begin");
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        COMMONUNITS_INFO("Req type[%d]", pTaskAction->GetReqType());
        return;
    }

    // 返回结果的类型为请求网络文件.
    CNetworkFileAction * pNetworkAction =
        dynamic_cast<CNetworkFileAction *>(pTaskAction);
    if (pNetworkAction == NULL)
    {
        return;
    }

    //Post
    if (!pNetworkAction->IsRead())
    {
        return;
    }

    if (!pNetworkAction->IsOperationSucceed())
    {
        configParser_SetConfigString(kszNetworkNATDynamicIPAdds, "");
        return;
    }

    yl::string strUrl;
    ParseInfo(kszFileSavedName, strUrl);

    COMMONUNITS_INFO("Public ip[%s]", strUrl.c_str());
    bool bChanged = configParser_SetConfigString(kszNetworkNATDynamicIPAdds, strUrl.c_str());
    bChanged |= configParser_SetConfigString(kszNetworkNATPubIPAdds, strUrl.c_str());

    //m_strPublicIP = strUrl;
    if (bChanged)
    {
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_VPPHONE, 0);
    }

    remove(pNetworkAction->GetFilePath().c_str());

    return;
}

LRESULT CProtocolHelper::ProcessMsg(msgObject & msg)
{
    if (msg.message == SYS_MSG_NOTIFY_NETWORK_STATUS)
    {
        _ProtocolHelper.ReqDynamicNat();
    }
    else if ((msg.message == CONFIG_MSG_BCAST_CHANGED
              && msg.wParam == ST_ADV_SETTING))
    {
        _ProtocolHelper.ProcessNatUpdate();
    }

    return 1;
}
