#ifndef DEFINE_DMCOMMON_H_
#define DEFINE_DMCOMMON_H_

#if 0
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace NS_TA
{
class CTaskActionCallbackBase;
}

namespace NS_DIRECTORY
{
struct ContactBroadSoftItemBase;
struct ContactBroadSoftPersonal;
}

// DirectoryManager所在的UI类型.
enum DirectoryManagerUIType
{
    DM_UI_TYPE_NONE = 0,  // 不正确的类型.
    DM_UI_LOCAL_DIRECTORY = 1,  // 本地联系人.
    DM_UI_REMOTE_DIRECTORY = 2,  // 远程联系人.
    DM_UI_LDAP_DIRECTORY = 3,  // LDAP联系人.
    DM_UI_BROADSOFT_DIRECTORY = 4,  // BroadSoft联系人
    DM_UI_BROADSOFT_UCDIR     = 5,  // UC联系人
};

#ifdef _T42
// Directory子页面的UI类型.
enum DIR_SUBPAGE_TYPE
{
    DIR_SUBPAGE_TYPE_NONE = 0,  // 不正确的子页面类型.
    DIR_SUBPAGE_CONTACT = 1,    // 联系人信息子页面.
    DIR_SUBPAGE_GROUP_MANAGER = 2,  // 群组管理子页面.
    DIR_SUBPAGE_NEW_GROUP = 3,  // 新建群组子页面.
    DIR_SUBPAGE_SEARCH_CONTACT = 4, //搜索联系人子页面.
    DIR_SUBPAGE_GROUP_LIST = 5, //群组列表子页面.
    DIR_SUBPAGE_NEW_CONTACT = 6, //新建联系人子页面.
    DIR_SUBPAGE_EDIT_CONTACT = 7, //编辑联系人子页面.
    DIR_SUBPAGE_SERACH_RESULT = 8 //联系人搜索结果子页面
};
#else
//  Directory界面的状态
enum DM_UI_STATUS_TYPE
{
    DM_STATUS_TYPE_NONE = 0,  // 无状态.

    //Group
    //-- 1
    DM_UI_GROUP_LIST_STATUS,        //群组列表状态
    DM_UI_GROUP_NEW_STATUS,     //新建组状态
    DM_UI_GROUP_EDIT_STATUS,        //群组编辑编辑状态
    DM_UI_GROUP_DEL_STATUS,     //删除群组

    //Contact
    //-- 5
    DM_UI_CONTACT_LIST_STATUS,      //联系人列表状态
    DM_UI_CONTACT_NEW_STATUS,       //新建联系个状态
    DM_UI_CONTACT_EDIT_STATUS,      //联系人编辑状态
    DM_UI_CONTACT_DEL_STATUS,       //删除联系人

    //Search
    //-- 9
    DM_UI_SEARCH_PRELIM_STATUS, //预搜索状态(普通搜索状态)
    DM_UI_SEARCH_ADVANCE_STATUS,    //高级搜索状态
    DM_UI_SEARCH_RESULT_STATUS,     //搜索结果状态(预搜索|普通搜索状态)
    DM_UI_SEARCH_ADV_RESULT_STATUS,//高级搜索结果状态

    //Favorite
    // -- 13
    DM_UI_FAVORITE_LOOK_STATUS,     //搜藏夹浏览状态
    DM_UI_FAVORITE_ADD_STATUS,      //搜藏夹添加状态
    DM_UI_FAVORITE_EDIT_STATUS,     //搜藏夹编辑

    DM_STATUS_TYPE_MAX,
};
#endif

// DirectoryManager传给逻辑层的数据结构.
struct DMData
{
    // DirectoryManager的命令类型.
    enum DMCommandType
    {
        DM_COMMAND_TYPE_NONE = 0,  // 不正确的类型.

