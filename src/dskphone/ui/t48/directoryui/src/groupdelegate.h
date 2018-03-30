
#ifndef __GROUP_DELEGATE_H__
#define __GROUP_DELEGATE_H__

#include <ETLLib.hpp>
#include <ylsmartptr.h>
#include "baseui/iframetitleaction.h"
#include "baseui/treelistdelegate.h"
#include "grouptreenode.h"


class CGroupDelegate : public CTreeListDelegate, public IFrameTitleAction
{
public:
    explicit CGroupDelegate(int nType);
    virtual ~CGroupDelegate();

public:
    // 处理逻辑层消息
    virtual LRESULT OnMessage(msgObject & refMessage)                    = 0;

    // 设置是否可以编辑(通话状态下不能编辑)
    virtual void SetReadOnly(bool bReadOnly)                            = 0;

    // Id容错
    virtual int AdjustGroupId(int nDirType, int nGroupId)               = 0;

    virtual void AdjustDirType(int & nDirType, int & nGroupId)            = 0;

    // 获取不显示群组对应的显示群组
    virtual int GetDefaultGroupId(int nDirType, int nGroupId)           = 0;

    virtual void OnChangeGroup(int nDirType) {}

protected:
    // 获取默认的联系人类型(默认为本地)
    virtual int GetDefaultDirType() const                               = 0;

    // 初始化显示列表
    virtual void InitGroupList()                                        = 0;

public:
    // 设置默认群组
    void SetDefaultGroup(int nGroupType, int nGroupId);

    // 根据联系人类型及群组ID获取群组节点
    CGroupTreeNodePtr GetGroupNode(int nGroupType, int nGroupId);

    int GetSelectGroupType();
    int GetSelectGroupId();

    // 获取类型
    int GetType() const;
    // 设置Setting模式
    void SetSetting(bool bSetting);

    // 是否处于Setting模式
    bool IsSetting() const;

    // 展开到选定的组
    void ExpandToSelectGroup();

    bool IsDirTypeValid(int nDirType);

protected:
    // 初始化ID范围
    void InitGroupId(int nGroupType, int nMinId, int nMaxId);

    // 申请一个类型的群组树ID
    int ApplyGroupTreeId(int nGroupType);
    // 重置一个类型的群组树ID
    void ResetGroupTreeId(int nGroupType);

    // 获取默认的节点
    CGroupTreeNodePtr GetDefaultGroupNode(int nGroupType);

    bool IsGroupCanShow(int nGroupType);

    // 判断节点是否展开
    bool IsExpand(int nGroupTreeId);
    // 获取同类群组中选择的群组
    int GetGroupSelectId(int nGroupType);
    // 获取节点附近的默认节点
    int GetDefaultRecentGroupId(int nGroupType, int nGroupTreeId);

    // 重新初始化界面的群组列表
    virtual bool ReInitGroupList(int nGroupType, int nRootTreeId, bool bExpand, int nGroupSelectId);

    // 重定位当前页
    void ReLocateCurrentPage();

    // 清除群组
    void ClearGroup(int nBeginTreeId, int nEndTreeId);

    // 重新刷新选中群组的联系人列表
    void ReSelectGroup();

protected:
    int m_nType;
    bool m_bSetting;

private:
    typedef std::map<int, int> MAP_GROUP_ID;
    MAP_GROUP_ID m_mapCurMaxId;
    MAP_GROUP_ID m_mapMinId;
    MAP_GROUP_ID m_mapMaxId;
};
typedef YLSmartPtr<CGroupDelegate> CGroupDelegatePtr;

#endif
