#include "uimanager_header.h"
#include "uimanager_inc.h"
#include "baseui/include/baseuicommon.h"


#if IF_MULTI_WORKSTATION_ENBALE
IMPLEMENT_MULTI_WORKSTATION_INSTANCE(CDialogFactory)
#else
IMPLEMENT_WORKSTATION_INSTANCE(CDialogFactory)
#endif

CDialogFactory::CDialogFactory()
{

}

CDialogFactory::~CDialogFactory()
{
    m_mapCreateFunc.clear();
}

// 通过类名来创建类实例
CBaseDialog * CDialogFactory::CreateClassByName(const yl::string & strClassName)
{
    MAP_DLG_CLASS_CREATE_FUNC::iterator iter = m_mapCreateFunc.find(strClassName);

    if (iter == m_mapCreateFunc.end())
    {
        return NULL;
    }
    else
    {
        return iter->second();
    }
}

// 将类名和类创建函数绑定并加入到map中
void CDialogFactory::RegisterDialog(const yl::string & strClassName,
                                    FUN_CREATE_DLG_INSTANCE pCreateFunc)
{
    MAP_DLG_CLASS_CREATE_FUNC::iterator iter = m_mapCreateFunc.find(strClassName);

    if (iter != m_mapCreateFunc.end())
    {
        UIMANAGER_ERR("Repeat RegisterDialog:%s", strClassName.c_str());
        return;
    }

    m_mapCreateFunc.insert(strClassName, pCreateFunc);
}