        // 本地联系人.
        DM_COMMAND_LOCAL_COMMAND_BEGIN = 0,
        // 获取本地联系人列表.
        // m_pData: DMLocalDirGetContactListByClickActionData*.
        DM_COMMAND_LOCAL_DIR_GET_CONTACT_LIST = 1,
        // 同步搜索本地联系人, 不显示多个号码.
        // m_pData: DMLocalDirSearchData*.
        DM_COMMAND_LOCAL_DIR_SEARCH_SYNC_WITHOUT_MULT_NUMBER = 2,
        // 同步搜索本地联系人, 并显示多个号码.
        // m_pData: DMLocalDirSearchWithMutlNumberData*.
        DM_COMMAND_LOCAL_DIR_SEARCH_SYNC_WITH_MULT_NUMBER = 3,
        // 异步搜索本地联系人, 不显示多个号码.
        // m_pData: DMLocalDirSearchAsyncData*.
        DM_COMMAND_LOCAL_DIR_SEARCH_ASYNC_WITHOUT_MULT_NUMBER = 4,
        // 异步搜索本地联系人, 显示多个号码.
        // m_pData: DMLocalDirSearchWithMutlNumberAsyncData*.
        DM_COMMAND_LOCAL_DIR_SEARCH_ASYNC_WITH_MULT_NUMBER = 5,
        // 获取本地联系人最大数量.
        // m_pData: DMLocalDirGetMaxContactCountData*.
        DM_COMMAND_LOCAL_DIR_GET_MAX_CONTACT_COUNT = 6,
        // 设置本地联系人最大数量.
        // m_pData: DMLocalDirSetMaxContactCountData*.
        DM_COMMAND_LOCAL_DIR_SET_MAX_CONTACT_COUNT = 7,
        // 获取当前本地联系人的数量.
        // m_pData: DMLocalDirGetCurrentContactCountData*.
        DM_COMMAND_LOCAL_DIR_GET_CUR_CONTACT_COUNT = 8,
        // 获取黑名单联系人最大数量.
        // m_pData: DMLocalDirGetMaxBlackCountData*.
        DM_COMMAND_LOCAL_DIR_GET_MAX_BLACK_COUNT = 9,
        // 设置黑名单联系人最大数量.
        // m_pData: DMLocalDirSetMaxBlackCountData*.
        DM_COMMAND_LOCAL_DIR_SET_MAX_BLACK_COUNT = 10,
        // 获取当前黑名单联系人的数量.
        // m_pData: DMLocalDirGetCurrentBlackCountData*.
        DM_COMMAND_LOCAL_DIR_GET_CUR_BLACK_COUNT = 11,
        // 移动本地联系人到组.
        // m_pData: DMLocalDirMoveListToGroupData*.
        DM_COMMAND_LOCAL_DIR_MOVE_CONTACT = 12,
        // 检查联系人是否可以移动到组.
        // m_pData: DMLocalDirIsListCanMoveData*.
        DM_COMMAND_LOCAL_DIR_CHECK_MOVABLE = 13,
        // 获取收藏夹列表.
        // m_pData: DMLocalDirGetFavorateListData*.
        DM_COMMAND_LOCAL_DIR_GET_FAVOR_LIST = 14,
        // 移动联系人到收藏夹.
        // m_pData: DMLocalDirToFavorateListData*.
        DM_COMMAND_LOCAL_DIR_TO_FAVOR = 15,
        // 删除收藏夹中的某些联系人.
        // m_pData: DMLocalDirRemoveFavorateListData*.
        DM_COMMAND_LOCAL_DIR_REMOVE_FAVOR = 16,
        // 清空收藏夹.
        // m_pData: NULL.
        DM_COMMAND_LOCAL_DIR_REMOTE_ALL_FAVOR = 17,
        // 删除所有联系人.
        // m_pData: DMLocalDirRemoveAllContactsByClickActionData*.
        DM_COMMAND_LOCAL_DIR_REMOVE_ALL_CONTACT = 18,
        // 增加联系人.
        // m_pData: DMLocalDirAddContactData*.
        DM_COMMAND_LOCAL_DIR_ADD_CONTACT = 19,
        // 删除本地联系人.
        // m_pData: DMLocalDirRemoveContactListData*.
        DM_COMMAND_LOCAL_DIR_REMOVE_CONTACT = 20,
        // 根据号码获取联系人数据.
        // m_pData: DMLocalDirGetContactDataByNumberData*.
        DM_COMMAND_LOCAL_DIR_GET_CONTACT_BY_NUMBER = 21,
        // 获取名字获取联系人数据.
        // m_pData: DMLocalDirGetContactDataData*.
        DM_COMMAND_LOCAL_DIR_GET_CONTACT_BY_NAME = 23,
        // 更新联系人数据.
        // m_pData: DMLocalDirUpdateContactDataData*.
        DM_COMMAND_LOCAL_DIR_UPDATE_CONTACT = 24,
        // 拨出联系人.
        // m_pData: DMLocalDirCallOutData*.
        DM_COMMAND_LOCAL_DIR_CALLOUT_CONTACT = 25,
        // 获取当前favorate数量.
        // m_pData: DMLocalDirGetFavorCountData*.
        DM_COMMAND_LOCAL_DIR_GET_CUR_FAVOR_COUNT = 26,

        //删除所有联系人组
        DM_COMMAND_LOCAL_REMOVE_ALL_GROUP = 27,

        // 本地组.
        // 获取组详情列表.
        // m_pData: DMLocalDirGetGroupListData*.
        DM_COMMAND_LOCAL_GET_GROUP_DETAIL_LIST = 40,
        // 获取组简要信息列表.
        // m_pData: DMLocalDirGetGroupSimpleListData*.
        DM_COMMAND_LOCAL_GET_GROUP_BRIEF_LIST = 41,
        // 获取组数量上限.
        // m_pData: DMLocalDirGetMaxGroupCountData*
        DM_COMMAND_LOCAL_GET_MAX_GROUP_COUNT = 42,
        // 设置组数量上限.
        // m_pData: DMLocalDirSetMaxGroupCountData*.
        DM_COMMAND_LOCAL_SET_MAX_GROUP_COUNT = 43,
        // 获取当前组数量.
        // m_pData: DMLocalDirGetCurrentGroupCountData*.
        DM_COMMAND_LOCAL_GET_CUR_GROUP_COUNT = 44,
        // 根据Click Action获取组数据.
        // m_pData: DMLocalDirGetGroupDataByClickActionData*.
        DM_COMMAND_LOCAL_GET_GROUP_DATA = 45,
        // 增加一个组.
        // m_pData: DMLocalDirAddGroupData*.
        DM_COMMAND_LOCAL_ADD_GROUP = 46,
        // 删除一个组.
        // m_pData: DMLocalDirRemoveGroupData*.
        DM_COMMAND_LOCAL_REMOVE_GROUP = 47,
        // 更新组数据.
        // m_pData: DMLocalDirUpdateGroupDataData*.
        DM_COMMAND_LOCAL_UPDATE_GROUP = 48,
        // 根据组名获取组id.
        // m_pData: DMLocalDirGetGroupIdByGroupNameData*.
        DM_COMMAND_LOCAL_GET_GID_BY_GNAME = 49,
        // 联系人是否正在同步.
        // m_pData: DMLocalDirIsSyncData*.
        DM_COMMAND_LOCAL_IS_SYNCING = 50,

        DM_COMMAND_LOCAL_COMMAND_END = 58,

