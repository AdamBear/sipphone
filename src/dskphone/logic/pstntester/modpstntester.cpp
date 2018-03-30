#include "modpstntester.h"

#include <features-define.h>
#include <dsklog/log.h>

#ifdef IF_FEATURE_PSTN

#include <commonapi/systemhelperapi.h>
#include <setting/include/messagedefine.h>
#include <etlmsghandler/modetlmsghandler.h>

#include <ETLLib.hpp>
#include <commonunits/modcommonunits.h>

#include <hotplug/commondefine.h>
// test mod pstnadapter
#include <pstnadapter/include/modpstnadapter.h>
// test mod account
#include <account/include/modaccount.h>
#include <account/include/accountmanager.h>
// test mod talklogic

#include <prof/prof.h>


enum PSTN_TESTER_MSG
{
    PSTN_TESTER_MSG_BEGIN = INNER_MSG_PSTN_MESSAGE_END + 1,

    PSTN_TESTER_MSG_PSTN_BOX_PLUG_IN = PSTN_TESTER_MSG_BEGIN,
    PSTN_TESTER_MSG_PSTN_BOX_PULL_OUT,
    PSTN_TESTER_MSG_PSTN_LINE_PLUG_IN,
    PSTN_TESTER_MSG_PSTN_LINE_PULL_OUT,
    PSTN_TESTER_MSG_PRINT_ALL_ACCOUNT,
    PSTN_TESTER_MSG_SET_DEFAULT_ACCOUNT,
    PSTN_TESTER_MSG_SWITCH_DEFAULT_ACCOUNT,
    PSTN_TESTER_MSG_AVAILABLE_ACCOUNT,
    PSTN_TESTER_MSG_IS_ENABLE,

    PSTN_TESTER_MSG_INCOMING_CALL,
    PSTN_TESTER_MSG_INCOMING_CID,
    PSTN_TESTER_MSG_INCOMING_CANCEL,
    PSTN_TESTER_MSG_DIAL_RESULT,

    PSTN_TESTER_MSG_START_PROF,
    PSTN_TESTER_MSG_END_PROF,

    PSTN_TESTER_MSG_END
};


