#ifndef __MOD_DIRINTERFACR_H__
#define __MOD_DIRINTERFACR_H__

#include "ylvector.h"
#include "yllist.h"
#include <ylstring.h>
#include "directoryenumtypes.h"
#include "commondata.h"
#include "directorystructure.h"

struct ContactListData;

struct SuperSearchItemListData;

struct DirShortcutListData;

// 初始化
// 返回是否操作成功
//strConfigFile 指定联系人配置路径
//strTmpFile 指定联系人tmp路径
bool Dir_Init(const yl::string & strConfigFile, const yl::string & strTmpFile);

// 定时器, 外部统一响应定时器消息
// | iTimerID | [in] 定时器ID
// | bInTalk | [in] 当前是否处在通话中
// 返回此定时器是否已处理
//bool Dir_OnTimer(unsigned int iTimerID, bool bInTalk);

// 获取联系人快捷菜单列表.
// | pData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool Dir_GetShortcutList(DirShortcutListData * pData);


/*****************联系人 公共接口************************/


//获取联系人id链表，在ContactListData中指定返回开始位置，及返回个数
//nGroupId 组Id
//eType | 指定联系人种类
bool Dir_GetContactListByGroupId(ContactListData * pData, const int nGroupId, DIR_TYPE eType);

#if IF_FEATURE_MUTI_CALLLOG
//
bool Dir_GetChildGroupByGroupID(YLVector<int> & output, const int nGroupId, DIR_TYPE eType);
#endif

//本地搜索接口，异步搜索
//通过消息 LOCAL_SEARCH_RESULT 告知UI 结果，wparm:0  Lparm:0/1 失败/成功  ext:SearchKey
//eType | 指定联系人种类
bool Dir_LocalSearch(const yl::string & strSearchKey, const int nGroupId, DIR_TYPE eType);


//获取搜索结果数据，UI搜索搜索结果成功返回之后，调用该接口获取数据
//eType | 指定联系人种类
bool Dir_GetSearchResultList(const yl::string & strSearchKey, ContactListData * pData,
                             DIR_TYPE eType);

//获取联系人详情
//可获取联系组/联系人详情
//Remote、BroadSoft需下载联系人可通过该接口获取组状态
SmartPtr Dir_GetDetailById(int nId, DIR_TYPE eType);

bool Dir_GetSuperDataById(const SuperDirData & refInIterm, DirectorySearchData & refData,
                          DIR_TYPE eType,
                          bool bGetPhoto = true);

bool Dir_IsSearchHit(int nId, const yl::string & strSearchKey, SearchMethod searchMethod,
                     DIR_TYPE eType,
                     StringMatchType eMatchType);

//通过ID获取父ID
int Dir_GetParentIdById(int nId, DIR_TYPE eType);

// 获取最外层组的组名列表.
// | refData | [out] 是buffer.
// 如果获取成功则返回true, 否则返回false.
bool Dir_GetConfigMenuNameList(YLList<yl::string> & refData, DIR_TYPE eType);

//获取最外层组的id列表
//以上注释接口通过该接口实现，获取id，再通过id获取显示名
bool Dir_GetConfigMenuIdList(YLVector<int> & output, DIR_TYPE eType);

//号码匹配名字,根据配置自动匹配各种联系人
bool Dir_GetCotactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync = true);

//号码匹配名字 ,单种联系人匹配
bool Dir_GetCotactNameByNumber(const yl::string & strNumber, yl::string & strName, DIR_TYPE eType, bool bSync = true);

#if IF_BUG_38979
//来电异步匹配联系人
bool Dir_AsynchronousGetCotactNameByNumber(const yl::string & strNumber, const int iCallID);
#endif

//通过Index获取Id  ***Added By Yuyt! 2014/3/19
int Dir_GetContactIdByIndex(int index, int nGroup, DIR_TYPE eType);

//通过ID获取Index
int Dir_GetContactIndexById(int nId, int nGroup, DIR_TYPE eType);


//联系人是否有配置
bool Dir_IsDirConfiged(DIR_TYPE eType);

