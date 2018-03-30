#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyFwdAction::CDsskeyFwdAction()
    : CDssTalkBaseAciton(DT_FWD)
{
    // FWD状态变化刷新DSSKey状态
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, CDsskeyFwdAction::OnFwdStatusChangeMsg);
    // 默认账号状态变化刷新DSSKey状态
    SingleMsgReg(ACCOUNT_DEFAULT_CHANGED, CDsskeyFwdAction::OnDefaultAccountChangedMsg);
}

CDsskeyFwdAction::~CDsskeyFwdAction()
{
    SingleMsgUnReg(FWD_MESSAGE_STATUS_CHANGED, CDsskeyFwdAction::OnFwdStatusChangeMsg);
    SingleMsgUnReg(ACCOUNT_DEFAULT_CHANGED, CDsskeyFwdAction::OnDefaultAccountChangedMsg);
}

bool CDsskeyFwdAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                  void* pExtraData)
{
    DSSKEY_INFO("FWD dsskey pressed: Value=%s", logicData.strValue.c_str());
    if (!fwd_IsActive())
    {
        DSSKEY_WARN("FWD is not active");
        return false;
    }

    // 判断当前是否通话中
    CSessionStateProxy focus;
    if (!focus || focus.IsHide())
    {
        int iAccountID = focus.GetAccountID();
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
        if (CallCenter_IsFlexibleSeatingOn(iAccountID))
        {
            return false;
        }
#endif

        if (!fwd_IsAllowForwardTo(logicData.strValue))
        {
            DSSKEY_INFO("Account #%d No allow FWD to %s", iAccountID, logicData.strValue.c_str());
            return true;
        }

        // 是否更新界面（默认需要更新）
        bool bHandled = ProcessNoBindDsskey(focus, logicData);
    }
    else // 当前没有通话
    {
#ifdef IF_FEATURE_ALWAYS_SETFWD
        // PCCW特制
        if (configParser_GetConfigInt(kszEnableAlwaysSetFWD) != 0)
        {
            // Idle界面直接跳转到Frame进行设置
            AdapterBox_SettingUIEnterPage(MENU_PAGE_ALWAYS_FORWARD);
            return true;
        }
#endif
        ListAccounts accounts = acc_GetValidAccountList(ACCOUNT_TYPE_SIP);
        if (fwd_GetMode() == FWDDND_MODE_ACCOUNT)
        {
            // 账号模式：进入配置界面
            DSSKEY_INFO("It's account mode, show FWD config UI");
            AdapterBox_SettingUIEnterPage(MENU_SETTING_CUSTOM_FORWARD);
            return true;
        }
        else // FWDDND_MODE_GLOBAL
        {
            const int iAccountID = FWDDND_ACCOUNT_GLOBAL;
            if (logicData.strValue.empty())
            {
                if (fwd_GetInfo(iAccountID, FWD_TYPE_ALWAYS)
                        || fwd_GetInfo(iAccountID, FWD_TYPE_BUSY)
                        || fwd_GetInfo(iAccountID, FWD_TYPE_NO_ANSWER))
                {
                    // 如果有任意FWD类型启用，则还原(禁用)所有类型
                    DSSKEY_INFO("Restore all forward config");
                    return fwd_RestoreInfo(iAccountID, FWD_TYPE_ALL);
                }
                else // 如果没有任何FWD类型启用，则恢复上次启用的类型
                {
                    // 获取全局模式最后一次修改配置时启用的FWD类型
                    int iLastSavedType = fwd_GetLastSavedType();
                    DSSKEY_INFO("Last saved forward type: %d", iLastSavedType);
                    if (iLastSavedType <= 0 || iLastSavedType > FWD_TYPE_ALL)
                    {
                        iLastSavedType = FWD_TYPE_ALWAYS;
                    }
                    // 如果目标为空，则要切到配置界面去设置，此变量保存要切的类型
                    FWD_TYPE eSwitchType = FWD_TYPE_MANUAL;
                    FWD_TYPE arrFWDTypes[] = { FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER };
                    for (int i = 0; i < sizeof(arrFWDTypes) / sizeof(arrFWDTypes[0]); ++i)
                    {
                        if ((iLastSavedType & arrFWDTypes[i]) == 0)
                        {
                            continue;
                        }
                        yl::string strTarget;
                        fwd_GetInfo(iAccountID, arrFWDTypes[i], &strTarget);
                        if (strTarget.empty())
                        {
                            if (eSwitchType == FWD_TYPE_MANUAL)
                            {
                                eSwitchType = arrFWDTypes[i];
                            }
                        }
                        else
                        {
                            fwd_SetInfo(iAccountID, arrFWDTypes[i], true);
                        }
                    }
                    // 对于没有设置转移目标的类型，跳转到配置界面
                    if (eSwitchType != FWD_TYPE_MANUAL)
                    {
                        DSSKEY_INFO("FWD: FWDType[%d] Need Set Target !", eSwitchType);
                        switch (eSwitchType)
                        {
                        case FWD_TYPE_ALWAYS:
                            AdapterBox_SettingUIEnterPage(MENU_PAGE_ALWAYS_FORWARD);
                            break;
                        case FWD_TYPE_BUSY:
                            AdapterBox_SettingUIEnterPage(MENU_PAGE_BUSY_FORWARD);
                            break;
                        case FWD_TYPE_NO_ANSWER:
                            AdapterBox_SettingUIEnterPage(MENU_PAGE_NOANSWER_FORWARD);
                            break;
                        default:
                            DSSKEY_WARN("FWD: Invalid FWD Type !");
                        }
                    }
                }
            }
            else // DSSKey Value非空
            {
                yl::string strCurrentTarget;
                bool bCurrentEnable = fwd_GetInfo(iAccountID, FWD_TYPE_ALWAYS, &strCurrentTarget);
                if (strCurrentTarget == logicData.strValue && bCurrentEnable)
                {
                    // Target与Value相等并启用：还原配置
                    DSSKEY_INFO("Restore always forward config");
                    return fwd_RestoreInfo(iAccountID, FWD_TYPE_ALWAYS);
                }
                else
                {
                    // Target与Value不等或未启用：修改配置并启用
                    DSSKEY_INFO("Update always forward config");
                    return fwd_SetInfo(iAccountID, FWD_TYPE_ALWAYS, true,
                                       &(logicData.strValue), NULL, FWD_CFG_SRC_TYPE_DSSKEY);
                }
            }
        }
    }
    return false;
}

