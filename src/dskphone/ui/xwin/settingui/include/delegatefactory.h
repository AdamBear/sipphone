#ifndef __DELEGATE_FACTORY_H__
#define __DELEGATE_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "basesettinguidelegate.h"

typedef CSettingUIDelegateBase * (*FUN_CREATE_DELEGATE)();
typedef YLHashMap<yl::string, FUN_CREATE_DELEGATE> MAP_CREATE_DELEGATE_FUN;

class CDelegateFactory
{
    DECLARE_SINGLETON_CLASS(CDelegateFactory);

public:
    void RegisterDelegate(const yl::string & strPageID, FUN_CREATE_DELEGATE pFunCreateDelegate);

    CSettingUIDelegateBase * GetDelegate(const yl::string & strPageID);

    // 获取默认Delegate
    CSettingUIDelegateBase * GetDefaultDelegate();

protected:
    MAP_CREATE_DELEGATE_FUN m_mapCreateDelegate;
};

#define g_DelegateFactory GET_SINGLETON_INSTANCE(CDelegateFactory)

#endif

