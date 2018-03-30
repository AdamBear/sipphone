#ifndef __ETLMSGSYSTEM_H__
#define __ETLMSGSYSTEM_H__
//////////////////////////////////////////////////////////////////////////
#ifndef __ETL_H__
#error ETLMsgSystem.h requires ETL.h to be included first
#endif

//#if LINUX_SYSTEM
//#include "xInc/xMsgSystem.h"
//#else
//#include "wInc/wMsgSystem.h"
//#endif

namespace ETL_MsgQueueHelper
{
////////////////////////////////////////////////////////////////////////
//class chNamedObjectManager
class chNamedObjectManager : public baseSharePoolAlloc
{
    typedef list_node_offset< chShareStringA >                      typeNamedNode;
    typedef typeNamedNode::TChain                                   typeNamedChain;
public:
    LPVOID GetNamedObject(LPCSTR lpszObjectName, int nBytes, BOOL * pbFirstCreate);
    LPVOID FindNamedObject(LPCSTR lpszObjectName);
    void FreeNamedObject(LPCSTR lpszObjectName);
protected:
    void FreeObject(LPVOID pObject);
public:
    typeNamedChain              m_vRootChain;
};



//////////////////////////////////////////////////////////////////////////
// class chSystemNamedObject
class chSystemNamedObject : public ipcObjectHeader
    , public baseStaticPoolAlloc
    , public chNamedObjectManager
{
public:
    chSystemNamedObject();
};
//////////////////////////////////////////////////////////////////////////
// class ucSystemNamedObjectHelper
typedef ipcObjectFrame<chSystemNamedObject, chMapFile, ipcMutex> ipcSystemNamedObject;
class uCSystemNamedObjectHelper : public ipcSystemNamedObject
{
public:
    uCSystemNamedObjectHelper();
    ~uCSystemNamedObjectHelper();
};

DECLARE_GLOBLE_OBJECT(uCSystemNamedObjectHelper);
#define g_uCSystemNamedObjectHelper     REFERENCE_GLOBLE_OBJECT(uCSystemNamedObjectHelper)

};
using namespace ETL_MsgQueueHelper;


#if IF_EXPORT_SYMBOLS
#include "ETLMsgSystem.cpp"
#endif
#endif //__ETLMSGSYSTEM_H__
