#ifndef __MOD_ACCOUNT_H__
#define __MOD_ACCOUNT_H__

#include "account_inc.h"

#include "account_datadefine.h"
typedef YLList<int> ListAccounts;

/*******************************************************************
** 函数描述: 账号模块初始化
** 参数：    无
** 返回:     无
********************************************************************/
void acc_Init();

/*******************************************************************
** 函数描述: 返回账号个数
** 参数：    账号类型，默认为SIP账号ACCOUNT_TYPE_SIP
** 返回:   账号个数
********************************************************************/
int acc_AccountNum(int eAccountType = ACCOUNT_TYPE_SIP);

/*******************************************************************
** 函数描述: 账号ID是否有效
** 参数：    账号id
** 返回:   账号ID有效性
********************************************************************/
bool acc_IsAccountIDValid(int iAccount);

/*******************************************************************
** 函数描述: 获取指定账号的状态
** 参数：    账号ID
** 返回:  账号状态
********************************************************************/
LINE_STATE acc_GetState(int iAccountID);

/*******************************************************************
** 函数描述: 获取已注册的账号数目
** 参数：    账号类型，默认为SIP账号ACCOUNT_TYPE_SIP
** 返回:  已注册的账号数目
********************************************************************/
int acc_GetRegisteredAccountNum(int eAccountType = ACCOUNT_TYPE_SIP);

/*******************************************************************
** 函数描述: 获取注册失败的账号数目
** 参数：    账号类型，默认为SIP账号ACCOUNT_TYPE_SIP
** 返回:    注册失败账号列表
********************************************************************/
ListAccounts acc_GetRegisteFailAccountList(int eAccountType  = ACCOUNT_TYPE_SIP);

/*******************************************************************
** 函数描述:   获取当前账号
** 参数：      无
** 返回:       当前账号ID
********************************************************************/
int acc_GetDefaultAccount();

/*******************************************************************
** 函数描述:   设置当前账号,当前账号必须可用
** 参数：      iAccount-账号ID; bSave-是否存入配置文件
** 返回:       是否默认账号设置成功
********************************************************************/
bool acc_SetDefaultAccount(int iAccount, bool bSave = false);

/*******************************************************************
** 函数描述:   切换当前账号（按账号ID大小切换当前账号）
** 参数：      bForward true=向前切换（账号ID增大），false=向后切换
** 返回:       当前账号ID
********************************************************************/
int acc_SwitchDefaultAccount(bool bForward = true);

/*******************************************************************
** 函数描述:   账号是否可用（账号状态为已注册或者无需注册，PSTN账号已注册且不在通话中）
** 参数：      账号ID
** 返回:       账号是否可用
********************************************************************/
bool acc_IsAccountAvailable(int iAccount);

/*******************************************************************
** 函数描述:   是否存在指定类型的可用账号
** 参数：      账号类型，默认为SIP账号ACCOUNT_TYPE_SIP
** 返回:       是否存在可用账号
********************************************************************/
bool acc_ExistAvailableAccount(int eAccountType = ACCOUNT_TYPE_SIP);

/*******************************************************************
** 函数描述:   获取一个可用的账号id
**   如果指定获取第一个账号，则在账号列表中按顺序查找，返回第一个对应类型可用账号。
**   如果不指定获取第一个账号，则优先使用默认账号；默认账号不是指定类型或者不可用则按列表顺序查找对应类型可用账号。
**   如果找不到可用账号则返回无效账号ID（INVALID_ACCOUNTID）。
** 参数：      bFirst是否获取第一个, false时返回默认账号
**             eAccountType 账号类型，默认为SIP账号ACCOUNT_TYPE_SIP
** 返回:       账号id
********************************************************************/
int acc_GetUsableAccount(bool bFirst = false, int eAccountType = ACCOUNT_TYPE_SIP);

/*******************************************************************
** 函数描述:   获取一个可用的账号id
** 参数：      bFirst是否获取第一个, false时返回默认账号
** 返回:       账号id
********************************************************************/
int acc_GetUsableAccount(const yl::string& strServer, const yl::string& strName = "");