        // 远程联系人.
        DM_COMMAND_REMOTE_COMMAND_BEGIN = 59,
        // 根据Click Action获取联系人列表.
        // m_pData: DMRemoteDirGetContactListByClickActionData*.
        DM_COMMAND_REMOTE_GET_CONTACT_LIST_BY_CLICKACTION = 60,
        // 获取联系人列表.
        // m_pData: DMRemoteDirGetContactListData*.
        DM_COMMAND_REMOTE_GET_CONTACT_LIST = 61,
        // 取消下载.
        // m_pData: NULL.
        DM_COMMAND_REMOTE_CANCEL_DOWNLOAD = 62,
        // 获取标题.
        // m_pData: DMRemoteDirGetTitleData*.
        DM_COMMAND_REMOTE_GET_TITLE = 63,
        // 根据下标获取具体联系人数据.
        // m_pData: DMRemoteDirGetUnitContactByIndexData*.
        DM_COMMAND_REMOTE_GET_UNIT_CONTACT_BY_INDEX = 64,
        // 根据下标进入下一级.
        // m_pData: DMRemoteDirEnterContactByIndexData*.
        DM_COMMAND_REMOTE_ENTER_CONTACT_BY_INDEX = 65,
        // 通过快捷键获取对应URL下的联系人数据
        // m_pData: DMRemoteDirEnterContactByHotKeyData*.
        DM_COMMAND_REMOTE_ENTER_CONTACT_BY_HOTKEY = 66,
        // 回退到上一级.
        // m_pData: NULL.
        DM_COMMAND_REMOTE_LEAVE_BACK = 67,
        // 退出远程地址薄.
        // m_pData: NULL.
        DM_COMMAND_REMOTE_LEAVE_OUT = 68,
        // 选中某个远程联系人.
        // m_pData: DMRemoteDirSetSelectedIndexData*.
        DM_COMMAND_REMOTE_SELECT_INDEX = 69,
        // 将远程联系人加入到本地ALL组.
        // m_pData: DMRemoteDirRemote2LocalData*.
        DM_COMMAND_REMOTE_REMOTE_2_LOCAL = 70,
        // 将远程联系人加入到黑名单组.
        // m_pData: DMRemoteDirRemote2BlackData*.
        DM_COMMAND_REMOTE_REMOTE_2_BLACK = 71,
        // 远程地址薄是否有配置.
        // m_pData: NULL.
        DM_COMMAND_REMOTE_IS_CONFIGURED = 72,
        // 某个指定下标的远程地址薄是否有配置.
        // m_pData: DMRemoteDirIsItemConfiguredData*.
        DM_COMMAND_REMOTE_IS_ITEM_CONFIGURED = 73,
        // 获取可配置的总数.
        // m_pData: DMRemoteDirGetTotalConfigurableItemCountData*.
        DM_COMMAND_REMOTE_GET_CONFIGURABLE_ITEM_COUNT = 74,
        // 获取某个配置项的名称.
        // m_pData: DMRemoteDirGetConfiguredItemNameData*.
        DM_COMMAND_REMOTE_GET_CONFIGURED_ITEM_NAME = 75,
        // 获取某个配置项的URL.
        // m_pData: DMRemoteDirGetConfiguredItemUrlData*.
        DM_COMMAND_REMOTE_GET_CONFIGURED_ITEM_URL = 76,
        // 获取配置上的列表.
        // m_pData: DMRemoteDirGetConfiguredListData*.
        DM_COMMAND_REMOTE_GET_CONFIGURED_LIST = 77,
        // 拨出远程联系人.
        // m_pData: DMRemoteCallOutData*.
        DM_COMMAND_REMOTE_CALLOUT = 78,
        // 获取远程地址薄的显示名.
        // m_pData: DMRemoteGetDisplayNameData*.
        DM_COMMAND_REMOTE_GET_DISPLAYNAME = 79,
        // 更新远程地址薄.
        // m_pData: DMRemoteUpdateDirData*.
        DM_COMMAND_REMOTE_UDPATE = 80,
        // 搜索远程联系人.
        // m_pData: DMRemoteSearchDirData*.
        DM_COMMAND_REMTOE_SEARCH = 81,

        DM_COMMAND_REMOTE_COMMAND_END = 88,

        // BroadSoft联系人.
        DM_COMMAND_BD_COMMAND_BEGIN = 89,
        // 根据Click Action获取远程联系人列表.
        // m_pData: DMBDGetContactListByClickActionData*.
        DM_COMMAND_BD_GET_CONTACT_LIST_BY_CLICKACTION = 90,
        // 取消下载.
        // m_pData: NULL.
        DM_COMMAND_BD_CANCEL_DOWNLOAD = 91,
        // 获取指定index的联系人数据.
        // m_pData: DMBDGetContactByIndexData*.
#ifdef _T42
        DM_COMMAND_BD_GET_CONTACT_BY_INDEX = 92,
#else
        DM_COMMAND_BD_GET_CONTACT_BY_NAME = 92,
#endif
        // 添加BroadSoft到本地ALL组.
        // m_pData: DMBDBroadSoft2LocalData*.
        DM_COMMAND_BD_BROADSOFT_2_LOCAL = 93,
        // 添加BroadSoft到本地Black组.
        // m_pData: DMBDBroadSoft2BlackData*.
        DM_COMMAND_BD_BROADSOFT_2_BLACK = 94,
        // BroadSoft地址薄是否有配置.
        // m_pData: NULL.
        DM_COMMAND_BD_IS_CONFIGURED = 95,
        // 某个指定下标的BroadSoft地址薄是否有配置.
        // m_pData: DMBDIsItemConfiguredData*.
        DM_COMMAND_BD_IS_ITEM_CONFIGURED = 96,
        // 获取可配置的总数.
        // m_pData: DMBDGetTotalConfigurableItemCountData*.
        DM_COMMAND_BD_GET_CONFIGURABLE_ITEM_COUNT = 97,
        // 获取某个配置项的名称.
        // m_pData: DMBDGetConfiguredItemNameData*.
        DM_COMMAND_BD_GET_CONFIGURED_ITEM_NAME = 98,
        // 获取配置上的列表.
        // m_pData: DMBDGetConfiguredListData*.
        DM_COMMAND_BD_GET_CONFIGURED_LIST = 100,
        // 根据BroadSoft联系人配置项index, 获取BroadSoft联系人配置数据.
        // m_pData: DMBDGetConfigDataByIndexData*.
        DM_COMMAND_BD_GET_CONFIGURED_DATA_BY_INDEX = 101,
        // // 根据index设置BroadSoft联系人配置项.
        // m_pData: DMBDSetConfigDataByIndexData*.
        DM_COMMAND_BD_SET_CONFIGURED_DATA_BY_INDEX = 102,
        // 拨出BroadSoft联系人.
        // m_pData: DMBDCallOutData*.
        DM_COMMAND_BD_CALLOUT = 103,
        // 获取BroadSoft地址薄的显示名.
        // m_pData: DMBDGetDisplayNameData*.
        DM_COMMAND_BD_GET_DISPLAYNAME = 104,
        // 根据Click Action和搜索关键字获本地搜索Broadsoft联系人.
        DM_COMMAND_BD_LOCAL_SEARCH = 105,
        // 删除Persoanl联系人中的联系人
        // m_pData: DMDBRemoveListData*.
        DM_COMMAND_BD_DEL_PERSONAL_CONTACT = 106,
        // 添加BroadSoft到BroadSoft Personal组.
        // m_pData: DMBDBroadSoft2Personal*.
#ifdef _T42
        DM_COMMAND_BD_BROADSOFT_ADD_2_PERSONAL = 107,
        // 编辑BroadSoft Personal组中的编辑人
        // m_pData: DMBDBroadSoftEditContact*.
        DM_COMMAND_BD_BROADSOFT_EDIT_PERSONAL_CONTACT = 108,
#else
        DM_COMMAND_BD_ADD_2_PERSONAL = 107,
        // 编辑BroadSoft Personal组中的编辑人
        // m_pData: DMBDBroadSoftEditContact*.
        DM_COMMAND_BD_EDIT_PERSONAL_CONTACT = 108,
        //更新BroadSoft联系人
        //m_pData: NULL
        DM_COMMAND_BD_UPDATE = 109,
#endif

