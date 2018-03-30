#include "settingadapterfactory.h"


IMPLEMENT_GETINSTANCE(CSettingAdapterFactory)

CSettingAdapterFactory::CSettingAdapterFactory()
{
}

CSettingAdapterFactory::~CSettingAdapterFactory()
{
}

void CSettingAdapterFactory::RegisterSettingAdapter(const yl::string & strPageID,
        FUN_CREATE_SETTING_ADAPTER pFunCreateSettingAdapter)
{
    if (NULL == pFunCreateSettingAdapter)
    {
        return;
    }

    m_mapCreateSettingAdapter[strPageID] = pFunCreateSettingAdapter;
}

CSettingUIAdapter * CSettingAdapterFactory::GetSettingAdapter(const yl::string & strPageID)
{
    MAP_CREATE_SETTING_ADAPTER_FUN::iterator it = m_mapCreateSettingAdapter.find(strPageID);
    FUN_CREATE_SETTING_ADAPTER pFun = NULL;
    if (it != m_mapCreateSettingAdapter.end())
    {
        pFun = it->second;
    }

    if (NULL == pFun)
    {
        return NULL;
    }

    return (*pFun)();
}

CSettingUIAdapter * CSettingAdapterFactory::GetDefaultSettingAdapter()
{
    return new CSettingUIAdapter();
}

