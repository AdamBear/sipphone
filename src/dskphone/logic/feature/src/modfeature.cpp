#include "feature_inc.h"

/*******************************************************************
** 函数描述:     feature的sip消息处理
********************************************************************/
LRESULT FeatureProcessSIPMsg(msgObject & objMessage)
{
    return _FeatureCode.ProcessSIPMsg(objMessage);
}

// feature code的定时器处理
LRESULT FeatureOnTimer(msgObject & objMessage)
{
    return _FeatureCode.OnTimer(objMessage.wParam);
}

// feature code的DND状态变化消息处理
LRESULT FeatureProcessDNDMsg(msgObject & objMessage)
{
    int iAccount = objMessage.wParam;
    DND_CONFIG_SOURCE_TYPE eSrcType = (DND_CONFIG_SOURCE_TYPE)objMessage.lParam;
    bool * pEnable = (bool *)objMessage.GetExtraData();
    if (pEnable)
    {
        if (eSrcType != DND_CFG_SRC_TYPE_STARTUP && eSrcType != DND_CFG_SRC_TYPE_SHUTDOWN)
        {
            _FeatureCode.ProcessDNDMsg(iAccount, *pEnable);
        }
        if (eSrcType != DND_CFG_SRC_TYPE_SYNC)
        {
            _FeatureSyncMng.ProcessDNDMsg(iAccount);
        }
    }
    return TRUE;
}

// feature code的FWD状态变化消息处理
LRESULT FeatureProcessFWDMsg(msgObject & objMessage)
{
    int iAccount = objMessage.wParam;
    FWD_CONFIG_SOURCE_TYPE eSrcType = (FWD_CONFIG_SOURCE_TYPE)objMessage.lParam;
    FwdMessageStatusData * pData = (FwdMessageStatusData *)objMessage.GetExtraData();
    if (pData)
    {
        COMMONUNITS_INFO("Got forward status changed notify: "
                         "account[%d] src[%d] type[%d], enable[%d], target[%s], ringDelay[%d]",
                         iAccount, eSrcType, pData->eType, pData->bEnabled, pData->szTarget,
                         pData->iRingDelay);
        if (eSrcType != FWD_CFG_SRC_TYPE_STARTUP && eSrcType != FWD_CFG_SRC_TYPE_SHUTDOWN)
        {
            _FeatureCode.ProcessFWDMsg(iAccount, pData->eType, pData->bEnabled, pData->szTarget);
        }
        if (eSrcType != FWD_CFG_SRC_TYPE_SYNC)
        {
            _FeatureSyncMng.ProcessFWDMsg(iAccount, pData->eType);
        }
    }
    return TRUE;
}

LRESULT FeatureProcessAccountMsg(msgObject & objMessage)
{
    return _FeatureSyncMng.ProcessAccountMsg(objMessage);
}

// 初始化
void feature_Init()
{
    RangeMsgReg(SIP_CALL_START, SIP_CALL_INFORMATION_UPDATE, FeatureProcessSIPMsg);
    RangeMsgReg(SIP_CALL_TRANSFERED_RESULT, SIP_CALL_SEND_INFO_RESULT, FeatureProcessSIPMsg);
    SingleMsgReg(DND_MESSAGE_STATUS_CHANGED, FeatureProcessDNDMsg);
    SingleMsgReg(FWD_MESSAGE_STATUS_CHANGED, FeatureProcessFWDMsg);
    SingleMsgReg(ACCOUNT_STATUS_CHANGED, FeatureProcessAccountMsg);
    SingleMsgReg(TM_TIMER, FeatureOnTimer);

    // 初始化同步管理模块
    _FeatureSyncMng.InitFeatureSync();

#if IF_FEATURE_PAGING
    _MultiPagingList.Init();
#endif
    _UserModeManager.Init();
    _RTPConfigManager.Init();
}

// 发送开启特征码
BOOL feature_SendOnCode(int iLineId, CCodeInfo & refCode)
{
    return _FeatureCode.SendOnCode(iLineId, refCode);
}

// 发送关闭特征码
BOOL feature_SendOffCode(int iLineId, CCodeInfo & refCode)
{
    return _FeatureCode.SendOffCode(iLineId, refCode);
}

// 跳过上电后的首次Code发送. 首次账号注册上时要避免发送
// http://192.168.1.99/Bug.php?BugID=22736
// | iType | [in] 某Code类型, 定义在CodeSendFlag里
// | iLineId | [in] 账号iD，-1为所有账号
bool feature_IsSkipCodeSend(int iType, int iLineId)
{
    return _FeatureCode.IsSkipCodeSend(iType, iLineId);
}

bool feature_IsDNDSync(int iAccountID/* = -1*/)
{
    return _FeatureSyncMng.IsDNDSync(iAccountID);
}

bool feature_IsForwardSync(int iAccountID/* = -1*/)
{
    return _FeatureSyncMng.IsForwardSync(iAccountID);
}

void feature_SetFeatureKeySync(bool bEnable, int iAccountID/* = -1*/)
{
    _FeatureSyncMng.SetFeatureKeySync(bEnable, iAccountID);
}

void feature_SetDNDSync(bool bEnable, int iAccountID/* = -1*/)
{
    _FeatureSyncMng.SetDNDSync(bEnable, iAccountID);
}

