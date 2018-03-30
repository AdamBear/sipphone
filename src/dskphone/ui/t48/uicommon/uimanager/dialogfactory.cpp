#include "uimanager_header.h"
#include "uimanager_inc.h"

// map类型,类名和类创建函数的映射
typedef std::map<QString, funCreateInstance> MAP_DLG_CLASS;

#define _ClassMap GetGlobalMap()
static MAP_DLG_CLASS & GetGlobalMap()
{
    // 为了控制全局ClassMap的构造顺序
    // 调用RegisterDialog时必须构造出来，否则insert会出错
    static MAP_DLG_CLASS sClassMap;
    return sClassMap;
}

// 通过类名来创建类实例
CBaseDialog * CDialogFactory::CreateClassByName(const QString & strClassName)
{
    MAP_DLG_CLASS::iterator itTarget = _ClassMap.find(strClassName);

    if (itTarget == _ClassMap.end())
    {
        return NULL;
    }
    else
    {
        return itTarget->second();
    }
}

// 将类名和类创建函数绑定并加入到map中
void CDialogFactory::RegisterDialog(const QString & strClassName, funCreateInstance pCreateFunc)
{
    // 此处必须保证_ClassMap已构造，不能将map作为CDialogFactory的静态成员
    // 因为如果其他cpp中全局对象的构造可能会早于该类静态成员，无法保证构造顺序
    _ClassMap.insert(std::pair<QString, funCreateInstance>(strClassName, pCreateFunc));
}
