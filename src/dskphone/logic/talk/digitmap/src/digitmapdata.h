#ifndef __DIAL_DIGITMAP_DATA_H__
#define __DIAL_DIGITMAP_DATA_H__

#include <ylstring.h>
#include <ylvector.h>
#include <ETLLib.hpp>

#ifdef IF_FEATURE_DIGIT_MAP

// 非法时间，默认取最长时间
#define invalid_timer (unsigned short)(-1)

#ifndef AUTO_SELECT_LINE_ID
// 定义自动选择账号的标识
#   define AUTO_SELECT_LINE_ID  -1
#endif

enum DM_MAP_FLAG
{
    DM_MATCH_RESULT_NONE                = 0x00,        // 匹配结果：不匹配
    DM_MATCH_RESULT_SEMI                = 0x01 << 0,   // 匹配结果：部分匹配
    DM_MATCH_RESULT_FULL                = 0x01 << 1,   // 匹配结果：完全匹配
    DM_MATCH_ACTION_BLOCK               = 0x01 << 3,   // 匹配动作：拒绝呼出
    DM_MATCH_ACTION_CALLOUT             = 0x01 << 4,   // 匹配动作：呼出操作
    DM_MATCH_ACTION_NOACTION_CALLOUT    = 0x01 << 5,   // 匹配动作：默认没有操作, 其实也是呼出，只不过是延时呼出
    DM_MATCH_ACTION_DIAL_TONE           = 0x01 << 6,   // 匹配动作：播放拨号音
    DM_MATCH_ACTION_SELECT_ACCOUNT      = 0x01 << 7,   // 匹配动作：匹配账号
    DM_MATCH_ATTR_REPLACE               = 0x01 << 10,  // 匹配属性：是否发生替换
    DM_MATCH_ATTR_TIMER                 = 0x01 << 11,  // 匹配属性：是否自带时间
    DM_MATCH_PARSE_ERROR                = 0x01 << 15,  // 匹配失败：解析失败
};

// 依模式读取配置
bool GetDMSwitchByAccount(LPCSTR lpCfg, int iAccount);

// 依模式读取配置
yl::string GetDMCfgByAccount(LPCSTR lpCfg, int iAccount);

struct DigitMapItem;
// 单个账号的DigitMap信息
struct DigitMapInfo
{
    // 数据结构
    struct DMConfigCache
    {
        enum NoMatchAction
        {
            NMA_UNKOWM      = -1,   // 非法值
            NMA_CALLOUT     = 0,    // 不匹配，则立即呼出
            NMA_BLOCK       = 1,    // 不匹配，则拒绝呼出
            NMA_NOACTION    = 2,    // 不匹配，不执行任何操作
        };

        NoMatchAction   eNoMatchAction;

        DMConfigCache() : eNoMatchAction(NMA_UNKOWM) {}

        void clear()
        {
            eNoMatchAction = NMA_UNKOWM;
        }
    };

    // DigitMap操作结果缓存
    struct DMAction
    {
        UINT        iResult;
        UINT        iTimer;
        INT         iLineID;
        yl::string  strFmt;

        DMAction() : iTimer(invalid_timer), iResult(DM_MATCH_RESULT_NONE), iLineID(AUTO_SELECT_LINE_ID) {}

        bool isFlag(DM_MAP_FLAG eResult)  const
        {
            return iResult & eResult;
        }
        void addFlag(UINT iFlag)
        {
            iResult |= iFlag;
        }
        bool isValid() const
        {
            return DigitMapInfo::IsAttrValid(iResult);
        }
        void clearFlag(DM_MAP_FLAG eResult)
        {
            iResult &= ~eResult;
        }

        bool isFullMatched() const
        {
            return isFlag(DM_MATCH_RESULT_FULL);
        }
        bool isSemiMatched() const
        {
            return isFlag(DM_MATCH_RESULT_SEMI);
        }

        void clear()
        {
            iTimer = invalid_timer;
            iResult = DM_MATCH_RESULT_NONE;
            iLineID = AUTO_SELECT_LINE_ID;
            strFmt.clear();
        }

        bool operator > (const DMAction & other);

        // 创建呼出定时器，规则略复杂
        int CreateTimer(const DigitMapInfo & dmInfo, int iIndex);
    };

    // 没有匹配上的处理
    bool CreateDefaultAction(const DigitMapInfo & refInfoData);

    bool IsValid()
    {
        return vecDigitMap.size() > 0;
    }

    static bool IsAttrValid(UINT uAttr)
    {
        return !((uAttr == DM_MATCH_RESULT_NONE) || (uAttr & DM_MATCH_PARSE_ERROR));
    }

    static bool isFullMatched(UINT uAttr)
    {
        return IsAttrValid(uAttr) && (uAttr & DM_MATCH_RESULT_FULL);
    }
    static bool isSemiMatched(UINT uAttr)
    {
        return IsAttrValid(uAttr) && (uAttr & DM_MATCH_RESULT_SEMI);
    }

    typedef YLVector<DigitMapItem> VecDigitMap;
    typedef YLVector<int>          VecDigitMapTimer;
    typedef DMConfigCache::NoMatchAction  NoMatchAction;

    VecDigitMap      vecDigitMap;     // 解析后的单条DigitMap规则
    yl::string       strLastMatch;    // 上一次匹配的号码
    DMAction         dmActionResult;  // 缓存的结果
    VecDigitMapTimer vecLongTimer;    // 长超时时间
    VecDigitMapTimer vecShortTimer;   // 短超时时间
    DMConfigCache    dmConfig;        // 配置缓存

    void clear()
    {
        vecDigitMap.clear();
        strLastMatch.clear();
        dmActionResult.clear();
        vecLongTimer.clear();
        vecShortTimer.clear();
        dmConfig.clear();
    }
};

#endif

#endif // __DIAL_DIGITMAP_DATA_H__