#ifdef IF_FEATURE_PSTN
/**
 * @brief 获取与指定账号相似的可用账号ID
 * @details 如果首选账号可用，则返回首选账号ID。其次优先使用相同类型的可用
 * 最小编号账号。再次使用其他类型的可用账号。如果没有可用账号，则返回无效
 * 账号ID（INVALID_ACCOUNTID）。
 *
 * @param iFirstChoiceAccountId 首选账号ID
 *
 * @return 可用账号ID。无指定条件的可用账号则返回INVALID_ACCOUNTID。
 */
int acc_GetUsableAccountLike(int iFirstChoiceAccountId);
#endif // IF_FEATURE_PSTN

/*******************************************************************
** 函数描述:   获取获取指定账号的PickupCode，如果账号的配置为空，会取公共的配置
** 参数：      [in] iAccountId: 账号的ID
**             [in] eType: PICKUP的类型s
** 返回:       是否获取成功
********************************************************************/
yl::string acc_GetPickupCodeByType(int iAccountId, PICKUP_TYPE eType);

/*******************************************************************
** 函数描述:        返回第一个有用户名的账号
** 参数：            无
** 返回:          账号ID
********************************************************************/
int acc_GetFirstUserNameAccount();

/*******************************************************************
** 函数描述:        通过账号名获取账号ID
** 参数：            账号名
** 返回:          账号ID
********************************************************************/
int acc_GetAccountIDByName(const char * pName, const char * pServer = NULL);

/*******************************************************************
** 函数描述:        通过服务器名获取账号ID
** 参数：            服务器名
** 返回:          账号ID
********************************************************************/
int acc_GetAccountIDByServer(const yl::string & strServer);

/*******************************************************************
** 函数描述:        通过账号全名获取账号ID
** 参数：            服务器名
** 返回:          账号ID
********************************************************************/
int acc_GetAccountIDByFullName(const char * pFullName);

#ifdef IF_FEATURE_PSTN
/**
 * @brief 通过PSTN账号名称获取账号ID
 *
 * @param szPstnName PSTN账号名称，格式为"PSTN%d"
 * @return 账号ID。无法找到则返回INVALID_ACCOUNTID。
 */
int acc_GetAccountIDByPstnName(const char * szPstnName);

/**
 * @brief 通过设备ID获取账号ID（针对PSTN）
 *
 * @param iDeviceId 设备ID
 * @return 账号ID。无法找到则返回INVALID_ACCOUNTID。
 */
int acc_GetAccountIDByDeviceID(int iDeviceId);

/**
 * @brief 通过账号ID获取设备ID（针对PSTN）
 *
 * @param iAccountId 账号ID
 * @return 设备ID
 */
int acc_GetDeviceIDByAccountID(int iAccountId);
#endif // IF_FEATURE_PSTN

/*******************************************************************
** 函数描述:        通过sip信息获取账号ID
** 参数：            服务器名
** 返回:          账号ID
********************************************************************/
int acc_GetAccountIDByUserInfo(BasicUserInfo * pUser);

/*******************************************************************
** 函数描述:        获取第一个有服务器配置的账号
** 参数：            void
** 返回:          账号ID
********************************************************************/
int acc_GetNoEmptyServerAccountID();

/**
 * @brief 判断账号是否启用
 * @param iAccount 账号ID
 * @return 账号是否启用
 */
bool acc_IsAccountEnable(int iAccount);
/**
 * @brief 设置账号是否启用
 * @param iAccount 账号ID
 * @param bEnable 账号是否启用
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountEnable(int iAccount, bool bEnable,
                          CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**
 * @brief 通过账号ID获取账号Label配置
 * @param iAccountID 账号ID
 * @return 账号Label配置
 */
yl::string acc_GetAccountLabel(int iAccountID);
/**
 * @brief 设置账号Label
 * @param iAccount 账号ID
 * @param strLabel 账号Label
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountLabel(int iAccountID, const yl::string& strLabel,
                         CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**
 * @brief 判断账号是否开启记录未接来电
 * @param iAccountID 账号ID
 * @return 账号是否开启记录未接来电
 */
bool acc_IsAccountSaveMissCallLog(int iAccountID);

/**
 * @brief 判断账号是否开启自动应答
 * @param iAccountID 账号ID。为INVALID_ACCOUNTID时，查询所有可用账号，有任意账号启用则返回true。
 * @return 账号是否开启自动应答
 */
bool acc_IsAccountAutoAnswerEnable(int iAccountID = INVALID_ACCOUNTID);

