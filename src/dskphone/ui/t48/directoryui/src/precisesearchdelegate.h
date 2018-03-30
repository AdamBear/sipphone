#ifndef __PRECISE_SEARCH_DELEGATE_H__
#define __PRECISE_SEARCH_DELEGATE_H__

#include "directorylistdelegate.h"


class CPreciseSearchDelegate : public CDirectoryListDelegate
{
public:
    CPreciseSearchDelegate();
    virtual ~CPreciseSearchDelegate();

    static CPreciseSearchDelegate * GetDirDelegate(CDirectoryListDelegatePtr & pDelegate);

public:
    // 初始化
    virtual bool Init(int nDirType, int nGroupId = knInvalidRId, int nPageDataIndex = 0);

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
};

#endif
