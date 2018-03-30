#if IF_FEATURE_PAGING
#include "feature_inc.h"

IMPLEMENT_GETINSTANCE(CMultiPagingList)

// MultiPagingList默认支持个数
#define MAX_PAGING_LIST_NUMBER  31//10

CMultiPagingList::CMultiPagingList()
    : m_pCallBack(NULL)
{
}

CMultiPagingList::~CMultiPagingList()
{
}

LRESULT CMultiPagingList::OnMessage(msgObject & refObj)
{
    switch (refObj.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_MULTICAST == refObj.wParam)
            {
                // 加载配置文件
                _MultiPagingList.LoadPagingList();
            }
        }
        break;
    default:
        break;
    }

    return 1;
}

// 初始化数据
void CMultiPagingList::Init()
{
    // 加载列表
    LoadPagingList();

    // 注册消息.
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CMultiPagingList::OnMessage);
}

// 加载列表
void CMultiPagingList::LoadPagingList()
{
    m_vecPagingList.clear();

    for (int i = 0 ; i < MAX_PAGING_LIST_NUMBER; ++i)
    {
        PagingGroupInfo sPagingGroup;

        sPagingGroup.iIndex = i + 1;

        int iTransfer = i + 1;

        sPagingGroup.strLabel = configParser_GetCfgItemStringValue(kszPagingAddressLabel, iTransfer);

        sPagingGroup.strAddress = configParser_GetCfgItemStringValue(kszPagingAddressIPAddress, iTransfer);

        sPagingGroup.iChannel = configParser_GetCfgItemIntValue(kszPagingAddressChannel, iTransfer);

        m_vecPagingList.push_back(sPagingGroup);
    }

    // 加载成功 通知UI刷新
    if (NULL != m_pCallBack)
    {
        m_pCallBack();
    }
}

bool CMultiPagingList::GetPagingList(PagingListVec & pagingList)
{
    pagingList.clear();

    for (int i = 0; i < m_vecPagingList.size(); ++i)
    {
        pagingList.push_back(m_vecPagingList[i]);
    }

    return true;
}

const PagingGroupInfo * CMultiPagingList::GetPagingByConfigId(int iConfigId)
{
    if (iConfigId < 1 || iConfigId > m_vecPagingList.size())
    {
        return NULL;
    }

    return &m_vecPagingList[iConfigId - 1];
}

bool CMultiPagingList::SaveGroupInfo(const PagingGroupInfo & groupInfo)
{
    if (groupInfo.iIndex < 1 || groupInfo.iIndex > m_vecPagingList.size())
    {
        return false;
    }

    // 数据有变化才保存
    if (m_vecPagingList[groupInfo.iIndex - 1].strLabel != groupInfo.strLabel)
    {
        m_vecPagingList[groupInfo.iIndex - 1].strLabel = groupInfo.strLabel;

        configParser_SetCfgItemStringValue(kszPagingAddressLabel, groupInfo.iIndex, groupInfo.strLabel,
                                           CONFIG_LEVEL_PHONE);

    }

    if (m_vecPagingList[groupInfo.iIndex - 1].strAddress != groupInfo.strAddress)
    {
        m_vecPagingList[groupInfo.iIndex - 1].strAddress = groupInfo.strAddress;

        configParser_SetCfgItemStringValue(kszPagingAddressIPAddress, groupInfo.iIndex,
                                           groupInfo.strAddress, CONFIG_LEVEL_PHONE);
    }

    if (m_vecPagingList[groupInfo.iIndex - 1].iChannel != groupInfo.iChannel)
    {
        m_vecPagingList[groupInfo.iIndex - 1].iChannel = groupInfo.iChannel;

        configParser_SetCfgItemIntValue(kszPagingAddressChannel, groupInfo.iIndex, groupInfo.iChannel,
                                        CONFIG_LEVEL_PHONE);

    }

    return true;
}

bool CMultiPagingList::DeleteGroup(int iConfigId)
{
    if (iConfigId < 1 || iConfigId > m_vecPagingList.size())
    {
        return false;
    }

    // 数据不为空时才清除
    if (!m_vecPagingList[iConfigId - 1].strLabel.empty())
    {
        m_vecPagingList[iConfigId - 1].strLabel.clear();

        configParser_SetCfgItemStringValue(kszPagingAddressLabel, iConfigId, "", CONFIG_LEVEL_PHONE);
    }

    if (!m_vecPagingList[iConfigId - 1].strAddress.empty())
    {
        m_vecPagingList[iConfigId - 1].strAddress.clear();

        configParser_SetCfgItemStringValue(kszPagingAddressIPAddress, iConfigId, "", CONFIG_LEVEL_PHONE);
    }

    if (m_vecPagingList[iConfigId - 1].iChannel != 0)
    {
        m_vecPagingList[iConfigId - 1].iChannel = 0;

        configParser_SetCfgItemIntValue(kszPagingAddressChannel, iConfigId, 0, CONFIG_LEVEL_PHONE);

    }


    return true;
}

bool CMultiPagingList::DeleteAllGroup()
{
    for (int i = 1 ; i <= m_vecPagingList.size(); ++i)
    {
        DeleteGroup(i);
    }

    return true;
}

// 注册/注销回调函数
void CMultiPagingList::InitCallBack(PagingListCallBack pCallBack, bool bRegister)
{
    if (bRegister)
    {
        m_pCallBack = pCallBack;
    }
    else
    {
        if (m_pCallBack == pCallBack)
        {
            m_pCallBack = NULL;
        }
    }
}

#endif //IF_FEATURE_PAGING