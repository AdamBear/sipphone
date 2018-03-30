#include "service_dsk_define.h"
#include "service_dsk_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

////////////////////////////////////错误码//////////////////////////////////////////////////////
#define ERROR_CODE_SUCCESS               1
#define ERROR_CODE_NONE                  0
#define ERROR_CODE_MKIT_NOT_FOUND       -1
#define ERROR_CODE_MKIT_NOTIFY_FAIL     -2
#define ERROR_CODE_MKIT_CALL_FAIL       -3
#define ERROR_CODE_MKIT_POST_FAIL       -4
#define ERROR_CODE_MKIT_SEND_FAIL       -5

#define NULLPOINTER     0

////////////////////////////////////基础接口区间//////////////////////////////////////////////////////
mkit_target_t getTarget(mkit_handle_t caller, const char* szName)
{
    mkit_target_t target = 0;
    if (mkit_find(caller, &target, szName) != 0)
    {
        //printf("get [%s]'s target failed\n", szName);
        return ERROR_CODE_MKIT_NOT_FOUND;
    }
    return target;
}

int dsk_request_notify(mkit_handle_t caller, int event)
{
    mkit_target_t notifier = getTarget(caller, "app_vpPhone");
    if (notifier == 0)
    {
        return ERROR_CODE_MKIT_NOT_FOUND;
    }

    return (mkit_notify_request(caller, notifier, event, -1) == 0 ? ERROR_CODE_SUCCESS : ERROR_CODE_MKIT_NOTIFY_FAIL);
}

int dsk_cancel_notify(mkit_handle_t caller, int event)
{
    mkit_target_t notifier = getTarget(caller, "app_vpPhone");
    if (notifier == 0)
    {
        return ERROR_CODE_MKIT_NOT_FOUND;
    }
    return (mkit_notify_cancel(caller, notifier, event) == 0 ? ERROR_CODE_SUCCESS : ERROR_CODE_MKIT_NOTIFY_FAIL);
}


////////////////////////////////////query接口区间//////////////////////////////////////////////////////
int query_logic_info(mkit_handle_t caller, int message, int type, int flag, void* data, int size, int timeout)
{
    char buf[512] = { 0 };
    mkit_message_t retmsg;

    retmsg.target = getTarget(caller, "app_vpPhone");
    retmsg.flags = MKIT_MFL_CALL_RET | MKIT_MFL_CALL_DAT;
    retmsg.sync = timeout;
    retmsg.message = message;
    retmsg.wparam = (unsigned long)type;
    retmsg.lparam = (unsigned long)flag;
    if (data == NULLPOINTER)
    {
        retmsg.data_ptr = (unsigned char *)buf;
        retmsg.data_len = sizeof(buf);
    }
    else
    {
        retmsg.data_ptr = (unsigned char *)data;
        retmsg.data_len = size;
    }
    retmsg.data_max = retmsg.data_len;

    if (mkit_call(caller, &retmsg) != 0)
    {
        //printf("query info failed\n");
        return ERROR_CODE_MKIT_CALL_FAIL;
    }
    else
    {
        if (retmsg.data_ptr && retmsg.data_len > 0)
        {
            //printf("w[%d] l[%d] data[%s]\n\n", retmsg.wparam, retmsg.lparam, retmsg.data_ptr);
        }
    }

    // 返回类型，根据后续的需要增加数据结构
    return retmsg.wparam;
}


DSK_API_DECLARE int dsk_query_info_phone_cfg(mkit_handle_t caller, char* data, int size, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GETPHONE_INFO, 0, 0, data, size, timeout);
}