        DM_COMMAND_BD_COMMAND_END = 118,

        // LDAP联系人.
        DM_COMMAND_LDAP_COMMAND_BEGIN = 119,
        // LDAP异步搜索.
        // m_pData: DMLdapSearchAsyncData*.
        DM_COMMAND_LDAP_SEARCH_ASYNC = 120,
        // 判断LDAP是否开启.
        // m_pData: NULL.
        DM_COMMAND_CHECK_LDAP_STATUS = 121,
        // 设置LDAP开启, 或者关闭.
        // m_pData: DMLdapSetSetStatusData*.
        DM_COMMAND_SET_LDAP_STATUS = 122,
        // 获取LDAP显示名.
        // m_pData: DMLdapGetNameData*.
        DM_COMMAND_GET_LDAP_NAME = 123,
        // 设置LDAP显示名.
        // m_pData: DMLdapSetNameData*.
        DM_COMMAND_SET_LDAP_NAME = 124,
        // 查询Ldap来电查询是否开启.
        // m_pData: NULL.
        DM_COMMAND_CHECK_LDAP_CALLIN_SEARCH = 125,
        // 设置Ldap来电查询.
        // m_pData: DMLdapSetCallInSearchData*.
        DM_COMMAND_SET_LDAP_CALLIN_SEARCH = 126,
        // 查询Ldap去电查询是否开启.
        // m_pData: NULL.
        DM_COMMAND_CHECK_LDAP_CALLOUT_SEARCH = 127,
        // 设置Ldap去电查询.
        // m_pData: DMLdapSetCallOutSearchData*.
        DM_COMMAND_SET_LDAP_CALLOUT_SEARCH = 128,
        // 拨出LDAP联系人.
        // m_pData: DMLdapCallOutData*.
        DM_COMMAND_LDAP_CALLOUT = 129,

        DM_COMMAND_LDAP_COMMAND_END = 150,
    };

    DMCommandType m_eType;  // [in] 命令类型.
    void * m_pData; // [in] 数据.
    int m_nResult;  // [out] 结果, ==0表示成功.
    bool m_bRefreshUI; //[in]  是否要界面界面

    DMData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = DMData::DM_COMMAND_TYPE_NONE;
        m_pData = NULL;
        m_nResult = 1;
        m_bRefreshUI = true;
    }
};

// 结构体构造函数的实现.
#define DM_CTOR_IMP(ClassType)  \
ClassType()  \
{  \
    Reset();  \
}

// 本地联系人列表.
struct DMLocalDirGetContactListByClickActionData
{
    ContactGeneralItemListData m_stCGID;  // [out] 本地联系人列表.
    yl::string m_strClickAction;  // [in] 本地组的Click Action.

    DM_CTOR_IMP(DMLocalDirGetContactListByClickActionData);

    void Reset()
    {
        m_stCGID.Reset();
        m_strClickAction.clear();
    }
};

// 同步搜索本地联系人, 不显示多个号码.
struct DMLocalDirSearchData
{
    ContactGeneralSearchItemListData m_stCGSID;  // [out] 本地联系人搜索结果.
    yl::string m_strClickAction;  // [in] 本地组的Click Action.
    yl::string m_strKey;  // [in] 搜索关键字.

    DM_CTOR_IMP(DMLocalDirSearchData);

    void Reset()
    {
        m_stCGSID.Reset();
        m_strClickAction.clear();
        m_strKey.clear();
    }
};

// 同步搜索本地联系人, 显示多个号码.
struct DMLocalDirSearchWithMutlNumberData
{
    ContactGeneralSearchItemListData m_stCGSID;  // [out] 本地联系人搜索结果.
    yl::string m_strClickAction;  // [in] 本地组的Click Action.
    yl::string m_strKey;  // [in] 搜索关键字.

    DM_CTOR_IMP(DMLocalDirSearchWithMutlNumberData);

    void Reset()
    {
        m_stCGSID.Reset();
        m_strClickAction.clear();
        m_strKey.clear();
    }
};

// 异步搜索本地联系人, 不显示多个号码.
struct DMLocalDirSearchAsyncData
{
    yl::string m_strClickAction;  // [in] 本地组的Click Action.
    yl::string m_strKey;  // [in] 搜索关键字.
    int m_nIndexFrom;  // [in] 开始下标.
    int m_nCountToGet;  // [in] 获取多少个.
    NS_TA::CTaskActionCallbackBase * m_pCallBackBase; // [in] 回调指针.

    DM_CTOR_IMP(DMLocalDirSearchAsyncData);

    void Reset()
    {
        m_strClickAction.clear();
        m_strKey.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_pCallBackBase = NULL;
    }
};

// 异步搜索本地联系人, 显示多个号码.
struct DMLocalDirSearchWithMutlNumberAsyncData
{
    yl::string m_strClickAction;  // [in] 本地组的Click Action.
    yl::string m_strKey;  // [in] 搜索关键字.
    NS_TA::CTaskActionCallbackBase * m_pCallBackBase; // [in] 回调指针.

    DM_CTOR_IMP(DMLocalDirSearchWithMutlNumberAsyncData);

    void Reset()
    {
        m_strClickAction.clear();
        m_strKey.clear();
        m_pCallBackBase = NULL;
    }
};

// 获取联系人数量的上限(不包括黑名单).
struct DMLocalDirGetMaxContactCountData
{
    int m_nMaxCount;  // [out] 联系人上限(不包括黑名单).

    DM_CTOR_IMP(DMLocalDirGetMaxContactCountData);

    void Reset()
    {
        m_nMaxCount = 0;
    }
};

// 设置联系人数量的上限(不包括黑名单).
struct DMLocalDirSetMaxContactCountData
{
    int m_nMaxCount;  // [in] 联系人上限(不包括黑名单).

