#ifndef __DIALOG_FACTORY__
#define __DIALOG_FACTORY__

#include <QtCore/QString>

class CBaseDialog;
typedef CBaseDialog * (*funCreateInstance)();

// 所有小窗口的创建工厂
class CDialogFactory
{
public:
    // 通过类名来创建类实例
    static CBaseDialog * CreateClassByName(const QString & strClassName);
    // 将类名和类创建函数绑定并加入到map中
    static void RegisterDialog(const QString & strClassName, funCreateInstance pCreateFunc);

    // 不能实例化对象
private:
    CDialogFactory() {}
};

#endif // __DIALOG_FACTORY__
