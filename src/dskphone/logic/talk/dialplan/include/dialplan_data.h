#ifndef __TALK_HELP_DIALPLAN_DATA_H_
#define __TALK_HELP_DIALPLAN_DATA_H_

#include "ylstring.h"
#include "ylvector.h"

// 呼出触发
enum CallOutTrigger
{
    COT_DEFAULT   = 0x00,
    COT_SEND      = 0x01, // 手动呼出
    COT_REDIAL    = 0x02, // Redial呼出
    COT_AUTODIAL  = 0x03, // AutoDial呼出
    COT_HOTLINE   = 0x04, // HotLine呼出
    COT_AUTHCALL  = 0x05, // AuthCall呼出
    COT_DSSKEY    = 0x06, // Dsskey呼出
    COT_PASSIVITY = 0x07, // 被动呼出
    COT_DIALNOW   = 0x08, // DialNow呼出
    COT_DIGITMAP  = 0x09, // DigitMap呼出
};

enum DialPlanCallMask
{
    DPCM_DEFAULT   = 0x00,
    DPCM_FWD       = 0x01 << 0, // FWD呼出
    DPCM_TRANSFER  = 0x01 << 1, // Transfer呼出
};

enum DialPlanReplaceMask
{
    DPRM_NONE        = 0,
    DPRM_DIALRULE    = 0x01 << 0,  // dial rule模式
    DPRM_DIGITMAP    = 0x01 << 1,  // digitmap模式
    DPRM_ALL         = DPRM_DIALRULE | DPRM_DIGITMAP,
};

struct CDialPlanMask
{
    int  iReplaceMask;

    CDialPlanMask() : iReplaceMask(DPRM_ALL)
    {}

    // 位标志操作的一组函数
    bool IsAllowDigitMapReplace() const
    {
        return (iReplaceMask & DPRM_DIGITMAP);
    }
    bool IsAllowDialRuleReplace() const
    {
        return (iReplaceMask & DPRM_DIALRULE);
    }
    bool Empty() const
    {
        return iReplaceMask == DPRM_NONE;
    }
    void RemoveDigitMapFlag()
    {
        iReplaceMask &= ~DPRM_DIGITMAP;
    }
    void RemoveDialRuleFlag()
    {
        iReplaceMask &= ~DPRM_DIALRULE;
    }
    void ClearDialRuleFalg()
    {
        iReplaceMask = DPRM_NONE;
    }
};

typedef struct DialNowData
{
    yl::string      strRule;        // 立即拨号规则
    unsigned short  iLineMask;      // 立即拨号线号掩码

    DialNowData()
    {
        strRule = "";
        iLineMask = 0;
    }
} DialNowData;

struct DialNowListData
{
    YLVector<DialNowData> m_listId;  // BroadSoft contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count.

    DialNowListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listId.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

#endif
