/**
 * @file configiddefine.h
 * @brief 配置ID定义
 *
 * 注意：
 * 根据Doxygen格式要求，注释中的`%d`应该写为`%%d`。
 * 如果默认值`default`为空，应该写为空字符串`""`。
 */

#ifndef _CONFIG_ITEMS_DEFINE_HEADER_
#define _CONFIG_ITEMS_DEFINE_HEADER_

/**
 * @brief 注释
 */
#define kszAASecondIntercom "AASecondIntercom"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.acd.attendantgroup
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | AttendantGroup
 *      default | ""
 */
#define kszACDAttendantGroup "ACDAttendantGroup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | acd.auto_available_timer
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoAvailableTimer
 *      default | 60
 *          reg | {required:true,range:[0,120,60],maxlength:3}
 */
#define kszACDAutoAvailableTime "ACDAutoAvailableTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.auto_login_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | ACDAutoLogin
 *      default | 0
 */
#define kszACDAutoLogin "ACDAutoLogin"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.acd.offcode
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | OffCode
 *      default | ""
 */
#define kszACDOffCode "ACDOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.acd.oncode
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | OnCode
 *      default | ""
 */
#define kszACDOnCode "ACDOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.genesys.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | ACDStatus
 *      default | 0
 */
#define kszACDStatus "ACDStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.acd.checkinenable
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | CheckInEnable
 *      default | ""
 */
#define kszACDSwitch "ACDSwitch"

/**
 * @brief AOCEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.aoc.enable
 *         path | /config/user/user.ini
 *      section | AOC
 *          key | AOCEnable
 *      default | 0
 */
#define kszAOCEnable "AOCEnable"

/**
 * @brief AOCShowTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.aoc.display_time
 *         path | /config/user/user.ini
 *      section | AOC
 *          key | AOCShowTime
 *      default | 3
 */
#define kszAOCShowTime "AOCShowTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.answer_new_incoming_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | AnswerNewInCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUAnswerNewCall "AUAnswerNewInCall"
#define kszAUAnswerNewInCall "AUAnswerNewInCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.attended_transfer_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | AttendedTransfer
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUAttendedTransfer "AUAttendedTransfer"

/**
 * @brief 咨询转失败的URL
 */
#define kszAUAttendedTransferFailed "AUAttendedTransferFailed"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.setup_autop_finish
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | SetupAutopFinish
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUAutopFinish "AUAutopFinish"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.blind_transfer_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | BlindTransfer
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUBlindTransfer "AUBlindTransfer"

/**
 * @brief 盲转失败的URL
 */
#define kszAUBlindTransferFailed "AUBlindTransferFailed"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.busy_to_idle
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | BusyToIdle
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUBusyToIdle "AUBusyToIdle"

/**
 * @brief 结束拨号的URL
 */
#define kszAUCEndDial "AUCEndDial"

/**
 * @brief 呼叫失败的URL
 */
#define kszAUCallFial "AUCallFial"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.outgoing_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CallOut
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallOut "AUCallOut"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.call_remote_canceled
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CallRemoteCanceled
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallRemoteCancel "AUCallRemoteCanceled"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.action_url.call_transfered_by_remote
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CallTransferedByRemote
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallTransferedByRemote "AUCallTransferedByRemote"

/**
 * @brief 配件拔插发送消息到URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.peripheral_information
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | PeripheralInformation
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUPeripheralInformation "AUPeripheralInformation"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.action_url.call_by_replaced
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CallByReplaced
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallByReplaced "AUCallByReplaced"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.action_url.call_info_update
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CallInfoUpdate
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallInfoUpdate "AUCallInfoUpdate"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.cancel_callout
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CancelCallOut
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallInterrupt "AUCancelCallOut"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.always_fwd_off
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CloseAlwaysForward
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCloseAlwaysForward "AUCloseAlwaysForward"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.busy_fwd_off
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CloseBusyForward
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCloseBusyForward "AUCloseBusyForward"

/**
 * @brief 关闭callwaiting的URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.call_waiting_off
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CloseCallWait
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCloseCallWait "AUCloseCallWait"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.dnd_off
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CloseDnd
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCloseDnd "AUCloseDnd"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.no_answer_fwd_off
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | CloseNoAnswerForward
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCloseNoAnswerForward "AUCloseNoAnswerForward"

/**
 * @brief 三方会议的URL
 */
#define kszAUConference "AUConference"

/**
 * @brief DTMF的URL
 */
#define kszAUDTMF "AUDTMF"

/**
 * @brief 进入拨号的URL
 */
#define kszAUEnterDial "AUEnterDial"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.call_established
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | Established
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUEstablished "AUEstablished"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.forward_incoming_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | ForwardIncomingCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUForwardCall "AUForwardIncomingCall"
#define kszAUForwardIncomingCall "AUForwardIncomingCall"

/**
 * @brief 免提
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.handfree
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | HandFree
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUHandFree "AUHandFree"

/**
 * @brief 耳麦
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.headset
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | HeadSet
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUHeadSet "AUHeadSet"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.hold
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | Hold
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUHold "AUHold"

/**
 * @brief 注释
 */
#define kszAUISUME "AUISUME"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.idle_to_busy
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | IdleToBusy
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUIdleToBusy "AUIdleToBusy"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.incoming_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | IncomingCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUIncomingCall "AUIncomingCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.ip_change
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | IpChanged
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUIpChanged "AUIpChanged"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.unregistered
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | LogOff
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAULogOff "AULogOff"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.registered
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | LogOn
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAULogOn "AULogOn"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.missed_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | MissedCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUMissedCall "AUMissedCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.mute
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | Mute
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUMute "AUMute"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.off_hook
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OffHook
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOffHook "AUOffHook"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.on_hook
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OnHook
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOnHook "AUOnHook"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.always_fwd_on
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OpenAlwaysForward
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOpenAlwaysForward "AUOpenAlwaysForward"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.busy_fwd_on
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OpenBusyForward
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOpenBusyForward "AUOpenBusyForward"

/**
 * @brief 开启callwaiting时URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.call_waiting_on
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OpenCallWait
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOpenCallWait "AUOpenCallWait"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.dnd_on
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OpenDnd
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOpenDnd "AUOpenDnd"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.no_answer_fwd_on
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | OpenNoAnswerForward
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUOpenNoAnswerForward "AUOpenNoAnswerForward"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_uri_limit_ip
 *         path | /config/user/user.ini
 *      section | ActionURI
 *          key | LimitIP
 *      default | ""
 *          reg | {maxlength:511}
 */
#define kszAURILimitIP "AURILimitIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.show_msgbox
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bActionURIShowMsgBox
 *      default | 1
 */
#define kszxEnableAURIShowMsgBox "AURIShowMsgBoxEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.register_failed
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RegisterFailed
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAURegisterFailed "AURegisterFailed"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.reject_incoming_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RejectIncomingCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAURejectIncomingCall "AURejectIncomingCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.actionurl.rejectnewincall
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RejectNewInCall
 *      default | ""
 */
#define kszAURejectNewInCall "AURejectNewInCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.remote_busy
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RemoteBusy
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUCallRemoteBusy "AURemoteBusy"

/**
 * @brief 注释
 */
#define kszAURemoteForward "AURemoteForward"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.held
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RemoteHold
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAURemoteHold "AURemoteHold"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.report_server
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RemoteIP
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAURemoteIP "AURemoteIP"

/**
 * @brief 对方无应答的URL
 */
#define kszAURemoteNoAnswer "AURemoteNoAnswer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.unheld
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | RemoteUnHold
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAURemoteUnHold "AURemoteUnHold"

/**
 * @brief AUResponseMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.action_url.response.mode
 *         path | /config/user/user.ini
 *      section | ActionURI
 *          key | ResponseMode
 *      default | 0
 */
#define kszAUResponseMode "AUResponseMode"

/**
 * @brief 回铃的URL
 */
#define kszAURingBack "AURingBack"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.setup_completed
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | SetupCompleted
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUSetupCompleted "AUSetupCompleted"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.call_terminated
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | Terminated
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUTerminated "AUTerminated"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.transfer_call
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | TransferCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUTransferCall "AUTransferCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.transfer_failed
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | TransferFailed
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUTransferFailed "AUTransferFailed"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.transfer_finished
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | TransferFinished
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUTransferFinished "AUTransferFinished"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.ume_server
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | UCServer
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUUCServer "AUUCServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.unhold
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | UnHold
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUUnHold "AUUnHold"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.unmute
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | UnMute
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUUnMute "AUUnMute"

/**
 * @brief ActionURL 查看Forward
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.action_url.view_forward
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | AUViewForWard
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUViewForWard "AUViewForWard"

/**
 * @brief ActionURL 查看Misscall
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.action_url.view_misscall
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | AUViewMissCall
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszAUViewMissCall "AUViewMissCall"

/**
 * @brief 注释 config id 没人用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.acd%%d.unavailreasonindex
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | UnavailReasonIndex
 *      default | ""
 */
#define kszAccountACDUnavailReasonIndex "AccountACDUnavailReasonIndex"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_answer_delay
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | AutoAnswer
 *          key | nTimeout
 *      default | ""
 */
#define kszAutoAnswerTimeout "AccountAutoAnswerTimeout"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dial_tone
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | dial_tone
 *      default | ""
 */
#define kszAccountDialTone "AccountDialTone"

/**
 * @brief PSTN自动应答静音开关
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/voip/pstnAccount.cfg
 *      section | PSTN%%d
 *          key | AutoAnswerMuteSwitch
 *      default | 0
 */
#define kszPSTNAccountAutoAnswerMute "Account.PSTN.%d.AutoAnswerMuteSwitch"

/**
 * @brief PSTN自动应答开关
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/voip/pstnAccount.cfg
 *      section | PSTN%%d
 *          key | AutoAnswerSwitch
 *      default | 0
 */
#define kszPSTNAccountAutoAnswer "Account.PSTN.%d.AutoAnswerSwitch"

/**
 * @brief PSTN账号启用
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/voip/pstnAccount.cfg
 *      section | PSTN%%d
 *          key | Enable
 *      default | 0
 */
#define kszPSTNAccountEnable "Account.PSTN.%d.Enable"

/**
 * @brief PSTN账号标签
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/voip/pstnAccount.cfg
 *      section | PSTN%%d
 *          key | Label
 *      default | ""
 */
#define kszPSTNAccountLabel "Account.PSTN.%d.Label"

/**
 * @brief PSTN账号未接来电记录开关
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/voip/pstnAccount.cfg
 *      section | PSTN%%d
 *          key | MissedCallLog
 *      default | 1
 */
#define kszPSTNAccountMissCallLog "Account.PSTN.%d.MissedCallLog"

/**
 * @brief PSTN铃声
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/voip/pstnAccount.cfg
 *      section | PSTN%%d
 *          key | RingFile
 *      default | Common
 */
#define kszPSTNAccountRing "Account.PSTN.%d.RingFile"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.available
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | bAvailabilityState
 *      default | 1
 */
#define kszAccountACDAvailabilityState "AccountACDAvailabilityState"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.initial_state
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | eInitialState
 *      default | 1
 */
#define kszAccountACDInitialState "AccountACDInitialState"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.bsacd.acdlogonstatus
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | BSACD
 *          key | ACDLogOnStatus
 *      default | 1
 */
#define kszAccountACDLogOnStatus "AccountACDLogOnStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.unavailable_reason_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | bEnableUnavailableReason
 *      default | 0
 */
#define kszAccountACDUnavailReasonSwitch "AccountACDUnavailReasonSwitch"

/**
 * @brief 注释 config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.calloption.bactivecamera
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallOption
 *          key | bActiveCamera
 *      default | ""
 */
#define kszAccountActiveCamera "AccountActiveCamera"

/**
 * @brief Anonymous call Mode.
 */
#define kszAnonymousCallMode "AccountAnonymousCallMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.anonymous_call_offcode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AnonymousCall_OffCode
 *      default | ""
 *          reg | {exclude:'!&',maxlength:32}
 */
#define kszAnonymousCallOffCode "AccountAnonymousCallOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.anonymous_call_oncode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AnonymousCall_OnCode
 *      default | ""
 *          reg | {exclude:'!&',maxlength:32}
 */
#define kszAnonymousCallOnCode "AccountAnonymousCallOnCode"

/**
 * @brief AccountAnonymousCallServerBaseOnly
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.anonymous_call.server_base_only
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AnonymousCallServerOnly
 *      default | ""
 */
#define kszAnonymousCallServerBaseOnly "AccountAnonymousCallServerBaseOnly"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.anonymous_call
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AnonymousCall
 *      default | 0
 */
#define kszAnonymousCallSwitch "AccountAnonymousCallSwitch"

/**
 * @brief AccountAudio
 */
#define kszAccountAudio "AccountAudio"

/**
 * @brief 注释
 */
#define kszAccountAudio0Name "AccountAudio0Name"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio0Priority "AccountAudio0Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio0Switch "AccountAudio0Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio0Type "AccountAudio0Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio10Priority "AccountAudio10Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio10Switch "AccountAudio10Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio10Type "AccountAudio10Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio11Priority "AccountAudio11Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio11Switch "AccountAudio11Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio11Type "AccountAudio11Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio12Priority "AccountAudio12Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio12Switch "AccountAudio12Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio12Type "AccountAudio12Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio13Priority "AccountAudio13Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio13Switch "AccountAudio13Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio13Type "AccountAudio13Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio15Priority "AccountAudio14Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio15Switch "AccountAudio14Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio15Type "AccountAudio14Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio16Priority "AccountAudio15Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio16Switch "AccountAudio15Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio16Type "AccountAudio15Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio17Priority "AccountAudio16Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio17Switch "AccountAudio16Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio17Type "AccountAudio16Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio18Priority "AccountAudio17Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio18Switch "AccountAudio17Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio18Type "AccountAudio17Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio19Priority "AccountAudio18Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio19Switch "AccountAudio18Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio19Type "AccountAudio18Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio1Priority "AccountAudio1Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio1Switch "AccountAudio1Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio1Type "AccountAudio1Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio2Priority "AccountAudio2Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio2Switch "AccountAudio2Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio2Type "AccountAudio2Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio3Priority "AccountAudio3Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio3Switch "AccountAudio3Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio3Type "AccountAudio3Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio4Priority "AccountAudio4Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio4Switch "AccountAudio4Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio4Type "AccountAudio4Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio5Priority "AccountAudio5Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio5Switch "AccountAudio5Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio5Type "AccountAudio5Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio6Priority "AccountAudio6Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio6Switch "AccountAudio6Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio6Type "AccountAudio6Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio7Priority "AccountAudio7Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio7Switch "AccountAudio7Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio7Type "AccountAudio7Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio8Priority "AccountAudio8Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio8Switch "AccountAudio8Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio8Type "AccountAudio8Type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *      default | ""
 */
#define kszAccountAudio9Priority "AccountAudio9Priority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *      default | 1
 */
#define kszAccountAudio9Switch "AccountAudio9Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *      default | ""
 */
#define kszAccountAudio9Type "AccountAudio9Type"

/**
 * @brief AccountAudioEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | audio%%d
 *          key | enable
 *      default | ""
 */
#define kszAccountAudioEnable "AccountAudioEnable"

/**
 * @brief AccountAudioPara
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.para
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | audio%%d
 *          key | para
 *      default | ""
 */
#define kszAccountAudioPara "AccountAudioPara"

/**
 * @brief AccountAudioPayloadType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.payload_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | audio%%d
 *          key | PayloadType
 *      default | ""
 */
#define kszAccountAudioPayloadType "AccountAudioPayloadType"

/**
 * @brief AccountAudioPriority
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.codec.%%d.priority
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | audio%%d
 *          key | priority
 *      default | ""
 */
#define kszAccountAudioPriority "AccountAudioPriority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_dial_num
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountAuthCallNumber
 *      default | ""
 */
#define kszAccountAuthCallNumber "AccountAuthCallNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_dial_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountAuthCallSwitch
 *      default | 0
 */
#define kszAccountAuthCallSwitch "AccountAuthCallSwitch"

/**
 * @brief 自动接听并静音
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_answer_mute_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | AutoAnswer
 *          key | MuteEnable
 *      default | 0
 */
#define kszAccountAutoAnswerMuteSwitch "AccountAutoAnswerMuteSwitch"
#define kszAutoAnswerMuteSwitch "AccountAutoAnswerMuteSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_answer
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | AutoAnswer
 *          key | bEnable
 *      default | 0
 */
#define kszAutoAnswerSwitch "AccountAutoAnswerSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_answer_delay
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | AutoAnswer
 *          key | nTimeout
 *      default | 0
 */
#define kszAccountAutoAnswerTimeout "AccountAutoAnswerTimeout"

/**
 * @brief auto label switch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_label.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AutoLable
 *      default | 0
 */
#define kszAccountAutoLable "AccountAutoLable"

/**
 * @brief auto label rule
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auto_label.rule
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AutoLableRule
 *      default | ""
 */
#define kszAccountAutoLableRule "AccountAutoLableRule"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.bla_number
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BLANumber
 *      default | ""
 *          reg | {exclude:'&',maxlength:99}
 */
#define kszBLANumber "AccountBLANumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.bla_subscribe_period
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BLASubscribePeriod
 *      default | 300
 *          reg | {required:true,range:[60,7200],maxlength:4}
 */
#define kszAccountBLASubscribePeriod "AccountBLASubscribePeriod"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.shared_line
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | ShareLine
 *      default | 0
 */
#define kszBLAType "AccountBLAType"

/**
 * @brief 通过BLF发生DTMF转接
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_dtmf_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BLFDTMFCode
 *      default | ""
 */
#define kszAccountBLFDTMFCode "AccountBLFDTMFCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_list_barge_in_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BlfListBargeInCode
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszBLFListBargeInCode "AccountBLFListBargeInCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_list_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BlfListCode
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszBLFListCode "AccountBLFListCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf.blf_list_uri
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | blf
 *          key | BLFList_URI
 *      default | ""
 *          reg | {exclude:'&',maxlength:255}
 */
#define kszAccountBLFListURL "AccountBLFListURL"

/**
 * @brief backup outbound
 */
#define kszAccountBakOutboundHost "AccountBakOutboundHost"

/**
 * @brief AccountBakOutboundPort
 */
#define kszAccountBakOutboundPort "AccountBakOutboundPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.backup_sip_server_host
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BakSIPServerHost
 *      default | ""
 */
#define kszAccountBakSIPServerHost "AccountBakSIPServerHost"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.backup_sip_server_port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BakSIPServerPort
 *      default | 5060
 */
#define kszAccountBakSIPServerPort "AccountBakSIPServerPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.balance_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BalanceEnable
 *      default | 0
 */
#define kszAccountBalanceEnable "AccountBalanceEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.balance_link_url
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BalanceLink
 *      default | ""
 */
#define kszAccountBalanceLink "AccountBalanceLink"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.advanced.totalbandwidth
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ADVANCED
 *          key | TotalBandwidth
 *      default | ""
 */
#define kszAccountBandwidth "AccountBandwidth"

/**
 * @brief SCA BargeIn 功能是否启用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.share_line.barge_in.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | bEnableBargeIn
 *      default | 1
 */
#define kszAccountBargeInEnable "AccountBargeInEnable"

/**
 * @brief 注释
 */
#define kszAccountBasicAnonymousCallEnabled "AccountBasicAnonymousCallEnabled"

/**
 * @brief 注释
 */
#define kszAccountBasicAnonymousCallOffCode "AccountBasicAnonymousCallOffCode"

/**
 * @brief 注释
 */
#define kszAccountBasicAnonymousCallOnCode "AccountBasicAnonymousCallOnCode"

/**
 * @brief 注释
 */
#define kszAccountBasicAnonymousRejectionEnabled "AccountBasicAnonymousRejectionEnabled"

/**
 * @brief 注释
 */
#define kszAccountBasicAnonymousRejectionOffCode "AccountBasicAnonymousRejectionOffCode"

/**
 * @brief 注释
 */
#define kszAccountBasicAnonymousRejectionOnCode "AccountBasicAnonymousRejectionOnCode"

/**
 * @brief 注释
 */
#define kszAccountBasicAutoAnswer "AccountBasicAutoAnswer"

/**
 * @brief 注释
 */
#define kszAccountBasicMissCalllog "AccountBasicMissCalllog"

/**
 * @brief 注释
 */
#define kszAccountBasicProxyRequire "AccountBasicProxyRequire"

/**
 * @brief 注释
 */
#define kszAccountBasicRingType "AccountBasicRingType"

/**
 * @brief AccountBasicSendAnonymousRejectionCode
 */
#define kszAccountBasicSendAnonymousRejectionCode "AccountBasicSendAnonymousRejectionCode"

/**
 * @brief 注释
 */
#define kszWebAccountBasicSwitchAccount "AccountBasicSwitchAccount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.account-basic.voice_mail
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Account-Basic
 *          key | voice_mail
 *      default | ""
 */
#define kszAccountBasicVoiceMail "AccountBasicVoiceMail"

/**
 * @brief AccountBfcpSwitch
 */
#define kszAccountBfcpSwitch "AccountBfcpSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.xsi.host
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIHostAddr
 *      default | ""
 *          reg | {address:true,maxlength:511}
 */
#define kszAccountXSIHostAddr "AccountBroadsoftXSIHostAddr"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.xsi.password
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIPassword
 *      default | ""
 *      encrypt | True
 *          reg | {maxlength:99}
 */
#define kszAccountXSIPassword "AccountBroadsoftXSIPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.xsi.port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIPort
 *      default | 80
 *          reg | {required:true,range:[1,65535,80],maxlength:5}
 */
#define kszAccountXSIPort "AccountBroadsoftXSIPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.xsi.server_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIServerType
 *      default | http
 */
#define kszAccountXSIServerType "AccountBroadsoftXSIServerType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.xsi.user
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIUserID
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszAccountXSIUserID "AccountBroadsoftXSIUserID"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.cid_source
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CIDSource
 *      default | 0
 */
#define kszAccountCIDSource "AccountCIDSource"

/**
 * @brief call decline
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.features.call_decline.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountCallDeclineEnable
 *      default | ""
 */
#define kszAccountCallDeclineEnable "AccountCallDeclineEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.show_call_info_time
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | nShowCallInfoTime
 *      default | 30
 */
#define kszAccountCallinfoTime "AccountCallInfoTime"
#define kszAccountShowCallInfoTime "AccountCallInfoTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.calloption.ecallmode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallOption
 *          key | eCallMode
 *      default | 1
 */
#define kszAccountCallMode "AccountCallMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.callparknumber
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CallParkNumber
 *      default | ""
 */
#define kszAccountCallParkNumber "AccountCallParkNumber"

/**
 * @brief SCA CallPull 功能是否启用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.share_line.call_pull.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | bEnableCallPull
 *      default | 1
 */
#define kszAccountCallPullEnable "AccountCallPullEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.shared_line_callpull_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | strCallFeatureCode
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszAccountCallPullFeatureCode "AccountCallPullFeatureCode"
#define kszCallPullFeatureCode "AccountCallPullFeatureCode"

/**
 * @brief AccountCallRecordSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_recording.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountCallRecordSwitch
 *      default | ""
 */
#define kszAccountCallRecordSwitch "AccountCallRecordSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.ringback_duration_time
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CancelTimer
 *      default | 180
 */
#define kszAccountCancelCallOutTime "AccountCancelCallOutTime"

/**
 * @brief 注释
 */
#define kszWebAccountCodecSwitchAccount "AccountCodecSwitchAccount"

/**
 * @brief 注释
 */
#define kszAccountCodecsGroup "AccountCodecsGroup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.conf_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | conf-type
 *      default | 0
 */
#define kszConfType "AccountConfType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.conf_uri
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | conf-uri
 *      default | ""
 *          reg | {maxlength:511}
 */
#define kszConfURI "AccountConfURI"

/**
 * @brief 账号是否使用新的key来创建Conference
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.phone_setting.call_appearance.conference_via_new_linekey
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountConferenceByNewLine
 *      default | ""
 */
#define kszAccountConferenceByNewLine "AccountConferenceByNewLine"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.csta_number
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CstaTelUri
 *      default | ""
 */
#define kszAccountCstaTelUrl "AccountCstaTelUrl"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.direct_pickup_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | DPickupCode
 *      default | ""
 *          reg | {exclude:':;<>[]^?%%',maxlength:32}
 */
#define kszAccountDPickupCode "AccountDPickupCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dtmf.info_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DTMF
 *          key | InfoType
 *      default | 0
 */
#define kszAccountDTMFInfoType "AccountDTMFInfoType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dtmf.dtmf_payload
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DTMF
 *          key | DTMFPayload
 *      default | 0
 *          reg | {range:[96,127],maxlength:3}
 */
#define kszAccountDTMFPayload "AccountDTMFPayload"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dtmf.type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DTMF
 *          key | DTMFInbandTransfer
 *      default | 0
 */
#define kszAccountDTMFType "AccountDTMFType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.timer_t1
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | default_t1
 *      default | 0.5
 *          reg | {float:true,range:[0.5,10],maxlength:32}
 */
#define kszAccountDefaultT1 "AccountDefaultT1"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.timer_t2
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | default_t2
 *      default | 4
 *          reg | {float:true,range:[2,40],maxlength:32}
 */
#define kszAccountDefaultT2 "AccountDefaultT2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.timer_t4
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | default_t4
 *      default | 5
 *          reg | {float:true,range:[2.5,60],maxlength:32}
 */
#define kszAccountDefaultT4 "AccountDefaultT4"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialoginfo_callpickup
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | dialoginfo_callpickup
 *      default | 0
 */
#define kszAccountDialogPickUp "AccountDialogPickup"

/**
 * @brief AccountDigitMapAutoFwd
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.apply_to.forward
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | EnabledInAutoFwd
 *      default | ""
 */
#define kszAccountDigitMapInAutoFwd "AccountDigitMapAutoFwd"

/**
 * @brief AccountDigitMapConfig
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.string
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | DigitMap
 *      default | ""
 */
#define kszAccountDigitMap "AccountDigitMapConfig"

/**
 * @brief AccountDigitMapDial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.apply_to.on_hook_dial
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | EnabledInDial
 *      default | ""
 */
#define kszAccountDigitMapInDial "AccountDigitMapDial"

/**
 * @brief AccountDigitMapDirDial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.apply_to.directory_dial
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | EnabledInDirDial
 *      default | ""
 */
#define kszAccountDigitMapInDirDial "AccountDigitMapDirDial"

/**
 * @brief AccountDigitMapHistoryDial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.apply_to.history_dial
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | EnabledInHistoryDial
 *      default | ""
 */
#define kszAccountDigitMapInHistoryDial "AccountDigitMapHistoryDial"

/**
 * @brief AccountDigitMapInPredial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.active.on_hook_dialing
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | EnabledInPredial
 *      default | ""
 */
#define kszAccountDigitMapInPredial "AccountDigitMapInPredial"

/**
 * @brief AccountDigitMapLongTimer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.interdigit_long_timer
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | LongTimer
 *      default | ""
 */
#define kszAccountDigitMapLongTimer "AccountDigitMapLongTimer"

/**
 * @brief AccountDigitMapNoMatchAction
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.no_match_action
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | NoMatchAction
 *      default | ""
 */
#define kszAccountDigitMapNoMatchAction "AccountDigitMapNoMatchAction"

/**
 * @brief AccountDigitMapPressSend
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.apply_to.press_send
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | EnabledInPressSend
 *      default | ""
 */
#define kszAccountDigitMapInPressSend "AccountDigitMapPressSend"

/**
 * @brief AccountDigitMapShortTimer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.interdigit_short_timer
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | ShortTimer
 *      default | ""
 */
#define kszAccountDigitMapShortTimer "AccountDigitMapShortTimer"

/**
 * @brief AccountDigitMapSwtich
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dialplan.digitmap.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DigitMap
 *          key | bEnabled
 *      default | ""
 */
#define kszAccountDigitMapSwitch "AccountDigitMapSwtich"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.direct_ip.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | directip
 *      default | 0
 */
#define kszAccountDirectIP "AccountDirectIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.alert_info_url_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | DisableAlertInfoURL
 *      default | 0
 */
#define kszAccountDisableAlertInfoURL "AccountDisableAlertInfoURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.display_name
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | DisplayName
 *      default | ""
 *          reg | {not:{regular:'[:;<>[\]?%%]'},maxlength:99}
 */
#define kszAccountDisplayName "AccountDisplayName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.distinctive_ring_tones
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | Distinctive_Ring_Tones
 *      default | ""
 */
#define kszAccountDistinctiveRingSwitch "AccountDistinctiveRingSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | Enable
 *      default | 0
 */
#define kszAccountEnable "AccountEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.100rel_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | Enable 100Rel
 *      default | 0
 */
#define kszAccountEnable100Rel "AccountEnable100Rel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | bEnable
 *      default | 0
 */
#define kszAccountEnableACD "AccountEnableACD"

/**
 * @brief 通过BLF发生DTMF转接
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_send_dtmf.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableBLFSendDTMF
 *      default | 0
 */
#define kszAccountEnableBLFSendDTMF "AccountEnableBLFSendDTMF"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.callpark_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableBroadworksCallpark
 *      default | 0
 */
#define kszAccountEnableBroadworksCallpark "AccountEnableBroadworksCallpark"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.cp_source
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CPSource
 *      default | ""
 */
#define kszAccountEnableCPSource "AccountEnableCPSource"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.call_info_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | bEnableCallInfo
 *      default | 0
 */
#define kszAccountEnableCallInfo "AccountEnableCallInfo"

/**
 * @brief AccountEnableDTMFReplaceTran
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.dtmf.dtmf_replace_tran
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DTMF
 *          key | DTMF_Replace_Tran
 *      default | 0
 */
#define kszAccountEnableDTMFReplaceTran "AccountEnableDTMFReplaceTran"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.disp_code_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | bEnableDispCode
 *      default | 0
 */
#define kszAccountEnableDiapositionCode "AccountEnableDiapositionCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.emergency_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | bEnableEmergency
 *      default | 0
 */
#define kszAccountEnableEmergencyEscalation "AccountEnableEmergencyEscalation"

/**
 * @brief 配置on/off code是否使用DialPlan规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.flexible_seating.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Hoteling
 *          key | FlexibleSeatingEnable
 *      default | 0
 */
#define kszAccountEnableFlexibleSeating "AccountEnableFlexibleSeating"

/**
 * @brief AccountEnableH323GkCheck
 */
#define kszAccountEnableH323GkCheck "AccountEnableH323GkCheck"

/**
 * @brief AccountEnableH323H460
 */
#define kszAccountEnableH323H460 "AccountEnableH323H460"

/**
 * @brief AccountEnableH323Tunneling
 */
#define kszAccountEnableH323Tunneling "AccountEnableH323Tunneling"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hoteling.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableHoteling
 *      default | 0
 */
#define kszAccountEnableHoteling "AccountEnableHoteling"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.trace_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | bEnableTrace
 *      default | 0
 */
#define kszAccountEnableOriginatedTrace "AccountEnableOriginatedTrace"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.out_dialog_blf_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableOutDialogBLF
 *      default | 0
 */
#define kszAccountEnableOutDialogBLF "AccountEnableOutDialogBLF"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.queue_status_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | bEnableQueueStatus
 *      default | 0
 */
#define kszAccountEnableQueueStatus "AccountEnableQueueStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.call_center.queue_status_light_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | CallCenter
 *          key | bEnableQueueStatusLight
 *      default | 0
 */
#define kszAccountEnableQueueStatusLight "AccountEnableQueueStatusLight"
#define kszAccountQueueStatusLight "AccountEnableQueueStatusLight"

/**
 * @brief 安全等级是否开启
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.security_classification.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableSecurityClassification
 *      default | 0
 */
#define kszAccountEnableSecurityClassification "AccountEnableSecurityClassification"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.session_timer.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableSessionTimer
 *      default | 0
 */
#define kszAccountEnableSessionTimer "AccountEnableSessionTimer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.enablesiplink
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableSipLink
 *      default | ""
 */
#define kszAccountEnableSipLink "AccountEnableSipLink"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.enable_user_equal_phone
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableUserEqualPhone
 *      default | 0
 */
#define kszAccountEnableUserEqualPhone "AccountEnableUserEqualPhone"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.expires.legacy
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | Expire
 *      default | 3600
 */
#define kszAccountExpire "AccountExpire"

/**
 * @brief AccountExtvAddOnce
 */
#define kszAccountExtvAddOnce "AccountExtvAddOnce"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.vaserver.%%d.nframerate
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | vaServer
 *          key | nFrameRate
 *      default | 30
 */
#define kszAccountFrameRate "AccountFrameRate"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.group_pickup_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | GPickupCode
 *      default | ""
 *          reg | {exclude:':;<>[]^?%%',maxlength:32}
 */
#define kszAccountGPickupCode "AccountGPickupCode"

/**
 * @brief AccountH323AutoAnswerSwitch
 */
#define kszAccountH323AutoAnswerSwitch "AccountH323AutoAnswerSwitch"

/**
 * @brief #define  kszAccountH323Label   "AccountH323Label"
 */
#define kszAccountH323DisplayName "AccountH323DisplayName"

/**
 * @brief H323账号开关
 */
#define kszAccountH323Enable "AccountH323Enable"

/**
 * @brief AccountH323Extension
 */
#define kszAccountH323Extension "AccountH323Extension"

/**
 * @brief AccountH323GkServerPort%%d
 */
#define kszAccountH323GkServerPort "AccountH323GkServerPort%d"

/**
 * @brief AccountH323GkType
 */
#define kszAccountH323GkType "AccountH323GkType"

/**
 * @brief AccountH323GkUserName
 */
#define kszAccountH323GkUserName "AccountH323GkUserName"

/**
 * @brief AccountH323MediaCapset
 */
#define kszAccountH323MediaCapset "AccountH323MediaCapset"

/**
 * @brief AccountH323Server%%d
 */
#define kszAccountH323Server "AccountH323Server%d"

/**
 * @brief AccountH323ServerOptions
 */
#define kszAccountH323ServerOptions "AccountH323ServerOptions"

/**
 * @brief AccountH323SrtpType
 */
#define kszAccountH323SrtpType "AccountH323SrtpType"

/**
 * @brief AccountH323password
 */
#define kszAccountH323password "AccountH323password"

/**
 * @brief 隐藏本地号码开关
 */
#define kszAccountHiddenLocalNumber "AccountHiddenLocalNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.hotdesking.registeredurl
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | HotDesking
 *          key | RegisteredURL
 *      default | ""
 */
#define kszAccountHotDeskingRegisteredURL "AccountHotDeskingRegisteredURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.hotdesking.sipnotifyurl
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | HotDesking
 *          key | SipNotifyURL
 *      default | ""
 */
#define kszAccountHotDeskingSipNotifyURL "AccountHotDeskingSipNotifyURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hoteling.auto_login_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Hoteling
 *          key | bAutoLogin
 *      default | 0
 */
#define kszAccountHotelingAutoLoginSwitch "AccountHotelingAutoLoginSwitch"

/**
 * @brief HotelingMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hoteling.mode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Hoteling
 *          key | Mode
 *      default | 0
 */
#define kszAccountHotelingMode "AccountHotelingMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hoteling.password
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Hoteling
 *          key | strPassword
 *      default | ""
 *      encrypt | True
 */
#define kszAccountHotelingPassword "AccountHotelingPassword"

/**
 * @brief HotelingUnlockPin
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hoteling.pin
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Hoteling
 *          key | UnlockPin
 *      default | ""
 */
#define kszAccountHotelingUnlockPin "AccountHotelingUnlockPin"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hoteling.user_id
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Hoteling
 *          key | strUserId
 *      default | ""
 */
#define kszAccountHotelingUserID "AccountHotelingUserID"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.hot_desking.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | IsHotDeskAccount
 *      default | 0
 */
#define kszAccountIsHotDeskAccount "AccountIsHotDeskAccount"

/**
 * @brief AccountKeepAlive
 */
#define kszAccountKeepAlive "AccountKeepAlive"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.label
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | Label
 *      default | ""
 *          reg | {not:{regular:'[:;<>[\]?%%]'},maxlength:99}
 */
#define kszAccountLabel "AccountLabel"

/**
 * @brief 限制账号路数
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.calls_per_line_key
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | LimitSessionNum
 *      default | 0
 */
#define kszAccountLimitSessionNum "AccountLimitSessionNum"

/**
 * @brief 每个账号允许的最大话路数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_appearance.calls_per_linekey
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AccountMaxCallNumPerLine
 *      default | 0
 */
#define kszAccountMaxCallNumPerLine "AccountMaxCallNumPerLine"

/**
 * @brief 账号的最大话路数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.phone_setting.call_appearance.calls_per_linekey
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountMaxCallNumber
 *      default | ""
 */
#define kszAccountMaxCallNumber "AccountMaxCallNumber"

/**
 * @brief 注释 config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.nat.maxrtpport
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | NAT
 *          key | MaxRTPPort
 *      default | ""
 */
#define kszAccountMaxRTPPort "AccountMaxRTPPort"

/**
 * @brief AccountMediaCapset
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.media.capset
 *      default | ""
 */
#define kszAccountMediaCapset "AccountMediaCapset"

/**
 * @brief 注释 config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.nat.minrtpport
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | NAT
 *          key | MinRTPPort
 *      default | ""
 */
#define kszAccountMinRTPPort "AccountMinRTPPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.missed_calllog
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | MissedCallLog
 *      default | 1
 */
#define kszAccountMissCallLog "AccountMissCallLog"

/**
 * @brief AccountMusicOnHoldType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.music_on_hold_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | MusicOnHoldType
 *      default | 1
 */
#define kszAccountMusicOnHoldType "AccountMusicOnHoldType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.music_server_uri
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | MusicServerUri
 *      default | ""
 *          reg | {exclude:'&',maxlength:511}
 */
#define kszAccountMusicServerUri "AccountMusicServerUri"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.nat.udp_update_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | NAT
 *          key | EnableUDPUpdate
 *      default | 1
 */
#define kszAccountNATEnableUDPUpdate "AccountNATEnableUDPUpdate"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.nat.rport
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | NAT
 *          key | rport
 *      default | 0
 */
#define kszAccountNATRport "AccountNATRport"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.nat.udp_update_time
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | NAT
 *          key | UDPUpdateTime
 *      default | 30
 *          reg | {required:true,range:[15,2147483647,30],maxlength:10}
 */
#define kszAccountNATUDPUpdateTime "AccountNATUDPUpdateTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.accountnum
 *         path | /phone/factory/system/system.ini
 *      section | Factory
 *          key | AccountNum
 *      default | 3
 */
#define kszAccountNum "AccountNum"

/**
 * @brief AccountOneTouchBargein
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.shared_line_one_touch_bargein.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SCAOneTouchBargein
 *      default | 0
 */
#define kszAccountOneTouchBargein "AccountOneTouchBargein"

/**
 * @brief AccountOneTouchRetrieve
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.shared_line_one_touch_retrieve.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SCAOneTouchRetrieve
 *      default | 0
 */
#define kszAccountOneTouchRetrieve "AccountOneTouchRetrieve"

/**
 * @brief 注释 config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.outboundproxy1.server
 *       t49-m7 | account.%%d.outbound_proxy.%%d.address
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | outboundProxy1
 *          key | server
 *      default | ""
 *          reg | {maxlength:255}
 */
#define kszAccountOutboundServerHost "AccountOutboundServerHost"

/**
 * @brief 注释 config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.outboundproxy1.port
 *       t49-m7 | account.%%d.outbound_proxy.%%d.port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | outboundProxy1
 *          key | port
 *      default | 5060
 *          reg | {required:true,range:[0,65535,5060],maxlength:5}
 */
#define kszAccountOutboundServerPort "AccountOutboundServerPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.outbound_proxy_enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | UseOutboundProxy
 *      default | ""
 */
#define kszAccountOutboundSwitch "AccountOutboundSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.password
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | password
 *      default | ""
 *      encrypt | True
 *          reg | {maxlength:99}
 */
#define kszAccountPassword "AccountPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.precondition
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | precondition
 *      default | 0
 */
#define kszAccountPrecondition "AccountPrecondition"

/**
 * @brief AccountProxyFallbackInterval
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.outbound_proxy_fallback_interval
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | OutboundFallbackInterval
 *      default | 3600
 *          reg | {required:true,range:[30,2147483647,3600],maxlength:10}
 */
#define kszAccountProxyFallbackInterval "AccountProxyFallbackInterval"
#define kszAccountProxyFallbackIntverval "AccountProxyFallbackInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.proxy_require
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | proxy-require
 *      default | ""
 *          reg | {exclude:'!&',maxlength:99}
 */
#define kszAccountProxyRequire "AccountProxyRequire"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.ptime
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | ptime
 *      default | 20
 */
#define kszAccountPtime "AccountPtime"

/**
 * @brief 注释
 *
 * rtp status包发往的服务器地址
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.vq_rtcpxr.collector_server_host
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CollectorServer
 *      default | ""
 *          reg | {maxlength:128}
 */
#define kszAccountRTCPCollectorAddress "AccountRTCPCollectorAddress"

/**
 * @brief 注释
 *
 * 服务器名
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.vq_rtcpxr.collector_name
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CollectorName
 *      default | ""
 *          reg | {not:{regular:'[:;<>[\]?%%\s]'},maxlength:127}
 */
#define kszAccountRTCPCollectorName "AccountRTCPCollectorName"

/**
 * @brief 注释
 *
 * 服务器端口
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.vq_rtcpxr.collector_server_port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CollectorPort
 *      default | 5060
 *          reg | {required:true,range:[0,65535,5060],maxlength:5}
 */
#define kszAccountRTCPCollectorPort "AccountRTCPCollectorPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.reg_fail_retry_interval
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | RegFailRetryInterval
 *      default | 30
 *          reg | {required:true,range:[0,1800],maxlength:4}
 */
#define kszAccountRegFailRetryInterval "AccountRegFailRetryInterval"

/**
 * @brief 账号是否需要注册
 *
 * 是否需要注册账号，0-不注册也可用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.enabled_account_send_reg.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableReg
 *      default | 1
 */
#define kszAccountRegisterEnable "AccountRegisterEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.register_line
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | RegisterLine
 *      default | 0
 */
#define kszAccountRegisterLine "AccountRegisterLine"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.register_mac
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | RegisterMAC
 *      default | 0
 */
#define kszAccountRegisterMAC "AccountRegisterMAC"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.auth_name
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AuthName
 *      default | ""
 *          reg | {not:{regular:'[:;<>[\]?%%]'},maxlength:99}
 */
#define kszAccountRegisterName "AccountRegisterName"

/**
 * @brief Reject anonymous call Mode.
 */
#define kszRejectAnonymousCallMode "AccountRejectAnonymousCallMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.reject_anonymous_call
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | RejectAnonymousCall
 *      default | 0
 */
#define kszRejectAnonymousCallSwitch "AccountRejectAnonymousCallSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.anonymous_reject_offcode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AnonymousReject_OffCode
 *      default | ""
 *          reg | {exclude:'!&',maxlength:32}
 */
#define kszRejectAnonymousCallOffCode "AccountRejectAnonymousRejectOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.anonymous_reject_oncode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AnonymousReject_OnCode
 *      default | ""
 *          reg | {exclude:'!&',maxlength:32}
 */
#define kszRejectAnonymousCallOnCode "AccountRejectAnonymousRejectOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.ringtone.ring_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | Ring
 *          key | strRingFile
 *      default | Common
 */
#define kszAccountRing "AccountRingFile"
#define kszAccountRingFile "AccountRingFile"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.number_of_linekey
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SCALineKeyNumber
 *      default | 1
 *          reg | {required:true,range:[1,999,1],maxlength:3}
 */
#define kszAccountSCALineKeyNumber "AccountSCALineKeyNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.scaonetouchaction
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SCAOneTouchAction
 *      default | 0
 */
#define kszAccountSCAOneTouchAction "AccountSCAOneTouchAction"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sca_manage_interface_display_time
 *       t49-m7 | phone_setting.sca_manage_interface_display_time
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SCAShowTime
 *      default | 10
 */
#define kszAccountSCAShowTime "AccountSCAShowTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.1.address
 *       t49-m7 | account.%%d.sip_server.%%d.address
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver1
 *          key | server
 *      default | ""
 */
#define kszAccountSIPServer1Host "AccountSIPServer1Host"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.1.port
 *       t49-m7 | account.%%d.sip_server.%%d.port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver1
 *          key | port
 *      default | 5060
 */
#define kszAccountSIPServer1Port "AccountSIPServer1Port"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.2.address
 *       t49-m7 | account.%%d.sip_server.%%d.address
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver2
 *          key | server
 *      default | ""
 */
#define kszAccountSIPServer2Host "AccountSIPServer2Host"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.2.port
 *       t49-m7 | account.%%d.sip_server.%%d.port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver2
 *          key | port
 *      default | 5060
 */
#define kszAccountSIPServer2Port "AccountSIPServer2Port"

/**
 * @brief AccountSIPServer3Host
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.3.address
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver3
 *          key | server
 *      default | ""
 */
#define kszAccountSIPServer3Host "AccountSIPServer3Host"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server_host.legacy
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SIPServerHost
 *      default | ""
 */
#define kszAccountSIPServerHost "AccountSIPServerHost"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server_port.legacy
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SIPServerPort
 *      default | 5060
 */
#define kszAccountSIPServerPort "AccountSIPServerPort"

/**
 * @brief AccountSIPServerTransport1
 */
#define kszAccountSIPServer1Transport "AccountSIPServerTransport1"

/**
 * @brief AccountSIPServerTransport2
 */
#define kszAccountSIPServer2Transport "AccountSIPServerTransport2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SIPServerType
 *      default | 0
 */
#define kszAccountSIPServerType "AccountSIPServerType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.srtp_auth_tag_mode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SrtpAuthTag
 *      default | 0
 */
#define kszAccountSRTPAuthTag "AccountSRTPAuthTag"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.send_anonymous_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SendAnonymousCode
 *      default | 0
 */
#define kszAccountSendAnonymous "AccountSendAnonymous"
#define kszSendAnonymousCode "AccountSendAnonymous"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.send_anonymous_rejection_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SendAnonymousRejectionCode
 *      default | 0
 */
#define kszAccountSendAnonymousRejection "AccountSendAnonymousRejection"

/**
 * @brief AccountSendMaxResolution
 */
#define kszAccountSendMaxResolution "AccountSendMaxResolution"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.session_timer.expires
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SessionExpires
 *      default | 1800
 *          reg | {required:true,range:[30,7200,1800],maxlength:4}
 */
#define kszAccountSessionExpires "AccountSessionExpires"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.session_timer.refresher
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SessionRefresher
 *      default | 0
 */
#define kszAccountSessionRefresher "AccountSessionRefresher"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.enable_signal_encode
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | EnableSignalEncode
 *      default | 0
 */
#define kszAccountSignalEncode "AccountSignalEncode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.signal_encode_key
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SignalEncodeKey
 *      default | 0
 */
#define kszAccountSignalEncodeKey "AccountSignalEncodeKey"

/**
 * @brief 账号同时呼出的数量限制
 */
#define kszAccountSimultaneousOutgoingNum "AccountSimultaneousOutgoingNum"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.srtp_encryption
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | srtp_encryption
 *      default | 0
 */
#define kszAccountSrtpEncryption "AccountSrtpEncryption"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.nat.nat_traversal
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | NAT
 *          key | NATTraversal
 *      default | 0
 */
#define kszAccountStunSwitch "AccountStunSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.subscribe_acd_expires
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeACDExpire
 *      default | 0
 *          reg | {required:true,range:[120,3600,3600],maxlength:4}
 */
#define kszAccountSubscribeACDExpire "AccountSubscribeACDExpire"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.subscribe_mwi_expires
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeMWIExpire
 *      default | 3600
 *          reg | {range:[0,84600],maxlength:5}
 */
#define kszAccountSubscribeMWIExpire "AccountSubscribeMWIExpire"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.subscribe_mwi_to_vm
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeMWIToVM
 *      default | 0
 */
#define kszAccountSubscribeMWIToVM "AccountSubscribeMWIToVM"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf.subscribe_period
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | blf
 *          key | SubscribePeriod
 *      default | 1800
 *          reg | {required:true,range:[30,2147483647,1800],maxlength:10}
 */
#define kszAccountSubscribePeriod "AccountSubscribePeriod"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.subscribe_mwi
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeMWI
 *      default | 0
 */
#define kszAccountSubsribeMWI "AccountSubsribeMWI"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.subscribe_register
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeRegister
 *      default | 0
 */
#define kszAccountSubsribeRegister "AccountSubsribeRegister"

/**
 * @brief AccountTitleBarLabel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.titlebarlabel
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | TitleBarLabel
 *      default | ""
 */
#define kszAccountStatusBarLabel "AccountTitleBarLabel"

/**
 * @brief 账号是否使用新的key来创建Transfer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.phone_setting.call_appearance.transfer_via_new_linekey
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | AccountTransferByNewLine
 *      default | ""
 */
#define kszAccountTransferByNewLine "AccountTransferByNewLine"

/**
 * @brief AccountTransferDTMF
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.dtmf.transferdtmf
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | DTMF
 *          key | TransferDTMF
 *      default | ""
 */
#define kszAccountTransferDTMF "AccountTransferDTMF"

/**
 * @brief account transport type
 */
#define kszAccountTransport "AccountTransport"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.tvout.bntsc
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | TVOut
 *          key | bNTSC
 *      default | ""
 */
#define kszAccountTvOutputFormat "AccountTvOutputFormat"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.tvout.btvoutput
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | TVOut
 *          key | bTvOutput
 *      default | ""
 */
#define kszAccountTvOutputSwitch "AccountTvOutputSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.unregister_on_reboot
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | UnRegisterOnReboot
 *      default | 0
 */
#define kszAccountUnRegisterOnReboot "AccountUnRegisterOnReboot"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.user_name
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | UserName
 *      default | ""
 *          reg | {not:{regular:'[:;<>[\]?%%\s]'},maxlength:32}
 */
#define kszAccountUserName "AccountUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.display_mwi.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | VMIndication
 *      default | 1
 */
#define kszAccountVMIndication "AccountVMIndication"

/**
 * @brief 注释
 */
#define kszAccountVideoCodecEnable "AccountVideoEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.vaserver.videoformat
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | vaServer
 *          key | VideoFormat
 *      default | ""
 */
#define kszAccountVideoFormat "AccountVideoFormat"

/**
 * @brief 注释
 */
#define kszAccountVideoCodecType "AccountVideoPayloadType"

/**
 * @brief AccountVideoProfileLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.video.%%d.para
 *      default | ""
 */
#define ACCOUNT_VIDEO_PROFILE_LEVEL "AccountVideoProfileLevel"

/**
 * @brief account voicemail enable
 */
#define kszAccountVoiceMailEnable "AccountVoiceMailEnable"

/**
 * @brief 注释
 *
 * account voicemail number
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice_mail.number.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | VoiceMail
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszAccountVoiceMailNumber "AccountVoiceMailNumber"

/**
 * @brief XSI自定义URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.xsi.custom_url
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSICustomURL
 *      default | ""
 */
#define kszAccountXSICustomURL "AccountXSICustomURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.xsi.xsihostport
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIHostPort
 *      default | ""
 */
#define kszAccountXSIHostPort "AccountXSIHostPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.xsi.xsiversion
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | XSI
 *          key | XSIVersion
 *      default | ""
 */
#define kszAccountXSIVersion "AccountXSIVersion"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.earlymedia
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | earlymedia
 *      default | 0
 */
#define kszAccountearlymedia "Accountearlymedia"

/**
 * @brief AccoutCallinfo
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.call_information
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | start2start
 *          key | CallInfo
 *      default | ""
 */
#define kszAccountCallInfo "AccoutCallinfo"

/**
 * @brief AccoutLoginout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.refresh_url
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | start2start
 *          key | Loginout
 *      default | ""
 */
#define kszAccountLoginOut "AccoutLoginout"

/**
 * @brief AccoutSetUnavail
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.away_url
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | start2start
 *          key | SetUnavail
 *      default | ""
 */
#define kszAccountSetUnAvail "AccoutSetUnavail"

/**
 * @brief AccoutSetavail
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.available_url
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | start2start
 *          key | SetAvail
 *      default | ""
 */
#define kszAccountSetAvail "AccoutSetavail"

/**
 * @brief ACD类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.acd.server_type
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | AcdType
 *      default | 0
 */
#define kszAcdType "AcdType"

/**
 * @brief 是否将Available/Avail or Unavailable/Unavail的softkey固定显示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | acd.available_fixed_display.enable
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | bAvailableFixedDisplay
 *      default | 0
 */
#define kszAcdAvailableFixedDisplay "ACDAvailableFixedDisplay"

/**
 * @brief ActionURLFlag
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.check_sync.action_url_flag
 *         path | /config/system/system.ini
 *      section | CheckSync
 *          key | bActionURLFlag
 *      default | 0
 */
#define kszActionURLFlag "ActionURLFlag"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.active_backlight_level
 *         path | /config/user/user.ini
 *      section | Backlight
 *          key | nActiveLevel
 *      default | 6
 */
#define kszActiveLevel "ActiveLevel"

/**
 * @brief 注释
 */
#define kszAdv8021x "Adv8021x"

/**
 * @brief 注释
 */
#define kszAdvACDSubscripPeriod "AdvACDSubscripPeriod"

/**
 * @brief 注释
 */
#define kszAdvBLANumber "AdvBLANumber"

/**
 * @brief 注释
 */
#define kszAdvBLASubscriptionPeriod "AdvBLASubscriptionPeriod"

/**
 * @brief 注释
 */
#define kszAdvBLFListBargeInCode "AdvBLFListBargeInCode"

/**
 * @brief 注释
 */
#define kszWebAdvBLFListRetrieveCode "AdvBLFListRetrieveCode"

/**
 * @brief 注释
 */
#define kszAdvBLFListURI "AdvBLFListURI"

/**
 * @brief 注释
 */
#define kszAdvBlfListCode "AdvBlfListCode"

/**
 * @brief 注释
 */
#define kszAdvBroadworksCallpark "AdvBroadworksCallpark"

/**
 * @brief 注释
 */
#define kszWebAdvCallPullFeatureCode "AdvCallPullFeatureCode"

/**
 * @brief 注释
 */
#define kszAdvCallerIDHeader "AdvCallerIDHeader"

/**
 * @brief 注释
 */
#define kszAdvConferenceType "AdvConferenceType"

/**
 * @brief 注释
 */
#define kszAdvConferenceUri "AdvConferenceUri"

/**
 * @brief 注释
 */
#define kszAdvDTMFItems "AdvDTMFItems"

/**
 * @brief 注释
 */
#define kszAdvDialogInfoCallPickup "AdvDialogInfoCallPickup"

/**
 * @brief 注释
 */
#define kszAdvDirectCallPickupCode "AdvDirectCallPickupCode"

/**
 * @brief 注释
 */
#define kszAdvDistinctiveRingTones "AdvDistinctiveRingTones"

/**
 * @brief 注释
 */
#define kszAdvEarlymedia "AdvEarlymedia"

/**
 * @brief 注释
 */
#define kszAdvEnablePrecondition "AdvEnablePrecondition"

/**
 * @brief 注释
 */
#define kszAdvEnableSignalEncode "AdvEnableSignalEncode"

/**
 * @brief 注释
 */
#define kszAdvGroupCallPickupCode "AdvGroupCallPickupCode"

/**
 * @brief 注释
 */
#define kszAdvICMPv6Status "AdvICMPv6Status"

/**
 * @brief 注释
 */
#define kszAdvLLDPItems "AdvLLDPItems"

/**
 * @brief 注释
 */
#define kszAdvLocalRTPPort "AdvLocalRTPPort"

/**
 * @brief 注释
 */
#define kszAdvLocalSIPPort "AdvLocalSIPPort"

/**
 * @brief 注释
 */
#define kszAdvLoginExpire "AdvLoginExpire"

/**
 * @brief 注释
 */
#define kszAdvMWIItems "AdvMWIItems"

/**
 * @brief 注释
 */
#define kszAdvMusicServerURI "AdvMusicServerURI"

/**
 * @brief 注释
 */
#define kszWebAdvNumberOfLinekey "AdvNumberOfLinekey"

/**
 * @brief 注释
 */
#define kszAdvOpenVPN "AdvOpenVPN"

/**
 * @brief 注释
 */
#define kszAdvOutDialogBLF "AdvOutDialogBLF"

/**
 * @brief 注释
 */
#define kszAdvPortLink "AdvPortLink"

/**
 * @brief 注释
 */
#define kszAdvPtimeItem "AdvPtimeItem"

/**
 * @brief 注释
 */
#define kszAdvRTCPCollectorAddress "AdvRTCPCollectorAddress"

/**
 * @brief 注释
 */
#define kszAdvRTCPCollectorName "AdvRTCPCollectorName"

/**
 * @brief 注释
 */
#define kszAdvRTCPCollectorPort "AdvRTCPCollectorPort"

/**
 * @brief 注释
 */
#define kszAdvRegSurgePrev "AdvRegSurgePrev"

/**
 * @brief 注释
 */
#define kszAdvReliableRetransmission "AdvReliableRetransmission"

/**
 * @brief 注释
 */
#define kszAdvRport "AdvRport"

/**
 * @brief 注释
 */
#define kszWebAdvSCAOneTouchAction "AdvSCAOneTouchAction"

/**
 * @brief 注释
 */
#define kszAdvSIPRegRetryTimer "AdvSIPRegRetryTimer"

/**
 * @brief 注释
 */
#define kszAdvSIPSendLine "AdvSIPSendLine"

/**
 * @brief 注释
 */
#define kszAdvSIPSendMAC "AdvSIPSendMAC"

/**
 * @brief 注释
 */
#define kszAdvSIPServerType "AdvSIPServerType"

/**
 * @brief 注释
 */
#define kszAdvSNMPItems "AdvSNMPItems"

/**
 * @brief 注释
 */
#define kszWebAdvSRTPAuthTag "AdvSRTPAuthTag"

/**
 * @brief 注释
 */
#define kszAdvSessionRefresher "AdvSessionRefresher"

/**
 * @brief 注释
 */
#define kszAdvSessionTimer "AdvSessionTimer"

/**
 * @brief 注释
 */
#define kszAdvShareLine "AdvShareLine"

/**
 * @brief 注释
 */
#define kszAdvSignalEncodeKey "AdvSignalEncodeKey"

/**
 * @brief 注释
 */
#define kszAdvSipSessionTimer "AdvSipSessionTimer"

/**
 * @brief 注释
 */
#define kszAdvSpanToPcport "AdvSpanToPcport"

/**
 * @brief 注释
 */
#define kszAdvSubscribePeriod "AdvSubscribePeriod"

/**
 * @brief 注释
 */
#define kszAdvSubscribeRegister "AdvSubscribeRegister"

/**
 * @brief 注释
 */
#define kszAdvSwitchAccount "AdvSwitchAccount"

/**
 * @brief 注释
 */
#define kszAdvUDPKeepAlive "AdvUDPKeepAlive"

/**
 * @brief 注释
 */
#define kszAdvUnregisterOnReboot "AdvUnregisterOnReboot"

/**
 * @brief 注释
 */
#define kszAdvUseSessionTimer "AdvUseSessionTimer"

/**
 * @brief 注释
 */
#define kszAdvUserEqualPhone "AdvUserEqualPhone"

/**
 * @brief 注释
 */
#define kszAdvVLANItems "AdvVLANItems"

/**
 * @brief AdvVMIndication
 */
#define kszAdvVMIndication "AdvVMIndication"

/**
 * @brief 注释
 */
#define kszAdvVoiceEncryption "AdvVoiceEncryption"

/**
 * @brief 注释
 */
#define kszAdvVoiceMail "AdvVoiceMail"

/**
 * @brief 注释
 */
#define kszAdvVoiceQoS "AdvVoiceQoS"

/**
 * @brief 注释
 */
#define kszAdvWebServer "AdvWebServer"

/**
 * @brief AdvancedAccountNameIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.special_account_name_ime.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AdvancedAccountNameIME
 *      default | 2aB
 */
#define kszAdvancedAccountNameIME "AdvancedAccountNameIME"

/**
 * @brief AdvancedAccountPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.special_account_password_ime.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AdvancedAccountPasswordIME
 *      default | abc
 */
#define kszAdvancedAccountPasswordIME "AdvancedAccountPasswordIME"

/**
 * @brief AdvancedSettingPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.advanced_setting_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AdvancedSettingPasswordIME
 *      default | 2
 */
#define kszAdvancedSettingPasswordIME "AdvancedSettingPasswordIME"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.var_enable
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | var_enabled
 *      default | 0
 */
#define kszAdvancedVarEnabled "AdvancedVarEnabled"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.alert_info_tone
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AlertInfoTone
 *      default | 0
 */
#define kszAlertInfo "AlertInfo"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.1.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo0
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo0Ringer "AlertInfo0Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.1.text
 *         path | /config/user/user.ini
 *      section | AlertInfo0
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo0Text "AlertInfo0Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.2.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo1
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo1Ringer "AlertInfo1Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.2.text
 *         path | /config/user/user.ini
 *      section | AlertInfo1
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo1Text "AlertInfo1Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.3.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo2
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo2Ringer "AlertInfo2Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.3.text
 *         path | /config/user/user.ini
 *      section | AlertInfo2
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo2Text "AlertInfo2Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.4.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo3
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo3Ringer "AlertInfo3Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.4.text
 *         path | /config/user/user.ini
 *      section | AlertInfo3
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo3Text "AlertInfo3Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.5.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo4
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo4Ringer "AlertInfo4Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.5.text
 *         path | /config/user/user.ini
 *      section | AlertInfo4
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo4Text "AlertInfo4Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.6.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo5
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo5Ringer "AlertInfo5Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.6.text
 *         path | /config/user/user.ini
 *      section | AlertInfo5
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo5Text "AlertInfo5Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.7.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo6
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo6Ringer "AlertInfo6Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.7.text
 *         path | /config/user/user.ini
 *      section | AlertInfo6
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo6Text "AlertInfo6Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.8.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo7
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo7Ringer "AlertInfo7Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.8.text
 *         path | /config/user/user.ini
 *      section | AlertInfo7
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo7Text "AlertInfo7Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.9.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo8
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo8Ringer "AlertInfo8Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.9.text
 *         path | /config/user/user.ini
 *      section | AlertInfo8
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo8Text "AlertInfo8Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.10.ringer
 *         path | /config/user/user.ini
 *      section | AlertInfo9
 *          key | Ringer
 *      default | 1
 */
#define kszAlertInfo9Ringer "AlertInfo9Ringer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | distinctive_ring_tones.alert_info.10.text
 *         path | /config/user/user.ini
 *      section | AlertInfo9
 *          key | Text
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAlertInfo9Text "AlertInfo9Text"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.voic.balertreportenable
 *         path | /config/user/vpm.cfg
 *      section | Voic
 *          key | bAlertReportEnable
 *      default | ""
 */
#define kszAlertReportSwitch "AlertReportSwitch"

/**
 * @brief AlertVoiceMail
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.voice_mail_alert.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AlertVoiceMail
 *      default | 1
 */
#define kszAlertVoiceMail "AlertVoiceMail"

/**
 * @brief AllAccountState
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account.strallstate
 *         path | /tmp/tmpAllAccountState
 *      section | Account
 *          key | strAllState
 *      default | ""
 */
#define kszAllAccountState "AllAccountState"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.allow_mute
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | AllowMute
 *      default | 1
 */
#define kszAllowMute "AllowMute"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.authentication_for_xsi
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | AllowSipAuth
 *      default | 0
 */
#define kszAllowSipAuth "AllowSipAuth"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.multi_call_trans_enable
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | BeAllowTransExistCall
 *      default | 1
 */
#define kszAllowTransExistCall "AllowTransExistCall"

/**
 * @brief 是否允许转接
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.tran.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AllowTransfer
 *      default | 1
 */
#define kszAllowTransfer "AllowTransfer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.always.off_code
 *         path | /config/user/user.ini
 *      section | AlwaysForward
 *          key | strOffCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszForwardAlwaysOffCode "AlwaysForwardOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.always.on_code
 *         path | /config/user/user.ini
 *      section | AlwaysForward
 *          key | strOnCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszForwardAlwaysOnCode "AlwaysForwardOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.always.enable
 *         path | /config/user/user.ini
 *      section | AlwaysForward
 *          key | bEnable
 *      default | 0
 */
#define kszForwardAlwaysSwitch "AlwaysForwardSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.always.target
 *         path | /config/user/user.ini
 *      section | AlwaysForward
 *          key | strTarget
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszForwardAlwaysTarget "AlwaysForwardTarget"

/**
 * @brief AlwaysForwardTargetBackup
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.forward.always.target
 *         path | /config/user/user.ini
 *      section | AlwaysForward
 *          key | strTargetBackup
 *      default | ""
 */
#define kszForwardAlwaysTargetBackup "AlwaysForwardTargetBackup"

/**
 * @brief AnimationDuration
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phonesetting.animation_duration
 *      default | 250
 */
#define kszAnimationDuration "AnimationDuration"

/**
 * @brief AnswerByHand
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.off_hook_answer.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AnswerByHand
 *      default | 1
 */
#define kszAnswerByHand "AnswerByHand"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_anywhere_remote_office_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AnyWhere&RmoteOfficeOn
 *      default | 1
 */
#define kszAnyWhere_RmoteOfficeEnable "AnyWhere&RmoteOfficeEnable"

/**
 * @brief 注释
 */
#define kszAppServerAddress "AppServerAddress"

/**
 * @brief 注释
 */
#define kszAppServerPort "AppServerPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.area_code.code
 *         path | /config/user/user.ini
 *      section | AreaCode
 *          key | strcode
 *      default | ""
 *          reg | {maxlength:16}
 */
#define kszAreaCode "AreaCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.area_code.line_id
 *         path | /config/user/user.ini
 *      section | AreaCode
 *          key | strLineID
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszAreaCodeAccount "AreaCodeAccount"

/**
 * @brief 是否开启AreaCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.area_code.enable
 *         path | /config/user/user.ini
 *      section | AreaCode
 *          key | IsEnable
 *      default | 0
 */
#define kszAreaCodeEnable "AreaCodeEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.area_code.max_len
 *         path | /config/user/user.ini
 *      section | AreaCode
 *          key | strmaxlen
 *      default | 15
 *          reg | {required:true,range:[1,15,15],maxlength:2}
 */
#define kszAreaCodeMaxLen "AreaCodeMaxLen"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.area_code.min_len
 *         path | /config/user/user.ini
 *      section | AreaCode
 *          key | strminlen
 *      default | 1
 *          reg | {required:true,range:[1,15,1],maxlength:2}
 */
#define kszAreaCodeMinLen "AreaCodeMinLen"

/**
 * @brief 紧急呼叫
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.asserted_id_source
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | AssertedIdSource
 *      default | ELIN
 */
#define kszAssertedIdSource "AssertedIdSource"

/**
 * @brief AudioOpusSampleRate
 */
#define kszAudioOpusSampleRate "AudioOpusSampleRate"

/**
 * @brief 自动接听静音
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.mute.autoanswer_mute.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoAnswerAudioMute
 *      default | 0
 */
#define kszAutoAnswerAudioMute "AutoAnswerAudioMute"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_answer_delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoAnswerDelay
 *      default | 3
 *          reg | {range:[1,4,1],maxlength:1}
 */
#define kszAutoAnswerDelay "AutoAnswerDelay"

/**
 * @brief AutoAnswerFirstCallOnly
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_answer.first_call_only
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoAnswerFirstCallOnly
 *      default | 1
 */
#define kszAutoAnswerFirstCallOnly "AutoAnswerFirstCallOnly"

/**
 * @brief AutoAnswerMultiCall
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.autoanswermulticall
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoAnswerMultiCall
 *      default | 0
 */
#define kszAutoAnswerMultiCall "AutoAnswerMultiCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.auto_blf_list_enable
 *         path | /config/user/user.ini
 *      section | AutoBlflist
 *          key | Enable
 *      default | 0
 */
#define kszAutoBlfListEnable "AutoBlfListEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.blf_list_sequence_type
 *         path | /config/user/user.ini
 *      section | AutoBlflist
 *          key | SequenceType
 *      default | 1
 */
#define kszAutoBlfListSequenceType "AutoBlfListSequenceType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.account1_username_prefix
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AccountPrefix
 *      default | ""
 */
#define kszAutoConfigAccountPrefix "AutoConfigAccountPrefix"

/**
 * @brief AutoIntercomToHeadset
 */
#define kszAutoIntercomToHeadset "AutoIntercomToHeadset"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.auto_line.enable
 *         path | /config/user/user.ini
 *      section | Sync
 *          key | AutoLine
 *      default | 0
 */
#define kszAutoLine "AutoLine"

/**
 * @brief AutoLinekeysSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_linekeys.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoLinekeysSwitch
 *      default | 0
 */
#define kszAutoLinekeys "AutoLinekeysSwitch"
#define kszAutoLinekeysSwitch "AutoLinekeysSwitch"

/**
 * @brief 注释
 */
#define kszAutoLogoutSwitch "AutoLogoutSwitch"

/**
 * @brief 注释
 */
#define kszAutoLogoutTime "AutoLogoutTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.strprivisionpwd
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strPrivisionPwd
 *      default | ""
 */
#define kszAutoPrivisionPwd "AutoPrivisionPwd"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.strprivisionuser
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strPrivisionUser
 *      default | ""
 */
#define kszAutoPrivisionUser "AutoPrivisionUser"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.aes_key_16.com
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strKeyAES16
 *      default | ""
 *      encrypt | True
 *          reg | {not:{regular:'[^\w~@#$%%^*[\]{},.:?=+-]'},maxlength:16}
 */
#define kszAesKey "AutoProvisionAESKey16"
#define kszAutoProvisionAESKey16 "AutoProvisionAESKey16"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.aes_key_16.mac
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strKeyAES16MAC
 *      default | ""
 *      encrypt | True
 *          reg | {not:{regular:'[^\w~@#$%%^*[\]{},.:?=+-]'},maxlength:16}
 */
#define kszAesMacKey "AutoProvisionAESKey16MAC"
#define kszAutoProvisionAESKey16MAC "AutoProvisionAESKey16MAC"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.cfg_erase_pwd
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | CfgErasePwd
 *      default | ""
 */
#define kszAutoProvisionCfgErasePwd "AutoProvisionCfgErasePwd"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.common_file_name
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strCommName
 *      default | ""
 */
#define kszAutoProvisionCommonName "AutoProvisionCommonName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.custom.protect
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | CustomProtect
 *      default | 0
 *  t49-default | 1
 */
#define kszAutoProvisionCustomProtect "AutoProvisionCustomProtect"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.custom.sync
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | CustomSync
 *      default | 0
 *  t49-default | 1
 */
#define kszAutoProvisionCustomSync "AutoProvisionCustomSync"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.dhcp_option.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnableDHCPOption
 *      default | 1
 */
#define kszAutoProvisionEnableDHCPOption "AutoProvisionEnableDHCPOption"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.pnp_enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnablePNP
 *      default | 1
 */
#define kszAutoProvisionEnablePNP "AutoProvisionEnablePNP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.weekly.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnableWeekly
 *      default | 0
 */
#define kszAutoProvisionEnableWeekly "AutoProvisionEnableWeekly"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.common_file_name
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strCommName
 *      default | ""
 */
#define kszAutoProvisionFctCommonName "AutoProvisionFctCommonName"
#define kszauto_provision_FacCommonFileName "AutoProvisionFctCommonName"

/**
 * @brief AutoProvisionFlexibleBeginTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.flexible.begin_time
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strFlexibleBeginTime
 *      default | 02:00
 */
#define kszAutoProvisionFlexibleBeginTime "AutoProvisionFlexibleBeginTime"

/**
 * @brief AutoProvisionFlexibleEndTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.flexible.end_time
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strFlexibleEndTime
 *      default | ""
 */
#define kszAutoProvisionFlexibleEndTime "AutoProvisionFlexibleEndTime"

/**
 * @brief AutoProvisionFlexibleInterval
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.flexible.interval
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | FlexibleInterval
 *      default | 30
 */
#define kszAutoProvisionFlexibleInterval "AutoProvisionFlexibleInterval"

/**
 * @brief AutoProvisionFlexibleSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.flexible.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnableFlexible
 *      default | 0
 */
#define kszAutoProvisionFlexibleSwitch "AutoProvisionFlexibleSwitch"

/**
 * @brief autop请求时话机空闲时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.inactivity_time_expire
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | nInactivityTimeExpire
 *      default | 0
 */
#define kszAutoProvisionInactivityTimeExpire "AutoProvisionInactivityTimeExpire"
#define kszInactivityTimeExpire "AutoProvisionInactivityTimeExpire"

/**
 * @brief 注释
 */
#define kszAutoProvisionMode "AutoProvisionMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.dhcp_option.list_user_options
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | listUserOptions
 *      default | ""
 *          reg | {range:[128,254],maxlength:3}
 */
#define kszAutoProvisionListUserOptions "AutoProvisionOptionType"
#define kszAutoProvisionOptionType "AutoProvisionOptionType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.dhcp_option.list_user6_options
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | listUser6Options
 *      default | ""
 */
#define kszAutoProvisionOptionTypeIPv6 "AutoProvisionOptionTypeIPv6"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.server.password
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strPassword
 *      default | ""
 *      encrypt | True
 *          reg | {maxlength:32}
 */
#define kszAutoProvisionPassword "AutoProvisionPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.power_on
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnablePowerOn
 *      default | 0
 */
#define kszAutoProvisionPowerOn "AutoProvisionPowerOn"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.repeat.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnableRepeat
 *      default | 0
 */
#define kszAutoProvisionRepeat "AutoProvisionRepeat"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.repeat.minutes
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | nRepeatMinutes
 *      default | 1440
 *          reg | {required:true,range:[1,43200,1440],maxlength:5}
 */
#define kszAutoProvisionRepeatMinutes "AutoProvisionRepeatMinutes"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.strserverip
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strServerIp
 *      default | ""
 */
#define kszAutoProvisionServerIp "AutoProvisionServerIp"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.server.url
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strServerURL
 *      default | ""
 *          reg | {maxlength:511}
 */
#define kszAutoProvisionUrl "AutoProvisionServerURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.tpg.option_pnp
 *         path | /config/system/system.ini
 *      section | TPG
 *          key | Option_PNP
 *      default | ""
 */
#define kszAutoProvisionTPGOptionPNP "AutoProvisionTPGOptionPNP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.server.username
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strUserName
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszAutoProvisionUsername "AutoProvisionUser"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.weekly.begin_time
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strWeeklyBeginTime
 *      default | 0:0
 *          reg | {range:[0,23,0],maxlength:2}
 */
#define kszAutoProvisionWeeklyBeginTime "AutoProvisionWeeklyBeginTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.weekly.end_time
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strWeeklyEndTime
 *      default | 0:0
 *          reg | {range:[0,23,0],maxlength:2}
 */
#define kszAutoProvisionWeeklyEndTime "AutoProvisionWeeklyEndTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.weekly.dayofweek
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strWeeklyMask
 *      default | ""
 */
#define kszAutoProvisionWeeklyMask "AutoProvisionWeeklyMask"

/**
 * @brief AutoProvisionWeeklyUpgradeInterval
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.weekly_upgrade_interval
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | nWeeklyUpgradeInterval
 *      default | 0
 */
#define kszAutoProvisionWeeklyUpgradeInterval "AutoProvisionWeeklyUpgradeInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_redial.interval
 *         path | /config/user/user.ini
 *      section | AutoRedial
 *          key | nInterval
 *      default | 1
 *  t49-default | 10
 *          reg | {required:true,range:[1,300,10],maxlength:3}
 */
#define kszAutoRedialInterval "AutoRedialInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_redial.enable
 *         path | /config/user/user.ini
 *      section | AutoRedial
 *          key | bEnable
 *      default | 0
 */
#define kszAutoRedialSwitch "AutoRedialSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_redial.times
 *         path | /config/user/user.ini
 *      section | AutoRedial
 *          key | nTimes
 *      default | 1
 *  t49-default | 10
 *          reg | {required:true,range:[1,300,10],maxlength:3}
 */
#define kszAutoRedialTimes "AutoRedialTimes"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.ringing_timeout
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoRefuseTime
 *      default | 120
 */
#define kszAutoRefuseTime "AutoRefuseTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_release_bla_line
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoReleaseLineBySrv
 *      default | 0
 */
#define kszAutoReleaseLineBySrv "AutoReleaseLineBySrv"

/**
 * @brief 失败重试延迟时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.nattemptdelay
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | nAttemptDelay
 *      default | 1
 */
#define kszAutopAttemptDelay "AutopAttemptDelay"

/**
 * @brief 失败重试次数
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.attempt_before_failed
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | nAttemptTimes
 *      default | 3
 */
#define kszAutopAttemptTimes "AutopAttemptTimes"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.com_aes
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | AES
 *      default | ""
 *      encrypt | True
 */
#define kszAutopCodeAES "AutopCodeAES%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.code
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | code
 *      default | ""
 */
#define kszAutopCodeCode "AutopCodeCode%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.mac_aes
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | MAC_AES
 *      default | ""
 *      encrypt | True
 */
#define kszAutopCodeMACAES "AutopCodeMACAES%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.name
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | Name
 *      default | ""
 */
#define kszAutopCodeName "AutopCodeName%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.password
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | password
 *      default | ""
 *      encrypt | True
 */
#define kszAutopCodePassword "AutopCodePassword%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.url
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | server_address
 *      default | ""
 */
#define kszAutopCodeServer "AutopCodeServer%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | autoprovision.%%d.user
 *         path | /config/system/autop_code.cfg
 *      section | autoprovision%%d
 *          key | user
 *      default | ""
 */
#define kszAutopCodeUser "AutopCodeUser%d"

/**
 * @brief AutopInTalking
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_uri_force_autop
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutopInTalking
 *      default | 0
 */
#define kszAutopInTalking "AutopInTalking"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.update_now.enable
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | AutopNow
 *      default | 0
 */
#define kszAutopNow "AutopNow"

/**
 * @brief autop闪灯
 */
#define kszPowerLightControlAutop "AutopPowerLightFlash"

/**
 * @brief AutopReTryCount
 */
#define kszAutopReTryCount "AutopReTryCount"

/**
 * @brief 下载库内部延迟
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.attempt_expired_time
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | TimeOut
 *      default | 5
 *          reg | {required:true,range:[1,300,5],maxlength:3}
 */
#define kszAutopRetryDelay "AutopRetryDelay"

/**
 * @brief AutopSeparateWebConfig
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.autopseparatewebconfig
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | AutopSeparateWebConfig
 *      default | 0
 */
#define kszAutopSeparateWebConfig "AutopSeparateWebConfig"

/**
 * @brief 成功执行autop的URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.autoprovision.strsuccessurl
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strSuccessURL
 *      default | ""
 */
#define kszAutopSucessUrl "AutopSucessURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.updatetimeout
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | UpdateTimeout
 *      default | 180000
 */
#define kszAutopUpdateTimeout "AutopUpdateTimeout"

/**
 * @brief AutoprovisionPromptEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.prompt.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | AutoprovisionPromptEnable
 *      default | 0
 */
#define kszAutoprovisionPromptEnable "AutoprovisionPromptEnable"

/**
 * @brief Broadsoft显示开关. 如果为关则不显示Broadsoft Calllog和Broadsoft Directory菜单.
 */
#define kszBDDirectoryShowSwitch "BDDirectoryShowSwitch"

/**
 * @brief 控制 BLF/BLFList 状态变化时，是否点亮背光灯
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_active_backlight.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BLFActiveBackLight
 *      default | 0
 *  t49-default | 1
 */
#define kszBLFActiveBackLight "BLFActiveBackLight"

/**
 * @brief blf callout mode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf.intercom_mode.enable
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | BLFIntercomMode
 *      default | 0
 */
#define kszBLFIntercomMode "BLFIntercomMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_led_mode
 *         path | /config/user/user.ini
 *      section | BLFLed
 *          key | BLFLedMode
 *      default | 0
 */
#define kszBLFLedMode "BLFLedMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_and_callpark_idle_led_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BLFLedOffIdle
 *      default | 0
 */
#define kszBLFLedOffInIdle "BLFLedOffInIdle"

/**
 * @brief 自定义配置BLFList
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.blf_list.custom_set.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BLFListCustomSet
 *      default | 0
 */
#define kszBLFListCustomSet "BLFListCustomSet"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_list_version
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IsBLFListVersion
 *      default | 0
 */
#define kszBLFListVersionActive "BLFListVersionActive"

/**
 * @brief BLFNotifyViaTCP
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.listen_mode
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | ListenAdaptive
 *      default | 0
 */
#define kszBLFNotifyViaTCP "BLFNotifyViaTCP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_list_retrieve_call_parked_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BLFRetrieveCallPark
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszAccountBLFListRetrieveCode "BLFRetrieveCallPark"
#define kszBLFRetrieveCallPark "BLFRetrieveCallPark"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_status_specific
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BLFStatusSpecific
 *      default | 0
 */
#define kszBLFStatusSpecifice "BLFStatusSpecifice"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_led_mode_legacy
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BLfNotifyType
 *      default | 0
 */
#define kszBLfNotifyType "BLfNotifyType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.broadsoft.baddcountrycode
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bAddCountryCode
 *      default | 0
 */
#define kszBSCallLogAddCtyCode "BSCallLogAddCountryCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.enable
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | bBroadsoftActive
 *      default | 0
 */
#define kszBroadsoftActive "BSFTActive"

/**
 * @brief Visual Voicemail Show Video
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.voice_mail.visual.display_videomail.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bBSFTShowVideoMailEnable
 *      default | 0
 */
#define kszBroadsoftShowVideoMail "BSFTShowVideoMail"

/**
 * @brief BSFTVisualVoiceMail
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.voice_mail.visual.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bBSFTVisualVoiceMailEnable
 *      default | 0
 */
#define kszBroadsoftVisualVoiceMail "BSFTVisualVoiceMail"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.calllog_and_dir
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | BWCallLogandDir
 *      default | 0
 */
#define kszBWCallLogandDir "BWCallLogandDir"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phonesetting.backlight
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | BackLight
 *      default | 2
 */
#define kszBacklightLevel "BacklightLevel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.backlight_time
 *         path | /config/user/user.ini
 *      section | Backlight
 *          key | nBacklightTime
 *      default | 0
 */
#define kszBackLightTime "BacklightTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.barge_in_via_username.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BargeInUserName
 *      default | 0
 */
#define kszBargeInUserName "BargeInUserName"

/**
 * @brief 当base 开启nemo模式后，需要间隔一段时间再使用nemo模式加载coma
 */
#define kszBaseNemoLoadDelayTime "BaseNemoLoadDelayTime"

/**
 * @brief Dect Base Setting
 */
#define kszBaseNemoSwitch "BaseNemoSwitch"

/**
 * @brief Base触发的强制更新
 */
#define kszBaseTrigger "BaseTrigger"

/**
 * @brief 注释
 */
#define kszBasicIpaddressMode "BasicIpaddressMode"

/**
 * @brief 注释
 */
#define kszBasicIpv4config "BasicIpv4config"

/**
 * @brief 注释
 */
#define kszBasicIpv6config "BasicIpv6config"

/**
 * @brief 注释
 */
#define kszBasicSwitchAccount "BasicSwitchAccount"

/**
 * @brief 注释
 */
#define kszBasicWanGroup "BasicWanGroup"

/**
 * @brief 注释
 */
#define kszBasicWanStaticip "BasicWanStaticip"

/**
 * @brief 注释
 */
#define kszBasicWandhcp "BasicWandhcp"

/**
 * @brief 注释
 */
#define kszBasicWanppoe "BasicWanppoe"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.change_183_to_180
 *         path | /config/user/user.ini
 *      section | Nology
 *          key | BeChange183to180
 *      default | 0
 */
#define kszBeChangeSec183to180 "BeChangeSec183to180"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.behave_bw_dir
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | BeHaveBWDir
 *      default | 0
 */
#define kszBeHaveBWDir "BeHaveBWDir"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.behave_calllog
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | BeHaveCallLog
 *      default | 0
 */
#define kszBeHaveCallLog "BeHaveCallLog"

/**
 * @brief BlfCallOutByNumber
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_call_out_by_number.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BlfCallOutByNumber
 *      default | 0
 */
#define kszBlfCallOutByNumber "BlfCallOutByNumber"

/**
 * @brief BLF Ring Type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf.ring_type
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | BLFRingType
 *      default | Resource:Splash.wav
 */
#define kszBlfRingType "BlfRingType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.blfvalue_format
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BLFValue_Format
 *      default | ""
 */
#define kszBlfValueFormate "BlfValueFormate"

/**
 * @brief 屏蔽长按Dsskey进入配置界面功能
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.blockdsskeyextendedpress
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BlockDsskeyExtendedPress
 *      default | 0
 */
#define kszBlockDsskeyExtendedPress "BlockDsskeyExtendedPress"

/**
 * @brief 屏蔽dsskey账号功能键
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.block_new_call_via_linekey.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BlockLine
 *      default | 0
 */
#define kszBlockLine "BlockLine"

/**
 * @brief BlockLinekeyInMenu
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.block_linekey_in_menu.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BlockLinekeyInMenu
 *      default | 0
 */
#define kszBlockLinekeyInMenu "BlockLinekeyInMenu"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.1
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 1
 *      default | ""
 */
#define kszBlockOut1 "BlockOut1"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.10
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 10
 *      default | ""
 */
#define kszBlockOut10 "BlockOut10"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.2
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 2
 *      default | ""
 */
#define kszBlockOut2 "BlockOut2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.3
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 3
 *      default | ""
 */
#define kszBlockOut3 "BlockOut3"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.4
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 4
 *      default | ""
 */
#define kszBlockOut4 "BlockOut4"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.5
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 5
 *      default | ""
 */
#define kszBlockOut5 "BlockOut5"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.6
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 6
 *      default | ""
 */
#define kszBlockOut6 "BlockOut6"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.7
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 7
 *      default | ""
 */
#define kszBlockOut7 "BlockOut7"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.8
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 8
 *      default | ""
 */
#define kszBlockOut8 "BlockOut8"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockout.9
 *         path | /config/user/user.ini
 *      section | BlockOut
 *          key | 9
 *      default | ""
 */
#define kszBlockOut9 "BlockOut9"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.1
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 1
 *      default | ""
 */
#define kszBlockOutLineID1 "BlockOutLineID1"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.10
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 10
 *      default | ""
 */
#define kszBlockOutLineID10 "BlockOutLineID10"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.2
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 2
 *      default | ""
 */
#define kszBlockOutLineID2 "BlockOutLineID2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.3
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 3
 *      default | ""
 */
#define kszBlockOutLineID3 "BlockOutLineID3"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.4
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 4
 *      default | ""
 */
#define kszBlockOutLineID4 "BlockOutLineID4"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.5
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 5
 *      default | ""
 */
#define kszBlockOutLineID5 "BlockOutLineID5"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.6
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 6
 *      default | ""
 */
#define kszBlockOutLineID6 "BlockOutLineID6"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.7
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 7
 *      default | ""
 */
#define kszBlockOutLineID7 "BlockOutLineID7"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.8
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 8
 *      default | ""
 */
#define kszBlockOutLineID8 "BlockOutLineID8"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.blockoutlineid.9
 *         path | /config/user/user.ini
 *      section | BlockOutLineID
 *          key | 9
 *      default | ""
 */
#define kszBlockOutLineID9 "BlockOutLineID9"

/**
 * @brief 蓝牙适配器显示名称
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.bluetooth_adapter_name
 *         path | /config/user/user.ini
 *      section | Bluetooth
 *          key | strAdapterName
 *      default | ""
 */
#define kszBluetoothAdapterName "BluetoothAdapterName"

/**
 * @brief 蓝牙适配器mac
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.bluetooth_adapter_mac
 *         path | /config/user/user.ini
 *      section | Bluetooth
 *          key | BluetoothAdapterMac
 *      default | ""
 */
#define kszBluetoothAdapterMac "BluetoothAdapterMac"

/**
 * @brief 是否同步话机DND配置
 */
#define kszMobileDNDEnable "BluetoothDNDSyncEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.bbluetoothenable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bBluetoothEnable
 *      default | ""
 */
#define kszBluetoothEnable "BluetoothEnable"

/**
 * @brief 蓝牙联系人最大支持数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.bluetooth.maxcontactnum
 *         path | /config/user/user.ini
 *      section | Bluetooth
 *          key | MaxContactNum
 *      default | 1500
 */
#define kszBluetoothMaxContactNum "BluetoothMaxContactNum"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.bluetooth_enable
 *         path | /config/user/user.ini
 *      section | Bluetooth
 *          key | bEnable
 *      default | 1
 */
#define kszBluetoothSwitch "BluetoothSwitch"

/**
 * @brief 蓝牙联系人解析测试文件路径
 */
#define kszBluetoothVCardFileDir "BluetoothVCardFileDir"

/**
 * @brief 蓝牙联系人解析测试开关
 */
#define kszBluetoothVCardTestEnable "BluetoothVCardTest"

/**
 * @brief BroadTouchHost
 */
#define kszBsftServer "BroadTouchHost"

/**
 * @brief BroadTouchPassword
 */
#define kszBsftPassword "BroadTouchPassword"

/**
 * @brief BroadTouchPort
 */
#define kszBsftPort "BroadTouchPort"

/**
 * @brief BroadTouchServerType
 */
#define kszBsftServerType "BroadTouchServerType"

/**
 * @brief BroadTouchUserName
 */
#define kszBsftUser "BroadTouchUserName"

/**
 * @brief BroadTouchVersion
 */
#define kszBsftVersion "BroadTouchVersion"

/**
 * @brief 注释
 */
#define kszBroadsoft_Active "Broadsoft-Active"

/**
 * @brief 注释
 */
#define kszBroadsoftDirectotries "BroadsoftDirectotries"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.feature_key_sync
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bBroadsoftFeatureKeySync
 *      default | 0
 */
#define kszBroadSoftFeatureSync "BroadsoftFeatureKeySync"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.bsyncallaccount
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bSyncAllAccount
 *      default | 1
 */
#define kszBroadSoftSyncToAllAccount "BroadsoftSyncToAllAccout"

/**
 * @brief 注释
 */
#define kszBroadsoftXSI "BroadsoftXSI"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.custom
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | EnableCustom
 *      default | 0
 */
#define kszBsftCustom "BsftCustom"
#define kszNetworkDirectoryCustomEnabled "BsftCustom"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.list.customlistall
 *       t49-m7 | tmp.bw_phonebook.custom_all_list
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | CustomListAll
 *      default | ""
 */
#define kszBsftCustomListAll "BsftCustomListAll"
#define kszNetworkDirectoryCustomListAll "BsftCustomListAll"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.info.strfailreason
 *       t49-m7 | tmp.bw_phonebook.custom_fail_reason
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | strFailReason
 *      default | ""
 */
#define kszBsftCustomListFailReason "BsftCustomListFailReason"
#define kszNetworkDirectoryCustomFailReason "BsftCustomListFailReason"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.info.eliststauts
 *       t49-m7 | tmp.bw_phonebook.custom_list_status
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | eListStauts
 *      default | 0
 */
#define kszBsftCustomListStatus "BsftCustomListStatus"
#define kszNetworkDirectoryCustomListStatus "BsftCustomListStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.list.customlistuse
 *       t49-m7 | tmp.bw_phonebook.custom_list
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | CustomListUse
 *      default | ""
 */
#define kszBsftCustomListUse "BsftCustomListUse"
#define kszNetworkDirectoryCustomListUse "BsftCustomListUse"

/**
 * @brief Broadsoft删除历史记录开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.call_log.delete.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | BsftLogDeleteEnable
 *      default | 0
 */
#define kszBsftLogDeleteEnable "BsftLogDeleteEnable"

/**
 * @brief BsftDirNameShowMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.directory.alphabetized_by_lastname.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | BsftDirNameShowMode
 *      default | 0
 */
#define kszBsftDirNameShowMode "BsftDirNameShowMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.enterprise_enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | EnableEnterprise
 *      default | 0
 */
#define kszBsftEnterprise "BsftEnterprise"
#define kszNetworkDirectoryEnterpriseEnabled "BsftEnterprise"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.enterprise_common_enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | EnableEnterpriseCommon
 *      default | 0
 */
#define kszBsftEnterpriseCommon "BsftEnterpriseCommon"
#define kszNetworkDirectoryEnterpriseCommon "BsftEnterpriseCommon"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.group_enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | EnableGroup
 *      default | 1
 */
#define kszBsftGroup "BsftGroup"
#define kszNetworkDirectoryGroupEnabled "BsftGroup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.group_common_enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | EnableGroupCommon
 *      default | 0
 */
#define kszBsftGroupCommon "BsftGroupCommon"
#define kszNetworkDirectoryGroupCommon "BsftGroupCommon"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.personal_enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | EnablePersonal
 *      default | 1
 */
#define kszBsftPersonal "BsftPersonal"
#define kszNetworkDirectoryPersonalEnabled "BsftPersonal"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.update_time_interval
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | UpdateInterVal
 *      default | 60
 */
#define kszBstDirUpdateInterVal "BstDirUpdateInterVal"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.busy.off_code
 *         path | /config/user/user.ini
 *      section | BusyForward
 *          key | strOffCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszForwardBusyOffCode "BusyForwardOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.busy.on_code
 *         path | /config/user/user.ini
 *      section | BusyForward
 *          key | strOnCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszForwardBusyOnCode "BusyForwardOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.busy.enable
 *         path | /config/user/user.ini
 *      section | BusyForward
 *          key | bEnable
 *      default | 0
 */
#define kszForwardBusySwitch "BusyForwardSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.busy.target
 *         path | /config/user/user.ini
 *      section | BusyForward
 *          key | strTarget
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszForwardBusyTarget "BusyForwardTarget"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.busy_tone_delay
 *         path | /config/user/user.ini
 *      section | BusyTone
 *          key | nBusyToneDelay
 *      default | 3
 */
#define kszBusytoneDelay "BusytoneDelay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.cdp.enable
 *         path | /config/system/system.ini
 *      section | CDP
 *          key | bEnableCDP
 *      default | 1
 */
#define kszCDPEnable "CDPEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.cdp.packet_interval
 *         path | /config/system/system.ini
 *      section | CDP
 *          key | nPacketInterval
 *      default | 60
 *          reg | {required:true,range:[1,3600,60],maxlength:4}
 */
#define kszCDPInterveral "CDPInterveral"

/**
 * @brief CMDAnswerAsIntercom
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.event_talk_as_intercom.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CMDAnswerAsIntercom
 *      default | 0
 */
#define kszCMDAnswerAsIntercom "CMDAnswerAsIntercom"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.csta_control.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEnableCstaControl
 *      default | 1
 */
#define kszCSTAControl "CSTAControl"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_completion.call_back.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bCallCompletionCallBackEnable
 *      default | 0
 */
#define kszCallCompletionCallBackSwitch "CallCompletionCallBackSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_completion_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bCallCompletionEnable
 *      default | 0
 */
#define kszCallCompletionSwitch "CallCompletionSwitch"

/**
 * @brief CallDeclineEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_decline.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallDeclineEnable
 *      default | 0
 */
#define kszCallDeclineEnable "CallDeclineEnable"

/**
 * @brief 注释
 */
#define kszCallDialedListContent "CallDialedListContent"

/**
 * @brief 注释
 */
#define kszCallDialedListTitle "CallDialedListTitle"

/**
 * @brief 注释
 */
#define kszCallForwardedListContent "CallForwardedListContent"

/**
 * @brief 注释
 */
#define kszCallForwardedListTitle "CallForwardedListTitle"

/**
 * @brief CallHistoryByOffHook
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_out_history_by_off_hook.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallHistoryByOffHook
 *      default | 1
 *  t49-default | 0
 */
#define kszCallHistoryByOffHook "CallHistoryByOffHook"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.callinanonymous
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallInAnonymous
 *      default | ""
 */
#define kszCallInAnonymous "CallInAnonymous"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_invite_format
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallInviteFormatTel
 *      default | 0
 */
#define kszCallInviteFormatTelMode "CallInviteFormatTelMode"

/**
 * @brief CallLogEncryptionEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.encryption.call_log
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bCallLogEncryptionEnable
 *      default | 0
 */
#define kszCallLogEncryptionEnable "CallLogEncryptionEnable"

/**
 * @brief 注释
 */
#define kszCallMissedListContent "CallMissedListContent"

/**
 * @brief 注释
 */
#define kszCallMissedListTitle "CallMissedListTitle"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_num_filter
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallNumFilter
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszCallNumFilter "CallNumFilter"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.park_code
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | CallParkCode
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszCallParkCode "CallParkCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.park_mode
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | ParkMode
 *      default | 0
 */
#define kszCallParkMode "CallParkMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.park_ring
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | eParkRing
 *      default | 0
 */
#define kszCallParkRingNotifyType "CallParkRingNotifyType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | bEnabledCallPark
 *      default | 0
 */
#define kszCallParkSwitch "CallParkSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.park_visual_notify_enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | bEnabledParkVisualNotify
 *      default | 0
 */
#define kszCallParkVisualNotifySwitch "CallParkVisualNotifySwitch"

/**
 * @brief call park key收到服务器的消息变更为忙状态(通话中)时响铃开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.callpark.blf_audio.enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | bEnabledParkBlfAudio
 *      default | 0
 */
#define kszCallParkBlfAudioSwitch "CallParkBlfAudioSwitch"

/**
 * @brief 注释
 */
#define kszCallReceivedListContent "CallReceivedListContent"

/**
 * @brief 注释
 */
#define kszCallReceivedListTitle "CallReceivedListTitle"

/**
 * @brief CallRecordCode
 */
#define kszCallRecordCode "CallRecordCode"

/**
 * @brief 0:Xsi 1:sip
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.call_recording.mode
 *         path | /config/user/user.ini
 *      section | CallRecord
 *          key | RecordMode
 *      default | 0
 */
#define kszCallRecordMode "CallRecordMode"

/**
 * @brief Call control下的Call Record改变录音模式功能是否开启
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_call_recording.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallRecordModeOn
 *      default | 1
 */
#define kszCallRecordModeEnable "CallRecordModeEnable"

/**
 * @brief CallRecordSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.call_recording.enable
 *         path | /config/user/user.ini
 *      section | CallRecord
 *          key | RecordSwitch
 *      default | ""
 */
#define kszCallRecordSwitch "CallRecordSwitch"

/**
 * @brief CallReturn功能使用：来电对应的账号ID
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/user.ini
 *      section | CallReturn
 *          key | AccountID
 *      default | -1
 */
#define kszCallReturnAccount "CallReturnAccount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_return_number
 *         path | /config/user/user.ini
 *      section | CallReturn
 *          key | strNumber
 *      default | ""
 */
#define kszCallReturnNumber "CallReturnNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_return_server
 *         path | /config/user/user.ini
 *      section | CallReturn
 *          key | strServer
 *      default | ""
 */
#define kszCallReturnServer "CallReturnServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_return_user_name
 *         path | /config/user/user.ini
 *      section | CallReturn
 *          key | strUser
 *      default | ""
 */
#define kszCallReturnUser "CallReturnUser"

/**
 * @brief call text优先显示sip发送过来的displayname
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.calltext_showsipdisplayfirst
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallTextShowSipDisplayFirst
 *      default | 0
 */
#define kszCallTextShowSipDisplayFirst "CallTextShowSipDisplayFirst"

/**
 * @brief CallWaitReply182
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | call_waiting.reply_182.enable
 *         path | /config/user/user.ini
 *      section | CallWaiting
 *          key | bCallWaitReply182
 *      default | 0
 */
#define kszCallWaitReply182 "CallWaitReply182"

/**
 * @brief broadsoft call waiting
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | call_waiting.mode
 *         path | /config/user/user.ini
 *      section | CallWaiting
 *          key | CallWaitingMode
 *      default | 0
 */
#define kszCallWaitingMode "CallWaitingMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | call_waiting.off_code
 *         path | /config/user/user.ini
 *      section | Features
 *          key | Call_WaitingOffCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszCallWaitingOffCode "CallWaitingOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | call_waiting.on_code
 *         path | /config/user/user.ini
 *      section | Features
 *          key | Call_WaitingOnCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszCallWaitingOnCode "CallWaitingOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | call_waiting.enable
 *         path | /config/user/user.ini
 *      section | CallWaiting
 *          key | bEnable
 *      default | 1
 */
#define kszCallWaitingSwitch "CallWaitingSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | call_waiting.tone
 *         path | /config/user/user.ini
 *      section | CallWaiting
 *          key | bToneEnable
 *      default | 1
 */
#define kszCallWaitingToneSwitch "CallWaitingToneSwitch"

/**
 * @brief CallforwardSelective开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.bforwardselective
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bForwardSelective
 *      default | 0
 */
#define kszCallforwardSelectiveSwitch "CallforwardSelectiveSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_log_show_num
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallListShowNum
 *      default | 0
 */
#define kszCalllogShowNumEnable "CalllogShowNumEnable"

/**
 * @brief CameraBlueGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.blue_gain
 *      default | 70
 *          reg | {required:true,range:[0,100,70],maxlength:3}
 */
#define kszCameraBlueGain "CameraBlueGain"

/**
 * @brief CameraBrightness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.brightness
 *      default | 50
 *          reg | {required:true,range:[0,100,50],maxlength:3}
 */
#define kszCameraBrightness "CameraBrightness"

/**
 * @brief CameraContrast
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.contrast
 *      default | 50
 *          reg | {required:true,range:[0,100,50],maxlength:3}
 */
#define kszCameraContrast "CameraContrast"

/**
 * @brief CameraExposureCompensation
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.exposure_compensation.enable
 *      default | 1
 */
#define kszCameraExposureCompensation "CameraExposureCompensation"

/**
 * @brief CameraExposureFlicker
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.flicker
 *      default | 50
 */
#define kszCameraExposureFlicker "CameraExposureFlicker"

/**
 * @brief CameraLabel%%d
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | door_phone.camera_url_label.%%d
 *         path | /config/user/user.ini
 *      section | DoorPhone
 *          key | CameraLabel%%d
 *      default | ""
 */
#define kszCameraLabel "CameraLabel%d"

/**
 * @brief CameraNR2DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.nr2d_level
 *      default | 32
 */
#define kszCameraNR2DLevel "CameraNR2DLevel"

/**
 * @brief CameraNR3DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.nr3d_level
 *      default | 0
 */
#define kszCameraNR3DLevel "CameraNR3DLevel"

/**
 * @brief CameraRedGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.red_gain
 *      default | 80
 *          reg | {required:true,range:[0,100,80],maxlength:3}
 */
#define kszCameraRedGain "CameraRedGain"

/**
 * @brief CameraSaturation
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.saturation
 *      default | 50
 *          reg | {required:true,range:[0,100,50],maxlength:3}
 */
#define kszCameraSaturation "CameraSaturation"

/**
 * @brief 摄像头相关配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.scene_mode
 *      default | 1
 */
#define kszCameraSceneMode "CameraSceneMode"

/**
 * @brief CameraSharpness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.sharpness
 *      default | 30
 *          reg | {required:true,range:[0,100,30],maxlength:3}
 */
#define kszCameraSharpness "CameraSharpness"

/**
 * @brief CameraStatusIcon
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.status_bar_icon.enable
 *      default | 1
 */
#define kszCameraStatusIcon "CameraStatusIcon"

/**
 * @brief CameraURL%%d
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | door_phone.camera_url.%%d
 *         path | /config/user/user.ini
 *      section | DoorPhone
 *          key | CameraURL%%d
 *      default | ""
 */
#define kszCameraURL "CameraURL%d"

/**
 * @brief CameraURLRefreshInterval
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | door_phone.picture_refresh_interval
 *         path | /config/user/user.ini
 *      section | DoorPhone
 *          key | CameraURLRefreshInterval
 *      default | 0.5
 *          reg | {signed:true,float:true,range:[0,300],maxlength:5}
 */
#define kszCameraURLRefreshInterval "CameraURLRefreshInterval"

/**
 * @brief 当前手动设置值
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.white_balance
 *      default | 5
 */
#define kszCameraWhiteBalanceMode "CameraWhiteBalanceMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.cancelcallback
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CancelCallback
 *      default | ""
 */
#define kszCancelCallback "CancelCallback"

/**
 * @brief CfgVersion
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.custom_version_info
 *       t49-m7 | custom.configuration.version
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CfgVersion
 *      default | ""
 */
#define kszCfgVersion "CfgVersion"

/**
 * @brief CheckSyncMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.check_sync.mode
 *         path | /config/system/system.ini
 *      section | CheckSync
 *          key | iMode
 *      default | 0
 */
#define kszCheckSyncMode "CheckSyncMode"

/**
 * @brief 屏保上显示的时钟多长时间移动一次
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.clock_move_interval
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | ClockMoveInterval
 *      default | 600
 */
#define kszClockMoveInterval "ClockMoveInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_led_on
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ClosePowerLight
 *      default | 0
 */
#define kszClosePowerLight "ClosePowerLight"

/**
 * @brief CloudAccountType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.custom.accounttype
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | CloudAccountType
 *      default | 0
 */
#define kszAccountType "CloudAccountType"

/**
 * @brief 云功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.custom.cloudenable
 *         path | /config/system/system.ini
 *      section | VCAccount
 *          key | bVCAccountEnable
 *      default | 1
 *  t49-default | 0
 */
#define kszCloudActive "CloudEnable"
#define kszCloudEnable "CloudEnable"

/**
 * @brief 免注册云平台：优先用H323呼出
 */
#define kszCloudH323CallOutPrior "CloudH323CallOutPrior"

/**
 * @brief CloudHttpServer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | cloud.http_server.address
 *         path | /config/user/user.ini
 *      section | Cloud
 *          key | strHttpServerUrl
 *      default | http://yealinkvc.com
 */
#define kszCloudHttpServer "CloudHttpServer"

/**
 * @brief CloudSipServer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | cloud.sip_server.address
 *         path | /config/user/user.ini
 *      section | Cloud
 *          key | strSipServerUrl
 *      default | yealinkvc.com
 */
#define kszCloudSipServer "CloudSipServer"

/**
 * @brief CloudYealinkEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | cloud.yealink.enable
 *         path | /config/user/user.ini
 *      section | Cloud
 *          key | YealinkCloudEnable
 *      default | 1
 */
#define kszCloudYealinkEnable "CloudYealinkEnable"

/**
 * @brief 注释
 */
#define kszCoantactsSRemoteNameEnable "CoantactsSRemoteNameEnable"

/**
 * @brief 注释
 */
#define kszCoantactsSRemoteNameFlashTime "CoantactsSRemoteNameFlashTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.show_code403
 *         path | /config/user/user.ini
 *      section | ShowCodeInfo
 *          key | Code403
 *      default | ""
 */
#define kszCode403Info "Code403Info"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.common_power_led_enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_Common
 *      default | 0
 */
#define kszCommonPowerLightOn "CommonPowerLightOn"
#define kszPowerLightControlCommon "CommonPowerLightOn"

/**
 * @brief ConfServCode
 */
#define kszConfServCode "ConfServCode"

/**
 * @brief 按下Conf时候是否使用新的Linekey呼出
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_appearance.conference_via_new_linekey
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bConfNewLinekey
 *      default | 1
 */
#define kszConfUseNewLinekey "ConfUseNewLinekey"

/**
 * @brief ConfWithOnePress
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.local_conf.combine_with_one_press.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ConfWithOnePress
 *      default | 0
 */
#define kszConfWithOnePress "ConfWithOnePress"

/**
 * @brief ConfWithPreviousCall
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.conference.with_previous_call.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ConfWithPreviousCall
 *      default | 0
 */
#define kszConfWithPreviousCall "ConfWithPreviousCall"

/**
 * @brief 是否使用新的key来创建Conference
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_appearance.conference_via_new_linekey
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bConfNewLinekey
 *      default | 1
 */
#define kszConferenceByNewLine "ConferenceByNewLine"

/**
 * @brief conference按键发送自定义的DTMF
 */
#define kszConferenceDTMF "ConferenceDTMF"

/**
 * @brief ConfigEncryptionEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.autoprovision.bconfigencryptionenable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bConfigEncryptionEnable
 *      default | 0
 */
#define kszConfigEncryptionEnable "ConfigEncryptionEnable"

/**
 * @brief 注释
 */
#define kszConfigSyncDownLoadURL "ConfigSyncDownLoadURL"

/**
 * @brief 注释
 */
#define kszConfigSyncEnable "ConfigSyncEnable"

/**
 * @brief 注释
 */
#define kszConfigSyncPassword "ConfigSyncPassword"

/**
 * @brief 注释
 */
#define kszConfigSyncUpLoadURL "ConfigSyncUpLoadURL"

/**
 * @brief 注释
 */
#define kszConfigSyncUsername "ConfigSyncUsername"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.beipconflicted.conflictedip
 *         path | /config/system/system.ini
 *      section | BeIPConflicted
 *          key | ConflictedIP
 *      default | ""
 */
#define kszConflictedIP "ConflictedIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.beipconflicted.conflictedmac
 *         path | /config/system/system.ini
 *      section | BeIPConflicted
 *          key | ConflictedMAC
 *      default | 00:00:00:00:00:00
 */
#define kszConflictedMAC "ConflictedMAC"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.edit_default_input_method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ContactDefaultIME
 *      default | ""
 */
#define kszContactDefaultIME "ContactDefaultIME"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.update_contact_display_priority
 *         path | /config/user/user.ini
 *      section | Wildix
 *          key | ContactDisplayFirst
 *      default | 0
 */
#define kszContactDisplayFirst "ContactDisplayFirst"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.contact_menu_hidden_items
 *         path | /config/user/user.ini
 *      section | Contact
 *          key | ContactMenuHiddenItems
 *      default | ""
 */
#define kszContactMenuHiddenItems "ContactMenuHiddenItems"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.search_default_input_method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ContactSearchDefaultIME
 *      default | ""
 */
#define kszContactSearchDefaultIME "ContactSearchDefaultIME"

/**
 * @brief 联系人更新标记
 */
#define kszContactUpdateMark "ContactUpdateMark"

/**
 * @brief 注释
 */
#define kszContactsLDAPDisplayName "ContactsLDAPDisplayName"

/**
 * @brief 注释
 */
#define kszContactsLDAPNameFilter "ContactsLDAPNameFilter"

/**
 * @brief 注释
 */
#define kszContactsLDAPNumberFilter "ContactsLDAPNumberFilter"

/**
 * @brief 注释
 */
#define kszContactsLDAPSearchDelay "ContactsLDAPSearchDelay"

/**
 * @brief 注释
 */
#define kszContactsLDAPSwitch "ContactsLDAPSwitch"

/**
 * @brief 注释
 */
#define kszWebContactsLDAPTLSMode "ContactsLDAPTLSMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.contrast
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | Contrast
 *      default | 6
 */
#define kszContrastLevel "ContrastLevel"

/**
 * @brief CoolBlueGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.blue_gain
 *      default | 85
 */
#define kszCameraCoolBlueGain "CoolBlueGain"

/**
 * @brief CoolBrightness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.brightness
 *      default | 70
 */
#define kszCameraCoolBrightness "CoolBrightness"

/**
 * @brief CoolContrast
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.contrast
 *      default | 50
 */
#define kszCameraCoolContrast "CoolContrast"

/**
 * @brief CoolNR2DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.nr2d_level
 *      default | 32
 */
#define kszCameraCoolNR2DLevel "CoolNR2DLevel"

/**
 * @brief CoolNR3DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.nr3d_level
 *      default | 0
 */
#define kszCameraCoolNR3DLevel "CoolNR3DLevel"

/**
 * @brief CoolRedGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.red_gain
 *      default | 70
 */
#define kszCameraCoolRedGain "CoolRedGain"

/**
 * @brief CoolSaturation
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.saturation
 *      default | 50
 */
#define kszCameraCoolSaturation "CoolSaturation"

/**
 * @brief CoolSharpness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.sharpness
 *      default | 30
 */
#define kszCameraCoolSharpness "CoolSharpness"

/**
 * @brief 柔光模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.cool.white_balance
 *      default | 5
 */
#define kszCameraCoolWhiteBalanceMode "CoolWhiteBalanceMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.disable_account_without_username.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CorrectAccount
 *      default | 1
 */
#define kszCorrectAccount "CorrectAccount"

/**
 * @brief 国家设置
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/user.ini
 *      section | Features
 *          key | Country
 *      default | 156
 */
#define kszCurrentCountry "Country"

/**
 * @brief 历史记录是否支持累计方式
 */
#define kszCumulativeCallLog "CumulativeCallLog"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.authority.currentauthority
 *         path | /config/system/system.ini
 *      section | Authority
 *          key | CurrentAuthority
 *      default | admin
 */
#define kszWebCurrentAuthority "CurrentAuthority"

/**
 * @brief CurrentExpWallpaper
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.backgrounds
 *      default | Resource:Default-exp50.jpg
 */
#define kszCurrentExpWallPaper "CurrentExpWallpaper"
#define kszCurrentExpWallpaper "CurrentExpWallpaper"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.ring_type
 *         path | /config/user/user.ini
 *      section | Ring
 *          key | strRingFile
 *      default | Resource:Ring1.wav
 */
#define kszCurrentRing "CurrentRing"
#define kszRingtype "CurrentRing"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.theme.strcurrenttheme
 *         path | /config/user/user.ini
 *      section | Theme
 *          key | strCurrentTheme
 *      default | ""
 */
#define kszCurrentTheme "CurrentTheme"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.time_zone
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | strTimeName
 *      default | ""
 */
#define kszCurrentTimeZoneTime "CurrentTimeZoneTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.time_zone_name
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | strZoneName
 *      default | ""
 */
#define kszCurrentTimeZoneZone "CurrentTimeZoneZone"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.backgrounds
 *         path | /config/system/system.ini
 *      section | Wallpaper
 *          key | strCurrentWallpaper
 *      default | ""
 */
#define kszCurrentWallPaper "CurrentWallpaper"

/**
 * @brief CustomAssertedId
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.custom_asserted_id
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | CustomAssertedId
 *      default | ""
 */
#define kszCustomAssertedId "CustomAssertedId"

/**
 * @brief CustomDateFmt
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | lcl.datetime.date.format
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | CustomDateFmt
 *      default | ""
 */
#define kszCustomDateFmt "CustomDateFmt"

/**
 * @brief 客户定制softkey动态添加
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.custom_softkey_dynamic.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CustomDynamicSoftkey
 *      default | 1
 */
#define kszCustomDynamicSoftkey "CustomDynamicSoftkey"

/**
 * @brief CustomSN
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.custom_sn
 *         path | /config/ADVnumber.cfg
 *      section | sn
 *          key | sn
 *      default | ""
 *      encrypt | True
 */
#define kszCustomSN "CustomSN"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.custom_softkey_enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | IsDefineKey
 *      default | 0
 */
#define kszCustomSoftkeySwitch "CustomSoftkeySwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | tmp.dhcp.*
 *         path | /tmp/yealink_dhcp_options
 *      section | DHCP
 *          key | 120
 *      default | ""
 */
#define kszDHCP120 "DHCP120"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.emergency_authorized_number
 *         path | /config/user/user.ini
 *      section | DNDEmergency
 *          key | DND_Authorized_Numbers
 *      default | ""
 *          reg | {multiple:{digits:true,splits:','},maxlength:511}
 */
#define kszDNDEmergencyNumberList "DNDEmergencyNumberList"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.emergency_enable
 *         path | /config/user/user.ini
 *      section | DNDEmergency
 *          key | Enable
 *      default | 0
 */
#define kszDNDEmergencySwitch "DNDEmergencySwitch"

/**
 * @brief 本地DND是否同步服务器
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.feature_key_sync.enable
 *         path | /config/user/user.ini
 *      section | DND
 *          key | bDNDFeatureKeySync
 *      default | ""
 */
#define kszDNDFeatureKeySync "DNDFeatureKeySync"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd_refuse_code
 *         path | /config/user/user.ini
 *      section | RefuseCode
 *          key | nDNDRefuseCode
 *      default | 480
 */
#define kszDNDRefuseCode "DNDRefuseCode"

/**
 * @brief DNDShareLineSync
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.share_line_sync.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bShareLineSync
 *      default | 1
 */
#define kszDNDShareLineSync "DNDShareLineSync"
#define kszDNDSync "DNDShareLineSync"

/**
 * @brief DND同步开启的情况下是否在本地处理DND（拒接）
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.feature_key_sync.local_processing.enable
 *         path | /config/user/user.ini
 *      section | DND
 *          key | bSyncLocalProcess
 *      default | 0
 */
#define kszDNDSyncLocalProcess "DNDSyncLocalProcess"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.direct_pickup_code
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | DPickupCode
 *      default | ""
 *          reg | {exclude:':;<>[]^?%%',maxlength:32}
 */
#define kszDPickupCode "DPickupCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.direct_pickup_enable
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | DPickup
 *      default | 0
 */
#define kszDPickupSwitch "DPickupSwitch"

/**
 * @brief 注释
 */
#define kszWebDSSKeyEnablePageTips "DSSKeyEnablePageTips"

/**
 * @brief 注释
 */
#define kszDSSKeyExtKey "DSSKeyExtKey"

/**
 * @brief 注释
 */
#define kszWebDSSKeyLabelLength "DSSKeyLabelLength"

/**
 * @brief 注释
 */
#define kszDSSKeyLineKey "DSSKeyLineKey"

/**
 * @brief 注释
 */
#define kszDSSKeyProgrammableKey "DSSKeyProgrammableKey"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.end_time
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | EndTime
 *      default | 12/31/23
 */
#define kszDSTEndTime "DSTEndTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.offset_time
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | OffSetTime
 *      default | 0
 *          reg | {required:true,signed:true,range:[-300,300],maxlength:4}
 */
#define kszDSTOffSetTime "DSTOffSetTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.start_time
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | StartTime
 *      default | 01/01/00
 */
#define kszDSTStartTime "DSTStartTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.dst_time_type
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | iDSTTimeType
 *      default | 0
 */
#define kszDSTType "DSTTimeType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.dtmf.dtmfpayload
 *       t49-m7 | account.7.dtmf.dtmf_payload
 *         path | /config/user/voip/sipAccount6.cfg
 *      section | DTMF
 *          key | DTMFPayload
 *      default | 101
 */
#define kszDTMFPayload "DTMFPayload"

/**
 * @brief idle上面日期显示在前面，日期在后面
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.bdateshowbefore
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bDateShowBefore
 *      default | 0
 */
#define kszDateShowBefore "DateShowBefore"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.is_deal180
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bDeal180
 *      default | 0
 */
#define kszDeal180After183Switch "Deal180After183"

/**
 * @brief 检测RTP长度
 */
#define kszDectectRTPPKGLength "DectectRTPPKGLength"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.default_account
 *         path | /config/user/user.ini
 *      section | Account
 *          key | DefaultAccount
 *      default | 0
 */
#define kszDefaultAccountID "DefaultAccountID"

/**
 * @brief DefaultUserLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.default_access_level
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | default_level
 *      default | 0
 */
#define kszDefaultUserLevel "DefaultUserLevel"

/**
 * @brief DefultServerType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.server.type
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strServerType
 *      default | 0
 */
#define kszDefultServerType "DefultServerType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.sip_service_delay
 *       t49-m7 | phone_settings.sip_service_delay
 *         path | /config/system/system.ini
 *      section | REGSURGE
 *          key | iSOLDelayToRun
 *      default | 0
 */
#define kszDelayInitSipService "DelayInitSipService"

/**
 * @brief 注释
 */
#define kszDetailDisplay "DetailDisplay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.login_note_text
 *         path | /config/user/user.ini
 *      section | login_note
 *          key | text
 *      default | ""
 */
#define kszDevtypeLoginNote "DevtypeLoginNote"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.dhcp_host_name
 *         path | /config/system/system.ini
 *      section | dhcp_option_value
 *          key | option12_value
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszDhcpHostnameValue "DhcpHostnameValue"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.dhcp_option.option60_value
 *         path | /config/system/system.ini
 *      section | dhcp_option_value
 *          key | option60_value
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszDhcpOptionValue "DhcpOptionValue"

/**
 * @brief 拨号过程是否允许Intercom自动接起
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.barge_in_dialing.enable
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | bDialAllowBarge
 *      default | 0
 */
#define kszDialAllowIntercomBarge "DialAllowIntercomBarge"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.dialnow_delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | nDialNowDelay
 *      default | 1
 *          reg | {required:true,range:[0,14,1],maxlength:2}
 */
#define kszDialNowDelay "DialNowDelay"

/**
 * @brief 通话过程hold之后按拨号键进入拨号
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dialpad_mode_on_hold.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DialOnHold
 *      default | 0
 */
#define kszDialOnHold "DialOnHold"

/**
 * @brief DialPlan-DialNow根据配置确定是咨询转还是盲转
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.transfer.mode
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | DialPlanTransferMode
 *      default | 0
 */
#define kszDialPlanTransferMode "DialPlanTransferMode"

/**
 * @brief 自动退出拨号界面时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call.dialtone_time_out
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DialToneTimeout
 *      default | 15
 */
#define kszDialToneTimeout "DialToneTimeout"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.talk.strdialimename
 *         path | /config/user/user.ini
 *      section | Talk
 *          key | strDialIMEName
 *      default | 123_Dial
 */
#define kszDialingIMEName "DialingIMEName"

/**
 * @brief DigitMapAutoFwd
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.apply_to.forward
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | EnabledInAutoFwd
 *      default | 1
 */
#define kszDigitMapInAutoFwd "DigitMapAutoFwd"

/**
 * @brief DigitMapConfig
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.string
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | DigitMap
 *      default | ""
 */
#define kszDigitMap "DigitMapConfig"

/**
 * @brief DigitMapDial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.apply_to.on_hook_dial
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | EnabledInDial
 *      default | 1
 */
#define kszDigitMapInDial "DigitMapDial"

/**
 * @brief DigitMapDirDial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.apply_to.directory_dial
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | EnabledInDirDial
 *      default | 1
 */
#define kszDigitMapInDirDial "DigitMapDirDial"

/**
 * @brief DigitMapHistoryDial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.apply_to.history_dial
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | EnabledInHistoryDial
 *      default | 1
 */
#define kszDigitMapInHistoryDial "DigitMapHistoryDial"

/**
 * @brief DigitMapInPredial
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.active.on_hook_dialing
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | EnabledInPredial
 *      default | 0
 */
#define kszDigitMapInPredial "DigitMapInPredial"

/**
 * @brief Digit Map 控制相关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.interdigit_long_timer
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | LongTimer
 *      default | ""
 */
#define kszDigitMapLongTimer "DigitMapLongTimer"

/**
 * @brief DigitMapNoMatchAction
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.no_match_action
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | NoMatchAction
 *      default | ""
 *  t49-default | 2
 */
#define kszDigitMapNoMatchAction "DigitMapNoMatchAction"

/**
 * @brief DigitMapPressSend
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.apply_to.press_send
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | EnabledInPressSend
 *      default | 1
 */
#define kszDigitMapInPressSend "DigitMapPressSend"

/**
 * @brief DigitMapShortTimer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.interdigit_short_timer
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | ShortTimer
 *      default | ""
 */
#define kszDigitMapShortTimer "DigitMapShortTimer"

/**
 * @brief DigitMapSwtich
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.enable
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | bEnabled
 *      default | 0
 */
#define kszDigitMapSwitch "DigitMapSwtich"
#define kszDigitMapSwtich "DigitMapSwtich"

/**
 * @brief 文件加密
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.encryption.directory
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bDirEncryptionEnable
 *      default | 0
 */
#define kszDirEncryptionEnable "DirEncryptionEnable"

/**
 * @brief 联系人搜索匹配方式
 *
 * 默认值：0
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.search_type
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | DirSearchMatchtype
 *      default | 0
 */
#define kszDirSearchMatchtype "DirSearchMatchtype"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/config/user/user.ini
 *      section | Features
 *          key | Direct2LocalContact
 *      default | 0
 */
#define kszDirect2LocalContact "Direct2LocalContact"

/**
 * @brief DirectIpAutoAnswerMuteSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.ip_call.auto_answer.enable
 *         path | /config/user/voip/sipAccount16.cfg
 *      section | AutoAnswer
 *          key | bEnable
 *      default | 0
 */
#define kszDirectIpAutoAnswerMuteSwitch "DirectIpAutoAnswerMuteSwitch"
#define kszIPCallAutoAnswerMuteSwitch "DirectIpAutoAnswerMuteSwitch"

/**
 * @brief 隐藏busy forward图标
 */
#define kszDisableCFBIcon "DisableCFBIcon"

/**
 * @brief 隐藏no answer forward图标
 */
#define kszDisableCFNAIcon "DisableCFNAIcon"

/**
 * @brief DisableHTTPSTrustCertificates
 */
#define kszDisableHTTPSTrustCertificates "DisableHTTPSTrustCertificates"

/**
 * @brief 关闭进入network界面，pin码验证
 */
#define kszDisableNetworkPinCode "DisableNetworkPinCode"

/**
 * @brief 关闭通过DHCP设置账号服务器
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.disablesetacountserverbydhcp
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisableSetAcountServerByDHCP
 *      default | 0
 */
#define kszDisableSetAcountServerByDHCP "DisableSetAcountServerByDHCP"

/**
 * @brief 是否显示匿名呼叫拒绝配置项
 */
#define kszDisplayAnonCallRejection "DisplayAnonCallRejection"

/**
 * @brief 隐藏黑名单选项
 */
#define kszDisplayBlackList "DisplayBlackList"

/**
 * @brief 是否支持call waiting功能
 */
#define kszDisplayCallWaiting "DisplayCallWaiting"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.contact_photo_display.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEnableDisplayContactPhoto
 *      default | 1
 */
#define kszDisplayContactPhoto "DisplayContactPhoto"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward_call_popup.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayForwardCallPopup
 *      default | 1
 */
#define kszDisplayForwardCallPopup "DisplayForwardCallPopup"

/**
 * @brief 是否显示Hoteling自动登录控件
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.auto_login.display
 *       t49-m7 | hoteling.auto_login.display
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayHotelingAutoLogin
 *      default | 1
 */
#define kszDisplayHotelingAutoLogin "DisplayHotelingAutoLogin"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.called_party_info_display.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bDisplayIncomingCallInfo
 *      default | 0
 */
#define kszDisplayIncomingCallInfo "DisplayIncomingCallInfo"

/**
 * @brief T42 LineKey分行显示时间间隔
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.roll_dsskey_label_interval
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayInterval
 *      default | 4000
 */
#define kszDisplayInterval "DisplayInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_info_display_method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | nDisplayMethod
 *      default | 0
 */
#define kszDisplayMethod "DisplayMethod"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.caller_name_type_on_dialing
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayMethodOnDialing
 *      default | 3
 */
#define kszDisplayMethodOnDialing "DisplayMethodOnDialing"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.missed_call_popup.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayMissedCallPopup
 *      default | 1
 */
#define kszDisplayMissedCallPopup "DisplayMissedCallPopup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.displayname.value
 *         path | /tmp/InitSave
 *      section | DisplayName
 *          key | Value
 *      default | ""
 */
#define kszDisplayNameValue "DisplayNameValue"

/**
 * @brief 隐藏SCA下的Private Hold的选项
 */
#define kszDisplayPrivateHold "DisplayPrivateHold"

/**
 * @brief 隐藏SCA下的BargeIn
 */
#define kszDisplayScaBargeIn "DisplayScaBargeIn"

/**
 * @brief 是否显示屏保时钟
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.display_clock.enable
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | DisplayScreensaverClock
 *      default | 1
 */
#define kszDisplayScreensaverClock "DisplayScreensaverClock"
#define kszScreensaverDisplayClock "DisplayScreensaverClock"

/**
 * @brief 手柄菜单隐藏SetKey1
 */
#define kszDisplaySetKeyOne "DisplaySetKeyOne"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.text_message_popup.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayTextMessagePopup
 *      default | 1
 */
#define kszDisplayTextMessagePopup "DisplayTextMessagePopup"

/**
 * @brief 注释
 *
 * 是否显示RTP Status
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr.states_show_on_gui.enable
 *         path | /config/user/user.ini
 *      section | Voice Quality
 *          key | bDisplayVQReportOnUI
 *      default | 0
 */
#define kszDisplayVQReportOnUI "DisplayVQReportOnUI"

/**
 * @brief 注释
 *
 * 是否在web显示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr.states_show_on_web.enable
 *         path | /config/user/user.ini
 *      section | Voice Quality
 *          key | bDisplayVQReportOnWeb
 *      default | 0
 */
#define kszDisplayVQReportOnWeb "DisplayVQReportOnWeb"

/**
 * @brief 隐藏配置VM的接口
 */
#define kszDisplayVoiceMail "DisplayVoiceMail"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.voice_mail_popup.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DisplayVoiceMailPopup
 *      default | 1
 */
#define kszDisplayVoiceMailPopup "DisplayVoiceMailPopup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.fwd_diversion_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DiversionEnable
 *      default | 1
 */
#define kszDiversionEnable "DiversionEnable"

/**
 * @brief 注释
 */
#define kszDndAllowance "DndAllowance"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.allow
 *         path | /config/user/user.ini
 *      section | Forbidden
 *          key | DND
 *      default | 0
 */
#define kszDndForbidden "DndForbidden"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd_mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DNDMode
 *      default | 0
 */
#define kszDndMode "DndMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.off_code
 *         path | /config/user/user.ini
 *      section | DND
 *          key | strOffCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszDndOffCode "DndOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.on_code
 *         path | /config/user/user.ini
 *      section | DND
 *          key | strOnCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszDndOnCode "DndOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.enable
 *         path | /config/user/user.ini
 *      section | DND
 *          key | bEnable
 *      default | 0
 */
#define kszDndStatus "DndStatus"

/**
 * @brief DnsAutoSwitchEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.dns_auto_switch_enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | dns_auto_switch_enable
 *      default | 0
 */
#define kszDnsAutoSwitchEnable "DnsAutoSwitchEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.dsskey_value_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IsDssKeyFWDEnable
 *      default | 0
 */
#define kszDssKeyFWDEnable "DssKeyFWDEnable"
#define kszMultiFWDEnable "DssKeyFWDEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dsskey_blind_tran
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | DsskeyBlindTran
 *      default | ""
 */
#define kszDsskeyBlindTran "DsskeyBlindTran"

/**
 * @brief DsskeyConfigMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local.dsskey_type_config.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DsskeyConfigMode
 *      default | 0
 */
#define kszDsskeyConfigMode "DsskeyConfigMode"

/**
 * @brief Dsskey改变延时刷新
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.dsskeydelayrefreshtimer
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DsskeyDelayRefreshTimer
 *      default | 0
 */
#define kszDsskeyDelayRefreshTimer "DsskeyDelayRefreshTimer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.config_dsskey_length
 *         path | /config/user/user.ini
 *      section | Dsskey
 *          key | DsskeyLength
 *      default | 0
 */
#define kszDsskeyLength "DsskeyLength"

/**
 * @brief Linekey 缩略显示
 *
 * T52 Linekey缩短功能
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.config_dsskey_length.shorten
 *      default | 1
 */
#define kszDsskeyLengthShorten "DsskeyLengthShorten"

/**
 * @brief dsskey 锁定密码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.dsskey_unlock_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DsskeyLockPassword
 *      default | ""
 *      encrypt | True
 */
#define kszDsskeyLockPassword "DsskeyLockPassword"

/**
 * @brief 注释
 */
#define kszDsskeyMemoryKey "DsskeyMemoryKey"

/**
 * @brief T48 Dsskey展开壁纸
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.backgrounds_with_dsskey_unfold
 *         path | /config/system/system.ini
 *      section | Wallpaper
 *          key | DsskeyWallpaper
 *      default | ""
 *  t49-default | Default:Default.png
 */
#define kszDsskeyWallPaper "DsskeyWallpaper"
#define kszDsskeyWallpaper "DsskeyWallpaper"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.repetition
 *         path | /config/user/vpm.cfg
 *      section | DTMF
 *          key | repetition
 *      default | 3
 */
#define kszDtmfrep "Dtmfrep"

/**
 * @brief E.164 格式自动带+
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.requesturi.e164.addglobalprefix
 *         path | /config/user/user.ini
 *      section | Features
 *          key | E164AddGlobalPrefix
 *      default | 0
 */
#define kszE164AddGlobalPrefix "E164AddGlobalPrefix"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sfb.e911_location_tip
 *         path | /config/user/user.ini
 *      section | Lync
 *          key | E911LocationTip
 *      default | ""
 */
#define kszE911LocationTip "E911LocationTip"

/**
 * @brief EHSPlayPhoneRingSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.headset_play_phone_ringtone.enable
 *         path | /config/user/user.ini
 *      section | RingerDevice
 *          key | AllowEHSPlayPhoneRing
 *      default | 0
 */
#define kszEHSPlayPhoneRingSwitch "EHSPlayPhoneRingSwitch"

/**
 * @brief EFK 功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.enhanced_dss_keys.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEdkEnable
 *      default | 0
 */
#define kszEdkSwitch "EdkSwitch"

/**
 * @brief EDK 提示模式配置开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | edk.id_mode.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEdkTipMode
 *      default | 0
 */
#define kszEdkTipMode "EdkTipMode"

/**
 * @brief EmengencySwtich
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.enable
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | bEnable
 *      default | 1
 */
#define kszEmengencySwtich "EmengencySwtich"

/**
 * @brief EmergencyCallWindow
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.emergency_call_window_time
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyCallWindowTime
 *      default | 5
 */
#define kszEmergencyCallWindows "EmergencyCallWindow"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.emergency.number
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | strNumberList
 *      default | ""
 *          reg | {multiple:{digits:true,splits:','},maxlength:99}
 */
#define kszEmergencyNumberList "EmergencyNumberList"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.emergency_ring
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | EmergencyRing
 *      default | Emergency.wav
 */
#define kszEmergencyRing "EmergencyRing"

/**
 * @brief EmergencyServerPriority%%d
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.%%d.server_priority
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyServerPriority%%d
 *      default | ""
 */
#define kszEmengencyServerPriority "EmergencyServerPriority%d"

/**
 * @brief EmergencyValue%%d
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.%%d.value
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyValue%%d
 *      default | ""
 */
#define kszEmergencyValue "EmergencyValue%d"

/**
 * @brief 是否支持阿拉伯语
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.language.isenabelarabic
 *         path | /config/system/system.ini
 *      section | Language
 *          key | IsEnabelArabic
 *      default | 0
 */
#define kszEnabelArabic "EnabelArabic"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | acd.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEnableACD
 *      default | 0
 */
#define kszEnableACDAutoAvailable "EnableACDAutoAvailable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | acd.bw
 *         path | /config/user/user.ini
 *      section | ACD
 *          key | Broadsoft
 *      default | 0
 */
#define kszEnableACDBroadsoft "EnableACDBroadsoft"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.idle_access_always_fwd.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AlwaysSetFWD
 *      default | 0
 */
#define kszEnableAlwaysSetFWD "EnableAlwaysSetFWD"

/**
 * @brief 是否支持匿名呼叫
 */
#define kszEnableAnonymousCall "EnableAnonymousCall"

/**
 * @brief EnableAutoAnswerTone
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_answer_tone.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | EnableAutoAnswerTone
 *      default | 1
 */
#define kszEnableAutoAnswerTone "EnableAutoAnswerTone"

/**
 * @brief 自动接起是否收话路限制  config id 没人用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.enableautoanwsercountlimit
 *       t49-m7 | features.limit_auto_answer_count.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | EnableAutoAnwserCountLimit
 *      default | 1
 */
#define kszEnableAutoAnwserCountLimit "EnableAutoAnwserCountLimit"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.dsskey_directory_auto.enable
 *         path | /config/user/user.ini
 *      section | Dsskey
 *          key | EnableAutoFavorite
 *      default | 1
 */
#define kszEnableAutoFavorite "EnableAutoFavorite"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.account1_auto_set.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoSetAccount
 *      default | 0
 */
#define kszEnableAutoSetAccount "EnableAutoSetAccount"

/**
 * @brief EnableCFChannel
 */
#define kszEnableCFChannel "EnableCFChannel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.csta.enable
 *         path | /config/user/user.ini
 *      section | CSTA
 *          key | bEnabled
 *      default | 0
 */
#define kszEnableCSTA "EnableCSTA"

/**
 * @brief UC 是否允许修改presence状态
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xmpp.change_presence.enable
 *         path | /config/user/user.ini
 *      section | Presence
 *          key | bEnableChangeStatus
 *      default | 0
 *  t49-default | 1
 */
#define kszEnableChangePresenceStatus "EnableChangePresenceStatus"

/**
 * @brief zero是否去执行option66更新
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.dhcp_option.enable
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | bEnableDHCPOption66
 *      default | 1
 */
#define kszEnableDHCPOption66 "EnableDHCPOption66"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.hide_delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HideDTMFDelay
 *      default | 0
 */
#define kszEnableDTMFDelay "EnableDTMFDelay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.replace_tran
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DTMF_Replace_Tran
 *      default | 0
 */
#define kszEnableDTMFReplaceTran "EnableDTMFReplaceTran"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phonesetting.sendpoundkey
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | SendPoundKey
 *      default | 0
 */
#define kszEnableDialPoundKey "EnableDialPoundKey"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.direct_ip_call_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEnableDirectIP
 *      default | 1
 */
#define kszEnableDirectIPCall "EnableDirectIPCall"

/**
 * @brief 是否支持DoublePinCode，Reg和Setting使用不同Pincode,由V73宏DOUBLE_PIN_CODE转换而来
 */
#define kszEnableDoublePinCode "EnableDoublePinCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.input.hebrew_enable
 *         path | /config/system/system.ini
 *      section | Language
 *          key | IsEnableHebrew
 *      default | 0
 */
#define kszEnableHebrew "EnableHebrew"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.hide
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HideDTMF
 *      default | 0
 */
#define kszEnableHideDTMF "EnableHideDTMF"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_default_account
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bShowDefaultAccount
 *      default | 0
 */
#define kszEnableIdleDefaultAccount "EnableIdleDefaultAccount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.shortcuts_enable
 *         path | /config/user/user.ini
 *      section | Idle
 *          key | bShortCutsEnable
 *      default | ""
 */
#define kszEnableIdleShortCuts "EnableIdleShortCuts"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.voice_mail_popup_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | NewVoiceMailPopWind
 *      default | 1
 */
#define kszNewVoiceMailPopWind "EnableNewVoiceMailPopWind"

/**
 * @brief 更改手柄配置功能的相关配置 begin
 */
#define kszEnableNotifyHandsetConfig "EnableNotifyHandsetConfig"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.password_dial.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PswDialEnable
 *      default | 0
 */
#define kszEnablePSWDial "EnablePSWDial"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.page_tip
 *         path | /config/user/user.ini
 *      section | Page
 *          key | PageTip
 *      default | 0
 */
#define kszEnablePageTips "EnablePageTips"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.partition_tone
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | PartitionTone
 *      default | 0
 */
#define kszUnregisterdPartitionTone "EnablePartitionTone"

/**
 * @brief 开放节电模式功能的开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.features.enable_powersaving
 *      default | 0
 */
#define kszPowerSavingSwitch "EnablePowerSaving"

/**
 * @brief 密码是否可以为空
 *
 * 默认值：1
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_empty_pwd.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | EnablePwdEmpty
 *      default | 0
 */
#define kszEnablePwdEmpty "EnablePwdEmpty"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.callcenter.benablequeuestatuslight
 *         path | /config/user/user.ini
 *      section | CallCenter
 *          key | bEnableQueueStatusLight
 *      default | 1
 */
#define kszEnableQueueStatusLight "EnableQueueStatusLight"

/**
 * @brief 更改repeater mode时，是否验证pin
 */
#define kszEnableRepeaterPinCode "EnableRepeaterPinCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.replacerule.replaceall
 *         path | /config/user/user.ini
 *      section | ReplaceRule
 *          key | ReplaceAll
 *      default | 1
 */
#define kszEnableReplaceAll "EnableReplaceAll"

/**
 * @brief Ringcentral BugID=26621
 */
#define kszEnableRingCentralVM "EnableRingCentralVM"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.sip_notify
 *         path | /config/user/user.ini
 *      section | PushXML
 *          key | SIPNotify
 *      default | 0
 */
#define kszEnableSIPNotifyPushXML "EnableSIPNotifyPushXML"

/**
 * @brief 屏保功能是否启用
 */
#define kszEnableScreensaver "EnableScreenSaver"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.send_pound_key
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bSendPoundKey
 *      default | 0
 */
#define kszSendPoundKey "EnableSendPoundKey"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.silence_during_call.enable
 *       t49-m7 | phone_settings.silence_during_call.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | EnableSilenceMode
 *      default | 0
 */
#define kszEnableSilenceMode "EnableSilenceMode"

/**
 * @brief 注释 // configmap 重复定义
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_stun.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | EnableSTUN
 *      default | 0
 */
#define kszEnableStun "EnableStun"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xmpp.enable
 *       t49-m7 | features.uc_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bEnabledUC
 *      default | 0
 */
#define kszEnableUC "EnableUC"

/**
 * @brief 配置on/off code是否使用DialPlan规则
 *
 * 默认值：0
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.replace_feature_onoff_code.enable
 *         path | /config/user/user.ini
 *      section | ReplaceRule
 *          key | ReplaceOnOffCode
 *      default | 0
 */
#define kszEnableUseDialPlanInCode "EnableUseDialPlanInCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.voice_mail_tone_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | VoiceMailTone
 *      default | 1
 */
#define kszEnableVoiceMailTone "EnableVoiceMailTone"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | watch_dog.enable
 *         path | /config/system/system.ini
 *      section | WatchDog
 *          key | bEnable
 *      default | 1
 */
#define kszEnableWatchDog "EnableWatchDog"

/**
 * @brief EnableWrapup
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | genesys.acd.after_call_work_display.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | EnableWrapup
 *      default | 0
 */
#define kszEnableWrapup "EnableWrapup"

/**
 * @brief W56 BETA SP1 , HAC受话器需求
 */
#define kszEnabletHacEQ "EnabletHacEQ"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.enlock
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | EnLock
 *      default | 0
 */
#define kszEnlockPWD "EnlockPWD"
#define kszSettingEnLock "EnlockPWD"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.pc_port.speed_duplex
 *         path | /config/system/system.ini
 *      section | Ethernet
 *          key | PCPortLink
 *      default | 0
 */
#define kszEthernetPCPortLink "EthernetPCPortLink"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.internet_port.speed_duplex
 *         path | /config/system/system.ini
 *      section | Ethernet
 *          key | WANPortLink
 *      default | 0
 */
#define kszEthernetWANPortLink "EthernetWANPortLink"

/**
 * @brief blf刷新时退出屏保
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.exitscreensaverforblf
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ExitScreenSaverForBLF
 *      default | 0
 */
#define kszExitScreenSaverForBLF "ExitScreenSaverForBLF"

/**
 * @brief 注册向导是否需要确认登录成功之后再退出
 *
 * 默认值：1
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_exit_wizard_after_login.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ExitWizardAfterLogin
 *      default | 1
 */
#define kszExitWizardAfterLogin "ExitWizardAfterLogin"

/**
 * @brief Exp39PageNum
 */
#define kszExp39PageNum "Exp39PageNum"

/**
 * @brief exp inactive backlight
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.exp40_inactive_backlight_level
 *         path | /config/user/user.ini
 *      section | Backlight
 *          key | nExpInactiveLevel
 *      default | -1
 */
#define kszExpInactiveLevel "ExpInactiveLevel"

/**
 * @brief ExpInsertRequestInterval
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.expinsertrequestinterval
 *      default | 2500
 */
#define kszExpInsertRequestInterval "ExpInsertRequestInterval"

/**
 * @brief ExpPageLedBlfCallInEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.page_tip.blf_call_in.enable
 *      default | 1
 */
#define kszExpPageLedBlfCallInEnable "ExpPageLedBlfCallInEnable"

/**
 * @brief Exp同步blf状态的闪烁规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.page_tip.blf_call_in.led
 *      default | $LEDr300o300$
 */
#define kdzExpPageLedBlfCallInRule "ExpPageLedBlfCallInRule"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.expupdate.exp_2
 *         path | /config/user/user.ini
 *      section | ExpUpdate
 *          key | Exp_2
 *      default | ""
 */
#define kszExp_2 "Exp_2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.expupdate.exp_5
 *         path | /config/user/user.ini
 *      section | ExpUpdate
 *          key | Exp_5
 *      default | ""
 */
#define kszExp_5 "Exp_5"

/**
 * @brief 注释
 */
#define kszExportAllDiagnosticFiles "ExportAllDiagnosticFiles"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.export_cfg_erase_pwd
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bExportCFGClearPWD
 *      default | 1
 */
#define kszExportCFGClearPWD "ExportCFGClearPWD"

/**
 * @brief 功能禁用开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hdmi_out.enable
 *      default | 1
 */
#define kszExtDisplayEnable "ExtDisplayEnable"

/**
 * @brief ExtDisplayResolution
 */
#define kszExtDisplayResolution "ExtDisplayResolution"

/**
 * @brief 功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hdmi_out_status
 *      default | 1
 */
#define kszExtDisplaySwitch "ExtDisplaySwitch"

/**
 * @brief 本地FWD是否同步服务器
 */
#define kszFWDFeatureKeySync "FWDFeatureKeySync"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.always.refuse_code
 *         path | /config/user/user.ini
 *      section | RefuseCode
 *          key | nFWDRefuseCode
 *      default | 302
 */
#define kszFWDRefuseCode "FWDRefuseCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.custom_factory_config.enable
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | bEnableCustom
 *      default | 0
 */
#define kszFactoryConfigCustomEnabled "FactoryConfigCustomEnabled"

/**
 * @brief FactoryHandsetName.%%d
 */
#define kszFactoryHandsetName "FactoryHandsetName.%d"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.factory_reset_option
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | eResetOption
 *      default | 3
 */
#define kszResetOption "FactoryResetOption"

/**
 * @brief FactorySN
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.factory_sn
 *         path | /config/vpPhone/ADVnumber.cfg
 *      section | sn
 *          key | sn
 *      default | ""
 *      encrypt | True
 */
#define kszFactorySN "FactorySN"

/**
 * @brief FavoriteDirAutoSetSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_contact.favorite.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bFavoriteDirAutoSetSwitch
 *      default | 0
 */
#define kszFavoriteDirAutoSetSwitch "FavoriteDirAutoSetSwitch"

/**
 * @brief FeatureDirectSendEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.direct_send.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ParkDirectSendEnable
 *      default | 1
 */
#define kszFeatureDirectSendEnable "FeatureDirectSendEnable"

/**
 * @brief 注释
 */
#define kszFeatureKeySynchronisation "FeatureKeySynchronisation"

/**
 * @brief 注释
 */
#define kszFeaturesAAUCLoginServer "FeaturesAAUCLoginServer"

/**
 * @brief 注释
 */
#define kszFeaturesAAUCLoginServerPort "FeaturesAAUCLoginServerPort"

/**
 * @brief 注释
 */
#define kszFeaturesACDAutoAvailable "FeaturesACDAutoAvailable"

/**
 * @brief 注释
 */
#define kszFeaturesACDAutoAvailableTimer "FeaturesACDAutoAvailableTimer"

/**
 * @brief 注释
 */
#define kszFeaturesActionURILimitIP "FeaturesActionURILimitIP"

/**
 * @brief 注释
 */
#define kszFeaturesAllowIntercom "FeaturesAllowIntercom"

/**
 * @brief 注释
 */
#define kszFeaturesAllowMute "FeaturesAllowMute"

/**
 * @brief 注释
 */
#define kszFeaturesAllowTransExistCall "FeaturesAllowTransExistCall"

/**
 * @brief 注释
 */
#define kszFeaturesAlwaysForward "FeaturesAlwaysForward"

/**
 * @brief 注释
 */
#define kszFeaturesAlwaysForwardEnabled "FeaturesAlwaysForwardEnabled"

/**
 * @brief 注释
 */
#define kszFeaturesAlwaysForwardOffCode "FeaturesAlwaysForwardOffCode"

/**
 * @brief 注释
 */
#define kszFeaturesAlwaysForwardOnCode "FeaturesAlwaysForwardOnCode"

/**
 * @brief 注释
 */
#define kszFeaturesAlwaysForwardTarget "FeaturesAlwaysForwardTarget"

/**
 * @brief 注释
 */
#define kszFeaturesAnswerNewInCall "FeaturesAnswerNewInCall"

/**
 * @brief 注释
 */
#define kszFeaturesAttendedTransferCall "FeaturesAttendedTransferCall"

/**
 * @brief 注释
 */
#define kszFeaturesAutoAnswer "FeaturesAutoAnswer"

/**
 * @brief 注释
 */
#define kszFeaturesAutoAnswerDelay "FeaturesAutoAnswerDelay"

/**
 * @brief 注释
 */
#define kszFeaturesAutoRedial "FeaturesAutoRedial"

/**
 * @brief 注释
 */
#define kszFeaturesAutoRedialInterval "FeaturesAutoRedialInterval"

/**
 * @brief 注释
 */
#define kszFeaturesAutoRedialTimes "FeaturesAutoRedialTimes"

/**
 * @brief 注释
 */
#define kszFeaturesBLFLedMode "FeaturesBLFLedMode"

/**
 * @brief 注释
 */
#define kszFeaturesBLFLedOffInIdle "FeaturesBLFLedOffInIdle"

/**
 * @brief 注释
 */
#define kszFeaturesBLFNotify "FeaturesBLFNotify"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.backgrounds
 *         path | /config/user/user.ini
 *      section | Features
 *          key | BackGrounds
 *      default | ""
 */
#define kszFeaturesBackGrounds "FeaturesBackGrounds"

/**
 * @brief 注释
 */
#define kszFeaturesBlindTranOnhook "FeaturesBlindTranOnhook"

/**
 * @brief 注释
 */
#define kszFeaturesBlindTransferCall "FeaturesBlindTransferCall"

/**
 * @brief 注释
 */
#define kszFeaturesBluetoothActive "FeaturesBluetooth"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForward "FeaturesBusyForward"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForwardEnabled "FeaturesBusyForwardEnabled"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForwardOff "FeaturesBusyForwardOff"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForwardOffCode "FeaturesBusyForwardOffCode"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForwardOn "FeaturesBusyForwardOn"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForwardOnCode "FeaturesBusyForwardOnCode"

/**
 * @brief 注释
 */
#define kszFeaturesBusyForwardTarget "FeaturesBusyForwardTarget"

/**
 * @brief 注释
 */
#define kszFeaturesBusytoIdle "FeaturesBusytoIdle"

/**
 * @brief 注释
 */
#define kszFeaturesBusytoneDelay "FeaturesBusytoneDelay"

/**
 * @brief 注释
 */
#define kszFeaturesButtonSound "FeaturesButtonSound"

/**
 * @brief 注释
 */
#define kszFeaturesCallCompletion "FeaturesCallCompletion"

/**
 * @brief 注释
 */
#define kszFeaturesCallEstablish "FeaturesCallEstablish"

/**
 * @brief 注释
 */
#define kszFeaturesCallNumFilter "FeaturesCallNumFilter"

/**
 * @brief 注释
 */
#define kszFeaturesCallPark "FeaturesCallPark"

/**
 * @brief 注释
 */
#define kszFeaturesCallPickup "FeaturesCallPickup"

/**
 * @brief 注释
 */
#define kszFeaturesCallTerminated "FeaturesCallTerminated"

/**
 * @brief 注释
 */
#define kszFeaturesCallWaiting "FeaturesCallWaiting"

/**
 * @brief 注释
 */
#define kszFeaturesCallWaitingTone "FeaturesCallWaitingTone"

/**
 * @brief 注释
 */
#define kszFeaturesCalllogShowNum "FeaturesCalllogShowNum"

/**
 * @brief 注释
 */
#define kszFeaturesClosePowerLight "FeaturesClosePowerLight"

/**
 * @brief 注释
 */
#define kszFeaturesCommonPowerLightOn "FeaturesCommonPowerLightOn"

/**
 * @brief 注释
 */
#define kszFeaturesDND "FeaturesDND"

/**
 * @brief 注释
 */
#define kszFeaturesDNDAccount "FeaturesDNDAccount"

/**
 * @brief 注释
 */
#define kszFeaturesDNDAuthorizedNumbers "FeaturesDNDAuthorizedNumbers"

/**
 * @brief 注释
 */
#define kszFeaturesDNDEmergency "FeaturesDNDEmergency"

/**
 * @brief 注释
 */
#define kszFeaturesDNDMode "FeaturesDNDMode"

/**
 * @brief 注释
 */
#define kszFeaturesDNDOff "FeaturesDNDOff"

/**
 * @brief 注释
 */
#define kszFeaturesDNDOffCode "FeaturesDNDOffCode"

/**
 * @brief 注释
 */
#define kszFeaturesDNDOn "FeaturesDNDOn"

/**
 * @brief 注释
 */
#define kszFeaturesDNDOnCode "FeaturesDNDOnCode"

/**
 * @brief 注释
 */
#define kszFeaturesDNDStatus "FeaturesDNDStatus"

/**
 * @brief 注释
 */
#define kszFeaturesDTMFRepetition "FeaturesDTMFRepetition"

/**
 * @brief 注释
 */
#define kszFeaturesDTMFReplaceTran "FeaturesDTMFReplaceTran"

/**
 * @brief 注释
 */
#define kszFeaturesDhcpHostname "FeaturesDhcpHostname"

/**
 * @brief 注释
 */
#define kszFeaturesDirectIPCall "FeaturesDirectIPCall"

/**
 * @brief 注释
 */
#define kszWebFeaturesDisplayForwardCallPopup "FeaturesDisplayForwardCallPopup"

/**
 * @brief FeaturesDisplayMethodOnDialing
 */
#define kszFeaturesDisplayMethodOnDialing "FeaturesDisplayMethodOnDialing"

/**
 * @brief 注释
 */
#define kszWebFeaturesDisplayMissedCallPopup "FeaturesDisplayMissedCallPopup"

/**
 * @brief 注释
 */
#define kszWebFeaturesDisplayTextMessagePopup "FeaturesDisplayTextMessagePopup"

/**
 * @brief 注释
 */
#define kszWebFeaturesDisplayVoiceMailPopup "FeaturesDisplayVoiceMailPopup"

/**
 * @brief 注释
 */
#define kszFeaturesDiversionHistoryInfo "FeaturesDiversionHistoryInfo"

/**
 * @brief 注释
 */
#define kszFeaturesDsskeyBlindTran "FeaturesDsskeyBlindTran"

/**
 * @brief 注释
 */
#define kszFeaturesDualHeadset "FeaturesDualHeadset"

/**
 * @brief 注释
 */
#define kszFeaturesEnableAutoFavorite "FeaturesEnableAutoFavorite"

/**
 * @brief 注释
 */
#define kszFeaturesForwardAccount "FeaturesForwardAccount"

/**
 * @brief 注释
 */
#define kszFeaturesForwardIncomingCall "FeaturesForwardIncomingCall"

/**
 * @brief 注释
 */
#define kszFeaturesForwardMode "FeaturesForwardMode"

/**
 * @brief 注释
 */
#define kszFeaturesForwardOffCode "FeaturesForwardOffCode"

/**
 * @brief 注释
 */
#define kszFeaturesForwardOnCode "FeaturesForwardOnCode"

/**
 * @brief 注释
 */
#define kszFeaturesFwdInternational "FeaturesFwdInternational"

/**
 * @brief 注释
 */
#define kszFeaturesHandfreeSendVolume "FeaturesHandfreeSendVolume"

/**
 * @brief 注释
 */
#define kszFeaturesHandsetSendVolume "FeaturesHandsetSendVolume"

/**
 * @brief 注释
 */
#define kszFeaturesHeadForward "FeaturesHeadForward"

/**
 * @brief 注释
 */
#define kszFeaturesHeadsetPrior "FeaturesHeadsetPrior"

/**
 * @brief 注释
 */
#define kszFeaturesHeadsetSendVolume "FeaturesHeadsetSendVolume"

/**
 * @brief 注释
 */
#define kszWebFeaturesHideAccessCodes "FeaturesHideAccessCodes"

/**
 * @brief 注释
 */
#define kszFeaturesHold "FeaturesHold"

/**
 * @brief 注释
 */
#define kszFeaturesHoldPowerLightFlash "FeaturesHoldPowerLightFlash"

/**
 * @brief 注释
 */
#define kszFeaturesHotlineDelay "FeaturesHotlineDelay"

/**
 * @brief 注释
 */
#define kszFeaturesHotlineNumber "FeaturesHotlineNumber"

/**
 * @brief 注释
 */
#define kszFeaturesIPChange "FeaturesIPChange"

/**
 * @brief 注释
 */
#define kszFeaturesIdletoBusy "FeaturesIdletoBusy"

/**
 * @brief 注释
 */
#define kszFeaturesIncomingCall "FeaturesIncomingCall"

/**
 * @brief 注释
 */
#define kszFeaturesIntercomBarge "FeaturesIntercomBarge"

/**
 * @brief 注释
 */
#define kszFeaturesIntercomMute "FeaturesIntercomMute"

/**
 * @brief 注释
 */
#define kszFeaturesIntercomTone "FeaturesIntercomTone"

/**
 * @brief 注释
 */
#define kszFeaturesIsDeal180 "FeaturesIsDeal180"

/**
 * @brief 注释
 */
#define kszFeaturesKeyasSend "FeaturesKeyasSend"

/**
 * @brief 注释
 */
#define kszFeaturesLogOff "FeaturesLogOff"

/**
 * @brief 注释
 */
#define kszFeaturesLogOn "FeaturesLogOn"

/**
 * @brief 注释
 */
#define kszFeaturesLogonWizard "FeaturesLogonWizard"

/**
 * @brief 注释
 */
#define kszFeaturesMailPowerLightFlash "FeaturesMailPowerLightFlash"

/**
 * @brief 注释
 */
#define kszFeaturesMissedCall "FeaturesMissedCall"

/**
 * @brief 注释
 */
#define kszFeaturesMulticastCodec "FeaturesMulticastCodec"

/**
 * @brief 注释
 */
#define kszFeaturesMute "FeaturesMute"

/**
 * @brief 注释
 */
#define kszFeaturesMutePowerLightFlash "FeaturesMutePowerLightFlash"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForward "FeaturesNoAnswerForward"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardEnabled "FeaturesNoAnswerForwardEnabled"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardOff "FeaturesNoAnswerForwardOff"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardOffCode "FeaturesNoAnswerForwardOffCode"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardOn "FeaturesNoAnswerForwardOn"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardOnCode "FeaturesNoAnswerForwardOnCode"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardRingtimes "FeaturesNoAnswerForwardRingtimes"

/**
 * @brief 注释
 */
#define kszFeaturesNoAnswerForwardTarget "FeaturesNoAnswerForwardTarget"

/**
 * @brief 注释
 */
#define kszFeaturesOffHook "FeaturesOffHook"

/**
 * @brief 注释
 */
#define kszFeaturesOnHook "FeaturesOnHook"

/**
 * @brief 注释
 */
#define kszFeaturesOutgoingCall "FeaturesOutgoingCall"

/**
 * @brief 注释
 */
#define kszFeaturesPlayHoldTone "FeaturesPlayHoldTone"

/**
 * @brief 注释
 */
#define kszFeaturesPlayHoldToneDelay "FeaturesPlayHoldToneDelay"

/**
 * @brief 注释
 */
#define kszFeaturesPlayLocalDTMFTone "FeaturesPlayLocalDTMFTone"

/**
 * @brief 注释
 */
#define kszFeaturesPswDial "FeaturesPswDial"

/**
 * @brief 注释
 */
#define kszFeaturesPswLength "FeaturesPswLength"

/**
 * @brief 注释
 */
#define kszFeaturesPswPrefix "FeaturesPswPrefix"

/**
 * @brief 注释
 */
#define kszFeaturesPushxmlServerAddr "FeaturesPushxmlServerAddr"

/**
 * @brief 注释
 */
#define kszFeaturesRFC2543Hold "FeaturesRFC2543Hold"

/**
 * @brief 注释
 */
#define kszFeaturesReLogOffTime "FeaturesReLogOffTime"

/**
 * @brief 注释
 */
#define kszFeaturesRebootInTalking "FeaturesRebootInTalking"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | super_search.recent_call
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RecentCallEnable
 *      default | 0
 */
#define kszFeaturesRecentCallEnable "FeaturesRecentCallEnable"

/**
 * @brief 注释
 */
#define kszFeaturesRedialTone "FeaturesRedialTone"

/**
 * @brief 注释
 */
#define kszFeaturesRegisterFail "FeaturesRegisterFail"

/**
 * @brief Base账号注册成功提示
 */
#define kszFeaturesRegisteredPowerLEDFlash "FeaturesRegisteredPowerLEDFlash"

/**
 * @brief 注释
 */
#define kszFeaturesRejectIncomingCall "FeaturesRejectIncomingCall"

/**
 * @brief 注释
 */
#define kszFeaturesReservePound "FeaturesReservePound"

/**
 * @brief 注释
 */
#define kszFeaturesReturnCodeDND "FeaturesReturnCodeDND"

/**
 * @brief 注释
 */
#define kszFeaturesReturnCodeRefuse "FeaturesReturnCodeRefuse"

/**
 * @brief 注释
 */
#define kszFeaturesRingPowerLightFlash "FeaturesRingPowerLightFlash"

/**
 * @brief 注释
 */
#define kszFeaturesRingerDev "FeaturesRingerDev"

/**
 * @brief 注释
 */
#define kszFeaturesSRemoteNameEnable "FeaturesSRemoteNameEnable"

/**
 * @brief 注释
 */
#define kszFeaturesSRemoteNameFlashTime "FeaturesSRemoteNameFlashTime"

/**
 * @brief 注释
 */
#define kszFeaturesSaveCallHistory "FeaturesSaveCallHistory"

/**
 * @brief 注释
 */
#define kszFeaturesSemiAttendTransOnHook "FeaturesSemiAttendTransOnHook"

/**
 * @brief 注释
 */
#define kszFeaturesSemiAttendTransfer "FeaturesSemiAttendTransfer"

/**
 * @brief 注释
 */
#define kszFeaturesSendPoundKey "FeaturesSendPoundKey"

/**
 * @brief 注释
 */
#define kszFeaturesSendSound "FeaturesSendSound"

/**
 * @brief 注释
 */
#define kszFeaturesSetupComplete "FeaturesSetupComplete"

/**
 * @brief 注释
 */
#define kszFeaturesSuppressDTMFDisplay "FeaturesSuppressDTMFDisplay"

/**
 * @brief 注释
 */
#define kszFeaturesSuppressDTMFDisplayDelay "FeaturesSuppressDTMFDisplayDelay"

/**
 * @brief 注释
 */
#define kszFeaturesTalkPowerLightFlash "FeaturesTalkPowerLightFlash"

/**
 * @brief 注释
 */
#define kszFeaturesTimeOutforDialNow "FeaturesTimeOutforDialNow"

/**
 * @brief 注释
 */
#define kszFeaturesTranSendDTMF "FeaturesTranSendDTMF"

/**
 * @brief 注释
 */
#define kszFeaturesTransferCall "FeaturesTransferCall"

/**
 * @brief 注释
 */
#define kszFeaturesTransferFailed "FeaturesTransferFailed"

/**
 * @brief 注释
 */
#define kszFeaturesTransferFinished "FeaturesTransferFinished"

/**
 * @brief 注释
 */
#define kszFeaturesTransferModeViaDsskey "FeaturesTransferModeViaDsskey"

/**
 * @brief 注释
 */
#define kszFeaturesTransferonConferenceHangup "FeaturesTransferonConferenceHangup"

/**
 * @brief 注释
 */
#define kszFeaturesUnhold "FeaturesUnhold"

/**
 * @brief 注释
 */
#define kszFeaturesUnmute "FeaturesUnmute"

/**
 * @brief 注释
 */
#define kszFeaturesUseLogo "FeaturesUseLogo"

/**
 * @brief 注释
 */
#define kszFeaturesUseOutboundinDialog "FeaturesUseOutboundinDialog"

/**
 * @brief 注释
 */
#define kszFeaturesVoiceMailTone "FeaturesVoiceMailTone"

/**
 * @brief 注释
 */
#define kszFeaturesXMLBlockInCalling "FeaturesXMLBlockInCalling"

/**
 * @brief 注释
 */
#define kszFeaturesXMLSIPNotify "FeaturesXMLSIPNotify"

/**
 * @brief 拍叉簧时间
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/user.ini
 *      section | Features
 *          key | FlashDuration
 *      default | -1
 */
#define kszFlashDuration "FlashDuration"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.flash_url_dsskey_led.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | FlashURLDsskey
 *      default | 1
 */
#define kszFlashURLDsskey "FlashURLDsskey"

/**
 * @brief ForbbinDelayTaskAction
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.taskaciton.delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ForbbinDelayTaskAction
 *      default | 0
 */
#define kszForbbinDelayTaskAction "ForbbinDelayTaskAction"

/**
 * @brief ForbidActionURI
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_uri.enable
 *         path | /config/user/user.ini
 *      section | ActionURI
 *          key | ForbidActionURI
 *      default | 0
 */
#define kszForbidActionURI "ForbidActionURI"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward_to_blf.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IsForbidFWDIncoming
 *      default | 1
 */
#define kszForbidBLFFWDIncoming "ForbidBLFFWDIncoming"

/**
 * @brief UI是否禁用wifi
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.wifi.bforbid
 *         path | /config/user/user.ini
 *      section | wifi
 *          key | bForbid
 *      default | 0
 */
#define kszForbidWifi "ForbidWifi"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.enable
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | ForbidZero
 *      default | 1
 */
#define kszForbidZeroSwitch "ForbidZeroSwitch"

/**
 * @brief forbidden refuse code
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.refusecode.nforbiddenrefusecode
 *         path | /config/user/user.ini
 *      section | RefuseCode
 *          key | nForbiddenRefuseCode
 *      default | 403
 */
#define kszForbiddenRefuseCode "ForbiddenRefuseCode"

/**
 * @brief 禁止通过预拨号界面进入测试模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.forbiddentestmode
 *      default | 0
 */
#define kszForbiddenTestMode "ForbiddenTestMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.unregister_account_block_out.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | ForbiddenUnregisterdAccount
 *      default | 0
 */
#define kszForbiddenUnregisterdAccount "ForbiddenUnregisterdAccount"

/**
 * @brief 配置开关控制按下Callpark键时，是否强制直接呼出
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.call_out_value.enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | bForceCallParkDirectCallOut
 *      default | 0
 */
#define kszForceCallParkDirectCallOut "ForceCallParkDirectCallOut"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_fail_busy_tone.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | EnableForceToPlayBusytone
 *      default | 1
 */
#define kszForceToPlayBusytoneAfterCallFailed "ForceToPlayBusytone"

/**
 * @brief ForceVoiceRing
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | force.voice.ring_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nForceVoiceRing
 *      default | ""
 */
#define kszForceVoiceRing "ForceVoiceRing"

/**
 * @brief Fortinet 特制DND同步
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.dnd_sync.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | FortinetDnd
 *      default | 0
 */
#define kszFortinetDnd "FortinetDnd"

/**
 * @brief Fortinet 特制Record
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.fortinetrecord
 *         path | /config/user/user.ini
 *      section | Features
 *          key | FortinetRecord
 *      default | 0
 */
#define kszFortinetRecord "FortinetRecord"

/**
 * @brief 注释
 */
#define kszForwardAllowance "ForwardAllowance"

/**
 * @brief 屏蔽历史记录forward组
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward_calllog.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ForwardCallLog
 *      default | 1
 */
#define kszForwardCallLog "ForwardCallLog"
#define kszForwardCallLogForbidden "ForwardCallLog"

/**
 * @brief ForwardEmergencyNumberList
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward.emergency.authorized_number
 *         path | /config/user/user.ini
 *      section | FWDEmergency
 *          key | FWD_Authorized_Numbers
 *      default | ""
 *          reg | {maxlength:511}
 */
#define kszForwardEmergencyNumberList "ForwardEmergencyNumberList"

/**
 * @brief ForwardEmergencySwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward.emergency.enable
 *         path | /config/user/user.ini
 *      section | FWDEmergency
 *          key | Enable
 *      default | 0
 */
#define kszForwardEmergencySwitch "ForwardEmergencySwitch"

/**
 * @brief 本地FWD是否同步服务器
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward.feature_key_sync.enable
 *         path | /config/user/user.ini
 *      section | Forward
 *          key | bForwardFeatureKeySync
 *      default | ""
 */
#define kszForwardFeatureKeySync "ForwardFeatureKeySync"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.fwd.allow
 *         path | /config/user/user.ini
 *      section | Forbidden
 *          key | FWD
 *      default | 0
 */
#define kszForwardForbidden "ForwardForbidden"

/**
 * @brief ForwardForbiddenInRinging
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.fwd.forbidden_in_ringing
 *         path | /config/user/user.ini
 *      section | Forbidden
 *          key | FwdInRinging
 *      default | 0
 */
#define kszForwardForbiddenInRinging "ForwardForbiddenInRinging"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.international.enable
 *         path | /config/user/user.ini
 *      section | Forward International
 *          key | Enable
 *      default | 1
 */
#define kszForwardInternationalEnable "ForwardInternationalEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.forbbiden_prefix
 *         path | /config/user/user.ini
 *      section | Forward International
 *          key | Number
 *      default | 00
 */
#define kszForwardInternationalNumber "ForwardInternationalNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.fwd_mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | FWDMode
 *      default | 0
 */
#define kszForwardMode "ForwardMode"

/**
 * @brief FWD同步开启的情况下是否在本地处理FWD（转移）
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward.feature_key_sync.local_processing.enable
 *         path | /config/user/user.ini
 *      section | Forward
 *          key | bSyncLocalProcess
 *      default | 0
 */
#define kszForwardSyncLocalProcess "ForwardSyncLocalProcess"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.custom_forward_type
 *       t49-m7 | phone_settings.custom_forward_type
 *         path | /config/user/user.ini
 *      section | Forward
 *          key | LastType
 *      default | 1
 */
#define kszForwardTypeLastSaved "ForwardTypeLastSaved"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.fullscreen_local_visible
 *         path | /config/user/user.ini
 *      section | FullScreen
 *          key | bLocalViewVisible
 *      default | ""
 */
#define kszFullScreenLocalViewSwitch "FullScreenLocalViewSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.fullscreen.escreenmodel
 *         path | /config/user/user.ini
 *      section | FullScreen
 *          key | eScreenModel
 *      default | 0
 */
#define kszFullScreenModelType "FullScreenModelType"

/**
 * @brief FwdCallByChannelKey
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.fwdcallbychannelkey
 *         path | /config/user/user.ini
 *      section | Features
 *          key | FwdCallByChannelKey
 *      default | 0
 */
#define kszFwdCallByChannelKey "FwdCallByChannelKey"

/**
 * @brief G726长度反转
 */
#define kszG726Reversed "G726Reversed"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.group_pickup_code
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | GPickupCode
 *      default | ""
 *          reg | {exclude:':;<>[]^?%%',maxlength:32}
 */
#define kszGPickupCode "GPickupCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.group_pickup_enable
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | GPickup
 *      default | 0
 */
#define kszGPickupSwitch "GPickupSwitch"

/**
 * @brief GbGabBookEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.gab.enable
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | GabBookEnable
 *      default | 0
 */
#define kszGbGabBookEnable "GbGabBookEnable"

/**
 * @brief GbPabBookEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.pab.enable
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | PabBookEnable
 *      default | 0
 */
#define kszGbPabBookEnable "GbPabBookEnable"

/**
 * @brief GbPabMatchCalling
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.pab.match_in_calling.enable
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | PabMatchCalling
 *      default | 0
 */
#define kszGbPabMatchCalling "GbPabMatchCalling"

/**
 * @brief GbPabMatchDailing
 */
#define kszGbPabMatchDailing "GbPabMatchDailing"

/**
 * @brief GbRetainSearchFilter
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.gab.retain_search_filter
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | RetainSearchFilter
 *      default | 0
 */
#define kszGbRetainSearchFilter "GbRetainSearchFilter"

/**
 * @brief GbSoupEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.enable
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | SoupEnable
 *      default | 0
 */
#define kszGbSoupEnable "GbSoupEnable"

/**
 * @brief GbSoupPsw
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.password
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | SoupPsw
 *      default | ""
 *      encrypt | True
 */
#define kszGbSoupPsw "GbSoupPsw"

/**
 * @brief GbSoupServer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.service_url
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | SoupServer
 *      default | ""
 */
#define kszGbSoupServer "GbSoupServer"

/**
 * @brief GbSoupUser
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gb.sopi.username
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | SoupUser
 *      default | ""
 */
#define kszGbSoupUser "GbSoupUser"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.generate_user_level_password.enable
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | GenerateMACPsw
 *      default | ""
 */
#define kszGenerateMACPsw "GenerateMACPsw"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.get_balance.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | GetBalanceEnable
 *      default | 0
 */
#define kszGetBalanceEnable "GetBalanceEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.group_park_code
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | GroupParkCode
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszGroupCallParkCode "GroupCallParkCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.group_enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | bEnabledGroupCallPark
 *      default | 0
 */
#define kszGroupCallParkSwitch "GroupCallParkSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.group_listen_in_talking_enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | GroupListenInTalkingEnable
 *      default | 1
 */
#define kszGroupListenInTalkingEnable "GroupListenInTalkingEnable"

/**
 * @brief H323协议开关：控制账号及ip账号
 */
#define kszH323ProtocolSwitch "H323ProtocolSwitch"

/**
 * @brief H323全局开关：控制整个功能
 */
#define kszH323Switch "H323Switch"

/**
 * @brief HSAutoAnswer
 */
#define kszHSAutoAnswer "HSAutoAnswer"

/**
 * @brief HSAutoIntercom
 */
#define kszHSAutoIntercom "HSAutoIntercom"

/**
 * @brief HSColorScheme
 */
#define kszHSColorScheme "HSColorScheme"

/**
 * @brief HSConfirmationTone
 */
#define kszHSConfirmationTone "HSConfirmationTone"

/**
 * @brief HSDateFormat
 */
#define kszHSDateFormat "HSDateFormat"

/**
 * @brief HSDispBackLightInCharger
 */
#define kszHSDispBackLightInCharger "HSDispBackLightInCharger"

/**
 * @brief HSDispBackLightOutofCharger
 */
#define kszHSDispBackLightOutofCharger "HSDispBackLightOutofCharger"

/**
 * @brief HSEcoMode
 */
#define kszHSEcoMode "HSEcoMode"

/**
 * @brief HSInputMethod
 */
#define kszHSInputMethod "HSInputMethod"

/**
 * @brief HSKeypadLight
 */
#define kszHSKeypadLight "HSKeypadLight"

/**
 * @brief HSKeypadTone
 */
#define kszHSKeypadTone "HSKeypadTone"

/**
 * @brief HSLanguage
 */
#define kszHSLanguage "HSLanguage"

/**
 * @brief HSLowBatteryTone
 */
#define kszHSLowBatteryTone "HSLowBatteryTone"

/**
 * @brief HSMissCallNotifyLight
 */
#define kszHSMissCallNotifyLight "HSMissCallNotifyLight"

/**
 * @brief HSScreenSaver
 */
#define kszHSScreenSaver "HSScreenSaver"

/**
 * @brief HSTimeFormat
 */
#define kszHSTimeFormat "HSTimeFormat"

/**
 * @brief HSVoiceMailNotifyLight
 */
#define kszHSVoiceMailNotifyLight "HSVoiceMailNotifyLight"

/**
 * @brief HSWallpaper
 */
#define kszHSWallpaper "HSWallpaper"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.trust_certificates
 *         path | /config/system/system.ini
 *      section | https
 *          key | TrustCertificates
 *      default | 1
 */
#define kszHTTPSTrustCertificates "HTTPSTrustCertificates"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.https.trustcustomcertificates
 *         path | /config/system/system.ini
 *      section | https
 *          key | TrustCustomCertificates
 *      default | 0
 */
#define kszHTTPSTrustCustomCertificates "HTTPSTrustCustomCertificates"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handfree_send
 *         path | /config/user/vpm.cfg
 *      section | Profile
 *          key | handfree_send
 *      default | 8
 *  t49-default | 0
 *          reg | {required:true,signed:true,range:[-50,50,0],maxlength:3}
 */
#define kszHandfreeSend "HandfreeSend"

/**
 * @brief HandsetCfgProcect
 */
#define kszHandsetCfgProcect "HandsetCfgProcect"

/**
 * @brief 手柄注册个数
 */
#define kszHandsetCount "HandsetCount"

/**
 * @brief Handset 是否注册
 */
#define kszHandsetEnable "HandsetEnable"

/**
 * @brief 可呼入的lines
 */
#define kszHandsetIncomingLines "HandsetIncomingLines"

/**
 * @brief 手柄注册掩码
 */
#define kszHandsetMask "HandsetMask"

/**
 * @brief HandsetModeEnabled
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.handset_mode.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HandsetModeEnabled
 *      default | 1
 */
#define kszHandsetPortEnable "HandsetModeEnabled"

/**
 * @brief Handset Name
 */
#define kszHandsetName "HandsetName"

/**
 * @brief Handset默认呼出账号
 */
#define kszHandsetOutGoingLine "HandsetOutGoingLine"

/**
 * @brief 可呼出的lines
 */
#define kszHandsetOutgongLines "HandsetOutgoingLines"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handset_send
 *         path | /config/user/vpm.cfg
 *      section | Profile
 *          key | handset_send
 *      default | 8
 *  t49-default | 0
 *          reg | {required:true,signed:true,range:[-50,50,0],maxlength:3}
 */
#define kszHandsetSend "HandsetSend"

/**
 * @brief 手柄触发的更新提示
 */
#define kszHandsetTip "HandsetTip"

/**
 * @brief 手柄触发的强制更新
 */
#define kszHandsetTrigger "HandsetTrigger"

/**
 * @brief 手柄Rom的URL
 */
#define kszHandsetURL "HandsetURL"

/**
 * @brief HandsetURL1
 */
#define kszHandsetURL1 "HandsetURL1"

/**
 * @brief HandsetURL2
 */
#define kszHandsetURL2 "HandsetURL2"

/**
 * @brief Handset Version
 */
#define kszHandsetVersion "HandsetVersion"

/**
 * @brief 通道开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.speaker_mode.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HandsfreeModeEnabled
 *      default | 1
 */
#define kszHandFreePortEnable "HandsfreeModeEnabled"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.transfer.hangupaftersuccesstrans
 *       t49-m7 | transfer.hang_up_after_success_trans
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | HangUpAfterSuccessTrans
 *      default | 1
 */
#define kszHangUpAfterSuccessTrans "HangUpAfterSuccessTrans"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.headsetkey_mode
 *         path | /config/user/user.ini
 *      section | HeadSet
 *          key | HeadSetKeyInTalk
 *      default | 1
 */
#define kszHeadSetKeyInTalkSwitch "HeadSetKeyInTalkSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.headset_cancel.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HeadsetAsCancel
 *      default | 0
 */
#define kszHeadsetAsCancel "HeadsetAsCancel"

/**
 * @brief HeadsetModeEnabled
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.headset_mode.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HeadsetModeEnabled
 *      default | 1
 */
#define kszHeadsetPortEnable "HeadsetModeEnabled"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.headset_prior
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HeadsetPrior
 *      default | 0
 */
#define kszHeadsetPriorSwitch "HeadsetPriorSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.headset_send
 *         path | /config/user/vpm.cfg
 *      section | Profile
 *          key | headset_send
 *      default | 8
 *  t49-default | 0
 *          reg | {required:true,signed:true,range:[-50,50,0],maxlength:3}
 */
#define kszHeadsetSend "HeadsetSend"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.headset_training
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | HeadSetTraining
 *      default | 0
 */
#define kszHeadsetTrainingSwitch "HeadsetTrainingSwitch"

/**
 * @brief 注释
 */
#define kszHiddenCustomFactoryConfig "HiddenCustomFactoryConfig"

/**
 * @brief 注释
 */
#define kszHiddenDNDActive "HiddenDNDActive"

/**
 * @brief 注释
 */
#define kszHiddenForwardActive "HiddenForwardActive"

/**
 * @brief 注释
 */
#define kszHiddenPhoneSetting "HiddenPhoneSetting"

/**
 * @brief 注释
 */
#define kszHiddenPushxml "HiddenPushxml"

/**
 * @brief 注释
 */
#define kszHiddenSettingNetwork "HiddenSettingNetwork"

/**
 * @brief 注释
 */
#define kszHiddenSettingTelnet "HiddenSettingTelnet"

/**
 * @brief 注释
 */
#define kszHiddenUserAgent "HiddenUserAgent"

/**
 * @brief HideCallin
 */
#define kszHideCallin "HideCallin"

/**
 * @brief 是否显示Default Line菜单
 */
#define kszHideDefaultLine "HideDefaultLineSet"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hide_feature_access_codes.enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | HideFeatureAccessCodes
 *      default | 0
 */
#define kszHideAccessCodes "HideFeatureAccessCodes"
#define kszHideFeatureAccessCodes "HideFeatureAccessCodes"

/**
 * @brief 是否显示Incoming Line菜单
 */
#define kszHideIncomingLine "HideIncomingLineSet"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hide_zero_touch_url.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HideZeroURL
 *      default | 0
 */
#define kszHideZeroURL "HideZeroURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.history.bblacklistshow
 *         path | /config/user/user.ini
 *      section | History
 *          key | bBlackListShow
 *      default | 1
 */
#define kszHistoryBlackListShowSwitch "HistoryBlackListShowSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.history.bsaved
 *         path | /config/user/user.ini
 *      section | History
 *          key | bSaved
 *      default | 1
 */
#define kszHistorySavedSwitch "HistorySavedSwitch"

/**
 * @brief 盲转CallPark之前是否Hold Session
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.send_hold_before_call_park.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HoldBeforeTransferCallpark
 *      default | 0
 */
#define kszHoldBeforeTransferCallpark "HoldBeforeTransferCallpark"

/**
 * @brief hold按键发送自定义的DTMF
 */
#define kszHoldDTMF "HoldDTMF"

/**
 * @brief 2路通话，hold、swap的显示规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.hold_or_swap.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HoldOrSwapMode
 *      default | 0
 */
#define kszHoldOrSwapMode "HoldOrSwapMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.hold_and_held_power_led_flash_enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_HoldHeld
 *      default | 0
 */
#define kszHoldPowerLightFlash "HoldPowerLightFlash"
#define kszPowerLightControlHold "HoldPowerLightFlash"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hold_trans_delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HoldTranDelay
 *      default | 0
 */
#define kszHoldTranDelay "HoldTranDelay"

/**
 * @brief hotdesk 需要清除的账号
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.clear_specified_account
 *         path | /config/user/user.ini
 *      section | HotDesking_Dsskey
 *          key | ClearAccount
 *      default | ""
 */
#define kszHotDeskingClearAccount "HotDeskingClearAccount"

/**
 * @brief HotDesking清除完账号信息的同时清除历史记录
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hotdesking_clear_calllog.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HotDeskingClearCallLog
 *      default | 0
 */
#define kszHotDeskingClearCallLog "HotDeskingClearCallLog"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.dsskey_outbound_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_Dsskey
 *          key | Outbound
 *      default | 1
 */
#define kszHotDeskingOutbound "HotDeskingOutbound"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.dsskey_password_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_Dsskey
 *          key | Password
 *      default | 1
 */
#define kszHotDeskingPassword "HotDeskingPassword"

/**
 * @brief HotDeskingPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.hot_desking_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HotDeskingPasswordIME
 *      default | abc
 */
#define kszHotDeskingPasswordIME "HotDeskingPasswordIME"

/**
 * @brief HotDeskingRegisterIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.hot_desking_register
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HotDeskingRegisterIME
 *      default | 2aB
 */
#define kszHotDeskingRegisterIME "HotDeskingRegisterIME"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.dsskey_register_name_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_Dsskey
 *          key | RegisterName
 *      default | 1
 */
#define kszHotDeskingRegisterName "HotDeskingRegisterName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.dsskey_sip_server_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_Dsskey
 *          key | SipServer
 *      default | 1
 */
#define kszHotDeskingSipServer "HotDeskingSipServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.dsskey_url
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | HotDeskingURL
 *      default | ""
 */
#define kszHotDeskingURL "HotDeskingURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.dsskey_username_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_Dsskey
 *          key | UserName
 *      default | 1
 */
#define kszHotDeskingUserName "HotDeskingUserName"

/**
 * @brief HotDeskingUserNameIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.hot_desking_username
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HotDeskingUserNameIME
 *      default | 2aB
 */
#define kszHotDeskingUserNameIME "HotDeskingUserNameIME"

/**
 * @brief Hoteling 提示登出多久没操作执行登出
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.log_out_prompt.duration
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | LogOutPromptDuration
 *      default | 300
 */
#define kszHotelingLogOutPromptDuration "HotelingLogOutPromptDuration"

/**
 * @brief Hoteling 登陆多久自动提示登出
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.log_out_prompt.time
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | LogOutPromptTime
 *      default | 0
 */
#define kszHotelingLogOutPromptTime "HotelingLogOutPromptTime"

/**
 * @brief Hoteling 登陆状态
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.user_status
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | LoginStatus
 *      default | 0
 */
#define kszHotelingLoginStatus "HotelingLoginStatus"

/**
 * @brief Hoteling 密码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.password
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | Password
 *      default | ""
 *      encrypt | True
 */
#define kszHotelingPassword "HotelingPassword"

/**
 * @brief HotelingPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.hoteling_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HotelingPasswordIME
 *      default | 1
 */
#define kszHotelingPasswordIME "HotelingPasswordIME"

/**
 * @brief Hoteling 持续登陆
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.persistent_login
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | PersistentLogin
 *      default | 1
 */
#define kszHotelingPersistentLogin "HotelingPersistentLogin"

/**
 * @brief Hoteling XMLBrowser 地址
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.http_request_server
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | ServerURL
 *      default | ""
 */
#define kszHotelingServer "HotelingServer"

/**
 * @brief HotelingUserIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.hoteling_username
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HotelingUserIME
 *      default | 1
 */
#define kszHotelingUserIME "HotelingUserIME"

/**
 * @brief Hoteling 用户名
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.user_name
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | UserName
 *      default | ""
 */
#define kszHotelingUserName "HotelingUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hotline_delay
 *         path | /config/user/user.ini
 *      section | Hotline
 *          key | nTimeDelay
 *      default | 2
 *  t49-default | 4
 *          reg | {required:true,range:[0,10,4],maxlength:2}
 */
#define kszHotlineDelay "HotlineDelay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hotline_number
 *         path | /config/user/user.ini
 *      section | Hotline
 *          key | strNumber
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszHotlineNumber "HotlineNumber"

/**
 * @brief 通话中放回底座是否挂断通话
 */
#define kszHandsetEndCallOnHook "HsEndCallOnHook"

/**
 * @brief HsInfoReportType
 */
#define kszHsInfoReportType "HsInfoReportType"

/**
 * @brief 解除手柄升级对OEM的限制，可升级到指定的特制版本
 */
#define kszHsUpgradeOEMUnLimit "HsUpgradeOEMUnLimit"

/**
 * @brief HttpConnectTimeOut
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.attempt_expired_time
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | TimeOut
 *      default | 10
 */
#define kszHttpDownLoadTimeOut "HttpConnectTimeOut"

/**
 * @brief 注释 config重复定义
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.port.http
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | nPort
 *      default | 80
 */
#define kszHttpServerPort "HttpServerPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.http_enable
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | bEnable
 *      default | 1
 */
#define kszHttpServerSwitch "HttpServerSwitch"

/**
 * @brief 0 put 1 post
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.custom.upload_method
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | HttpUploadMethod
 *      default | 0
 */
#define kszHttpUploadMethod "HttpUploadMethod"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.http.user_agent
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | strUserAgent
 *      default | ""
 */
#define kszHttpUserAgent "HttpUserAgent"

/**
 * @brief HttpUserAgentTransfer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.http.user_agent_transfertype
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | HttpUserAgentTransfer
 *      default | 1
 */
#define kszHttpUserAgentTransfer "HttpUserAgentTransfer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.cn_validation
 *         path | /config/system/system.ini
 *      section | https
 *          key | CNValidation
 *      default | 0
 */
#define kszHttpsCNValidation "HttpsCNValidation"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.ca_cert
 *         path | /config/system/system.ini
 *      section | https
 *          key | CaCert
 *      default | 0
 */
#define kszHttpsCaCert "HttpsCaCert"

/**
 * @brief HttpsConnectTimeOut
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.attempt_expired_time
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | TimeOut
 *      default | 10
 */
#define kszHttpsDownLoadTimeOut "HttpsConnectTimeOut"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.dev_cert
 *         path | /config/system/system.ini
 *      section | https
 *          key | DevCert
 *      default | 0
 */
#define kszHttpsDevCert "HttpsDevCert"

/**
 * @brief HttpsSSLCipherList
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.tls_cipher_list
 *         path | /config/system/system.ini
 *      section | https
 *          key | SSLCipherList
 *      default | ""
 */
#define kszHttpsSSLCipherList "HttpsSSLCipherList"

/**
 * @brief 注释 config重复定义
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.port.https
 *         path | /config/system/system.ini
 *      section | HttpsServer
 *          key | nPort
 *      default | 443
 */
#define kszHttpsServerPort "HttpsServerPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.https_enable
 *         path | /config/system/system.ini
 *      section | HttpsServer
 *          key | bEnable
 *      default | 0
 */
#define kszHttpsServerSwitch "HttpsServerSwitch"

/**
 * @brief 注释
 */
#define kszHuaweiSoftwareVersion "HuaweiSoftwareVersion"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.striframedata
 *         path | /config/user/user.ini
 *      section | Features
 *          key | strIFrameData
 *      default | ""
 */
#define kszIFrameData "IFrameData"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.wui.features.ilbc_enabled
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ILBC_enabled
 *      default | 1
 */
#define kszILBCEnable "ILBCEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.dialing
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IMEType
 *      default | 1
 */
#define kszIMEInDialUI "IMEInDialUI"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | action_url.is_ume
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | ISUME
 *      default | 0
 */
#define kszISUME "ISUME"

/**
 * @brief Idle 是否显示时钟
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.idle_clock_display.enable
 *      default | 1
 */
#define kszIdleClockDisplay "IdleClockDisplay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.calllog.bidleinremotecalllog
 *         path | /config/system/system.ini
 *      section | CallLog
 *          key | bIdleInRemoteCallLog
 *      default | ""
 */
#define kszIdleInRemoteCallLog "IdleInRemoteCallLog"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.factory_pwd_enable
 *         path | /config/user/user.ini
 *      section | reset
 *          key | IdleResetNeedPwd
 *      default | 0
 */
#define kszIdleResetNeedPwd "IdleResetNeedPwd"

/**
 * @brief idle文本颜色 0：白色，1：黑色
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.idle_text.color
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | IdleTextColor
 *      default | 0
 */
#define kszIdleTextColor "IdleTextColor"

/**
 * @brief multi paging 忽略DND
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.receive.ignore_dnd.priority
 *         path | /config/user/MulticastPage.cfg
 *      section | ReceivePriority
 *          key | IgnoreDNDPriority
 *      default | 0
 */
#define kszIgnoreDNDPriority "IgnoreDNDPriority"
#define kszMulticastIPIgnorePriority "IgnoreDNDPriority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.inactive_backlight_level
 *         path | /config/user/user.ini
 *      section | Backlight
 *          key | nInactiveLevel
 *      default | 1
 */
#define kszInactiveLevel "InactiveLevel"

/**
 * @brief IncomingBandwidth
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.downlink_bandwidth
 *      default | ""
 */
#define kszIncomingBandwidth "IncomingBandwidth"

/**
 * @brief ignor incoming call
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.ignore_incoming_call.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IncomingCallIgnor
 *      default | 0
 */
#define kszIncomingCallIgnor "IncomingCallIgnor"

/**
 * @brief 呼出时, 新来电的优先级
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.incoming_call_when_dialing.priority
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IncomingCallPriorityWhenDial
 *      default | 1
 */
#define kszIncomingCallPriorityWhenDial "IncomingCallPriorityWhenDial"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.blf_visual_enable
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | Indication_BLFPickup
 *      default | 0
 */
#define kszIndicationBLFPickup "IndicationBLFPickup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_completion.call_back.on_code
 *         path | /config/user/user.ini
 *      section | Features
 *          key | InitiateCallback
 *      default | 0
 */
#define kszInitiateCallback "InitiateCallback"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.hebrew_input_method.all_edit_enable
 *         path | /config/system/system.ini
 *      section | Language
 *          key | IsInstallHebrewForAllEdit
 *      default | 0
 */
#define kszInstallHebrewForAllEdit "InstallHebrewForAllEdit"

/**
 * @brief 智能穿透
 */
#define kszIntelligentPenetrationEnable "IntelligentPenetrationEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.inter_digit_time
 *         path | /config/user/user.ini
 *      section | Features
 *          key | nInterDigitTime
 *      default | 4
 *          reg | {required:true,range:[1,14,4],maxlength:2}
 */
#define kszInterDigitTime "InterDigitTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.allow
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | bAllowed
 *      default | 1
 */
#define kszIntercomAllowance "IntercomAllowance"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.barge
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | bIntercomBargeEnable
 *      default | 1
 */
#define kszIntercomBargeSwitch "IntercomBargeSwitch"

/**
 * @brief IntercomCode
 */
#define kszIntercomCode "IntercomCode"

/**
 * @brief intercom fac code
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.feature_access_code
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | IntercomFeatureAccessCode
 *      default | ""
 */
#define kszIntercomFeatureAccessCode "IntercomFeatureAccessCode"

/**
 * @brief IntercomHeadsetPrior
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.headset_prior.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AutoIntercomToHeadset
 *      default | 1
 */
#define kszIntercomHeadsetPrior "IntercomHeadsetPrior"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.led.enable
 *         path | /config/user/user.ini
 *      section | Ergocom
 *          key | IsIntercomLed
 *      default | 0
 */
#define kszIntercomLedEnable "IntercomLedEnable"

/**
 * @brief intercom mode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.mode
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | IntercomMode
 *      default | 0
 */
#define kszIntercomMode "IntercomMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.mute
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | bIntercomMuteEnable
 *      default | 1
 */
#define kszIntercomMuteSwitch "IntercomMuteSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.tone
 *         path | /config/user/user.ini
 *      section | Intercom
 *          key | bIntercomToneEnable
 *      default | 1
 */
#define kszIntercomToneSwitch "IntercomToneSwitch"

/**
 * @brief 注释
 *
 * Interval Report 定时发送的周期
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_interval_period
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_interval_period
 *      default | 20
 *          reg | {required:true,range:[5,20,20],maxlength:2}
 */
#define kszIntervalPeriod "IntervalPeriod"
#define kszRTCPIntervalPeriod "IntervalPeriod"

/**
 * @brief 注释
 *
 * Interval Report开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr.interval_report.enable
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | bIntervalReportEnable
 *      default | 0
 */
#define kszIntervalReportSwitch "IntervalReportSwitch"
#define kszRTCPIntervalReport "IntervalReportSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.end_call_net_disconnect.enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | InvalidNetEndCall
 *      default | 0
 */
#define kszInvalidNetEndCall "InvalidNetEndCall"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.17.sip_server.1.transport_type
 *      default | 1
 */
#define kszIpCallTransportSwitch "IpCallTransportType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.linekey_call_with_default_account
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bAutoChangeDefaultAccount
 *      default | 0
 */
#define kszIsAutoChangeDefaultAccount "IsAutoChangeDefaultAccount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf_pickup_only_send_code
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IsBLFPickupOnlySendCode
 *      default | 0
 */
#define kszIsBLFPickupOnlySendCode "IsBLFPickupOnlySendCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.disp_incall_to_info
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | DispInCallToInfo
 *      default | 0
 */
#define kszIsDispInCallToInfo "IsDispInCallToInfo"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.setup_wizard.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SetupWizard
 *      default | ""
 */
#define kszIsEnableSetupWizard "IsEnableSetupWizard"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.hotdesking.ishotdeskaccount
 *         path | /config/user/voip/sipAccount0.cfg
 *      section | HotDesking
 *          key | IsHotDeskAccount
 *      default | 0
 */
#define kszIsHotDeskAccount "IsHotDeskAccount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | joyce_setting.enable
 *         path | /config/user/user.ini
 *      section | Joyce
 *          key | IsJoyce
 *      default | 0
 */
#define kszIsJoyce "IsJoyce"

/**
 * @brief 网络会议创建后是否需要发送hold消息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.send_hold_after_network_conf.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bNeedHoldAferNetworkConf
 *      default | 1
 *  t49-default | 0
 */
#define kszIsNeedHoldAferNetworkConf "IsNeedHoldAferNetworkConf"

/**
 * @brief Linekey长度过长是否滚动
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.roll_dsskey_label.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bScroolLineKey
 *      default | 0
 */
#define kszIsScroolLineKey "IsScroolLineKey"

/**
 * @brief 配置变更是否弹窗提示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_config_update_tips.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bShowConfigUpdateTips
 *      default | 1
 */
#define kszIsShowConfigUpdateTips "IsShowConfigUpdateTips"

/**
 * @brief 是否在通话时显示 controltip
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_gesture_tips_method
 *      default | 1
 */
#define kszIsShowControlTips "IsShowControlTips"

/**
 * @brief 彩屏Idle界面是否显示客户Logo
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_logo.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IsShowLogo
 *      default | 0
 */
#define kszIsShowLogo "IsShowLogo"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phone_lock.is_lock
 *         path | /config/user/user.ini
 *      section | Phone_Lock
 *          key | Is_lock
 *      default | 1
 */
#define kszIslock "Islock"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.jib.adaptive
 *         path | /config/user/vpm.cfg
 *      section | Jitter
 *          key | Adaptive
 *      default | 1
 */
#define kszJitterAdaptive "JitterAdaptive"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.jib.max
 *         path | /config/user/vpm.cfg
 *      section | Jitter
 *          key | Max
 *      default | 300
 *          reg | {required:true,range:[60,300,240],maxlength:3}
 */
#define kszJitterMax "JitterMax"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.jib.min
 *         path | /config/user/vpm.cfg
 *      section | Jitter
 *          key | Min
 *      default | 60
 *          reg | {required:true,range:[60,300,60],maxlength:3}
 */
#define kszJitterMin "JitterMin"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.jib.normal
 *         path | /config/user/vpm.cfg
 *      section | Jitter
 *          key | Nominal
 *      default | 120
 *          reg | {required:true,range:[60,300,120],maxlength:3}
 */
#define kszJitterNominal "JitterNominal"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.key_as_send
 *         path | /config/user/user.ini
 *      section | KeyAsSend
 *          key | eType
 *      default | 1
 */
#define kszKeyAsSend "KeyAsSendSwitch"
#define kszQuickDialOnDirAndHistory "KeyAsSendSwitch"

/**
 * @brief KOIP在Queue等待的号码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.koipqueuenumber
 *         path | /config/user/user.ini
 *      section | Features
 *          key | KoipQueueNumber
 *      default | ""
 */
#define kszKoipQueueNumber "KoipQueueNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.lcd_logo.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LCD_Logo
 *      default | 1
 */
#define kszLCDLogo "LCDLogo"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.call_out_lookup
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bLookup4CallOut
 *      default | 1
 */
#define kszLDAPCallOutLookup "LDAPCallOutLookup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.connect_expires
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nConnectTimeOut
 *      default | 5
 */
#define kszLDAPConnectTimeOut "LDAPConnectTimeOut"

/**
 * @brief 指定MAXHINTS限定的是返回的电话数量还是号码数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.hint_type
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nHintType
 *      default | 1
 */
#define kszLDAPHintType "LDAPHintType"

/**
 * @brief 注释
 */
#define kszLDAPLookupforCallout "LDAPLookupforCallout"

/**
 * @brief 注释
 */
#define kszLDAPLookupforIncomingCall "LDAPLookupforIncomingCall"

/**
 * @brief 注释
 */
#define kszLDAPLookupforPreDial "LDAPLookupforPreDial"

/**
 * @brief 注释
 */
#define kszLDAPMaxhits "LDAPMaxhits"

/**
 * @brief 注释
 */
#define kszLDAPNameAttributes "LDAPNameAttributes"

/**
 * @brief 注释
 */
#define kszLDAPNumberAttributes "LDAPNumberAttributes"

/**
 * @brief LdapNumberDisplayMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.numb_display_mode
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | LdapNumberDisplayMode
 *      default | 0
 */
#define kszLdapNumberDisplayMode "LdapNumberDisplayMode"

/**
 * @brief 注释
 */
#define kszLDAPPassword "LDAPPassword"

/**
 * @brief LDAPPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.ldap.password.default_input_method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LDAPPasswordIME
 *      default | abc
 */
#define kszLDAPPasswordIME "LDAPPasswordIME"

/**
 * @brief 注释
 */
#define kszLDAPPort "LDAPPort"

/**
 * @brief 注释
 */
#define kszLDAPProtocol "LDAPProtocol"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.search_delay
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nSearchDelay
 *      default | 0
 */
#define kszLDAPSearchDelay "LDAPSearchDelay"

/**
 * @brief 注释
 */
#define kszLDAPServerAddress "LDAPServerAddress"

/**
 * @brief 注释
 */
#define kszLDAPSortingResults "LDAPSortingResults"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.incoming_call_special_search.enable
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bSpecialSearch
 *      default | 0
 */
#define kszLDAPSpecialSearch "LDAPSpecialSearch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.tls_mode
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | TLS_mode
 *      default | 0
 */
#define kszLDAPTLSMode "LDAPTLSMode"

/**
 * @brief LDAPUserIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.ldap.username.default_input_method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LDAPUserIME
 *      default | 2aB
 */
#define kszLDAPUserIME "LDAPUserIME"

/**
 * @brief 注释
 */
#define kszLDAPUserName "LDAPUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.base
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strBase
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPbase "LDAPbase"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.lldp.packet_interval
 *         path | /config/system/system.ini
 *      section | LLDP
 *          key | nPacketInterval
 *      default | 60
 *          reg | {required:true,range:[1,3600,60],maxlength:4}
 */
#define kszLLDPPacketInterval "LLDPPacketInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.lldp.enable
 *         path | /config/system/system.ini
 *      section | LLDP
 *          key | bEnableLLDP
 *      default | 0
 */
#define kszLLDPSwitch "LLDPSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | gui_lang.block_list
 *         path | /config/user/user.ini
 *      section | Language
 *          key | BlockList
 *      default | ""
 */
#define kszLangBlockList "LangBlockList"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | lang.gui
 *         path | /config/user/user.ini
 *      section | Language
 *          key | ActiveWebLanguage
 *      default | English
 */
#define kszCurrentLang "LanguageGuiLang"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | lang.wui
 *         path | /config/user/user.ini
 *      section | Lang
 *          key | WebLanguage
 *      default | English
 */
#define kszWebCurrentLang "LanguageWebLang"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.custom_headset_mode_status
 *       t49-m7 | phone_settings.custom_headset_mode_status
 *         path | /config/user/user.ini
 *      section | HeadSet
 *          key | LastHeadSet
 *      default | 0
 */
#define kszLastHeadSet "LastHeadSet"

/**
 * @brief 手柄最新的OEM版本号
 */
#define kszLastedHsOEMVer "LastedHsOEMVer"

/**
 * @brief 手柄最新的Rom版本号
 */
#define kszLastedHsRomVer "LastedHsRomVer"

/**
 * @brief 注释
 */
#define kszLDAPBase "LdapBase"
#define kszLdapBase "LdapBase"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.call_in_lookup
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bLookup4CallIn
 *      default | 1
 */
#define kszLDAPCallInLookup "LdapCallInSearchSwitch"
#define kszLdapCallInSearchSwitch "LdapCallInSearchSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.dial_lookup
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bLookup4Dial
 *      default | 1
 */
#define kszLDAPDialLookup "LdapCallOutSearchSwitch"
#define kszLdapCallOutSearchSwitch "LdapCallOutSearchSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.ldap.strdefaultfilter
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strDefaultFilter
 *      default | ""
 */
#define kszLDAPDefaultFilter "LdapDefaultFilter"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.ldap.benablenamefilter
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bEnableNameFilter
 *      default | 0
 */
#define kszLDAPEnableNameFilterFirst "LdapEnableNameFilterFirst"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.max_hits
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nMaxHits
 *      default | 50
 *          reg | {required:true,range:[1,32000,50],maxlength:5}
 */
#define kszLDAPMaxHits "LdapMaxHits"
#define kszLdapMaxHits "LdapMaxHits"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.customize_label
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strLdapName
 *      default | ""
 */
#define kszLDAPName "LdapName"
#define kszLdapName "LdapName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.name_attr
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strNameAttr
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPNameAttr "LdapNameAttr"
#define kszLdapNameAttr "LdapNameAttr"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.name_filter
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strNameFilter
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPNameFilter "LdapNameFilter"
#define kszLdapNameFilter "LdapNameFilter"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.numb_attr
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strNumbAttr
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPNumbAttr "LdapNumberAttr"
#define kszLdapNumberAttr "LdapNumberAttr"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.number_filter
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strNumberFilter
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPNumberFilter "LdapNumberFilter"
#define kszLdapNumberFilter "LdapNumberFilter"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.port
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nServerPort
 *      default | 389
 *          reg | {required:true,range:[1,65535,389],maxlength:5}
 */
#define kszLDAPport "LdapPort"
#define kszLdapPort "LdapPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.version
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nProtocolVersion
 *      default | 3
 */
#define kszLDAPversion "LdapProtocolVersion"
#define kszLdapProtocolVersion "LdapProtocolVersion"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.display_name
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strDisplayName
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPDisplayName "LdapResultDisplayName"
#define kszLdapResultDisplayName "LdapResultDisplayName"

/**
 * @brief LdapSearchType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.search_type
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | nSearchType
 *      default | 0
 */
#define kszLdapSearchType "LdapSearchType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.host
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strServerName
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPhost "LdapServer"
#define kszLdapServer "LdapServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.ldap_sort
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bSortResult
 *      default | 0
 */
#define kszLDAPSort "LdapSortSwitch"
#define kszLdapSortSwitch "LdapSortSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.enable
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bEnable
 *      default | 0
 */
#define kszLDAPSwitch "LdapSwitch"
#define kszLdapSwitch "LdapSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.user
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strUserName
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPuser "LdapUserName"
#define kszLdapUserName "LdapUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.password
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strUserPin
 *      default | ""
 *      encrypt | True
 *          reg | {maxlength:99}
 */
#define kszLDAPpswd "LdapUserPassword"
#define kszLdapUserPassword "LdapUserPassword"

/**
 * @brief 注释
 */
#define kszLevelEnableStun "Level_EnableStun"

/**
 * @brief 注释
 */
#define kszLevelSipTlsListenPort "Level_SipTlsListenPort"

/**
 * @brief 注释
 */
#define kszLicenseUploadLicense "LicenseUploadLicense"

/**
 * @brief LineKeyMaxNum
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.linekeymaxnum
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | LineKeyMaxNum
 *      default | /config/data/linekey.xml=28
 */
#define kszLineKeyMaxNum "LineKeyMaxNum"

/**
 * @brief 网页自己使用，不通过configMapper的定义
 */
#define kszListenAddress "ListenAddress"

/**
 * @brief 注释  config id 没人用
 */
#define kszListenAddressVolume "ListenAddress%dVolume"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.10.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress10
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress10IPAddress "ListenAddress10IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.10.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress10
 *          key | Label
 *      default | ""
 */
#define kszListenAddress10Label "ListenAddress10Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.1.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress1
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress1IPAddress "ListenAddress1IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.1.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress1
 *          key | Label
 *      default | ""
 */
#define kszListenAddress1Label "ListenAddress1Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.2.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress2
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress2IPAddress "ListenAddress2IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.2.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress2
 *          key | Label
 *      default | ""
 */
#define kszListenAddress2Label "ListenAddress2Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.3.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress3
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress3IPAddress "ListenAddress3IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.3.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress3
 *          key | Label
 *      default | ""
 */
#define kszListenAddress3Label "ListenAddress3Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.4.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress4
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress4IPAddress "ListenAddress4IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.4.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress4
 *          key | Label
 *      default | ""
 */
#define kszListenAddress4Label "ListenAddress4Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.5.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress5
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress5IPAddress "ListenAddress5IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.5.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress5
 *          key | Label
 *      default | ""
 */
#define kszListenAddress5Label "ListenAddress5Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.6.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress6
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress6IPAddress "ListenAddress6IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.6.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress6
 *          key | Label
 *      default | ""
 */
#define kszListenAddress6Label "ListenAddress6Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.7.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress7
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress7IPAddress "ListenAddress7IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.7.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress7
 *          key | Label
 *      default | ""
 */
#define kszListenAddress7Label "ListenAddress7Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.8.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress8
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress8IPAddress "ListenAddress8IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.8.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress8
 *          key | Label
 *      default | ""
 */
#define kszListenAddress8Label "ListenAddress8Label"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.9.ip_address
 *       t49-m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress9
 *          key | IPAddress
 *      default | ""
 */
#define kszListenAddress9IPAddress "ListenAddress9IPAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.9.label
 *       t49-m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress9
 *          key | Label
 *      default | ""
 */
#define kszListenAddress9Label "ListenAddress9Label"

/**
 * @brief 注释
 */
#define kszLocalAllContactsControl "LocalAllContactsControl"

/**
 * @brief LocalCallLogSyncEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.local_calllog.backup.enable
 *         path | /config/user/user.ini
 *      section | History
 *          key | bSyncEnable
 *      default | 0
 */
#define kszLocalCallLogSyncEnable "LocalCallLogSyncEnable"

/**
 * @brief 是否允许创建本地会议
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.conference.local.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LocalConferenceEnabled
 *      default | 1
 */
#define kszLocalConferenceEnabled "LocalConferenceEnabled"

/**
 * @brief 注释
 */
#define kszLocalContactDefaultIME "LocalContactDefaultIME"

/**
 * @brief 注释
 */
#define kszLocalContactList "LocalContactList"

/**
 * @brief LocalDirSyncEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.local_contact.backup.enable
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | bSyncEnable
 *      default | 0
 */
#define kszLocalDirSyncEnable "LocalDirSyncEnable"

/**
 * @brief LocalDirSyncPath
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.local_contact.backup.path
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | strSyncPath
 *      default | ""
 */
#define kszLocalDirSyncPath "LocalDirSyncPath"

/**
 * @brief LocalDirectoryDisplayname
 */
#define kszLocalDirectoryDisplayname "LocalDirectoryDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_local_dtmf_tone_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PLAY_LOCAL_DTMF_TONE
 *      default | 0
 */
#define kszLocalDtmfTone "LocalDtmfTone"

/**
 * @brief 注释
 */
#define kszLocalImportExportContacts "LocalImportExportContacts"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_log.level
 *         path | /config/system/system.ini
 *      section | LocalLog
 *          key | level
 *      default | 3
 */
#define kszLocalLogLevel "LocalLogLevel"

/**
 * @brief LocalLogMaxFileSize
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_log.max_file_size
 *         path | /config/system/system.ini
 *      section | LocalLog
 *          key | max_file_size
 *      default | 1024
 */
#define kszLocalLogMaxFileSize "LocalLogMaxFileSize"

/**
 * @brief LocalLogSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_log.enable
 *         path | /config/system/system.ini
 *      section | LocalLog
 *          key | bEnable
 *      default | 1
 */
#define kszLocalLogSwitch "LocalLogSwitch"

/**
 * @brief 历史记录同步路径
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.local_calllog.backup.path
 *         path | /config/user/user.ini
 *      section | History
 *          key | strSyncPath
 *      default | ""
 */
#define kszLocalLogSyncPath "LocalLogSyncPath"

/**
 * @brief 注释
 */
#define kszLocalOptContacts "LocalOptContacts"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.date_format
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | eDateFormat
 *      default | 0
 */
#define kszDateFormatType "LocalTimeDateFormatType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.summer_time
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | iDSTType
 *      default | 0
 */
#define kszDaylightSaving "LocalTimeDaylightSaving"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.dhcp_time
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | bEnableDHCPTime
 *      default | 0
 */
#define DhcpTimeSwitch "LocalTimeDhcpTimeSwitch"
#define kszDhcpTimeSwitch "LocalTimeDhcpTimeSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.ntp_server1
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | strPrimaryNTPServer
 *      default | ""
 *          reg | {required:true,address:true,maxlength:99}
 */
#define kszNtpServer1 "LocalTimeServer1"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.ntp_server2
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | strSecondaryNTPServer
 *      default | ""
 *          reg | {address:true,maxlength:99}
 */
#define kszNtpServer2 "LocalTimeServer2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.time_format
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | eTimeFormat
 *      default | 0
 */
#define kszTimeFormatType "LocalTimeTimeFormatType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.manual_time_enable
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | bNTPEnable
 *      default | 0
 */
#define kszTimeType "LocalTimeType"

/**
 * @brief 注释
 */
#define kszPhoneLocalConfiguration "Local_Configuration"

/**
 * @brief 注释
 */
#define kszLocalbtnExportCSV "LocalbtnExportCSV"

/**
 * @brief 注释
 */
#define kszLocalbtnExportXML "LocalbtnExportXML"

/**
 * @brief 注释
 */
#define kszLocalbtnImportCSV "LocalbtnImportCSV"

/**
 * @brief 注释
 */
#define kszLocalbtnImportXML "LocalbtnImportXML"

/**
 * @brief LockDownEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.provider_lock.enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | bEnableProviderLock
 *      default | 0
 */
#define kszLockDownEnable "LockDownEnable"
#define kszLockLevel "LockDownEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.phone_lock.enable
 *         path | /config/user/user.ini
 *      section | Phone_Lock
 *          key | Enable
 *      default | ""
 */
#define kszKeyGuardEnable "LockEnable"
#define kszLockEnable "LockEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.phone_lock.lock_time_out
 *         path | /config/user/user.ini
 *      section | Phone_Lock
 *          key | lock_time_out
 *      default | 0
 *          reg | {required:true,range:[0,3600,0],maxlength:4}
 */
#define kszLockTimeout "LockTimeout"

/**
 * @brief 历史记录上传延迟时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.local_calllog.write_delay.terminated
 *         path | /config/user/user.ini
 *      section | History
 *          key | nSyncTimer
 *      default | 60
 */
#define kszLogUploadTime "LogUploadTime"

/**
 * @brief 注释
 */
#define kszWebLoginAddress "LoginAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.user_name
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | LoginName
 *      default | ""
 */
#define kszLoginName "LoginName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.acd.user_password
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ACD
 *          key | LoginPassword
 *      default | ""
 */
#define kszLoginPassword "LoginPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_exit_logon_wizard.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LoginWizardAutoExit
 *      default | 0
 */
#define kszLoginWizardAutoExit "LoginWizardAutoExit"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.lcd_logo.wait_time
 *         path | /config/user/user.ini
 *      section | Logo
 *          key | DelayTime
 *      default | 10
 */
#define kszLogoDelayTime "LogoDelayTime"

/**
 * @brief LogoShowTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.lcd_logo.show_time
 *         path | /config/user/user.ini
 *      section | Logo
 *          key | ShowTime
 *      default | 3
 */
#define kszLogoShowTime "LogoShowTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.logon_wizard
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | LogonWizard
 *      default | 0
 */
#define kszLogonWizard "LogonWizard"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.logon_wizard_forever_wait
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LogonWizardNeverTimeout
 *      default | 0
 */
#define kszLogonWizardNeverTimeout "LogonWizardNeverTimeout"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.lync.autodeviceupdateenable
 *         path | /config/user/user.ini
 *      section | Lync
 *          key | AutoDeviceUpdateEnable
 *      default | ""
 */
#define kszLyncAutoDeviceUpdateEnable "LyncAutoDeviceUpdateEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sfb.presence.away_time
 *         path | /config/user/user.ini
 *      section | Lync
 *          key | AwayTime
 *      default | ""
 */
#define kszLyncAwayTime "LyncAwayTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.lync.conferenceenable
 *         path | /config/user/user.ini
 *      section | Lync
 *          key | ConferenceEnable
 *      default | ""
 */
#define kszLyncConferenceEnable "LyncConferenceEnable"

/**
 * @brief MDNSActive
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.mdns.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnableMdns
 *      default | 0
 */
#define kszMDNSActive "MDNSActive"

/**
 * @brief MTSWACDEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.acd.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bACDEnable
 *      default | 0
 */
#define kszMTSWACDEnable "MTSWACDEnable"

/**
 * @brief metaswitch 总开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bMTSWActive
 *      default | 0
 */
#define kszMTSWActive "MTSWActive"
#define kszMetaSwitchActive "MTSWActive"

/**
 * @brief Metaswitch历史记录开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.call_list.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bCallListEnable
 *      default | 0
 */
#define kszMTSWCallListEnable "MTSWCallListEnable"

/**
 * @brief MTSWCallListSearchEnable
 */
#define kszMTSWCallListSearchEnable "MTSWCallListSearchEnable"

/**
 * @brief MTSWCallParkCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.park_code
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | CallParkCode
 *      default | ""
 */
#define kszMTSWCallParkCode "MTSWCallParkCode"

/**
 * @brief CommPortal web的URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bCommportalActive
 *      default | 0
 */
#define kszMTSWCommportalEnable "MTSWCommportalEnable"

/**
 * @brief 访问CommPortal web的密码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.server.password
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | strCommportalPW
 *      default | ""
 *      encrypt | True
 */
#define kszMTSWCommportalPassword "MTSWCommportalPassword"

/**
 * @brief CommPortal web的URL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.server.url
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | strCommportalUrl
 *      default | ""
 */
#define kszMTSWCommportalServer "MTSWCommportalServer"

/**
 * @brief 访问CommPortal web的用户名
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.server.username
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | strCommportalDN
 *      default | ""
 */
#define kszMTSWCommportalUser "MTSWCommportalUser"

/**
 * @brief MTSWContactsGroupEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.group.contacts.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bContactsGroupEnable
 *      default | 1
 */
#define kszMTSWContactLists "MTSWContactsGroupEnable"

/**
 * @brief MTSWContactsGroupLabel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.group.contacts.label
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | strContactsGroupLabel
 *      default | Contacts
 */
#define kszMTSWContactName "MTSWContactsGroupLabel"

/**
 * @brief MTSWContactsSearchEnable
 */
#define kszMTSWContactsSearchEnable "MTSWContactsSearchEnable"

/**
 * @brief MTSWDirEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bMTSWDirEnable
 *      default | 0
 */
#define kszMTSWContactsEnable "MTSWDirEnable"
#define kszMTSWDirEnable "MTSWDirEnable"

/**
 * @brief MTSWDirUpdateInterVal
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.update_interval
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | UpdateInterVal
 *      default | 60
 */
#define kszMTSWDirUpdateInterVal "MTSWDirUpdateInterVal"

/**
 * @brief MTSWExtensionsGroupEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.group.extensions.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bExtensionsGroupEnable
 *      default | 1
 */
#define kszMTSWExtentions "MTSWExtensionsGroupEnable"

/**
 * @brief MTSWExtensionsGroupLabel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.group.extensions.label
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | strExtensionsGroupLabel
 *      default | Extentions
 */
#define kszMTSWExtentionName "MTSWExtensionsGroupLabel"

/**
 * @brief Intercom Push-To-Talk功能
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.ptt_mode.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bPushToTalkMode
 *      default | 0
 */
#define kszMTSWIntercomPTTEnable "MTSWIntercomPTTEnable"
#define kszPushToTalkMode "MTSWIntercomPTTEnable"

/**
 * @brief Metaswitch 是否已鉴权
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.metaswitch.blogined
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bLogined
 *      default | 0
 */
#define kszMTSWIsLogined "MTSWIsLogined"

/**
 * @brief KeepAlive查询时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.keep_alive_interval_time
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | nKeepAliveTimer
 *      default | 30
 */
#define kszMTSWKeepAliveTimer "MTSWKeepAliveTimer"

/**
 * @brief Metaswitch 鉴权登陆模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.login_mode
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | nLoginMode
 *      default | 0
 */
#define kszMTSWLoginMode "MTSWLoginMode"

/**
 * @brief 是否允许配置logout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.logout.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bLogoutEnable
 *      default | 0
 */
#define kszMTSWLogoutEnable "MTSWLogoutEnable"

/**
 * @brief MTSWMLGHGroupEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.group.mlhgs.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bMLGHGroupEnable
 *      default | 1
 */
#define kszMTSWMLHGS "MTSWMLGHGroupEnable"

/**
 * @brief MTSWMLHGGroupLabel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.contacts.group.mlhgs.label
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | strMLHGGroupLabel
 *      default | MLHGS
 */
#define kszMTSWMLHGName "MTSWMLHGGroupLabel"

/**
 * @brief MessagesList 开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.message.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bMessageEnable
 *      default | 0
 */
#define kszMTSWMessageEnable "MTSWMessageEnable"
#define kszMTSWVMListEnable "MTSWMessageEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.mail_power_led_flash_enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_Mail
 *      default | 1
 */
#define kszMailPowerLightFlash "MailPowerLightFlash"
#define kszPowerLightControlMessage "MailPowerLightFlash"

/**
 * @brief MainMenuFontSize
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.main_menu.font.size
 *         path | /config/user/user.ini
 *      section | webstyle
 *          key | MainMenuFontSize
 *      default | ""
 */
#define kszMainMenuFontSize "MainMenuFontSize"

/**
 * @brief ManualDay
 */
#define kszManualDay "ManualDay"

/**
 * @brief ManualHour
 */
#define kszManualHour "ManualHour"

/**
 * @brief ManualMinute
 */
#define kszManualMinute "ManualMinute"

/**
 * @brief ManualMonth
 */
#define kszManualMonth "ManualMonth"

/**
 * @brief ManualSecond
 */
#define kszManualSecond "ManualSecond"

/**
 * @brief ManualYear
 */
#define kszManualYear "ManualYear"

/**
 * @brief 注释
 */
#define kszMasterGetRandomPath "MasterGetRandomPath"

/**
 * @brief 注释
 */
#define kszMasterLoginPath "MasterLoginPath"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.max_calls
 *         path | /config/user/user.ini
 *      section | Features
 *          key | Max_Calls
 *      default | 3
 */
#define kszxMaxCallCount "MaxCallCount"

/**
 * @brief 所下载的文件最大值
 */
#define kszMaxDownloadSize "MaxDownloadSize"

/**
 * @brief base可连接的最大手柄数
 */
#define kszMaxNumberOfHandset "MaxNumberOfHandset"

/**
 * @brief MediaSnifferTimeout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.early_media.rtp_sniffer.timeout
 *         path | /config/user/vpm.cfg
 *      section | RTP
 *          key | rtp_sniffer_timeout
 *      default | 0
 */
#define kszMediaSnifferTimeout "MediaSnifferTimeout"

/**
 * @brief 注释
 */
#define kszMemoryKeyType "MemoryKey.%d.DKtype"

/**
 * @brief 注释
 */
#define kszMemoryKeyHistoryType "MemoryKey.%d.HistoryType"

/**
 * @brief 注释
 */
#define kszMemoryKeyLine "MemoryKey.%d.LineId"

/**
 * @brief 注释
 */
#define kszMemoryKeyValue "MemoryKey.%d.Value"

/**
 * @brief 注释
 */
#define kszMemoryKeyLabel "MemoryKey.%d.label"

/**
 * @brief 注释
 */
#define kszMemoryKeyPickupValue "MemoryKey.%d.pickupValue"

/**
 * @brief 注释
 */
#define kszMemoryKeyXmlPhoneBook "MemoryKey.%d.xmlPhoneBook"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.memorylight.dnd
 *         path | /config/user/vpPhone.ini
 *      section | MemoryLight
 *          key | DND
 *      default | -1
 */
#define kszMemoryLightDND "MemoryLightDND"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.memorylight.fwd
 *         path | /config/user/vpPhone.ini
 *      section | MemoryLight
 *          key | FWD
 *      default | -1
 */
#define kszMemoryLightFWD "MemoryLightFWD"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.message.voicenumber%%d
 *         path | /config/user/user.ini
 *      section | Message
 *          key | VoiceNumber%%d
 *      default | ""
 */
#define kszMessageVoiceNumber "MessageVoiceNumber.%d"

/**
 * @brief 注释
 *
 * misscall闪灯
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.missed_call_power_led_flash.enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_MissCall
 *      default | 0
 */
#define kszMissCallPowerLightFlash "MissCallPowerLightFlash"
#define kszPowerLightControlMissCall "MissCallPowerLightFlash"

/**
 * @brief 需要记录的MissCall的类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.missed_calllog_type
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | MissCallType
 *      default | 0
 */
#define kszMissCallType "MissCallType"

/**
 * @brief Mobile VM Number  config id 没人用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.mobile_vm_number
 *       t49-m7 | features.mobile_vm_number
 *         path | /config/user/user.ini
 *      section | Message
 *          key | MobileVMNumber
 *      default | ""
 */
#define kszMobileVMNumber "MobileVMNumber"

/**
 * @brief 音量加大
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.receive.enhance_volume
 *         path | /config/user/MulticastPage.cfg
 *      section | audio
 *          key | EnhanceVolume
 *      default | 0
 */
#define kszMulticastEnhanceVolume "MulticastEnhanceVolume"

/**
 * @brief 注释
 */
#define kszMulticastIPConfig "MulticastIPConfig"

/**
 * @brief 注释
 */
#define kszMulticastIPPagePriorityActive "MulticastIPPagePriorityActive"

/**
 * @brief 注释
 */
#define kszMulticastIPReceivePriority "MulticastIPReceivePriority"

/**
 * @brief MulticastPagingList
 */
#define kszMulticastPagingList "MulticastPagingList"

/**
 * @brief 使用免提
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.receive.use_speaker
 *         path | /config/user/MulticastPage.cfg
 *      section | audio
 *          key | UseSpeaker
 *      default | 0
 */
#define kszMulticastUseSpeaker "MulticastUseSpeaker"

/**
 * @brief Handset Hold on Music 是否开启
 */
#define kszMusicOnHold "MusicOnHold"

/**
 * @brief 静音模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.keep_mute.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | MuteMode
 *      default | 0
 */
#define kszMuteMode "MuteMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.mute_power_led_flash_enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_Mute
 *      default | 0
 */
#define kszMutePowerLightFlash "MutePowerLightFlash"
#define kszPowerLightControlMute "MutePowerLightFlash"

/**
 * @brief 静音模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.mute_strategy.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | MuteStrategy
 *      default | 0
 */
#define kszMuteStrategy "MuteStrategy"

/**
 * @brief MutilBroadSoftLog
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.call_log.multiple_accounts.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | MultipleBsftlogEnable
 *      default | 0
 */
#define kszMutilBroadSoftLog "MutilBroadSoftLog"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.manual_ntp_srv_prior
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | NtpCfgPriorDhcp
 *      default | 0
 */
#define kszNTPPriority "NTPPriority"

/**
 * @brief NatICESwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ice.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | EnableICE
 *      default | 0
 */
#define kszNatICESwitch "NatICESwitch"

/**
 * @brief NatManualIP
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.static_nat.addr
 *         path | /config/system/system.ini
 *      section | Network
 *          key | StaticNATAddr
 *      default | ""
 */
#define kszNatManualIP "NatManualIP"

/**
 * @brief NatManualSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.static_nat.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | StaticNAT
 *      default | 0
 */
#define kszNatManualSwitch "NatManualSwitch"

/**
 * @brief NatSTUNPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_stun.port
 *         path | /config/system/system.ini
 *      section | Network
 *          key | STUNPort
 *      default | 3478
 */
#define kszNatSTUNPort "NatSTUNPort"

/**
 * @brief NatSTUNServer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_stun.server
 *         path | /config/system/system.ini
 *      section | Network
 *          key | STUNServer
 *      default | ""
 */
#define kszNatSTUNServer "NatSTUNServer"

/**
 * @brief NatSTUNSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_stun.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | EnableSTUN
 *      default | 0
 */
#define kszNatSTUNSwitch "NatSTUNSwitch"

/**
 * @brief NatTURNPassword
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_turn.password
 *         path | /config/system/system.ini
 *      section | Network
 *          key | TURNPassword
 *      default | ""
 *      encrypt | True
 */
#define kszNatTURNPassword "NatTURNPassword"

/**
 * @brief NatTURNPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_turn.port
 *         path | /config/system/system.ini
 *      section | Network
 *          key | TURNPort
 *      default | 3478
 */
#define kszNatTURNPort "NatTURNPort"

/**
 * @brief NatTURNServer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_turn.server
 *         path | /config/system/system.ini
 *      section | Network
 *          key | TURNServer
 *      default | ""
 */
#define kszNatTURNServer "NatTURNServer"

/**
 * @brief NatTURNSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_turn.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | EnableTURN
 *      default | 0
 */
#define kszNatTURNSwitch "NatTURNSwitch"

/**
 * @brief NatTURNUserName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_turn.username
 *         path | /config/system/system.ini
 *      section | Network
 *          key | TURNUsername
 *      default | ""
 */
#define kszNatTURNUserName "NatTURNUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phonesetting.manual_time
 *         path | /tmp/InitSave
 *      section | PhoneSetting
 *          key | Manual_Time
 *      default | 0
 */
#define kszNetManualTime "NetManualTime"

/**
 * @brief Bsft联系人下载页数配置，0表示不限制，1表示下载一页，2最多两页，类推
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.download_contact_page
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | PageCount
 *      default | 0
 */
#define kszNetWorkDirServerPageCount "NetWorkDirServerPageCount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.network.802_1x.ca_file_name
 *       t49-m7 | network.802_1x.ca_file_name
 *         path | /config/system/system.ini
 *      section | 802.1X
 *          key | CA_FileName
 *      default | ""
 */
#define kszNetwork802_1CA_FileName "Network802_1CA_FileName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.network.802_1x.dev_file_name
 *       t49-m7 | network.802_1x.dev_file_name
 *         path | /config/system/system.ini
 *      section | 802.1X
 *          key | Device_FileName
 *      default | ""
 */
#define kszNetwork802_1Device_FileName "Network802_1Device_FileName"

/**
 * @brief Network802_1XAnonymousIdentity
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.802_1x.anonymous_identity
 *         path | /config/system/system.ini
 *      section | 802.1x
 *          key | anonymous_identity
 *      default | ""
 */
#define kszNetwork802_1XAnonymousIdentity "Network802_1XAnonymousIdentity"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.802_1x.identity
 *         path | /config/system/system.ini
 *      section | 802.1X
 *          key | nIdentity
 *      default | ""
 *          reg | {exclude:'&',maxlength:32}
 */
#define ksz802Dot1xId "Network802_1XIdentity"
#define kszNetwork802_1XIdentity "Network802_1XIdentity"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.802_1x.md5_password
 *         path | /config/system/system.ini
 *      section | 802.1X
 *          key | strMD5Passwd
 *      default | ""
 *      encrypt | True
 *          reg | {exclude:'&',maxlength:32}
 */
#define ksz802Dot1xPassword "Network802_1XMD5Passwd"
#define kszNetwork802_1XMD5Passwd "Network802_1XMD5Passwd"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.802_1x.mode
 *         path | /config/system/system.ini
 *      section | 802.1X
 *          key | bEnable
 *      default | 0
 */
#define ksz802Dot1xMode "Network802_1XMode"
#define kszNetwork802_1XMode "Network802_1XMode"

/**
 * @brief Network802_1XProvisioningMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.802_1x.eap_fast_provision_mode
 *         path | /config/system/system.ini
 *      section | 802.1x
 *          key | provision_mode
 *      default | 0
 */
#define kszNetwork802_1XProvisioningMode "Network802_1XProvisioningMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.list.networkcallloglist
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkCalllogList
 *      default | ""
 */
#define kszNetworkCalllogGroupName "NetworkCalllogGroupName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.call_log.enable
 *       t49-m7 | bw_phonebook.call_log_enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bNetworkCallLogEnable
 *      default | 0
 */
#define kszBsftEnableCallLog "NetworkCalllogSwitch"
#define kszNetworkCalllogSwitch "NetworkCalllogSwitch"

/**
 * @brief NetworkCalllogUpdateRealtime
 */
#define kszBDCalllogUpdateRealtimeSwitch "NetworkCalllogUpdateRealtime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.common_displayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryCommonDisplayname
 *      default | ""
 */
#define kszNetworkDirectoryCommonDisplayname "NetworkDirectoryCommonDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.broadsoft.networkdirectorycustomdisplayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryCustomDisplayname
 *      default | Custom
 */
#define kszBsftGCustomName "NetworkDirectoryCustomDisplayname"

/**
 * @brief NetworkDirectoryDisplayname
 */
#define kszNetworkDirectoryDisplayname "NetworkDirectoryDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.enterprise_common_displayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryEnterpriseCommonDisplayname
 *      default | EnterpriseCommon
 *          reg | {maxlength:99}
 */
#define kszBsftECommonName "NetworkDirectoryEnterpriseCommonDisplayname"
#define kszNetworkDirectoryEnterpriseCommonName "NetworkDirectoryEnterpriseCommonDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.enterprise_displayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryEnterpriseDisplayname
 *      default | Enterprise
 *          reg | {maxlength:99}
 */
#define kszBsftEnterpriseName "NetworkDirectoryEnterpriseDisplayname"
#define kszNetworkDirectoryEnterpriseName "NetworkDirectoryEnterpriseDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.group_common_displayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryGroupCommonDisplayname
 *      default | GroupCommon
 *          reg | {maxlength:99}
 */
#define kszBsftGCommonName "NetworkDirectoryGroupCommonDisplayname"
#define kszNetworkDirectoryGroupCommonName "NetworkDirectoryGroupCommonDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.group_displayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryGroupDisplayname
 *      default | Group
 *          reg | {maxlength:99}
 */
#define kszBsftGroupName "NetworkDirectoryGroupDisplayname"
#define kszNetworkDirectoryGroupName "NetworkDirectoryGroupDisplayname"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.personal_displayname
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryPersonalDisplayname
 *      default | Personal
 *          reg | {maxlength:99}
 */
#define kszBsftPersonalName "NetworkDirectoryPersonalDisplayname"
#define kszNetworkDirectoryPersonalName "NetworkDirectoryPersonalDisplayname"

/**
 * @brief Broadworks Directory的开关.
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.directory.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bNetworkDirectoryEnable
 *      default | 0
 */
#define kszBsftEnableDirectory "NetworkDirectorySwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw_phonebook.type
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | NetworkDirectoryType
 *      default | 0
 */
#define kszBsftDirecoryType "NetworkDirectoryType"

/**
 * @brief NetworkEnableStaticNAT
 */
#define kszNetworkEnableStaticNAT "NetworkEnableStaticNAT"

/**
 * @brief NetworkH323MinTCPPort
 */
#define kszH323MinTCPPort "NetworkH323MinTCPPort"

/**
 * @brief NetworkH323MinUDPPort
 */
#define kszH323MinUDPPort "NetworkH323MinUDPPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ip_address_mode
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPAddressMode
 *      default | 2
 */
#define kszNetworkIPAddressMode "NetworkIPAddressMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_icmp_v6.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6AutoIPConf
 *      default | ""
 */
#define kszNetworkIPv6AutoIPConf "NetworkIPv6AutoIPConf"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_prefix
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6Prefix
 *      default | 64
 *          reg | {range:[0,128],maxlength:3}
 */
#define kszNetworkWanIPv6Prefix "NetworkIPv6Prefix"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_internet_port.type
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6WANType
 *      default | 0
 */
#define kszNetworkIPv6WANType "NetworkIPv6WANType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_internet_port.gateway
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6WanGateway
 *      default | ""
 *          reg | {required:true,ipv6:true,maxlength:99}
 */
#define kszNetworkIPv6WanGateway "NetworkIPv6WanGateway"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_internet_port.ip
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6WanIP
 *      default | ""
 *          reg | {required:true,ipv6:true,maxlength:99}
 */
#define kszNetworkWanIPv6WanIP "NetworkIPv6WanIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_primary_dns
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6WanPrimaryDNS
 *      default | ""
 *          reg | {ipv6:true,maxlength:99}
 */
#define kszNetworkIPv6WanPrimaryDNS "NetworkIPv6WanPrimaryDNS"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_secondary_dns
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6WanSecondaryDNS
 *      default | ""
 *          reg | {ipv6:true,maxlength:99}
 */
#define kszNetworkIPv6WanSecondaryDNS "NetworkIPv6WanSecondaryDNS"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.ipv6_static_dns_enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | IPv6StaticDNS
 *      default | 0
 */
#define kszNetworkIPv6WanStaticDNSEnable "NetworkIPv6WanStaticDNSEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.strdhcpclientend
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strDHCPClientEnd
 *      default | 10.0.0.100
 */
#define kszNetworkLanDHCPEndIP "NetworkLanDHCPEndIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.strdhcpclientbegin
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strDHCPClientBegin
 *      default | 10.0.0.100
 */
#define kszNetworkLanDHCPStartIP "NetworkLanDHCPStartIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.blandhcpserver
 *         path | /config/system/system.ini
 *      section | Network
 *          key | bLanDHCPServer
 *      default | 0
 */
#define kszNetworkLanEnableDHCP "NetworkLanEnableDHCP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.strlanip
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strLanIP
 *      default | 10.0.0.1
 */
#define kszNetworkLanRouterIP "NetworkLanRouterIP"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.span_to_pc_port
 *         path | /config/system/system.ini
 *      section | Network
 *          key | SpanToPCPort
 *      default | 0
 */
#define kszNetworkLanSpanToPCPort "NetworkLanSpanToPCPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.strlanmask
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strLanMask
 *      default | 255.255.255.0
 */
#define kszNetworkLanSubnetMask "NetworkLanSubnetMask"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.network.bbridgemode
 *         path | /config/system/system.ini
 *      section | Network
 *          key | bBridgeMode
 *      default | 1
 */
#define kszNetworkLanType "NetworkLanType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.port.max_rtpport
 *         path | /config/system/system.ini
 *      section | RTPPORT
 *          key | MaxRTPPort
 *      default | ""
 *          reg | {required:true,range:[1,65535],maxlength:5}
 */
#define kszMaxRTPPort "NetworkMaxRTPPort"
#define kszNetworkMaxRTPPort "NetworkMaxRTPPort"

/**
 * @brief NetworkMaxTCPPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.max_tcp_port
 *      default | 50499
 *          reg | {required:true,range:[1024,65535],maxlength:5}
 */
#define kszH323MaxTCPPort "NetworkMaxTCPPort"

/**
 * @brief NetworkMaxUDPPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.max_udp_port
 *      default | 50499
 *          reg | {required:true,range:[1024,65535],maxlength:5}
 */
#define kszH323MaxUDPPort "NetworkMaxUDPPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.port.min_rtpport
 *         path | /config/system/system.ini
 *      section | RTPPORT
 *          key | MinRTPPort
 *      default | ""
 *          reg | {required:true,range:[1,65535],maxlength:5}
 */
#define kszMinRTPPort "NetworkMinRTPPort"
#define kszNetworkMinRTPPort "NetworkMinRTPPort"

/**
 * @brief NetworkMinTCPPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.min_tcp_port
 *      default | 50000
 *          reg | {required:true,range:[1024,65535],maxlength:5}
 */
#define kszSipMinTCPPort "NetworkMinTCPPort"

/**
 * @brief Reserved Port
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.min_udp_port
 *      default | 50000
 *          reg | {required:true,range:[1024,65535],maxlength:5}
 */
#define kszSipMinUDPPort "NetworkMinUDPPort"

/**
 * @brief NetworkNATDynamicIPAdds
 */
#define kszNetworkNATDynamicIPAdds "NetworkNATDynamicIPAdds"

/**
 * @brief NetworkNATPubIPAdds
 */
#define kszNetworkNATPubIPAdds "NetworkNATPubIPAdds"

/**
 * @brief NetworkNATRequestAdds
 */
#define kszNetworkNATRequestAdds "NetworkNATRequestAdds"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.pc_port.enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | UsePcPort
 *      default | 1
 */
#define kszNetworkPCPortSwitch "NetworkPCPortSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.pppoe.password
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strPPPoEPin
 *      default | ""
 *      encrypt | True
 *          reg | {maxlength:99}
 */
#define kszNetworkPppoePin "NetworkPppoePin"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.pppoe.user
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strPPPoEUser
 *      default | ""
 *          reg | {required:true,maxlength:32}
 */
#define kszNetworkPppoeUser "NetworkPppoeUser"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.qos.audiotos
 *       t49-m7 | network.qos.rtptos
 *         path | /config/system/system.ini
 *      section | QoS
 *          key | nAudioQos
 *      default | 46
 *          reg | {required:true,range:[0,63],maxlength:2}
 */
#define kszNetworkRTPTOS "NetworkRTPTOS"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.reg_surge_prevention
 *         path | /config/system/system.ini
 *      section | REGSURGE
 *          key | RegSurgePrevention
 *      default | 0
 *          reg | {required:true,range:[0,60,0],maxlength:2}
 */
#define kszNetworkRegSurgePrevention "NetworkRegSurgePrevention"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.qos.signaltos
 *         path | /config/system/system.ini
 *      section | QoS
 *          key | nSipQoS
 *      default | 26
 *          reg | {required:true,range:[0,63],maxlength:2}
 */
#define kszNetworkSIGNALTOS "NetworkSIGNALTOS"

/**
 * @brief NetworkSIPHMaxTCPPort
 */
#define kszSipMaxTCPPort "NetworkSIPHMaxTCPPort"

/**
 * @brief NetworkSIPHMaxUDPPort
 */
#define kszSipMaxUDPPort "NetworkSIPHMaxUDPPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.static_dns_enable
 *         path | /config/system/system.ini
 *      section | Network
 *          key | bEnableStaticDNS
 *      default | 0
 */
#define kszNetworkWanStaticDNSEnable "NetworkWanStaticDNSEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.internet_port.gateway
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strWanGateway
 *      default | ""
 *          reg | {required:true,ipv4:true,maxlength:99}
 */
#define kszNetworkWanStaticGateWay "NetworkWanStaticGateWay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.internet_port.ip
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strWanIP
 *      default | ""
 *          reg | {required:true,ipv4:true,maxlength:99}
 */
#define kszNetworkWanStaticIp "NetworkWanStaticIp"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.internet_port.mask
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strWanMask
 *      default | ""
 *          reg | {required:true,mask:true,maxlength:99}
 */
#define kszNetworkWanStaticMask "NetworkWanStaticMask"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.primary_dns
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strWanPrimaryDNS
 *      default | ""
 *          reg | {ipv4:true,maxlength:99}
 */
#define kszNetworkWanStaticPriDns "NetworkWanStaticPriDns"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.secondary_dns
 *         path | /config/system/system.ini
 *      section | Network
 *          key | strWanSecondaryDNS
 *      default | ""
 *          reg | {ipv4:true,maxlength:99}
 */
#define kszNetworkWanStaticSecDns "NetworkWanStaticSecDns"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.internet_port.type
 *         path | /config/system/system.ini
 *      section | Network
 *          key | eWANType
 *      default | 0
 */
#define kszNetworkWanType "NetworkWanType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.web_server_type
 *         path | /config/system/system.ini
 *      section | Webserver Type
 *          key | WebType
 *      default | 1
 */
#define kszNetworkWebType "NetworkWebType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.port.http
 *         path | /config/system/system.ini
 *      section | HttpServer
 *          key | nPort
 *      default | 80
 *          reg | {required:true,range:[1,65535],maxlength:5}
 */
#define kszNetworkhttpPort "NetworkhttpPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.port.https
 *         path | /config/system/system.ini
 *      section | HttpsServer
 *          key | nPort
 *      default | 443
 *          reg | {required:true,range:[1,65535],maxlength:5}
 */
#define kszNetworkhttpsPort "NetworkhttpsPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.snmp.port
 *         path | /config/system/system.ini
 *      section | SNMP
 *          key | nPort
 *      default | ""
 *          reg | {required:true,range:[1,65535],maxlength:5}
 */
#define kszNetworksnmpPort "NetworksnmpPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.snmp.enable
 *         path | /config/system/system.ini
 *      section | SNMP
 *          key | bEnable
 *      default | 0
 */
#define kszNetworksnmpSwitch "NetworksnmpSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.snmp.trust_ip
 *         path | /config/system/system.ini
 *      section | SNMP
 *          key | strServerName
 *      default | ""
 *          reg | {multiple:{ip:true,splits:' ,'},maxlength:255}
 */
#define kszNetworksnmpTrustedAddress "NetworksnmpTrustedAddress"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.no_answer.timeout
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | Timeout
 *      default | 20
 */
#define kszForwardNoAnswerDelayTime "NoAnswerForwardDelayTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.no_answer.off_code
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | strOffCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszForwardNoAnswerOffCode "NoAnswerForwardOffCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.no_answer.on_code
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | strOnCode
 *      default | ""
 *          reg | {exclude:'<>!',maxlength:32}
 */
#define kszForwardNoAnswerOnCode "NoAnswerForwardOnCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.noanswerforward.nringcount
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | nRingCount
 *      default | 5
 */
#define kszNoAnswerForwardRingCount "NoAnswerForwardRingCount"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.no_answer.enable
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | bEnable
 *      default | 0
 */
#define kszForwardNoAnswerSwitch "NoAnswerForwardSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | forward.no_answer.target
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | strTarget
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszForwardNoAnswerTarget "NoAnswerForwardTarget"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.normal_refuse_code
 *         path | /config/user/user.ini
 *      section | RefuseCode
 *          key | nNormalRefuseCode
 *      default | 486
 */
#define kszNormalRefuseCode "NormalRefuseCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.send_hold_with_intercom.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | NotHoldByIntercomCall
 *      default | 0
 */
#define kszNotHoldByIntercomCall "NotHoldByIntercomCall"

/**
 * @brief NotLocalDirSortEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.sorted_alphabetically
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | Sort_Alphabetically
 *      default | 1
 */
#define kszNotLocalDirSortEnable "NotLocalDirSortEnable"

/**
 * @brief 是否通知 RPS 結果
 */
#define kszNotifyRPSResult "NotifyRPSResult"

/**
 * @brief 注释
 */
#define kszNumInEachPage "NumInEachPage"

/**
 * @brief talking状态下OK键的处理
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.ok_key.talking.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | OKMapStrategyInTalking
 *      default | 0
 */
#define kszOKMapStrategyInTalking "OKMapStrategyInTalking"

/**
 * @brief 注释
 */
#define kszOneTouchExport "OneTouchExport"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.on_hook_trans_enable
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | bOnhookAttendTranEnable
 *      default | 1
 */
#define kszOnhookAttendTranSwitch "OnhookAttendTranSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.blind_tran_on_hook_enable
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | bOnhookBlindTranEnable
 *      default | 1
 */
#define kszOnhookBlindTranSwitch "OnhookBlindTranSwitch"

/**
 * @brief VPN 连接状态
 */
#define kszOpenVpnStatus "OpenVpnLinkStatus"

/**
 * @brief OpenvpnStatus
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | tmp.openvpn.status
 *         path | /tmp/openvpn_status
 *      section | link_status
 *          key | link
 *      default | 0
 */
#define kszOpenvpnStatus "OpenvpnStatus"

/**
 * @brief OutgoingBandwidth
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.uplink_bandwidth
 *      default | ""
 */
#define kszOutgoingBandwidth "OutgoingBandwidth"

/**
 * @brief 注释
 */
#define kszPCPortActive "PCPortActive"

/**
 * @brief 注释
 */
#define kszPCPortConfig "PCPortConfig"

/**
 * @brief PSTN功能开关
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PSTNEnable
 *      default | 1
 */
#define kszPSTNEnable "PSTNEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.password_dial.length
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PswLength
 *      default | 0
 *          reg | {range:[0,32],maxlength:2}
 */
#define kszPSWDialLength "PSWDialLength"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.password_dial.prefix
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PswPrefix
 *      default | 60
 *          reg | {exclude:'&',maxlength:32}
 */
#define kszPSWDialPrefix "PSWDialPrefix"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.packetcapture.strclipfileprefix
 *         path | /config/user/user.ini
 *      section | PacketCapture
 *          key | strClipFilePrefix
 *      default | /tmp/log/data
 */
#define kszPacketCapClipFilePrefix "PacketCapClipFilePrefix"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | packet_capture.max_file_counts
 *         path | /config/user/user.ini
 *      section | PacketCapture
 *          key | nFileCount
 *      default | 5
 *  t49-default | 15
 *          reg | {required:true,range:[1,100,15],maxlength:3}
 */
#define kszPacketCapFileCount "PacketCapFileCount"

/**
 * @brief PagingAddress
 */
#define kszPagingAddress "PagingAddress"

/**
 * @brief PagingAddressChannel%%d
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.paging_address.%%d.channel
 *         path | /config/user/MulticastPage.cfg
 *      section | PagingAddress%%d
 *          key | Channel
 *      default | 0
 */
#define kszPagingAddressChannel "PagingAddressChannel%d"

/**
 * @brief PagingAddressIPAddress
 */
#define kszPagingListIPAddress "PagingAddressIPAddress"

/**
 * @brief Multicast Paging List
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.paging_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | PagingAddress%%d
 *          key | IPAddress
 *      default | ""
 */
#define kszPagingAddressIPAddress "PagingAddressIPAddress%d"

/**
 * @brief PagingAddressLabel
 */
#define kszPagingListLabel "PagingAddressLabel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.paging_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | PagingAddress%%d
 *          key | Label
 *      default | ""
 */
#define kszPagingAddressLabel "PagingAddressLabel%d"

/**
 * @brief PagingType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.paging_type
 *         path | /config/user/MulticastPage.cfg
 *      section | PagingType
 *          key | Type
 *      default | 0
 */
#define kszPagingType "PagingType"

/**
 * @brief ParkLineRestrictionEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.line_restriction.enable
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | ParkLineRestrictionEnable
 *      default | 0
 */
#define kszParkLineRestrictionEnable "ParkLineRestrictionEnable"

/**
 * @brief 表示digitmap对于按下Prefix的情况, 要不要生效
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.digitmap.apply_to.prefix_key
 *         path | /config/user/user.ini
 *      section | DigitMap
 *          key | DigitMapInPressDesskeyPrefix
 *      default | 1
 */
#define kszDigitMapInPressDesskeyPrefix "DigitMapInPressDesskeyPrefix"

/**
 * @brief 配置calllog界面下，分账号显示时各账号显示的方式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.bw_calllog.caller_name_type
 *         path | /config/user/user.ini
 *      section | Features
 *          key | NetworkCalllogCallerNameType
 *      default | 0
 */
#define kszNetworkCalllogCallerNameType "NetworkCalllogCallerNameType"

/**
 * @brief 配置call control界面下，分账号显示时各账号显示的方式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.bw_features.caller_name_type
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CallControlCallerNameType
 *      default | 1
 */
#define kszCallControlCallerNameType "CallControlCallerNameType"

/**
 * @brief blflist解析value方式类型
 */
#define kszParseBlflistValueType "ParseBlflistValueType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.codec
 *         path | /config/user/MulticastPage.cfg
 *      section | audio
 *          key | PayloadType
 *      default | G722
 */
#define kszPayloadType "PayloadType"

/**
 * @brief 注释
 */
#define kszPhoneACSPassword "PhoneACSPassword"

/**
 * @brief 注释
 */
#define kszPhoneACSURL "PhoneACSURL"

/**
 * @brief 注释
 */
#define kszPhoneACSUsername "PhoneACSUsername"

/**
 * @brief 注释
 */
#define kszPhoneAreacodeControl "PhoneAreacodeControl"

/**
 * @brief 注释
 */
#define kszPhoneAutoProvisionAccount "PhoneAutoProvisionAccount"

/**
 * @brief 注释
 */
#define kszPhoneAutoProvisionPassword "PhoneAutoProvisionPassword"

/**
 * @brief 注释
 */
#define kszPhoneAutopTime "PhoneAutopTime"

/**
 * @brief 注释
 */
#define kszPhoneBlockoutControl "PhoneBlockoutControl"

/**
 * @brief 注释
 */
#define kszPhoneCallControl "PhoneCallControl"

/**
 * @brief 注释
 */
#define kszPhoneCommonAESkey "PhoneCommonAESkey"

/**
 * @brief 注释
 */
#define kszPhoneCustomOption "PhoneCustomOption"

/**
 * @brief 注释
 */
#define kszPhoneCustomSoftKey "PhoneCustomSoftKey"

/**
 * @brief 注释
 */
#define kszPhoneDHCPOptionValue "PhoneDHCPOptionValue"

/**
 * @brief 注释
 */
#define kszPhoneDayofWeek "PhoneDayofWeek"

/**
 * @brief 注释
 */
#define kszPhoneDialnowControl "PhoneDialnowControl"

/**
 * @brief 注释
 */
#define kszPhoneEchoCancellation "PhoneEchoCancellation"

/**
 * @brief 注释
 */
#define kszPhoneEmergencySetting "PhoneEmergencySetting"

/**
 * @brief 注释
 */
#define kszPhoneEnablePeriodicInform "PhoneEnablePeriodicInform"

/**
 * @brief 注释
 */
#define kszPhoneEnableTR069 "PhoneEnableTR069"

/**
 * @brief PhoneExportCfgConfig
 */
#define kszPhoneExportCfgConfig "PhoneExportCfgConfig"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.version.firmware_version
 *         path | /phone/version
 *      section | version
 *          key | FIRMWARE_VERSION
 *      default | ""
 */
#define kszPhoneFirmware "PhoneFirmware"

/**
 * @brief 注释  config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.old_ver.hwver
 *       t49-m7 | phone_settings.hardware
 *         path | /tmp/InitSave
 *      section | OLD_VER
 *          key | HwVer
 *      default | Unknown
 */
#define kszOLD_VERHwVer "PhoneHardware"
#define kszPhoneHardware "PhoneHardware"

/**
 * @brief PhoneImportCfgConfig
 */
#define kszPhoneImportCfgConfig "PhoneImportCfgConfig"

/**
 * @brief 注释
 */
#define kszPhoneJitterBuffer "PhoneJitterBuffer"

/**
 * @brief 注释
 */
#define kszPhoneKeyboardLock "PhoneKeyboardLock"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.virtual_keyboard.enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | EnableVirtualKeyboard
 *      default | "0"
 */
#define kszPhoneVirtualKeyboardEnable "PhoneVirtualKeyboard"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.phone_lock.lock_key_type
 *         path | /config/user/user.ini
 *      section | Phone_Lock
 *          key | Lock
 *      default | 0
 */
#define kszKeyGuardType "PhoneLock"
#define kszPhoneLockType "PhoneLock"

/**
 * @brief 注释
 */
#define kszPhoneLockEnable "PhoneLockEnable"

/**
 * @brief 注释
 */
#define kszPhoneLockTimeout "PhoneLockTimeout"

/**
 * @brief 注释
 */
#define kszPhoneMACOrientedAESkey "PhoneMACOrientedAESkey"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.version.modelname
 *         path | /phone/version
 *      section | version
 *          key | modelName
 *      default | ""
 */
#define kszPhoneModel "PhoneModel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.oem_firmware_update.custom
 *         path | /config/system/system.ini
 *      section | firmware_upgrade
 *          key | transit_to
 *      default | ""
 */
#define kszPhoneOEM "PhoneOEM"

/**
 * @brief 注释
 */
#define kszPhoneOptConfigfile "PhoneOptConfigfile"

/**
 * @brief 注释
 */
#define kszPhoneOptFactoryConfigfile "PhoneOptFactoryConfigfile"

/**
 * @brief 注释
 */
#define kszPhonePNPActive "PhonePNPActive"

/**
 * @brief 注释
 */
#define kszPhonePcapTrace "PhonePcapTrace"

/**
 * @brief 注释
 */
#define kszPhonePeriodicInformInterval "PhonePeriodicInformInterval"

/**
 * @brief 注释
 */
#define kszPhonePersonalRing "PhonePersonalRing"

/**
 * @brief 注释
 */
#define kszPhonePoweronActive "PhonePoweronActive"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.date.date
 *         path | /phone/version
 *      section | date
 *          key | date
 *      default | ""
 */
#define kszPhoneProductId "PhoneProductId"

/**
 * @brief 注释
 */
#define kszPhoneRebootDevice "PhoneRebootDevice"

/**
 * @brief 注释
 */
#define kszPhoneRepeatInterval "PhoneRepeatInterval"

/**
 * @brief 注释
 */
#define kszPhoneRepeatlyActive "PhoneRepeatlyActive"

/**
 * @brief 注释
 */
#define kszPhoneReplaceRuleControl "PhoneReplaceRuleControl"

/**
 * @brief 注释
 */
#define kszPhoneRequestPassword "PhoneRequestPassword"

/**
 * @brief 注释
 */
#define kszPhoneRequestUsername "PhoneRequestUsername"

/**
 * @brief 注释
 */
#define kszPhoneResetLocalConfig "PhoneResetLocalConfig"

/**
 * @brief 注释
 */
#define kszPhoneResetToFactory "PhoneResetToFactory"

/**
 * @brief 注释
 */
#define kszPhoneServerURL "PhoneServerURL"

/**
 * @brief 注释
 */
#define kszPhoneSoftKeySelect "PhoneSoftKeySelect"

/**
 * @brief 注释
 */
#define kszPhoneSoftKeyType "PhoneSoftKeyType"

/**
 * @brief 注释
 */
#define kszPhoneSyslog "PhoneSyslog"

/**
 * @brief 注释
 */
#define kszPhoneSystemlogLevel "PhoneSystemlogLevel"

/**
 * @brief 注释
 */
#define kszPhoneTR069Header "PhoneTR069Header"

/**
 * @brief 注释
 */
#define kszPhoneToneItems "PhoneToneItems"

/**
 * @brief 注释
 */
#define kszPhoneUnlockPIN "PhoneUnlockPIN"

/**
 * @brief 注释
 */
#define kszPhoneUpgradeFirmware "PhoneUpgradeFirmware"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.date.date
 *         path | /phone/version
 *      section | date
 *          key | date
 *      default | ""
 */
#define kszPhoneVersion "PhoneVersion"

/**
 * @brief 注释
 */
#define kszPhoneVersionInfo "PhoneVersionInfo"

/**
 * @brief 注释
 */
#define kszPhoneWaitTime "PhoneWaitTime"

/**
 * @brief 注释
 */
#define kszPhoneWeeklyActive "PhoneWeeklyActive"

/**
 * @brief 注释
 */
#define kszPhoneZeroSwitch "PhoneZeroSwitch"

/**
 * @brief 注释
 */
#define kszPhonebtnAutopNow "PhonebtnAutopNow"

/**
 * @brief 注释
 */
#define kszPhonedhcpActive "PhonedhcpActive"

/**
 * @brief 屏保背景图片多长时间换一次
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.picture_change_interval
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | PicChangeInterval
 *      default | 60
 */
#define kszPicChangeInterval "PicChangeInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.blf_audio_enable
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | AllowBlfRing
 *      default | 0
 */
#define kszPickupAllowBlfRing "PickupAllowBlfRing"

/**
 * @brief Pickup显示方式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup_display.method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PickupDisplayMethod
 *      default | 0
 */
#define kszPickupDisplayMethod "PickupDisplayMethod"

/**
 * @brief PIN码
 */
#define kszPinCode "PinCode"

/**
 * @brief ping的目标地址  config id 没人用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.diagnostics.strpingtarget
 *         path | /config/user/user.ini
 *      section | Diagnostics
 *          key | strPingTarget
 *      default | ""
 */
#define kszPingTarget "PingTarget"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sfb.pre_dial_tone.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayDialToneInPreDialEnable
 *      default | 1
 */
#define kszPlayDialToneInPreDialEnable "PlayDialToneInPreDialEnable"

/**
 * @brief Held提示音开始时间(s)
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_held_tone.delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayHeldToneDelay
 *      default | 30
 */
#define kszPlayHeldToneDelay "PlayHeldToneDelay"

/**
 * @brief Held提示音间隔(s)
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_held_tone.interval
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayHeldToneInterval
 *      default | 60
 */
#define kszPlayHeldToneInterval "PlayHeldToneInterval"

/**
 * @brief 是否播放Held提示音
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_held_tone.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayHeldTone
 *      default | 0
 */
#define kszPlayHeldToneSwitch "PlayHeldToneSwitch"

/**
 * @brief Hold提示音开始时间(s)
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_hold_tone.delay
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayHoldToneDelay
 *      default | 30
 *          reg | {required:true,range:[3,3600,30],maxlength:4}
 */
#define kszPlayHoldToneDelay "PlayHoldToneDelay"

/**
 * @brief Hold提示音间隔(s)
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_hold_tone.interval
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayHoldToneInterval
 *      default | 30
 */
#define kszPlayHoldToneInterval "PlayHoldToneInterval"

/**
 * @brief 是否播放Hold提示音
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.play_hold_tone.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayHoldTone
 *      default | 1
 */
#define kszPlayHoldToneSwitch "PlayHoldToneSwitch"

/**
 * @brief 183时是否播放本地铃声
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.playlocalonprovresponsesdp
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlayLocalOnProvResponseSdp
 *      default | 1
 */
#define kszPlayLocalOnProvResponseSdp "PlayLocalOnProvResponseSdp"

/**
 * @brief YUK BT PLCM Phonebook
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.plcm_phone_services.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlcmPhoneEnable
 *      default | 0
 */
#define kszPlcmPhoneEnable "PlcmPhoneEnable"

/**
 * @brief PLCM Phonebook更新间隔
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.plcm_phone_services_update_interval
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PlcmPhoneUpdateInterval
 *      default | 60
 */
#define kszPlcmPhoneUpdateInterval "PlcmPhoneUpdateInterval"

/**
 * @brief 注释
 */
#define kszPowerLedHold "PowerLedHold"

/**
 * @brief 注释
 */
#define kszPowerLedIdle "PowerLedIdle"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.idle_talk_power_led_flash_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PowerLedIdle&Talk
 *      default | 0
 */
#define kszPowerLedIdle_Talk "PowerLedIdle&Talk"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.powerledmail
 *         path | /config/user/user.ini
 *      section | Features
 *          key | PowerLedMail
 *      default | 1
 */
#define kszPowerLedMail "PowerLedMail"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.powerled.powerledmute
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLedMute
 *      default | 0
 */
#define kszPowerLedMute "PowerLedMute"

/**
 * @brief PowerSavingDarkTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.power_led_flash.off_time
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | off_time
 *      default | 3000
 */
#define kszPowerSavingDarkTime "PowerSavingDarkTime"

/**
 * @brief PowerSavingEXPEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.expansion_module.enable
 *      default | 1
 */
#define kszPowerSavingEXPEnable "PowerSavingEXPEnable"

/**
 * @brief PowerSavingEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.enable
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | bEnable
 *      default | 1
 */
#define kszPowerSavingEnable "PowerSavingEnable"

/**
 * @brief PowerSavingIntelligentMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.intelligent_mode
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | intelligent_mode
 *      default | 1
 */
#define kszPowerSavingIntelligentMode "PowerSavingIntelligentMode"

/**
 * @brief PowerSavingLightTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.power_led_flash.on_time
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | on_time
 *      default | 500
 */
#define kszPowerSavingLightTime "PowerSavingLightTime"

/**
 * @brief PowerSavingOffTimeout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.off_hour.idle_timeout
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | off_timeout
 *      default | 10
 *          reg | {required:true,range:[1,10,10],maxlength:2}
 */
#define kszPowerSavingOffTimeout "PowerSavingOffTimeout"

/**
 * @brief PowerSavingOfficeFriday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.friday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Friday
 *      default | 7,19
 *  t49-default | 7,12
 */
#define kszPowerSavingOfficeFriday "PowerSavingOfficeFriday"

/**
 * @brief PowerSavingOfficeMonday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.monday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Monday
 *      default | 7,19
 *  t49-default | 7,12
 */
#define kszPowerSavingOfficeMonday "PowerSavingOfficeMonday"

/**
 * @brief PowerSavingOfficeSaturday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.saturday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Saturday
 *      default | 7,7
 *  t49-default | 7,0
 */
#define kszPowerSavingOfficeSaturday "PowerSavingOfficeSaturday"

/**
 * @brief PowerSavingOfficeSunday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.sunday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Sunday
 *      default | 7,7
 *  t49-default | 7,0
 */
#define kszPowerSavingOfficeSunday "PowerSavingOfficeSunday"

/**
 * @brief PowerSavingOfficeThursday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.thursday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Thursday
 *      default | 7,19
 *  t49-default | 7,12
 */
#define kszPowerSavingOfficeThursday "PowerSavingOfficeThursday"

/**
 * @brief PowerSavingOfficeTimeout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.idle_timeout
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_timeout
 *      default | 360
 *  t49-default | 120
 *          reg | {required:true,range:[1,240,120],maxlength:3}
 */
#define kszPowerSavingOfficeTimeout "PowerSavingOfficeTimeout"

/**
 * @brief PowerSavingOfficeTuesday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.tuesday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Tuesday
 *      default | 7,19
 *  t49-default | 7,12
 */
#define kszPowerSavingOfficeTuesday "PowerSavingOfficeTuesday"

/**
 * @brief PowerSavingOfficeWednesday
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.office_hour.wednesday
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | office_Wednesday
 *      default | 7,19
 *  t49-default | 7,12
 */
#define kszPowerSavingOfficeWednesday "PowerSavingOfficeWednesday"

/**
 * @brief PowerSavingSrceenProtectTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.powersaving.srceenprotecttime
 *       t49-m7 | features.power_saving.screen_protect_time
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | SrceenProtectTime
 *      default | 4320
 */
#define kszPowerSavingSrceenProtectTime "PowerSavingSrceenProtectTime"

/**
 * @brief PowerSavingUserInputTimeout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.power_saving.user_input_ext.idle_timeout
 *         path | /config/user/user.ini
 *      section | PowerSaving
 *          key | user_input_ext_timeout
 *      default | 10
 *          reg | {required:true,range:[1,30,10],maxlength:2}
 */
#define kszPowerSavingUserInputTimeout "PowerSavingUserInputTimeout"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.predial_autodial
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bPredialAutodial
 *      default | 0
 */
#define kszPredialAutodialSwitch "PredialAutodial"

/**
 * @brief 注释
 */
#define kszPresence "Presence"

/**
 * @brief 话机是否无条件同步到桌面端状态
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xmpp.change_presence.force_manual.enable
 *         path | /config/user/user.ini
 *      section | Presence
 *          key | PresenceForceSyncManualOpt
 *      default | 1
 */
#define kszPresenceForceSyncManualOpt "PresenceForceSyncManualOpt"

/**
 * @brief 桌面端是否无条件同步到话机状态
 */
#define kszPresenceForceSyncOpt "PresenceForceSyncOpt"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.uc_username
 *         path | /config/user/user.ini
 *      section | Presence
 *          key | strUserID
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszPresenceJID "PresenceJID"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.uc_password
 *         path | /config/user/user.ini
 *      section | Presence
 *          key | strPassWord
 *      default | ""
 *      encrypt | True
 *          reg | {maxlength:99}
 */
#define kszPresencePassword "PresencePassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phonesetting.pressdigittime
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | PressDigitTime
 *      default | 1
 */
#define kszPressDigitTime "PressDigitTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account.primaryaw
 *         path | /config//user/user.ini
 *      section | Account
 *          key | PrimaryAW
 *      default | ""
 */
#define kszPrimaryAW "PrimaryAW"

/**
 * @brief PrivacyButtonSwitch
 */
#define kszPrivacyButtonSwitch "PrivacyButtonSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.product_name
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | ProductName
 *      default | ""
 */
#define kszProductName "ProductName"

/**
 * @brief ProfileCNG
 */
#define kszProfileCNG "ProfileCNG"

/**
 * @brief voice&Eco Setting
 */
#define kszProfileECHO "ProfileECHO"

/**
 * @brief ProfilePLC
 */
#define kszProfilePLC "ProfilePLC"

/**
 * @brief ProfileVAD
 */
#define kszProfileVAD "ProfileVAD"

/**
 * @brief 注释
 */
#define kszProgrameKeyValue "ProgrameKey.%d.Value"

/**
 * @brief 注释
 */
#define kszProgramekeyDKtype "Programekey.%d.DKtype"

/**
 * @brief 注释
 */
#define kszProgramekeyHistoryType "Programekey.%d.HistoryType"

/**
 * @brief 注释
 */
#define kszProgramekeyLineId "Programekey.%d.LineId"

/**
 * @brief 注释
 */
#define kszProgramekeylabel "Programekey.%d.label"

/**
 * @brief 注释
 */
#define kszProgramekeypickupValue "Programekey.%d.pickupValue"

/**
 * @brief 注释
 */
#define kszProgramekeyxmlPhoneBook "Programekey.%d.xmlPhoneBook"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.block_in_calling
 *         path | /config/user/user.ini
 *      section | PushXML
 *          key | BlockInCalling
 *      default | 0
 */
#define kszPushXMLBlockInCalling "PushXMLBlockInCalling"

/**
 * @brief PushXMLPassWord
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.password
 *         path | /config/user/user.ini
 *      section | PushXML
 *          key | PassWord
 *      default | ""
 *      encrypt | True
 */
#define kszPushXMLPassWord "PushXMLPassWord"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.server
 *         path | /config/user/user.ini
 *      section | PushXML
 *          key | ServerIP
 *      default | ""
 *          reg | {maxlength:511}
 */
#define kszPushXMLServerIP "PushXMLServerIP"

/**
 * @brief PushXMLUserName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.username
 *         path | /config/user/user.ini
 *      section | PushXML
 *          key | UserName
 *      default | ""
 */
#define kszPushXMLUserName "PushXMLUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.quick_login
 *         path | /config/user/user.ini
 *      section | Features
 *          key | QuickLogin
 *      default | 0
 */
#define kszQuickLogin "QuickLogin"

/**
 * @brief RPS是否为最高优先级
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.rps_highest_priority.enable
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | bEnableRPSPriority
 *      default | 0
 *  t49-default | 1
 */
#define kszRPSPriority "RPSPriority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_delay_threshold_critical
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_delay_threshold_critical
 *      default | ""
 *          reg | {range:[10,2000],maxlength:4}
 */
#define kszRTCPCriticalDelay "RTCPCriticalDelay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_moslq_threshold_critical
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_moslq_threshold_critical
 *      default | ""
 *          reg | {range:[15,40],maxlength:2}
 */
#define kszRTCPCriticalMoslq "RTCPCriticalMoslq"

/**
 * @brief RTCPInterval
 */
#define kszRTCPInterval "RTCPInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_delay_threshold_warning
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_delay_threshold_warming
 *      default | ""
 *          reg | {range:[10,2000],maxlength:4}
 */
#define kszRTCPWarmingDelay "RTCPWarmingDelay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_moslq_threshold_warning
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_moslq_threshold_warming
 *      default | ""
 *          reg | {range:[15,40],maxlength:2}
 */
#define kszRTCPWarmingMoslq "RTCPWarmingMoslq"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_delay_threshold_warning
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_delay_threshold_warming
 *      default | ""
 */
#define kszRTCPWarningDelay "RTCPWarningDelay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr_moslq_threshold_warning
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | vq_rtcpxr_moslq_threshold_warming
 *      default | ""
 */
#define kszRTCPWarningMoslq "RTCPWarningMoslq"

/**
 * @brief XR开关，控制手柄是否发往服务器，与发往gui无关。sipphone是vpm读取配置项，w52p需要gui传给手柄。
 */
#define kszRTCPXRSwitch "RTCPXRSwitch"

/**
 * @brief ReCallUseLocalSipServer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.redial_via_local_sip_server.enable
 *       t49-m7 | custom.features.redial_via_local_sip_server.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ReCallUseLocalSipServer
 *      default | 0
 */
#define kszReCallUseLocalSipServer "ReCallUseLocalSipServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.redial_tone
 *         path | /config/user/user.ini
 *      section | ReDialtone
 *          key | strReDialtoneNumber
 *      default | ""
 *          reg | {digits:true,maxlength:5}
 */
#define kszReDialtoneNumber "ReDialtoneNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.relog_offtime
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ReLogOffTime
 *      default | 5
 *          reg | {required:true,range:[1,1000,5],maxlength:4}
 */
#define kszReLogOffTime "ReLogOffTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.reboot_in_talk_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RebootInTalking
 *      default | 0
 */
#define kszRebootInTalking "RebootInTalking"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.receive_priority.enable
 *         path | /config/user/MulticastPage.cfg
 *      section | ReceivePriority
 *          key | Active
 *      default | 1
 */
#define kszReceiveActive "ReceiveActive"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.receive_priority.priority
 *         path | /config/user/MulticastPage.cfg
 *      section | ReceivePriority
 *          key | Priority
 *      default | 10
 */
#define kszReceivePriority "ReceivePriority"

/**
 * @brief 重拨是否隐藏呼出信息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.redial.redialhidesipname
 *         path | /config/user/user.ini
 *      section | Redial
 *          key | RedialHideSipName
 *      default | 0
 */
#define kszIsRedialHideSipName "RedialHideSipName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.redial_user_name
 *         path | /config/user/user.ini
 *      section | Redial
 *          key | RedialDisplayName
 *      default | ""
 */
#define kszRedialName "RedialName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.redial_number
 *         path | /config/user/user.ini
 *      section | Redial
 *          key | RedialNum
 *      default | ""
 */
#define kszRedialNumber "RedialNumber"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.redial_server
 *         path | /config/user/user.ini
 *      section | Redial
 *          key | RedialServerName
 *      default | ""
 */
#define kszRedialServer "RedialServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.redial.redialusername
 *         path | /config/user/user.ini
 *      section | Redial
 *          key | RedialUserName
 *      default | ""
 */
#define kszRedialUser "RedialUser"

/**
 * @brief 注释
 */
#define kszRedirect_Active "Redirect-Active"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.network_fail_delay_times
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | DelayTime
 *      default | 1000
 */
#define kszRedirectDelayTime "RedirectDelayTime"

/**
 * @brief 注释
 *
 * Redirect Define
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | redirect.enable
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | bEnable
 *      default | 1
 *  t49-default | 0
 */
#define kszRedirectEnable "RedirectEnable"
#define kszRedirectSwitch "RedirectEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | redirect.password
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | strPassword
 *      default | ""
 *      encrypt | True
 */
#define kszRedirectPassword "RedirectPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.redirect.server
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | Server
 *      default | ""
 */
#define kszRedirectServer "RedirectServer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | redirect.user_name
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | UserName
 *      default | ""
 */
#define kszRedirectUser "RedirectUser"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.redirect.strusername
 *       t49-m7 | redirect.user_name
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | strUserName
 *      default | ""
 */
#define kszRedirectUserName "RedirectUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.network_fail_wait_times
 *         path | /config/system/system.ini
 *      section | Redirect
 *          key | WaitTimes
 *      default | 5
 */
#define kszRedirectWaitTimes "RedirectWaitTimes"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.phonesetting.redirection
 *         path | /config/system/system.ini
 *      section | PhoneSetting
 *          key | Redirection
 *      default | ""
 */
#define kszRedirection "Redirection"

/**
 * @brief 注释
 */
#define kszRefreshCycle "RefreshCycle"

/**
 * @brief 注释
 */
#define kszRegisterAccountLabel "RegisterAccountLabel"

/**
 * @brief 注释
 */
#define kszRegisterAccountPassword "RegisterAccountPassword"

/**
 * @brief 注释
 */
#define kszRegisterAccountStatus "RegisterAccountStatus"

/**
 * @brief 注释
 */
#define kszRegisterBackupOutboundProxyServer "RegisterBackupOutboundProxyServer"

/**
 * @brief 注释
 */
#define kszRegisterDisplayName "RegisterDisplayName"

/**
 * @brief 注释
 */
#define kszRegisterEnabledOutboundProxyServer "RegisterEnabledOutboundProxyServer"

/**
 * @brief 注释
 */
#define kszRegisterLineActive "RegisterLineActive"

/**
 * @brief 注释
 */
#define kszRegisterNATTraversal "RegisterNATTraversal"

/**
 * @brief 注释
 */
#define kszRegisterName "RegisterName"

/**
 * @brief 注释
 */
#define kszRegisterOutboundProxyServer "RegisterOutboundProxyServer"

/**
 * @brief 注册用PIN码
 */
#define kszRegisterPinCode "RegisterPinCode"

/**
 * @brief 注释
 */
#define kszRegisterSTUNServer "RegisterSTUNServer"

/**
 * @brief 注释
 */
#define kszRegisterSipServer "RegisterSipServer"

/**
 * @brief 注释
 */
#define kszRegisterSwitchAccount "RegisterSwitchAccount"

/**
 * @brief 注释
 */
#define kszRegisterTransportItem "RegisterTransportItem"

/**
 * @brief 注释
 */
#define kszRegisterUserName "RegisterUserName"

/**
 * @brief ReinviteRespondIgnoreCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.respond_ignore_code
 *      default | 200
 */
#define kszReinviteRespondIgnoreCode "ReinviteRespondIgnoreCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.anonymous_response_code
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AnonymousCode
 *      default | 433
 */
#define kszRejectAnonymousCallCode "RejectAnonymousCallCode"

/**
 * @brief 远程联系人界面直接按数字键进行搜索
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | remote_phonebook.super_search.enable
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | RemoteDirEasySearch
 *      default | 0
 */
#define kszRemoteDirEasySearch "RemoteDirEasySearch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_phonebook.enter_update_enable
 *         path | /config/user/user.ini
 *      section | SRemoteName
 *          key | RemoteDirEnterUpdate
 *      default | ""
 */
#define kszRemoteDirEnterUpdate "RemoteDirEnterUpdate"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | directory.incoming_call_match_enable
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | bRemoteDirectorySearchOn
 *      default | ""
 */
#define kszRemoteDirectorySearchSwtich "RemoteDirectorySearchSwtich"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.directory.nremoteupdatetimeinterval
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | nRemoteUpdateTimeInterval
 *      default | ""
 */
#define kszRemoteDirectoryUpdateTimeInterval "RemoteDirectoryUpdateTimeInterval"

/**
 * @brief 来电号码名称滚动速度
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.incoming_call.horizontal_roll_interval
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RemoteDispScrollTimer
 *      default | 0
 */
#define kszRemoteDispScrollTimer "RemoteDispScrollTimer"

/**
 * @brief 定制Remotehold当做Hold处理
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_hold_to_local_hold.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RemoteHoldToHold
 *      default | 0
 */
#define kszRemoteHoldToHold "RemoteHoldToHold"

/**
 * @brief RemoteOfficePasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.remote_office_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RemoteOfficePasswordIME
 *      default | 5
 */
#define kszRemoteOfficePasswordIME "RemoteOfficePasswordIME"

/**
 * @brief RemotePcapPassword
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_pcap.password
 *         path | /config/system/system.ini
 *      section | RemotePcap
 *          key | password
 *      default | ""
 *      encrypt | True
 */
#define kszRemotePcapPassword "RemotePcapPassword"

/**
 * @brief RemotePcapPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_pcap.port
 *         path | /config/system/system.ini
 *      section | RemotePcap
 *          key | port
 *      default | 2002
 */
#define kszRemotePcapPort "RemotePcapPort"

/**
 * @brief RemotePcapSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_pcap.enable
 *         path | /config/system/system.ini
 *      section | RemotePcap
 *          key | bEnable
 *      default | 0
 */
#define kszRemotePcapSwitch "RemotePcapSwitch"

/**
 * @brief 注释
 */
#define kszRemotePhonebookList "RemotePhonebookList"

/**
 * @brief RepeaterSwitch
 */
#define kszRepeaterSwitch "RepeaterSwitch"

/**
 * @brief 本地历史记录开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | meta.comm_portal.replace_local_call_list.enable
 *         path | /config/user/user.ini
 *      section | Metaswitch
 *          key | bReplaceLocalCalllogEnable
 *      default | 0
 */
#define kszReplaceLocalCallListEnable "ReplaceLocalCallListEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.reserve_certs_enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | IsReserveCerts
 *      default | 0
 */
#define kszReserveCerts "ReserveCerts"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.reset_by_long_press_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ResetByLongPress
 *      default | 1
 */
#define kszResetByLongPress "ResetByLongPress"

/**
 * @brief 取消注册后，重置手柄名
 */
#define kszResetHandsetNameAfterDeReg "ResetHandsetNameAfterDeReg"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.brestarttalkafter183
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bRestartTalkAfter183
 *      default | 1
 */
#define kszRestartalkAfter183Switch "RestartalkAfter183"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.resume_after_exit_transfer.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ResumeAfterTran
 *      default | 0
 */
#define kszResumeAfterTran "ResumeAfterTran"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.park_retrieve_code
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | RetriveCode
 *      default | ""
 *          reg | {maxlength:32}
 */
#define kszRetriveParkCode "RetriveParkCode"

/**
 * @brief 回拨是否隐藏呼出信息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.callreturn.returnhidesipname
 *         path | /config/user/user.ini
 *      section | CallReturn
 *          key | ReturnHideSipName
 *      default | 0
 */
#define kszIsReturnHideSipName "ReturnHideSipName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.ring_for_tranfailed
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | RingForTranFailed
 *      default | Ring1.wav
 */
#define kszRingForTranFailed "RingForTranFailed"

/**
 * @brief 注释
 *
 * 宏定义不同，字符串内容一样
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.ring_power_led_flash_enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_Ring
 *      default | 1
 */
#define kszPowerLightControlRinging "RingPowerLightFlash"
#define kszRingPowerLightFlash "RingPowerLightFlash"

/**
 * @brief RingbackTimeout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.ringback_timeout
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RingbackTimeout
 *      default | 180
 */
#define kszRingBackTimeout "RingbackTimeout"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.ringer_device.is_use_headset
 *         path | /config/user/user.ini
 *      section | RingerDevice
 *          key | eUseHeadSet
 *      default | 0
 */
#define kszRingerDevice "RingerDevice"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.rmcallinfwd
 *         path | /config/user/user.ini
 *      section | Features
 *          key | RmCallInFWD
 *      default | 0
 */
#define kszRmCallInFWD "RmCallInFWD"

/**
 * @brief 注释 SIPListenPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.listen_port
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | SipPort
 *      default | 5060
 *          reg | {required:true,range:[1024,65535,5060],maxlength:5}
 */
#define kszSIPListenPort "SIPListenPort"

/**
 * @brief  SIPTrustCtrl
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.trust_ctrl
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | SIPTrustCtrl
 *      default | 0
 */
#define kszSIPTrustCtrl "SIPTrustCtrl"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.text_message.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | SMSEnable
 *      default | 1
 */
#define kszSMSEnable "SMSEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.specialfunctions.smsenableinlock
 *         path | /phone/factory/user/user.ini
 *      section | SpecialFunctions
 *          key | SMSEnableInLock
 *      default | 1
 */
#define kszSMSEnableInLock "SMSEnableInLock"

/**
 * @brief SNMPFORBID
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.wui.features.snmp_forbid
 *         path | /phone/factory/user/user.ini
 *      section | Features
 *          key | snmp_forbid
 *      default | 1
 */
#define kszSNMPFORBID "SNMPFORBID"

/**
 * @brief SRTPFORBID
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.srtp_forbid.enable
 *         path | /phone/factory/system/system.ini
 *      section | sipServerOptions
 *          key | srtp_forbid
 *      default | 0
 */
#define kszSRTPFORBID "SRTPFORBID"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_phonebook.flash_time
 *         path | /config/user/user.ini
 *      section | SRemoteName
 *          key | FlashTime
 *      default | 3600
 *          reg | {maxlength:9}
 */
#define kszSRemoteNameFlashTime "SRemoteNameFlashTime"

/**
 * @brief SRemoteNameMinFlashTime
 */
#define kszSRemoteNameMinFlashTime "SRemoteNameMinFlashTime"

/**
 * @brief SSLMethod
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.default_ssl_method
 *         path | /config/system/system.ini
 *      section | https
 *          key | DefaultSSLMethod
 *      default | 0
 */
#define kszSSLMethod "SSLMethod"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.save_call_history
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bSaveCallHistory
 *      default | 1
 */
#define kszSaveCallHistory "SaveCallHistory"
#define kszSaveCallLogSwitch "SaveCallHistory"

/**
 * @brief 是否保存蓝牙通话历史记录
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.savemobilecalllog
 *       t49-m7 | features.bluetooth.savemobilecalllog
 *         path | /config/user/user.ini
 *      section | Features
 *          key | savemobilecalllog
 *      default | 1
 */
#define kszSaveMobileCallHistory "SaveMobileCallHistory"

/**
 * @brief SaveOriginalNum2History
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.save_init_num_to_history.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SaveOriginalNum2History
 *      default | 1
 */
#define kszSaveOriginalNum2History "SaveOriginalNum2History"

/**
 * @brief 通话中放回底座是否挂断通话
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.auto_provision.save_rps_info.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bSaveRPSInfoToAutop
 *      default | 0
 */
#define kszSaveRPSInfoToAutop "SaveRPSInfoToAutop"

/**
 * @brief SaveSrcCallLog
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.save_initia_calllog.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SaveSrcCallLog
 *      default | 0
 */
#define kszSaveSrcCallLog "SaveSrcCallLog"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.screensaver.ntimeout
 *         path | /config/user/user.ini
 *      section | ScreenSaver
 *          key | nTimeout
 *      default | 30
 */
#define kszScreenInterval "ScreenSaverTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.screensaver.nusigntimeout
 *         path | /config/user/user.ini
 *      section | ScreenSaver
 *          key | nUsignTimeout
 *      default | 30
 */
#define kszScreenTimeout "ScreenSaverTimeout"

/**
 * @brief 注释
 *
 * 用于配置屏保的类型，0系统默认，1用户自定义
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.mode
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | eType
 *      default | 0
 */
#define kszScreenSaverType "ScreenSaverType"
#define kszScreenType "ScreenSaverType"

/**
 * @brief 用于配置话机多久无操作后要进入屏保
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.wait_time
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | ScreenSaverWaitTime
 *      default | 21600
 *  t49-default | 600
 */
#define kszScreenSaverWaitTime "ScreenSaverWaitTime"

/**
 * @brief 注释
 */
#define kszScreenshotLevel "ScreenshotLevel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.remote_phonebook.enable
 *         path | /config/user/user.ini
 *      section | SRemoteName
 *          key | Enable
 *      default | 0
 */
#define kszSearchNameFromRemoteSwitch "SearchNameFromRemoteSwitch"

/**
 * @brief SecureDomainList
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.secure_domain_list
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SecureDomainList
 *      default | any
 */
#define kszSecureDomainList "SecureDomainList"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.semi_attend_tran_enable
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | bSemiAttendedTranEnable
 *      default | 1
 */
#define kszSemiAttendedTranSwitch "SemiAttendedTranSwitch"

/**
 * @brief 截答时保存2条记录
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup_caller_num_record.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | Send2CallRecord
 *      default | 0
 */
#define kszSend2CallRecord "Send2CallRecord"

/**
 * @brief 注释
 */
#define kszServerCertsHttpsDevCert "ServerCertsHttpsDevCert"

/**
 * @brief 注释
 */
#define kszServerCertsRecords "ServerCertsRecords"

/**
 * @brief 注释
 */
#define kszServerCertsUpload "ServerCertsUpload"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.log_level
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | nLogLevel
 *      default | 1
 *  dbx-default | 3
 */
#define kszServerLogLevel "ServerLogLevel"

/**
 * @brief 注释
 *
 * Session Report开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.vq_rtcpxr.session_report.enable
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | bSessionReportEnable
 *      default | 0
 */
#define kszRTCPSessionReport "SessionReportSwitch"
#define kszSessionReportSwitch "SessionReportSwitch"

/**
 * @brief 注释
 */
#define kszSettingFavoriteTable "SettingFavoriteTable"

/**
 * @brief 注释
 */
#define kszSettingSearchTable "SettingSearchTable"

/**
 * @brief 注释
 */
#define kszSettingsActiveLevel "SettingsActiveLevel"

/**
 * @brief 注释
 */
#define kszSettingsBacklightTime "SettingsBacklightTime"

/**
 * @brief 注释
 */
#define kszWebSettingsCallDisplayMethod "SettingsCallDisplayMethod"

/**
 * @brief 注释
 */
#define kszSettingsContrastCtrl "SettingsContrastCtrl"

/**
 * @brief 注释
 */
#define kszSettingsDHCPTime "SettingsDHCPTime"

/**
 * @brief 注释
 */
#define kszSettingsDateFormatSetting "SettingsDateFormatSetting"

/**
 * @brief 注释
 */
#define kszSettingsDaylightTime "SettingsDaylightTime"

/**
 * @brief 注释
 */
#define kszWebSettingsDisplayContactPhoto "SettingsDisplayContactPhoto"

/**
 * @brief 注释
 */
#define kszWebSettingsDisplayVQReportOnUI "SettingsDisplayVQReportOnUI"

/**
 * @brief 注释
 */
#define kszWebSettingsDisplayVQReportOnWeb "SettingsDisplayVQReportOnWeb"

/**
 * @brief 注释
 */
#define kszWebSettingsIncomingCallRingerAnimation "SettingsIncomingCallRingerAnimation"

/**
 * @brief 注释
 */
#define kszSettingsInterDigitTime "SettingsInterDigitTime"

/**
 * @brief 注释
 */
#define kszSettingsLiveDialpad "SettingsLiveDialpad"

/**
 * @brief 注释
 */
#define kszSettingsManualTime "SettingsManualTime"

/**
 * @brief 注释
 */
#define kszSettingsNTPPriority "SettingsNTPPriority"

/**
 * @brief 注释
 */
#define kszSettingsPrimServer "SettingsPrimServer"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPCriticalDelay "SettingsRTCPCriticalDelay"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPCriticalMoslq "SettingsRTCPCriticalMoslq"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPIntervalPeriod "SettingsRTCPIntervalPeriod"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPIntervalReport "SettingsRTCPIntervalReport"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPReportOptions "SettingsRTCPReportOptions"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPSessionReport "SettingsRTCPSessionReport"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPWarmingDelay "SettingsRTCPWarmingDelay"

/**
 * @brief 注释
 */
#define kszWebSettingsRTCPWarmingMoslq "SettingsRTCPWarmingMoslq"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.rtcp_xr_report.enable
 *         path | /config/user/vpm.cfg
 *      section | Voice Quality
 *          key | rtcp_xr_report_enable
 *      default | 0
 */
#define kszSettingsRTCPXRReport "SettingsRTCPXRReport"

/**
 * @brief 注释
 */
#define kszSettingsRingTypeSetting "SettingsRingTypeSetting"

/**
 * @brief 注释
 */
#define kszSettingsSecServer "SettingsSecServer"

/**
 * @brief 注释
 */
#define kszSettingsTimeFormatSetting "SettingsTimeFormatSetting"

/**
 * @brief 注释
 */
#define kszSettingsTimeZone "SettingsTimeZone"

/**
 * @brief 注释
 */
#define kszSettingsUnusedBackLight "SettingsUnusedBackLight"

/**
 * @brief 注释
 */
#define kszSettingsUpdateInterval "SettingsUpdateInterval"

/**
 * @brief 注释
 */
#define kszSettingsWallpaper "SettingsWallpaper"

/**
 * @brief 注释
 */
#define kszSettingsWatchdog "SettingsWatchdog"

/**
 * @brief 注释
 */
#define kszSettingsWebLanguage "SettingsWebLanguage"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_action_uri_option
 *         path | /config/user/user.ini
 *      section | ActionURI
 *          key | ShowActionURIOption
 *      default | 1
 */
#define kszShowActionURIOption "ShowActionURIOption"

/**
 * @brief 显示calllog menu
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.showcalllogmenu
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ShowCalllogMenu
 *      default | 0
 */
#define kszShowCalllogMenu "ShowCalllogMenu"

/**
 * @brief 当使用用户图片的时候是否显示时钟
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.display_clock_on_upload_picture.enable
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | ShowClockOnUploadPic
 *      default | 1
 */
#define kszShowClockWhenUserPic "ShowClockOnUploadPic"

/**
 * @brief 显示DND大图标
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd.large_icon.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ShowDNDLargeIcon
 *      default | 0
 */
#define kszShowDNDLargeIcon "ShowDNDLargeIcon"

/**
 * @brief 在msgbox中显示gpicpup
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_group_pickup_in_popup.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ShowGPickUpInMsgbox
 *      default | 0
 */
#define kszShowGPickUpInMsgbox "ShowGPickUpInMsgbox"

/**
 * @brief 门禁配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | door_phone.show_image_list
 *         path | /config/user/user.ini
 *      section | DoorPhone
 *          key | ShowImageList
 *      default | 0
 */
#define kszShowImageList "ShowImageList"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.show_num_first
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ShowNumFirst
 *      default | 0
 */
#define kszShowNumFirst "ShowNumFirst"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.lync.simulincomeingname
 *         path | /config/user/user.ini
 *      section | Lync
 *          key | SimulIncomeingName
 *      default | ""
 */
#define kszSimultaneousName "SimultaneousName"

/**
 * @brief SipInfoInRing
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | door_phone.sip_info_type
 *         path | /config/user/user.ini
 *      section | DoorPhone
 *          key | SipInfoInRing
 *      default | 1
 */
#define kszSipInfoInRing "SipInfoInRing"

/**
 * @brief SipLoopBackStatus
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.loop_back_status.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bLoopBackStatus
 *      default | 0
 */
#define kszSipLoopBackStatus "SipLoopBackStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.rfc2543_hold
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | bRFC2543Hold
 *      default | 0
 */
#define kszSipRFC2543Hold "SipRFC2543Hold"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.reserve_characters
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | ReservedCharacter
 *      default | ""
 */
#define kszSipReserveChar "SipReserveChar"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.use_23_as_pound
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | bUse23AsPound
 *      default | 0
 */
#define kszSipReservePound "SipReservePound"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.tls_listen_port
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | SipsPort
 *      default | 5061
 *          reg | {required:true,range:[1024,65535,5061],maxlength:5}
 */
#define kszSipTlsListenPort "SipTlsListenPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.sip.unregisteronreboot
 *         path | /config/user/user.ini
 *      section | sip
 *          key | UnregisterOnReboot
 *      default | 0
 */
#define kszSipUnregisterOnReboot "SipUnregisterOnReboot"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.use_out_bound_in_dialog
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | bUseOutBoundInDialog
 *      default | 1
 */
#define kszSipUseOutBoundInDialog "SipUseOutBoundInDialog"

/**
 * @brief sip useragent
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.general.useragent
 *       t49-m7 | sip.user_agent_prefix
 *         path | None
 *      section | General
 *          key | UserAgent
 *      default | ""
 */
#define kszSipUserAgent "SipUserAgent"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.srtp_encryption_type
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | VoiceEncryptType
 *      default | 0
 */
#define kszSipVoiceEncryptType "SipVoiceEncryptType"

/**
 * @brief SkipDisplayName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.skipdisplayname
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SkipDisplayName
 *      default | 0
 */
#define kszSkipDisplayName "SkipDisplayName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.shorten_linekey_label.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SmallLabel
 *      default | 0
 */
#define kszDsskeySmallLabel "SmallLabel"

/**
 * @brief softkey上的park键使用的call park code
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.callpark.softkeycallparkcode
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | SoftKeyCallParkCode
 *      default | ""
 */
#define kszSoftKeyCallParkCode "SoftKeyCallParkCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.incoming_call_change_softkey.enable
 *         path | /config/user/user.ini
 *      section | YUK
 *          key | SoftkeyChangedCallIncome
 *      default | 1
 */
#define kszSoftkeyChangedCallIncome "SoftkeyChangedCallIncome"

/**
 * @brief SoftkeyCutoff
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.softkey_label_cutoff.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SoftkeyCutoff
 *      default | 0
 */
#define kszSoftkeyCutoff "SoftkeyCutoff"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.custom_auto_answer_tone.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SpecialAutoAnswerTone
 *      default | 0
 */
#define kszSpecialAutoAnswerTone "SpecialAutoAnswerTone"

/**
 * @brief StandardBlueGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.blue_gain
 *      default | 70
 */
#define kszCameraStandardBlueGain "StandardBlueGain"

/**
 * @brief StandardBrightness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.brightness
 *      default | 50
 */
#define kszCameraStandardBrightness "StandardBrightness"

/**
 * @brief StandardContrast
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.contrast
 *      default | 50
 */
#define kszCameraStandardContrast "StandardContrast"

/**
 * @brief StandardNR2DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.nr2d_level
 *      default | 32
 */
#define kszCameraStandardNR2DLevel "StandardNR2DLevel"

/**
 * @brief StandardNR3DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.nr3d_level
 *      default | 0
 */
#define kszCameraStandardNR3DLevel "StandardNR3DLevel"

/**
 * @brief StandardRedGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.red_gain
 *      default | 80
 */
#define kszCameraStandardRedGain "StandardRedGain"

/**
 * @brief StandardSaturation
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.saturation
 *      default | 50
 */
#define kszCameraStandardSaturation "StandardSaturation"

/**
 * @brief StandardSharpness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.sharpness
 *      default | 30
 */
#define kszCameraStandardSharpness "StandardSharpness"

/**
 * @brief 标准模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.standard.white_balance
 *      default | 5
 */
#define kszCameraStandardWhiteBalanceMode "StandardWhiteBalanceMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.idle_input_star_only.enable
 *       t49-m7 | phone_settings.idle_input_star_only.enable
 *         path | /config/user/user.ini
 *      section | YUK
 *          key | StarOnlyStarDial
 *      default | 0
 */
#define kszStarOnlyOnDialUI "StarOnlyOnDialUI"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.hotdesking_startup.callcenterautopurl
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | CallCenterAutopURL
 *      default | ""
 */
#define kszStart_HotDeskingCallCenterURL "Start_CallCenterAutopURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_autop_url_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | AutopURL
 *      default | 0
 */
#define kszStart_HotDeskingAutopurl "Start_HotDeskingAutopurl"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_outbound_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | Outbound
 *      default | 1
 */
#define kszStart_HotDeskingOutbound "Start_HotDeskingOutbound"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_password_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | Password
 *      default | 1
 */
#define kszStart_HotDeskingPassword "Start_HotDeskingPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_register_name_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | RegisterName
 *      default | 1
 */
#define kszStart_HotDeskingRegisterName "Start_HotDeskingRegisterName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_sip_server_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | SipServer
 *      default | 1
 */
#define kszStart_HotDeskingSipServer "Start_HotDeskingSipServer"

/**
 * @brief http://192.168.1.99/Bug.php?BugID=29534
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_autop_url
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | StartUp_AutopURL
 *      default | ""
 */
#define kszStart_HotDeskingURL "Start_HotDeskingURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hotdesking.startup_username_enable
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | UserName
 *      default | 1
 */
#define kszStart_HotDeskingUserName "Start_HotDeskingUserName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.hotdesking_startup.officephoneautopurl
 *         path | /config/user/user.ini
 *      section | HotDesking_StartUp
 *          key | OfficePhoneAutopURL
 *      default | ""
 */
#define kszStart_HotDeskingOfficeURL "Start_OfficePhoneAutopURL"

/**
 * @brief 注释
 */
#define kszStatesHead "StatesHead"

/**
 * @brief StaticNatEffectType
 */
#define kszStaticNatEffectType "StaticNatEffectType"

/**
 * @brief Station Name
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.station_name.value
 *         path | /config/user/user.ini
 *      section | Features
 *          key | StationName
 *      default | ""
 */
#define kszStationName "StationName"

/**
 * @brief 注释
 */
#define kszStatusAccountStatus "StatusAccountStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.popup_prompt_box_enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | StatusChangeIsNotPopWind
 *      default | 0
 */
#define kszStatusChangeIsNotPopWind "StatusChangeIsNotPopWind"

/**
 * @brief 注释
 */
#define kszStatusExttable "StatusExttable"

/**
 * @brief 注释
 */
#define kszStatusIPv4 "StatusIPv4"

/**
 * @brief 注释
 */
#define kszStatusIPv6 "StatusIPv6"

/**
 * @brief 注释
 */
#define kszStatusNetwork "StatusNetwork"

/**
 * @brief 注释
 */
#define kszStatusNetworkCommon "StatusNetworkCommon"

/**
 * @brief 注释
 */
#define kszStatusVersion "StatusVersion"

/**
 * @brief 注释 configmap重复
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_stun.port
 *         path | /config/system/system.ini
 *      section | Network
 *          key | STUNPort
 *      default | 3478
 *          reg | {required:true,range:[1024,65000,3478],maxlength:5}
 */
#define kszStunPort "StunPort"

/**
 * @brief 注释 configmap重复
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.nat_stun.server
 *         path | /config/system/system.ini
 *      section | Network
 *          key | STUNServer
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszStunServer "StunServer"

/**
 * @brief GCS相关配置-------->
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.gcs.sub
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeGCS
 *      default | 0
 */
#define kszSubscribeGCS "SubscribeGCS"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.intercom.subscribe.enable
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | bSubscribeIntercom
 *      default | 0
 */
#define kszSubscribeIntercom "SubscribeIntercom"

/**
 * @brief SubscriptXsiEventList
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.broadsoft.subscriptxsieventlist
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | SubscriptXsiEventList
 *      default | ""
 */
#define kszSubscriptXsiEventList "SubscriptXsiEventList"

/**
 * @brief 注释
 */
#define kszSummaryDisplay "SummaryDisplay"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.supersearch.bsoftkeyboard
 *         path | /config/user/user.ini
 *      section | SuperSearch
 *          key | bSoftKeyboard
 *      default | 0
 */
#define kszSuperSearchKeypadSwitch "SuperSearchKeypadSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.supersearch.bsearchrecent
 *         path | /config/user/user.ini
 *      section | SuperSearch
 *          key | bSearchRecent
 *      default | ""
 */
#define kszSuperSearchRecentSwitch "SuperSearchRecentSwitch"

/**
 * @brief 注释  config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.specialfunctions.supportkorean
 *       t49-m7 | ime.korean.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | SupportKorean
 *      default | 0
 */
#define kszSupportKorean "SupportKorean"

/**
 * @brief SupportMacAddressRange
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.supportmacaddressrange
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | SupportMacAddressRange
 *      default | 001565;805EC0
 */
#define kszMacAddressRange "SupportMacAddressRange"

/**
 * @brief 注释
 */
#define kszSwitchAccount "SwitchAccount"

/**
 * @brief SwitchAudioByte
 */
#define kszSwitchAudioByte "SwitchAudioByte"

/**
 * @brief SwitchPageKeyMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.keep_switch_page_key.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SwitchPageKeyMode
 *      default | 0
 */
#define kszSwitchPageKeyMode "SwitchPageKeyMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.mode
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | LogType
 *      default | 1
 */
#define kszSysLogType "SysLogType"

/**
 * @brief SyslogFacility
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.facility
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | facility
 *      default | 16
 */
#define kszSyslogFacility "SyslogFacility"

/**
 * @brief 注释  config id 没人用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.level
 *       t49-m7 | syslog.mode
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | level
 *      default | 3
 */
#define kszSysLogLevel "SyslogLevel"

/**
 * @brief SyslogPrependMac
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.prepend_mac_address.enable
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | AppendMAC
 *      default | 0
 */
#define kszSyslogPrependMac "SyslogPrependMac"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.server
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | SyslogServer
 *      default | ""
 *          reg | {required:true,address:true,maxlength:99}
 */
#define kszSyslogServerName "SyslogServerName"

/**
 * @brief SyslogServerPort
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.server_port
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | server_port
 *      default | ""
 */
#define kszSyslogServerPort "SyslogServerPort"

/**
 * @brief SyslogSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.enable
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | bEnable
 *      default | 0
 */
#define kszSyslogSwitch "SyslogSwitch"

/**
 * @brief SyslogTransportType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.transport_type
 *         path | /config/system/system.ini
 *      section | Syslog
 *          key | transport_type
 *      default | 0
 */
#define kszSyslogTransportType "SyslogTransportType"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.zultys.t60softkey3callpark
 *         path | /phone/factory/system.ini
 *      section | Zultys
 *          key | T60Softkey3CallPark
 *      default | ""
 */
#define kszT60Softkey3CallPark "T60Softkey3CallPark"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.connection_request_password
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | ConnectionRequestPassword
 *      default | ""
 *      encrypt | True
 *          reg | {exclude:'&',maxlength:64}
 */
#define kszTR069ConnectionRequestPassword "TR069ConnectionRequestPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.connection_request_username
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | ConnectionRequestUsername
 *      default | ""
 *          reg | {exclude:'$*%%&:!',maxlength:128}
 */
#define kszTR069ConnectionRequestUsername "TR069ConnectionRequestUsername"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.manufacturer_oui
 *         path | /config/TR069/tr069.ini
 *      section | DeviceInfo
 *          key | ManufacturerOUI
 *      default | ""
 */
#define kszTR069DeviceManufacturerOUI "TR069DeviceManufacturerOUI"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.product_class
 *         path | /config/TR069/tr069.ini
 *      section | DeviceInfo
 *          key | ProductClass
 *      default | ""
 */
#define kszTR069DeviceProductClass "TR069DeviceProductClass"

/**
 * @brief V81新增
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.wui.features.tr069_forbid
 *         path | /phone/factory/user/user.ini
 *      section | Features
 *          key | tr069_forbid
 *      default | 0
 */
#define kszTR069FORBID "TR069FORBID"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.periodic_inform_interval
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | PeriodicInformInterval
 *      default | 30
 *          reg | {required:true,range:[5,4294967295,60],maxlength:10}
 */
#define kszTR069PeriodicInformInterval "TR069PeriodicInformInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.periodic_inform_enable
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | PeriodicInformEnable
 *      default | 1
 */
#define kszTR069PeriodicInformSwitch "TR069PeriodicInformSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.password
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | Password
 *      default | ""
 *      encrypt | True
 *          reg | {exclude:'&',maxlength:64}
 */
#define kszTR069ServerPassword "TR069ServerPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.enable
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | EnableTR069
 *      default | 0
 */
#define kszTR069ServerTR069Switch "TR069ServerTR069Switch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.url
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | URL
 *      default | ""
 *          reg | {url:true,maxlength:511}
 */
#define kszTR069ServerURL "TR069ServerURL"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | managementserver.username
 *         path | /config/TR069/tr069.ini
 *      section | ManagementServer
 *          key | Username
 *      default | ""
 *          reg | {exclude:'$*%%&:!',maxlength:128}
 */
#define kszTR069ServerUsername "TR069ServerUsername"

/**
 * @brief TalkError.%%d.Prompt
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.talkerrorinfo.error.%%d.prompt
 *         path | /config/user/user.ini
 *      section | TalkErrorInfo
 *          key | Error.%%d.Prompt
 *      default | ""
 */
#define kszTalkErrorInfoPrompt "TalkError.%d.Prompt"

/**
 * @brief TalkError.%%d.Reason
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.talkerrorinfo.error.%%d.reason
 *         path | /config/user/user.ini
 *      section | TalkErrorInfo
 *          key | Error.%%d.Reason
 *      default | ""
 */
#define kszTalkErrorInfoCode "TalkError.%d.Reason"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.talk_and_dial_power_led_enable
 *         path | /config/user/user.ini
 *      section | PowerLed
 *          key | PowerLed_TalkDial
 *      default | 0
 */
#define kszPowerLightControlTalk "TalkPowerLightFlash"
#define kszTalkPowerLightFlash "TalkPowerLightFlash"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.telnet.benable
 *         path | /config/system/system.ini
 *      section | Telnet
 *          key | bEnable
 *      default | 0
 */
#define kszTelnetEnable "TelnetEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.logo.textlogo
 *         path | /config/user/user.ini
 *      section | Logo
 *          key | TextLogo
 *      default | Yealink
 */
#define kszTextLogo "TextLogo"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | local_time.interval
 *         path | /config/system/system.ini
 *      section | LocalTime
 *          key | nNTPLeaseSeconds
 *      default | ""
 *          reg | {required:true,range:[15,86400,1000],maxlength:5}
 */
#define kszTimeInterval "TimeInterval"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.sip.tlsportrandom
 *         path | /config/user/user.ini
 *      section | sip
 *          key | TlsPortRandom
 *      default | ""
 */
#define kszTlsPortRandom "TlsPortRandom"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.autoanswer
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | autoanswer
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneAutoanswer "ToneAutoanswer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.busy
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | busy
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneBusy "ToneBusy"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.callwaiting
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | callwaiting
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneCallwaiting "ToneCallwaiting"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.tone param.callwaiting2
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | callwaiting2
 *      default | ""
 */
#define kszToneCallwaiting2 "ToneCallwaiting2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.tone param.callwaiting3
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | callwaiting3
 *      default | ""
 */
#define kszToneCallwaiting3 "ToneCallwaiting3"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.tone param.callwaiting4
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | callwaiting4
 *      default | ""
 */
#define kszToneCallwaiting4 "ToneCallwaiting4"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.congestion
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | congestion
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneCongestion "ToneCongestion"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.country
 *         path | /config/user/vpm.cfg
 *      section | vpm_tone_Country
 *          key | Country
 *      default | Custom
 */
#define kszToneCountry "ToneCountry"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.dial
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | dial
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneDial "ToneDial"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.dialrecall
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | dialrecall
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneDialrecall "ToneDialrecall"

/**
 * @brief DTMF 播放间隔时长
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.min_interval
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ToneDtmfOfTime
 *      default | 50
 */
#define kszToneDtmfOftime "ToneDtmfOfTime"

/**
 * @brief DTMF 播放时长
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.duration
 *         path | /config/user/vpm.cfg
 *      section | DTMF
 *          key | duration
 *      default | 50
 */
#define kszToneDtmfOntime "ToneDtmfOnTime"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.info
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | info
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneInfo "ToneInfo"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.message
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | message
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneMessage "ToneMessage"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.record
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | record
 *      default | ""
 */
#define kszToneRecord "ToneRecord"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.ring
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | ring
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneRing "ToneRing"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.secondary_dial
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | seconddial
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneSecondDial "ToneSecondDial"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.call_preview_mode
 *         path | /config/user/user.ini
 *      section | Profile
 *          key | tone_strategy
 *      default | 1
 */
#define kszToneStrategy "ToneStrategy"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.stutter
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | stutter
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszToneStutter "ToneStutter"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.trace.level
 *         path | /config/user/MulticastPage.cfg
 *      section | Trace
 *          key | Level
 *      default | 1
 */
#define kszTraceLevel "TraceLevel"

/**
 * @brief touceroute的目标地址  config id 没人用
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.diagnostics.strtraceroutetarget
 *         path | /config/user/user.ini
 *      section | Diagnostics
 *          key | strTracerouteTarget
 *      default | ""
 */
#define kszTrouterouteTarget "TracerouteTarget"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.tran_others_after_conf_enable
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | bTranAfterConfExitEnable
 *      default | 0
 */
#define kszTranAfterConfExitSwitch "TranAfterConfExitSwitch"

/**
 * @brief TransToVMCode
 */
#define kszTransToVMCode "TransToVMCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.transfer_type
 *         path | /config/user/user.ini
 *      section | Features
 *          key | TransferByByeReason
 *      default | ""
 */
#define kszTransferByByeReason "TransferByByeReason"

/**
 * @brief 是否使用新的key来创建Transfer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_appearance.transfer_via_new_linekey
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bTransNewLinekey
 *      default | 1
 */
#define kszTransferByNewLine "TransferByNewLine"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.transfer
 *         path | /config/user/user.ini
 *      section | Features
 *          key | TransferDTMF
 *      default | ""
 *          reg | {contain:'0123456789ABCD*#',maxlength:32}
 */
#define kszTransferDTMF "TransferDTMF"

/**
 * @brief 禁用转接
 */
#define kszTransferForbidden "TransferForbidden"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | transfer.dsskey_deal_type
 *         path | /config/user/user.ini
 *      section | Transfer
 *          key | DSSKeyDealType
 *      default | 2
 */
#define kszTransferModeViaDsskey "TransferModeViaDsskey"

/**
 * @brief TransferNeedSeizeLine
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.sca_transfer_seize_line.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | TransferNeedSeizeLine
 *      default | 1
 */
#define kszTransferNeedSeizeLine "TransferNeedSeizeLine"

/**
 * @brief 咨询转用Hold那路通话作为Host Session
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.transfer.set_hold_session_as_host.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | TransferSetHoldSessionAsHost
 *      default | 0
 */
#define kszTransferSetHoldSessionAsHost "TransferSetHoldSessionAsHost"

/**
 * @brief 按下Trans时候是否使用新的Linekey呼出
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.call_appearance.transfer_via_new_linekey
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bTransNewLinekey
 *      default | 1
 */
#define kszTransferUseNewLinekey "TransferUseNewLinekey"

/**
 * @brief 咨询转之后必须要等到Transfer结果才能退出
 *
 * 默认值：0
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.transfer_keep_session2_after_failed.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | TransferWaitResult
 *      default | 0
 */
#define kszTransferWaitResult "TransferWaitResult"

/**
 * @brief 高透明度开关是否开启
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.idle_dsskey_and_title.transparency
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | TransparencyType
 *      default | 100%%
 */
#define kszTransparencyType "TransparencyType"

/**
 * @brief 注释
 */
#define kszTrustCertsOnlyAcceptTrustedCerts "TrustCertsOnlyAcceptTrustedCerts"

/**
 * @brief 注释
 */
#define kszTrustCertsRecords "TrustCertsRecords"

/**
 * @brief 注释
 */
#define kszTrustCertsUploadTrustCerts "TrustCertsUploadTrustCerts"

/**
 * @brief 注释
 */
#define kszTrustCertsbtndeleteCerts "TrustCertsbtndeleteCerts"

/**
 * @brief 注释
 */
#define kszUC_Active "UC-Active"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.broadsoft uc.strpassword
 *         path | /config/user/user.ini
 *      section | Broadsoft UC
 *          key | strPassword
 *      default | ""
 */
#define kszUCPassword "UCPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.broadsoft uc.strusername
 *         path | /config/user/user.ini
 *      section | Broadsoft UC
 *          key | strUsername
 *      default | ""
 */
#define kszUCUsername "UCUsername"

/**
 * @brief 注释
 */
#define kszUCVersion "UCVersion"

/**
 * @brief URIUseSipAccountCall
 */
#define kszURIUseSipAccountCall "URIUseSipAccountCall"

/**
 * @brief 注释
 */
#define kszURLHead "URLHead"

/**
 * @brief 通话自动USB录音开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.auto_recording.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | USBAutoRecordEnable
 *      default | 0
 */
#define kszUSBAutoRecordEnable "USBAutoRecordEnable"

/**
 * @brief USBIdleRecordEnable
 *
 *         name | value
 * ------------:|:----------------
 *         path | /config/user/user.ini
 *      section | Features
 *          key | USBIdleRecordEnable
 *      default | 0
 */
#define kszUSBIdleRecordEnable "USBIdleRecordEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.usb_call_recording.enable
 *       t49-m7 | features.call_recording.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | USBRecordEnable
 *      default | 1
 */
#define kszUSBRecordEnable "USBRecordEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.record_warning_time
 *       t49-m7 | phone_setting.record_warming_time
 *         path | /config/user/user.ini
 *      section | USBRecord
 *          key | WarningTime
 *      default | 60
 *  dbx-default | ""
 *  t49-default | 30;10
 */
#define kszUSBRecordWarningTime "USBRecordWarningTime"

/**
 * @brief USBScreenshotEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.screenshot.enable
 *      default | 1
 */
#define kszUSBScreenshotEnable "USBScreenshotEnable"

/**
 * @brief USDectInvertSwitch
 */
#define kszUSDectInvertSwitch "USDectInvertSwitch"

/**
 * @brief UC联系人尾部匹配号码个数
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.uc_dir.match_tail_number
 *         path | /config/user/user.ini
 *      section | Features
 *          key | UcTailMatchNumber
 *      default | 1
 *  dbx-default | 0
 */
#define kszUcTailMatchNumber "UcTailMatchNumber"

/**
 * @brief UniqueLinekeyLabel
 */
#define kszUniqueLinekeyLabel "UniqueLinekeyLabel"

/**
 * @brief GUI还未给，我自己先定一个测试，等GUI给了，需改回来
 */
#define kszUniqueLinekeyLabelSwitch "UniqueLinekeyLabelSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.phone_lock.unlock_pin
 *         path | /config/user/user.ini
 *      section | Phone_Lock
 *          key | Unlock_PIN
 *      default | ""
 *          reg | {digits:true,maxlength:15}
 */
#define kszUnlockPIN "UnlockPIN"

/**
 * @brief UnlockPinIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.phone_unlock_pin
 *         path | /config/user/user.ini
 *      section | Features
 *          key | UnlockPinIME
 *      default | 123
 */
#define kszUnlockPinIME "UnlockPinIME"

/**
 * @brief 注释
 *
 * Autop 更新状态
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.proc.status
 *         path | /tmp/UpdateStatus
 *      section | PROC
 *          key | STATUS
 *      default | ""
 */
#define kszAutopUpdateStatus "UpdateStatus"
#define kszUpdateStatus "UpdateStatus"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.ftp.append_limit_mode
 *         path | /config/system/system.ini
 *      section | UploadLogFile
 *          key | AppendLimitMode
 *      default | 0
 */
#define kszUploadLogFileAppendLimitMode "UploadLogFileAppendLimitMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.ftp.max_logfile
 *         path | /config/system/system.ini
 *      section | UploadLogFile
 *          key | MaxLogSize
 *      default | 512
 *          reg | {required:true,range:[200,65535,512],maxlength:5}
 */
#define kszUploadLogFileMaxLogSize "UploadLogFileMaxLogSize"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.log_upload_period
 *         path | /config/system/system.ini
 *      section | UploadLogFile
 *          key | Period
 *      default | 86400
 *          reg | {required:true,range:[30,2592000,86400],maxlength:7}
 */
#define kszUploadLogFilePeriod "UploadLogFilePeriod"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | syslog.ftp.post_mode
 *         path | /config/system/system.ini
 *      section | UploadLogFile
 *          key | PostMode
 *      default | 0
 */
#define kszUploadLogFilePostMode "UploadLogFilePostMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.upload_server
 *         path | /config/user/user.ini
 *      section | Features
 *          key | UploadServer
 *      default | ""
 */
#define kszSystemInfoServerURL "UploadServer"

/**
 * @brief 一律使用From头域的号码，进行DND/FWD紧急号码的匹配
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.emergency_match_from_number.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | UseFromNumberMatchEmergency
 *      default | 0
 */
#define kszUseFromNumberMatchEmergency "UseFromNumberMatchEmergency"

/**
 * @brief 注释
 */
#define kszUseLocalFwd "UseLocalFwd"

/**
 * @brief 注释  config id 没人用
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.account%%d.general.useragent
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | General
 *          key | UserAgent
 *      default | ""
 */
#define kszUserAgent "UserAgent"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.user_level.enable
 *         path | /config/user/user.ini
 *      section | SpecialFunctions
 *          key | UserEnable
 *      default | 1
 */
#define kszUserEnable "UserEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.var_enable
 *         path | /config/system/system.ini
 *      section | Advanced
 *          key | var_enabled
 *      default | 0
 */
#define kszUserMode "UserMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.user_name.admin
 *         path | /config/user/user.ini
 *      section | UserName
 *          key | admin
 *      default | admin
 */
#define kszUserNameAdmin "UserNameAdmin"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.user_name.user
 *         path | /config/user/user.ini
 *      section | UserName
 *          key | user
 *      default | user
 */
#define kszUserNameUser "UserNameUser"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | security.user_name.var
 *         path | /config/user/user.ini
 *      section | UserName
 *          key | var
 *      default | var
 */
#define kszUserNameVar "UserNameVar"

/**
 * @brief user密码限制不能为user
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | advanced.user_pwd_limit
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | UserPwdLimit
 *      default | 0
 */
#define kszUserPwdLimit "UserPwdLimit"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.cng
 *         path | /config/user/vpm.cfg
 *      section | Profile
 *          key | CNG
 *      default | 1
 */
#define kszVPMCNG "VPMCNG"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.echo_cancellation
 *         path | /config/user/vpm.cfg
 *      section | Profile
 *          key | ECHO
 *      default | 1
 */
#define kszVPMECHO "VPMECHO"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.vad
 *         path | /config/user/vpm.cfg
 *      section | Profile
 *          key | VAD
 *      default | 0
 */
#define kszVPMVAD "VPMVAD"

/**
 * @brief 是否禁用VPN
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.wui.features.vpn_forbid
 *         path | /phone/factory/user/user.ini
 *      section | Features
 *          key | vpn_forbid
 *      default | 0
 */
#define kszForbidVPN "VPNFORBID"
#define kszVPNFORBID "VPNFORBID"

/**
 * @brief 是否开启var权限
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | advanced.var_enable
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | VarEnable
 *      default | 1
 */
#define kszVarEnable "VarEnable"

/**
 * @brief 自动应答是否mute 视频
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | video.auto_answer_video_mute.enable
 *      default | 0
 */
#define kszVideoAutoAnswerMute "VideoAutoAnswerMute"

/**
 * @brief 是否自动开始视频
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | video.auto_start_video.enable
 *      default | 1
 */
#define kszVideoAutoStart "VideoAutoStart"

/**
 * @brief 视频主流：最小帧率
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.video_min_framerate
 *      default | 30
 */
#define kszVideoMinFramerate "VideoMinFramerate"

/**
 * @brief 是否支持重协商
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | video.re_negotiation.enable
 *      default | 1
 */
#define kszVideoRenegotiationEnable "VideoRenegotiationEnable"

/**
 * @brief Local Early Media
 */
#define kszVideoSdpReportTwice "VideoSdpReportTwice"

/**
 * @brief 是否支持视频
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | video.enable
 *      default | 1
 */
#define kszVideoTalkEnable "VideoTalkEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.dhcp_option
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | DHCPOption
 *      default | 132
 *          reg | {required:true,multiple:{range:[1,255,1],splits:',',count:5},maxlength:128}
 */
#define kszVlanDhcpOption "VlanDhcpOption"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.dhcp_enable
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | VlanDHCP
 *      default | 1
 */
#define kszVlanDhcpSwitch "VlanDhcpSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.pc_port_priority
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | nPcPriority
 *      default | 0
 */
#define kszVlanPcPriority "VlanPcPriority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.pc_port_enable
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | bPcSwitch
 *      default | 0
 */
#define kszVlanPcSwitch "VlanPcSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.pc_port_vid
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | nPcVid
 *      default | 1
 *          reg | {required:true,range:[1,4094,1],maxlength:4}
 */
#define kszVlanPcVid "VlanPcVid"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.internet_port_priority
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | nPriority
 *      default | 0
 */
#define kszVlanWanPriority "VlanWanPriority"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.internet_port_enable
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | bEnable
 *      default | 0
 */
#define kszVlanWanSwitch "VlanWanSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vlan.internet_port_vid
 *         path | /config/system/system.ini
 *      section | VLAN
 *          key | nID
 *      default | 1
 *          reg | {required:true,range:[1,4094,1],maxlength:4}
 */
#define kszVlanWanVid "VlanWanVid"

/**
 * @brief 8x8 客户特制
 */
#define kszVoiceMailCode "VoiceMailCode"

/**
 * @brief VoiceMail MessageKeyMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice_mail.message_key.mode
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | VoiceMailMessageKeyMode
 *      default | 0
 */
#define kszVoiceMailMessageKeyMode "VoiceMailMessageKeyMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.voice_mail.skin_pin.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ADVANCED
 *          key | vm_skip_pin
 *      default | ""
 */
#define kszVoiceMailWithoutPIN "VoiceMailWithoutPIN"

/**
 * @brief RTCP-XR开关，是否发往gui
 *
 * 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.rtcp_xr.enable
 *         path | /config/user/vpm.cfg
 *      section | RTCP
 *          key | xr_enable
 *      default | 0
 */
#define kszVoiceRTCPXRReport "VoiceRTCPXRReport"
#define kszVoiceRTCPXRReportSwitch "VoiceRTCPXRReport"

/**
 * @brief VoipH239Switch
 */
#define kszVoipH239Switch "VoipH239Switch"

/**
 * @brief h323 dtmf
 */
#define kszVoipUserCapEnable "VoipUserCapEnable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.key_tone
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | bButtonSoundOn
 *      default | 1
 */
#define kszKeypressSound "VolumeButtonSoundOn"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.volume.ncolorringback
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nColorRingback
 *      default | 5
 */
#define kszVolumeColorRingback "VolumeColorRingback"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.group_listening.spk_vol
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | GroupSpkVol
 *      default | 8
 */
#define kszVolumeOutputGroup "VolumeOutputGroup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.ring_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nRing
 *      default | 5
 */
#define kszVolumeRing "VolumeRing"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.send_key_tone
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | bSendKeySoundOn
 *      default | 1
 */
#define kszVolumeSendKeySoundOn "VolumeSendKeySoundOn"

/**
 * @brief 配置Stutter dial tone提示音的频率/周期值
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.stutter_dial_tone
 *         path | /config/user/vpm.cfg
 *      section | Tone Param
 *          key | stutterdial
 *      default | ""
 */
#define kszStutterDialTone "StutterDialTone"

/**
 * @brief 开启/关闭未读语音邮件拨号提醒
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.stutter_dial_tone.apply_to_vm.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bStutterDialToneVMEnable
 *      default | 0
 */
#define kszStutterDialToneVMSwitch "StutterDialToneVMSwitch"

/**
 * @brief 开启/关闭免扰拨号提醒
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.stutter_dial_tone.apply_to_dnd.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bStutterDialToneDNDEnable
 *      default | 0
 */
#define kszStutterDialToneDNDSwitch "StutterDialToneDNDSwitch"

/**
 * @brief 开启/关闭转接拨号提醒
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tone.stutter_dial_tone.apply_to_fwd.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bStutterDialToneFWDEnable
 *      default | 0
 */
#define kszStutterDialToneFWDSwitch "StutterDialToneFWDSwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.volumesetinlockenable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | VolumeSetInLockEnable
 *      default | ""
 */
#define kszVolumeSetInLock "VolumeSetInLock"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.volumeset.bprofile
 *         path | /config/system/system.ini
 *      section | VolumeSet
 *          key | bProfile
 *      default | 1
 */
#define kszVolumeSetProfile "VolumeSetProfile"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.volume.ntalkinputhandfree
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkInputHandfree
 *      default | 8
 */
#define kszVolumeTalkInputHandfree "VolumeTalkInputHandfree"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.volume.ntalkinputhandset
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkInputHandset
 *      default | 8
 */
#define kszVolumeTalkInputHandset "VolumeTalkInputHandset"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.volume.ntalkinputheadset
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkInputHeadset
 *      default | 8
 */
#define kszVolumeTalkInputHeadset "VolumeTalkInputHeadset"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handfree.spk_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkOutputHandfree
 *      default | 8
 */
#define kszVolumeTalkOutputHandfree "VolumeTalkOutputHandfree"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handset.spk_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkOutputHandset
 *      default | 8
 */
#define kszVolumeTalkOutputHandset "VolumeTalkOutputHandset"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.headset.spk_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkOutputHeadset
 *      default | 8
 */
#define kszVolumeTalkOutputHeadset "VolumeTalkOutputHeadset"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handfree.tone_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nToneHandfree
 *      default | 8
 */
#define kszVolumeToneHandfree "VolumeToneHandfree"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handset.tone_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nToneHandset
 *      default | 8
 */
#define kszVolumeToneHandset "VolumeToneHandset"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.headset.tone_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nToneHeadset
 *      default | 8
 */
#define kszVolumeToneHeadset "VolumeToneHeadset"

/**
 * @brief VpmToneCountry
 */
#define kszVpmToneCountry "VpmToneCountry"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.vpn_enable
 *         path | /config/system/system.ini
 *      section | VPN
 *          key | bEnableVPN
 *      default | ""
 */
#define kszVpnOnSwitch "VpnSwitch"

/**
 * @brief 注释
 */
#define kszWEBDisplay "WEBDisplay"

/**
 * @brief WaitAutopTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.check_autop_result_before_register
 *         path | /config/system/system.ini
 *      section | REGSURGE
 *          key | CheckAutopResultBeforRegister
 *      default | 0
 */
#define kszWaitAutopTime "WaitAutopTime"

/**
 * @brief WallpaperBackgroundColor
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.backgrounds.color
 *         path | /config/user/user.ini
 *      section | Features
 *          key | WallpaperBackgroundColor
 *      default | ""
 */
#define kszWallpaperBackgroundColor "WallpaperBackgroundColor"

/**
 * @brief WallpaperMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.backgrounds.position
 *         path | /config/user/user.ini
 *      section | Features
 *          key | WallpaperMode
 *      default | 0
 */
#define kszWallpaperMode "WallpaperMode"

/**
 * @brief WarmBlueGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.blue_gain
 *      default | 70
 */
#define kszCameraWarmBlueGain "WarmBlueGain"

/**
 * @brief WarmBrightness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.brightness
 *      default | 45
 */
#define kszCameraWarmBrightness "WarmBrightness"

/**
 * @brief WarmContrast
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.contrast
 *      default | 50
 */
#define kszCameraWarmContrast "WarmContrast"

/**
 * @brief WarmNR2DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.nr2d_level
 *      default | 32
 */
#define kszCameraWarmNR2DLevel "WarmNR2DLevel"

/**
 * @brief WarmNR3DLevel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.nr3d_level
 *      default | 0
 */
#define kszCameraWarmNR3DLevel "WarmNR3DLevel"

/**
 * @brief WarmRedGain
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.red_gain
 *      default | 90
 */
#define kszCameraWarmRedGain "WarmRedGain"

/**
 * @brief WarmSaturation
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.saturation
 *      default | 50
 */
#define kszCameraWarmSaturation "WarmSaturation"

/**
 * @brief WarmSharpness
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.sharpness
 *      default | 30
 */
#define kszCameraWarmSharpness "WarmSharpness"

/**
 * @brief 强光模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.warm.white_balance
 *      default | 5
 */
#define kszCameraWarmWhiteBalanceMode "WarmWhiteBalanceMode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | watch_dog.time
 *         path | /config/user/user.ini
 *      section | WatchDog
 *          key | Time
 *      default | 3
 *  t49-default | 10
 */
#define kszWatchDogTime "WatchDogTime"

/**
 * @brief 注释
 */
#define kszWebAccountBasicVoiceMail "WebAccountBasicVoiceMail"

/**
 * @brief 注释
 */
#define kszWebAccountSendAnonymous "WebAccountSendAnonymous"

/**
 * @brief 登入Web GUI配置界面后的无操作允许时长
 *
 * 默认值：15mins
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.stay_active_wait_time
 *         path | /config/system/system.ini
 *      section | WebTimer
 *          key | nActivityTimer
 *      default | 0
 */
#define kszWebActivityTimer "WebActivityTimer"

/**
 * @brief WebGuidesURL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | web_setting.guides_link_url
 *         path | /config/user/user.ini
 *      section | WebSetting
 *          key | guides_link
 *      default | ""
 */
#define kszWebGuidesURL "WebGuidesURL"

/**
 * @brief 登入Web GUI配置界面前的允许时长
 *
 * 默认值：15mins
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.login_wait_time
 *         path | /config/system/system.ini
 *      section | WebTimer
 *          key | nLoginTimer
 *      default | 0
 */
#define kszWebLoginTimer "WebLoginTimer"

/**
 * @brief WebLogoLink
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | web_setting.logo_link
 *         path | /config/user/user.ini
 *      section | WebSetting
 *          key | logo_link
 *      default | http://www.yealink.com
 */
#define kszWebLogoLink "WebLogoLink"

/**
 * @brief 登入Web GUI配置界面后最大允许操作时长
 *
 * 默认值：15mins
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.operate_wait_time
 *         path | /config/system/system.ini
 *      section | WebTimer
 *          key | nMaxTimer
 *      default | 0
 */
#define kszWebMaxTimer "WebMaxTimer"

/**
 * @brief WebModelName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | web_setting.model_name
 *         path | /config/user/user.ini
 *      section | WebSetting
 *          key | model_name
 *      default | $DEV
 *  t49-default | ""
 */
#define kszWebModelName "WebModelName"

/**
 * @brief WebTitleName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | web_setting.title
 *         path | /config/user/user.ini
 *      section | WebSetting
 *          key | title
 *      default | Yealink $DEV Phone
 *  t49-default | ""
 */
#define kszWebTitleName "WebTitleName"

/**
 * @brief EAP 用户名
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.eap_user_name
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | identity
 *      default | ""
 */
#define kszWiFiEapName "WiFiEapName%d"

/**
 * @brief EAP 密码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.eap_password
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | password
 *      default | ""
 *      encrypt | True
 */
#define kszWiFiEapPassword "WiFiEapPassword%d"

/**
 * @brief EAP 类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.eap_type
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | eap
 *      default | ""
 */
#define kszWiFiEapType "WiFiEapType%d"

/**
 * @brief 加密方式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.cipher_type
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | pairwise
 *      default | 0
 */
#define kszWiFiEncryptType "WiFiEncryptType%d"

/**
 * @brief SSID
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.ssid
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | ssid
 *      default | ""
 */
#define kszWiFiName "WiFiName%d"

/**
 * @brief 密码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.password
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | psk
 *      default | ""
 *      encrypt | True
 */
#define kszWiFiPassword "WiFiPassword%d"

/**
 * @brief 优先级
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.priority
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | priority
 *      default | ""
 */
#define kszWiFiPriority "WiFiPriority%d"

/**
 * @brief ProfileName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.label
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | profilename
 *      default | ""
 */
#define kszWiFiProfileName "WiFiProfileName%d"

/**
 * @brief 安全模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.%%d.security_mode
 *         path | /config/system/system.ini
 *      section | WifiNetwork%%d
 *          key | key_mgmt
 *      default | 0
 */
#define kszWiFiSecureMode "WiFiSecureMode%d"

/**
 * @brief WiFi
 *
 * 开关
 * wifi功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.enable
 *         path | /config/user/user.ini
 *      section | wifi
 *          key | bEnable
 *      default | 0
 */
#define kszWifiSwitch "WiFiSwitch"

/**
 * @brief wifi适配器mac
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.features.wifi_adapter_mac
 *         path | /config/user/user.ini
 *      section | wifi
 *          key | WifiAdapterMac
 *      default | ""
 */
#define kszWifiAdapterMac "WifiAdapterMac"

/**
 * @brief WifiTestPassLossRate
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.pass_loss_rate
 *      default | ""
 */
#define kszWifiTestPassLossRate "WifiTestPassLossRate"

/**
 * @brief 插入WiFiDongle时,是否显示扫描提示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.show_scan_prompt
 *         path | /config/user/user.ini
 *      section | wifi
 *          key | ShowScanPrompt
 *      default | 1
 */
#define kszWifiShowScanPrompt "WifiShowScanPrompt"

/**
 * @brief WifiTestPassLossRate2
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.pass_loss_rate2
 *      default | ""
 */
#define kszWifiTestPassLossRate2 "WifiTestPassLossRate2"

/**
 * @brief WifiTestPassRate
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.pass_rate
 *      default | ""
 */
#define kszWifiTestPassRate "WifiTestPassRate"

/**
 * @brief WifiTestPassRate2
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.pass_rate2
 *      default | ""
 */
#define kszWifiTestPassRate2 "WifiTestPassRate2"

/**
 * @brief wifi测试是否通过
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.passedflag
 *      default | 0
 */
#define kszWifiTestPassedFlag "WifiTestPassedFlag"

/**
 * @brief WifiTestSendRate
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.send_rate
 *      default | ""
 */
#define kszWifiTestSendRate "WifiTestSendRate"

/**
 * @brief WifiTestSendRate2
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.send_rate2
 *      default | ""
 */
#define kszWifiTestSendRate2 "WifiTestSendRate2"

/**
 * @brief 测试模式wifi测试配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.server_ip
 *      default | ""
 */
#define kszWifiTestServerIP "WifiTestServerIP"

/**
 * @brief test2
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.wifi_test.server_ip2
 *      default | ""
 */
#define kszWifiTestServerIP2 "WifiTestServerIP2"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.word.wordflag
 *         path | /phone/factory/user/user.ini
 *      section | Word
 *          key | WordFlag
 *      default | , .
 */
#define kszWordFlag "WordFlag"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.check_sync.action_url
 *         path | /config/system/system.ini
 *      section | CheckSync
 *          key | strXMLActionURL
 *      default | ""
 */
#define kszXMLActionURL "XMLActionURL"

/**
 * @brief XSI功能总开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bXSIEnable
 *      default | 0
 */
#define kszXSISwitch "XSISwitch"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.xml_browser.pwd
 *         path | /config/user/user.ini
 *      section | XmlBrowser
 *          key | Password
 *      default | ""
 *      encrypt | True
 */
#define kszXmlBrowserPassword "XmlBrowserPassword"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.xml_browser.user_name
 *         path | /config/user/user.ini
 *      section | XmlBrowser
 *          key | UserName
 *      default | ""
 */
#define kszXmlBrowserUser "XmlBrowserUser"

/**
 * @brief xml input 默认输入法
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.xml_browser_input_screen
 *         path | /config/user/user.ini
 *      section | Features
 *          key | XmlInputStringDefaultIME
 *      default | 2aB
 */
#define kszXmlInputStringDefaultIME "XmlInputStringDefaultIME"

/**
 * @brief XmlPhonebookUrl
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.phonebook_in_talking.url
 *         path | /config/user/user.ini
 *      section | Features
 *          key | XmlPhonebookUrl
 *      default | ""
 */
#define kszXmlPhonebookUrl "XmlPhonebookUrl"

/**
 * @brief XmlScreenSaverURL
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | screensaver.xml_browser.url
 *         path | /config/user/user.ini
 *      section | Screensaver
 *          key | XmlScreenSaverURL
 *      default | ""
 */
#define kszXmlBrowserScreenSaverURL "XmlScreenSaverURL"

/**
 * @brief XsiEventExpiresTime
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi_event_expires_time
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | XsiEventExpiresTime
 *      default | 3600
 */
#define kszXsiEventExpiresTime "XsiEventExpiresTime"

/**
 * @brief XsiPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.xsi_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | XsiPasswordIME
 *      default | 2aB
 */
#define kszXsiPasswordIME "XsiPasswordIME"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.zero.benable
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | bEnable
 *      default | ""
 */
#define kszZeroActive "ZeroActive"

/**
 * @brief ZeroDirectUpdate
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.direct_update.enable
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | ZeroDirectUpdate
 *      default | 0
 */
#define kszZeroDirectUpdate "ZeroDirectUpdate"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hide_zero_touch_network_url.enable
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | IsShowNetWorkAndURL
 *      default | 1
 */
#define kszZeroNetworkAndURLEnable "ZeroNetworkAndURLEnable"

/**
 * @brief ZeroPasswordIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.zero_touch_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ZeroPasswordIME
 *      default | 2
 */
#define kszZeroPasswordIME "ZeroPasswordIME"

/**
 * @brief Autop简化流程
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.simplification
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | bSimplification
 *      default | 0
 */
#define kszZeroSimplification "ZeroSimplification"

/**
 * @brief ZeroUserIME
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.zero_touch_username
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ZeroUserIME
 *      default | 5
 */
#define kszZeroUserIME "ZeroUserIME"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | zero_touch.wait_time
 *         path | /config/user/user.ini
 *      section | Zero
 *          key | iZeroWaitTime
 *      default | 5
 *          reg | {required:true,range:[1,100,5],maxlength:3}
 */
#define kszZeroWaitTime "ZeroWaitTime"

/**
 * @brief Zultys Park Softkey默认value
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.callpark.zultysparkvalue
 *         path | /config/user/user.ini
 *      section | CallPark
 *          key | ZultysParkValue
 *      default | callpark
 */
#define kszZultysParkValue "ZultysParkValue"

/**
 * @brief 注释
 */
#define kszaccount_adv "account-adv"

/**
 * @brief 注释
 */
#define kszaccount_basic "account-basic"

/**
 * @brief 注释
 */
#define kszaccount_codec "account-codec"

/**
 * @brief 注释
 */
#define kszaccount_register "account-register"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.always_fwd.enable
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | AF_Enable
 *      default | ""
 */
#define kszAccountAlwaysFWDStatus "account.%d.always_fwd.enable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.always_fwd.off_code
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | AF_Off_Code
 *      default | ""
 */
#define kszAccountAlwaysFWDOffCode "account.%d.always_fwd.off_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.always_fwd.on_code
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | AF_On_Code
 *      default | ""
 */
#define kszAccountAlwaysFWDOnCode "account.%d.always_fwd.on_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.always_fwd.target
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | AF_Target
 *      default | ""
 */
#define kszAccountAlwaysFWDTarget "account.%d.always_fwd.target"

/**
 * @brief account.%%d.always_fwd.target_backup
 */
#define kszAccountAlwaysFWDTargetBackup "account.%d.always_fwd.target_backup"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.busy_fwd.enable
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | BF_Enable
 *      default | ""
 */
#define kszAccountBusyFWDStatus "account.%d.busy_fwd.enable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.busy_fwd.off_code
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | BF_Off_Code
 *      default | ""
 */
#define kszAccountBusyFWDOffCode "account.%d.busy_fwd.off_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.busy_fwd.on_code
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | BF_On_Code
 *      default | ""
 */
#define kszAccountBusyFWDOnCode "account.%d.busy_fwd.on_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.busy_fwd.target
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | BF_Target
 *      default | ""
 */
#define kszAccountBusyFWDTarget "account.%d.busy_fwd.target"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.bw_disp_code.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | BWDispCode
 *          key | Code.%%d
 *      default | ""
 */
#define kszAccountACDDispCode "AccountACDDispCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.bw_disp_code_name.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | BWDispCode
 *          key | CodeName.%%d
 *      default | ""
 */
#define kszAccountACDDispCodeName "AccountACDDispCodeName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dnd.enable
 *         path | /config/user/user.ini
 *      section | DNDAccount%%d
 *          key | dnd
 *      default | ""
 */
#define kszAccountDNDStatus "account.%d.dnd.enable"

/**
 * @brief 具体账号是否DND同步服务器
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dnd.feature_key_sync.enable
 *         path | /config/user/user.ini
 *      section | DNDAccount%%d
 *          key | dndsync
 *      default | ""
 */
#define kszAccountDNDFeatureKeySync "account.%d.dnd.feature_key_sync.enable"

/**
 * @brief DND同步开启的情况下是否在本地处理对应账号的DND（拒接）
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.features.dnd.feature_key_sync.local_processing.enable
 *         path | /config/user/user.ini
 *      section | DNDAccount%%d
 *          key | DNDLocalProcessing
 *      default | ""
 */
#define kszAccountDNDSyncLocalProcess "account.%d.dnd.local_processing"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dnd.off_code
 *         path | /config/user/user.ini
 *      section | DNDAccount%%d
 *          key | DND_Off_Code
 *      default | ""
 */
#define kszAccountDNDOffCode "account.%d.dnd.off_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.dnd.on_code
 *         path | /config/user/user.ini
 *      section | DNDAccount%%d
 *          key | DND_On_Code
 *      default | ""
 */
#define kszAccountDNDOnCode "account.%d.dnd.on_code"

/**
 * @brief FWD同步开启的情况下是否在本地处理对应账号的FWD（转移）
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.features.forward.feature_key_sync.local_processing.enable
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | FWDLocalProcessing
 *      default | ""
 */
#define kszAccountForwardSyncLocalProcess "account.%d.fwd.local_processing"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.outbound_proxy.%%d.port
 *       t49-m7 | account.%%d.outbound_port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | outbound%%d
 *          key | port
 *      default | 5060
 */
#define kszAccountOutboundPort "AccountOutboundPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.outbound_proxy.%%d.address
 *       t49-m7 | account.%%d.outbound_host
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | outbound%%d
 *          key | server
 *      default | ""
 */
#define kszAccountOutboundProxy "AccountOutboundProxy"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.reason_code.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ReasonCode
 *          key | Code.%%d
 *      default | ""
 */
#define kszAccountACDReasonCode "AccountACDReasonCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.reason_code_name.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | ReasonCode
 *          key | CodeName.%%d
 *      default | ""
 */
#define kszAccountACDReasonCodeName "AccountACDReasonCodeName"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.%%d.address
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver%%d
 *          key | server
 *      default | ""
 *          reg | {maxlength:255}
 */
#define kszAccountServerAddr "AccountServerAddr"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.%%d.expires
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver%%d
 *          key | expires
 *      default | 3600
 *  t49-default | ""
 *          reg | {required:true,range:[30,2147483647,3600],maxlength:10}
 */
#define kszAccountServerExpires "AccountServerExpires"
#define kszAccountSipserverExpire "AccountServerExpires"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.%%d.port
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver%%d
 *          key | port
 *      default | ""
 *          reg | {required:true,range:[0,65535,5060],maxlength:5}
 */
#define kszAccountServerPort "AccountServerPort"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.%%d.retry_counts
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver%%d
 *          key | RetryCounts
 *      default | 3
 *  t49-default | ""
 *          reg | {required:true,range:[0,65535,3],maxlength:5}
 */
#define kszAccountServerRetryCounts "AccountServerRetryCounts"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.sip_server.%%d.transport_type
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | sipserver%%d
 *          key | transport
 *      default | ""
 */
#define kszAccountServerTransport "AccountServerTransport"
#define kszSipserverTransport "AccountServerTransport"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.supervisor_info_code.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | SupervisorInfo
 *          key | Code.%%d
 *      default | ""
 */
#define kszAccountACDSupervisorCode "AccountACDSupervisorCode"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.supervisor_info_code_name.%%d
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | SupervisorInfo
 *          key | CodeName.%%d
 *      default | ""
 */
#define kszAccountACDSupervisorCodeName "AccountACDSupervisorCodeName"

/**
 * @brief 注释
 */
#define kszAccountTimeoutFWDOncode "account.%d.time out_fwd.on_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.timeout_fwd.enable
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | TF_Enable
 *      default | ""
 */
#define kszAccountNoASWFWDStatus "account.%d.timeout_fwd.enable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.timeout_fwd.off_code
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | TF_Off_Code
 *      default | ""
 */
#define kszAccountNoASWFWDOffCode "account.%d.timeout_fwd.off_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.timeout_fwd.on_code
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | TF_On_Code
 *      default | ""
 */
#define kszAccountNoASWFWDOnCode "account.%d.timeout_fwd.on_code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.timeout_fwd.target
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | TF_Target
 *      default | ""
 */
#define kszAccountNoASWFWDTarget "account.%d.timeout_fwd.target"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.timeout_fwd.timeout
 *         path | /config/user/user.ini
 *      section | Account%%d
 *          key | Timeout
 *      default | ""
 */
#define kszAccountNoASWFWDTimeout "account.%d.timeout_fwd.timeout"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.video.%%d.enable
 *      default | ""
 */
#define kszAccountCodecEnable "account.%d.video.%d.enable"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.video.%%d.payload_type
 *      default | ""
 */
#define kszAccountCodecType "account.%d.video.%d.payload_type"

/**
 * @brief 注释
 */
#define kszAccountH323CodecEnable "account_h323.video.%d.enable"

/**
 * @brief 注释
 */
#define kszAccountH323CodecType "account_h323.video.%d.payload_type"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.advanced.adminpsw
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | adminPsw
 *      default | ""
 */
#define kszadminPsw "adminPsw"

/**
 * @brief 注释
 */
#define kszarea_code "area-code"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.common_file_name
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | strCommName
 *      default | ""
 */
#define kszAutopCommonFileName "auto_provision.common_file_name"

/**
 * @brief 多路通话时是否hold新page
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hold_received_paging_when_talking.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bHoldPageInMultiSession
 *      default | 0
 */
#define kszHoldPageInMultiSession "bHoldPageInMultiSession"

/**
 * @brief 注释
 */
#define kszBlock_out "block-out"

/**
 * @brief 注释
 */
#define kszBsftCustomName "bw_phonebook.Custom_displayname"

/**
 * @brief 注释
 */
#define kszContacts_LDAP "contacts-LDAP"

/**
 * @brief 注释
 */
#define kszContacts_basic "contacts-basic"

/**
 * @brief 注释
 */
#define kszContacts_broadsoft "contacts-broadsoft"

/**
 * @brief 注释
 */
#define kszContacts_callinfo "contacts-callinfo"

/**
 * @brief 注释
 */
#define kszContacts_favorite "contacts-favorite"

/**
 * @brief 注释
 */
#define kszContacts_multicastIP "contacts-multicastIP"

/**
 * @brief 注释
 */
#define kszContacts_remote "contacts-remote"

/**
 * @brief 注释
 */
#define kszDaylight_day "daylight-day"

/**
 * @brief 注释
 */
#define kszDaylight_week "daylight-week"

/**
 * @brief 注释
 */
#define kszDial_now "dial-now"

/**
 * @brief 注释
 */
#define kszDsskey_extkey "dsskey-extkey"

/**
 * @brief 注释
 */
#define kszDsskey_linekey "dsskey-linekey"

/**
 * @brief 注释
 */
#define kszDsskey_memorykey "dsskey-memorykey"

/**
 * @brief 注释
 */
#define kszDsskey_programkey "dsskey-programkey"

/**
 * @brief 注释
 */
#define kszAutopFacCommonFileName "empty"
#define kszUnregisterOnReboot "empty"

/**
 * @brief 注释
 */
#define kszFeatures_acd "features-acd"

/**
 * @brief 注释
 */
#define kszFeatures_actionurl "features-actionurl"

/**
 * @brief 注释
 */
#define kszFeatures_audio "features-audio"

/**
 * @brief 注释
 */
#define kszFeaturesBluetooth "features-bluetooth"

/**
 * @brief 注释
 */
#define kszFeatures_callpickup "features-callpickup"

/**
 * @brief 注释
 */
#define kszFeatures_forward "features-forward"

/**
 * @brief 注释
 */
#define kszFeatures_general "features-general"

/**
 * @brief 注释
 */
#define kszFeatures_intercom "features-intercom"

/**
 * @brief 注释
 */
#define kszFeaturesNotifypop "features-notifypop"

/**
 * @brief 注释
 */
#define kszFeatures_phonelock "features-phonelock"

/**
 * @brief 注释
 */
#define kszFeaturesPowerled "features-powerled"

/**
 * @brief 注释
 */
#define kszFeatures_remote "features-remote"

/**
 * @brief 注释
 */
#define kszFeatures_sms "features-sms"

/**
 * @brief 注释
 */
#define kszFeatures_transfer "features-transfer"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.default_account
 *         path | /config/user/user.ini
 *      section | Account
 *          key | DefaultAccount
 *      default | 0
 */
#define kszAccountDefaultID "features.default_account"

/**
 * @brief 注释
 */
#define kszLicense "license"

/**
 * @brief 注释
 */
#define kszManualTime "manual-time"

/**
 * @brief 注释
 */
#define kszMode "mode"

/**
 * @brief multicast.listen_address.%%d.channel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.%%d.channel
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress%%d
 *          key | Channel
 *      default | 0
 */
#define kszMulticastListenChannel "multicast.listen_address.%d.channel"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.%%d.ip_address
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress%%d
 *          key | IPAddress
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszMulticastListenIP "multicast.listen_address.%d.ip_address"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.%%d.label
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress%%d
 *          key | Label
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszMulticastListenLabel "multicast.listen_address.%d.label"

/**
 * @brief 音量固定
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | multicast.listen_address.%%d.volume
 *         path | /config/user/MulticastPage.cfg
 *      section | ListenAddress%%d
 *          key | Volume
 *      default | 0
 */
#define kszMulticastListenVolume "multicast.listen_address.%d.volume"

/**
 * @brief 注释
 */
#define kszNetwork_advanced "network-advanced"

/**
 * @brief 注释
 */
#define kszNetwork_basic "network-basic"

/**
 * @brief 注释
 */
#define kszNetwork_pcport "network-pcport"

/**
 * @brief 注释
 */
#define kszOffset "offset"

/**
 * @brief 注释
 */
#define kszPassword "password"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.remotephonebook%%d.name
 *         path | /config/user/user.ini
 *      section | RemotePhoneBook%%d
 *          key | name
 *      default | ""
 */
#define kszRemoteName "remote.%d.name"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.remotephonebook%%d.url
 *         path | /config/user/user.ini
 *      section | RemotePhoneBook%%d
 *          key | url
 *      default | ""
 */
#define kszRemoteUrl "remote.%d.url"

/**
 * @brief 注释
 */
#define kszReplace_rule "replace-rule"

/**
 * @brief 注释
 */
#define kszRowTimeType "rowTimeType"

/**
 * @brief 注释
 */
#define kszWebRtpstatus "rtpstatus"

/**
 * @brief 注释
 */
#define kszServer_cert "server_cert"

/**
 * @brief 注释
 */
#define kszSettings_autop "settings-autop"

/**
 * @brief 注释
 */
#define kszWebSettingsCalldisplay "settings-calldisplay"

/**
 * @brief 注释
 */
#define kszSettings_config "settings-config"

/**
 * @brief 注释
 */
#define kszSettings_datetime "settings-datetime"

/**
 * @brief 注释
 */
#define kszSettings_dialplan "settings-dialplan"

/**
 * @brief 注释
 */
#define kszSettings_preference "settings-preference"

/**
 * @brief 注释
 */
#define kszSettings_ring "settings-ring"

/**
 * @brief 注释
 */
#define kszSettings_softkey "settings-softkey"

/**
 * @brief 注释
 */
#define kszSettings_tones "settings-tones"

/**
 * @brief 注释
 */
#define kszSettings_tr069 "settings-tr069"

/**
 * @brief 注释
 */
#define kszSettings_upgrade "settings-upgrade"

/**
 * @brief 注释
 */
#define kszSettings_voice "settings-voice"

/**
 * @brief 注释
 */
#define kszWebSettingsVoicemonitoring "settings-voicemonitoring"

/**
 * @brief 注释
 */
#define kszWebStatusPhone "status-phone"

/**
 * @brief 注释
 */
#define kszTrusted_cert "trusted_cert"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.advanced.userpsw
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | userPsw
 *      default | ""
 */
#define kszuserPsw "userPsw"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.advanced.varpsw
 *         path | /config/user/user.ini
 *      section | Advanced
 *          key | varPsw
 *      default | ""
 */
#define kszvarPsw "varPsw"

/**
 * @brief UC联系人状态图标模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xmpp.presence_icon.mode
 *         path | /config/user/user.ini
 *      section | Presence
 *          key | PresenceIconMode
 *      default | 0
 */
#define kszPresenceIconMode "PresenceIconMode"

/**
 * @brief WEB一键导出功能，记录当前一键导出状态
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | tmp.web.onetouchexportstatus
 *         path | /tmp/webcustom.ini
 *      section | TmpVar
 *          key | OTEStatus
 *      default | 0
 */
#define kszWebOTEStatus "WebOTEStatus"

/**
 * @brief WEB一键导出功能，临时保存话机原有 LocalLogSwitch 配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | tmp.web.locallogswitch
 *         path | /tmp/webcustom.ini
 *      section | TmpVar
 *          key | LocalLogSwitch
 *      default | 1
 */
#define kszWebTmpLocalLogSwitch "WebTmpLocalLogSwitch"

/**
 * @brief WEB一键导出功能，临时保存话机原有 LocalLogLevel 配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | tmp.web.localloglevel
 *         path | /tmp/webcustom.ini
 *      section | TmpVar
 *          key | LocalLogLevel
 *      default | 3
 */
#define kszWebTmpLocalLogLevel "WebTmpLocalLogLevel"

/**
 * @brief WEB一键导出功能，临时保存话机原有 ServerLogLevel 配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | tmp.web.serverloglevel
 *         path | /tmp/webcustom.ini
 *      section | TmpVar
 *          key | ServerLogLevel
 *      default | 3
 */
#define kszWebTmpServerLogLevel "WebTmpServerLogLevel"

/**
 * @brief 抓包功能,设置每个分片文件的最大存储容量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | packet_capture.max_file_bytes
 *         path | /config/system/system.ini
 *      section | PacketCapture
 *          key | nClipFileKBytes
 *      default | 1024
 */
#define kszPcapClipFileKBytes "PcapClipFileKBytes"

/**
 * @brief 抓包功能,抓包类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | packet_capture.filter_type
 *         path | /config/system/system.ini
 *      section | PacketCapture
 *          key | nFilterType
 *      default | 1
 */
#define kszPcapFilterType "PcapFilterType"

/**
 * @brief 抓包功能,抓包字符串
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | packet_capture.filter
 *         path | /config/system/system.ini
 *      section | PacketCapture
 *          key | strFilter
 *      default | ""
 */
#define kszPcapFilterString "PcapFilterString"

/**
 * @brief 控制网页是否开放features-general页面Advice Of Charge配置项
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.aoc.display_menu_items
 *         path | /config/user/user.ini
 *      section | Features
 *          key | enable_aoc
 *      default | 0
 */
#define kszWuiSupportAoc "WuiSupportAoc"

/**
 * @brief 控制网页是否开放features-general页面Auto BLF List配置项
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.phone_setting.auto_blf_list.display_menu_items
 *         path | /config/user/user.ini
 *      section | Features
 *          key | enable_blflist
 *      default | 0
 */
#define kszWuiSupportAutoBLFList "WuiSupportAutoBLFList"

/**
 * @brief 控制网页是否开放Account-advance页面Callpark配置项
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.features.call_park.display_menu_items
 *         path | /config/user/user.ini
 *      section | Features
 *          key | enable_callpark
 *      default | 0
 */
#define kszWuiSupportAccountCallPark "WuiSupportAccountCallPark"

/**
 * @brief 控制网页是否开放门禁功能配置页面菜单项
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.door_phone.display_menu_items
 *         path | /config/user/user.ini
 *      section | Features
 *          key | enable_doorphone
 *      default | 0
 */
#define kszWuiSupportDoorPhone "WuiSupportDoorPhone"

/**
 * @brief No Answer Forward响铃范围
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.forward.no_answer.show_ring_times
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | AfterRingTimesRange
 *      default | 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
 */
#define kszNoAnswerForwardAfterRingTimesRang "NoAnswerForwardAfterRingTimesRange"

/**
 * @brief No Answer Forward响铃一次时长
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.ring_duration
 *         path | /config/user/user.ini
 *      section | NoAnswerForward
 *          key | RingDuration
 *      default | 6
 */
#define kszNoAnswerForwardRingDuration "NoAnswerForwardRingDuration"

/**
 * @brief 进入LDAP后是否自动loading加载联系人并显示出来
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.auto_load.enable
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | bLDAPLoadJustEnter
 *      default | 1
 */
#define kszLDAPAutoLoadContact "LDAPAutoLoadContact"

/**
 * @brief 联系人号码正则表达式匹配规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.reverse_lookup.contact_list.replace.pattern
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ContactReplacePattern
 *      default | ""
 */
#define kszContactReplacePattern "ContactReplacePattern"

/**
 * @brief 联系人号码正则表达式替换规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.reverse_lookup.contact_list.replace.with
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ContactReplaceWith
 *      default | ""
 */
#define kszContactReplaceWith "ContactReplaceWith"

/**
 * @brief 来电号码正则表达式匹配规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.reverse_lookup.incoming_call.replace.pattern
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IncomeCallRelacePattern
 *      default | ""
 */
#define kszIncomeCallRelacePattern "IncomeCallRelacePattern"

/**
 * @brief 来电号码正则表达式替换规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.reverse_lookup.incoming_call.replace.with
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IncomeCallRelaceWith
 *      default | ""
 */
#define kszIncomeCallRelaceWith "IncomeCallRelaceWith"

/**
 * @brief GCS相关配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%d.gcs.sub_expires
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | SubscribeGCSExpire
 *      default | 3600
 */
#define kszSubscribeGCSExpire "SubscribeGCSExpire"

/**
 * @brief 是否允许访问网页开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wui.limit_ip
 *         path | /config/user/user.ini
 *      section | VisitWeb
 *          key | LimitIP
 *      default | any
 */
#define kszVisitWebLimitIP "VisitWebLimitIP"

/**
 * @brief 用来自定义location request中要带的参数节点Name
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.held.request_element.%%d.name
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyHeldElementName%%d
 *      default | ""
 */
#define kszEmergencyHeldElementName "EmergencyHeldElementName%d"

/**
 * @brief 用来自定义location request中要带的参数节点Value
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.held.request_element.%%d.value
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyHeldElementValue%%d
 *      default | ""
 */
#define kszEmergencyHeldElementValue "EmergencyHeldElementValue%d"

/**
 * @brief HELD Location服务器的URL.
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.held.server_url
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyHeldServer
 *      default | ""
 */
#define kszEmergencyHeldServer "EmergencyHeldServer"

/**
 * @brief HELD服务器的类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.held.request_type
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyHeldRequestType
 *      default | SIMPLE
 */
#define kszEmergencyHeldRequestType "EmergencyHeldRequestType"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | linekey.%%d.type
 *         path | /config/user/linekey.ini
 *      section | key%%d
 *          key | type
 *      default | ""
 */
#define kszLinekeyType "LinekeyType"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | linekey.%%d.label
 *         path | /config/user/linekey.ini
 *      section | key%%d
 *          key | label
 *      default | ""
 */
#define kszLinekeyLabel "LinekeyLabel"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | linekey.%%d.value
 *         path | /config/user/linekey.ini
 *      section | key%%d
 *          key | value
 *      default | ""
 */
#define kszLinekeyValue "LinekeyValue"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | linekey.%%d.line
 *         path | /config/user/linekey.ini
 *      section | key%%d
 *          key | line
 *      default | ""
 */
#define kszLinekeyLine "LinekeyLine"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | linekey.%%d.extension
 *         path | /config/user/linekey.ini
 *      section | key%%d
 *          key | extension
 *      default | ""
 */
#define kszLinekeyExtention "LinekeyExtention"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | linekey.%%d.shortlabel
 *         path | /config/user/linekey.ini
 *      section | key%%d
 *          key | shortlabel
 *      default | ""
 */
#define kszLinekeyShortLabel "LinekeyShortLabel"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | programablekey.%%d.type
 *         path | /config/user/programkey.ini
 *      section | key%%d
 *          key | type
 *      default | ""
 */
#define kszProgramkeyType "ProgramkeyType"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | programablekey.%%d.label
 *         path | /config/user/programkey.ini
 *      section | key%%d
 *          key | label
 *      default | ""
 */
#define kszProgramkeyLabel "ProgramkeyLabel"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | programablekey.%%d.value
 *         path | /config/user/programkey.ini
 *      section | key%%d
 *          key | value
 *      default | ""
 */
#define kszProgramkeyValue "ProgramkeyValue"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | programablekey.%%d.line
 *         path | /config/user/programkey.ini
 *      section | key%%d
 *          key | line
 *      default | ""
 */
#define kszProgramkeyLine "ProgramkeyLine"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | programablekey.%%d.extension
 *         path | /config/user/programkey.ini
 *      section | key%%d
 *          key | extension
 *      default | ""
 */
#define kszProgramkeyExtention "ProgramkeyExtention"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.%%d.key.%%d.type
 *         path | /config/user/expkey%%d.ini
 *      section | key%%d
 *          key | type
 *      default | ""
 *       offset | "1"
 */
#define kszExpkeyType "ExpkeyType"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.%%d.key.%%d.label
 *         path | /config/user/expkey%%d.ini
 *      section | key%%d
 *          key | label
 *      default | ""
 *       offset | 1
 */
#define kszExpkeyLabel "ExpkeyLabel"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.%%d.key.%%d.value
 *         path | /config/user/expkey%%d.ini
 *      section | key%%d
 *          key | value
 *      default | ""
 *       offset | 1
 */
#define kszExpkeyValue "ExpkeyValue"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.%%d.key.%%d.line
 *         path | /config/user/expkey%%d.ini
 *      section | key%%d
 *          key | line
 *      default | ""
 *       offset | 1
 */
#define kszExpkeyLine "ExpkeyLine"

/**
 * @brief linekey type
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | expansion_module.%%d.key.%%d.extension
 *         path | /config/user/expkey%%d.ini
 *      section | key%%d
 *          key | extension
 *      default | ""
 *       offset | 1
 */
#define kszExpkeyExtention "ExpkeyExtention"

/**
 * @brief 话机支持的linekey数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.linkeynum
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | LinekeyNum
 *      default | "0"
 */
#define kszLinekeyNum "LinekeyNum"

/**
 * @brief 话机支持的programable数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.programablekeynum
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | ProgrameKeyNum
 *      default | "14"
 */
#define kszProgrameKeyNum "ProgrameKeyNum"

/**
 * @brief 话机支持的exp 数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.expnum
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | ExpNum
 *      default | "6"
 */
#define kszExpNum "ExpNum"

/**
 * @brief 话机支持的exp key数量
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.factory.expkeynum
 *         path | /config/system/system.ini
 *      section | Factory
 *          key | ExpKeyNum
 *      default | "40"
 */
#define kszExpKeyNum "ExpKeyNum"

/**
 * @brief Emergency 回呼功能支持与否配置项
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.emergency_calling.enable
 *         path | /config/user/user.ini
 *      section | Directory
 *          key | bBSFTEmergencyCallingEnable
 *      default | "0"
 */
#define kszBroadsoftEmergencyCalling "BSFTEmergencyCalling"

/**
 * @brief Autoset Favorite策略dsskey排布顺序
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.favorite_sequence_type
 *         path | /config/user/user.ini
 *      section | SequenceType
 *          key | Favorite
 *      default | "0"
 */
#define kszFavoriteSequenceType "FavoriteSequenceType"

/**
 * @brief Autoset UCFavorite策略dsskey排布顺序
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.uc_favorite_sequence_type
 *         path | /config/user/user.ini
 *      section | SequenceType
 *          key | UCFavorite
 *      default | "0"
 */
#define kszUCFavoriteSequenceType "UCFavoriteSequenceType"

/**
 * @brief Autoset 策略优先级动态可调
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.keytype_sequence
 *         path | /config/user/user.ini
 *      section | SequenceType
 *          key | Strategy
 *      default | ""
 */
#define kszStrategySequence "StrategySequence"

/**
 * @brief AMR授权控制配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.power.amr.enable
 *         path | /phone/factory/system/system.ini
 *      section | PowerAward
 *          key | amr
 *      default | "0"
 */
#define kszPowerAMREnable "PowerAMREnable"

/**
 * @brief 控制网页是否开放AMR License功能
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.wui.features.amrlicense.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | AMRLicenseEnable
 *      default | 0
 */
#define kszAMRLicenseEnable "AMRLicenseEnable"

/**
 * @brief XML Phonebook自动搜索时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | push_xml.phonebook.search.delay
 *         path | /config/user/user.ini
 *      section | XmlBrowser
 *          key | PhonebookSearchDelay
 *      default | 1000
 */
#define kszXMLPhonebookSearchDelay "XMLPhonebookSearchDelay"

/**
 * @brief XML Phonebook提示窗延迟显示时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.xml_browser.loading_tip.delay
 *         path | /config/user/user.ini
 *      section | XmlBrowser
 *          key | LoadingTipDelay
 *      default | 100
 */
#define kszXMLLoadingTipDelay "XMLLoadingTipDelay"

/**
 * @brief 通话中修改的DND状态是否需要在通话结束后保留
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.keep_dnd.enable
 *         path | /config/user/user.ini
 *      section | DND
 *          key | bKeepDNDAfterTalk
 *      default | 1
 */
#define kszKeepDNDAfterTalk "KeepDNDAfterTalk"

/**
 * @brief 通话结束后重置免提通话音量
 *
 * 值为0时不重置，1-15时重置为指定值
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handfree.autoreset_spk_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkResetHandfree
 *      default | 0
 */
#define kszVolumeTalkResetHandfree "VolumeTalkResetHandfree"

/**
 * @brief 通话结束后重置手柄通话音量
 *
 * 值为0时不重置，1-15时重置为指定值
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.handset.autoreset_spk_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkResetHandset
 *      default | 0
 */
#define kszVolumeTalkResetHandset "VolumeTalkResetHandset"

/**
 * @brief 通话结束后重置耳麦通话音量
 *
 * 值为0时不重置，1-15时重置为指定值
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.headset.autoreset_spk_vol
 *         path | /config/user/user.ini
 *      section | Volume
 *          key | nTalkResetHeadset
 *      default | 0
 */
#define kszVolumeTalkResetHeadset "VolumeTalkResetHeadset"

/**
 * @brief Genband E911 Location 地址
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gb.emergency.location_address
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | EmergencyAddress
 *      default | ""
 */
#define kszE911LocationAddr "E911LocationAddr"

/**
 * @brief Genband E911 Location ID
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gb.emergency.location_id
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | EmergencyLocationID
 *      default | ""
 */
#define kszE911LocationID "E911LocationID"

/**
 * @brief   Genband 指定账号信息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | dialplan.emergency.asserted_id.sip_account
 *         path | /config/user/user.ini
 *      section | Emergency
 *          key | EmergencyAccountID
 *      default | "1"
 */
#define kszEmergencyAccountID "EmergencyAccountID"

/**
 * @brief Genband E911 请求地址url
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gb.emergency.sopi.url
 *         path | /config/user/user.ini
 *      section | Genband
 *          key | EmergencyUrl
 *      default | ""
 */
#define kszE911SopiUrl "E911SopiUrl"

/**
 * @brief 话机是否可以作为source发送loopback通话
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.loopback.enable
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | bLoopBackEnable
 *      default | 0
 */
#define kszLoopBackEnable "LoopBackEnable"

/**
 * @brief loopback来电话机是否要自动接起
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.loopback.auto_answer.mode
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | LoopBackAAMode
 *      default | 0
 */
#define kszLoopBackAAMode "LoopBackAutoAnswerMode"

/**
 * @brief call log能够记录全程的信息功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.calllog_detailed_information
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bDetailInfoEnable
 *      default | 1
 */
#define kszDetailInfoEnable "DetailInfoEnable"

/**
 * @brief 启用自定义softkey的Call states
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.custom_softkey.apply_to_states
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SoftkeyApplyStates
 *      default | ""
 */
#define kszSoftkeyApplyStates "SoftkeyApplyStates"

/**
 * @brief Hoteling和flexible seating功能默认在主屏幕上显示的softkey按键是否要显示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.homescreen_softkey.hoteling.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SoftkeyIdleHotelingEnable
 *      default | 1
 */
#define kszSoftkeyIdleHotelingEnable "SoftkeyIdleHotelingEnable"

/**
 * @brief ACD功能默认在主屏幕上显示的softkey按键是否要显示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.homescreen_softkey.acd.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | SoftkeyIdleACDEnable
 *      default | 1
 */
#define kszSoftkeyIdleACDEnable "SoftkeyIdleACDEnable"

/**
 * @brief 指定IPv4/IPv6优先级
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.preference
 *         path | /config/system/system.ini
 *      section | Network
 *          key | net_preference
 *      default | 0
 */
#define kszNetWorkPreference "NetWorkPreference"

/**
 * @brief 网络会议是否显示通话成员, 以及是否有成员管理功能
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.bw.conference.member_manage.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | BroadSoft
 *          key | NetConfMemberManagerEnable
 *      default | 0
 */
#define kszNetConfMemberManagerEable "NetConfMemberManagerEnable"

/**
 * @brief 耳麦键是否能够控制通话功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.headset.ctrl_call.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HeadsetCtrlCall
 *      default | 0
 */
#define kszHeadsetCtrlCall "HeadsetCtrlCall"

/**
 * @brief 开关usb口（对所有usb口都生效）
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | usb.power.enable
 *         path | /config/system/system.ini
 *      section | Phone_Func
 *          key | usb_enable
 *      default | 1
 */
#define kszUsbPortEnable "UsbPortEnable"

/**
 * @brief wifi功能是否启用开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.function.enable
 *         path | /config/system/system.ini
 *      section | Phone_Func
 *          key | wifi_enable
 *      default | 1
 */
#define kszWifiFuncEnable "WifiFuncEnable"

/**
 * @brief 蓝牙功能是否启用开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bluetooth.function.enable
 *         path | /config/system/system.ini
 *      section | Phone_Func
 *          key | bluetooth_enable
 *      default | 1
 */
#define kszBluetoothFuncEnable "BluetoothFuncEnable"

/**
 * @brief 摄像头功能是否启用开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | camera.function.enable
 *         path | /config/system/system.ini
 *      section | Phone_Func
 *          key | camera_enable
 *      default | 1
 */
#define kszCameraFuncEnable "CameraFuncEnable"

/**
 * @brief 来电时屏蔽reject功能
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.incoming_call.reject.enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | RejectIncomingCallSwitch
 *      default | 1
 */
#define kszRejectIncomingCallSwitch "RejectIncomingCallSwitch"

/**
 * @brief StationName滚动显示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.station_name.scrolling_display
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bStationNameScrolling
 *      default | 1
 */
#define kszStationNameScrolling "StationNameScrolling"

/**
 * @brief 是否支持蓝牙通话
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bluetooth.hfp_ag.call.enable
 *         path | /config/user/user.ini
 *      section | Bluetooth
 *          key | hfp_ag_call_enable
 *      default | 0
 */
#define kszBluetoothSupportTalk "BluetoothSupportTalk"

/**
 * @brief 是否支持蓝牙联系人
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.gui.bluetooth.support_contact
 *         path | /config/user/user.ini
 *      section | Bluetooth
 *          key | bBtSupportContact
 *      default | 0
 */
#define kszBluetoothSupportContact "BluetoothSupportContact"

/**
 * @brief wifi密码输入法默认配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | default_input_method.wifi_password
 *         path | /config/user/user.ini
 *      section | Features
 *          key | WifiPasswordIME
 *      default | abc
 */
#define kszWifiPasswordIME "WifiPasswordIME"

/**
 * @brief 搜索结果是否高亮显示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.search.highlight_keywords.enable
 *         path | /config/user/user.ini
 *      section | SuperSearch
 *          key | bHighLight
 *      default | 0
 */
#define kszEnableHighLight "bHighLight"

/**
 * @brief Dsskey label对齐方式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.dsskey_label.display_method
 *         path | /config/user/user.ini
 *      section | Features
 *          key | DsskeyAlignMode
 *      default | 0
 */
#define kszDsskeyShowType "DsskeyAlignMode"

/**
 * @brief 是否开启虚拟账号
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.enable
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | Enable
 *      default | 0
 */
#define kszVirXsiAccountEnable "bw.virtual_user.%d.enable"

/**
 * @brief 虚拟账号显示名称
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.label
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | Label
 *      default | ""
 */
#define kszVirXsiAccountLabel "bw.virtual_user.%d.label"

/**
 * @brief 虚拟账号userid
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.user
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | XSIUserID
 *      default | ""
 */
#define kszVirXsiAccountUserID "bw.virtual_user.%d.xsi.user"

/**
 * @brief 虚拟账号密码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.password
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | XSIPassword
 *      default | ""
 *      encrypt | True
 */
#define kszVirXsiAccountPassword "bw.virtual_user.%d.xsi.password"

/**
 * @brief 虚拟账号host地址
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.host
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | XSIHostAddr
 *      default | ""
 */
#define kszVirXsiAccountHost "bw.virtual_user.%d.xsi.host"

/**
 * @brief 虚拟账号服务器类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.server_type
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | XSIServerType
 *      default | ""
 */
#define kszVirXsiAccountServerType "bw.virtual_user.%d.xsi.server_type"

/**
 * @brief 虚拟账号服务器端口号
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.port
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | XSIPort
 *      default | 80
 */
#define kszVirXsiAccountPort "bw.virtual_user.%d.xsi.port"

/**
 * @brief 虚拟账号自定义url
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.custom_url
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | XSICustomURL
 *      default | ""
 */
#define kszVirXsiAccountCustomUrl "bw.virtual_user.%d.xsi.custom_url"

/**
 * @brief 虚拟账号dnd开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.virtual_user.%%d.xsi.dnd.enable
 *         path | /config/user/voip/virxsiAccount.cfg
 *      section | Account%%d
 *          key | DNDEable
 *      default | 0
 */
#define kszVirXsiAccountDND "bw.virtual_user.%d.xsi.dnd.enable"

/**
 * @brief XmlBrowser Hoteling鉴权方式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | hoteling.authentication_mode
 *         path | /config/user/user.ini
 *      section | Hoteling
 *          key | AuthMode
 *      default | 0
 */
#define kszHotelingAuthMode "HotelingAuthMode"

/**
 * @brief 网页是否默认优先使用https访问
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.web.onlyhttps
 *         path | /phone/factory/system/system.ini
 *      section | HttpServer
 *          key | bOnlyHttps
 *      default | 0
 */
#define kszWebOnlyHttps "WebOnlyHttps"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.extra_attr
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strExtraAttr
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPExtraAttr "LdapExtraAttr"

/**
 * @brief 注释
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | ldap.display_extra_attr
 *         path | /config/user/LDAP.cfg
 *      section | LDAP
 *          key | strDisplayExtraAttr
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszLDAPDisplayExtraAttr "LdapDisplayExtraAttr"

/**
 * @brief Broadsoft联系人是自动更新，还是进入才去下载
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | bw.xsi.directory.update.enable
 *         path | /config/user/user.ini
 *      section | BroadSoft
 *          key | bUpdateEnable
 *      default | 1
 */
#define kszBsftDirUpdateEnable "BsftDirUpdateEnable"

/**
 * @brief XML Browser是否默认响Beep
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.xml_browser_beep.enable
 *         path | /config/user/user.ini
 *      section | XmlBrowser
 *          key | bBeepTone
 *      default | 1
 */
#define kszXMLBrowserBeep "XMLBrowserBeep"

/**
 * @brief 长按查看BLF通话信息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.blf.show_callinfo.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | bShowBLFCallid
 *      default | 0
 */
#define kszBLFShowCallId "BLFShowCallId"

/**
 * @brief 长按查看BLF通话信息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.incoming_call_display_name.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | IncomeCallDisplayName
 *      default | 0
 */
#define kszIncomeCallDisplayName "IncomeCallDisplayName"

/**
 * @brief 通话中硬件Hold键是否生效
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.keypad.hold.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | EnableHoldFunKeyInTalk
 *      default | 1
 */
#define kszEnableHoldFunKeyInTalk "EnableHoldFunKeyInTalk"

/**
 * @brief 超时未接返回code
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.normal_busy_code
 *         path | /config/user/user.ini
 *      section | Features
 *          key | NormalBusyRefuseCode
 *      default | 486
 */
#define kszNormalBusyRefuseCode "NormalBusyRefuseCode"

/**
 * @brief 超时未接返回code
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.congestion_tone.codelist
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CongestionToneCodeList
 *      default | ""
 */
#define kszCongestionToneCodeList "CongestionToneCodeList"

/**
 * @brief 隐藏未读voice mail数量提示
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hide_unread_vm_number.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | HideUnreadVMNumber
 *      default | 0
 */
#define kszHideUnreadVMNumber "HideUnreadVMNumber"

/**
 * @brief 来电记录替换规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.local_calllog.received.replace_rule
 *         path | /config/user/user.ini
 *      section | Features
 *          key | LocalCalllogReplaceRule
 *      default | ""
 */
#define kszLocalCalllogReplaceRule "LocalCalllogReplaceRule"

/**
 * @brief 来电记录替换规则
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | sip.dhcp.option120.mode
 *         path | /config/system/system.ini
 *      section | sipServerOptions
 *          key | DHCPOpt120Mode
 *      default | 0
 */
#define kszDhcpOptionMode "DhcpOptionMode"

/**
 * @brief 是否保留证书配置
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.reserve_certs_config.enable
 *         path | /config/user/user.ini
 *      section | PhoneSetting
 *          key | bEnableSaveCertConfig
 *      default | 0
 */
#define kszReserveCertsConfig "ReserveCertsConfig"

/**
 * @brief U盘功能开关
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | usbdisk.function.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | USBDiskFunctionEnable
 *      default | 1
 */
#define kszUSBDiskFunctionEnable "USBDiskFunctionEnable"

/**
 * @brief Reset local settings菜单项从Advanced移至Basic
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.basic_menu.reset_local_config.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | ShowResetLocalInBasic
 *      default | 0
 */
#define kszShowResetLocalInBasic "ShowResetLocalInBasic"

/**
 * @brief AutoP鉴权窗口多长时间后自动消失
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | auto_provision.authentication.expired_time
 *         path | /config/system/system.ini
 *      section | AutoProvision
 *          key | AuthExpiredTime
 *      default | 7200
 */
#define kszAuthExpiredTime "AuthExpiredTime"

/**
 * @brief 网页Meminfo页面刷新时间
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.web.status.meminfo.intervaltime
 *         path | /config/system/system.ini
 *      section | WebOptions
 *          key | TopIntervalTime
 *      default | 0
 */
#define kszTopIntervalTime "TopIntervalTime"

/**
 * @brief 网页Configuration页面抓包类型
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.web.setting.pcap_type
 *         path | /config/system/system.ini
 *      section | WebOptions
 *          key | PcapType
 *      default | 1
 */
#define kszPcapType "PcapType"

/**
 * @brief 是否开启自动化测试
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.autotest.switch
 *         path | /config/system/system.ini
 *       section | Features
 *          key | AutoTestSwitch
 *      default | 0
 */
#define kszAutoTestSwitch "AutoTestSwitch"

/**
 * @brief ume 绑定用户，唯一标识
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.ume.bind.name
 *         path | /config/user/user.ini
 *      section | ActionUrl
 *          key | UMEBindName
 *      default | ""
 */
#define kszUMEBindName "UMEBindName"

/**
 * @brief CP920的按键touch音
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.touch_tone
 *         path | /config/user/user.ini
 *       section | Volume
 *          key | bTouchSoundOn
 *      default | 1
 */
#define kszTouchSound "TouchSoundOn"

/**
 * @brief (android) AccountCloudPassword
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.cloud.password
 *      default | ""
 *          reg | {maxlength:99}
 */
#define kszAccountCloudPassword "AccountCloudPassword"

/**
 * @brief (android) AccountCloudUserName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.cloud.user_name
 *      default | ""
 *          reg | {not:{regular:'[:;&amp;lt;&amp;gt;[\]?%\s]'},maxlength:32}
 */
#define kszAccountCloudUserName "AccountCloudUserName"

/**
 * @brief (android) AlwaysFwdToVmOffCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.always_fwd_to_vm.off_code
 *      default | ""
 */
#define kszAlwaysFwdToVmOffCode "AlwaysFwdToVmOffCode"

/**
 * @brief (android) AlwaysFwdToVmOnCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.always_fwd_to_vm.on_code
 *      default | ""
 */
#define kszAlwaysFwdToVmOnCode "AlwaysFwdToVmOnCode"

/**
 * @brief (android) AnonymousRejectionMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.anonymous_call_rejection.mode
 *      default | 0
 */
#define kszAnonymousRejectionMode "AnonymousRejectionMode"

/**
 * @brief (android) AnonymousRejectionOffCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.anonymous_call_rejection.off_code
 *      default | ""
 */
#define kszAnonymousRejectionOffCode "AnonymousRejectionOffCode"

/**
 * @brief (android) AnonymousRejectionOnCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.anonymous_call_rejection.on_code
 *      default | ""
 */
#define kszAnonymousRejectionOnCode "AnonymousRejectionOnCode"

/**
 * @brief (android) AppSystemuiList 
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.app_systemui_list
 *      default | ""
 */
#define kszAppSystemuiList "AppSystemuiList"

/**
 * @brief (android) AppWhiteList
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.app_white_list
 *      default | ""
 */
#define kszAppWhiteList "AppWhiteList"

/**
 * @brief (android) BLUETOOTHFORBID
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.ui.features.bluetooth_forbid
 *      default | 0
 */
#define kszBluetoothforbid "BLUETOOTHFORBID"

/**
 * @brief (android) BrowserUrl
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.browser.url
 *      default | http://www.yealink.com
 */
#define kszBrowserUrl "BrowserUrl"

/**
 * @brief (android) BusyFwdToVmOffCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.busy_fwd_to_vm.off_code
 *      default | ""
 */
#define kszBusyFwdToVmOffCode "BusyFwdToVmOffCode"

/**
 * @brief (android) BusyFwdToVmOnCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.busy_fwd_to_vm.on_code
 *      default | ""
 */
#define kszBusyFwdToVmOnCode "BusyFwdToVmOnCode"

/**
 * @brief (android) CloudDirUpdateInterVal
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | cloud.directory.update_time_interval
 *      default | 2
 */
#define kszCloudDirUpdateInterVal "CloudDirUpdateInterVal"

/**
 * @brief (android) CustomStaticSystemMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.static.system_mode
 *      default | 0
 */
#define kszCustomStaticSystemMode "CustomStaticSystemMode"

/**
 * @brief (android) CustomStaticSystemModeEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.static.system_mode_enable
 *      default | 0
 */
#define kszCustomStaticSystemModeEnable "CustomStaticSystemModeEnable"

/**
 * @brief (android) CustomStaticSystemModeSelection
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.static.system_mode_selection
 *      default | 0
 */
#define kszCustomStaticSystemModeSelection "CustomStaticSystemModeSelection"

/**
 * @brief (android) DoorPhoneAmount
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.amount
 *      default | 2
 */
#define kszDoorPhoneAmount "DoorPhoneAmount"

/**
 * @brief (android) DoorPhoneAutoPreviewEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.autopreview.enable
 *      default | 0
 */
#define kszDoorPhoneAutoPreview "DoorPhoneAutoPreviewEnable"

/**
 * @brief (android) DoorPhoneAutoVideoViewEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.autovideoview.enable
 *      default | 1
 */
#define kszDoorPhoneAutoVideoViewEnable "DoorPhoneAutoVideoViewEnable"

/**
 * @brief (android) DoorPhoneDeviceModel
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.device_model
 *      default | 0
 */
#define kszDoorPhoneDeviceModel "DoorPhoneDeviceModel"

/**
 * @brief (android) DoorPhoneDisplayName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.display_name
 *      default | ""
 */
#define kszDoorPhoneDisplayName "DoorPhoneDisplayName"

/**
 * @brief (android) DoorPhoneFullScreen
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.full_screen
 *      default | 1
 */
#define kszDoorPhoneFullScreen "DoorPhoneFullScreen"

/**
 * @brief (android) DoorPhoneHttpApiPassWord
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.httpapi.password
 *      default | ""
 */
#define kszDoorPhoneHttpApiPassWord "DoorPhoneHttpApiPassWord"

/**
 * @brief (android) DoorPhoneHttpApiUserName
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.httpapi.username
 *      default | ""
 */
#define kszDoorPhoneHttpApiUserName "DoorPhoneHttpApiUserName"

/**
 * @brief (android) DoorPhoneNumber
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.phone_number
 *      default | ""
 */
#define kszDoorPhoneNumber "DoorPhoneNumber"

/**
 * @brief (android) DoorPhoneSendAudio
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.send_audio
 *      default | 1
 */
#define kszDoorPhoneSendAudio "DoorPhoneSendAudio"

/**
 * @brief (android) DoorPhoneSendVideo
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.send_video
 *      default | 1
 */
#define kszDoorPhoneSendVideo "DoorPhoneSendVideo"

/**
 * @brief (android) DoorPhoneUnlockPin
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.unlock_pin
 *      default | ""
 */
#define kszDoorPhoneUnlockPin "DoorPhoneUnlockPin"

/**
 * @brief (android) DoorPhoneVideoPreviewEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.videopreview.enable
 *      default | 0
 */
#define kszDoorPhoneVideoPreview "DoorPhoneVideoPreviewEnable"

/**
 * @brief (android) DoorPhoneVideoStreamHttpUrl
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.doorphone.%%d.video.stream.httpurl
 *      default | ""
 */
#define kszDoorPhoneVideoStreamHttpUrl "DoorPhoneVideoStreamHttpUrl"

/**
 * @brief (android) DsskeyMissCallEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.dsskey_miss_call_enable
 *      default | 0
 */
#define kszDsskeyMissCallEnable "DsskeyMissCallEnable"

/**
 * @brief (android) DtmfFlash
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.dtmf_flash
 *      default | 0
 */
#define kszDtmfFlash "DtmfFlash"

/**
 * @brief (android) DtmfRelay
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.relay
 *      default | 3
 */
#define kszDtmfRelay "DtmfRelay"

/**
 * @brief (android) DtmfVolume
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dtmf.volume
 *      default | -10
 */
#define kszDtmfVolume "DtmfVolume"

/**
 * @brief (android) EmergencyUsePrimaryAcc
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.emergency_call_keep_first_account.enable
 *      default | 0
 */
#define kszEmergencyUsePrimaryAcc "EmergencyUsePrimaryAcc"

/**
 * @brief (android) EnableBlueToothInTalk
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.bluetooth_talk.enable
 *      default | 1
 */
#define kszEnableBlueToothInTalk "EnableBlueToothInTalk"

/**
 * @brief (android) EnableNeutralCallPark
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_park.neutral.enable
 *      default | 1
 */
#define kszEnableNeutralCallPark "EnableNeutralCallPark"

/**
 * @brief (android) EnablePowerSavingSrceenProtect
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.features.powersaving_srceen_protect
 *      default | 1
 */
#define kszEnablePowerSavingSrceenProtect "EnablePowerSavingSrceenProtect"

/**
 * @brief (android) ExitDndModeEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.exit_dnd_mode_enable
 *      default | 1
 */
#define kszExitDndModeEnable "ExitDndModeEnable"

/**
 * @brief (android) FactoryResetExternalSD
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.factory_reset_external_sd
 *      default | 0
 */
#define kszFactoryResetExternalSD "FactoryResetExternalSD"

/**
 * @brief (android) FactoryResetInternalSD
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.factory_reset_internal_sd
 *      default | 0
 */
#define kszFactoryResetInternalSD "FactoryResetInternalSD"

/**
 * @brief (android) FactoryResetStorageUSB
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.factory_reset_usb_storage
 *      default | 0
 */
#define kszFactoryResetStorageUSB "FactoryResetStorageUSB"

/**
 * @brief (android) FeaturesActionCallAnswer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_call_answer
 *      default | ""
 */
#define kszFeaturesActionCallAnswer "FeaturesActionCallAnswer"

/**
 * @brief (android) FeaturesActionContact
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_contact
 *      default | ""
 */
#define kszFeaturesActionContact "FeaturesActionContact"

/**
 * @brief (android) FeaturesActionDialer
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_dialer
 *      default | ""
 */
#define kszFeaturesActionDialer "FeaturesActionDialer"

/**
 * @brief (android) FeaturesActionDsskey
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_dsskey
 *      default | ""
 */
#define kszFeaturesActionDsskey "FeaturesActionDsskey"

/**
 * @brief (android) FeaturesActionHistory
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.action_history
 *      default | ""
 */
#define kszFeaturesActionHistory "FeaturesActionHistory"

/**
 * @brief (android) FullScreenInCallEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.full_screen_in_call_enable
 *      default | 1
 */
#define kszFullScreenInCallEnable "FullScreenInCallEnable"

/**
 * @brief (android) HideUrlDsskeyIcon
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.hide_url_dsskey_icon.enable
 *      default | 0
 */
#define kszHideUrlDsskeyIcon "HideUrlDsskeyIcon"

/**
 * @brief (android) NetworkAudioTOS
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.qos.audiotos
 *      default | 46
 *          reg | {required:true,range:[0,63],maxlength:2}
 */
#define kszNetworkAudioTOS "NetworkAudioTOS"

/**
 * @brief (android) NetworkVideoTOS
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | network.qos.videotos
 *      default | 46
 *          reg | {required:true,range:[0,63],maxlength:2}
 */
#define kszNetworkVideoTOS "NetworkVideoTOS"

/**
 * @brief (android) NoAswFwdToVmOffCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.timeout_fwd_to_vm.off_code
 *      default | ""
 */
#define kszNoAswFwdToVmOffCode "NoAswFwdToVmOffCode"

/**
 * @brief (android) NoAswFwdToVmOnCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.timeout_fwd_to_vm.on_code
 *      default | ""
 */
#define kszNoAswFwdToVmOnCode "NoAswFwdToVmOnCode"

/**
 * @brief (android) OutboundCallIdMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.calling_line_id_delivery_blocking.mode
 *      default | 0
 */
#define kszOutboundCallIdMode "OutboundCallIdMode"

/**
 * @brief (android) OutboundCallIdOffCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.calling_line_id_delivery_blocking.off_code
 *      default | ""
 */
#define kszOutboundCallIdOffCode "OutboundCallIdOffCode"

/**
 * @brief (android) OutboundCallIdOnCode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.calling_line_id_delivery_blocking.on_code
 *      default | ""
 */
#define kszOutboundCallIdOnCode "OutboundCallIdOnCode"

/**
 * @brief (android) PermitSilentMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.permit_silent_mode.enable
 *      default | 1
 */
#define kszPermitSilentMode "PermitSilentMode"

/**
 * @brief (android) ResetMediaCapsetAfterReboot
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.reset_mediacapset_after_reboot.enable
 *      default | 1
 */
#define kszResetMediaCapsetAfterReboot "ResetMediaCapsetAfterReboot"

/**
 * @brief (android) ResolutionType
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.resolution.type
 *      default | 0
 */
#define kszResolutionType "ResolutionType"

/**
 * @brief (android) SCAHistoryEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.sync_sca_call_log.enable
 *      default | 0
 */
#define kszSCAHistoryEnable "SCAHistoryEnable"

/**
 * @brief (android) ScreenSaverMode
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | custom.phone_setting.screen_saver_mode
 *      default | 0
 */
#define kszScreenSaverMode "ScreenSaverMode"

/**
 * @brief (android) ShowOtherDisplayInCall
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.show_other_dispaly_incall
 *      default | 0
 */
#define kszShowOtherDisplayInCall "ShowOtherDisplayInCall"

/**
 * @brief (android) StatusBarHide
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.status_bar.hide
 *      default | 0
 */
#define kszStatusBarHide "StatusBarHide"

/**
 * @brief (android) SystemFuntionBarHide
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.system_funtion_bar.hide
 *      default | 0
 */
#define kszSystemFuntionBarHide "SystemFuntionBarHide"

/**
 * @brief (android) TestModePicUrl
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.test_picture_url
 *      default | ""
 */
#define kszTestModePicUrl "TestModePicUrl"

/**
 * @brief (android) UseLocalDND
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.dnd_local.enable
 *      default | 0
 */
#define kszUseLocalDND "UseLocalDND"

/**
 * @brief (android) VCAccountActive
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | vcaccount.active
 *      default | 1
 */
#define kszVCAccountActive "VCAccountActive"

/**
 * @brief (android) VCAccountEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | vcaccount.enable
 *      default | 0
 */
#define kszVCAccountEnable "VCAccountEnable"

/**
 * @brief (android) VideoCallBandwidth
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.video_call_bandwidth
 *      default | 0
 */
#define kszVideoCallBandwidth "VideoCallBandwidth"

/**
 * @brief (android) VideoGop
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.video.gop
 *      default | 0
 */
#define kszVideoGop "VideoGop"

/**
 * @brief (android) VoiceAnsEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.ans.enable
 *      default | 1
 */
#define kszVoiceAnsEnable "VoiceAnsEnable"

/**
 * @brief (android) VoiceAnsNbEnalbe
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.ans_nb.enable
 *      default | 0
 */
#define kszVoiceAnsNbEnalbe "VoiceAnsNbEnalbe"

/**
 * @brief (android) VoiceTnsEnable
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | voice.tns.enable
 *      default | 1
 */
#define kszVoiceTnsEnable "VoiceTnsEnable"

/**
 * @brief (android) VoipBfcpSwitch
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.17.bfcp.enable
 *      default | 0
 */
#define kszVoipBfcpSwitch "VoipBfcpSwitch"

/**
 * @brief (android) WIFIFORBID
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.ui.features.wifi_forbid
 *      default | 0
 */
#define kszWififorbid "WIFIFORBID"

/**
 * @brief (android) WebServerBusyDuration
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.web.fcgiserver.busy_duration
 *      default | 0
 */
#define kszWebServerBusyDuration "WebServerBusyDuration"

/**
 * @brief (android) WiFiStatusDetectionTimeout
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | wifi.status_detection_timeout
 *      default | 120
 *          reg | {required:true,range:[0,4294967295,120],maxlength:10}
 */
#define kszWiFiStatusDetectionTimeout "WiFiStatusDetectionTimeout"

/**
 * @brief (android) WorldClockHidden
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.hide_world_clock.enable
 *      default | 0
 */
#define kszWorldClockHidden "WorldClockHidden"

/**
 * @brief (android) WuiRfTest
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.rf_flag
 *      default | 1
 */
#define kszWuiRfTest "WuiRfTest"

/**
 * @brief (android) WuiSupportCloud
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | priv.wui.features.cloud
 *      default | 0
 */
#define kszWuiSupportCloud "WuiSupportCloud"

/**
 * @brief (sip phone) account feature key sync
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%d%.feature_key_sync.enable
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | bBroadsoftFeatureKeySync
 *      default | 0
 */
#define kszAccountFeatureKeySync "AccountFeatureKeySync"

/**
 * @brief (sip phone) account fwd feature key sync
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.forward.feature_key_sync.enable 
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | bForwardFeatureKeySync
 *      default | 0
 */
#define kszAccountForwardFeatureKeySync "AccountForwardFeatureKeySync"

/**
 * @brief 是否开启calllog合并显示模式
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.call_log_merge.enable
 *         path | /config/user/user.ini
 *      section | Features
 *          key | CalllogMergeEnable
 *      default | 0
 */
#define kszCalllogMergeEnable "CalllogMergeEnable"

/**
 * @brief 警告显示模式是否开启
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | phone_setting.warnings_display.mode
 *         path | /config/user/user.ini
 *      section | Features
 *          key | WarningDisplayMode
 *      default | 0
 */
#define kszWarningDisplayMode "WarningDisplayMode"

/**
 * @brief 定义对应账号下的BLF List中，哪些对象在通话中点击对应DSSKey后会执行一键park
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_list_call_parked_list
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BlflistForCallParklist
 *      default | ""
 */
#define kszBlflistForCallParklist "BlflistForCallParklist"

/**
 * @brief 账号的BLF List park特征码
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.blf_list_call_parked_code
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | BlflistForCallParkCode
 *      default | ""
 */
#define kszBlflistForCallParkCode "BlflistForCallParkCode"

/**
 * @brief 设置当features.pickup.blf_visual_enable = 1时，哪些BLF的号码，执行BLF visual alert
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | features.pickup.blf_visual.list
 *         path | /config/user/user.ini
 *      section | Pickup
 *          key | BlfPickupVisualList
 *      default | any
 */
#define kszBlfPickupVisualList "BlfPickupVisualList"

#endif // _CONFIG_ITEMS_DEFINE_HEADER_
