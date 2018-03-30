#ifndef __ADDCONTACT_DELEGATE_H__
#define __ADDCONTACT_DELEGATE_H__

#include "directorycommon.h"
#include "directorylistdelegate.h"
#include "baseui/framelistdelegate.h"


class CAdditionDelegate : public CDirectoryListDelegate
{
public:
    explicit CAdditionDelegate();
    virtual ~CAdditionDelegate(void);

    static CAdditionDelegate * GetDirDelegate(CDirectoryListDelegatePtr & pDelegate);

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
};

#endif
