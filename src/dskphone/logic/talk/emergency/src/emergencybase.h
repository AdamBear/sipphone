#ifndef __EMERGENCY_BASE_H__
#define __EMERGENCY_BASE_H__

enum EM_TYPE
{
    EM_NONE        = 0,            //
    EM_WL          = 0x01 << 0,    // 白名单模式（最早的紧急呼叫）
    EM_WL_DND      = 0x01 << 1,    // 白名单模式（最早的紧急呼叫）
    EM_WL_FWD      = 0x01 << 2,    // 白名单模式（最早的紧急呼叫）
    EM_ADV         = 0x01 << 10,   // 高级模式的紧急呼叫
    EM_WL_ALL      = EM_WL + EM_WL_DND + EM_WL_FWD,   // 所有白名单模式
    EM_ALL_MOD     = EM_WL_ALL + EM_ADV,              // 所有模式
};

enum EM_ASSERTED_ID_SOURCE
{
    EM_SRC_NONE = 0,
    EM_SRC_ELIN = 1,
    EM_SRC_CUSTOM = 2,
    EM_SRC_HELD = 3
};

// 结果缓存
struct CMatchedInfo
{
    yl::string          m_strLastMatched;
    int                 m_iLastMatchedMode;
    int                 m_iLastMatchedIndex;
    bool                m_bSemiMatch;
    int                 m_iLastMode;

    CMatchedInfo()
    {
        clear();
    }

    void clear()
    {
        m_strLastMatched.clear();
        m_iLastMatchedIndex = -1;
        m_iLastMatchedMode = 0;
        m_bSemiMatch = false;
        m_iLastMode = 0;
    }
};

class CBaseEmergency
{
public:
    virtual void Init() {};

    virtual void UnInit() {};

    virtual bool Reload(int iType, int lParam)
    {
        return false;
    }

    virtual bool IsMatched(const char * cszTarget, int iType, CMatchedInfo * pInfo = NULL,
                           bool IsCharCmp = false)
    {
        return false;
    }

    EM_TYPE GetType() const
    {
        return m_eType;
    }

    CBaseEmergency(EM_TYPE eType) : m_eType(eType) {}

    virtual ~CBaseEmergency() {}

    virtual bool IsEmpty()
    {
        return false;
    }

private:
    EM_TYPE        m_eType;
};

#endif
