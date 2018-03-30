#ifndef __DETAIL_DELEGATE_H__
#define __DETAIL_DELEGATE_H__

#include "directorycommon.h"
#include "directorylistdelegate.h"
#include "baseui/framelistdelegate.h"


class CDetailDelegate : public CDirectoryListDelegate
{
public:
    explicit CDetailDelegate();
    virtual ~CDetailDelegate(void);

    static CDetailDelegate * GetDirDelegate(CDirectoryListDelegatePtr & pDelegate);

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

#ifdef FL_SUPPORT_TITLE
    // IFrameTitleAction
public:
    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction);
#endif
};

#endif
