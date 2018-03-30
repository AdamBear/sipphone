#include "keypadlockidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "keyguard/include//keyguardmessagedefine.h"
#include "idle/idle_data_define.h"

#include "idlescreen/include/modidlescreen.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <configiddefine.h>
#include "idleuilogic/include/modidleul.h"

CKeypadLockHandle::CKeypadLockHandle(int nType /*= PS_STATE_LOCK*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED,
                          &CKeypadLockHandle::OnKeypadMessage);
    etl_RegisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED,
                          &CKeypadLockHandle::OnKeypadMessage);
    etl_RegisterMsgHandle(KEYGUARD_AUTOLOCK_TIME_CHANGED, KEYGUARD_AUTOLOCK_TIME_CHANGED,
                          &CKeypadLockHandle::OnKeypadMessage);
}

CKeypadLockHandle::~CKeypadLockHandle()
{
    etl_UnregisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED,
                            &CKeypadLockHandle::OnKeypadMessage);
    etl_UnregisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED,
                            &CKeypadLockHandle::OnKeypadMessage);
    etl_UnregisterMsgHandle(KEYGUARD_AUTOLOCK_TIME_CHANGED, KEYGUARD_AUTOLOCK_TIME_CHANGED,
                            &CKeypadLockHandle::OnKeypadMessage);
}

bool CKeypadLockHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_KEYPAD_LOCKED);
    popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
    popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //tips
    popData.m_nId = MSGBOX_ID_NOTE;
    popData.m_vecSoftkey.clear();
    unsigned int nNotifyType = GetNeedClearNotifyType();
    idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
    return true;
}

bool CKeypadLockHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    unsigned int nNotifyType = GetNeedClearNotifyType();
    if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
    }
    return false;
}

LRESULT CKeypadLockHandle::OnKeypadMessage(msgObject & objMessage)
{
    BOOL bRet = FALSE;
    switch (objMessage.message)
    {
    case KEYGUARD_LOCK_CHANGED:
        {
            if ((bool)objMessage.wParam)
            {
                idleScreen_SetStatus(PS_STATE_LOCK, "");
                // 上锁给予提示
            }
            else
            {
                idleScreen_DropStatus(PS_STATE_LOCK);
                // 解锁则需要重新开始计时
                IdleUL_ResetKeypadLock();
            }
            bRet = TRUE;
        }
        break;
    case KEYGUARD_ENABLE_CHANGED:
    case KEYGUARD_AUTOLOCK_TIME_CHANGED:
        {
            IdleUL_ResetKeypadLock();
            bRet = TRUE;
        }
        break;
    default:
        break;
    }
    return bRet;
}
