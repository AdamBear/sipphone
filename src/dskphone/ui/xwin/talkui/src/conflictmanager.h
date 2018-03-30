#ifndef _TALK_CONFLICT_MANAGER_H_
#define _TALK_CONFLICT_MANAGER_H_

#include "talkuilogic/talkuilogiccommon.h"
#include "ylstring.h"

typedef void(*FUN_WIDGET_SHOW)(bool bShow);

enum CONFLICT_PRIORITY
{
    CONFLICT_PRIORITY_START = 0,

    CONFLICT_PRIORITY_VOLUMEBAR,
    CONFLICT_PRIORITY_DTMF,
    CONFLICT_PRIORITY_NETWORK,

    //通话界面来电提示
    CONFLICT_PRIORITY_INCOMING_INFO,
    //transfer fail提示
    CONFLICT_PRIORITY_TRANSFAIL_INFO,
    //hold、mute状态提示
    CONFLICT_PRIORITY_STATUS_INFO,

    //来电界面目标号码提示
    CONFLICT_PRIORITY_RINGING_TOLINE_INFO,

    //finish界面提示信息
    CONFLICT_PRIORITY_FINISHI_INFO,

    //网络会议界面成员数量提示
    CONFLICT_NET_CONF_MEMBER_HINT,

    CONFLICT_PRIORITY_CCSTATUS,

    //计时信息
    CONFLICT_PRIORITY_TIME,

    CONFLICT_PRIORITY_END,
};

struct CONFLICT_UNIT
{
    //控件是否处于要显示的状态（不一定是显示）
    bool    m_bShow;
    //控件是否可以在优先级列表中待操作（开关）
    bool    m_bEnable;
    FUN_WIDGET_SHOW m_ShowFunction;

    CONFLICT_UNIT()
    {
        m_bShow = false;
        m_bEnable = true;
        m_ShowFunction = NULL;
    }
};

typedef YLHashMap<CONFLICT_PRIORITY, CONFLICT_UNIT> ConflictMap;

class CTalkConflictManager
{
private:
    CTalkConflictManager(void);

public:
    virtual ~CTalkConflictManager(void);
    static CTalkConflictManager & GetInstance();

public:
    void removeAllUnit();

    void addConflictUnit(CONFLICT_PRIORITY ePriority, CONFLICT_UNIT stUnit);

    void setConflictUnitShow(CONFLICT_PRIORITY ePriority, bool bShow);
    void setConflictUnitEnable(CONFLICT_PRIORITY ePriority, bool bEnable);

    void removeConflictUnit(CONFLICT_PRIORITY ePriority);

    void checkShowUnit();

    CONFLICT_PRIORITY getCurrentShowUnit();

    void setVolumebarVisible(bool bShow);
    bool getVolumebarVisible();

private:
    ConflictMap m_mapConflict;

    CONFLICT_PRIORITY m_eCurrentPriority;

    bool    m_bVolumebarVisible;
};

#define _TalkConflictManager CTalkConflictManager::GetInstance()

#endif //_TALK_CONFLICT_MANAGER_H_

