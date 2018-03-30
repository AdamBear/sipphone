#include "autopidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "idleuilogic/include/modidleul.h"

//////////////////CAutoPHandle//////////////////
CAutoPHandle::CAutoPHandle(int nType /*= PS_STATE_AUTO_PROVISION*/)
    : CBaseIdleHandle(nType)
{

}

CAutoPHandle::~CAutoPHandle()
{

}

bool CAutoPHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    // 输入了配置文件中的特征码
    yl::string strCodeName = Autop_AutopCodeGetCurrentName();

    if (strCodeName.empty())
    {
        return false;
    }

    popData.m_strNote = yl::string(_UILOGIC_LANG_TRAN(TRID_AUTOP_TO)) + yl::string(" ") +
                        (strCodeName) + yl::string("\n")
                        + yl::string(_UILOGIC_LANG_TRAN(TRID_IF_PROVISION_NOW));
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //messeagebox
    popData.m_vecSoftkey[0] =  ST_CANCEL;
    popData.m_vecSoftkey[1] =  ST_EMPTY;
    popData.m_vecSoftkey[2] =  ST_EMPTY;
    popData.m_vecSoftkey[3] =  ST_OK;
    return true;
}

bool CAutoPHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        //清除注册状态
        idleScreen_DropStatus(PS_STATE_AUTO_PROVISION);
        AutopStartCodeConnect();
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_DropStatus(PS_STATE_AUTO_PROVISION);
        bRet = true;
    }

    return bRet;
}

//////////////////CAutopErrorHandle//////////////////
CAutopErrorHandle::CAutopErrorHandle(int nType /*= PS_STATE_AUTO_PROVISION_ERROR*/)
    : CBaseIdleHandle(nType)
{

}

CAutopErrorHandle::~CAutopErrorHandle()
{

}

bool CAutopErrorHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
    popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_WRONG_USER_NAME_OR_PASSWORD);
    popData.m_strTitle = _UILOGIC_LANG_TRAN(TRID_NOTE);
    popData.m_iType = IDLE_MSGBOX_TYPE_WARNING;//tips
    popData.m_nId = MSGBOX_ID_NOTE;
    idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);
    return true;
}

bool CAutopErrorHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    unsigned int nNotifyType = GetNeedClearNotifyType();
    idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
    return true;
}