DSK_API_DECLARE int dsk_query_info_phone_status(mkit_handle_t caller, int type, int flag, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GET_PHONE_STATUS, type, flag, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_phone_active_status(mkit_handle_t caller, int type, int flag)
{
    return query_logic_info(caller, DSK_MSG_GET_ACTIVITE_STATUS, type, flag, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_phone_active_status2(mkit_handle_t caller, int type, int flag, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GET_ACTIVITE_STATUS, type, flag, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_account_count(mkit_handle_t caller, int type)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_CONFIG_ACCOUNT_COUNT, type, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_account_count2(mkit_handle_t caller, int type, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_CONFIG_ACCOUNT_COUNT, type, NULLPOINTER, 0, timeout);
}

int dsk_query_info_account_state(mkit_handle_t caller, int account_id, int type, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GETLINESTATUS, account_id, type, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_pstn_account_state(mkit_handle_t caller, int account_id, int type, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GET_PSTN_LINE_STATUS, account_id, type, NULLPOINTER, 0, timeout);
}

int dsk_query_info_talk_exist(mkit_handle_t caller)
{
    return query_logic_info(caller, DSK_MSG_GETWORKSTATUS, 0, 0, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

int dsk_query_info_talk_exist2(mkit_handle_t caller, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GETWORKSTATUS, 0, 0, NULLPOINTER, 0, timeout);
}

int dsk_query_info_talk_state(mkit_handle_t caller, int call_id)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_TALK_STATE, call_id, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

int dsk_query_info_talk_state2(mkit_handle_t caller, int call_id, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_TALK_STATE, call_id, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_bluetooth_info(mkit_handle_t caller, Dsk_BluetoothInfo* info)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG,
                            DSK_DIAG_INFO_TYPE_MENU_BLUETOOTH_INFO, 0, info, sizeof(Dsk_BluetoothInfo), DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_bluetooth_info2(mkit_handle_t caller, Dsk_BluetoothInfo* info, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG,
                            DSK_DIAG_INFO_TYPE_MENU_BLUETOOTH_INFO, 0, info, sizeof(Dsk_BluetoothInfo), timeout);
}

DSK_API_DECLARE int dsk_query_info_wifi_info(mkit_handle_t caller, Dsk_WifiInfo* info)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_WIFI_INFO, 0, info,
		sizeof(Dsk_WifiInfo), DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_wifi_info2(mkit_handle_t caller, Dsk_WifiInfo* info, int timeout)
{
	return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_WIFI_INFO, 0, info, sizeof(Dsk_WifiInfo), timeout);
}

DSK_API_DECLARE int dsk_query_info_usb_info(mkit_handle_t caller, Dsk_USBInfo* info)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_USB_INFO, 0, info, sizeof(Dsk_USBInfo), DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_usb_info2(mkit_handle_t caller, Dsk_USBInfo* info, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_USB_INFO, 0, info, sizeof(Dsk_USBInfo), timeout);
}

DSK_API_DECLARE int dsk_query_info_exp_info(mkit_handle_t caller, Dsk_EXPInfo* info)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_EXP_INFO, 0, info, sizeof(Dsk_EXPInfo), DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_exp_info2(mkit_handle_t caller, Dsk_EXPInfo* info, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_EXP_INFO, 0, info, sizeof(Dsk_EXPInfo), timeout);
}

DSK_API_DECLARE int dsk_query_info_license_info(mkit_handle_t caller, Dsk_LicenseInfo* info)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_LICENSE, 0, info, sizeof(Dsk_LicenseInfo), DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_license_info2(mkit_handle_t caller, Dsk_LicenseInfo* info, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_LICENSE, 0, info, sizeof(Dsk_LicenseInfo), timeout);
}

int dsk_query_info_sfb_presence(mkit_handle_t caller, int account_id)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG,
                            DSK_DIAG_INFO_TYPE_MENU_SFB_PRESENCE, account_id, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

int dsk_query_info_sfb_presence2(mkit_handle_t caller, int account_id, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_SFB_PRESENCE, account_id, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_sfb_btoe_state(mkit_handle_t caller)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_SFB_BTOE_STATE, 0, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_sfb_btoe_state2(mkit_handle_t caller, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_SFB_BTOE_STATE, 0, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_sfb_cap_state(mkit_handle_t caller)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_SFB_CAP_STATE, 0, NULLPOINTER, 0, DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_sfb_cap_state2(mkit_handle_t caller, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_SFB_CAP_STATE, 0, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_contact_update_state(mkit_handle_t caller, int type, int flag, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GET_CONTACT_UPDATESTATS, type, flag, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_contact_count(mkit_handle_t caller, int type, int group, int timeout)
{
    return query_logic_info(caller, DSK_MSG_GET_CONTACTCOUNT, type, group, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_query_info_sfb_exchange_state(mkit_handle_t caller, Dsk_ExchangeInfo* info)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG,
                            DSK_DIAG_INFO_TYPE_MENU_SFB_EXCHANGE_STATE, 0, info, sizeof(Dsk_ExchangeInfo), DSK_TIME_OUT_DEFAULT);
}

DSK_API_DECLARE int dsk_query_info_sfb_exchange_state2(mkit_handle_t caller, Dsk_ExchangeInfo* info, int timeout)
{
    return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG,
                            DSK_DIAG_INFO_TYPE_MENU_SFB_EXCHANGE_STATE, 0, info, sizeof(Dsk_ExchangeInfo), timeout);
}

DSK_API_DECLARE int dsk_query_info_camera_list(mkit_handle_t caller, Dsk_CameraIDList* Idlist)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_CAMERA_ID_LIST, 0, Idlist, sizeof(Dsk_CameraIDList));
}

DSK_API_DECLARE int dsk_query_info_camera_info(mkit_handle_t caller, int cameraId, Dsk_CameraInfo* info)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_CAMERA_INFO, cameraId, info, sizeof(Dsk_CameraInfo));
}

DSK_API_DECLARE int dsk_query_info_audio_input_output_dev_info(mkit_handle_t caller, Dsk_Audio_IO_dev* info)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_AUDIO_IO_INFO, 0, info, sizeof(Dsk_Audio_IO_dev));
}

