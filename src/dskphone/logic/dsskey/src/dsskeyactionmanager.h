#ifndef _DSSKEY_ACTION_MANAGER_HEADER_
#define _DSSKEY_ACTION_MANAGER_HEADER_
#include <ylhashmap.h>
#include "../include/dsskey_enumtypes.h"
class CDSSKeyAction;
class CDSSKeyActionManger
{
    //typedef YLHashMap<DssKey_Type, CDSSKeyAction *> MapDsskeyAction;
public:
    static CDSSKeyActionManger * GetInstance() DECLARE_ATTR;

    CDSSKeyAction * GetDSSKeyAction(DssKey_Type dsskeyType);


protected:
    CDSSKeyActionManger();
    ~CDSSKeyActionManger();

    void Init();
protected:
    typedef YLHashMap<int, CDSSKeyAction *> typeDsskeyactionMap;
    typeDsskeyactionMap m_mapDsskeyaction;

};

#define _DSSKeyActionManger (*CDSSKeyActionManger::GetInstance())
#endif
