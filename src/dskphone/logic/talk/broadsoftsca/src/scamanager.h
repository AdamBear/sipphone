#ifndef _BSSCAManager_H_
#define _BSSCAManager_H_

#ifdef IF_FEATURE_BROADSOFT_SCA
#include "ylhashmap.h"
#include "scastatusmanager.h"

typedef YLHashMap<int, CSCAStatusManager*> AccountSCAMap;

class CSCAManager
{
    DECLARE_SINGLETON_CLASS(CSCAManager)

    struct GetInfoByIndex
        : public CBaseInfoTraver
    {
        virtual bool operator()(CCallInfo* p);

        GetInfoByIndex(int acc, int index)
            : iAccountID(acc), iIndex(index)
        {}

        int iAccountID;
        int iIndex;
    };

    struct GetInfoByStatus
        : public CBaseInfoTraver
    {
        virtual bool operator()(CCallInfo* p);

        GetInfoByStatus(int acc, sip_sca_status_t& t)
            : iAccountID(acc), tStateData(t)
        {}

        int                 iAccountID;
        sip_sca_status_t&   tStateData;
    };

    struct GetInfoByNetConfURI
        : public GetInfoByIndex
    {
        virtual bool operator()(CCallInfo* p);

        GetInfoByNetConfURI(int acc, int index)
            : GetInfoByIndex(acc, index)
        {}
    };

public:
    // 初始化
    void Init();

    // 解析指定账号的SIP状态返回
    BOOL ParseSCANotify(int iAccount, int iIndex, BLA_TYPE eType, void* pStateArray, int iSize);

    // 设置空闲线路
    void SetFreeIndex(int iIndex, int iAccount);
    // 获取指定账号首个空闲线路
    int GetFirstFreeIndex(int iAccount);
    // 获取指定账号响铃线路
    int GetAlertingIndex(int iAccount);
    // 获取指账号状态的线路序号
    int GetFirstIndexByStatus(BLF_STATUS_CODE eState, int iAccount, int iDsskeyIndex);

    // 获取账号的指定线路状态
    BLF_STATUS_CODE GetStatusByIndex(int iIndex, int iAccount);

    // 获取账号线路的呼出信息
    BOOL GetLineIndexCallInfo(int iAccoundID, int iScaIndex, yl::string& strUserName,
                              yl::string& strUri);
    // 获取指定账号的SCA 呼入数据信息
    SCA_Info* GetCallInData(int iAccoundID, int iScaIndex);

    // 设置要显示账号SCA话路状态
    BOOL ShowSCAStatus(int iAccoundID);
    // 刷新状态界面显示
    VOID FlashShowMenu(int iAccoundID);

    // 更新灯状态
    VOID UpdateLightInfo(int iAccoundID);

    // 更新所以账号灯的状态
    void UpdateAllLightInfo();

    // BLA占线
    VOID SeizeLine(CCallInfo* pCallInfo, BOOL bSeize = TRUE);

    int GetBLAIndex(int iAccountID, int& iBLAIndex, int iKeyCounts);

    CSessionProxy GetDsskeySessionByStatus(int iAccount, BLF_STATUS_CODE eStatus);

    // 获取dsskey对应的线路状态
    BLF_STATUS_CODE GetStatusByDsskeyID(int iDsskeyID);

    ListSCAInfo* GetListByAccount(int iAccount);

    int GetCurrentCallNumByAccount(int iAccount, int nDsskeyIndex = 0);

    // 获取状态对应的显示名称
    BLF_STATUS_CODE GetStatus2UIShow(int iAccountId, int iIndex);

    // 获取账号对应的状态管理类
    CSCAStatusManager* GetManagerByAccount(int iAccount);

private:
    // 状态改变的响应
    static LRESULT OnMessage(msgObject& objMsg);
    // 定时器响应
    //static LRESULT OnTimer(msgObject& objMsg);

    // 释放
    VOID ReleaseScaMap();

    //未开启SCA时，设置index信息
    bool ParseSCAIndex(int iAccount, int iUpdatNum, void* pStateArray);
    bool SetCallInfoIndex(int iAccount, sip_sca_status_t* pStateData);

public:

    bool ConfigChange(msgObject& objMsg);
    //通过index设置conf_uri
    bool SetNetConfUribyIndex(int iAccount, int iIndex, const char* ConfUri);
    // 获取CallID
    int GetCallIdBySCAInfo(int iAccount, int iIndex);

private:
    AccountSCAMap    m_mapSCA;          // 账号与SCA通话管理的映射表
    ScaMenuData      m_stScaMenuData;   // SCA状态显示菜单数据，与UI共享
    int              m_iCurrentUpdateNum; //当前更新index总数
};

#define _BSSCAManager GET_SINGLETON_INSTANCE(CSCAManager)

#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
#endif // _BSSCAManager_H_
