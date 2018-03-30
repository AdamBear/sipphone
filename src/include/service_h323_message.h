/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen  Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:
*
*******************************************************************/
#ifndef __SERVICE_H323_MESSAGE_H__
#define __SERVICE_H323_MESSAGE_H__

#include "service_h323_define.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __H323_REG_API_EXPORT_xxx__
/*
 *------------------------------------
 *      上报注册相关(0x80000-0x800FF)
 *------------------------------------
 */
#define H323_REG_MSG_BEGIN                          MKSRV_MSG(MKSRV_ID_H323, 0x0000)

/*
* usage   : 上报注册状态
*
* msg_id  : 0x80001
* target  : none
* wparam  : line id
* lparam  :
* data_ptr:
* data_len:
*/
#define H323_REG_UNKNOWN   0  /*初始状态*/
#define H323_REG_REGING         1  /*注册中*/
#define H323_REG_REGED           2  /*已注册*/
#define H323_REG_FAILED           3  /*注册失败*/
#define H323_REG_UNREGING    4  /*注销中*/
#define H323_REG_UNREGED      5  /*注销成功*/
#define H323_REG_UPDATE_STATUS                      MKSRV_MSG(MKSRV_ID_H323, 0x0001)

/*
* usage   : 上报H323当前运行状态
*
* msg_id  : 0x80002
* target  : none
* wparam  : line id
* lparam  : service status
* data_ptr: none
* data_len: none
*/
#define H323_SERVICE_UNKNOWN                           0/* h323_start_service()接口调用之前的状态，此时状态机未跑起来 */
#define H323_SERVICE_NETWORK_UNAVALIABLE     1/* h323_start_service()调用后，但是当前没IP地址时会挂起，等待IP地址变化 */
#define H323_SERVICE_RUNING                                2/* 此时APP处于运行状态 */
#define H323_SERVICE_DESTORYING                         3/* 这是一个中间状态，用于reload APP，reload之后状态会变更为RUNING */
#define H323_SERVICE_EXIT_SUCC                             4 /*Function Run方式调用exit，上报的异步消息，通知UI H323退出*/
#define H323_SERVICE_ERROR                                    5/*协议层内部数据错误，上报UI处理*/
#define H323_MSG_UPDATE_SERVICE_STATUS           MKSRV_MSG(MKSRV_ID_H323, 0x0002)


#define H323_REG_MSG_END                            MKSRV_MSG(MKSRV_ID_H323, 0x00FF)

#endif/* __H323_REG_API_EXPORT_xxx__ */

#ifndef __H323_CALL_API_RESULT_xxx__

/*
 *------------------------------------
 *      上报通话相关(0x80100--0x801FF)
 *------------------------------------
 */
#define H323_CALL_MSG_BEGIN                         MKSRV_MSG(MKSRV_ID_H323, 0x0100)

/*
* usage   : 上报新来电
*
* msg_id  : 0x80101
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: h323_call_info_t
* data_len: sizeof(h323_call_info_t)
*/
#define H323_CALL_NEW_INCOMING                      MKSRV_MSG(MKSRV_ID_H323, 0x0101)

/*
* usage   : 上报通话正在连接中(未实现)
*
* msg_id  : 0x80102
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_PROCEEDING                        MKSRV_MSG(MKSRV_ID_H323, 0x0102)

/*
* usage   : 上报通话正在回铃中
*
* msg_id  : 0x80103
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: string:callee name
* data_len: strlen(string)+1
*/
#define H323_CALL_RINGBACK                          MKSRV_MSG(MKSRV_ID_H323, 0x0103)

/*
* usage   : 上报通话被远端拒绝
*
* msg_id  : 0x80104
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_REFUSE                            MKSRV_MSG(MKSRV_ID_H323, 0x0104)

/*
* usage   : 上报通话被远端接起
*
* msg_id  : 0x80105
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_ACCEPT                            MKSRV_MSG(MKSRV_ID_H323, 0x0105)

/*
* usage   : 上报通话失败
*
* msg_id  : 0x80106
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:reason code
* data_len: sizeof(int)
*/
#define H323_CALL_FAIL                              MKSRV_MSG(MKSRV_ID_H323, 0x0106)

