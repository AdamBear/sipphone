#ifndef __DIR_UI_HELPER_FACTORY_H__
#define __DIR_UI_HELPER_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "idiruihelper.h"

class CDirUIHelperFactory
{
    DECLARE_SINGLETON_CLASS(CDirUIHelperFactory)

public:
    IDirUIHelperPtr FindDirUIHelper(int nType);
    IDirUIHelperPtr GetDirUIHelper(int nType);
    void DeleteDirUIHelper(int nType);
    void Clear();

protected:
    typedef YLHashMap<int, IDirUIHelperPtr> MAP_DIR_UI_HELPER;
    MAP_DIR_UI_HELPER m_mapDirUIHelper;
};

#define g_DirUIHelperFactory CDirUIHelperFactory::GetInstance()

#endif
