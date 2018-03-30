#ifndef __EMERGENCY_DIALPLAN_H__
#define __EMERGENCY_DIALPLAN_H__

#include "emergencybase.h"
#include "ylvector.h"

#ifdef IF_FEATURE_EMERGENCY

class CEmergencyMode
    : public CBaseEmergency
{
private:
    struct EmergencyItem
    {
        int         iIndex;  // 序号
        yl::string  strNum;  // 格式化过后的号码

        bool operator == (const yl::string & strUri) const
        {
            return strNum == strUri;
        }

        bool operator < (const EmergencyItem & item) const
        {
            return strNum < item.strNum;
        }

        bool operator < (const yl::string & strUri) const
        {
            return strNum < strUri;
        }

        bool operator > (const yl::string & strUri) const
        {
            return strNum > strUri;
        }

        static bool EmergencyCmp(const EmergencyItem & lItem, const EmergencyItem & rItem)
        {
            return lItem < rItem;
        }

        EmergencyItem() : iIndex(-1) {}
        EmergencyItem(int nIndex, const yl::string strValue) : iIndex(nIndex), strNum(strValue) {}
    };

    typedef bool (*EmergencyCmpFun)(const EmergencyItem & lItem, const EmergencyItem & rItem);

    typedef YLVector<EmergencyItem> VecEmergency;

public:
    CEmergencyMode();

    virtual ~CEmergencyMode() {}

    const EmergencyItem * Find(const yl::string & strNum);

    const EmergencyItem * SemiFind(const yl::string & strNum);

    virtual bool IsMatched(const char * cszTarget, int iType, CMatchedInfo * pInfo = NULL,
                           bool IsCharCmp = false);

    LPCSTR GetAssertedId();

    virtual bool Reload(int iType, int lParam);

    virtual void Init();

    virtual bool IsEmpty()
    {
        return m_vecEmergency.size() == 0;
    }

private:

    yl::string          m_strAssertId;  // 已经计算好的assert_id
    VecEmergency        m_vecEmergency; // 存储紧急呼叫号码表
    EM_ASSERTED_ID_SOURCE   m_eAssertedIdSrc;
};

#endif

#endif // __EMERGENCY_DIALPLAN_H__