/**
 * @brief 设置账号是否开启自动应答
 * @param iAccount 账号ID
 * @param bAutoAnswer 账号是否开启自动应答
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountAutoAnswer(int iAccountID, bool bAutoAnswer,
                              CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**
 * @brief 判断账号是否开启自动应答静音
 * @param iAccountID 账号ID
 * @return 账号是否开启自动应答静音
 */
bool acc_IsAccountAutoAnswerMute(int iAccountID);
/**
 * @brief 设置账号是否开启自动应答静音
 * @param iAccount 账号ID
 * @param bAutoAnswerMute 账号是否开启自动应答静音
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountAutoAnswerMute(int iAccountID, bool bAutoAnswerMute,
                                  CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**
 * @brief 通过账号ID获取账号铃声配置
 * @param iAccountID 账号ID
 * @return 账号铃声配置
 */
yl::string acc_GetAccountRing(int iAccountID);
/**
 * @brief 设置账号铃声配置
 * @param iAccount 账号ID
 * @param strRing 账号铃声配置
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountRing(int iAccountID, const yl::string& strRing,
                        CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/*******************************************************************
** 函数描述:        获取账号显示名
** 参数：            iAccountID-账号ID
**                  bUseDisplayNameFirst 是否优先使用Display Name。
**                      true=CustomShowText>DisplayName>Label>RegisterName>UserName
**                      false=CustomShowText>Label>DisplayName>RegisterName>UserName
** 返回:          账号的显示名称
********************************************************************/
yl::string acc_GetAccountShowText(int iAccountID, bool bUseDisplayNameFirst = false);

/************************************************************************
** 函数描述:   设置临时的账号显示文本，该显示文本拥有最高显示优先级
               该函数只影响acc_GetAccountShowText的结果
** 参数：      iAccount 账号id
               strText 需要显示的文本
** 返回:       是否成功
************************************************************************/
bool acc_SetCustomShowText(int iAccount, const yl::string & strText);

/************************************************************************
** 函数描述:   获取临时的账号显示文本，该显示文本拥有最高显示优先级
               该函数只影响acc_GetAccountShowText的结果
** 参数：      iAccount 账号id
** 返回:       显示文本
************************************************************************/
yl::string acc_GetCustomShowText(int iAccount);

#ifndef _T49
/************************************************************************
** 函数描述:   获取未读语音留言数量
** 参数：      [in] iAccountID: 账号ID
** 返回:      未读语音留言数量
************************************************************************/
int acc_GetUnReadVoiceMailNum(int iAccountID = INVALID_ACCOUNTID);

/************************************************************************
** 函数描述:   获取已读语音留言数量
** 参数：      [in] iAccountID: 账号ID
** 返回:      已读语音留言数量
************************************************************************/
int acc_GetReadedVoiceMailNum(int iAccountID = INVALID_ACCOUNTID);

/************************************************************************
** 函数描述:   获取最后一个来语音留言的账号
** 参数：
** 返回:      最后一个来语音留言的账号ID
************************************************************************/
int acc_GetLastVMailAccountId();

#endif
/************************************************************************

** 函数描述:   获取账号注册失败代码
** 参数：      账号ID
** 返回:      失败代码
************************************************************************/
int acc_GetRegisterFailId(int iAccount);

/************************************************************************
** 函数描述:   获取账号注册失败字符串信息
** 参数：      账号ID
** 返回:       失败信息
************************************************************************/
yl::string acc_GetRegisterFailInfo(int iAccount);

/************************************************************************
** 函数描述:   获取账号是否为IP直播账号
** 参数：      账号ID
** 返回:       是否为IP直播
************************************************************************/
bool acc_IsDirectIPAccount(int iAccount, bool bCheck = true);

#ifdef IF_FEATURE_PSTN
/**
 * @brief 判断账号是否为PSTN账号
 *
 * @param iAccount 账号ID
 *
 * @return 账号是否为PSTN账号
 */
bool acc_IsPstnAccount(int iAccount);
#endif // IF_FEATURE_PSTN

/************************************************************************
** 函数描述:   获取账号的服务器名
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string acc_GetServerName(int iAccount);

/************************************************************************
** 函数描述:   获取账号的服务器端口
** 参数：      账号ID
** 返回:       服务器端口
************************************************************************/
int acc_GetServerPort(int iAccount);

