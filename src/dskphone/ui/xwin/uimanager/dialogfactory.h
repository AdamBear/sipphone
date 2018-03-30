#ifndef __DIALOG_FACTORY__
#define __DIALOG_FACTORY__

#include <ylstring.h>
#include <ylhashmap.h>

class CBaseDialog;
typedef CBaseDialog * (*FUN_CREATE_DLG_INSTANCE)();

// 所有小窗口的创建工厂
class CDialogFactory
{
public:
    CDialogFactory();
    ~CDialogFactory();

public:
    static CDialogFactory & GetInstance();

public:
    typedef YLHashMap<yl::string, FUN_CREATE_DLG_INSTANCE> MAP_DLG_CLASS_CREATE_FUNC;

    // 通过类名来创建类实例
    CBaseDialog * CreateClassByName(const yl::string & strClassName);
    // 将类名和类创建函数绑定并加入到map中
    void RegisterDialog(const yl::string & strClassName, FUN_CREATE_DLG_INSTANCE pCreateFunc);

protected:
    MAP_DLG_CLASS_CREATE_FUNC m_mapCreateFunc;
};

#define g_DlgFactory CDialogFactory::GetInstance()

#endif // __DIALOG_FACTORY__
