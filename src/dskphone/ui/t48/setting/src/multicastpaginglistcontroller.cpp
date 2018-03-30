#include "multicastpaginglistcontroller.h"

#include "configiddefine.h"
#include "interfacedefine.h"
#include "configparser/modconfigparser.h"
#include "setting/include/messagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"

// Multicast Paging List个数
#define MAX_PAGING_LIST_NUMBER  31//10

CMulticastPagingListController * m_pPagingList = NULL;
CMulticastPagingListController * CMulticastPagingListController::GetInstance()
{
    if (NULL == m_pPagingList)
    {
        m_pPagingList = new CMulticastPagingListController();
    }

    return m_pPagingList;
}

LRESULT CMulticastPagingListController::OnMessage(msgObject & refObj)
{
    switch (refObj.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_MULTICAST == refObj.wParam)
            {
                // 加载配置文件
                g_pPagingListController->LoadPagingList();

                // 通知UI刷新
                etl_NotifyApp(true, MULTICAST_MESSAGE_PAGING_LIST_CHANGED, 0, 0);
            }
        }
        break;
    default:
        break;
    }

    return 1;
}

CMulticastPagingListController::CMulticastPagingListController(void)
{
    // 加载列表
    LoadPagingList();

    // 注册消息.
    etl_RegisterMsgHandle(
        CONFIG_MSG_BCAST_CHANGED,
        CONFIG_MSG_BCAST_CHANGED,
        CMulticastPagingListController::OnMessage);
}

CMulticastPagingListController::~CMulticastPagingListController(void)
{
    etl_UnregisterMsgHandle(
        CONFIG_MSG_BCAST_CHANGED,
        CONFIG_MSG_BCAST_CHANGED,
        CMulticastPagingListController::OnMessage);
}

// 加载列表
void CMulticastPagingListController::LoadPagingList()
{
    m_vecPagingList.clear();

    for (int i = 1 ; i <= MAX_PAGING_LIST_NUMBER; ++i)
    {
        PagingGroupInfo sPagingGroup;
        sPagingGroup.iIndex = i;

        sPagingGroup.strLabel = configParser_GetCfgItemStringValue(kszPagingAddressLabel, i);

        sPagingGroup.strAddress = configParser_GetCfgItemStringValue(kszPagingAddressIPAddress, i);

        sPagingGroup.iChannel = configParser_GetCfgItemIntValue(kszPagingAddressChannel, i);

        m_vecPagingList.push_back(sPagingGroup);
    }
}


bool CMulticastPagingListController::GetPagingList(PagingListVec & groupList)
{
    groupList.clear();

    for (int i = 0; i < m_vecPagingList.size(); ++i)
    {
        groupList.push_back(m_vecPagingList[i]);
    }

    return true;
}

const PagingGroupInfo * CMulticastPagingListController::GetPagingByConfigId(int iConfigId)
{
    if (iConfigId < 1 || iConfigId > m_vecPagingList.size())
    {
        return NULL;
    }

    return &m_vecPagingList[iConfigId - 1];
}

bool CMulticastPagingListController::SaveGroupInfo(PagingGroupInfo groupInfo)
{
    if (groupInfo.iIndex < 1 || groupInfo.iIndex > m_vecPagingList.size())
    {
        return false;
    }

    // 数据有变化才保存
    if (m_vecPagingList[groupInfo.iIndex - 1].strAddress != groupInfo.strAddress
            || m_vecPagingList[groupInfo.iIndex - 1].strLabel != groupInfo.strLabel
            || m_vecPagingList[groupInfo.iIndex - 1].iChannel != groupInfo.iChannel)
    {
        m_vecPagingList[groupInfo.iIndex - 1].strAddress = groupInfo.strAddress;
        m_vecPagingList[groupInfo.iIndex - 1].strLabel = groupInfo.strLabel;
        m_vecPagingList[groupInfo.iIndex - 1].iChannel = groupInfo.iChannel;

        configParser_SetCfgItemStringValue(kszPagingAddressLabel, groupInfo.iIndex,
                                           groupInfo.strLabel.c_str(), CONFIG_LEVEL_PHONE);

        configParser_SetCfgItemStringValue(kszPagingAddressIPAddress, groupInfo.iIndex,
                                           groupInfo.strAddress.c_str(), CONFIG_LEVEL_PHONE);

        configParser_SetCfgItemIntValue(kszPagingAddressChannel, groupInfo.iIndex, groupInfo.iChannel,
                                        CONFIG_LEVEL_PHONE);
    }

    return true;
}

bool CMulticastPagingListController::DeleteGroup(int iConfigId)
{
    if (iConfigId < 1 || iConfigId > m_vecPagingList.size())
    {
        return false;
    }

    // 数据不为空时才清除
    // 数据不为空时才清除
    //http://10.2.1.199/Bug.php?BugID=96509 同步T46的实现 之前没有清除channel的信息
    if (!m_vecPagingList[iConfigId - 1].strLabel.empty())
    {
        m_vecPagingList[iConfigId - 1].strLabel.clear();
#ifdef _T49
        configParser_SetCfgItemStringValue(kszPagingAddressLabel, iConfigId - 1, "", true);
#else
        configParser_SetCfgItemStringValue(kszPagingAddressLabel, iConfigId, "", CONFIG_LEVEL_PHONE);
#endif
    }

    if (!m_vecPagingList[iConfigId - 1].strAddress.empty())
    {
        m_vecPagingList[iConfigId - 1].strAddress.clear();
#ifdef _T49
        configParser_SetCfgItemStringValue(kszPagingAddressIPAddress, iConfigId - 1, "", true);
#else
        configParser_SetCfgItemStringValue(kszPagingAddressIPAddress, iConfigId, "", CONFIG_LEVEL_PHONE);
#endif
    }

    if (m_vecPagingList[iConfigId - 1].iChannel != 0)
    {
        m_vecPagingList[iConfigId - 1].iChannel = 0;
#ifdef _T49
        configParser_SetCfgItemIntValue(kszPagingAddressChannel, iConfigId - 1, 0, true);
#else
        configParser_SetCfgItemIntValue(kszPagingAddressChannel, iConfigId, 0, CONFIG_LEVEL_PHONE);
#endif
    }

    return true;
}

bool CMulticastPagingListController::DeleteAllGroup()
{
    for (int i = 1 ; i <= m_vecPagingList.size(); ++i)
    {
        DeleteGroup(i);
    }

    return true;
}


