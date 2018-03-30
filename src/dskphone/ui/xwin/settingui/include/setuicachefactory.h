#ifndef __SET_UI_CACHE_FACTORY_H__
#define __SET_UI_CACHE_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "setuidlgbasecache.h"

typedef CSetUIDlgBaseCache * (*FUN_CREATE_DLG_CACHE)();
typedef YLHashMap<yl::string, FUN_CREATE_DLG_CACHE>     MAP_CREATE_DLG_CACHE;

class CSetUICacheFactory
{
    DECLARE_SINGLETON_CLASS(CSetUICacheFactory);

public:
    void RegisterDlgCache(const yl::string & strPageID, FUN_CREATE_DLG_CACHE pFunCreateDlgCache);
    CSetUIDlgBaseCache * GetDlgCache(const yl::string & strPageID);

private:
    MAP_CREATE_DLG_CACHE            m_mapCreateDlgCache;
};

#define g_DlgCache      GET_SINGLETON_INSTANCE(CSetUICacheFactory)

#endif

