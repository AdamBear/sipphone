#include "editcheckmethodfactory.h"
#include "dsklog/log.h"

IMPLEMENT_GETINSTANCE(CEditCheckMethodFactory)

CEditCheckMethodFactory::CEditCheckMethodFactory()
{
}

CEditCheckMethodFactory::~CEditCheckMethodFactory()
{
}

void CEditCheckMethodFactory::RegisterCheckMethod(const yl::string & strType,
        FUN_CHECK_METHOD pFunCheckMethod)
{
    if (NULL == pFunCheckMethod)
    {
        return;
    }

    m_mapCheckMethod[strType] = pFunCheckMethod;
}

bool CEditCheckMethodFactory::CheckValueValidByType(const yl::string & strType,
        const yl::string & strValue)
{
    MAP_CHECK_METHOD_FUN::iterator it = m_mapCheckMethod.find(strType);
    FUN_CHECK_METHOD pFun = NULL;
    if (it != m_mapCheckMethod.end())
    {
        pFun = it->second;
    }

    if (NULL != pFun)
    {
        return (*pFun)(strValue);
    }

    SETTINGUI_INFO("[CEditCheckMethodFactory::CheckValueValid] not such type[%s] check method."
                   , strType.c_str());
    return true;
}

FUN_CHECK_METHOD CEditCheckMethodFactory::GetCheckMethod(const yl::string & strType)
{
    MAP_CHECK_METHOD_FUN::iterator it = m_mapCheckMethod.find(strType);
    FUN_CHECK_METHOD pFun = NULL;
    if (it != m_mapCheckMethod.end())
    {
        pFun = it->second;
    }

    return pFun;
}

