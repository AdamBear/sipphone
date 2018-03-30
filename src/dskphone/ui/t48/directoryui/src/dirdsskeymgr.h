#ifndef __DIR_DSS_KEY_MGR_H__
#define __DIR_DSS_KEY_MGR_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "dirdsskey.h"


#ifndef IDsskeyImplActionPtr
typedef IDsskeyImplAction * IDsskeyImplActionPtr;
#endif


class CDirDssKeyMgr
{
    DECLARE_SINGLETON_CLASS(CDirDssKeyMgr)

public:
    void Init();
    void UnInit();
    bool IsDirDssKey(int nDssKeyType);

protected:
    void CreateDssKeyXmlGroup();
    void CreateDssKeyIdleCallList();
    void CreateDssKeyLDAP();
    void CreateDssKeyLocalDir();
    void CreateDssKeyBSDir();
    void CreateDssKeyLocalGroup();
    void CreateDssKeyBSGroup();
    void CreateDssKeyIdleRemote();
    void CreateDssKeyFavorites();
    void CreateDssKeyNetworkFavorite();
    void CreateDssKeyUCFavorite();
    void CreateDssKeyBuddies();
    void CreateDssKeyStatus();
    void CreateDsskeyRedial();
#if IF_BUG_14856
    void CreateDssKeyMissedCall();
#endif
#if IF_BUG_PLCMDIR
    void CreateDssKeyPlcmDir();
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    void CreateDssKeyMTSWDir();
#endif
protected:
    typedef YLHashMap<int, IDsskeyImplActionPtr> MAP_DSS_KEY;

    MAP_DSS_KEY m_mapDssKey;
};

#define g_DirDssKeyMgr GET_SINGLETON_INSTANCE(CDirDssKeyMgr)

#endif
