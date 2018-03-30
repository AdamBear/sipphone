#include "commonaction.h"
#include "edkactionmanager.h"
#include <etlmsghandler/modetlmsghandler.h>
#include "talk/talklogic/include/modtalklogic.h"
#include "account/include/modaccount.h"
#include "adapterbox/include/modadapterbox.h"
#include <setting_page_define.h>
#include "dsskey/include/moddsskey.h"
#include "edkdatamanager.h"

#if IF_FEATURE_EDK
CCommonAction::CCommonAction(void)
    : CEdkActionBase(EDK_EXCUTECOMMON)
{
}

CCommonAction::~CCommonAction(void)
{

}

/*
**公共执行类的执行动作
*/
bool CCommonAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    bool bContinue = true;
    switch (tEdkAction.m_eActionType)
    {
    case EDK_URL:
        {

        }
        break;
    // 功能入口
    case EDK_FUNCTION:
        {
            // 进入拨号界面
            if (tEdkAction.m_strAction == "NewCall")
            {
                talklogic_EnterDialUIByAccount(acc_GetDefaultAccount());
            }
            // 回拨
            else if (tEdkAction.m_strAction == "ReCall")
            {
                talklogic_CallReturn();
            }
            else if (tEdkAction.m_strAction == "Menu")
            {
                AdapterBox_SettingUIEnterPage(MENU_PAGE_MAIN);
            }
            else if (tEdkAction.m_strAction == "Anwser")
            {
                talklogic_AcceptIncomingCall();
            }
            else if (tEdkAction.m_strAction == "ReDial")
            {
                //      talklogic_ReDial();
            }
        }
        break;
    default:
        break;
    }

    return true;
}
#endif