    DM_CTOR_IMP(DMLocalDirSetMaxContactCountData);

    void Reset()
    {
        m_nMaxCount = 0;
    }
};

// 获取当前联系人总数(不包括黑名单).
struct DMLocalDirGetCurrentContactCountData
{
    int m_nCurCount;  // [out] 联系人上限(不包括黑名单).

    DM_CTOR_IMP(DMLocalDirGetCurrentContactCountData);

    void Reset()
    {
        m_nCurCount = 0;
    }
};

// 获取黑名单联系人的上限值.
struct DMLocalDirGetMaxBlackCountData
{
    int m_nMaxCount;  // [out] 联系人上限(不包括黑名单).

    DM_CTOR_IMP(DMLocalDirGetMaxBlackCountData);

    void Reset()
    {
        m_nMaxCount = 0;
    }
};

// 设置黑名单联系人数量的上限值.
struct DMLocalDirSetMaxBlackCountData
{
    int m_nMaxCount;  // [in] 联系人上限(不包括黑名单).

    DM_CTOR_IMP(DMLocalDirSetMaxBlackCountData);

    void Reset()
    {
        m_nMaxCount = 0;
    }
};

// 获取当前联系人总数(不包括黑名单).
struct DMLocalDirGetCurrentBlackCountData
{
    int m_nCurCount;  // [out] 联系人上限(不包括黑名单).

    DM_CTOR_IMP(DMLocalDirGetCurrentBlackCountData);

    void Reset()
    {
        m_nCurCount = 0;
    }
};

// 更改联系人群组.
struct DMLocalDirMoveListToGroupData
{
    YLList<yl::string> m_listContact;  // [in] 联系人名列表.
    yl::string m_strGroupName;  // [in] 组名.

    DM_CTOR_IMP(DMLocalDirMoveListToGroupData);

    void Reset()
    {
        m_listContact.clear();
        m_strGroupName.clear();
    }
};

// 判断是否可以移动到某个组.
struct DMLocalDirIsListCanMoveData
{
    YLList<yl::string> m_listContact;  // [in] 联系人名列表.
    yl::string m_strGroupName;  // [in] 组名.

    DM_CTOR_IMP(DMLocalDirIsListCanMoveData);

    void Reset()
    {
        m_listContact.clear();
        m_strGroupName.clear();
    }
};

// 获取favorate联系人列表.
struct DMLocalDirGetFavorateListData
{
    ContactGeneralItemListData m_stCGILD;  // [out] 收藏夹列表.

    DM_CTOR_IMP(DMLocalDirGetFavorateListData);

    void Reset()
    {
        m_stCGILD.Reset();
    }
};

// 增加一组favorate联系人.
struct DMLocalDirToFavorateListData
{
    YLList<yl::string> m_listContact;  // [in] 联系人名列表.

    DM_CTOR_IMP(DMLocalDirToFavorateListData);

    void Reset()
    {
        m_listContact.clear();
    }
};

// 删除一组favorate联系人.
struct DMLocalDirRemoveFavorateListData
{
    YLList<yl::string> m_listContact;  // [in] 联系人名列表.

    DM_CTOR_IMP(DMLocalDirRemoveFavorateListData);

    void Reset()
    {
        m_listContact.clear();
    }
};

// 根据click action删除组中的所有联系人.
struct DMLocalDirRemoveAllContactsByClickActionData
{
    yl::string m_strClickAction;  // [in] 组的Click Action.

    DM_CTOR_IMP(DMLocalDirRemoveAllContactsByClickActionData);

    void Reset()
    {
        m_strClickAction.clear();
    }
};

// 新增一个联系人.
struct DMLocalDirAddContactData
{
    ContactGeneralItemData m_stCGID;  // [in] 联系人数据.

    DM_CTOR_IMP(DMLocalDirAddContactData);

    void Reset()
    {
        m_stCGID.Reset();
    }
};

#ifndef _T42
// 删除一组联系人.
struct DMLocalDirRemoveContactListData
{
    YLList<yl::string> m_listContact;  // [in] 联系人名列表.

    DM_CTOR_IMP(DMLocalDirRemoveContactListData);

    void Reset()
    {
        m_listContact.clear();
    }
};
#endif

// 获取联系人数据.
struct DMLocalDirGetContactDataData
{
    ContactGeneralItemData m_stCGID;  // [out] 数据.
    yl::string m_strContact;  // [in] 联系人名.

    DM_CTOR_IMP(DMLocalDirGetContactDataData);

    void Reset()
    {
        m_stCGID.Reset();
        m_strContact.clear();
    }
};

// 根据号码获取联系人数据.
struct DMLocalDirGetContactDataByNumberData
{
    ContactGeneralItemData m_stCGID;  // [out] 数据.
    yl::string m_strNumber;  // [in] 联系人号码.

    DM_CTOR_IMP(DMLocalDirGetContactDataByNumberData);

    void Reset()
    {
        m_stCGID.Reset();
        m_strNumber.clear();
    }
};

// 更改某个联系人的数据.
struct DMLocalDirUpdateContactDataData
{
    yl::string m_strContact;  // [in] 联系人名.
    ContactGeneralItemData m_stCGID;  // [in] 数据.

    DM_CTOR_IMP(DMLocalDirUpdateContactDataData);

    void Reset()
    {
        m_strContact.clear();
        m_stCGID.Reset();
    }
};

// 拨出联系人.
struct DMLocalDirCallOutData
{
    yl::string m_strNumber;  // [in] 号码.
    yl::string m_strName;  // [in] 名字.
    int m_iAccountID;           //[in] 帐号的ID, -1表示使用默认帐号

    DM_CTOR_IMP(DMLocalDirCallOutData);

    void Reset()
    {
        m_strNumber.clear();
        m_strName.clear();
        //-1表示使用默认帐号
        m_iAccountID = -1;
    }
};

// 获取当前favorate数量.
struct DMLocalDirGetFavorCountData
{
    int m_nCurCount;

    DM_CTOR_IMP(DMLocalDirGetFavorCountData);

    void Reset()
    {
        m_nCurCount = 0;
    }
};

// 获取组列表.
struct DMLocalDirGetGroupListData
{
    ContactGroupListData m_stCGLD;  // [out] 组详情列表.
    bool m_bIsShowBlacklist;    //[in] 是否显示黑名单

