#ifndef __EDIT_CHECK_METHOD_FACTORY_H__
#define __EDIT_CHECK_METHOD_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "ETLLib/ETLLib.hpp"

typedef bool (*FUN_CHECK_METHOD)(const yl::string & strValue);
typedef YLHashMap<yl::string, FUN_CHECK_METHOD> MAP_CHECK_METHOD_FUN;

class CEditCheckMethodFactory
{
    DECLARE_SINGLETON_CLASS(CEditCheckMethodFactory);

public:
    void RegisterCheckMethod(const yl::string & strType, FUN_CHECK_METHOD pFunCheckMethod);

    bool CheckValueValidByType(const yl::string & strType, const yl::string & strValue);

    FUN_CHECK_METHOD GetCheckMethod(const yl::string & strType);

protected:
    MAP_CHECK_METHOD_FUN    m_mapCheckMethod;
};

#define g_CheckMethodFactory GET_SINGLETON_INSTANCE(CEditCheckMethodFactory)

#endif // __EDIT_CHECK_METHOD_FACTORY_H__

