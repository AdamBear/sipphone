
#ifndef __CONTACT_GROUP_DELEGATE_H__
#define __CONTACT_GROUP_DELEGATE_H__

#include <map>
#include <ylstring.h>
#include <ETLLib.hpp>
#include <ylsmartptr.h>
#include "directorycommon.h"
#include "groupdelegate.h"


class CContactGroupNode : public CGroupTreeNode
{
public:
    explicit CContactGroupNode(int nId, int nParentId, bool bHadChildren, int nType, int nGroupId);
    ~CContactGroupNode(void);

public:
    // 设置是否可以添加新联系人
    void SetAddContact(bool bAddContact);

    // 设置是否可以编辑
    void SetEditable(bool bEditable);

#if IF_FEATURE_GBBOOK
    void SetShowBtnRing(bool bShow);
#endif

    // 是否可以添加新联系人
    bool IsAddContact() const;
    // 是否可编辑
    bool IsEditable() const;

#if IF_FEATURE_GBBOOK
    bool IsShowBtnRing() const;
#endif

private:
    // 是否可以增加联系人
    bool m_bAddContact;
    // 是否可编辑
    bool m_bEditable;

#if IF_FEATURE_GBBOOK
    bool m_bShowBtnRing;
#endif
};
typedef CContactGroupNode * CContactGroupNodePtr;
typedef std::vector<CContactGroupNodePtr> VEC_CONTACT_GROUP_NODE;

class QString;
class CGroupTreeItem;
class CGroupEditTreeItem;

class CContactGroupDelegate : public CGroupDelegate
{
public:
    CContactGroupDelegate();
    virtual ~CContactGroupDelegate();

    static CContactGroupDelegate * GetGroupDelegate(CGroupDelegatePtr & pDelegate);

public:
    enum
    {
        CONTACT_GROUP_ID_PLCM_BEGIN             = 0,
        CONTACT_GROUP_ID_PLCM                       = 1,
        CONTACT_GROUP_ID_PLCM_END                   = 99999,
        CONTACT_GROUP_ID_LOCAL_BEGIN                    = 100000,
        CONTACT_GROUP_ID_ALL_LOCAL_CONTACT              = 100001,
        CONTACT_GROUP_ID_LOCAL_END                      = 199999,
        CONTACT_GROUP_ID_BLUETOOTH_BEGIN                = 200000,
        CONTACT_GROUP_ID_BLUETOOTH_CONTACT              = 200001,
        CONTACT_GROUP_ID_BLUETOOTH_END                  = 299999,
        CONTACT_GROUP_ID_REMOTE_BEGIN                   = 300000,
        CONTACT_GROUP_ID_NETWORK_PHONE_BOOK             = 300001,
        CONTACT_GROUP_ID_REMOTE_END                     = 399999,
        CONTACT_GROUP_ID_LDAP_BEGIN                     = 400000,
        CONTACT_GROUP_ID_LDAP_ROOT                      = 400001,
        CONTACT_GROUP_ID_LDAP_END                       = 499999,
        CONTACT_GROUP_ID_BROADSOFT_BEGIN                = 500000,
        CONTACT_GROUP_ID_NETWORK_DIRECTORY              = 500001,
        CONTACT_GROUP_ID_BROADSOFT_END                  = 599999,
        CONTACT_GROUP_ID_BTUC_BEGIN                     = 600000,
        CONTACT_GROUP_ID_BUDDIES                        = 600001,
        CONTACT_GROUP_ID_BTUC_END                       = 699999,
        CONTACT_GROUP_ID_BLACKLIST_BEGIN                = 700000,
        CONTACT_GROUP_ID_BLACKLIST                      = 700001,
        CONTACT_GROUP_ID_BLACKLIST_END                  = 799999,
        CONTACT_GROUP_ID_GAB_BEGIN                  = 800000,
        CONTACT_GROUP_ID_GAB                        = 800001,
        CONTACT_GROUP_ID_GAB_END                    = 899999,
        CONTACT_GROUP_ID_PAB_BEGIN                  = 900000,
        CONTACT_GROUP_ID_PAB                        = 900001,
        CONTACT_GROUP_ID_PAB_END                    = 999999,
        CONTACT_GROUP_ID_METASWITCH_BEGIN               = 1000000,
        CONTACT_GROUP_ID_METASWITCH_DIRECTORY           = 1000001,
        CONTACT_GROUP_ID_METASWITCH_END                 = 1099999,
        CONTACT_GROUP_ID_FAVORITE_BEGIN = 1100000,
        CONTACT_GROUP_ID_FAVORITE = 1100001,
        CONTACT_GROUP_ID_FAVORITE_END               = 1199999,
    };

    enum
    {
        CONTACT_GROUP_TYPE_NONE                         = 0,
        CONTACT_GROUP_TYPE_LOCAL                        = 1,
        CONTACT_GROUP_TYPE_REMOTE                       = 2,
        CONTACT_GROUP_TYPE_LDAP                         = 3,
        CONTACT_GROUP_TYPE_BROADSOFT                    = 4,
        CONTACT_GROUP_TYPE_BUDDIES                      = 5,
        CONTACT_GROUP_TYPE_BLACKLIST                    = 6,
        CONTACT_GROUP_TYPE_PLCM                         = 7,
        CONTACT_GROUP_TYPE_BLUETOOTH                    = 8,
        CONTACT_GROUP_TYPE_GAB                          = 9,
        CONTACT_GROUP_TYPE_PAB                          = 10,
        CONTACT_GROUP_TYPE_METASWITCH                   = 11,
        CONTACT_GROUP_TYPE_FAVORITE                 = 12,
    };