    DM_CTOR_IMP(DMLocalDirGetGroupListData);

    void Reset()
    {
        m_stCGLD.Reset();
        m_bIsShowBlacklist = true;
    }
};

// 获取组简要信息列表.
struct DMLocalDirGetGroupSimpleListData
{
    ContactGroupSimpleListData m_stCGSLD;  // [out] 组简要信息列表.

    DM_CTOR_IMP(DMLocalDirGetGroupSimpleListData);

    void Reset()
    {
        m_stCGSLD.Reset();
    }
};

// 获取组数量上限.
struct DMLocalDirGetMaxGroupCountData
{
    int m_nMaxCount;  // [out] 组数量上限.

    DM_CTOR_IMP(DMLocalDirGetMaxGroupCountData);

    void Reset()
    {
        m_nMaxCount = 0;
    }
};

// 设置组数量上限.
struct DMLocalDirSetMaxGroupCountData
{
    int m_nMaxCount;  // [in] 组数量上限.

    DM_CTOR_IMP(DMLocalDirSetMaxGroupCountData);

    void Reset()
    {
        m_nMaxCount = 0;
    }
};

// 获取当前组数量.
struct DMLocalDirGetCurrentGroupCountData
{
    int m_nCurCount;  // [out] 当前组数量.

    DM_CTOR_IMP(DMLocalDirGetCurrentGroupCountData);

    void Reset()
    {
        m_nCurCount = 0;
    }
};

// 根据ClickAction获取组数据.
struct DMLocalDirGetGroupDataByClickActionData
{
    ContactGroupData m_stCGD;  // [out] 组数据.
    yl::string m_strClickAction;  // [in] Click Action.

    DM_CTOR_IMP(DMLocalDirGetGroupDataByClickActionData);

    void Reset()
    {
        m_stCGD.Reset();
        m_strClickAction.clear();
    }
};

// 增加一个组.
struct DMLocalDirAddGroupData
{
    ContactGroupData m_stCGD;  // [in] 组数据.

    DM_CTOR_IMP(DMLocalDirAddGroupData);

    void Reset()
    {
        m_stCGD.Reset();
    }
};

// 删除一个组.
struct DMLocalDirRemoveGroupData
{
    yl::string m_strGroupName;  // [in] 组名.

    DM_CTOR_IMP(DMLocalDirRemoveGroupData);

    void Reset()
    {
        m_strGroupName.clear();
    }
};

// 更新组数据.
struct DMLocalDirUpdateGroupDataData
{
    yl::string m_strGroupName;  // [in] 组名.
    ContactGroupData m_stCGD;  // [in] 组数据.

    DM_CTOR_IMP(DMLocalDirUpdateGroupDataData);

    void Reset()
    {
        m_strGroupName.clear();
        m_stCGD.Reset();
    }
};

// 根据组名获取组id.
struct DMLocalDirGetGroupIdByGroupNameData
{
    int m_nGroupId;  // [out] 组id.
    yl::string m_strGroupName;  // [in] 组名.

    DM_CTOR_IMP(DMLocalDirGetGroupIdByGroupNameData);

    void Reset()
    {
        m_nGroupId = 0;
        m_strGroupName.clear();
    }
};

// 联系人是否正在同步.
struct DMLocalDirIsSyncData
{
    bool m_isSyncing;  // [out] 为true则正在同步.

    DM_CTOR_IMP(DMLocalDirIsSyncData);

    void Reset()
    {
        m_isSyncing = false;
    }
};

// 远程联系人列表.
struct DMRemoteDirGetContactListByClickActionData
{
    ContactRemoteListData m_stCRLD;  // [out] 远程联系人列表.
    yl::string m_strClickAction;  // [in] Click Action.

    DM_CTOR_IMP(DMRemoteDirGetContactListByClickActionData);

    void Reset()
    {
        m_stCRLD.Reset();
        m_strClickAction.clear();
    }
};

// 获取联系人列表.
struct DMRemoteDirGetContactListData
{
    ContactRemoteListData m_stCRLD;  // [out] 远程联系人列表.

    DM_CTOR_IMP(DMRemoteDirGetContactListData);

    void Reset()
    {
        m_stCRLD.Reset();
    }
};

// 获取标题.
struct DMRemoteDirGetTitleData
{
    yl::string m_strTitle;  // [out] 标题.

    DM_CTOR_IMP(DMRemoteDirGetTitleData);

    void Reset()
    {
        m_strTitle.clear();
    }
};

// 根据下标获取具体联系人数据.
struct DMRemoteDirGetUnitContactByIndexData
{
    ContactRemoteItemData m_stCRID;  // [out] 远程联系人数据.
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMRemoteDirGetUnitContactByIndexData);

    void Reset()
    {
        m_stCRID.Reset();
        m_nIndex = 0;
    }
};

// 根据下标进入下一级.
struct DMRemoteDirEnterContactByIndexData
{
    ContactRemoteItemType m_eRetType;  // [in] 该index项的类型.
    int m_nIndex;  // [in] 指定的下标.
    int m_nEnterError;  // [out] 错误类型, 为GetListResult枚举值.

    DM_CTOR_IMP(DMRemoteDirEnterContactByIndexData);

    void Reset()
    {
        m_eRetType = CONTACT_REMOTE_TYPE_NONE;
        m_nIndex = 0;
        m_nEnterError = 0;  // GET_LIST_OK
    }
};

// 通过快捷键获取对应URL下的联系人数据
struct DMRemoteDirEnterContactByHotKeyData
{
#ifdef _T42
    int m_nIndex;  // [in] 指定的下标.

    DM_CTOR_IMP(DMRemoteDirEnterContactByHotKeyData);

    void Reset()
    {
        m_nIndex = 0;
    }
#else
    ContactRemoteListData  m_stCRLD;  // [out] 远程联系人列表.
    int m_nKey;  // [in] 快捷键键值.
    int m_eStatus; //[out] 状态

    DM_CTOR_IMP(DMRemoteDirEnterContactByHotKeyData);

    void Reset()
    {
        m_stCRLD.Reset();
        m_nKey = -1;
        m_eStatus = -1;
    }
#endif
};

// 选中某个远程联系人.
struct DMRemoteDirSetSelectedIndexData
{
    int m_nIndex;  // [in] 指定的下标.

    DM_CTOR_IMP(DMRemoteDirSetSelectedIndexData);

