#include "callforwardidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "popupboxmsgdefine.h"
#include "account/include/modaccount.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "idlescreen/include/modidlescreen.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <configiddefine.h>
#include "idleuilogic/include/modidleul.h"

CCallForwardHandle::CCallForwardHandle(int nType /*= PS_STATE_CALLFORWARD*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CCallForwardHandle::OnForwardMessage);
    etl_RegisterMsgHandle(LOCAL_CALLLOG_MESSAGE_FORWARD, LOCAL_CALLLOG_MESSAGE_FORWARD,
                          &CCallForwardHandle::OnForwardMessage);
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                          &CCallForwardHandle::OnForwardMessage);
    // 开启Multifwd弹窗提示
#ifdef IF_FEATURE_MULTIFWD
    SingleMsgReg(MULTIFWD_ENABLED_NOTICE, &CCallForwardHandle::OnForwardMessage);
#endif
    UpdateNotify(kszDisplayForwardCallPopup, PS_STATE_CALLFORWARD);
}

CCallForwardHandle::~CCallForwardHandle()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CCallForwardHandle::OnForwardMessage);
    etl_UnregisterMsgHandle(LOCAL_CALLLOG_MESSAGE_FORWARD, LOCAL_CALLLOG_MESSAGE_FORWARD,
                            &CCallForwardHandle::OnForwardMessage);
#ifdef IF_FEATURE_MULTIFWD
    etl_UnregisterMsgHandle(MULTIFWD_ENABLED_NOTICE, MULTIFWD_ENABLED_NOTICE,
                            &CCallForwardHandle::OnForwardMessage);
#endif
}

bool CCallForwardHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    IdleStatusItem StatusItems = idleScreen_GetTopStatus(SNT_POPUP);
    yl::string  strTarget = StatusItems.m_strHint;
    if (!strTarget.empty())
    {
#if IF_SUPPORT_MIDDLE_POPBOX
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUP_MIDDLE;
#else
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
#endif
        popData.m_strNote = yl::string(_UILOGIC_LANG_TRAN(TRID_FORWARD_TO)) + yl::string(": ") + strTarget;
        popData.m_vecSoftkey[0] = ST_EXIT;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_VIEW;
        return true;
    }
    return false;
}

bool CCallForwardHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    unsigned int nNotifyType = GetNeedClearNotifyType();
    if (ST_VIEW == eSoftkey || ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        idleScreen_DropStatus(PS_STATE_CALLFORWARD);
        IdleUL_EnterPageByType(IEPT_LOCAL_CALLLOG_CALL_FORWARD);
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
        IdleUL_ExitPopupBox();
        IdleUL_UpdateStatus();
        bRet = true;
    }
    return bRet;
}

#if IF_SUPPORT_TEXT_NOTIFY
bool CCallForwardHandle::GetTextNotifyData(ST_TextNotifyData& stNotifyData)
{
    IdleStatusItem StatusItems = idleScreen_GetTopStatus(SNT_TEXT_NOTIFY);
    yl::string  strTarget = StatusItems.m_strHint;
    if (!strTarget.empty())
    {
        stNotifyData.m_strNote = yl::string(_UILOGIC_LANG_TRAN(TRID_FORWARD_TO)) + yl::string(": ") + strTarget;
        return true;
    }
    return false;
}

bool CCallForwardHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    m_vecSoftkey.clear();
    m_vecSoftkey.push_back(ST_EXIT);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_VIEW);
    return true;
}
#endif

LRESULT CCallForwardHandle::OnForwardMessage(msgObject & msg)
{
    BOOL bRet = FALSE;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_VOICEMAIL_POPUP == msg.wParam)
            {
                UpdateNotify(kszDisplayForwardCallPopup, PS_STATE_CALLFORWARD);
                bRet = TRUE;
            }
        }
        break;
    case LOCAL_CALLLOG_MESSAGE_FORWARD:
        {
            //forward 数量为0时删除状态
            if (0 == msg.lParam)
            {
                idleScreen_DropStatus(PS_STATE_CALLFORWARD);
            }
            bRet = TRUE;
        }
        break;
#ifdef IF_FEATURE_MULTIFWD
    case MULTIFWD_ENABLED_NOTICE:
        {
            yl::string strMsg = (LPCSTR)msg.GetExtraData();
            PopupBoxInfo popboxInfo;
            popboxInfo.m_iType = IDLE_MSGBOX_TYPE_NOTE; //tips
            popboxInfo.m_nTimes = 1000;
            popboxInfo.m_strNote = yl::string(_UILOGIC_LANG_TRAN(TRID_ALWAYS_FORWARD_TO)) + strMsg;
            IdleUL_PopupBox(popboxInfo);
            bRet = TRUE;
        }
        break;
#endif
    case ACCOUNT_DEFAULT_CHANGED:
        {
            // 默认账号变化，需要重新设置FWD&DND状态
            if (FWDDND_MODE_ACCOUNT == fwd_GetMode())
            {
                if (fwd_GetInfo(acc_GetDefaultAccount(), FWD_TYPE_ALL))
                {
                    idleScreen_SetStatus(PS_STATE_FWD);
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_FWD);
                }
            }
        }
        break;
    default:
        break;
    }//end of switch
    return bRet;
}
