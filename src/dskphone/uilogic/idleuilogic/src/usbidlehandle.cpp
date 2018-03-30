#include "usbidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include <configiddefine.h>
#include "idle/idle_data_define.h"

#include "configparser/modconfigparser.h"
#include "record/include/recordmessagedefine.h"
#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "adapterbox/include/modadapterbox.h"
#include "hotplug/commondefine.h"
#include "record/include/modrecord.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "idleuilogic/include/modidleul.h"
#include "dsklog/log.h"

#ifdef IF_USB_SUPPORT

CUSBHandle::CUSBHandle(int nType /*= PS_STATE_USB_DISCONNECT*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE,
                          &CUSBHandle::OnUSBMessage);
#ifndef _WIN32
#warning
#endif
//  etl_RegisterMsgHandle(HOTPLUG_USB_STORAGE_CURRENT_OVER, HOTPLUG_USB_STORAGE_CURRENT_OVER, &CUSBHandle::OnUSBMessage);
}

CUSBHandle::~CUSBHandle()
{
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE,
                            &CUSBHandle::OnUSBMessage);
#ifndef _WIN32
#warning
#endif
//  etl_UnregisterMsgHandle(HOTPLUG_USB_STORAGE_CURRENT_OVER, HOTPLUG_USB_STORAGE_CURRENT_OVER, &CUSBHandle::OnUSBMessage);
}

bool CUSBHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
    if (PS_STATE_USB_CONNECT == m_iType)
    {
        popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_USB_DEVICE_CONNECTED);
        popData.m_nId = MSGBOX_ID_NOTE;
        if (0 != configParser_GetConfigInt(kszUSBRecordEnable)
                && talklogic_SessionExist())
        {
            popData.m_strNote += "\n";
            popData.m_strNote += _UILOGIC_LANG_TRAN(TRID_RECORD_TIME);
            popData.m_strNote += ":";
            yl::string strTime = modRecord_GetAvailableRecordingTime();
            popData.m_strNote.append(strTime.c_str());
        }
        idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);
    }
    else if (PS_STATE_USB_DISCONNECT == m_iType)
    {
        popData.m_iType = IDLE_MSGBOX_TYPE_WARNING; //警告
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_USB_DEVICE_REMOVED);
        popData.m_nId = MSGBOX_ID_NOTE;
        idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);
    }
#warning PS_STATE_USB_CURRENT_OVERLOAD
//    else if (PS_STATE_USB_CURRENT_OVERLOAD == m_iType)
//    {
//        popData.m_vecSoftkey[0] = ST_EMPTY;
//        popData.m_vecSoftkey[1] = ST_EMPTY;
//        popData.m_vecSoftkey[2] = ST_EMPTY;
//        popData.m_vecSoftkey[3] = ST_OK;
//        popData.m_nTimes = -1;
//        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //警告
//#ifndef _WIN32
//#warning
//#endif
//        popData.m_strNote = "";// = _UILOGIC_LANG_TRAN(TRID_USB_CURRENT_OVERLOAD);
//    }
    else if (PS_STATE_USBDEVICE_DETECTFAILED == m_iType)
    {
        popData.m_vecSoftkey[0] = ST_EMPTY;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_OK;
        popData.m_nTimes = -1;
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //警告
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_NEW_USB_DETECT_FAILED);
    }
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
//    else if (PS_STATE_USBDEVICE_NOT_SUPPORT == m_iType)
//    {
//        popData.m_vecSoftkey[0] = ST_EMPTY;
//        popData.m_vecSoftkey[1] = ST_EMPTY;
//        popData.m_vecSoftkey[2] = ST_EMPTY;
//        popData.m_vecSoftkey[3] = ST_OK;
//        popData.m_nTimes = -1;
//        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //警告
//#ifndef _WIN32
//#warning
//#endif
//        popData.m_strNote = "";// = _UILOGIC_LANG_TRAN(TRID_NEW_USB_NOT_SUPPROT);
//    }

    return true;
}

bool CUSBHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if (PS_STATE_USB_DISCONNECT == m_iType
            || PS_STATE_USB_RECORD_SUCCESS == m_iType)//断开连接删除状态,保存成功也删除
    {
        idleScreen_DropStatus(m_iType);
    }
#warning PS_STATE_USB_CURRENT_OVERLOAD
    //else if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode)
    //         && PS_STATE_USB_CURRENT_OVERLOAD == m_iType)
    //{
    //    idleScreen_DropStatus(PS_STATE_USB_CURRENT_OVERLOAD);
    //}
    else if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode)
             && PS_STATE_USBDEVICE_DETECTFAILED == m_iType)
    {
        idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTFAILED);
    }
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
    //else if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode)
    //         && PS_STATE_USBDEVICE_NOT_SUPPORT == m_iType)
    //{
    //    idleScreen_DropStatus(PS_STATE_USBDEVICE_NOT_SUPPORT);
    //}
    return true;
}

LRESULT CUSBHandle::OnUSBMessage(msgObject & msg)
{
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    IDLEUI_INFO("OnUSBMessage : msg[0x%x], wParam[%d], lParam[%d].", msg.message, msg.wParam,
                msg.lParam);
    if (RECORD_STORAGE_STATE_CHANGE == msg.message)
    {
        if (STATE_DISCONNECT == msg.wParam)//断开
        {
            if (STATE_CONNECT == msg.lParam)
            {
                idleScreen_SetStatus(PS_STATE_USB_DISCONNECT);
            }
            idleScreen_DropStatus(PS_STATE_USB_CONNECT);
            idleScreen_DropStatus(PS_STATE_USB_CONNECTING);
            idleScreen_DropStatus(PS_STATE_USBDEVICE_DETECTFAILED);
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
            //idleScreen_DropStatus(PS_STATE_USBDEVICE_NOT_SUPPORT);
        }
        else if (STATE_CONNECT == msg.wParam)//连接上
        {
            idleScreen_SetStatus(PS_STATE_USB_CONNECT);
            idleScreen_DropStatus(PS_STATE_USB_DISCONNECT);
            idleScreen_DropStatus(PS_STATE_USB_CONNECTING);
        }
        else if (STATE_DETECTING == msg.wParam)//检测中
        {
            idleScreen_SetStatus(PS_STATE_USB_CONNECTING);
            idleScreen_DropStatus(PS_STATE_USB_DISCONNECT);
            idleScreen_DropStatus(PS_STATE_USB_CONNECT);
        }
        else if (STATE_UNRECOGNIZED == msg.wParam)
        {
            // 取消显示图标
            idleScreen_DropStatus(PS_STATE_USB_CONNECT);
            idleScreen_DropStatus(PS_STATE_USB_CONNECTING);
            // 显示识别失败
            idleScreen_SetStatus(PS_STATE_USBDEVICE_DETECTFAILED);
        }
#ifndef _WIN32
#warning
#endif
        else if (false)//STATE_MOUNT_FAIL == msg.wParam)
        {
            // 取消显示图标
            idleScreen_DropStatus(PS_STATE_USB_CONNECT);
            idleScreen_DropStatus(PS_STATE_USB_CONNECTING);
            // 显示识别失败
#warning PS_STATE_USBDEVICE_NOT_SUPPORT
            //idleScreen_SetStatus(PS_STATE_USBDEVICE_NOT_SUPPORT);
        }
        else if (STATE_OTHER_DEVICE == msg.wParam)
        {
            // 取消显示图标
            idleScreen_DropStatus(PS_STATE_USB_CONNECT);
            idleScreen_DropStatus(PS_STATE_USB_CONNECTING);
        }
    }
#ifndef _WIN32
#warning
#endif
    else if (false)//HOTPLUG_USB_STORAGE_CURRENT_OVER == msg.message)
    {
#warning PS_STATE_USB_CURRENT_OVERLOAD
//#ifndef _WIN32
//#warning
//#endif
//        if (false)//USB_CURRENT_STATE_NORMAL == msg.wParam)
//        {
//            idleScreen_DropStatus(PS_STATE_USB_CURRENT_OVERLOAD);
//        }
//#ifndef _WIN32
//#warning
//#endif
//        else if (false)//USB_CURRENT_STATE_OVER == msg.wParam)
//        {
//            idleScreen_SetStatus(PS_STATE_USB_CURRENT_OVERLOAD);
//        }
    }

#if IF_SUPPORT_EXTRA_IDLE_NOTE
    yl::string strNote = "";
    bool bUpdate = false;
    switch (msg.message)
    {
    case RECORD_STORAGE_STATE_CHANGE:
        {
            // u盘连接上
            if (STATE_CONNECT == msg.wParam)
            {
                strNote = _UILOGIC_LANG_TRAN(TRID_USB_CONNECTED);
                bUpdate = true;
            }
            else if (STATE_DISCONNECT == msg.wParam
                     && (STATE_CONNECT == msg.lParam || STATE_DETECTING == msg.lParam))
            {
                //从连接上到断开
                bUpdate = true;
            }
            else if ((STATE_UNRECOGNIZED == msg.wParam || STATE_OTHER_DEVICE)
                     && STATE_CONNECT != msg.lParam)
            {
                //识别失败，弹窗提示
                bUpdate = true;
            }
        }
        break;
    default:
        break;
    }

    if (bUpdate)
    {
        IdleUL_UpdateExtraNote(strNote);
    }
#endif
#endif
    return true;
}

bool CUSBHandle::GetIconInfo(IdleIconInfo & IconData)
{
    CBaseIdleHandle::GetIconInfo(IconData);
    if (PS_STATE_USB_CONNECTING == m_iType)
    {
        IconData.m_bFlash = true;
    }
    return true;
}
#endif