/************************************************************************
** 函数描述:   获取账号的服务器类型
** 参数：      账号ID
** 返回:       服务器类型
************************************************************************/
SIP_SERVER_TYPE acc_GetServerType(int iAccount);

/************************************************************************
** 函数描述:   是否是Genesys账号
** 参数：      账号ID
** 返回:       是否
************************************************************************/
bool acc_IsGenesysAccount(int iAccountID);

/************************************************************************
** 函数描述:   获取账号当前服务器的传输协议
** 参数：      账号ID
** 返回:       服务器序号
************************************************************************/
int acc_GetServerTransport(int iAccount);

/************************************************************************
** 函数描述:   获取账号的注册名
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string acc_GetRegisterName(int iAccount);

/************************************************************************
** 函数描述:   获取账号的标签
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string acc_GetLabel(int iAccount);

/************************************************************************
** 函数描述:   获取账号的显示名
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string acc_GetDisplayName(int iAccount);

/************************************************************************
** 函数描述:   获取账号的用户名
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string acc_GetUsername(int iAccount);

/************************************************************************
** 函数描述:   获取账号的密码
** 参数：      账号ID
** 返回:       密码
************************************************************************/
yl::string acc_GetPassward(int iAccount);

/************************************************************************
** 函数描述:   获取账号的全名 User@Domain
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string acc_GetFullName(int iAccount);

/************************************************************************
** 函数描述:   获取账号的BLA类型名
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
BLA_TYPE acc_GetBLAType(int iAccount);

/************************************************************************
** 函数描述:   获取ACD账号
** 参数：      void
** 返回:       服务器名
************************************************************************/
int acc_GetAcdAccountID();

/************************************************************************
** 函数描述:   获取hetling账号
** 参数：      void
** 返回:       服务器名
************************************************************************/
int acc_GetHetlingAccountID();

/************************************************************************
** 函数描述:   获取SIP IP直拨账号
** 参数:       bCheck 是否校验IP直拨功能是否启用。
** 返回:       IP直拨账号ID，没有IP直拨账号则返回无效账号ID（INVALID_ACCOUNTID）。
************************************************************************/
int acc_GetDirectIPAccountID(bool bCheck = true);

/************************************************************************
** 函数描述:   获取所有有效账号ID
** 参数：      void
** 返回:       服务器名
************************************************************************/
ListAccounts acc_GetValidAccountList(int eAccountType = ACCOUNT_TYPE_ALL);

/************************************************************************
** 函数描述:   获取所有可用账号ID
** 参数：      void
** 返回:       账号列表
************************************************************************/
ListAccounts acc_GetAvailableAccountList(int eAccountType = ACCOUNT_TYPE_ALL);

/**
 * @brief 获取指定类型所有账号ID列表
 *
 * @param eAccountType 账号类型，默认为所有账号ACCOUNT_TYPE_ALL
 * @return 指定类型所有账号ID列表
 */
ListAccounts acc_GetAccountList(int eAccountType = ACCOUNT_TYPE_ALL, bool bOnlyUser = false);

/************************************************************************
** 函数描述:   获取所账号的通话模式
** 参数：      账号ID
** 返回:       通话模式
************************************************************************/
CALL_AV_TYPE acc_GetDefaultCallMode(int iAccount);

/************************************************************************
** 函数描述:   清除内存中的账号数据
** 参数：      eAccountType 擦除的账号主类型, iType 擦除的账号子类型: -1代表所有
** 返回:       void
************************************************************************/
void acc_EraseAllAccount(int eAccountType = ACCOUNT_TYPE_SIP, int iType = -1);

/************************************************************************
** 函数描述:   重置账号文件
** 参数：      eAccountType 擦除的账号主类型, iType 擦除的账号子类型: -1代表所有
** 返回:       void
************************************************************************/
void acc_EmptyAllAccount(int eAccountType = ACCOUNT_TYPE_SIP, int iType = -1);

void acc_EraseAccountById(int iAccount);

void acc_EmptyAccountById(int iAccount);

/************************************************************************
** 函数描述:   从文件加载一个账号的数据
** 参数：      账号ID
** 返回:       void
************************************************************************/
void acc_LoadAccount(int iAccount);

