#include "pickupmenuidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "idleuilogic/include/modidleul.h"

CPickupMenuHandle::CPickupMenuHandle(int nType /*= PS_STATE_PICKUP_MENU*/)
    : CBaseIdleHandle(nType)
{

}

CPickupMenuHandle::~CPickupMenuHandle()
{

}

bool CPickupMenuHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    //进入pickup界面
    popData.m_iType = IDLE_MSGBOX_TYPE_PICKUP_PAGE;
    idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, OnPickupMenuChange);
    return true;
}

bool CPickupMenuHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    return false;
}

void CPickupMenuHandle::OnPickupMenuChange(STATUS_ACTION eAction)
{
    //退出PickupMenu
    switch (eAction)
    {
    case STATUS_ACTION_DELETE:
        {
            IdleUL_EnterPageByType(IEPT_PICKUP_MENU_EXIT);
            idleScreen_SetStatusChangeCallback(PS_STATE_PICKUP_MENU, NULL);
        }
        break;
    case STATUS_ACTION_UPDATE:
        {
            IdleUL_EnterPageByType(IEPT_PICKUP_MENU_UPDATE);
        }
        break;
    default:
        break;
    }
}
