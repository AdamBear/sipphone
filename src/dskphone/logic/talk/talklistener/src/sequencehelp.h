#pragma once

#include "listencenter.h"

/* 辅助排序实现 */
struct CSequenceHelp
{
protected:

    typedef CListenCenter::VEC_HANDLE                       VEC_HANDLE;
    typedef CListenCenter::VEC_LISTENERS                    VEC_LISTENERS;
    typedef CListenCenter::HandlerInfo                      HandlerInfo;
    typedef YLVector<ListenerName>                          VEC_COLLECT;

    /* 辅助排序：搜集排序信息 */
    struct CCollect
    {
        CCollect& operator % (ListenerName name)
        {
            if (name)
            {
                m_vec.push_back(name);
            }

            return *this;
        }

        operator const VEC_COLLECT& () const
        {
            return m_vec;
        }

        VEC_COLLECT          m_vec;
    };

    /* 辅助排序：提供比较函数 */
    struct CSortHelp
    {
        CSortHelp(const VEC_LISTENERS& v)
            : m_vec(v)
        {}

        bool operator()(const HandlerInfo& l, const HandlerInfo& r);

        const VEC_LISTENERS& m_vec;
    };

    typedef YLHashMap<TALK_SIGNAL_TYPE, CCollect>   MAP_SEQUENCE;

public:

    CSequenceHelp(const VEC_LISTENERS& v);

    bool Sort(TALK_SIGNAL_TYPE sig, VEC_HANDLE& vec, bool bsync);

private:

    MAP_SEQUENCE             m_mapSequence;
    const VEC_LISTENERS&     m_vecListeners;
};

