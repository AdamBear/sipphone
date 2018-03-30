#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyBindAction::CDsskeyBindAction(DssKey_Type eDsskeyType)
    : CDssTalkBaseAciton(eDsskeyType)
{
    //pass
}

CDsskeyBindAction::~CDsskeyBindAction()
{
    //pass
}

bool CDsskeyBindAction::OnKeyLongPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                       void* pExtraData)
{
    //pass
    return true;
}

bool CDsskeyBindAction::OnBindedAction(int dsskeyDataID, const DssKeyLogicData& logicData,
                                       void* pExtraData)
{
    //pass
    return true;
}

bool CDsskeyBindAction::OnDebinded(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
    //更新状态会刷新界面，Talk未刷新导致空白，此处延时刷新
    dsskey_SetTimer(dsskeyDataID, 100);
    dsskey_RemoveStatus(dsskeyDataID, DS_SESSION_FIRST, DS_SESSION_END, false);
    return true;
}

bool CDsskeyBindAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    strCallOutNumber = logicData.strValue;
    return true;
}

bool CDsskeyBindAction::IsBindType()
{
    return true;
}

bool CDsskeyBindAction::CallOutByDsskey(const DssKeyLogicData& logicData)
{
    CSessionStateProxy focus;

    if (!focus || focus.IsHide())
    {
        return ProcessCallOut(logicData);
    }
    else
    {
        CSessionStateProxy bind = _DsskeyBindManager.GetSessionByDsskey(logicData.iDssKeyID);
        if (!bind)
        {
            if (!focus.IsAllowCreateNewCall())
            {
                return false;
            }

            if (!bind.IsTopestHostFocused())
            {
                return false;
            }

            // 由Session处理未绑定的Dsskey按键事件
            return ProcessNoBindDsskey(bind, logicData);
        }
        else
        {
            if (bind == focus)
            {
                if (!bind.IsTopestHostFocused())
                {
                    return false;
                }

                return ProcessFocusDsskey(bind, logicData);
            }
            else
            {
                return ProcessUnFocusDsskey(bind, logicData);
            }
        }
    }

    return false;
}

bool CDsskeyBindAction::CallOutByLine(const DssKeyLogicData& logicData)
{
    CSessionStateProxy focus;

    if (!focus || focus.IsHide())
    {
        return EnterDialUI(logicData);
    }
    else
    {
        CSessionStateProxy bind = _DsskeyBindManager.GetSessionByDsskey(logicData.iDssKeyID);
        if (!bind)
        {
            if (!bind.IsTopestHostFocused())
            {
                return false;
            }

            return ProcessNoBindDsskey(focus, logicData);
        }
        else
        {
            /* 交给top host session 处理 */
            if (bind.IsBla())
            {
                bind = bind.GetThisOrTopestHost();
            }

            if (focus == bind)
            {
                if (!bind.IsTopestHostFocused())
                {
                    return false;
                }

                return ProcessFocusDsskey(bind, logicData);
            }
            else
            {
                if (ProcessUnFocusDsskey(bind, logicData))
                {
                    bind.HoldSession(false);
                }

                return true;
            }
        }
    }

    return false;
}

bool CDsskeyBindAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_CONNECTING)
            || proxy.IsInherits(ROUTINE_RETRIEVE_CALLPARK))
    {
        return false;
    }

    /* 兼容原有逻辑，Pretran Pretrive需要特殊处理 */
    else if (proxy.IsInherits(ROUTINE_PRETRAN))
    {
        return OnActionSend(proxy, GetNumber(logicData));
    }
    else if (proxy.IsInherits(ROUTINE_DIALING))
    {
        return DialByCurrent(proxy, logicData);
    }
    else if (proxy.IsInherits(ROUTINE_RINGING))
    {
        if (proxy.IsPreForword())
        {
            return OnActionSend(proxy, GetNumber(logicData));
        }
        else
        {
            // 通过DSSKey的绑定呼出
            return ProcessCallOut(logicData);
        }

    }
    else if (proxy.IsInherits(ROUTINE_FINISHED))
    {
        proxy.ExitSession();
        return EnterDialUI(logicData);
    }
    else if (proxy.IsTalkSession()
             || proxy.IsInherits(ROUTINE_PARK_NOTIFY)
             || proxy.IsInherits(ROUTINE_AOC_FINISH))
    {
        return ProcessCallOut(logicData);
    }

    return true;
}

bool CDsskeyBindAction::ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    /* 兼容旧版本体验 */
    if (proxy.IsInherits(ROUTINE_RINGING))
    {
        return false;
    }
    else if (proxy.IsInherits(ROUTINE_AUTOREDIAL))
    {
        return false;
    }

    return proxy.ExitSession();
}

bool CDsskeyBindAction::ProcessUnFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    /* 兼容旧版本体验 */
    if (proxy.IsInherits(ROUTINE_DIALING)
            || proxy.IsInherits(ROUTINE_RINGING))
    {
        return false;
    }

    return proxy.SetFocus();
}

bool CDsskeyBindAction::OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData& logicData,
                                void* pExtraData)
{
    if (timerID == dsskey_GetRelatedTimerID(dsskeyDataID))
    {
        dsskey_KillTimer(dsskeyDataID);
        timerKillThreadTimer(timerID);
        return AfterDebind(dsskeyDataID, logicData, pExtraData);
    }
    return false;
}

bool CDsskeyBindAction::AfterDebind(int dsskeyDataID, const DssKeyLogicData& logicData,
                                    void* pExtraData)
{
    dsskey_refreshDSSKeyUI(dsskeyDataID);
    return false;
}

bool CDsskeyBindAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
        void* pExtraData)
{
    return true;
}

bool CDsskeyBindAction::OnConfigChange(int dsskeyID, void* pExtraData)
{
    return true;
}

#if IF_FEATRUE_PTT
bool CDsskeyBindAction::ExitSessionByDsskey(int iDsskey)
{
    return _DsskeyBindManager.GetSessionByDsskey(iDsskey).ExitSession();
}
#endif
#endif  //IF_FEATURE_DSSKEY