/*
* usage   : 上报通话结束
*
* msg_id  : 0x80107
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:reason code
* data_len: sizeof(int)
*/
#define H323_CALL_RELEASED                          MKSRV_MSG(MKSRV_ID_H323, 0x0107)
/*
* usage   : 上报通话被远端Hold
*
* msg_id  : 0x80109
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_REMOTE_HOLD                       MKSRV_MSG(MKSRV_ID_H323, 0x0109)

/*
* usage   : 上报通话被远端Resume
*
* msg_id  : 0x8010A
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_REMOTE_RESUME                     MKSRV_MSG(MKSRV_ID_H323, 0x010A)

/*
* usage   : 上报通话被远端Mute
*
* msg_id  : 0x8010B
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_REMOTE_MUTE                       MKSRV_MSG(MKSRV_ID_H323, 0x010B)

/*
* usage   : 上报通话被远端unMute
*
* msg_id  : 0x8010C
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_REMOTE_UN_MUTE                    MKSRV_MSG(MKSRV_ID_H323, 0x010C)

/*
* usage   : 上报通话收到远端发送的DTMF
*
* msg_id  : 0x8010D
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: string:
* data_len: strlen(string)+1
*/
#define H323_CALL_RECV_DTMF                         MKSRV_MSG(MKSRV_ID_H323, 0x010D)

/*
* usage   : 上报开启本地媒体通道
*
* msg_id  : 0x8010E
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: h323_channel_t
* data_len: sizeof(h323_channel_t)
*/
#define H323_CALL_OPEN_LOGIC_CHANNEL                MKSRV_MSG(MKSRV_ID_H323, 0x010E)

/*
* usage   : 上报通话被Forward(备注：当前未支持)
*
* msg_id  : 0x8010F
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr:
* data_len:
*/
#define H323_CALL_FORWARD_BY_REMOTE                 MKSRV_MSG(MKSRV_ID_H323, 0x010F)

/*
* usage   : 上报通话被Transer(备注：当前未支持)
*
* msg_id  : 0x80110
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr:
* data_len:
*/
#define H323_CALL_TRANSFER_BY_REMOTE                MKSRV_MSG(MKSRV_ID_H323, 0x0110)

/*
* usage   : 上报通话被Replace(备注：当前未支持)
*
* msg_id  : 0x80111
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr:
* data_len:
*/
#define H323_CALL_REPLACE_BY_REMOTE                 MKSRV_MSG(MKSRV_ID_H323, 0x0111)

/*
* usage   : 上报辅流状态，本地、远端开或者关
*
* msg_id  : 0x80120
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:ext video state
* data_len: sizeof(int)
*/
#define H323_EXT_VIDEO_UNKNOWN          -1
#define H323_EXT_VIDEO_CLOSE            0
#define H323_EXT_VIDEO_LOCAL_OPEN       1
#define H323_EXT_VIDEO_REMOTE_OPEN      2
#define H323_CALL_EXT_VIDEO_STATE                   MKSRV_MSG(MKSRV_ID_H323, 0x0120)


/*
* usage   : 上报远端要求本地发送视频帧同步
*
* msg_id  : 0x80121
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:video type
* data_len: sizeof(int)
*/
#define H323_CALL_VIDEO_FRAME_SYNC                   MKSRV_MSG(MKSRV_ID_H323, 0x0121)

/*
* usage   : 上报远端控制摄像头开始
*
* msg_id  : 0x80122
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:action
* data_len: sizeof(int)
*/
#define H323_CALL_CAMERA_ACTION_START               MKSRV_MSG(MKSRV_ID_H323, 0x0122)