bool CDsskeyFwdAction::OnConfigChange(int dsskeyID, void* pExtraData)
{
    DSSKEY_INFO("FWD dsskey config changed: DsskeyID=%d", dsskeyID);
    UpdateDsskey(dsskeyID);
    return true;
}

bool CDsskeyFwdAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.GetRoutineState() == ROUTINE_RINGING)
    {
        if (OnActionSend(proxy, logicData.strValue))
        {
            return true;
        }

        if (!proxy.IsPreForword() && proxy.IsAllowFwd())
        {
            return talklogic_UIEvent(proxy, TALK_ACTION_FWD_TO);
        }
    }

    return false;
}

LRESULT CDsskeyFwdAction::OnFwdStatusChangeMsg(msgObject& msg)
{
    // FWD状态变化刷新DSSKey状态
    UpdateAllDsskeys();
    return TRUE;
}

LRESULT CDsskeyFwdAction::OnDefaultAccountChangedMsg(msgObject& msg)
{
    if (fwd_GetMode() == FWDDND_MODE_ACCOUNT)
    {
        // 账号模式下默认账号变化刷新DSSKey状态
        UpdateAllDsskeys();
    }
    return TRUE;
}

void CDsskeyFwdAction::UpdateAllDsskeys()
{
    typedef YLList<int> listID;
    listID list;
    if (dsskey_getAllDsskeys(list, DT_FWD)
            && list.size() > 0)
    {
        for (listID::iterator it = list.begin(); it != list.end(); it++)
        {
            UpdateDsskey(*it);
        }
    }
}

void CDsskeyFwdAction::UpdateDsskey(int dsskeyID)
{
    // 获取DSSKey Value
    yl::string strValue = dsskey_GetDsskeyValue(dsskeyID);
    DSSKEY_INFO("Update FWD dsskey: DsskeyID=%d value=%s", dsskeyID, strValue.c_str());
    // 获取当前FWD状态信息
    yl::string strTarget;
    int iAccountID = FWDDND_ACCOUNT_GLOBAL;
    if (fwd_GetMode() == FWDDND_MODE_ACCOUNT)
    {
        iAccountID = acc_GetDefaultAccount();
    }
    bool bAlwaysFwdEnable = fwd_GetInfo(iAccountID, FWD_TYPE_ALWAYS, &strTarget);
    DSSKEY_INFO("Always forward info for dsskey: account=%d enable=%d target=%s",
                iAccountID, bAlwaysFwdEnable, strTarget.c_str());
    // 判断是否需要亮灯
    if (!strValue.empty())
    {
        // DSSKey Value非空：Always FWD启用且Target相等才亮灯
        dsskey_UpdateFWDStatus(dsskeyID, bAlwaysFwdEnable && strValue == strTarget);
    }
    else
    {
        // 其他情况：根据全局/默认账号任意FWD是否启用决定是否亮灯
        dsskey_UpdateFWDStatus(dsskeyID,
                               bAlwaysFwdEnable || fwd_GetInfo(iAccountID, FWD_TYPE_BUSY)
                               || fwd_GetInfo(iAccountID, FWD_TYPE_NO_ANSWER));
    }
}

#endif  //IF_FEATURE_DSSKEY