DSK_API_DECLARE int dsk_query_info_vcs_phone_dev_info(mkit_handle_t caller, Dsk_VcsphoneInfo* info)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_VCS_PHONE_INFO, 0, info, sizeof(Dsk_VcsphoneInfo));
}

DSK_API_DECLARE int dsk_query_info_dect_micpod_dev_info(mkit_handle_t caller, Dsk_DectMicpodInfo* info)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_QUERY_INFO_DIAG, DSK_DIAG_INFO_TYPE_MENU_DECT_MIC_INFO, 0, info, sizeof(Dsk_DectMicpodInfo));
}

DSK_API_DECLARE int dsk_notify_vcp_phone_upgrade_info(mkit_handle_t caller, int status, int type, char* data, int size)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_CP_UPGRADE, status, type, data, size);
}

DSK_API_DECLARE int dsk_notify_check_vcp_phone_header_info(mkit_handle_t caller, char* data, int size)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_CP_CHECKHEADER, 0, 0, data, size);
}

DSK_API_DECLARE int dsk_notify_get_vcp_phone_ip_addr_info(mkit_handle_t caller, Dsk_VcsPhoneIPinfo* info)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_CP_GETIP, 0, 0, info, sizeof(Dsk_VcsPhoneIPinfo));
}

DSK_API_DECLARE int dsk_notify_get_vcp_phone_check_result(mkit_handle_t caller)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_CP_GET_CHECKRESULT, 0, 0, NULLPOINTER, 0);
}

DSK_API_DECLARE int dsk_notify_vcc22_upgrade(mkit_handle_t caller, int status, int type, char* data, int size)
{
    return 0;
//  return query_logic_info(caller, DSK_MSG_VCC22_UPGRADE, status, type, data, size);
}

////////////////////////////////////CMD 接口区间//////////////////////////////////////////////////////
int cmd_logic_action(mkit_handle_t caller, unsigned int message, int type, int flag, void* data, int size, int timeout)
{
    mkit_target_t notifier = getTarget(caller, "app_vpPhone");
    if (notifier < 0)
    {
        return ERROR_CODE_MKIT_NOT_FOUND;
    }

    if (timeout >= DSK_TIME_OUT_WAIT)
    {
        // 支持外部call调用：建议使用post
        return query_logic_info(caller, message, type, flag, data, size, timeout);
    }
    else if (mkit_post(caller, notifier, message, type, flag, data, size) != 0)
    {
        //printf("post failed\n");
        return ERROR_CODE_MKIT_POST_FAIL;
    }

    // 返回类型，根据后续的需要增加数据结构
    return ERROR_CODE_SUCCESS;
}

DSK_API_DECLARE int dsk_cmd_change_phone_cfg(mkit_handle_t caller, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_SETPHONE_CFG, 0, 0, data, size, timeout);
}

int dsk_cmd_show_message(mkit_handle_t caller, char* data, int size, int time)
{
    return cmd_logic_action(caller, DSK_MSG_CMD_ACTION_SHOW_MESSAGE, time, 0, data, size, DSK_TIME_OUT_NONE);
}

DSK_API_DECLARE int dsk_cmd_talk_hang_up(mkit_handle_t caller, int call_id, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_HANG_UP, call_id, 0, NULLPOINTER, 0, timeout);
}

DSK_API_DECLARE int dsk_cmd_talk_call_out(mkit_handle_t caller, int account_id, int mask, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_MAKE_CALL, account_id, mask, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_talk_redirect_target(mkit_handle_t caller, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_REDIRECTCALL, 0, 0, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_action_url(mkit_handle_t caller, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_ACTION_URI, 0, 0, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_action_url_confirm(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_ACTION_URI_CONFIRM, type, flag, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_csta(mkit_handle_t caller, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_CSTA, 0, 0, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_push_xml(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_PUSH_XML, type, flag, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_request_xml(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_REQUEST_XML, type, flag, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_show_maintenance(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_UPLOAD_CONFIG, type, flag, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_save_calllog(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_SAVE_CALLLOG, type, flag, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_delete_calllog(mkit_handle_t caller, int group, int type, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_DEl_LOCALLOG, group, type, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_set_lcd_contrast(mkit_handle_t caller, int level, int type, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_CONTRAST_CHANGE, level, type, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_set_light_level(mkit_handle_t caller, int level, int type, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_LIGHT_LEVEL_CHANGE, level, type, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_reset_factory(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_REQUES_RESET_FACTORY, type, flag, data, size, timeout);
}

DSK_API_DECLARE int dsk_cmd_do_edk(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout)
{
    return cmd_logic_action(caller, DSK_MSG_REQUES_RESET_FACTORY, type, flag, data, size, timeout);
}

#ifdef __cplusplus
}
#endif
