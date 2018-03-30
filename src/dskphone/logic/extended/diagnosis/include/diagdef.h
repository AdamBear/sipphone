#ifndef __DIAG_DEF_H__
#define __DIAG_DEF_H__

// 增加类型需要更新到service_dsk_define.h头文件，同步给诊断程序
// report event information
struct EventInfo
{
#define EVENT_TYPE_SIGNED_OUT 0                     // sign out 
#define EVENT_TYPE_USER_CANCELED_SIGN_IN 1
#define EVENT_TYPE_USER_CANCELED_CALL 2
#define EVENT_TYPE_AUTO_DISCOVERY_FAILED 3          // Failure to resolve well know SRV records or auto discover services/server
#define EVENT_TYPE_EXCHANGE_AUTH_FAILED 4           // Failure to authenticate with exchange web services
#define EVENT_TYPE_CALENDAR_SYNC_FAILED 5           // Failure to sync and retrieve calendar data from Exchange failed
#define EVENT_TYPE_GET_VM_FROM_EXCHANGE 6           // Failure to retrieve and list visual voicemail from Exchange
#define EVENT_TYPE_PLAYING_VOICEMAIL_FAILED 7       // Failure to play visual voicemail 
#define EVENT_TYPE_TRAN_FAILED 8                    // Failed transfer attempts 
#define EVENT_TYPE_FWD_FAILED 9                     // Failed call forward attempts
#define EVENT_TYPE_HOLD_FAILED 10                   // Failed to hold the call
#define EVENT_TYPE_RESUME_FAILED 11                 // Failed to resume held call
#define EVENT_TYPE_BOSS_ADMIN_MODE 12               // Whether the device is configured in Boss or Admin mode. iResult(0/1:boss/admin)
#define EVENT_TYPE_DEVICE_UPDATE_POLICY 13          // Device Update policy for IP phones is enabled or disabled. iResult(0/1:disabled/enabled)
#define EVENT_TYPE_DEVICE_UPDATE_FAILED 14          // Update on the device failed.iResult(0/1:failed/success)
#define EVENT_TYPE_DISPLAY_MEETING_ROSTER_FAIL 15   // Failed to display meeting roster with participants’ info
#define EVENT_TYPE_IN_CALL_NETWORK_OUTAGE 16        // Call dropped due to network unavailability 
#define EVENT_TYPE_DEVICE_UP_TIME 17                // Is a measure of time that the device is available without crashing or needing to be rebooted
#define EVENT_TYPE_DROPPED_CALL 18                  // Calls being dropped due to Network connectivity problems, During Handoff for Wifi phones etc
#define EVENT_TYPE_DEVICE_REBOOTED 19               // Device rebooted
#define EVENT_TYPE_DROPPED_CONF_CALL 20             // Conference calls being dropped due to Network connectivity problems, device rebooted mid call etc

    int iEventType; // event type

    long int iValue;
};

#endif // __DIAG_DEF_H__