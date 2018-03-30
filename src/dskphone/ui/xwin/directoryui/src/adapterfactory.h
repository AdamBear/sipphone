#ifndef __ADAPTER_FACTORY_H__
#define __ADAPTER_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "directoryadapter.h"


class CAdapterFactory
{
    DECLARE_SINGLETON_CLASS(CAdapterFactory)

public:
    // 获取联系人数据代理
    CDirectoryAdapterPtr FindContactAdapter(int nType);
    // 创建联系人数据代理
    CDirectoryAdapterPtr GetContactAdapter(int nType);

    void Clear();

private:
    typedef YLHashMap<int, CDirectoryAdapterPtr> MAP_CONTACT_ADAPTER;

    // 列表框代理
    MAP_CONTACT_ADAPTER m_mapContactAdapter;
};

#define g_AdapterFactory GET_SINGLETON_INSTANCE(CAdapterFactory)

#endif
