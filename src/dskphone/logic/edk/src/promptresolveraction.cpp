#include "promptresolveraction.h"
#include "edkactionmanager.h"
#include "adapterbox/include/modadapterbox.h"

#if IF_FEATURE_EDK
CPromptResolverAction::CPromptResolverAction()
    : CEdkActionBase(EDK_EXCUTEPROMPT)
{

}

CPromptResolverAction::~CPromptResolverAction()
{

}

bool CPromptResolverAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    if (tEdkAction.m_eActionType != EDK_POPUP)
    {
        return false;
    }

    // 调用ui弹窗
    AdapterBox_ShowEdkPrompt(atoi(tEdkAction.m_strAction.c_str()), iActionId, PromptUIActionCallBack);
    return false;
}

/*
**UI响应后的回调函数
*/
bool CPromptResolverAction::PromptUIActionCallBack(bool bOK, int iId, yl::string & strInput)
{
    if (bOK)
    {
        // 给edkActionManager数据,继续剩下的命令
        _EdkActionManager.FeedBackData(iId, strInput);
    }
    else
    {
        // 输入框按了cancel后，取消命令
        return _EdkActionManager.CancelAction(iId);
    }

    return false;
}
#endif
