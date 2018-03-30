#ifdef IF_SUPPORT_WIFI
#include "wifiidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "extended/wifimessagedefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsklog/log.h"
#include "wireless/wifi/include/modwifi.h"

#include "idleuilogic/include/modidleul.h"
#if IF_SUPPORT_EXTRA_IDLE_NOTE
#include "service_system.h"
#endif

CWifiHandle::CWifiHandle(int nType /*= PS_STATE_WIFI*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, &CWifiHandle::OnWifiMessageMessage);
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    etl_RegisterMsgHandle(SYS_MSG_NET_WOULD_CHANGED, SYS_MSG_NET_WOULD_CHANGED, &CWifiHandle::OnWifiMessageMessage);
#endif
}

CWifiHandle::~CWifiHandle()
{
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, &CWifiHandle::OnWifiMessageMessage);
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    etl_UnregisterMsgHandle(SYS_MSG_NET_WOULD_CHANGED, SYS_MSG_NET_WOULD_CHANGED, &CWifiHandle::OnWifiMessageMessage);
#endif
}

bool CWifiHandle::GetPopupBoxInfo(PopupBoxData& popData)
{
    if (PS_STATE_WIFI_CONNECTOVERTIME == m_iType)
    {
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_OVERTIME);
        popData.m_vecSoftkey[0] = ST_CANCEL;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_OK;
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //messagebox
    }
    else if (PS_STATE_WIFI_CONNECT == m_iType)
    {
        popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_CONNECTED);
        popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //messagebox
    }
    return true;
}

bool CWifiHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if (PS_STATE_WIFI_CONNECTOVERTIME == m_iType)//超时状态
    {
        bool bRet = false;
        if (ST_OK == eSoftkey
                || PHONE_KEY_OK == eKeyCode)
        {
            // 进入wifi设置界面,并清除弹出状态
            IdleUL_EnterPageByType(IEPT_WIFI_MENU);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
        }
        else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
                 || PHONE_KEY_CANCEL == eKeyCode)
        {
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            bRet = true;
        }
        return bRet;
    }
    return false;
}

LRESULT CWifiHandle::OnWifiMessageMessage(msgObject& msg)
{
    IDLE_INFO("\n WifiMessageMessage %d  %d  \n ", msg.wParam, msg.lParam);
    if (WIFI_MSG_UPDATE_UI == msg.message)
    {
        if (OP_WIFI_OPEN_RESULT == msg.wParam
                && (WIRELESS_OP_OPEN_RESULT_SUCCESS == msg.lParam
                    || WIRELESS_OP_OPEN_RESULT_ING == msg.lParam))
        {
            // 开启成功
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
        }
        if (OP_WIFI_CLOSE_RESULT == msg.wParam)
        {
            //关闭成功
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
        }
        else if (OP_CONNECT_WIFI_RESULT == msg.wParam
                 && WIRELESS_OP_CONNECTING == msg.lParam)
        {
            //连接中
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI);
            idleScreen_SetStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
        }
        else if (OP_CONNECT_WIFI_RESULT == msg.wParam
                 && WIRELESS_OP_CONNECT_SUCCESS == msg.lParam)
        {
            //连接成功
            idleScreen_DropStatus(PS_STATE_WIFI);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_SetStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
        }
        else if (OP_CONNECT_WIFI_RESULT == msg.wParam
                 && WIRELESS_OP_CONNECT_FAILED == msg.lParam)
        {
            //连接失败
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            if (WIFI_IsEnable())
            {
                //http://10.2.1.199/Bug.php?BugID=74877
                idleScreen_DropStatus(PS_STATE_WIFI);
                idleScreen_SetStatus(PS_STATE_WIFI_CONNECT_FAIL);
            }
        }
        else if (OP_DISCONNECT_WIFI_RESULT == msg.wParam
                 && WIRELESS_OP_DISCONNECT_SUCCESS == msg.lParam)
        {
            //断开成功
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
        }
        else if (OP_AUTOCONNECT_TIMEROUT == msg.wParam)
        {
            //wifi连接超时
            IDLE_INFO("wifi over time ");
            idleScreen_SetStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
        }
        else if (WIRELESS_OP_CONNECT_CANCLE == msg.lParam)
        {
            IDLE_INFO("cancel wifi connect ");
            if (WIFI_IsEnable())
            {
                idleScreen_SetStatus(PS_STATE_WIFI);
            }
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTTING);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECTOVERTIME);
            idleScreen_DropStatus(PS_STATE_WIFI_CONNECT_FAIL);
        }
    }

#if IF_SUPPORT_EXTRA_IDLE_NOTE
    yl::string strNote = "";
    bool bUpdate = false;
    switch (msg.message)
    {
    case WIFI_MSG_UPDATE_UI:
        {
            // dongle状态改变
            if (OP_WIFI_DONGLE_STATUS_MODIFY == msg.wParam)
            {
                if (WIRELESS_OP_DONGLE_ADD_IGNORE_SWITCH == msg.lParam)
                {
                    if (WIFI_IsEnable())
                    {
                        strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_ON);
                    }
                    bUpdate = true;
                }
                else if (WIRELESS_OP_DONGLE_REMOVE == msg.lParam)
                {
                    bUpdate = true;
                }
            }
            // 开关结果
            else if (OP_WIFI_OPEN_RESULT == msg.wParam)
            {
                strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_ON);
                bUpdate = true;
            }
            else if (OP_WIFI_CLOSE_RESULT == msg.wParam)
            {
                bUpdate = true;
            }
            // 连接结果
            else if (OP_CONNECT_WIFI_RESULT == msg.wParam)
            {
                if (WIRELESS_OP_CONNECT_SUCCESS == msg.lParam)
                {
                    /**
                    * sys_get_active_port说明：配合网络自适应功能
                    *    1. 原有体验：打开/关闭wifi则意味着切换网络, 因此打开/关闭wifi后设置到wifi/有线
                    *   2. 配置wifi/有线网络优先：实际数据通道切换之后, 设置到wifi/有线
                    */
                    if (SYS_PHY_TYPE_WIFI == sys_get_active_port())
                    {
                        strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_CONNECTED);
                    }
                    bUpdate = true;
                }
                else if (WIRELESS_OP_CONNECT_FAILED == msg.lParam)
                {
                    strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_ON);
                    bUpdate = true;
                }
            }
            // wps/wps-pin 连接结果
            else if (OP_WPSCONNECT_RESULT == msg.wParam
                     || OP_WPSPINCONNECT_RESULT == msg.wParam)
            {
                if (WIRELESS_OP_WPSCONNECT_SUCCESS == msg.lParam
                        || WIRELESS_OP_WPSCONNECT_FAILED == msg.lParam)
                {
                    strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_CONNECTED);
                    bUpdate = true;

                }
                else if (WIRELESS_OP_WPSCONNECT_FAILED == msg.lParam)
                {
                    strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_ON);
                    bUpdate = true;
                }
            }
            // 断开结果
            else if (OP_DISCONNECT_WIFI_RESULT == msg.wParam)
            {
                strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_ON);
                bUpdate = true;
            }
        }
        break;
    case SYS_MSG_NET_WOULD_CHANGED:
        {
            /**
            * 网络自适应需要处理该事件
            *   1. 原有体验：打开/关闭wifi则意味着切换网络, 因此打开/关闭wifi前接收到此消息
            *   2. 配置wifi/优先网络优先：实际数据通道切换前接收到此消息
            */
            if (WIFI_IsConnected())
            {
                // 即将切换为wifi网络
                if (0 == msg.wParam)
                {
                    strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_CONNECTED);
                    bUpdate = true;
                }
                // 即将切换为有线网络
                else if (1 == msg.wParam)
                {
                    bUpdate = true;
                }
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

    return true;
}

