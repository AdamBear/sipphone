#ifndef _UILOGIC_HELPER_H_
#define _UILOGIC_HELPER_H_
//头文件
#include <ylhashmap.h>
#include <ylvector.h>
#include <singletonclassdefine.h>
#include "features-define.h"
//类声明
class CBaseIdleHandle;
typedef CBaseIdleHandle * (*funCreateHandle)();
class CIdleUILogicHelper
{
    DECLARE_SINGLETON_CLASS(CIdleUILogicHelper)
public:
    //根据类型获取对应类型handle
    CBaseIdleHandle * GetHandleByType(int iType);
    void RegisterHandle(int iType, CBaseIdleHandle * pHandle); //map

private:
    bool UnRegisterHandle(int iType);

private:
    typedef YLHashMap<int, CBaseIdleHandle *>    MapIdleHandle;
    typedef MapIdleHandle::iterator             IterMapIdleHandle;
    MapIdleHandle                               m_mapHandle;
};


#define REGISTER_IDLE_HANDLE(type, className) \
    _UiIdleLogicHelper.RegisterHandle(type, new className(type));

#define GET_IDLE_HANDLE(type) _UiIdleLogicHelper.GetHandleByType(type);

#define _UiIdleLogicHelper GET_SINGLETON_INSTANCE(CIdleUILogicHelper)

#endif //end of _UILOGIC_HELPER_H_
