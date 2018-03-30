#ifndef __DIR_UI_HELPER_FACTORY_H__
#define __DIR_UI_HELPER_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "diruihelper.h"

class CDirUIHelperFactory
{
    DECLARE_SINGLETON_CLASS(CDirUIHelperFactory)

public:
    CDirUIHelperPtr FindDirUIHelper(int nType);
    CDirUIHelperPtr GetDirUIHelper(int nType);
    void DeleteDirUIHelper(int nType);
    void Clear();

protected:
    typedef YLHashMap<int, CDirUIHelperPtr> MAP_DIR_UI_HELPER;
    MAP_DIR_UI_HELPER m_mapDirUIHelper;
};

#define g_DirUIHelperFactory CDirUIHelperFactory::GetInstance()

#endif