/************************************************************************
** 函数描述:   向sip请求刷新账号状态
** 参数：      账号ID
** 返回:       void
************************************************************************/
void acc_RefreshLineState(int iAccount);

/************************************************************************
** 函数描述:   出厂前登出账号
** 参数：      void
** 返回:       bool
************************************************************************/
bool acc_LogoutBeforeReset();

/************************************************************************
** 函数描述:   是否为出厂前登出账号中
** 参数：      void
** 返回:       bool
************************************************************************/
bool acc_BeWaitLogoutBeforeReset();


/************************************************************************
** 函数描述:   设置同步标志
** 参数：      ID
** 返回:       void
************************************************************************/
void acc_SetSyncSubscribe(int iAccount,  bool bState);

/************************************************************************
** 函数描述:   获取同步标志
** 参数：      ID
** 返回:       同步标志
************************************************************************/
bool acc_GetSyncSubscribe(int iAccount);


/************************************************************************
** 函数描述:   获取以字符串形式获取所有账号的信息
** 参数：      void
** 返回:       void
************************************************************************/
void acc_GetAccountInfo(yl::string & strInfoOut, int eAccountType = ACCOUNT_TYPE_SIP);

/************************************************************************
** 函数描述:   以字符串形式配置账号信息
** 参数：      strInfoIn  配置信息
** 返回:       void
************************************************************************/
bool acc_SetAccountInfo(yl::string & strInfoIn);

/************************************************************************
** 函数描述:   是否所有的账号都已禁用(仅判断sip账号)
** 参数：      void
** 返回:       bool
************************************************************************/
bool acc_IsAllAccountDisable();


/************************************************************************
** 函数描述:   备份账号配置文件
** 参数：      void
** 返回:       void
************************************************************************/
void acc_BackupRestoreCfg(int iAccountId, bool bIsRestore);

/************************************************************************
** 函数描述:   映射sip账号状态到本地的状态
** 参数：      iState:sip传过来的状态
** 返回:       本地状态类型
************************************************************************/
// 映射sip账号状态到本地的账号
LINE_STATE acc_MapState(int iState);

#if IS_CODE_TEST
//测试用，发送消息
LRESULT acc_AccountOnMessage(msgObject & msg);
#endif

#ifdef IF_FEATURE_GETBALANCE
/*******************************************************************
** 函数描述:    下载当前通话的账号余额信息
** 参数：        [in] iAccountId: 账号的ID
**              [in] eType: PICKUP的类型
** 返回:      是否获取成功
********************************************************************/
void acc_LoadBalance();

/*******************************************************************
** 函数描述:    设置在结束通话后需要获取账号余额信息的账号
** 参数：        [in] iAccount: 在结束通话后需要获取账号余额信息的账号
** 返回:      无
********************************************************************/
void acc_SetNeedGetBalanceAccount(int iAccount, bool bRealLoad = false);

/*******************************************************************
** 函数描述:    获取账号的视频参数
** 参数：        [in] iAccount: 账号ID
            [in] iConfigID: 配置项ID
            [in] strConfigID: 配置项名称
            [in] strParamName: 配置名称
** 返回:      配置值
********************************************************************/
yl::string acc_GetVideoParam(int iAccountID, int iConfigID, const yl::string & strConfigID,
                             const yl::string & strParamName);

/*******************************************************************
** 函数描述:    获取账号余额信息
** 参数：        [Balance_Info_t] tInfo:获取账号余额信息的账号
** 返回:      无
********************************************************************/
void acc_GetBalanceInfo(Balance_Info_t & tInfo);

/*******************************************************************
** 函数描述:    取消此次账号余额下载
** 参数：        无
** 返回:      无
********************************************************************/
void acc_CancelLoadBalance();

/*******************************************************************
** 函数描述:    返回是否正在加载余额信息
** 参数：        无
** 返回:      无
********************************************************************/
bool acc_IsLoadingBalance();
#endif

yl::string acc_GetBLANum(int iAccount);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
bool acc_GetHotelingSwitch(int iAccount);

bool acc_GetFlexibleSeatingSwitch(int iAccount);
#endif

// 是否是不需要注册的账号
bool acc_IsNoNeedRegisterAccount(const int iAccount);
// 账号状态是否是最终的注册状态（不含Disable），相对中间状态而言
bool acc_IsInFinalRegisterState(const int iAccount);
// 是否是可用的账号状态
bool acc_IsUseableStatus(const LINE_STATE eState);
// 是否是禁用的账号状态
bool acc_IsDisableStatus(int iAccount);

