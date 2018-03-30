#ifndef __DSSKEY_DEF_H__
#define __DSSKEY_DEF_H__

#ifndef EFAULT
#   define EFAULT -1
#endif


#define OffsetIndex(index)  ((index) + FIRST_EXPKEY_INDEX)
#define ReMoveOffset(index) ((index) - FIRST_EXPKEY_INDEX)

#if 1
#define  FIRST_LINEKEY_INDEX            10  //linekey的起始序号，在Dsskey的处理中
// yzh t2x是20,t4x是30,这里有两个地方定义这个
#define  FIRST_EXPKEY_INDEX             20  //扩展台的起始序号

#define MAX_DSSANDEXP_KEYS_NUM          (FIRST_EXPKEY_INDEX + MAX_EXP_NUM*40) //EXP39有40个按键
//#define MAX_DSSANDEXP_KEYS_NUM            (FIRST_EXPKEY_INDEX)
#endif

#define MAX_LINEKEY_NUM                 30

enum DSSKEY_ICON_TYPE
{
    DSSKEY_NONE = 0x00,

    DSSKEY_REMOTE_OFFLINE = 0x01,
    DSSKEY_REMOTE_TALKING = 0x02,
    DSSKEY_REMOTE_ONLINE = 0x03,
    DSSKEY_REMOTE_RINGING = 0x04,

    DSSKEY_DND = 0x05,
    DSSKEY_HOLD = 0x06,

    DSSKEY_REGISTERED = 0x07,
    DSSKEY_UNREGISTERED = 0x08,
    DSSKEY_USING = 0x09,
    DSSKEY_REGISTERING = 0x0a,

    DSSKEY_SMS = 0x0b,
    DSSKEY_SPEEDDIAL = 0x0c,

    DSSKEY_TRIANGLE = 0x0d,
    DSSKEY_WARNING = 0x0e,

    DSSKEY_FWD = 0x0f,

    DSSKEY_RINGING,

    DSSKEY_ACD_LOGOUT,
    DSSKEY_ACD_AVAILABLE,
    DSSKEY_ACD_UNAVAILABLE,
    DSSKEY_ACD_WRAPUP,
};

// 内部状态已经和sip状态脱钩，Not 必须和sip统一,本定义有修改需同步修改enum Dsskey_Status
enum BLF_STATUS_CODE
{
    //BLF Status
    CN_BLF_STATUS_IDLE,
    CN_BLF_STATUS_RINGING,
    CN_BLF_STATUS_TALKING,
    CN_BLF_STATUS_UNKOWN,
    CN_BLF_STATUS_HOLD,
    CN_BLF_STATUS_TALKING_ME,
    CN_BLF_STATUS_RINGING_ME,
    CN_BLF_STATUS_PARKED,
    CN_BLF_STATUS_HELD_ME,
    CN_BLF_STATUS_CALLING_ME,
    CN_BLF_STATUS_CALLOUT,        //呼出
    CN_BLF_STATUS_PARK,
    CN_BLF_STATUS_DND,

    // BLA Status
    CN_BLA_STATUS_IDLE,             // BLA IDLE
    CN_BLA_STATUS_SEIZED,           // BLA SEIZE
    CN_BLA_STATUS_PROGRESSING,      // BLA PROGRESSING (OUTGOING)
    CN_BLA_STATUS_ALERTING,         // BLA ALERTING Ringing
    CN_BLA_STATUS_ACTIVE,           // BLA ACTIVE (TALKING)
    CN_BLA_STATUS_HELD,             // BLA HELD (HELD_PUBLIC)
    CN_BLA_STATUS_HELD_PRIVATE,     // BLA HELD_PRIVATE
    CN_BLA_STATUS_BRIDGE_ACITVE,    // BLA BRIDGE_ACITVE
    CN_BLA_STATUS_BRIDGE_HELD,      // BLA BRIDGE_HELD
    CN_BLA_STATUS_UNKOWN,           // BLA UNKOWN
};

#define GetCallPickupStatus(i)  ((i) & 0x80)

//获取BLF后5为的值，Idle,Ring,Talk时增加2种情况的处理，
#define GetBLFStatusNoMode(i)   ((i) & 0x1f)
#define GetBLFMode(i)           ((i) & 0x60)

#define GetBLFStatus(i)         ((i) & 0x7f)


#endif // __DSSKEY_DEF_H__
