#ifndef __SETTING_ADAPTER_FACTORY_H__
#define __SETTING_ADAPTER_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "settinguiadapter.h"

typedef CSettingUIAdapter * (*FUN_CREATE_SETTING_ADAPTER)();
typedef YLHashMap<yl::string, FUN_CREATE_SETTING_ADAPTER> MAP_CREATE_SETTING_ADAPTER_FUN;

class CSettingAdapterFactory
{
    DECLARE_SINGLETON_CLASS(CSettingAdapterFactory);

public:
    void RegisterSettingAdapter(const yl::string & strPageID,
                                FUN_CREATE_SETTING_ADAPTER pFunCreateSettingAdapter);

    CSettingUIAdapter * GetSettingAdapter(const yl::string & strPageID);

    CSettingUIAdapter * GetDefaultSettingAdapter();

protected:
    MAP_CREATE_SETTING_ADAPTER_FUN  m_mapCreateSettingAdapter;
};

#define g_SettingAdapterFactory GET_SINGLETON_INSTANCE(CSettingAdapterFactory)

#endif // __SETTING_ADAPTER_FACTORY_H__

