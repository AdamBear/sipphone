#ifndef _DSK_MSG_ID_DEFINE_H_
#define _DSK_MSG_ID_DEFINE_H_

#include "service_dsk_interface_extension.h"

/************************************************************************/
/*
    1、common消息段消息个数不多，请谨慎使用，将各个模块功能尽量合并
        到一个消息里面处理
    2、消息定义命名规则：DSK_MSG_模块_具体功能 DSK_MSG_开头
        用于区分各个部门的定义

*/
/************************************************************************/

// usage: get line status
// wParam:line id(0~5)
// lParam:0/1
// return: lParam=0, return line status,
//         lParam=1, return the ServerID on which the Account registered.
//         dsk_query_info_account_state
#define DSK_MSG_GETLINESTATUS                           DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 1)

// usage: get talk status
// wParam:0
// lParam:0
// return: 1: talking, 0: idle
// dsk_query_info_talk_exist
#define DSK_MSG_GETWORKSTATUS                           DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 2)

// usage: reset channel
// wParam:0
// lParam:0
// 测试模式使用：不需要封装接口
#define DSK_MSG_SELECT_CHANNEL                          DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 3)

// usage: get num of ext device
// wParam:0
// lParam:0
// return: the num of ext device
// dsk_query_info_exp_info
#define DSK_MSG_ENUM_EXTENSION                          DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 4)

// usage: hung up talk
// wParam: 0: hang up current talk, 1: hang up all talks
// lParam: 0
// dsk_cmd_talk_hang_up
#define DSK_MSG_HANG_UP                                 DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 5)

// usage: execute action uri
// wParam:0
// lParam:0
// extra data: uri code
// dsk_cmd_action_url
#define DSK_MSG_ACTION_URI                              DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 6)

// usage: push xml
// wParam:0
// lParam:0
// dsk_cmd_push_xml
#define DSK_MSG_PUSH_XML                                DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 7)

// usage: change user level
// wParam: 0/1/2(user/var/admin)
// lParam: 0
// todo v80
#define DSK_MSG_USER_LEVEL_CHANGED                      DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 8)

// usage: query blflist value
// wParam:0:Line key,1:MemKey,2:Exp
// lParam:exp index
// todo v80
#define DSK_MSG_WEB_QUERY_BLFLIST                       DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 9)

// usage: write phone info to /tmp/phonecfg.ini
// wParam:0
// lParam:0
// extra: phone info command
// return: 1: succ; 0: fail
// dsk_query_info_phone
#define DSK_MSG_GETPHONE_INFO                           DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 10)

// usage: redirect call
// wparam: 0
// lparam: 0
// extra: redirect command
// dsk_cmd_talk_redirect_target
#define DSK_MSG_REDIRECTCALL                            DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 11)

// usage: set phone cfg
// wparam: 0
// lparam: 0
// extra: phone cfg
// dsk_cmd_change_phone_cfg
#define DSK_MSG_SETPHONE_CFG                            DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 12)

// usage: process multicast talk
// wParam: 0: huagup talk; 1: answer start
// lParam: 0
// extra: PAGING_PARAM
// 内部使用
#define DSK_MSG_LISTEN_MULTICAST                        DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 13)

// usage: close powerlight
// wParam: 0
// lParam: 0
// todo v80
#define DSK_MSG_CLOSE_POWERLIGHT                        DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 14)

// usage: acd result
// wParam: line id
// lParam: acd status
// extra: result
// 内部使用
#define DSK_MSG_ACD_RESULT                              DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 15)

// usage: notify blf config is channed, the light of blf should be updated
// wParam: 0
// lParam: 0
// 建议使用配置改变消息实现
#define DSK_MSG_WEB_BLFLISTCONFIG_CHANGE                DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 16)

// usage: call out number
// wParam: line id
// lParam: Make Call Mask
// extra: number, example: "sip:UserName@ServerName"
// dsk_cmd_talk_call_out
#define DSK_MSG_MAKE_CALL                               DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 17)

// usage: csta
// wParam: 0
// lParam: 0
// extra: csta content
#define DSK_MSG_CSTA                                    DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 18)

// usage: upload user config
// wParam: 0
// lParam: 0
// dsk_cmd_show_maintenance
#define DSK_MSG_UPLOAD_CONFIG                           DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 19)

// usage: unknown
// wParam: 0
// lParam: 0
// dsk_cmd_action_url_confirm
#define DSK_MSG_ACTION_URI_CONFIRM                      DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 20)

// usage: notify ext is changed
// wParam: 0: linekey; 1: memkey; 2: programkey; 38: exp38; 39: exp39; 40: exp40; 41: ehs
// lParam: exp index
// 内部使用
#define DSK_MSG_EXT_CHANGE                              DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 21)

// usage: save calllog to file
// wParam: 0
// lParam: 0
#define DSK_MSG_SAVE_CALLLOG                            DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 22)

