#include "balanceidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include <configiddefine.h>
#include "idle/idle_data_define.h"

#include "configparser/modconfigparser.h"
#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "dsklog/log.h"
#include "idleuilogic/include/modidleul.h"

#ifdef IF_FEATURE_GETBALANCE

namespace
{
#define BALANCE_POPUP_BOX_SHOWING_TIME      5000
}

CBalanceHandle::CBalanceHandle(int nType /*= PS_STATE_BALANCE_LOADING*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(GET_BALANCE_MESSAGE_BEGIN, GET_BALANCE_MESSAGE_END,
                          &CBalanceHandle::OnBalanceMessage);
}

CBalanceHandle::~CBalanceHandle()
{
    etl_UnregisterMsgHandle(GET_BALANCE_MESSAGE_BEGIN, GET_BALANCE_MESSAGE_END,
                            &CBalanceHandle::OnBalanceMessage);
}

bool CBalanceHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_nTimes = BALANCE_POPUP_BOX_SHOWING_TIME;
    if (PS_STATE_BALANCE_LOADING == m_iType)
    {
        popData.m_vecSoftkey[0] = ST_CANCEL;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_EMPTY;
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_GETTING_BALENCE);
        popData.m_nTimes = -1;
    }
    else if (PS_STATE_BALANCE_RESULT == m_iType)
    {
        Balance_Info_t balanceInfo;
        memset(&balanceInfo, 0, sizeof(balanceInfo));
        // 获取余额信息
        acc_GetBalanceInfo(balanceInfo);
        popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
        popData.m_strNote = balanceInfo.szBalance;
        idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);
    }
    else if (PS_STATE_BALANCE_LOAD_ERROR == m_iType)
    {
        popData.m_vecSoftkey[0] = ST_CANCEL;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_EMPTY;
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //警告
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_GET_BALANCE_FAILED);
    }

    return true;
}

bool CBalanceHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if ((ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode) &&
            (PS_STATE_BALANCE_LOADING == m_iType || PS_STATE_BALANCE_LOAD_ERROR == m_iType))
    {
        idleScreen_DropStatus(m_iType);
    }
    return true;
}

LRESULT CBalanceHandle::OnBalanceMessage(msgObject & msg)
{
    BOOL bRet = TRUE;
    idleScreen_DropStatus(PS_STATE_BALANCE_LOADING);
    switch (msg.message)
    {
    case GET_BALANCE_MESSAGE_LOADING:
    {
        // 正在下载
        idleScreen_SetStatus(PS_STATE_BALANCE_LOADING);
    }
    break;
    case GET_BALANCE_MESSAGE_GET_RESULT:
    {
        Balance_Info_t balanceInfo;
        memset(&balanceInfo, 0, sizeof(balanceInfo));
        // 获取余额信息
        acc_GetBalanceInfo(balanceInfo);
        if (0 == balanceInfo.iResult)
        {
            // 进入余额信息显示界面
            IdleUL_EnterPageByType(IEPT_BALANCE);
        }
        else
        {
            idleScreen_SetStatus(PS_STATE_BALANCE_RESULT);
        }
    }
    break;
    case GET_BALANCE_MESSAGE_LOAD_ERROR:
    {
        IDLEUI_WARN("Load balance faild....");
        idleScreen_SetStatus(PS_STATE_BALANCE_LOAD_ERROR);
    }
    break;
    case GET_BALANCE_MESSAGE_CLEAN_TIPS:
    {
        // clean Tip仅需进行去除现有的下载余额信息提示框即可
        bRet = TRUE;
    }
    break;
    default:
    {
        bRet = FALSE;
    }
    break;
    }
    return bRet;
}

#endif