//通过传入id获取对应数据显示名
const yl::string & Dir_GetDisplayNameById(int nId, DIR_TYPE eType);

//通过组ID获取其下联系人总数
//多级结构获取其下所有具体联系人数目
int Dir_GetContactCount(int nGroupId, DIR_TYPE eType);

//取消下载
//使用组属性FileDownloadStatus才需使用该接口
void Dir_CancelLoad(int nGroupId, DIR_TYPE eType);

//超级搜索接口
bool Dir_SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output, DIR_TYPE eType,
                     StringMatchType eMatchType);

// 获取联系人搜索匹配方式
StringMatchType Dir_GetSearchMatchType();

//通过组Id返回下载状态 Added by Yuyt, 2014/03/20.
FileDownloadStatus Dir_GetDownLoadStatus(int nGroupId, DIR_TYPE eType);

//该组及其下组，是否都下载完毕
//该接口只能表示服务器是否有数据返回，
//如bsft联系人，服务器分页返回，有数据返回改接口返回true，如需知道是否全部分页结果已返回，请配合bsft联系人消息返回标志位一起使用
bool Dir_IsAllLoadFinish(int nGroupId, DIR_TYPE eType);

//目前只提供给BSTD_DIR及LOCAOL_DIR使用
//存在返回ID，不存在返回-1
//nGroupId某个组
int Dir_IsContactNameExist(const yl::string & strName, DIR_TYPE eType);

// 清除联系人数据
bool Dir_ClearContactData(int nGroupId, DIR_TYPE eType);

//成功返回true及快速索引列表
//refList:该组中存在的索引列表，如有A打头，则存在一项，否则没有
bool Dir_GetFastIndexListByGroupId(int nGroupId, FastIndexList & refList, DIR_TYPE eType);

DIR_INHERIT_TYPE Dir_GetFirstChildTypeByGroupId(int nGroupId, DIR_TYPE eType);


//清除MissCall等状态
void Dir_ClearNotifyList(int nGroupId, REQUEST_ID eReqID = DEFAULT_ID);

/*****************本地 历史记录接口************************/
// 增加一条通话记录.
// | refData | 是要添加的通话记录.
// 添加成功返回记录的ID, 否则返回knInvalidRId.
int LocalLog_AddLog(const LocalLogItemRecord & refData);

// 删除一条或多条通话记录.必须确保顺序，否则会删除出错
// | vecId | 是需通话记录的iD
//nGroupId | 为组id
// 删除成功返回true, 否则返回false.
bool LocalLog_RemoveLog(VectorId & vecId, int nGroupId);


//清空某类别下的历史记录
//nGroupId | 为组id
bool LocalLog_ClearLogs(int nGroupId, int nAccountID = -1);

//获取上次呼出的号码和呼出号码的账号id
bool LocalLog_GetLastOutCallNumAndAccountID(yl::string & strNumber, int & nAccountId);

//获取最近呼出最平凡的历史记录
bool LocalLog_SearchCallLogRecentOutCall(SuperSearchItemListData * pData);

// 重载本地历史记录
void LocalLog_Reload();

bool LocalLog_DetailedEnable();

/*******************************************************************
* @brief 是否启用合并本地历史记录功能
* @param 无
* @reture 返回是否启用
********************************************************************/
bool LocalLog_MergeLogEnable();

/*****************本地 联系人接口************************/

//添加一个组
//成功返回id,失败返回失败类型DIRECTORY_RET_CODE
int LocalDir_AddGroup(const ContactGroupData & groupData);

//更新组
//返回DIRECTORY_RET_CODE,如成功返回DIR_RET_SUCC
int LocalDir_UpdateGroup(int groupID, const ContactGroupData & groupData);

//删除组
bool LocalDir_RemoveGroup(int groupID);

//清空组
void LocalDir_ClearGroup();

//添加一条联系人，nGroupIndex为组Index，bSetDirty是否保存联系人到文件，返回联系人Id值或错误码
//nGroupId 组ID
//成功返回id,失败返回失败类型DIRECTORY_RET_CODE
int LocalDir_AddLocalContact(const ContactGeneralItemData & refContactData, int nGroupID);

