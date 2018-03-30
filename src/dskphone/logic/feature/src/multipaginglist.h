#if IF_FEATURE_PAGING
#ifndef MULTI_PAGING_LIST_H_
#define MULTI_PAGING_LIST_H_

#include <singletonclassdefine.h>
#include "../include/featurecommon.h"

class CMultiPagingList
{
    DECLARE_SINGLETON_CLASS(CMultiPagingList)

public:
    // 初始化数据
    void Init();

    // 获取Paging List
    bool GetPagingList(PagingListVec & pagingList);

    // 根据配置项项数获取相应数据
    const PagingGroupInfo * GetPagingByConfigId(int iConfigId);

    // 修改某个Paging数据
    bool SaveGroupInfo(const PagingGroupInfo & groupInfo);

    // 删除Paging
    bool DeleteGroup(int iConfigId);

    // 删除所有配置
    bool DeleteAllGroup();

    // 注册/注销回调函数
    void InitCallBack(PagingListCallBack pCallBack, bool bRegister);

private:
    // 加载配置
    void LoadPagingList();

    // 消息响应
    static LRESULT OnMessage(msgObject & refObj);

private:
    // paginglist 列表
    PagingListVec m_vecPagingList;

    // 界面刷新的回调函数
    PagingListCallBack m_pCallBack;

};

#define _MultiPagingList GET_SINGLETON_INSTANCE(CMultiPagingList)

#endif // MULTI_PAGING_LIST_H_

#endif //IF_FEATURE_PAGING