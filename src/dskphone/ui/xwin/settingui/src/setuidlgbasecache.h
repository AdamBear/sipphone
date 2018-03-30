#ifndef __SET_UI_DLG_BASE_CACHE_H__
#define __SET_UI_DLG_BASE_CACHE_H__

#include "xwindow/xListView.h"

class CSettingUIDelegateBase;

class CSetUIDlgBaseCache
{
public:
    static CSetUIDlgBaseCache * GetDlgCache();

public:
    virtual bool DialogCache(CSettingUIDelegateBase * pSetDelegate, xListView * pListView);

private:
    CSetUIDlgBaseCache();
    ~CSetUIDlgBaseCache();

private:
    static CSetUIDlgBaseCache   *   s_SetUIDlgBaseCache;
};

#define g_SetUIDlgBaseCache     (CSetUIDlgBaseCache::GetDlgCache())

#endif

