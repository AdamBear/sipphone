#include "../include/uilogichelper.h"
#include "../include/baseidlehandle.h"


IMPLEMENT_GETINSTANCE(CIdleUILogicHelper)
CIdleUILogicHelper::CIdleUILogicHelper()
{

}

CIdleUILogicHelper::~CIdleUILogicHelper()
{
    IterMapIdleHandle it = m_mapHandle.begin();
    while (it != m_mapHandle.end()
            && NULL != it->second)
    {
        delete it->second;
    }
}

CBaseIdleHandle * CIdleUILogicHelper::GetHandleByType(int iType)
{
    IterMapIdleHandle Iter = m_mapHandle.find(iType);
    if (Iter != m_mapHandle.end())
    {
        return Iter->second;
    }
    return NULL;
}

bool CIdleUILogicHelper::UnRegisterHandle(int iType)
{
    IterMapIdleHandle Iter = m_mapHandle.find(iType);
    if (Iter != m_mapHandle.end())
    {
        delete Iter->second;
        m_mapHandle.erase(Iter);
        return true;
    }
    return false;
}

void CIdleUILogicHelper::RegisterHandle(int iType, CBaseIdleHandle * pHandle)
{
    if (NULL == pHandle)
    {
        return;
    }

    // 先注销
    UnRegisterHandle(iType);

    // 注册并创建模块对象
    m_mapHandle[iType] = pHandle;
    pHandle->SetType(iType);
}