#if IF_SUPPORT_WIFI_ADAPTER
CWifiAdapterHandle::CWifiAdapterHandle(int nType /*= PS_STATE_WIFI_ADAPTER_ADD*/)
{
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, &CWifiAdapterHandle::OnWifiMessage);
}

CWifiAdapterHandle::~CWifiAdapterHandle()
{
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, &CWifiAdapterHandle::OnWifiMessage);
}

LRESULT CWifiAdapterHandle::OnWifiMessage(msgObject& msg)
{
    if (WIFI_MSG_UPDATE_UI != msg.message)
    {
        return FALSE;
    }

    if (OP_WIFI_DONGLE_STATUS_MODIFY == msg.wParam
            && WIRELESS_OP_DONGLE_ADD == msg.lParam)
    {
        idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_REMOVE);

        //dongle插入不进行弹框，直接开启
        if (0 != WIFI_GetSavedNetworkNum() &&
                1 != configParser_GetConfigInt(kszWifiShowScanPrompt))
        {
            WIFI_SetEnable(true, true);
        }
        else
        {
            idleScreen_SetStatus(PS_STATE_WIFI_ADAPTER_ADD, "");
        }
    }
    else if (OP_WIFI_DONGLE_STATUS_MODIFY == msg.wParam
             && WIRELESS_OP_DONGLE_REMOVE == msg.lParam)
    {
        idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);
        idleScreen_SetStatus(PS_STATE_WIFI_ADAPTER_REMOVE, "");
    }
    return TRUE;
}

bool CWifiAdapterHandle::GetPopupBoxInfo(PopupBoxData& popData)
{
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
    popData.m_vecSoftkey[0] = ST_CANCEL;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_EMPTY;
    popData.m_vecSoftkey[3] = ST_OK;

    if (PS_STATE_WIFI_ADAPTER_ADD == m_iType)
    {
        popData.m_strNote = _UILOGIC_LANG_TRAN(
                                TRID_WIFI_ADAPTER_HAS_ADDED_SCANNING_THE_WIFI_DEVICES_RIGHT_NOW);
    }
    else if (PS_STATE_WIFI_ADAPTER_REMOVE == m_iType)
    {
        popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_WIFI_ADAPTER_HAS_REMOVED_OUT);
    }

    return true;
}

bool CWifiAdapterHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode)
            && PS_STATE_WIFI_ADAPTER_ADD == m_iType)
    {
        //清空弹出标志
        idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);
        IdleUL_EnterPageByType(IEPT_WIFI);
    }
    else if ((ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode)
             && PS_STATE_WIFI_ADAPTER_ADD == m_iType)
    {
        //清空弹出标志
        idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_ADD);
    }
    else if ((ST_OK == eSoftkey || PHONE_KEY_OK == eKeyCode
              || ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode)
             && PS_STATE_WIFI_ADAPTER_REMOVE == m_iType)
    {
        //清空弹出标志
        idleScreen_DropStatus(PS_STATE_WIFI_ADAPTER_REMOVE);
    }
    return true;
}
#endif

#endif
