#ifndef __IO_COMMAND_H__
#define __IO_COMMAND_H__

// IO接口常用值
#define IO_VALUE_OK     "ok"
#define IO_VALUE_ERR    "err"
#define IO_VALUE_ON     "on"
#define IO_VALUE_OFF    "off"

// 上位机发送命令
#define IO_CMD_OPEN         "open"
#define IO_CMD_GET_LINEID   "lineid"
#define IO_CMD_HOOK         "hook"
#define IO_CMD_MUTE         "mute"
#define IO_CMD_DND          "dnd"
#define IO_CMD_HOLD         "hold"
#define IO_CMD_REJECT       "reject"
#define IO_CMD_SET_COUNTRY  "setcountry"
#define IO_CMD_GET_COUNTRY  "getcountry"
#define IO_CMD_DIAL         "dial"
#define IO_CMD_GET_STATE    "state"
#define IO_CMD_GET_VERSION  "ver"
#define IO_CMD_SET_LOGLEVEL "setloglevel"
#define IO_CMD_GET_LOGLEVEL "getloglevel"
#define IO_CMD_SET_LOGMOD   "logmod"
#define IO_CMD_GET_FW_STATE "fwstate"
#define IO_CMD_SET_SN       "setsn"
#define IO_CMD_GET_SN       "getsn"
#define IO_CMD_UPGRADE      "upgrade"
#define IO_CMD_REBOOT       "reboot"
#define IO_CMD_GET_FXOID    "fxoid"
#define IO_CMD_SET_FLASH_DURATION	"flashtime"
// 下位机上报命令
#define IO_CMD_RING         "ring"
#define IO_CMD_CID          "cid"
#define IO_CMD_LINE         "line"
#define IO_CMD_FXO_INIT     "fxoinit"
#define IO_CMD_LOG          "log"

#endif // !__IO_COMMAND_H__