LRESULT ProcessPstnTesterMsg(msgObject & msg)
{
    COMMONUNITS_INFO("===== %s(m=%d,w=%d,l=%d) =====", __FUNCTION__, msg.message, msg.wParam,
                     msg.lParam);
    switch (msg.message)
    {
    ////////////////////////////////////////////////// pstnadapter //////////////////////////////////////////////////
    case PSTN_TESTER_MSG_PSTN_BOX_PLUG_IN:
        // PSTN Box插入
        etl_NotifyAppEx(false, HOTPLUG_USB_PSTN_BOX_STATE_CHANGE, 1 /* plug in */, 0 /* DeviceID */, 0,
                        NULL);
        break;

    case PSTN_TESTER_MSG_PSTN_BOX_PULL_OUT:
        // PSTN Box拔出
        etl_NotifyAppEx(false, HOTPLUG_USB_PSTN_BOX_STATE_CHANGE, 0 /* pull out */, 0 /* DeviceID */, 0,
                        NULL);
        break;
    ////////////////////////////////////////////////// pstnadapter //////////////////////////////////////////////////

    ////////////////////////////////////////////////// account //////////////////////////////////////////////////
    case PSTN_TESTER_MSG_PSTN_LINE_PLUG_IN:
        // PSTN线路插入
        etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, 0 /* DeviceID */, PSTN_LINE_CONNECTED, 0, NULL);
        break;

    case PSTN_TESTER_MSG_PSTN_LINE_PULL_OUT:
        // PSTN线路拔出
        etl_NotifyAppEx(false, PSTN_MSG_LINE_CHANGED, 0 /* DeviceID */, PSTN_LINE_DISCONNECTED, 0, NULL);
        break;

    case PSTN_TESTER_MSG_PRINT_ALL_ACCOUNT:
    {
        // 打印所有账号信息
        COMMONUNITS_INFO("acc_AccountNum: ACCOUNT_TYPE_SIP=%d, ACCOUNT_TYPE_PSTN=%d, ACCOUNT_TYPE_ALL=%d",
                         acc_AccountNum(ACCOUNT_TYPE_SIP), acc_AccountNum(ACCOUNT_TYPE_PSTN),
                         acc_AccountNum(ACCOUNT_TYPE_ALL));
        COMMONUNITS_INFO("acc_GetRegisteredAccountNum: ACCOUNT_TYPE_SIP=%d, ACCOUNT_TYPE_PSTN=%d, ACCOUNT_TYPE_ALL=%d",
                         acc_GetRegisteredAccountNum(ACCOUNT_TYPE_SIP), acc_GetRegisteredAccountNum(ACCOUNT_TYPE_PSTN),
                         acc_GetRegisteredAccountNum(ACCOUNT_TYPE_ALL));
        _AccountManager.PrintAllAccountInfo();
        // 获取账号信息
        yl::string strAccountInfo;
        acc_GetAccountInfo(strAccountInfo, ACCOUNT_TYPE_SIP);
        COMMONUNITS_INFO("acc_GetAccountInfo(ACCOUNT_TYPE_SIP): %s", strAccountInfo.c_str());
        strAccountInfo.clear();
        acc_GetAccountInfo(strAccountInfo, ACCOUNT_TYPE_ALL);
        COMMONUNITS_INFO("acc_GetAccountInfo(ACCOUNT_TYPE_ALL): %s", strAccountInfo.c_str());
        break;
    }

    case PSTN_TESTER_MSG_SET_DEFAULT_ACCOUNT:
        // 设置默认账号
        acc_SetDefaultAccount(msg.wParam, true);
        COMMONUNITS_INFO("acc_GetDefaultAccount=%d, acc_GetAccountShowText=%s",
                         acc_GetDefaultAccount(), acc_GetAccountShowText(acc_GetDefaultAccount()).c_str());
        break;

    case PSTN_TESTER_MSG_SWITCH_DEFAULT_ACCOUNT:
        // 切换默认账号
        COMMONUNITS_INFO("acc_SwitchDefaultAccount: FORWARD=%d, default=%d", acc_SwitchDefaultAccount(),
                         acc_GetDefaultAccount());
        //COMMONUNITS_INFO("acc_SwitchDefaultAccount: FORWARD=%d, default=%d", acc_SwitchDefaultAccount(), acc_GetDefaultAccount());
        //COMMONUNITS_INFO("acc_SwitchDefaultAccount: BACKWARD=%d, default=%d", acc_SwitchDefaultAccount(false), acc_GetDefaultAccount());
        break;

    case PSTN_TESTER_MSG_AVAILABLE_ACCOUNT:
    {
        // 可用账号
        COMMONUNITS_INFO("acc_ExistAvailableAccount: ACCOUNT_TYPE_SIP=%d, ACCOUNT_TYPE_PSTN=%d, ACCOUNT_TYPE_ALL=%d",
                         acc_ExistAvailableAccount(ACCOUNT_TYPE_SIP), acc_ExistAvailableAccount(ACCOUNT_TYPE_PSTN),
                         acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL));
        COMMONUNITS_INFO("acc_GetUsableAccount[NOT FIRST]: ACCOUNT_TYPE_SIP=%d, ACCOUNT_TYPE_PSTN=%d, ACCOUNT_TYPE_ALL=%d",
                         acc_GetUsableAccount(false, ACCOUNT_TYPE_SIP), acc_GetUsableAccount(false, ACCOUNT_TYPE_PSTN),
                         acc_GetUsableAccount(false, ACCOUNT_TYPE_ALL));
        COMMONUNITS_INFO("acc_GetUsableAccount[FIRST]: ACCOUNT_TYPE_SIP=%d, ACCOUNT_TYPE_PSTN=%d, ACCOUNT_TYPE_ALL=%d",
                         acc_GetUsableAccount(true, ACCOUNT_TYPE_SIP), acc_GetUsableAccount(true, ACCOUNT_TYPE_PSTN),
                         acc_GetUsableAccount(true, ACCOUNT_TYPE_ALL));
        ListAccounts listAvailableAccount = acc_GetAvailableAccountList();
        yl::string strAvailableAccount;
        for (ListAccounts::iterator it = listAvailableAccount.begin(); it != listAvailableAccount.end();
                ++it)
        {
            char szAvailableAccount[8] = { 0 };
            sprintf(szAvailableAccount, " %d", *it);
            strAvailableAccount += szAvailableAccount;
        }
        COMMONUNITS_INFO("acc_GetAvailableAccountList size=%d: %s", listAvailableAccount.size(),
                         strAvailableAccount.c_str());
        break;
    }

    case PSTN_TESTER_MSG_IS_ENABLE:
        // 账号是否启用
        COMMONUNITS_INFO("acc_IsAccountEnable(#%d: %s)=%d", msg.wParam,
                         acc_GetCustomShowText(msg.wParam).c_str(), acc_IsAccountEnable(msg.wParam));
        break;
    ////////////////////////////////////////////////// account //////////////////////////////////////////////////

    ////////////////////////////////////////////////// talk //////////////////////////////////////////////////
    case PSTN_TESTER_MSG_INCOMING_CALL:
        // PSTN来电
        etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CALL, 0 /* DeviceID */, 9999 /* CallID */, 0, NULL);
        break;

    case PSTN_TESTER_MSG_INCOMING_CID:
    {
        // PSTN来电显示
        const char * szCallNumber = "10086";
        etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CID, 0 /* DeviceID */, 9999 /* CallID */,
                        strlen(szCallNumber) + 1, szCallNumber);
        break;
    }

    case PSTN_TESTER_MSG_INCOMING_CANCEL:
        // PSTN来电取消
        etl_NotifyAppEx(false, PSTN_MSG_INCOMING_CANCEL, 0 /* DeviceID */, 9999 /* CallID */, 0, NULL);
        break;

    case PSTN_TESTER_MSG_DIAL_RESULT:
    {
        // PSTN外呼结果
        bool bResult = msg.wParam;
        etl_NotifyAppEx(false, PSTN_MSG_DIAL_RESULT, 0 /* DeviceID */, msg.lParam /* CallID */,
                        sizeof(bool), &bResult);
        break;
    }
    ////////////////////////////////////////////////// talk //////////////////////////////////////////////////

    ////////////////////////////////////////////////// prof //////////////////////////////////////////////////
    case PSTN_TESTER_MSG_START_PROF:
    {
        COMMONUNITS_INFO("Start profiling...");
        start_profile("/mnt/test_prof");
        break;
    }
    case PSTN_TESTER_MSG_END_PROF:
    {
        COMMONUNITS_INFO("Stop profiling...");
        stop_profile();
        break;
    }
    ////////////////////////////////////////////////// prof //////////////////////////////////////////////////

    default:
        break;
    }
    return TRUE;
}

void pstntester_Init()
{
    COMMONUNITS_INFO("PSTN tester module init.");
    etl_RegisterMsgHandle(PSTN_TESTER_MSG_BEGIN, PSTN_TESTER_MSG_END, ProcessPstnTesterMsg);
}

#else // IF_FEATURE_PSTN

void pstntester_Init()
{
    COMMONUNITS_INFO("PSTN feature is not enabled.");
}

#endif // IF_FEATURE_PSTN