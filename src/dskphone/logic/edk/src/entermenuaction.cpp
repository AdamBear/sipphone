#include "entermenuaction.h"
#include "edkactionmanager.h"
#include <setting_page_define.h>
#include "adapterbox/include/modadapterbox.h"

#if IF_FEATURE_EDK

CEnterMenuAction::CEnterMenuAction()
    : CEdkActionBase(EDK_EXCUTEENTERMENU)
{
}

CEnterMenuAction::~CEnterMenuAction()
{

}

/*
**进入某指定菜单界面执行类的执行动作
*/
bool CEnterMenuAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    EDK_INFO("CEnterMenuAction::DoAction Action[%s]", tEdkAction.m_strAction.c_str());

    bool bHandle = AdapterBox_EnterMenu(tEdkAction.m_strAction);
    bResult = bHandle;

    return bHandle;
}

bool CEnterMenuAction::MenuUIActionCallBack(bool bSuccess)
{
    if (!bSuccess)
    {
        //TODO
        //return _EdkActionManager.CancelAction();
    }

    return true;
}

#endif
