#include "crtrequestidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "extended/actionuri_url/include/modactionuri_url.h"

CCRTRequestHandle::CCRTRequestHandle(int nType /*= PS_STATE_CTR_REQUEST*/)
    : CBaseIdleHandle(nType)
{

}

CCRTRequestHandle::~CCRTRequestHandle()
{

}

bool CCRTRequestHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    if (!accessCtr_NeedRequest())
    {
        return false;
    }

    // 弹出提示
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_IF_ALLOW_REMOTE_CONTROL);
    popData.m_strNote += " ?";
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
    popData.m_vecSoftkey[0] = ST_BLOCK;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;
    return true;
}

bool CCRTRequestHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        // 成功弹出提示后清除标志
        accessCtr_PopSuccess();
        idleScreen_DropStatus(m_iType);
        accessCtr_SetPermissions(TRUE);
        bRet = true;
    }
    else if (ST_BLOCK == eSoftkey || ST_CANCEL == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        // 成功弹出提示后清除标志
        accessCtr_PopSuccess();
        idleScreen_DropStatus(m_iType);
        accessCtr_SetPermissions(FALSE);
        bRet = true;
    }
    return bRet;
}

