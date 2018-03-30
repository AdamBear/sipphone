#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyDNDAciton::CDsskeyDNDAciton() : CDssTalkBaseAciton(DT_DND)
{
    // DND状态变化刷新DSSKey状态
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, CDsskeyDNDAciton::OnDndStatusChangeMsg);
    // 默认账号状态变化刷新DSSKey状态
    SingleMsgReg(ACCOUNT_DEFAULT_CHANGED, CDsskeyDNDAciton::OnDefaultAccountChangedMsg);
}

CDsskeyDNDAciton::~CDsskeyDNDAciton()
{
    SingleMsgUnReg(DND_MESSAGE_STATUS_CHANGED, CDsskeyDNDAciton::OnDndStatusChangeMsg);
    SingleMsgUnReg(ACCOUNT_DEFAULT_CHANGED, CDsskeyDNDAciton::OnDefaultAccountChangedMsg);
}

bool CDsskeyDNDAciton::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    DSSKEY_INFO("DND dsskey pressed: LineID=%d, LineIndex=%d",
                logicData.iLineIndex, logicData.iLineIndex);
    if (!dnd_IsActive())
    {
        DSSKEY_WARN("DND is not active");
        return false;
    }
    int iAccountID = 0;
    if (dnd_GetMode() == FWDDND_MODE_GLOBAL)
    {
        // 全局模式，则切换全局模式DND状态
        iAccountID = FWDDND_ACCOUNT_GLOBAL;
    }
    else // FWDDND_MODE_ACCOUNT
    {
        ListAccounts accounts = acc_GetValidAccountList(ACCOUNT_TYPE_SIP);
        if (accounts.size() == 1)
        {
            // 只有一个已启用SIP账号，则切换该账号
            iAccountID = accounts.front();
        }
        else if (accounts.size() > 1)
        {
            // 多于一个已启用SIP账号，则进入配置界面
            DSSKEY_INFO("There are %d registered accounts, show DND config UI",
                        accounts.size());
            AdapterBox_SettingUIEnterPage(MENU_SETTING_CUSTOM_DND);
            return true;
        }
        else // accounts.size() == 0
        {
            // 没有已启用SIP账号，则不做任何操作
            DSSKEY_WARN("No registered account when DND dsskey pressed!");
            return false;
        }
    }
    DSSKEY_INFO("Switch DND status of account #%d", iAccountID);
    dnd_SetEnable(iAccountID, !dnd_IsEnable(iAccountID));
    return true;
}

bool CDsskeyDNDAciton::OnConfigChange(int dsskeyID, void* pExtraData)
{
    DSSKEY_INFO("DND dsskey config changed: DsskeyID=%d", dsskeyID);
    UpdateDsskey();
    return true;
}

LRESULT CDsskeyDNDAciton::OnDndStatusChangeMsg(msgObject& msg)
{
    // DND状态变化刷新DSSKey状态
    UpdateDsskey();
    return TRUE;
}

LRESULT CDsskeyDNDAciton::OnDefaultAccountChangedMsg(msgObject& msg)
{
    if (dnd_GetMode() == FWDDND_MODE_ACCOUNT)
    {
        // 账号模式下默认账号变化刷新DSSKey状态
        UpdateDsskey();
    }
    return TRUE;
}

void CDsskeyDNDAciton::UpdateDsskey()
{
    int iAccountID = acc_GetDefaultAccount();

    if (dnd_GetMode() == FWDDND_MODE_GLOBAL)
    {
        // 全局模式，则展示全局模式DND状态
        iAccountID = FWDDND_ACCOUNT_GLOBAL;
    }

    dsskey_UpdateDNDStatus(dnd_IsEnable(iAccountID));
}

#endif
