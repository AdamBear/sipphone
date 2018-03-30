#ifndef __ACD_MODEL_MTSW_H__
#define __ACD_MODEL_MTSW_H__

//#ifdef 1 //IF_FEATURE_ACD
#if IF_FEATURE_METASWITCH_ACD

#include "acdmodelbase.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"
#include "metaswitch/mtswcontrol/include/mtswactioncmdbase.h"

class CACDModelMTSW : public CACDModelBase
{
public:
    CACDModelMTSW(void);
    ~CACDModelMTSW(void);

    // 登录ACD
    virtual bool ReqLogin(acd_accout * pAcd = NULL);

    // 请求状态改变，包括登陆
    virtual bool ReqChangeACDStatus(ACD_ACTION eACDAction);

    void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType);

    //strFilePath是保存服务器返回的数据的文件路径。在此需要解析数据，通知上一层进行处理
    bool OnMTSWActionResponse(cJSON * pOjbParse, int nType);

    // 下载数据
    bool DownloadData(MtswActionType eDownloadType, yl::string strMlhgName = "");

    // 获取MLHG列表
    bool GetMLHGList(YLVector<SMLHGData> & vecData);

    // 获取MLHG成员列表
    bool GetMLHGMemberList(YLVector<SMLHGMemberData> & vecData, yl::string strMlhgName);

    // 获取UnvailReason列表
    bool GetUnavailReasonList(YLList<yl::string> & listUnavailReason);

    // 获取Disposition列表
    bool GetDispositionCodeList(YLVector<yl::string> & vecData, yl::string strMLHGName);

    // 获取队列数据
    const SMLGHQueuesData * GetQueuesData(yl::string strMlhgName);

    bool ParseAllData();

    virtual void SetAccountId(int iAccountId);

    // 是否有 Premium ACD 类型的acd
    bool BeHasPreminumAcd();

    // 设置当前焦点组
    bool SetCurrentMLHG(yl::string strMLHGName);

    // 获取当前焦点组
    yl::string  GetCurrentMLHG();

    // 是否允许登录多个组
    bool IsAllowMultipleLogin();

    //
    virtual void GetCurReason(yl::string & strReasonName);
    //设置Unavailable Reason
    virtual void SetCurReason(const yl::string & strReason);

    // 从Wrap-Up设置到其它状态
    bool SetDispCode(yl::string strCode, yl::string strMlhgName);

    // 设置刷新状态的时间间隔
    bool SetTimer();

    // 处理计时器到时消息.
    BOOL OnTimeoutMessage(msgObject & refObj);

    // 重写获取ACD当前状态
    virtual ACD_STATUS GetAcdStatus();

    // 当前组是否强制需要dispcode
    // strMlhgName默认值为取当时组
    bool IsDispCodeRequired(yl::string strMlhgName);

    // 判断是否所有的mlhg组都登出
    bool IsAllMLHGLogout();

    // 获取最后一次来电所属的组
    yl::string GetLastAnseredMHLG();

private:
    bool ParseMultipleLoginsAllowed(cJSON * pObj);
    bool ParseMLHGGroup(cJSON * pObj);
    bool ParseMLHGList(cJSON * pObj, int nType);
    // 解析mlhg组类型
    bool ParseMLHGServiceLevel(cJSON * pObj);
    // 解析unvailreason列表
    bool ParseUnavailReasonList(cJSON * pObj);
    // 解析acd状态
    bool ParseACDStatus(cJSON * pObj);
    // 解析数据
    bool ParseData(cJSON * pObj, int nType);
    // 解析列表成员
    bool ParseMembers(cJSON * pObj, yl::string strMlhgName);
    // 解析Disposition code成员
    bool ParseDispCode(cJSON * pObj, yl::string strMlhgName);
    // 解析Queue
    bool ParseQueueDetail(cJSON * pObj);
    yl::string GetQueueStatisticValue(cJSON * pObj);

    //http://10.2.1.199/Bug.php?BugID=116117
    //本地做兼容，获取一个类型数据时候会存在返回其他数据在里面情况
    bool IsCurrentDataType(yl::string & strDataType, int & nType);

private:
    yl::string          m_strDirectoryNumber;
    bool                m_bMultipleLoginsAllowed; // 是否允许同时登录多个组
    int                 m_nRefreshInterval; // acd状态刷新时间
    yl::string          m_strTempUnavailReason; // 临时保存的unavailability reason
    yl::string          m_strCurUnavailReason; // 当前所属的unavailability reason
    bool                m_bIsDispCodesRequired; // 是否需要使用disposition code
    yl::string          m_strCurMLHGName; //当前界面所选的焦点组
    int                 m_nDispCodeIndex; // 当前使用的dispostion code序号
    yl::string          m_strLastAnsweredMHLG; // 最后一次应答的组

    YLVector<SMLHGData> m_vecMLHG; // MLHG组列表
    YLHashMap<yl::string, YLVector<SMLHGMemberData> > m_mapMemberList; // MLHG组成员列表
    YLHashMap<yl::string, yl::string> m_mapUnavailabilityReason; // acd 的 Unavailability Reason列表
    YLHashMap<yl::string, YLHashMap<yl::string, yl::string> >
    m_mapDispCodesList; // Disposition codes列表
    YLHashMap<yl::string, SMLGHQueuesData> m_mapQueuesList; // Queues数据
    YLHashMap<yl::string, int>  m_mapSequenceNumber;
    YLHashMap<yl::string, bool> m_mapDispCodesRequired; // 是否需要disposition code

    ACD_ACTION          m_eChangeDelayAcdAction;
    mkThreadTimer       m_objTimer;  // 计时器.
};

#endif

#endif  // IF_FEATURE_BROADSOFT_CALL_CENTER

//#endif    // __ACD_MODEL_MTSW_H__
