#include "popupboxproxyfactory.h"

// 初始化类静态变量
CPopupboxProxyFactory::MapProxy CPopupboxProxyFactory::m_mapProxy;

bool CPopupboxProxyFactory::IsPopupboxType(int iType)
{
    IterMapProxy it = m_mapProxy.find(iType);
    if (it != m_mapProxy.end())
    {
        return true;
    }

    return false;
}

// 通过类名来创建类实例
CBaseIdlePopupboxProxy * CPopupboxProxyFactory::CreateClassByType(int iType)
{
    IterMapProxy it = m_mapProxy.find(iType);
    if (it != m_mapProxy.end()
            && NULL != it->second)
    {
        return it->second();
    }

    return NULL;
}

// 将类名和类创建函数绑定并加入到map中
void CPopupboxProxyFactory::RegisterProxy(int iType,
        funCreateProxy pCreateFunc)//绑定的是创建函数和类对应的int类型
{
    m_mapProxy[iType] = pCreateFunc;
}
