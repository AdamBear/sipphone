
#ifndef __DIRECTORY_LIST_DELEGATE_H__
#define __DIRECTORY_LIST_DELEGATE_H__

#include <ETLLib.hpp>
#include <ylsmartptr.h>
#include "directorycommon.h"
#include "baseui/framelistdelegate.h"
#ifdef FL_SUPPORT_TITLE
#include "baseui/iframetitleaction.h"
#endif
#include "directorycontroler.h"
#include "diruihelper.h"


class CDirectoryListDelegate : public CFrameListDelegate
#ifdef FL_SUPPORT_TITLE
    , public IFrameTitleAction
#endif
{
public:
    explicit CDirectoryListDelegate(int nState);
    virtual ~CDirectoryListDelegate(void);

public:
    // 初始化
    virtual bool Init(int nDirType, int nGroupId = knInvalidRId, int nPageDataIndex = 0);
    // 恢复初始化
    void UnInit();

    CDirectoryControlerPtr GetControler();
    CDirUIHelperPtr GetUIHelper();

    // 获取代理状态
    int GetState() const;
    // 获取联系人类型
    int GetDirType();
    // 获取联系人群组Id
    int GetGroupId();

// IFrameListDelegate
public:
    // 重新加载数据
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

#ifdef FL_SUPPORT_TITLE
    // IFrameTitleAction
public:
    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction);
#endif

protected:
    virtual void InitFrameList();
    virtual void UnInitFrameList();

protected:
    // 联系人业务逻辑控制器
    CDirectoryControlerPtr m_pControler;
    // 联系人UI表现管理器
    CDirUIHelperPtr m_pUIHelper;

    // 当前状态
    int m_nState;
};

#ifndef CDirectoryListDelegatePtr
typedef YLSmartPtr<CDirectoryListDelegate> CDirectoryListDelegatePtr;
#endif

#endif