//批量添加联系人，返回成功添加个数，或则错误码
int LocalDir_AddLocalContactList(YLList<ContactGeneralItemData> & refList, int nGroupId);


//vecRemoveId为删除联系人id列，nParentId为组Id，可为All组ID
bool LocalDir_RemoveContacts(const VectorId & vecRemoveId, int nParentId);

//清空某组下所有联系人,All组清空除黑名单外所有联系人
void LocalDir_ClearContact(int nGroupId);

//更新联系人
//refContactData为更新后联系人数据，nId为联系人ID
//nGroupId为组Id  更新后的组ID
//返回联系人Id值或错误码
int LocalDir_UpdateContact(const ContactGeneralItemData & refContactData, int nGroupId, int nId);

/************************************************************************
** 函数描述:   获取联系人的转接号码
** 参数：      strNum 联系人号码  strDivertNum 联系人转接号码
** 返回:       [out] true：成功 false：失败
************************************************************************/
bool LocalDir_GetDivertNumberByNumber(const yl::string & strNum, yl::string & strDivertNum);

//是否是黑名单号码
bool LocalDir_IsBlackContactNumber(const yl::string & strNum);

//通过号码获取铃声
const yl::string & LocalDir_GetRingByNumber(const yl::string & strNum);

// 将多个联系人移到某个组里。如果其中某个移动失败，直接返回。
// | listContactID | [in] 需要移动的联系人列表
// | nGroupID | [in] 把联系人移到某组的组ID
// | nGroupFromId | [in] 把联系人来自组，如all组为kszAllContactId
// 返回DIRECTORY_RET_CODE类型的错误码
DIRECTORY_RET_CODE LocalDir_MoveToGroup(YLVector<int> & vecContactID, int nGroupID,
                                        int nGroupFromId);

//通过号码获取头像
//strName为空，返回号码命中第一个，不为空，同时命中号码和命中才返回
const yl::string & LocalDir_GetContactImageByNumber(const yl::string & strNumber,
        const yl::string & strName = "");

const yl::string & BTDir_GetContactImageByNumber(const yl::string & strNumber,
        const yl::string & strName = "");

#if IF_FEATURE_FAVORITE_DIR
/*******************************************************************
* @brief Favorite联系人开关是否打开
* @param 无
* @reture 开关是否打开
********************************************************************/
bool LocalDir_IsFavoriteEnable();

/*******************************************************************
* @brief Favorite的Index是否存在
* @param nIndex 传入index
* @param nContactID 查找index排除该ID
* @reture index是否存在
********************************************************************/
bool LocalDir_IsFavoriteIndexExist(const int & nIndex, const int & nContactID);

/*******************************************************************
* @brief 自动创建Favorite Index
* @param 无
* @reture 返回Index
********************************************************************/
int LocalDir_GetFavoriteIndex();

/*******************************************************************
* @brief 联系人是否在Favorite 中
* @param nContactId 联系人ID
* @reture 返回是否存在
********************************************************************/

bool LocalDir_IsContactInFavorite(const int & nContactId);

/*******************************************************************
* @brief 复制联系人到Favorite组
* @param nContactId 联系人ID
* @param nFavoriteIndex 使用的Favorite Index，
* 为0表示使用自动生成Index
* @reture 返回复制结果
********************************************************************/
int LocalDir_CopyContactToFavorite(int nContactId, int nFavoriteIndex = 0);

/*******************************************************************
* @brief 从Favorite组中移除联系人（不删除联系人）
* @param vecRemoveId 移除联系人ID列表
* @reture 返回是否成功
********************************************************************/
bool LocalDir_RemoveContactsIdFromFavorite(const VectorId& vecRemoveId, int nParentID);

#endif
/*****************远程 联系人接口************************/

//通过hotKey获取信息，是否存在hotKey，获取其下联系人和其他流程一致
//FileDownloadStatus返回当前状态,如存在通过nReHotkeyId返回hotKeyId
//不存在返回FILE_DOWNLOAD_STATUS_NONE
FileDownloadStatus RemoteDir_GetHotKeyStatus(const yl::string & strKey, int nGroupId,
        int & nReHotkeyId);

