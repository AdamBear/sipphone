#include "e911locationidlehandle.h"

#if IF_FEATURE_GENBAND_E911
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsklog/log.h"
#include "e911address/include/e911address_def.h"
#include "e911address/include/mode911address.h"
#include "idleuilogic/include/modidleul.h"

namespace
{
#define E911_SET_LOCATION_POPUP_BOX_SHOWING_TIME      10000
}

CE911LocationHandle::CE911LocationHandle(int nType /*= PS_STATE_E911_LOCATION*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(E911_MESSAGE_BEGIN, E911_MESSAGE_END,
                          &CE911LocationHandle::OnE911LocationMessage);
}

CE911LocationHandle::~CE911LocationHandle()
{
    etl_UnregisterMsgHandle(E911_MESSAGE_BEGIN, E911_MESSAGE_END,
                            &CE911LocationHandle::OnE911LocationMessage);
}

bool CE911LocationHandle::GetPopupBoxInfo(PopupBoxData& popData)
{
    if (popData.m_vecSoftkey.size() < MIN_IDLE_MESSAGE_BOX_SOFTKEY_NUM)
    {
        return false;
    }

    popData.m_vecSoftkey[0] = ST_CANCEL;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //
    popData.m_strTitle = _UILOGIC_LANG_TRAN(TRID_E911);
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_SET_YOU_LOCATION);
    popData.m_nTimes = E911_SET_LOCATION_POPUP_BOX_SHOWING_TIME;
    return true;
}

bool CE911LocationHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        // 成功弹出提示后清除标志
        idleScreen_DropStatus(m_iType);
        IdleUL_EnterPageByType(IEPT_E911_SET_LOCATION);
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        e911_ClearListData();
        idleScreen_DropStatus(m_iType);
        bRet = true;
    }

    return bRet;
}

LRESULT CE911LocationHandle::OnE911LocationMessage(msgObject& msg)
{
    if (E911_MESSAGE_DOWNLOAD_ADDRESS != msg.message)
    {
        return FALSE;
    }

    idleScreen_SetStatus(PS_STATE_E911_LOCATION);
    return TRUE;
}

#endif
