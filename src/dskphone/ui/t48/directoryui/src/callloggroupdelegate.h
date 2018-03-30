
#ifndef __CALL_LOG_GROUP_DELEGATE_H__
#define __CALL_LOG_GROUP_DELEGATE_H__

#include <ETLLib.hpp>
#include "uikernel/ilistitemaction.h"
#include "groupdelegate.h"
#include "directorycommon.h"

class CCallLogGroupDelegate : public CGroupDelegate
{
public:
    CCallLogGroupDelegate();
    virtual ~CCallLogGroupDelegate();

    static CCallLogGroupDelegate * GetGroupDelegate(CGroupDelegatePtr & pDelegate);

public:
    // 处理逻辑层消息
    virtual LRESULT OnMessage(msgObject & refMessage);

    virtual void SetReadOnly(bool bReadOnly);

    // Id容错
    virtual int AdjustGroupId(int nDirType, int nGroupId);

    virtual void AdjustDirType(int & nDirType, int & nGroupId);

    // 获取不显示群组对应的显示群组
    virtual int GetDefaultGroupId(int nDirType, int nGroupId);

    enum
    {
        CALL_LOG_GROUP_ID_LOCAL_BEGIN                   = 100000,
        CALL_LOG_GROUP_ID_ALL_LOCAL_CALL                = 100001,
        CALL_LOG_GROUP_ID_LOCAL_END                     = 199999,
        CALL_LOG_GROUP_ID_BROADSOFT_BEGIN               = 0,
        CALL_LOG_GROUP_ID_ALL_NETWORK_CALL              = 1,
        CALL_LOG_GROUP_ID_BROADSOFT_END                 = 99999,
        CALL_LOG_GROUP_ID_METASWITCH_BEGIN              = 200000,
        CALL_LOG_GROUP_ID_ALL_METASWITCH_CALL           = 200001,
        CALL_LOG_GROUP_ID_METASWITCH_END                = 299999,
    };

#if IF_FEATURE_MUTI_CALLLOG
    // 刷新BroadSoft联系人群组
    void RefreshBroadSoftGroup(int nAccGroupID);

    virtual bool IsIdenticalTypeNote(int nParentID, int nTreeID);
#endif

// CTreeListDelegate
public:
    virtual void Init();

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

// IFrameTitleAction
public:
    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction);

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    virtual void OnChangeGroup(int nDirType);
#endif

protected:
    // 获取默认的联系人类型(默认为本地)
    virtual int GetDefaultDirType() const;
    // 初始化显示列表
    virtual void InitGroupList();

    // 初始化本地联系人
    void InitLocalGroup();
    // 初始化远程联系人
    void InitBroadSoftGroup(int nAccGroupID = -1);


    // 刷新本地联系人群组
    void RefreshLocalGroup(int nSelGroupId);

    // 刷新BroadSoft联系人群组
    void RefreshBroadSoftGroup();
#if IF_FEATURE_METASWITCH_DIRECTORY
    void InitMetaSwitchGroup();

// 刷新metaswitch联系人群组
    void RefreshMetaSwitchGroup();
#endif

    virtual void RefreshTreeListItem(int nCount, int nIndex);

    bool RefreshItem(int nItemIndex, CGroupTreeNodePtr pGroupNode);
};

#endif
