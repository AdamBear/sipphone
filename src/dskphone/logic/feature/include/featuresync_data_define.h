#ifndef __FEATURE_SYNC_DATA_DEFINE_H__
#define __FEATURE_SYNC_DATA_DEFINE_H__

// 发送特征码开启的通话信息.
struct CodeCallInfo
{
    int m_nLineId;  //
    unsigned int m_uCallId;

    CodeCallInfo()
    {
        Reset();
    }

    void Reset()
    {
        m_nLineId = -1;
        m_uCallId = (unsigned int) - 1;
    }

    // Overwrite operator=.
    CodeCallInfo & operator= (const CodeCallInfo & rhs)
    {
        if (this != &rhs)
        {
            m_nLineId = rhs.m_nLineId;
            m_uCallId = rhs.m_uCallId;
        }
        return *this;
    }

    // Overwrite the operator ==.
    bool operator== (const CodeCallInfo & rhs)
    {
        return m_nLineId == rhs.m_nLineId && m_uCallId == (unsigned int)rhs.m_nLineId;
    }
};

// Feature type to sync.
enum SyncFeatureType
{
    SYNC_FEATURE_TYPE_NONE = 0,  // no sync type.
    SYNC_FEATURE_DND = 1,  // sync DND.
    SYNC_FEATURE_FORWARD_ALWAYS = 2,  // sync Forward always..
    SYNC_FEATURE_FORWARD_BUSY = 3,  // Sync forward busy.
    SYNC_FEATURE_FORWARD_NOANSWER = 4,  // Sync forward no answer.
    SYNC_FEATURE_FWD = 5,        // compatible with t2x
    SYNC_FEATURE_TYPE_MAX
};

#endif //__FEATURE_SYNC_DATA_DEFINE_H__