#ifdef _DECT
/*******************************************************************
** 函数描述:        是否为TLS 加密
** 参数：        [in] iAccount: 在结束通话后需要获取账号余额信息的账号
** 返回:          bool
********************************************************************/
bool acc_IsTLSEncrypt(int iAccount);

/*******************************************************************
** 函数描述:        通过display name
** 参数：        [in] basicInfo: user info
[out] strDisplayName: displayname
** 返回:          bool
********************************************************************/
bool acc_GetDisplayNameByFullName(const BasicUserInfo & basicInfo, yl::string & strDisplayName);
#endif // _DECT

#ifdef _DECT
// 获取账号信息列表
bool acc_GetAccountInfo(YLList<AccountInfo> & listAccount);
#endif

// 获取紧急呼叫的账号
int acc_GetEmergencyAccount();
// 是否开启BLA
bool acc_IsBLAEnable(int iAccount);

// BLA是否需要占线
bool acc_IsBLANeedSeize(int iAccount);

int acc_GetAvailableAccountCount();

#if IF_FEATURE_32591
yl::string acc_GetStatusBarLabel(int iAccount);

yl::string acc_GetAccountStatusShowText(int iAccount);
#endif

/*******************************************************************
** 函数描述:    获取账号类型:sip/h323/pstn
** 参数：        [in] iAccount: Account Id
** 返回:      无
********************************************************************/
ACCOUNT_TYPE acc_GetAccountType(int iAccount);

/*******************************************************************
** 函数描述:    获取sip账号类型:common/yealink/other cloud
** 参数：        [in] iAccount: Account Id
** 返回:      无
********************************************************************/
SipAccountType acc_GetSipAccountType(int iAccount);

#ifdef IF_FEATURE_CLOUD
/*******************************************************************
** 函数描述:    获取已经注册的Cloud类型
** 参数：        [in] iAccount: Account Id
** 返回:      无
********************************************************************/
SipAccountType acc_GetRegisterSipAccountTypeById(int iAccount);

/*******************************************************************
** 函数描述:    获取注册上的Yealink Cloud账号Id
** 参数：        无
** 返回:      [out] 账号id，没有注册上的Yealink Cloud账号返回-1
********************************************************************/
int acc_GetRegisterYlCloudAccount();

/*******************************************************************
** 函数描述:    使用账号密码登陆云账号(支持所有云平台)
** 参数：        [in] stInfo:登陆的用户信息
** 返回:      [out] true 成功 false 失败
********************************************************************/
bool acc_CloudAccountLogin(CloudAccountInfo & stInfo);

/*******************************************************************
** 函数描述:    使用Pin登陆云账号
** 参数：        [in] nAccountId: 账号Id
                [in] strPinCode: Pin Code
** 返回:      [out] true 成功 false 失败
********************************************************************/
bool acc_CloudPinLogin(int nAccountId, const yl::string& strPinCode);

/*******************************************************************
** 函数描述:    登出云账号
** 参数：        [in] nAccountId: 账号Id
** 返回:      [out] true 成功 false 失败
********************************************************************/
bool acc_CloudAccountLogout(int nAccountId);

/*******************************************************************
** 函数描述:    登出云账号
** 参数：        [in] nAccountId: 账号Id
** 返回:      [out] true 成功 false 失败
********************************************************************/
bool acc_CancelCloudLogin(int nAccountId);

/*******************************************************************
** 函数描述:    删除登陆过的用户名密码信息
** 参数：        [in] strUserName: 云账号用户名
** 返回:      [out] true 成功 false 失败
********************************************************************/
bool acc_RemoveCloudAccountInfo(const yl::string & strUserName,
                                SipAccountType eType = ACCOUNT_YEALINK);

/*******************************************************************
** 函数描述:    获取登陆过的用户名密码信息
** 参数：        [in] eOrderType: REVERSE_ORDER 顺序从新到旧，POSITIVE_ORDER 顺序从旧到新 LEXICOGRAPHIC_ORDER 字典序排序
** 返回:      [out] refData：已经登陆的云账号用户名密码等信息
********************************************************************/
bool acc_GetCloudAccountHistory(VectorInfo & refData, SipAccountType eType = ACCOUNT_YEALINK,
                                OrderType eOrderType = REVERSE_ORDER);