    void Reset()
    {
        m_nIndex = 0;
    }
};

// 将远程联系人加入到本地ALL组.
struct DMRemoteDirRemote2LocalData
{
    int m_nIndex;  // [in] 指定的下标.

    DM_CTOR_IMP(DMRemoteDirRemote2LocalData);

    void Reset()
    {
        m_nIndex = 0;
    }
};

// 将远程联系人加入到黑名单组.
struct DMRemoteDirRemote2BlackData
{
    int m_nIndex;  // [in] 指定的下标.

    DM_CTOR_IMP(DMRemoteDirRemote2BlackData);

    void Reset()
    {
        m_nIndex = 0;
    }
};

// 某个指定下标的远程地址薄是否有配置.
struct DMRemoteDirIsItemConfiguredData
{
    int m_nIndex;  // [in] 指定的下标.

    DM_CTOR_IMP(DMRemoteDirIsItemConfiguredData);

    void Reset()
    {
        m_nIndex = 0;
    }
};

// 获取可配置的总数.
struct DMRemoteDirGetTotalConfigurableItemCountData
{
    int m_nCount;  // [out] 获取到的配置总数.

    DM_CTOR_IMP(DMRemoteDirGetTotalConfigurableItemCountData);

    void Reset()
    {
        m_nCount = 0;
    }
};


// 获取某个配置项的名称.
struct DMRemoteDirGetConfiguredItemNameData
{
    yl::string m_strName;  // [out] 名称.
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMRemoteDirGetConfiguredItemNameData);

    void Reset()
    {
        m_strName.clear();
        m_nIndex = 0;
    }
};

// 获取某个配置项的URL.
struct DMRemoteDirGetConfiguredItemUrlData
{
    yl::string m_strUrl;  // [out] URL.
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMRemoteDirGetConfiguredItemUrlData);

    void Reset()
    {
        m_strUrl.clear();
        m_nIndex = 0;
    }
};

// 获取配置好的列表.
struct DMRemoteDirGetConfiguredListData
{
    YLList<yl::string> m_listItem;  // [out] 配置好的列表.

    DM_CTOR_IMP(DMRemoteDirGetConfiguredListData);

    void Reset()
    {
        m_listItem.clear();
    }
};

// 拨出远程联系人.
struct DMRemoteCallOutData
{
    yl::string m_strNumber;  // [in] 号码.
    yl::string m_strName;  // [in] 名称.

    DM_CTOR_IMP(DMRemoteCallOutData);

    void Reset()
    {
        m_strNumber.clear();
        m_strName.clear();
    }
};

// 获取远程地址薄的显示名.
struct DMRemoteGetDisplayNameData
{
    yl::string m_strName;  // [out] 显示名.

    DM_CTOR_IMP(DMRemoteGetDisplayNameData);

    void Reset()
    {
        m_strName.clear();
    }
};

// 更新远程联系人.
struct DMRemoteUpdateDirData
{
    int m_nRetStatus;  // [out] 显示名.

    DM_CTOR_IMP(DMRemoteUpdateDirData);

    void Reset()
    {
        m_nRetStatus = -1;
    }
};

// 搜索远程联系人.
struct DMRemoteSearchDirData
{
    yl::string m_strKey;  // [in] 搜索关键字.
    int m_nIndexFrom;  // [in] 搜索开始下标.
    int m_nCountToGet;  // [in] 搜索获取数量.
    NS_TA::CTaskActionCallbackBase * m_pBase; // [in] 回调类指针.
    yl::string m_strClickAction;  // [in] ClickAction.
    bool bSearchAll; //[in]搜索所有的分组

    DM_CTOR_IMP(DMRemoteSearchDirData);

    void Reset()
    {
        m_strKey.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_pBase = NULL;
        bSearchAll = false;
    }
};

// 根据Click Action获取远程联系人列表.
struct DMBDGetContactListByClickActionData
{
    ContactBroadSoftListData m_stCBSLD;  // 远程联系人列表.
    yl::string m_strClickAction;  // Click Action.
    ContactBroadSoftItemData m_searchData;  // [in] 搜索关键字.
    int m_eStatus;  // [out] 下载状态.
    bool m_isToDownload;  // [in] 是否重新下载.
    bool m_bIsSearch;  // [in] 是否为搜索

    DM_CTOR_IMP(DMBDGetContactListByClickActionData);

    void Reset()
    {
        m_stCBSLD.Reset();
        m_strClickAction.clear();
        m_searchData.Reset();
        m_eStatus = 2;  // GET_LIST_DOWNLOADED_FAIL
        m_isToDownload = false;
        m_bIsSearch = false;
    }
};

// 根据Click Action和搜索关键字本地搜索Broadsoft联系人.
struct DMBDLocalSearchData
{
    ContactBroadSoftListData m_stCBSLD;  // 远程联系人列表.
    yl::string m_strClickAction;  // Click Action.
    yl::string m_strKey;

    DM_CTOR_IMP(DMBDLocalSearchData);

    void Reset()
    {
        m_stCBSLD.Reset();
        m_strClickAction.clear();
        m_strKey.clear();
    }
};

// 删除BroadSoft联系人信息.
struct DMDBRemoveListData
{
    YLList<yl::string> m_listContact;  // [in] 联系人名列表.

    yl::string m_strMenuAction; //[in] 对应组的Action

    void Reset()
    {
        m_listContact.clear();
        m_strMenuAction.clear();
    }
};

// BroadSoft添加到Personal组
struct DMBDBroadSoft2Personal
{
    yl::string m_strMenuAction;
    NS_DIRECTORY::ContactBroadSoftPersonal * m_pCbpData;

    void Reset()
    {
        m_strMenuAction.clear();
        m_pCbpData = NULL;
    }
};

// BroadSoft编辑Personal组中的联系人
struct DMBDBroadSoftEditContact
{
    yl::string m_strMenuAction;
    yl::string m_strOldNmae;
    NS_DIRECTORY::ContactBroadSoftPersonal * m_pCbpData;

    void Reset()
    {
        m_strMenuAction.clear();
        m_strOldNmae.clear();
        m_pCbpData = NULL;
    }
};

