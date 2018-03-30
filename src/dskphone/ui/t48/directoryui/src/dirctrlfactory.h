#ifndef __DIR_CTRL_FACTORY_H__
#define __DIR_CTRL_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "directorycontroler.h"

class CDirCtrlFactory
{
    DECLARE_SINGLETON_CLASS(CDirCtrlFactory)

public:
    CDirectoryControlerPtr GetControler(int nType);
    CDirectoryControlerPtr FindControler(int nType);
    void DeleteControler(int nType);
    void Clear();

    bool IsControlerValid(int nType, CDirectoryControlerPtr pControler);

protected:
    typedef YLHashMap<int, CDirectoryControlerPtr> MAP_DIR_CTRLER;
    MAP_DIR_CTRLER m_mapDirCtrler;
};

#define g_DirCtrlFactory GET_SINGLETON_INSTANCE(CDirCtrlFactory)

#endif
