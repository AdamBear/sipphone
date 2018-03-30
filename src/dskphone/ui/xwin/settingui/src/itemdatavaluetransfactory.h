#ifndef _SETTINGUI_DATA_TRANS_FACTORY_H_
#define _SETTINGUI_DATA_TRANS_FACTORY_H_

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "itemdatavaluetrans.h"

typedef CItemDataValueTransBase * (*FUN_CREATE_DATATRANS)();
typedef YLHashMap<int, CItemDataValueTransBase *> MAP_CREATE_DATATRANS_FUN;

class CItemDataValueTransFactory
{
    DECLARE_SINGLETON_CLASS(CItemDataValueTransFactory);

public:
    void RegisterItemDataValueTrans(int nItemDataType, FUN_CREATE_DATATRANS pFunCreateDataTrans);

    CItemDataValueTransBase * GetItemDataValueTrans(int nItemDataType);

protected:
    MAP_CREATE_DATATRANS_FUN    m_mapCreateDataTrans;
};

#define g_ItemDataValueTransFactory GET_SINGLETON_INSTANCE(CItemDataValueTransFactory)

#endif //_SETTINGUI_DATA_TRANS_FACTORY_H_