//定时更新，UI点击更新
//nGroupId未指定更新某一地址簿，默认值更新所有
DIRECTORY_RET_CODE RemoteDir_Update(int nGroupId = -1);

//获取远程地址簿显示名
const yl::string & RemoteDir_GetRemoteDisplayName();

//从新下载某个组联系人
//strKey 用于远程搜索地址簿
DIRECTORY_RET_CODE RemoteDir_ReqLoadContact(int nGroupId, const yl::string & strKey = "");

int RemoteDir_GetRootGroupId(int nId);

bool RemoteDir_IsSpecialSearchGroup(int nGroupId);

/*****************BroadSoft 联系人接口************************/

//从新下载某个组联系人
DIRECTORY_RET_CODE BsftDir_ReqLoadContact(int nGroupId);

/*******************************************************************
** 函数描述:    下载下一页联系人
** 参数：       [in] GroupID
** 返回:        [out] 成功或者错误码
********************************************************************/
DIRECTORY_RET_CODE BsftDir_ReqNextPageContact(int nGroupId, bool bSearch = false);

//请求超级搜索联系人
DIRECTORY_RET_CODE BsftDir_ReqSearchContact(int nGroupId, ContactBroadSoftItemData & refSearchData);

//预加载函数，初始化,nGroup可以指定预加载组
//bLoad | 指定是否立即下载组
bool BsftDir_PreLoad(int nGroupId = knInvalidRId, bool bLoad = true);

//新建联系人，加入到Perosnal联系人中
//|refData|是加入到联系人的信息
//返回新增联系人ID,失败返回错误类型
//nId 不为-1则指定id
int BsftDir_AddContact2Personal(ContactBroadSoftPersonal & refData);

// 删除personal联系人
DIRECTORY_RET_CODE BsftDir_DeletePersonalContact(VectorId & vecRemoveId,
        BSFT_SEARCH_TYPE eType = SEARCH_NONE);

//新建联系人，加入到Perosnal联系人中
//|refData|是加入到联系人的信息
//返回失败信息，成功返回0
DIRECTORY_RET_CODE BsftDir_EditPersonalContact(int nId, ContactBroadSoftPersonal & refData,
        BSFT_SEARCH_TYPE eType = SEARCH_NONE);

//定时更新，UI点击更新
//nGroupId未指定更新某一地址簿，默认值更新所有
DIRECTORY_RET_CODE BsftDir_Update(int nGroupId = kszAllContactId);

//调用该接口之后，就会去下载对应组，UI不需要再次进行下载请求
DIRECTORY_RET_CODE BsftDir_OnSubmitXSIPassword(const yl::string & strPswd, int nGroupId);

//新建联系人，加入到本地的 All Conatct 联系人中
//|refData|是加入到联系人的信息
// iUIID | 是手柄的ID
//返回新增联系人ID,失败返回错误类型
//nId 不为-1则指定id
int BsftDir_AddContact2AllContact(ContactGeneralItemData & refData);

/*******************************************************************
** 函数描述:    是否需要自动更新Broadsoft联系人
** 参数：       NONE
** 返回:        [out] 是否需要自动更新Broadsoft联系人
********************************************************************/
bool BsftDir_UpdateEnable();

/*******************************************************************
** 函数描述:    清空Broadsoft 联系人 信息 包含组内联系人，状态等
** 参数：       [in] GroupID
** 返回:        [out] NONE
********************************************************************/
void BsftDir_ClearContactInfo(int nGroupID);

/*******************************************************************
** 函数描述:    是否有下一页数据
** 参数：       [in] nGroupID
** 返回:        [out] NONE
********************************************************************/
bool BsftDir_HasNextPage(int nGroupID);

/*****************BroadSoft 历史记录接口************************/

bool BsftLog_LogDeleteEnable();

//删除一条BSFT通话记录
DIRECTORY_RET_CODE BsftLog_RemoveSingleLog(int nId);

//删除一组BSFT通话记录
DIRECTORY_RET_CODE BsftLog_RemoveGroupLog(int nGroupId);

