#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

static Dsskey_Status MapLineState(LINE_STATE eState, int lineID)
{
    Dsskey_Status eLineStatus = DS_LINE_NONE;
    switch (eState)
    {
    case LS_REGISTERING:
    case LS_UNREGISTERING:
        {
            eLineStatus = DS_LINE_REGISTERING;
        }
        break;
    case LS_REGISTERED:
    case LS_DISABLE_REG:
        {
#ifdef IF_FEATURE_FORWARD_OPT
            FWD_TYPE eFWDType = FWD_TYPE_ALWAYS;
#else
            FWD_TYPE eFWDType = FWD_TYPE_ALL;
#endif
            //只有注册上了才显示DND和FWD图标
            if (fwd_GetMode() == FWDDND_MODE_ACCOUNT
                    && fwd_GetInfo(lineID, eFWDType))
            {
                eLineStatus = DS_LINE_FWD;
            }
            else if (dnd_GetMode() == FWDDND_MODE_ACCOUNT
                     && dnd_IsEnable(lineID))
            {
                eLineStatus = DS_LINE_DND;
            }
            else
            {
                eLineStatus = DS_LINE_REGISTERED;
            }
        }
        break;
    case LS_INEXISTENCE:
    case LS_UNKNOW:
        {
            eLineStatus = DS_LINE_NONE;
        }
        break;
    case LS_DISABLE:
        {
            eLineStatus = DS_LINE_DISABLE;
        }
        break;
    case LS_USING:
        {
            eLineStatus = DS_LINE_USING;
        }
        break;
    case LS_REGISTER_FAIL:
    case LS_BE_UNREGISTERED:
    default:
        {
            eLineStatus = DS_LINE_UNREGISTERED;
        }
        break;
    }

    return eLineStatus;
}

CDsskeyLineAction::CDsskeyLineAction()
    : CDsskeyBindAction(DT_LINE)
{
    // FWD&DND状态变化刷新LineKey状态
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, CDsskeyLineAction::OnFwdDndStatusChangeMsg);
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, CDsskeyLineAction::OnFwdDndStatusChangeMsg);
}

CDsskeyLineAction::~CDsskeyLineAction()
{
    SingleMsgUnReg(FWD_MESSAGE_STATUS_CHANGED, CDsskeyLineAction::OnFwdDndStatusChangeMsg);
    SingleMsgUnReg(DND_MESSAGE_STATUS_CHANGED, CDsskeyLineAction::OnFwdDndStatusChangeMsg);
}

bool CDsskeyLineAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
#if IF_FEATURE_BLOCK_LINE
    if (configParser_GetConfigInt(kszBlockLine) == 1)
    {
        return true;
    }
#endif //IF_FEATURE_BLOCK_LINE

    // 是否被处理
    bool bHandled = true;

    int iAccount = logicData.iLineIndex;

#if IF_FEATURE_32591
    if (!acc_IsAccountAvailable(iAccount))
    {
        DSSKEY_INFO("CDsskeyLineAction::OnKeyPress dsskey=[%d] account=[%d] not avail", dsskeyDataID,
                    iAccount);
        return false;
    }
#endif

    if (iAccount != INVALID_ACCOUNTID)
    {
#if IF_BUG_22849_SWICTH_ACCOUNT_BY_DSSKEY
        if (configParser_GetConfigInt(kszIsAutoChangeDefaultAccount) != 0
                && acc_IsAccountAvailable(iAccount))
        {
            acc_SetDefaultAccount(iAccount, true);
        }
#endif

        if (ProcessBLAKeyPress(iAccount, dsskeyDataID, logicData))
        {
            return true;
        }
    }

    return CallOutByLine(logicData);
}