// usage: change light status
// wparam: 0: normal, 1: always light on
// lParam: usaging when waram is 1; 1: open; 0: close
// 内部使用
#define DSK_MSG_LIGHT_STATUS_CHANGE                     DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 23)

// usage: control whern time change
// wParam: 0
// lParam: 0
// todo v80
#define DSK_MSG_LIGHT_TIME_CHANGE                       DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 24)

// usage: contrast change
// wParam: level
// lParam: 0
// dsk_cmd_set_lcd_contrast
#define DSK_MSG_CONTRAST_CHANGE                         DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 25)

// usage: light level change
// wParam: level
// lParam: 0
#define DSK_MSG_LIGHT_LEVEL_CHANGE                      DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 26)


// usage: request autop xml file
// wParam: 0
// lParam: 0
// dsk_cmd_request_xml
#define DSK_MSG_REQUEST_XML                             DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 27)

// usage: get talk stats
// wParam: call id, if call id not found, return focuse session
// lParam: 1: get MediaStatsInfo; 0: get TalkStatisticsInfo
// return: MediaStatsInfo/TalkStatisticsInfo
// todo
#define DSK_MSG_GET_MEDIA_STATS                         DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 28)

// usage: get contact update mark
// wParam: 0
// lParam: 0
// return: 0:nochange / 1:update contacts/ 2:update group / 3:update both
#define DSK_MSG_GET_CONTACT_UPDATESTATS                         DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 29)

// usage: get extern device status 0x60D1F?
// wParam: 1:wifi dongle status 2:bluetooth dongle status 3:mobile connect status
// lParam: 0
// dsk_query_info_bluetooth_info
// dsk_query_info_wifi_info
#define DSK_MSG_GET_DONGLE_STATUS                       DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 30)

// usage: web/autop notify gui update hs
// wParam: 0:ROM 1:ROM Head
// lParam: 0:Autop, 1:Web
// TODO
#define DSK_MSG_HS_UPDATE                               DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 31)

// usage: web/autop 查询话机状态
// wParam: 0：Register  1:paging  2:手柄升级  3:GUI启动
// lParam:
// dsk_query_info_phone_status
#define DSK_MSG_GET_PHONE_STATUS                        DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 32)

// usage: web/autop 设置话机状态
// wParam: 0：Register  1:paging
// lParam: 0:off 1:on
// todo v80
#define DSK_MSG_SET_PHONE_STATUS                        DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 33)

// usage: get lcd/hdmi display info
// wParam: 0/lcd 1/hdmi
// lParam: 0
// return: MediaStatsInfo/TalkStatisticsInfo
// todo
#define DSK_MSG_GET_DISPLAY_INFO                       DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 34)

// usage: get talk rtp status(rtcp-xr)
// wParam: 0 recent call vpm data
// lParam: 0
// return: char *(xml file path)
#define DSK_MSG_GET_RTP_STATS                           DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 35)

// usage: get PSTN line status
// wParam: PSTN line id(PSTN_LINE_BEGIN_ID~PSTN_LINE_END_ID)
// lParam: 0
// return: lParam=0, return PSTN line status
// dsk_query_info_pstn_account_state
#define DSK_MSG_GET_PSTN_LINE_STATUS                    DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 36)

// usage: get contactcount
// wParam: DIR_TYPE
// lParam: 0
// return: lParam=0, return contactcount
// dsk_query_info_contact_count
#define DSK_MSG_GET_CONTACTCOUNT                            DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 37)

// usage: del locallog
// wParam: GROUP_ID
// lParam: 0
// return: lParam=0, return contactcount
// dsk_cmd_delete_calllog
#define DSK_MSG_DEl_LOCALLOG                              DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 38)

// usage: web/autop notify gui update hs
// wParam: 0:Start Kill 1:Start Upgrade
// lParam: 0:Autop, 1:Web
#define DSK_MSG_CP_UPGRADE								DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 39)

// usage: autop notify gui to check vcp fireware autopÏÂÔØÍê³ÉÎÄ¼þÍ·Ö®ºóÍ¨ÖªUIÈ¥Ð£Ñé
//extra data:path of header file
#define DSK_MSG_CP_CHECKHEADER							DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 40)

// usage: web/autop notify gui toupaddr. return 1 VCP conected extra data is vcp ipaddr retunr 0 VCP disconnected
#define DSK_MSG_CP_GETIP								DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 41)

// usage: autop notify gui to return vcp fireware check result .return 1 check success retun 0 check fail
#define DSK_MSG_CP_GET_CHECKRESULT								DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 42)

// usage: web/autop notify gui update vcc22
// wParam: 0:Start Kill 1:Start Upgrade
// lParam: 0:Autop, 1:Web
#define DSK_MSG_VCC22_UPGRADE								DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 43)

// 注意：为了避免多项目同时开发导致消息定义重复，因此Lync项目原则上100-150
// // usage: get lync status
// wParam: account id, if account id not found, return NULL
// lParam: 0
// return: MediaStatsInfo/TalkStatisticsInfo
#define DSK_MSG_GET_LYNC_STATUS                         DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 100)

