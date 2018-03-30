#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetoothidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "extended/bluetoothmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsklog/log.h"
#ifdef IF_OLD_BLUETOOTH
#include "wireless/bluetoothOld/include/modbluetooth.h"
#else
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif // IF_OLD_BLUETOOTH
#include "idleuilogic/include/modidleul.h"

CBlueToothHandle::CBlueToothHandle(int nType /*= PS_STATE_BLUETOOTH*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                          &CBlueToothHandle::OnBlueToothMessageMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE,
                          &CBlueToothHandle::OnBlueToothMessageMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_DONGLE_CHANGE, BLUETOOTH_MSG_DONGLE_CHANGE,
                          &CBlueToothHandle::OnBlueToothMessageMessage);
}

CBlueToothHandle::~CBlueToothHandle()
{
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                            &CBlueToothHandle::OnBlueToothMessageMessage);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE,
                            &CBlueToothHandle::OnBlueToothMessageMessage);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_DONGLE_CHANGE, BLUETOOTH_MSG_DONGLE_CHANGE,
                            &CBlueToothHandle::OnBlueToothMessageMessage);
}

LRESULT CBlueToothHandle::OnBlueToothMessageMessage(msgObject& msg)
{
    IDLEUI_INFO("OnBlueToothMsg: msg[%d], wParam[%d], lParam[%d].", msg.message, msg.wParam,
                msg.lParam);
    if (BLUETOOTH_MSG_CONNECT_STATE == msg.message)
    {
        if (1 == msg.wParam) //connected
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECTING);
            if (_ISBTPHONE(msg.lParam))
            {
                idleScreen_SetStatus(PS_STATE_BLUETOOTH_MOBILECONNECT);
            }
            else
            {
                idleScreen_SetStatus(PS_STATE_BLUETOOTH_CONNECT);
            }
        }
        else if (2 == msg.wParam) //connectting
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECT);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILECONNECT);

            idleScreen_SetStatus(PS_STATE_BLUETOOTH_CONNECTING);
        }
        else if (0 == msg.wParam) //disconnect
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECT);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_MOBILECONNECT);
            idleScreen_DropStatus(PS_STATE_BLUETOOTH_CONNECTING);
            if (Bluetooth_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_BLUETOOTH);
            }
        }
    }
    else if (BLUETOOTH_MSG_STATE_CHANGE == msg.message)
    {
        if (1 == msg.wParam) //ENABLE
        {
            idleScreen_SetStatus(PS_STATE_BLUETOOTH);
        }
        else if (0 == msg.wParam)//DISABLE
        {
            idleScreen_DropStatus(PS_STATE_BLUETOOTH);
        }
    }

#if IF_SUPPORT_EXTRA_IDLE_NOTE
    yl::string strNote = "";
    switch (msg.message)
    {
    case BLUETOOTH_MSG_DONGLE_CHANGE:
    case BLUETOOTH_MSG_STATE_CHANGE:
        {
            // dongle 插入或者开关状态改变
            if ((BLUETOOTH_MSG_DONGLE_CHANGE == msg.message
                    && 1 == msg.wParam
                    && Bluetooth_IsEnable())
                    || (BLUETOOTH_MSG_STATE_CHANGE == msg.message
                        && 1 == msg.wParam))
            {
                strNote = _UILOGIC_LANG_TRAN(TRID_BLUETOOTH_ON);
            }
        }
        break;
    case BLUETOOTH_MSG_CONNECT_STATE:
        {
            // 连接结果
            if (1 == msg.wParam)
            {
                if (BT_HEADSET == msg.lParam)
                {
                    strNote = _UILOGIC_LANG_TRAN(TRID_HEADSET_CONNECTED);
                }
                else
                {
                    strNote = _UILOGIC_LANG_TRAN(TRID_MOBILE_CONNECTED);
                }
            }
            else if (Bluetooth_IsEnable())
            {
                strNote = _UILOGIC_LANG_TRAN(TRID_BLUETOOTH_ON);
            }
        }
        break;
    default:
        break;
    }
    IdleUL_UpdateExtraNote(strNote);
#endif

    return TRUE;
}

#if IF_SUPPORT_BLUETOOTH_ADAPTER
CBlueToothAdapterHandle::CBlueToothAdapterHandle(int nType /*= PS_STATE_ADAPTER_ADD*/)
{

}

CBlueToothAdapterHandle::~CBlueToothAdapterHandle()
{

}

bool CBlueToothAdapterHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
    popData.m_vecSoftkey[0] = ST_CANCEL;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;

    if (PS_STATE_ADAPTER_ADD == m_iType)
    {
        popData.m_strNote = _UILOGIC_LANG_TRAN(
                                TRID_ADAPTER_HAS_ADDED_WHETHER_SCAN_BLUETOOTH_DEVICES_RIGHT_NOW);

    }
    else if (PS_STATE_ADAPTER_REMORE == m_iType)
    {
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_ADAPTER_HAS_REMOVED_OUT);
    }
    return true;
}

bool CBlueToothAdapterHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    if (PS_STATE_ADAPTER_ADD == m_iType)
    {
        if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode))
        {
            //清空弹出标志
            idleScreen_DropStatus(PS_STATE_ADAPTER_ADD);
            IdleUL_EnterPageByType(IEPT_BLUE_TOOTH);
            bRet = true;
        }
        else if (ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode)
        {
            //清空弹出标志
            idleScreen_DropStatus(PS_STATE_ADAPTER_ADD);
            bRet = true;
        }
    }
    else if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode
              || ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode)
             && PS_STATE_ADAPTER_REMORE == m_iType)
    {
        //清空弹出标志
        idleScreen_DropStatus(PS_STATE_ADAPTER_REMORE);
        bRet = true;
    }

    return bRet;
}
#endif

#endif
