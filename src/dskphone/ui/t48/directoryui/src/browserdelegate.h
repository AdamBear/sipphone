#ifndef __BROWSER_DELEGATE_H__
#define __BROWSER_DELEGATE_H__

#include "directorycommon.h"
#include "directorylistdelegate.h"

class CBrowserDelegate : public CDirectoryListDelegate
{
public:
    explicit CBrowserDelegate();
    virtual ~CBrowserDelegate();

    static CBrowserDelegate * GetDirDelegate(CDirectoryListDelegatePtr & pDelegate);

public:
    // 进入群组
    virtual bool OnChangeGroup(int nGroupId, bool bRefresh = true);

    // 回退处理（如果可以回退界面返回true，否则返回false）
    bool OnBack();
};

#endif