#define DSK_MSG_GET_LYNC_BTOE_STATUS                    DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 102)

// 注意：为了避免多项目同时开发导致消息定义重复，因此Lync项目原则上100-150

// todo
// usage: autop get phone activite status
// wParam: 0
// lParam: 0
#define DSK_MSG_GET_ACTIVITE_STATUS                     DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 101)

// usage: reset to factory
// wParam: option enum ResetOptionType;
// lParam: 0
#define DSK_MSG_REQUES_RESET_FACTORY                    DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 103)

// todo 诊断上报消息, V83 sp1之前使用
#define DSK_MSG_REPORT_DIAG_EVENT_INFO                      DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 101)

// usage: xsi-event
// wParam: 0
// lParam: 0
// extra: event content
#define DSK_MSG_XSI_EVENT                               DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 104)

// todo V83 sp1之前使用
// 弹窗显示文本----------hwz 暂定同步lync代码，按照讨论，需要诊断程序提供消息定义
// 扩展参数传入显示的字符
#define DSK_MSG_SHOW_TEXT_INFO                          DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 105)

// web导bin信息，gui统计信息到config/data目录下，一起导出bin文件
#define DSK_MSG_EXPORT_CONFIG                           DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 106)

////////////////技术支持之后从120往上叠加/////////////////////
// usage: 相关通话事件发生时，上报该消息通知TR069
// wParam: 事件类型
// lParam: 通话使用的账号
// todo
#define DSK_MSG_TALK_STATS_INFO                         DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 120)

// usage: get wifi status
// wParam: 0
// lParam: 0
// todo v80
#define DSK_MSG_GET_WIFI_STATUS                         DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 121)

////////////////技术支持之后叠加到149/////////////////////

// Make Call Mask   used as DSK_MSG_MAKE_CALL in L param
#define DSK_MAKE_CALL_MASK_NORMAL     0x00
#define DSK_MAKE_CALL_MASK_MANUAL     0x01
#define DSK_MAKE_CALL_MASK_HISTORY    0x02
#define DSK_MAKE_CALL_MASK_DIRECTORY  0x03
#define DSK_MAKE_CALL_MASK_HISTORY_DIALED       0x10
#define DSK_MAKE_CALL_MASK_HISTORY_MISSED       0x11
#define DSK_MAKE_CALL_MASK_HISTORY_RECEIVED     0x12
#define DSK_MAKE_CALL_MASK_HISTORY_FORWARDED    0x13


////////////////sip中性之后从150往上叠加/////////////////////

// usage: Set ICON
// wParam: ICOE Type ID
// lParam: On/Off 1/0
// extra: 0
#define DSK_MSG_SET_ICON                                   DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 150)


// usage: Set LED
// wParam: LED ID
// lParam: 0
// extra: 0
#define DSK_MSG_SET_LED                                    DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 151)

// usage: REBOOT
// wParam: 0
// lParam: 0
// extra: 0
#define DSK_MSG_REQUES_CMD                                 DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 152)

// usage: CLOSE
// wParam: 0
// lParam: 0
// extra: 0
#define DSK_MSG_REQUES_QUIT                               DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 153)

// usage : Do EDK Action
// wParam: undefine
// lParam: undefine
// extra : EDK Command String
#define DSK_MSG_DO_EDK                                    DSK_MSG(DSK_MSG_COMMON, DSK_MSG_INDEX + 154)


// //////////////////////////////GUI service msg define///////////////////////////////////
////////////////////////////////message define:对外接口使用的消息///////////////////////////////////
/**********************************************************************************************//**
// usage: 外部模块获取信息：消息起始
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_QUERY_INFO_MIN                      DSK_MSG(DSK_MSG_QUERY, DSK_MSG_INDEX + 0)

/**********************************************************************************************//**
// usage: 诊断平台获取信息
// wParam: DSK_DiagInfoType
// lParam: 0
// extra: string(json)
**************************************************************************************************/
#define DSK_MSG_QUERY_INFO_DIAG                     DSK_MSG(DSK_MSG_QUERY, DSK_MSG_INDEX + 1)

/**********************************************************************************************//**
// usage: 外部模块获取信息：消息段结尾
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_QUERY_INFO_MAX                      DSK_MSG(DSK_MSG_QUERY, DSK_MSG_INDEX + 1023)
////////////////////////////////////message define:对外提供的接口封装/////////////////////////////
#define DSK_MSG_CMD_ACTION_MIN                      DSK_MSG(DSK_MSG_CMD, DSK_MSG_INDEX + 0)

/**********************************************************************************************//**
// usage: 外部请求显示告示
// wParam: 0
// lParam: 0
// extra: message data(string)
**************************************************************************************************/
#define DSK_MSG_CMD_ACTION_SHOW_MESSAGE                     DSK_MSG(DSK_MSG_CMD, DSK_MSG_INDEX + 0)

#endif // _DSK_MSG_ID_DEFINE_H_