void feature_SetForwardSync(bool bEnable, int iAccountID/* = -1*/)
{
    _FeatureSyncMng.SetForwardSync(bEnable, iAccountID);
}

bool feature_IsDNDLocalProcess(int iAccount)
{
    return _FeatureSyncMng.IsDNDLocalProcess(iAccount);
}

bool feature_IsForwardLocalProcess(int iAccount)
{
    return _FeatureSyncMng.IsForwardLocalProcess(iAccount);
}


#if IF_FEATURE_PAGING
//////////////////////////////////////////////////////////////////////////////////////////////
// 获取Paging List
bool feature_MutliPaingGetList(PagingListVec & pagingList)
{
    return _MultiPagingList.GetPagingList(pagingList);
}

// 根据配置项项数获取相应数据
const PagingGroupInfo * feature_MutliPaingGetGroupInfo(int iConfigId)
{
    return _MultiPagingList.GetPagingByConfigId(iConfigId);
}

// 修改某个Paging数据
bool feature_MutliPaingSaveGroupInfo(const PagingGroupInfo & groupInfo)
{
    return _MultiPagingList.SaveGroupInfo(groupInfo);
}

// 删除某个Paging数据
bool feature_MutliPaingDeleteGroupInfo(int iConfigId)
{
    return _MultiPagingList.DeleteGroup(iConfigId);
}

// 删除所有配置
bool feature_MutliPaingDeleteAll()
{
    return _MultiPagingList.DeleteAllGroup();
}

// 注册/注销回调函数，主要用于通知UI刷新
void feature_MutliPaingCallBack(PagingListCallBack pCallBack, bool bRegister/* = true*/)
{
    _MultiPagingList.InitCallBack(pCallBack, bRegister);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// 是否启用三级权限
bool feature_UserModeIsOpen()
{
    return _UserModeManager.IsEnable();
}

// 获取当前用户使用的权限
USER_TYPE feature_UserModeGetCurrType(int iUIID/* = DEF_UIIID*/)
{
    return _UserModeManager.GetUserType(iUIID);
}

USER_TYPE feature_UserModeGetTypeByName(const yl::string & strName)
{
    for (int i = UT_USER; i < UT_NONE; ++i)
    {
        if (strName == feature_UserModeGetUserName((USER_TYPE)i))
        {
            return (USER_TYPE)i;
        }
    }

    return UT_NONE;
}

// 改变话机的使用权限
void feature_UserModeSetCurrType(USER_TYPE eType, int iUIID /*= DEF_UIIID*/)
{
    if (eType >= UT_USER && eType <= UT_ADMIN)
    {
        _UserModeManager.SetUserType(eType, iUIID);
    }
}

// 根据权限获取显示名，亦可用于密码匹配
yl::string feature_UserModeGetUserName(USER_TYPE eType)
{
    yl::string strUserName = "";
    _UserModeManager.GetUserNameByUserMode(eType, strUserName);

    return strUserName;
}

// 获取默认的显示名
yl::string feature_UserModeGetDefaultUserName(USER_TYPE eType)
{
    yl::string strUserName = "";
    _UserModeManager.GetDefaultUserName(eType, strUserName);

    return strUserName;
}

// 判断配置项是否有显示权限
// | [in] strItemAction| 黑白屏：strID  彩屏：ClickAction
bool feature_UserModeConfigItemShow(const yl::string & strItemAction, int iUIID/* = DEF_UIIID*/)
{
    return _UserModeManager.IsItemHasShowAuth(strItemAction, iUIID);
}

// 判断配置界面是否只读
// | [in] strItemAction| 配置界面的Action
bool feature_UserModeConfigItemReadOnly(const yl::string & strItemAction, int iUIID/* = DEF_UIIID*/)
{
    return _UserModeManager.IsItemReadOnly(strItemAction, iUIID);
}

//#ifdef _DECT
/*******************************************************************
** 函数名:     feature_UserModeGetData
** 函数描述:   三级权限数据
**             [out] refData:               三级权限列表
**             [in] iUIID:              手柄ID
** 返回:
*******************************************************************/
bool feature_UserModeGetData(UserModeListData & refData, int iUIID/* = DEF_UIIID*/)
{
    return _UserModeManager.GetUserModeData(refData, iUIID);
}

/*******************************************************************
** 函数名:     feature_UserModeGetCnt
** 函数描述:   三级权限数量
**             [in] iUIID:              手柄ID
** 返回:
*******************************************************************/
int  feature_UserModeGetCnt(int iUIID/* = DEF_UIIID*/)
{
    return _UserModeManager.GetUserModeCnt(iUIID);
}
// #endif

// 获取显示的RTP配置项列表
const RTPConfigureList * feature_RTPConfigGetEnableList()
{
    return _RTPConfigManager.GetRTPOptionEnabledList();
}

// bool feature_IsBroadsoftDndSyncEnable()
// {
//  yl::string strDndEnable = configParser_GetConfigString(kszDNDFeatureKeySync);
//  if (!strDndEnable.empty())
//  {
//      return strDndEnable == "1";
//  }
//
//  return configParser_GetConfigInt(kszBroadSoftFeatureSync) == 1;
// }
