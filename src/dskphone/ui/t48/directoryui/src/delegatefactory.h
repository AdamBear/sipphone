#ifndef __DELEGATE_FACTORY_H__
#define __DELEGATE_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "groupdelegate.h"
#include "directorylistdelegate.h"

class CDelegateFactory
{
    DECLARE_SINGLETON_CLASS(CDelegateFactory)

public:
    // 获取群组数据代理
    CGroupDelegatePtr FindGroupDelegate(int nType);
    // 创建群组数据代理
    CGroupDelegatePtr GetGroupDelegate(int nType);
    // 获取联系人数据代理
    CDirectoryListDelegatePtr FindContactDelegate(int nType);
    // 创建联系人数据代理
    CDirectoryListDelegatePtr GetContactDelegate(int nType);

    void Clear();

private:
    typedef YLHashMap<int, CGroupDelegatePtr> MAP_GROUP_DELEGATE;
    typedef YLHashMap<int, CDirectoryListDelegatePtr> MAP_CONTACT_DELEGATE;

    // 列表框代理
    MAP_GROUP_DELEGATE m_mapGroupDelegate;
    MAP_CONTACT_DELEGATE m_mapContactDelegate;
};

#define g_DirDelegateFactory GET_SINGLETON_INSTANCE(CDelegateFactory)

#endif
