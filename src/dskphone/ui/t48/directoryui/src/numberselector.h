
#ifndef __NUMBER_SELECTOR_H__
#define __NUMBER_SELECTOR_H__

#include "baseui/framelistdelegate.h"
#include "directorycommon.h"

class CNumberSelector : public CFrameListDelegate
{
public:
    CNumberSelector();
    virtual ~CNumberSelector(void);

    bool GetNumber(yl::string & strNumber) const;
    const yl::string & GetName() const;

    void SetName(const yl::string & strName);

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

private:
    int m_nSelIndex;
    yl::string m_strName;
};

#endif
