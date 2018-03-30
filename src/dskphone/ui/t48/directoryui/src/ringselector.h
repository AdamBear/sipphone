
#ifndef __RING_SELECTOR_H__
#define __RING_SELECTOR_H__

#include <ylstring.h>
#include <ylvector.h>
#include "baseui/framelistdelegate.h"
#include "messagebox/imsgboxeventfilter.h"

class qRingPlayer;
class CRingSelector : public CFrameListDelegate, public IMsgBoxEventFilter
{
public:
    CRingSelector(void);
    virtual ~CRingSelector(void);

    void Init(int nId, const yl::string & strRingPath);
    int GetId() const;
    bool GetRingPath(yl::string & strRingPath);

    void StopRingPlay();

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

    // 数据代理绑定
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

// IMsgBoxEventFilter
public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress, bool & bFilter);

private:
    qRingPlayer * m_pRing;
    int m_nId;
    int m_nSelIndex;
    typedef YLVector<yl::string> VEC_RING_STRING;
    VEC_RING_STRING m_vecRingName;
    VEC_RING_STRING m_vecRingPath;
};

#endif