//请求下载联系人
DIRECTORY_RET_CODE BsftLog_ReqLoadCallLog(int nGroupId);

//调用该接口之后，就会去下载对应组，UI不需要再次进行下载请求
DIRECTORY_RET_CODE BsftLog_OnSubmitXSIPassword(const yl::string & strPswd, int nGroup);


//该接口对应于网页界面bsftLog配置项是否开启
bool BsftLog_IsEnable();

/*******************************************************************
** 函数描述:    获取当前组的历史记录类型
** 参数：       [in] nGroupID：组ID
** 返回:        [out] 历史记录类型
********************************************************************/
CALL_TYPE BsftLog_GetCallTypeByGroupID(int nGroupID);

#if IF_FEATURE_MUTI_CALLLOG
/*******************************************************************
** 函数描述:    返回最后一个Msii Call 的账号ID
** 参数：       NONE
** 返回:        [out] 账号ID
********************************************************************/
int LocalLog_GetLastChildMissAccountID();

/*******************************************************************
** 函数描述:    通过组ID和类型转换成各个账号对应的组ID
** 参数：       [in] nGroupID：Group ID
**              [in] eType：历史记录类型
** 返回:        [out] Group ID
********************************************************************/
int BsftLog_ChangeToActualGroup(int nGroupID, CALL_TYPE eType = CT_CALLALL);

/*******************************************************************
** 函数描述:    通过账号ID和类型转换成各个账号对应的组ID
** 参数：       [in] nAccountID：Account ID
**              [in] eType：历史记录类型
** 返回:        [out] Group ID
********************************************************************/
int BsftLog_GetGroupIDByAccountID(int nAccountID, CALL_TYPE eType = CT_CALLALL);

/*******************************************************************
** 函数描述:    清空Broadsoft 历史记录
** 参数：       [in] NONE
** 返回:        [out] NONE
********************************************************************/
void BsftLog_ClearCallLog();

/*******************************************************************
** 函数描述:    多账号 Broadsoft 历史记录开关是否打开
** 参数：       [in] NONE
** 返回:        [out] true 打开 false 关闭
********************************************************************/
bool BsftLog_IsMutilLogEnable();

/*******************************************************************
** 函数描述:    是否是賬號組ID
** 参数：       [in] nGroupID：Group ID
** 返回:        [out] true 是 false 否
********************************************************************/
bool BsftLog_IsAccountGroup(int nGroupID);

/*******************************************************************
** 函数描述:    通過组ID获取该组的账号ID
** 参数：       [in] nGroupID：Group ID
** 返回:        [out] 账号ID
********************************************************************/
int BsftLog_GetAccountIDByGroupID(int nGroupID);


/*******************************************************************
** 函数描述:    通過账号ID获取该组ID
** 参数：       [in] nAccountID：Account ID
** 返回:        [out] 组ID
********************************************************************/
int BsftLog_GetAccGroupIDByAccountID(int nAccountID);
#endif

/*******************************************************************
** 函数描述:    Broadsoft联系人是否打开
** 参数：       [in] NA
** 返回:        [out] true 是 false 否
********************************************************************/
bool BsftDir_IsEnable();

//手机蓝牙联系人重新同步
//返回DIR_RET_SUCC表示正在更新，其他返回值表示失败

//消息BLUETOOTH_CAONTACT_RELOAD，表示数据刷新完毕
DIRECTORY_RET_CODE BlueToothDir_Update();

//同步中取消同步
void BlueToothDir_CancelUpdate();


//暂时不实现图片下载功能
//蓝牙联系人保存到本地时，需调用此接口，触发后台下载图片
bool BlueToothDir_LoadContactPhoto();

//获取蓝牙联系人下载状态
FileDownloadStatus BlueToothDir_GetDownloadStatus();

/*****************PLCM Dir接口************************/

#if IF_BUG_PLCMDIR
DIRECTORY_RET_CODE PclmDir_ReqLoadContact(int nGroupId);


DIRECTORY_RET_CODE PclmDir_Update();

#endif //IF_BUG_PLCMDIR