// 获取指定index的联系人数据.
#ifdef _T42
struct DMBDGetContactByIndexData
{
    NS_DIRECTORY::ContactBroadSoftItemBase * m_pItem; // [in out] 联系人数据的基类.
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMBDGetContactByIndexData);

    void Reset()
    {
        m_pItem = NULL;
        m_nIndex = 0;
    }
};
#else
struct DMBDGetContactByNameData
{
    NS_DIRECTORY::ContactBroadSoftItemBase * m_pItem; // [in out] 联系人数据的基类.
    yl::string m_strName;

    DM_CTOR_IMP(DMBDGetContactByNameData);

    void Reset()
    {
        m_pItem = NULL;
        m_strName = "";
    }
};
#endif

// 添加BroadSoft到本地ALL组.
struct DMBDBroadSoft2LocalData
{
#ifdef _T42
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMBDBroadSoft2LocalData);

    void Reset()
    {
        m_nIndex = 0;
    }
#else
    yl::string m_strName;

    DM_CTOR_IMP(DMBDBroadSoft2LocalData);

    void Reset()
    {
        m_strName = "";
    }
#endif
};

// 添加BroadSoft到本地Black组.
struct DMBDBroadSoft2BlackData
{
#ifdef _T42
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMBDBroadSoft2BlackData);

    void Reset()
    {
        m_nIndex = 0;
    }
#else
    yl::string m_strName;

    DM_CTOR_IMP(DMBDBroadSoft2BlackData);

    void Reset()
    {
        m_strName = "";
    }
#endif
};

// 某个指定下标的BroadSoft地址薄是否有配置.
struct DMBDIsItemConfiguredData
{
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMBDIsItemConfiguredData);

    void Reset()
    {
        m_nIndex = 0;
    }
};

// 获取可配置的总数.
struct DMBDGetTotalConfigurableItemCountData
{
    int m_nCount;  // [out] 总数.

    DM_CTOR_IMP(DMBDGetTotalConfigurableItemCountData);

    void Reset()
    {
        m_nCount = 0;
    }
};

// 获取某个配置项的名称.
struct DMBDGetConfiguredItemNameData
{
    yl::string m_strName;  // [out] 名称.
    int m_nIndex;  // [in] 下标.

    DM_CTOR_IMP(DMBDGetConfiguredItemNameData);

    void Reset()
    {
        m_strName.clear();
        m_nIndex = 0;
    }
};

// 获取配置上的列表.
struct DMBDGetConfiguredListData
{
    YLList<yl::string> m_listConfigured;  // [out] 配置列表.

    DM_CTOR_IMP(DMBDGetConfiguredListData);

    void Reset()
    {
        m_listConfigured.clear();
    }
};

// 根据BroadSoft联系人配置项index, 获取BroadSoft联系人配置数据.
struct DMBDGetConfigDataByIndexData
{
    const BroadSoftConfigItem * m_pstBSCI; // [in out] 获取到的配置.
    int m_nIndex;  // [in] 配置项的下标.

    DM_CTOR_IMP(DMBDGetConfigDataByIndexData);

    void Reset()
    {
        m_pstBSCI = NULL;
        m_nIndex = 0;
    }
};

// 根据index设置BroadSoft联系人配置项.
struct DMBDSetConfigDataByIndexData
{
    int m_nIndex;  // [in] 配置项的下标.
    BroadSoftConfigItem m_stBSCI;  // [in] 获取到的配置.

    DM_CTOR_IMP(DMBDSetConfigDataByIndexData);

    void Reset()
    {
        m_nIndex = 0;
        m_stBSCI.Reset();
    }
};

// 拨出BroadSoft联系人.
struct DMBDCallOutData
{
    yl::string m_strNumber;  // [in] 号码.
    yl::string m_strName;  // [in] 名称.

    DM_CTOR_IMP(DMBDCallOutData);

    void Reset()
    {
        m_strNumber.clear();
        m_strName.clear();
    }
};

// 获取BroadSoft地址薄的显示名.
struct DMBDGetDisplayNameData
{
    yl::string m_strName;  // [out] 显示名.

    DM_CTOR_IMP(DMBDGetDisplayNameData);

    void Reset()
    {
        m_strName.clear();
    }
};

// LDAP异步搜索.
struct DMLdapSearchAsyncData
{
    yl::string m_strKey;  // [in] 搜索关键字.
    int m_nIndexFrom;  // [in] 搜索起始下标.
    int m_nCountToGet;  // [in] 获取多少个.
    NS_TA::CTaskActionCallbackBase * m_pBase; // [in] 回调类指针.

    DM_CTOR_IMP(DMLdapSearchAsyncData);

    void Reset()
    {
        m_strKey.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_pBase = NULL;
    }
};

// 设置LDAP开启, 或者关闭.
struct DMLdapSetSetStatusData
{
    bool m_isOn;  // [in] 是否开启.

    DM_CTOR_IMP(DMLdapSetSetStatusData);

    void Reset()
    {
        m_isOn = false;
    }
};

// 获取LDAP显示名.
struct DMLdapGetNameData
{
    yl::string m_strName;  // [out] 显示名.

    DM_CTOR_IMP(DMLdapGetNameData);

    void Reset()
    {
        m_strName.clear();
    }
};

// 设置LDAP显示名.
struct DMLdapSetNameData
{
    yl::string m_strName;  // [in] 显示名.

    DM_CTOR_IMP(DMLdapSetNameData);

    void Reset()
    {
        m_strName.clear();
    }
};

// 设置Ldap来电查询.
struct DMLdapSetCallInSearchData
{
    bool m_isOn;  // [in] 是否开启.

    DM_CTOR_IMP(DMLdapSetCallInSearchData);

    void Reset()
    {
        m_isOn = false;
    }
};

// 设置Ldap去电查询.
struct DMLdapSetCallOutSearchData
{
    bool m_isOn;  // [in] 是否开启.

    DM_CTOR_IMP(DMLdapSetCallOutSearchData);

    void Reset()
    {
        m_isOn = false;
    }
};

// 拨出LDAP联系人.
struct DMLdapCallOutData
{
    yl::string m_strNumber;  // [in] 号码.
    yl::string m_strName;  // [in] 名字.

    DM_CTOR_IMP(DMLdapCallOutData);

    void Reset()
    {
        m_strNumber.clear();
        m_strName.clear();
    }
};

#ifdef __cplusplus
}  // __cplusplus
#endif

#endif

#endif  // DEFINE_DMCOMMON_H_
