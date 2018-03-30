#ifdef IF_FEATURE_PSTN
#include "pstnidlehandle.h"

#include "idle/idle_data_define.h"
#include "translateiddefine.h"

#include "uilogiccommon/include/uilogiclanghelper.h"
#include "idlescreen/include/modidlescreen.h"
#include "account/include/modaccount.h"
#include "account/include/accountmessagedefine.h"
#include "pstnadapter/include/modpstnadapter.h"
#include "uilogichelper.h"
#include "modidleul.h"


CPstnHandle::CPstnHandle(int nType /*= PS_STATE_PSTN_ACCOUNT_NOTE*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CPstnHandle::ProcessPstnAccountMsg);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_CREATED,
                          &CPstnHandle::ProcessPstnAccountMsg);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_REMOVED, ACCOUNT_PSTN_REMOVED,
                          &CPstnHandle::ProcessPstnAccountMsg);

}

CPstnHandle::~CPstnHandle()
{
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                            &CPstnHandle::ProcessPstnAccountMsg);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_CREATED, ACCOUNT_PSTN_CREATED,
                            &CPstnHandle::ProcessPstnAccountMsg);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_REMOVED, ACCOUNT_PSTN_REMOVED,
                            &CPstnHandle::ProcessPstnAccountMsg);
}

bool CPstnHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_CONNECT_PSTN);

    const VEC_ID & vecId = GetPstnIdList();

    for (int i = 0; i < vecId.size(); ++i)
    {
        popData.m_strNote.append("  ");
        popData.m_strNote.append(acc_GetAccountShowText(vecId[i]));
    }

    popData.m_strNote.append("\n");
    popData.m_strNote.append(_UILOGIC_LANG_TRAN(TRID_ENTER_PSTN));
    popData.m_vecSoftkey[0] = ST_EXIT;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;
    return true;
}

bool CPstnHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        IdleUL_EnterPageByType(IEPT_PSTN_ACCOUNT_LIST);
        idleScreen_DropStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_DropStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
        ClearPstnIdList();
    }
    return true;
}

LRESULT CPstnHandle::ProcessPstnAccountMsg(msgObject & objMsg)
{
    IDLE_INFO("CPstnHandle::ProcessPstnAccountMsg %x %d  %d ", objMsg.message, objMsg.wParam,
              objMsg.lParam);
    if (ACCOUNT_PSTN_CREATED != objMsg.message
            && ACCOUNT_PSTN_REMOVED != objMsg.message
            && ACCOUNT_STATUS_CHANGED != objMsg.message)
    {
        return FALSE;
    }

    CBaseIdleHandle * pBaseIdleHandle = NULL;
    pBaseIdleHandle = GET_IDLE_HANDLE(PS_STATE_PSTN_ACCOUNT_NOTE);
    CPstnHandle * pPstnHandle = static_cast<CPstnHandle *>(pBaseIdleHandle);
    if (NULL == pPstnHandle)
    {
        return FALSE;
    }

    switch (objMsg.message)
    {
    case ACCOUNT_PSTN_CREATED:
        {
            pPstnHandle->AddPstnIdToList(objMsg.wParam);
            idleScreen_SetStatus(PS_STATE_PSTN_ACCOUNT_ICON);
            idleScreen_SetStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
            idleScreen_DropStatus(PS_STATE_USB_CONNECTING);
            etl_NotifyApp(FALSE, ACCOUNT_PSTN_STATUS_CHANGED, objMsg.wParam, 0);
        }
        break;
    case ACCOUNT_PSTN_REMOVED:
        {
            pPstnHandle->RemovePstnIdFromList(objMsg.wParam);
            const VEC_ID& vecId = pPstnHandle->GetPstnIdList();
            int nPSTNCnt = vecId.size();
            idleScreen_DropStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
            if (nPSTNCnt > 0)
            {
                idleScreen_SetStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
            }

            if (!acc_AccountNum(ACCOUNT_TYPE_PSTN))
            {
                idleScreen_DropStatus(PS_STATE_PSTN_ACCOUNT_ICON);
            }

            etl_NotifyApp(FALSE, ACCOUNT_PSTN_STATUS_CHANGED, objMsg.wParam, 0);
        }
        break;
    case ACCOUNT_STATUS_CHANGED:
        {
            if (acc_IsPstnAccount(objMsg.wParam))
            {
                int nPSTNCnt = acc_GetRegisteredAccountNum(ACCOUNT_TYPE_PSTN);
                IDLEUI_INFO("CPstnHandle::ProcessPstnAccountMsg nPSTNCnt is  %d", nPSTNCnt);
                if (nPSTNCnt > 0)
                {
                    idleScreen_SetStatus(PS_STATE_PSTN_ACCOUNT_ICON, "");
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_PSTN_ACCOUNT_ICON);
                }
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

const VEC_ID & CPstnHandle::GetPstnIdList()
{
    return m_vecAddPstnId;
}

void CPstnHandle::AddPstnIdToList(int nAccountId)
{
    RemovePstnIdFromList(nAccountId);
    m_vecAddPstnId.push_back(nAccountId);
}

void CPstnHandle::RemovePstnIdFromList(int nAccountId)
{
    for (int i = 0; i < m_vecAddPstnId.size(); ++i)
    {
        if (m_vecAddPstnId[i] == nAccountId)
        {
            m_vecAddPstnId.removeAt(i);
            break;
        }
    }
}

void CPstnHandle::ClearPstnIdList()
{
    m_vecAddPstnId.clear();
}

#endif