/*******************************************************************
** 函数描述:    获取账号id登陆的信息
** 参数：        [in] refData 传入账号id
** 返回:      [out] refData 返回账号id对应的登录信息
********************************************************************/
bool acc_GetCloudAccount(CloudAccountInfo & refData);

/*******************************************************************
** 函数描述:    是否需要进入云账号注册向导
** 参数：        [in]
** 返回:      [out] true：是 false：否
********************************************************************/
bool acc_NeedCloudAccountGuide();

/*******************************************************************
** 函数描述:    是否需要进入云账号注册向导
** 参数：        [in]
** 返回:      [out] true：是 false：否
********************************************************************/
bool acc_IsCloudAccount(int nAccountId);

/*******************************************************************
** 函数描述:    是否第三方云平台
** 参数：        [in]
** 返回:      [out] true：是 false：否
********************************************************************/
bool acc_Is3rdCloudAccount(SipAccountType eType);

/*******************************************************************
** 函数描述:    是否云账号可用
** 参数：        [in]
** 返回:      [out] true：是 false：否
********************************************************************/
bool acc_IsCloudEnable();
#endif //#ifdef IF_FEATURE_CLOUD

/*******************************************************************
** 函数描述:    获取Emergency账号
** 参数：        [in] void
** 返回:      [out] account id
********************************************************************/
int acc_GetEmergencyAccount();

/*******************************************************************
** 函数描述:    获取账号协议:sip/h323
** 参数：        [in] iAccount: Account Id
** 返回:      无
********************************************************************/
AccountProtocolType acc_GetAccountProtocol(int iAccount);

/*******************************************************************
** 函数描述:    账号是否支持H264
** 参数：        [in] iAccount: Account Id
** 返回:      TRUE：支持 FALSE：不支持
********************************************************************/
bool acc_IsSupportH264(int iAccountId);

/*******************************************************************
** 函数描述:        通过配置名和账号ID获取配置
** 参数：            szConfigName-配置名 iAccountID-账号ID
** 返回:          配置信息
********************************************************************/
int acc_GetCfgItemIntValue(const char * szConfigName, int iAccountID);

#ifdef IF_FEATURE_H323
/*******************************************************************
** 函数描述:    获取H323账号
** 参数：        [in] void
** 返回:      [out] account id
********************************************************************/
int acc_GetH323DirectIPAccountID();

/*******************************************************************
** 函数描述:    H323账号id映射GUIH323账号Id
** 参数：        [in] void
** 返回:      [out] account id
********************************************************************/
int acc_MapH323AccountId2Logic(int iAccountId);

/*******************************************************************
** 函数描述:    H323账号状态映射逻辑层账号状态
** 参数：        [in] void
** 返回:      [out] account id
********************************************************************/
LINE_STATE acc_MapH323State2Logic(int iState);
#endif //#ifdef IF_FEATURE_H323

/*******************************************************************
** 函数描述:    账号的注册方式
** 参数:      [in] 账号ID
** 返回:      [out] 账号注册方式 0 UDP，1 TCP，2 TLS
********************************************************************/
int acc_GetAccountTransPort(int nAccountId);

// 获取自定义Label
bool acc_GetCustomLabel(int iAccount, int index, yl::string & strLabel);

// 自定义Label是否开启
bool acc_IsAutoLabelEnable(int iAccount);

/************************************************************************
** 函数描述:   根据ServerID设置账号的服务器
** 参数：      账号ID, ServerID
** 返回:       [out] true：成功 false：失败
************************************************************************/
bool acc_SetServerByServerID(int nAccountId, int iServerID);

#if IF_BUG_39041
/************************************************************************
** 函数描述:   设置账号是否支持SCA功能
** 参数：      账号ID, 支持情况
** 返回:       [out] true：设置成功 false：设置失败
************************************************************************/
bool acc_SetSupportSCA(int nAccountId, bool bSupport);

/************************************************************************
** 函数描述:   获取账号是否支持SCA功能
** 参数：      账号ID
** 返回:       [out] true：支持 false：不支持
************************************************************************/
bool acc_IsSupportSCA(int nAccountId);
#endif

#endif // __MOD_ACCOUNT_H__
