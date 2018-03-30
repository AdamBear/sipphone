#ifndef _POPUPBOX_PROXY_FACTORY_H_
#define _POPUPBOX_PROXY_FACTORY_H_

#include <map>

class CBaseIdlePopupboxProxy;
typedef CBaseIdlePopupboxProxy * (*funCreateProxy)();

// 弹出框的创建工厂
class CPopupboxProxyFactory
{
public:
    // 判断是否为弹框类型
    static bool IsPopupboxType(int iType);

    // 通过类型来创建类实例
    static CBaseIdlePopupboxProxy * CreateClassByType(int iType);

    // 将类型和类创建函数绑定并加入到map中
    static void RegisterProxy(int iType, funCreateProxy pCreateFunc);

private:
    // 不能实例化对象
    CPopupboxProxyFactory() {}

private:
    typedef std::map<int, funCreateProxy>  MapProxy;
    typedef MapProxy::iterator             IterMapProxy;

private:
    static  MapProxy    m_mapProxy;//这个成语static函数里面要使用到，所有使用static
};

#endif // _POPUPBOX_PROXY_FACTORY_H_