/*
* usage   : 上报远端控制摄像头结束
*
* msg_id  : 0x80123
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: NULL
* data_len: 0
*/
#define H323_CALL_CAMERA_ACTION_STOP                MKSRV_MSG(MKSRV_ID_H323, 0x0123)


/*
* usage   : 上报远端控制摄像头保存
*
* msg_id  : 0x80124
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:preset
* data_len: sizeof(int)
*/
#define H323_CALL_CAMERA_PRESET_STORE               MKSRV_MSG(MKSRV_ID_H323, 0x0124)

/*
* usage   : 上报远端控制摄像头生效
*
* msg_id  : 0x80125
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int:preset
* data_len: sizeof(int)
*/
#define H323_CALL_CAMERA_PRESET_ACTIVE              MKSRV_MSG(MKSRV_ID_H323, 0x0125)


/*
* usage   : 上报codec列表
*
* msg_id  : 0x80126
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: h323_codec_desc_t
* data_len: sizeof(h323_codec_desc_t)
*/
#define H323_CALL_UPDATE_CODEC_LIST                 MKSRV_MSG(MKSRV_ID_H323, 0x0126)

/*
* usage   : 上报远端终端UA
*
* msg_id  : 0x80127
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: string:remote user agent
* data_len: strlen(string)+1
*/
#define H323_CALL_UPDATE_REMOTE_USER_AGENT          MKSRV_MSG(MKSRV_ID_H323, 0x0127)

/*
* usage   : 上报流控信息
*
* msg_id  : 0x80128
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: h323_flow_ctrl_para_t
* data_len: sizeof(h323_flow_ctrl_para_t)
*/
#define H323_CALL_FLOW_CONTROL_RATE                 MKSRV_MSG(MKSRV_ID_H323, 0x0128)


/*
* usage   : 上报诊断监听端口，在运营商禁用1720时作为备用端口
*
* msg_id  : 0x80129
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int
* data_len: sizeof(int)
*/
#define H323_CALL_DIAGNOSE_LISTEN_PORT                MKSRV_MSG(MKSRV_ID_H323, 0x0129)

/*
* usage   : 上报信令诊断信息
*
* msg_id  : 0x8012a
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: h323_diagnose_sig_t
* data_len: sizeof(h323_diagnose_sig_t)
*/
#define H323_CALL_DIAGNOSE_SIGNAL_INFO                           MKSRV_MSG(MKSRV_ID_H323, 0x012A)

/*
* usage   : 上报媒体诊断信息
*
* msg_id  : 0x8012b
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: h323_diagnose_media_t
* data_len: sizeof(h323_diagnose_media_t)
*/
#define H323_CALL_DIAGNOSE_MEDIA_INFO                           MKSRV_MSG(MKSRV_ID_H323, 0x012B)

/*
* usage   : 上报对端发过来的自定义信息（XML文本）
*
* msg_id  : 0x8012c
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: xml_body
* data_len: sizeof(xml_body)
*/
#define H323_USER_CUSTOM_INFO                                     MKSRV_MSG(MKSRV_ID_H323, 0x012C)

/*
* usage   : 上报转接结果(备注：当前未实现)
*
* msg_id  : 0x80180
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int result
* data_len: sizeof(int)
*/
#define H323_CALL_TRANSFERED_RESULT             MKSRV_MSG(MKSRV_ID_H323, 0x0180)

/*
* usage   : 上报h323_call_open_ext_video()结果
*
* msg_id  : 0x80181
* target  : none
* wparam  : line id
* lparam  : call id
* data_ptr: int result
* data_len: sizeof(int)
*/
#define H323_RESULT_OPEN_EXT_VIDEO                  MKSRV_MSG(MKSRV_ID_H323, 0x0181)


#define H323_CALL_MSG_END                           MKSRV_MSG(MKSRV_ID_H323, 0x01FF)
#endif/* __H323_CALL_API_RESULT_xxx__ */


#endif


