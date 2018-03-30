#ifndef MULTICAST_PAGING_LIST_CONTROLLER_H_
#define MULTICAST_PAGING_LIST_CONTROLLER_H_

#include "feature/include/featurecommon.h"
#include <ETLLib.hpp>
#include <vector>

// 定义结构体数组
// typedef std::vector<PagingGroupInfo> PagingListVec;

class CMulticastPagingListController
{
public:
    CMulticastPagingListController(void);
    ~CMulticastPagingListController(void);

    static CMulticastPagingListController * GetInstance();

public:
    // 获取Paging List
    bool GetPagingList(PagingListVec & groupList);

    // 根据配置项项数获取相应数据
    const PagingGroupInfo * GetPagingByConfigId(int iConfigId);

    // 修改某个Paging数据
    bool SaveGroupInfo(PagingGroupInfo groupInfo);

    // 删除Paging
    bool DeleteGroup(int iConfigId);

    // 删除所有配置
    bool DeleteAllGroup();

    // 消息响应
    static LRESULT OnMessage(msgObject & refObj);

private:
    // 加载配置
    void LoadPagingList();

private:
    // paginglist 列表
    PagingListVec m_vecPagingList;

};

#define g_pPagingListController (CMulticastPagingListController::GetInstance())

#endif // MULTICAST_PAGING_LIST_CONTROLLER_H_