    // 初始化
    virtual void Init();

    virtual void SetReadOnly(bool bReadOnly);

    // 处理逻辑层消息
    virtual LRESULT OnMessage(msgObject & refMessage);

    // Id容错
    virtual int AdjustGroupId(int nDirType, int nGroupId);

    virtual void AdjustDirType(int & nDirType, int & nGroupId);
    // 重新初始化界面的群组列表
    virtual bool ReInitGroupList(int nGroupType, int nRootTreeId, bool bExpand, int nGroupSelectId);

    // 获取不显示群组对应的显示群组
    virtual int GetDefaultGroupId(int nDirType, int nGroupId);

    // 设置
    int UpdateLocalGroupRing(int nGroupId, const yl::string & strRing);
    bool GetLocalGroupRing(int nGroupId, yl::string & strRing);

    void GetAdditiveGroup(VEC_CONTACT_GROUP_NODE & veCContactGroupNode, bool bSameType,
                          int nContactType);

    static bool CompareGroupNode(CContactGroupNodePtr pGroupNode1, CContactGroupNodePtr pGroupNode2);

    int GetCurrentGroupId(int nContactType, int nOriGroupId);

    void DelGroup(int nGroupTreeId);

    bool IsCanAddGroup();

#if IF_FEATURE_GBBOOK
    void SetGABGroupIsSelect();
#endif

// IListItemAction
public:
    // 列表项Action处理
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

// IFrameTitleAction
public:
    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction);

protected:
    // 获取默认的联系人类型(默认为本地)
    virtual int GetDefaultDirType() const;
    // 初始化本地联系人
    void InitLocalGroup();
    // 初始化蓝牙联系人
    void InitBluetoothGroup();
    // 初始化远程联系人
    void InitRemoteGroup();
    // 初始化LDAP
    void InitLDAPGroup();
    // 初始化BroadSoft
    void InitBroadSoftGroup();

    // 初始化BroadTouchUC群组
    void InitBTUCGroup();

    // 初始化黑名单
    void InitBlackListGroup();

#if IF_BUG_PLCMDIR
    void InitPlcmGroup();
#endif

#if IF_FEATURE_GBBOOK
    void InitGabGroup();

    void InitPabGroup();
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
// 初始化mtsw
    void InitMetaSwitchGroup();
#endif

    // 初始化显示列表
#if IF_FEATURE_FAVORITE_DIR
    void InitFavoriteGroup();
#endif

    void InitGroupList();

    // 刷新本地联系人群组
    void RefreshLocalGroup(int nSelGroupId);
    // 刷新蓝牙联系人群组(开启或关闭)
    void RefreshBluetoothGroup();
    // 刷新远程联系人群组(开启或关闭)
    void RefreshRemoteGroup();
    // 刷新LDAP联系人群组(开启或关闭)
    void RefreshLDAPGroup();
    // 刷新BroadSoft联系人群组
    void RefreshBroadSoftGroup();
    // 刷新BroadTouchUC联系人群组
    void RefreshBTUCGroup();
#if IF_FEATURE_GBBOOK
    // 刷新genband联系人群组
    void RefreshPABGroup(int nSelGroupId);
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    // 刷新MetaSwitch联系人群组
    void RefreshMetaSwitchGroup();
#endif

    virtual void RefreshTreeListItem(int nCount, int nIndex);

protected:
    // 刷新添加群组节点
    void RefreshAddNewItem(int nItemIndex);
    // 刷新正常群组节点
    bool RefreshNormalItem(int nItemIndex, CContactGroupNodePtr pGroupNode);
    // 刷新编辑群组节点
    bool RefreshEditItem(int nItemIndex, CContactGroupNodePtr pGroupNode);
    // 更新通用信息到群组列表项中
    bool UpdateGroupItem(int nItemIndex, CContactGroupNodePtr pGroupNode,
                         CGroupTreeItem * pGroupTreeItem);

    // 响应删除群组
    void OnDelGroup(CContactGroupNodePtr pGroupNode, CGroupEditTreeItem * pTreeItem);
    // 响应更新群组
    void OnModifyGroup(CContactGroupNodePtr pGroupNode, CGroupEditTreeItem * pTreeItem);
    // 响应设置群组铃声
    void OnSetGroupRing(CContactGroupNodePtr pGroupNode, CGroupEditTreeItem * pTreeItem);

    // 响应添加本地群组
    void OnAddLocalGroup();
    // 响应完成添加本地群组
    void OnFinishAddLocalGroup(const QString & strGroupName);

#if IF_FEATURE_GBBOOK
    void OnAddGenbandGroup();
    void OnFinishAddGenbandGroup(const QString & strGroupName);
#endif

private:
    // 添加群组时记录选中的行
    int m_nLastSelId;

#if IF_FEATURE_GBBOOK
    int m_AddGroupDirType;
#endif

    typedef std::map<int, int> MAP_GROUP_MAX_ID;
    MAP_GROUP_MAX_ID m_mapMaxId;
};

#endif