/*****************Cloud 联系人接口************************/
#ifdef IF_FEATURE_CLOUD
//从新下载某个组联系人
DIRECTORY_RET_CODE CloudDir_ReqLoadContact(int nGroupId);

bool CloudDir_Init(int nAccountId);

bool CloudDir_UnInit(bool bRefreshUI = true);

// 关闭/启用自动更新功能
bool CloudDir_AutoUpdate(bool bEnable);

bool CloudDir_IsAccountBindCloudDir(int nAccountId);
#endif //#ifdef IF_FEATURE_CLOUD

/*******************************************************************
** 函数描述:    是否是云历史记录
** 参数：        [in] nId：历史记录ID
** 返回:      [out] true 是 false 不是
********************************************************************/
bool Dir_IsCloudCallLog(int nId);

#if IF_FEATURE_GBBOOK

//GAB
DIRECTORY_RET_CODE GAB_ReqSearchGABByType(const yl::string & strkey, GBBOOK_OPERATION_TYPE eType);

const yl::string & GAB_GetCurentGabKey();

GBBOOK_OPERATION_TYPE GAB_GetCurrentGabOptType();

void GAB_ClearSearchInfo();

//PAB
DIRECTORY_RET_CODE PAB_ReqAddGroup(const yl::string & strName);

DIRECTORY_RET_CODE PAB_ReqRemoveGroup(int nGroupId);

DIRECTORY_RET_CODE PAB_ReqModifyGroup(int nGroupId, const yl::string & strName);

DIRECTORY_RET_CODE PAB_ReqAddContact(ContactGBBookUnit & refData);

DIRECTORY_RET_CODE PAB_ReqRemoveContact(int nId);

DIRECTORY_RET_CODE PAB_ReqModifyContact(int nId, ContactGBBookUnit & refData);

bool PAB_UpdatePabData();

int PAB_IsContactNameExist(const yl::string & strName);

#endif

#if IF_FEATURE_METASWITCH_CALLLOG
/*****************Metaswitch 历史记录接口************************/

/*******************************************************************
* @brief 是否启用MTSW历史记录
* @param 无
* @reture 返回是否启用
********************************************************************/
bool MTSWLog_IsEnable();

/*******************************************************************
* @brief 是否启用本地历史记录
* @param 无
* @reture 返回是否启用
********************************************************************/
bool LocalLog_IsEnable();

/*******************************************************************
* @brief 请求下载联系人
* @param 无
* @reture 返回执行结果
********************************************************************/
DIRECTORY_RET_CODE MTSWLog_ReqLoadCallLog();

/*******************************************************************
* @brief Metaswitch历史记录是否在下载中
* @param 无
* @reture 返回结果
********************************************************************/
bool MTSWLog_IsDownLoading();
#endif


#if IF_FEATURE_METASWITCH_DIRECTORY
/*****************Metaswitch 联系人接口************************/

/*******************************************************************
* @brief 新建联系人，加入到contact联系人中 nId 不为-1则指定id
* @param refData| 联系人添加的信息
* @reture 返回新增联系人ID,失败返回错误类型
********************************************************************/
int MtswDir_AddContact2ContactList(ContactMetaSwitchContactList & refData, bool bFouceAdd = false);

/*******************************************************************
* @brief 编辑联系人
* @param nId| 联系人ID
* @param refData| 联系人修改的信息
* @reture 返回执行结果
********************************************************************/
DIRECTORY_RET_CODE MtswDir_EditContactList(int nId, ContactMetaSwitchContactList & refData,
        bool bFouceEdit = false);

//删除联系人
DIRECTORY_RET_CODE MtswDir_DeleteContactList(VectorId & vecRemoveId);

//删除所有联系人
DIRECTORY_RET_CODE MtswDir_ClearContact(int nGroupId);

//是否启用MTSW联系人
bool MtswDir_IsEnable();

//取消下载
void MtswDir_CancelLoad();

DIRECTORY_RET_CODE MtswDir_Update(int nGroupId  = kszAllContactId);

//排序联系人
void MtswDir_SortContactList();

//获取排序类型
SORT_TYPE MtswDir_GetSortType();
#endif
#endif
