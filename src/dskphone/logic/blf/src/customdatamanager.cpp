#include "customdatamanager.h"
#include "blflistmsgdefine.h"
#include "blf_inc.h"

#define BLF_CUSTOM_DATA_PATH CONFIG_DATA_PATH "blfcustomaction.xml"
#define NODE_BLF                "blf"


LRESULT OnCustomDataMsg(msgObject & msg)
{
    bool bHandled = true;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_BLF_CUSTOM_DATA == msg.wParam)
            {
                g_objBLFCustomDataManager.StartUpdateTime();
            }
        }
        break;
    case TM_TIMER:
        {
            g_objBLFCustomDataManager.OnTimer(msg.wParam);
        }
        break;
    default:
        break;
    }

    return 1;
}

CBLFCustomDataManager & CBLFCustomDataManager::GetInstance()
{
    static CBLFCustomDataManager Instace;
    return Instace;
}

CBLFCustomDataManager::CBLFCustomDataManager()
{
    m_iTimerLoad = 300;

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnCustomDataMsg);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnCustomDataMsg);
}

CBLFCustomDataManager::~CBLFCustomDataManager()
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnCustomDataMsg);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnCustomDataMsg);
}

void CBLFCustomDataManager::StartUpdateTime()
{
    timerSetThreadTimer((UINT)&m_iTimerLoad, m_iTimerLoad);
}

// 处理定时器消息
bool CBLFCustomDataManager::OnTimer(UINT uTimerID)
{
    if (uTimerID != (UINT)&m_iTimerLoad)
    {
        return false;
    }

    timerKillThreadTimer((UINT)&m_iTimerLoad);

    LoadFromConfigFiles();

    // 通知BLF模块, 重新刷新Dsskey
    etl_NotifyApp(false, BLF_CUSTOM_DATA_CHANAGE, 0, 0);

    return true;
}

bool CBLFCustomDataManager::LoadFromConfigFiles()
{
    m_lstCustomData.clear();

    xml_document doc;
    if (doc.load_file(BLF_CUSTOM_DATA_PATH).status != status_ok)
    {
        APP_INFO("Load: %s failed", BLF_CUSTOM_DATA_PATH);
        return false;
    }

    xml_node nodeRoot = doc.child(NODE_BLF);
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node pItemNode;
    pItemNode = nodeRoot.first_child();

    // 遍历节点item
    while (pItemNode)
    {
        CBLFCustomData refData;
        refData.ParseFromXmlNode(pItemNode);
        m_lstCustomData.push_back(refData);

        pItemNode = pItemNode.next_sibling();
    }

    return true;
}

int CBLFCustomDataManager::GetPriority(Dsskey_Status eDssStatus)
{
    yl::string strDssStatus = MapMonitorStatus(eDssStatus);

    CBLFCustomData * pData = FindCustomData(strDssStatus);
    if (NULL != pData && pData->IsEnableCustom())
    {
        return pData->GetPriority();
    }

    return -1;
}

yl::string CBLFCustomDataManager::GetLedInfo(Dsskey_Status eDssStatus)
{
    yl::string strDssStatus = MapMonitorStatus(eDssStatus);

    CBLFCustomData * pData = FindCustomData(strDssStatus);
    if (NULL != pData && pData->IsEnableCustom())
    {
        return pData->GetLedInfo();
    }

    return "";
}

eBLFOperateType CBLFCustomDataManager::GetOperationType(Dsskey_Status eDssStatus,
        SESSION_STATE eSessState)
{
    yl::string strDssStatus = MapMonitorStatus(eDssStatus);

    CBLFCustomData * pData = FindCustomData(strDssStatus);
    if (NULL != pData && pData->IsEnableCustom())
    {
        yl::string strSessState = MapSessionStatus(eSessState);
        return pData->GetOperationType(strSessState);
    }

    return OPERATE_BLF_TYPE_NONE;
}

yl::string CBLFCustomDataManager::GetOperationValue(Dsskey_Status eDssStatus,
        SESSION_STATE eSessState)
{
    yl::string strDssStatus = MapMonitorStatus(eDssStatus);

    CBLFCustomData * pData = FindCustomData(strDssStatus);
    if (NULL != pData && pData->IsEnableCustom())
    {
        yl::string strSessState = MapSessionStatus(eSessState);
        return pData->GetOperationValue(strSessState);
    }

    return "";
}

CBLFCustomData * CBLFCustomDataManager::FindCustomData(const yl::string & strStatus)
{
    if (strStatus.empty())
    {
        return NULL;
    }

    YLList<CBLFCustomData>::ListIterator itr = m_lstCustomData.begin();
    for (; itr != m_lstCustomData.end(); itr++)
    {
        CBLFCustomData & data = *itr;
        if (data.GetStatus().compare(strStatus) == 0)
        {
            return &(*itr);
        }
    }

    return NULL;
}

yl::string CBLFCustomDataManager::MapMonitorStatus(Dsskey_Status eDssStatus)
{
    switch (eDssStatus)
    {
    case DS_BLF_STATUS_IDLE:
        return "idle";
        break;
    case DS_BLF_STATUS_RINGING:
        return "callin";
        break;
    case DS_BLF_STATUS_TALKING:
        return "talking";
        break;
    case DS_BLF_STATUS_UNKOWN:
        return "unknown";
        break;
    case DS_BLF_STATUS_CALLOUT:
        return "callout";
        break;
    case DS_BLF_STATUS_PARK:
        return "parked";
        break;
    default:
        return "";
        break;
    }
}

yl::string CBLFCustomDataManager::MapSessionStatus(SESSION_STATE eSessState)
{
    switch (eSessState)
    {
    case SESSION_UNKNOWN:
        return "idle";
        break;
    case SESSION_RINGING:
        return "callin";
        break;
    case SESSION_TALKING:
        return "talking";
        break;
    case SESSION_RINGBACK:
    case SESSION_CONNECTING:
    case SESSION_CONNECTING_TRAN:
    case SESSION_RINGBACK_TRAN:
        return "callout";
        break;
    case SESSION_PARK_NOTIFY:
        return "parked";
        break;
    default:
        return "";
        break;
    }
}