// BLA按键处理
bool CDsskeyLineAction::ProcessBLAKeyPress(int iAccountID, int iDsskeyID, const DssKeyLogicData& logicData)
{
    // 找到与dsskey绑定的session，SCA有绑定话路时按原有linekey类型处理
    if (!_DsskeyBindManager.GetSessionByDsskey(iDsskeyID))
    {
        return false;
    }

    BLF_STATUS_CODE eStatus = (BLF_STATUS_CODE)(GetBlaStatus(iDsskeyID) - DS_BLA_STATUS_IDLE +
                              CN_BLA_STATUS_IDLE);
    int iBlaIndex = bsSCA_GetFirstIndexByStatus(eStatus, iAccountID, iDsskeyID);
    BLA_TYPE eBLAType = acc_GetBLAType(iAccountID);

    DSSKEY_INFO("ProcessBLAKeyPress type=[%d] eStatus=[%d] account=[%d] dsskey=[%d] index=[%d]",
                eBLAType, eStatus, iAccountID, iDsskeyID, iBlaIndex);
    switch (eBLAType)
    {
#ifdef IF_FEATURE_BROADSOFT_SCA
    case BLA_BROADSOFT_SCA:
        {
            switch (eStatus)
            {
            case CN_BLA_STATUS_SEIZED:
            case CN_BLA_STATUS_PROGRESSING:
                {
                    if (ProcessBeforeScaCallout(logicData))
                    {
                        // 创建拨号界面
                        EnterDialUI(logicData);
                    }
                    return true;
                }
                break;
            case CN_BLA_STATUS_BRIDGE_HELD:
            case CN_BLA_STATUS_ACTIVE:
            case CN_BLA_STATUS_BRIDGE_ACITVE:
                {
                    if (1 == configParser_GetCfgItemIntValue(kszAccountOneTouchBargein, iAccountID)
                            && 1 == configParser_GetCfgItemIntValue(kszAccountBargeInEnable, iAccountID))
                    {
                        return talklogic_CallOutBySCALine(iAccountID, iBlaIndex);
                    }
                    else
                    {
                        EnterDialUI(logicData);
                        return true;
                    }
                }
                break;
            case CN_BLA_STATUS_HELD:
                {
                    if (1 == configParser_GetCfgItemIntValue(kszAccountOneTouchRetrieve, iAccountID))
                    {
                        return talklogic_CallOutBySCALine(iAccountID, iBlaIndex);
                    }
                    else
                    {
                        EnterDialUI(logicData);
                        return true;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
#endif
    case BLA_DRAFT_BLA:
        {
            switch (eStatus)
            {
            case CN_BLA_STATUS_PROGRESSING:
            case CN_BLA_STATUS_SEIZED:
            case CN_BLA_STATUS_ACTIVE:
                {
                    return true;
                }
            case CN_BLA_STATUS_HELD:
                {
                    yl::string strNumber = bsSCA_GetBLANum(iAccountID);
                    talklogic_CallOutBySCALine(iAccountID, iBlaIndex, strNumber);
                    // 呼出失败没有刷新的问题，接口返回值不可靠
                    return true;
                }
            default:
                break;
            }
        }
        break;
    case BLA_GENBAND:
        {
            switch (eStatus)
            {
            case CN_BLA_STATUS_PROGRESSING:
            case CN_BLA_STATUS_ALERTING:
                {
                    return true;
                }
                break;
            // 通话中按下，直接BargeIn
            case CN_BLA_STATUS_ACTIVE:
            case CN_BLA_STATUS_HELD:
                {
                    if (eStatus == CN_BLA_STATUS_ACTIVE
                            && (configParser_GetCfgItemIntValue(kszAccountOneTouchBargein, iAccountID) == 0
                                || configParser_GetCfgItemIntValue(kszAccountBargeInEnable, iAccountID) == 0))
                    {
                        // Bridge需要开关控制，以优化体验
                        return true;
                    }

                    yl::string strNumber = bsSCA_GetBLANum(iAccountID);
                    talklogic_CallOutBySCALine(iAccountID, iBlaIndex, strNumber);
                    // 呼出失败没有刷新的问题，接口返回值不可靠
                    return true;
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

bool CDsskeyLineAction::AfterDebind(int dsskeyDataID, const DssKeyLogicData& logicData,
                                    void* pExtraData)
{
    UpdateLineInfo(logicData);
    return true;
}



void CDsskeyLineAction::UpdateLineInfo(const DssKeyLogicData& logicData)
{
    //lusy note 在DsskeyData有CorrectRealBindAccount
    //CorrectRealBindAccount(logicData);
    UpdateLineDsskeyStatus(logicData);
}

void CDsskeyLineAction::CorrectRealBindAccount(const DssKeyLogicData& logicData)
{
    //throw std::exception("The method or operation is not implemented.");
}

void CDsskeyLineAction::UpdateLineDsskeyStatus(const DssKeyLogicData& logicData)
{
    if (!acc_IsAccountIDValid(logicData.iLineIndex))
    {
        UpdateLineStatus(logicData, LS_INEXISTENCE);
        return;
    }

    UpdateLineStatus(logicData, acc_GetState(logicData.iLineIndex));
}

void CDsskeyLineAction::UpdateLineStatus(const DssKeyLogicData& logicData, LINE_STATE eLineState)
{
    // 如果账号可用,则判断当前是否有与之对应的session,如果有,则要更改相应的图标显示
    // 解决当通话中断网,又插上后,linekey图标未更改为正在使用中的图标
    // http://10.2.1.199/Bug.php?BugID=18348

    if (_DsskeyBindManager.GetSessionByDsskey(logicData.iDssKeyID)
            && eLineState != LS_USING)
    {
        eLineState = LS_USING;
    }

    Dsskey_Status eDsskeyStatus = MapLineState(eLineState, logicData.iLineIndex);
    dsskey_SetLineStatus(logicData.iDssKeyID, eDsskeyStatus);
    dsskey_refreshDSSKeyUI(logicData.iDssKeyID);
}


bool CDsskeyLineAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
        void* pExtraData)
{
    if (oldType == DT_LINE
            && pExtraData == NULL)
    {
        etl_NotifyApp(FALSE, DSSKEY_RELOAD_FINISHED, newType, oldType);
    }

    return true;
}

bool CDsskeyLineAction::OnConfigChange(int dsskeyID, void* pExtraData)
{
    int iLineID = dsskey_GetDsskeyLine(dsskeyID);
    dsskey_SetLineStatus(dsskeyID, MapLineState(acc_GetState(iLineID), iLineID));

    if (pExtraData == NULL)
    {
        etl_NotifyApp(FALSE, DSSKEY_RELOAD_FINISHED, 0, 0);
    }

    return true;
}

LRESULT CDsskeyLineAction::OnFwdDndStatusChangeMsg(msgObject& msg)
{
    // FWD&DND状态变化刷新LineKey状态
    const int iLineID = msg.wParam;
    DSSKEY_DEBUG("CDsskeyLineAction::OnFwdDndStatusChangeMsg account#%d", iLineID);
    YLList<int> dsskeys;
    dsskey_GetDsskeysByAccID(dsskeys, iLineID);
    for (YLList<int>::iterator it = dsskeys.begin(); it != dsskeys.end(); it++)
    {
        DSSKEY_DEBUG("Set DSSKey #%d status: %d", *it, MapLineState(acc_GetState(iLineID), iLineID));
        dsskey_SetLineStatus(*it, MapLineState(acc_GetState(iLineID), iLineID));
        dsskey_refreshDSSKeyUI(*it);
    }
    return TRUE;
}

bool CDsskeyLineAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    ROUTINE_STATE eRoutine = proxy.GetInheritsRoutine();
    if (ROUTINE_PARK_NOTIFY == eRoutine
#if IF_FEATURE_INCOMEING_SUPPORT_NEWCALL
            || ROUTINE_RINGING == eRoutine
#endif
            || ROUTINE_AOC_FINISH == eRoutine
            || proxy.IsTalkSession()
            || ROUTINE_AUTOREDIAL == proxy.GetRoutineState()
            || ROUTINE_PRETRAN == proxy.GetRoutineState())
    {
        return EnterDialUI(logicData);
    }
    else if (ROUTINE_DIALING == eRoutine)
    {
        return DialByCurrent(proxy, logicData);
    }
    else if (ROUTINE_FINISHED == eRoutine)
    {
        proxy.ExitSession();
        return EnterDialUI(logicData);
    }
    else if (ROUTINE_CONNECTING == eRoutine)
    {
        return true;
    }

    return false;
}

bool CDsskeyLineAction::ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_RINGING))
    {
        return talklogic_UIEvent(proxy, TALK_ACTION_ANSWER);
    }
    else if (proxy.IsTalkSession())
    {
        // 改变会议的hold状态
        proxy.HoldSession(!proxy.IsHold());

        if (proxy.IsHold())
        {
            EnterDialUI(logicData);
        }

        return true;
    }
    else if (proxy.IsInherits(ROUTINE_AOC_FINISH)
             || proxy.IsInherits(ROUTINE_PARK_NOTIFY)
             || proxy.IsInherits(ROUTINE_AUTOREDIAL))
    {
        return EnterDialUI(logicData);
    }

    return CDsskeyBindAction::ProcessFocusDsskey(proxy, logicData);
}

bool CDsskeyLineAction::ProcessUnFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_RINGING))
    {
        return talklogic_UIEvent(proxy, TALK_ACTION_ANSWER);
    }

    return CDsskeyBindAction::ProcessUnFocusDsskey(proxy, logicData);
}

int CDsskeyLineAction::GetBlaStatus(int iDsskeyID)
{
    YLList<Dsskey_Status> listOutput;
    Dsskey_Status eSCAStatus;
    dsskey_GetDsskeyStatus(iDsskeyID, listOutput);
    YLList<Dsskey_Status>::ListIterator iter = listOutput.begin();
    for (; iter != listOutput.end(); iter++)
    {
        if (*iter > DS_BLF_FIRST
                && *iter < DS_BLF_LAST)
        {
            eSCAStatus = *iter;
            break;
        }
    }

    return (int)eSCAStatus;
}

bool CDsskeyLineAction::ProcessBeforeScaCallout(const DssKeyLogicData& logicData)
{
    int iDsskey = logicData.iDssKeyID;
    if (iDsskey == INVALID_DSSKEY_ID)
    {
        return false;
    }

    //不允许则进入finish界面
    if (!_DsskeyBindManager.IsAllowNewCallByLineKey(iDsskey))
    {
        CallOut_Param calloutParam;
        calloutParam.iErrorInfo = TALK_ERROR_CALLOUT_SLU;
        calloutParam.eRoutineType = ROUTINE_FINISHED;
        talklogic_CreateSession(calloutParam);
        return false;
    }

    return true;
}

bool CDsskeyLineAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (DSSKEY_ACTION_MAKE_CALL == iActionType)
    {
        if (proxy.IsFocusSession() && proxy.IsHold())
        {
            /* 不需要删除 */
            if (!dsskey_IsSCATypeLinekey(logicData.iDssKeyID)
                    && _DsskeyBindManager.GetAccountMaxCallNumFromCFG(logicData.iLineIndex) > 0)
            {
                param.bDelOldLineSession = false;
            }
        }
    }

    return CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);
}

#endif  //IF_FEATURE_DSSKEY
