#include "usbidlerecordidlehandle.h"
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

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)

CUSBIdleRecordHandle::CUSBIdleRecordHandle(int nType /*= PS_STATE_IDLE_RECORD*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(RECORD_STORAGE_RECORD_RESULT, RECORD_STORAGE_RECORD_RESULT,
                          &CUSBIdleRecordHandle::OnUSBReocrdMessage);
    etl_RegisterMsgHandle(RECORD_STORAGE_VOLUME_WARNING, RECORD_STORAGE_VOLUME_WARNING,
                          &CUSBIdleRecordHandle::OnUSBReocrdMessage);
}

CUSBIdleRecordHandle::~CUSBIdleRecordHandle()
{
    etl_UnregisterMsgHandle(RECORD_STORAGE_RECORD_RESULT, RECORD_STORAGE_RECORD_RESULT,
                            &CUSBIdleRecordHandle::OnUSBReocrdMessage);
    etl_UnregisterMsgHandle(RECORD_STORAGE_VOLUME_WARNING, RECORD_STORAGE_VOLUME_WARNING,
                            &CUSBIdleRecordHandle::OnUSBReocrdMessage);
}

bool CUSBIdleRecordHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    if (PS_STATE_USB_RECORD_SUCCESS == m_iType)
    {
        popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
        popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_RECORD_FILE_SAVED);
        idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);
    }
#warning PS_STATE_IDLE_RECORD_WARNING
    //else if (PS_STATE_IDLE_RECORD_WARNING == m_iType)
    //{
    //    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //提示
    //    popData.m_strNote = m_strHint + " " + _UILOGIC_LANG_TRAN(TRID_MINUTES_LEFT_FOR_CALL_RECORDING);
    //    popData.m_vecSoftkey[0] = ST_CANCEL;
    //    popData.m_vecSoftkey[1] = ST_EMPTY;
    //    popData.m_vecSoftkey[2] = ST_EMPTY;
    //    popData.m_vecSoftkey[3] = ST_EMPTY;
    //}
    return true;
}

LRESULT CUSBIdleRecordHandle::OnUSBReocrdMessage(msgObject & msg)
{
    if (RECORD_STORAGE_RECORD_RESULT == msg.message)
    {
        IDLEUI_INFO("CUSBIdleRecordHandle::OnUSBMessage record result wParam[%d] lParam[%d]", msg.wParam,
                    msg.lParam);
        // 不是在通话界面收到录制成功的消息，需要提示录制成功的弹出框set一个状态进去
        if (!AdatapterBox_IsInTalkUIFrame() && 1 == msg.wParam && 1 == msg.lParam)
        {
            idleScreen_SetStatus(PS_STATE_USB_RECORD_SUCCESS);
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD);
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD_PAUSE);
        }
        else if (0 == msg.wParam && 0 == msg.lParam)
        {
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD);
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD_PAUSE);

            PopupBoxInfo popData;
            popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
            popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
            popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_USB_RECORD_FAILED);
            IdleUL_PopupBox(popData);
        }
        else if ((0 == msg.wParam && 1 == msg.lParam)
                 || (4 == msg.wParam && 1 == msg.lParam))
        {
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD_PAUSE);
            idleScreen_SetStatus(PS_STATE_IDLE_RECORD);
        }
        else if (1 == msg.wParam && 0 != msg.lParam)
        {
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD);
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD_PAUSE);
        }
        else if (3 == msg.wParam && 1 == msg.lParam)
        {
            idleScreen_DropStatus(PS_STATE_IDLE_RECORD);
            idleScreen_SetStatus(PS_STATE_IDLE_RECORD_PAUSE);
        }
        IdleUL_UpdateSoftkey();
    }
    else if (RECORD_STORAGE_VOLUME_WARNING == msg.message && AdapterBox_IsInIdle())
    {
        //提示上来的是 以秒为单位的 所以 要除60
        int iTime = msg.wParam / 60;
        yl::string strHint = commonAPI_FormatString("%d", iTime);
#warning PS_STATE_IDLE_RECORD_WARNING
        //idleScreen_SetStatus(PS_STATE_IDLE_RECORD_WARNING, strHint);
    }
    else if (RECORD_STORAGE_SPACE_NOTENOUGH == msg.message)
    {
        PopupBoxInfo popData;
        popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
        popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_SPACE_FULL_SAVE_FILE);
        IdleUL_PopupBox(popData);
    }
    return true;
}

bool CUSBIdleRecordHandle::GetIconInfo(IdleIconInfo & IconData)
{
    CBaseIdleHandle::GetIconInfo(IconData);
    if (PS_STATE_IDLE_RECORD == m_iType)
    {
        IconData.m_bFlash = true;
    }
    return true;
}

bool CUSBIdleRecordHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
#warning PS_STATE_IDLE_RECORD_WARNING
    //if ((ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode)
    //        && PS_STATE_IDLE_RECORD_WARNING == m_iType)
    //{
    //    idleScreen_DropStatus(PS_STATE_IDLE_RECORD_WARNING);
    //    bRet = true;
    //}
    return bRet;
}

#endif
