#ifndef COMMONUNITS_STATUSMANAGER_H_
#define COMMONUNITS_STATUSMANAGER_H_


#ifdef IF_FEATURE_IDLESCREEN

#include "idlescreen_inc.h"
#include "idlescreen_def.h"
#include "modidlescreen.h"


// 话机状态链表.
typedef YLList<IdleStatusItem> NonFixedStatusList;

struct TopPhoneStateList;

// Class CStatusManager, this class managers the status list. Because the
// status needs be flushed and list is a good way to do that, we use the list.
// ABOUT flush: the flush means when new status to add, if the same status
// is exists in the list, then removes it and pushes back or pushes front new status.
class CStatusManager : private yl::NonCopyable
{
public:
    //通知回调保存结构
    struct StatusNotifyInfo
    {
        StatusNotifyFunction    m_fnNotifyfunc;    //状态的回调函数
        int                     m_iCount;          //状态个数       —— 非xwin使用
        StateNotifyType         m_eNotifyType;     //回调函数的类型  —— xwin使用
        StatusNotifyInfo()
            : m_fnNotifyfunc(NULL),
              m_iCount(0),
              m_eNotifyType(SNT_NONE)
        {}
    };

    // 状态配置项.
    struct StatusConfigItem
    {
        StatusChangeFunction    funcChange; //状态变化回调函数
        // —— xwin使用
        VecNotifyTypePriority   m_vecPriotify;
        // —— 非xwin使用
        int     m_nLoc;         // 在链表中的位置, 从1开始, 如果为0, 则表示不固定顺序.
        int     m_nPopupType;   // 弹窗提示, 为PopupWindowType枚举.

        StatusConfigItem()
        {
            Reset();
        }

        void Reset()
        {
            m_nLoc = 0;
            m_nPopupType = 0;
            funcChange = NULL;
            m_vecPriotify.clear();
        }
    };
    // 状态配置map. Key: 状态id; value: 配置项.
    typedef YLHashMap<int, StatusConfigItem> StatusConfigureMap;

public:
    CStatusManager();
    ~CStatusManager();

    // Initializes the instance, it is a lazy singleton.
    static CStatusManager& GetInstance() DECLARE_ATTR;

    void SetIsXwin(bool bIsXwin)
    {
        m_bIsXwin = bIsXwin;
    }

    bool GetIsXwin()
    {
        return m_bIsXwin;
    }

    static LRESULT OnTimerMsgProgress(msgObject& msg);

    BOOL OnTimer(UINT iTimerId);

    bool IsTimerRuning();

    //设置状态回调
    bool SetStatusChangeCallback(int nStstus, StatusChangeFunction funcCallback);

    //获取配置
    StatusConfigItem* GetConfigItem(int nStatus);

    // 状态管理开关
    void TurnNotify(bool bNotified = true);

    // 增加状态, 增加状态时会将以前同一组的状态清除.
    // | nStatus | 为PhoneState枚举值.
    // | strHint | 为状态的提示语句.
    void AddStatus(int nStatus, const yl::string& strHint = "");

    // 删除状态.
    // | nStatus | 为PhoneState枚举值.|bFlush 是否为刷新操作调用的Flush|
    bool RemoveStatus(int nStatus, bool bFlush = false);

    bool RegisterStatus(int nStatus, const StatusConfigItem& sConfig);

    //通知UI更新
    bool NotifyStatusChange();

    bool UpdateNotifyStatus();

    //修改Hint
    bool ChangeHint(int nStatus, const yl::string& strNewHint,
                    unsigned int uNotifyType = SNT_ICON);

    //获取状态
    IdleStatusItem* GetStatusItem(int nStatus);

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    // 话机状态链表.
    typedef YLVector<IdleStatusItem> FixedStatusVector;  // 固定状态数组.

    void UpdateNotifyState(bool bAdd, int iStatus, bool bForce = false);

    // Get number of iCount Status, return all status when iCount is -1.
    YLList<IdleStatusItem> GetTopStateList(int iCount = -1);

    // 清除弹出标记.
    // | nStatus | 为要清除标记的状态.
    void ClearPopupFlag(int nStatus);

    // 设置弹窗状态.
    // | nStatus | 是要设置的状态.
    // | nPopStatus | 是要设置的弹窗状态, 为PopupWindowType枚举.
    void SetPopupStatus(int nStatus, int nPopupStatus);

    // 获取弹窗状态.
    // | nStatus | 是要查询的状态.
    // 返回状态(为PopupWindowType枚举).
    int GetPopupStatus(int nStatus);

    //获取弹窗状态为POPUP_WINDOW_POPUP的状态列表
    YLVector<int> GetStatusToPopup();

    //注册一种状态
    bool RegisterStatus(int nStatus, PopupWindowType ePopupStatus, int nLoc);

    // 增加固定位置状态
    bool AddFixedStatus(int nStatus, const yl::string & strHint = "");

    //设置通知UI的状态数量
    bool SetNotifyFunByCount(StatusNotifyFunction func, int nCount);

    //设置Popup配置
    bool SetStatusPopupType(int nStatus, PopupWindowType ePopupType);

#endif // IF_NOTIFICATION_CENTER_ON_IDLE

    //根据状态通知类型获取对应类型状态类型所有的状态
    YLList<IdleStatusItem> GetNotifyStateList(StateNotifyType eNotifyType);

    // 清除通知类型
    // | nStatus | 为要清除标记的状态.
    // | iNotifyType | 是要设置的通知类型StateNotifyType枚举或者组合值.
    void ClearNotifyTypeFlag(int nStatus, unsigned int iNotifyType);

    // 清除配置通知类型
    // | nStatus | 为要清除标记的状态.
    // | iNotifyType | 是要设置的通知类型StateNotifyType枚举或者组合值.
    void ClearCfgStatusFlag(int nStatus, unsigned int iNotifyType);

    // 设置通知类型
    // | nStatus | 是要设置的状态.
    // | eNotifyType | 是要设置的通知类型StateNotifyType枚举.
    // |iPriority | 优先级0,1,2,3,....n 数字越大优先级越高
    bool SetNotifyType(int nStatus, StateNotifyType eNotifyType, int iPriority);

    // 获取通知类型
    // | nStatus | 是要查询的状态.
    // 返回状态通知类型StateNotifyType枚举值
    unsigned int  GetNotifyType(int nStatus);

    //注册状态
    bool RegisterStatus(int nStatus, StateNotifyType eNotifyType, int iPriority);

    //设置通知UI的状态回调
    bool SetNotifyFunction(StatusNotifyFunction func, StateNotifyType eNotifyType);

private:
    //从状态statusItem是否包含了eNotifyType 类型
    bool IsNotifyTypeInStatusItem(IdleStatusItem & statusItem, StateNotifyType eNotifyType);

    //从状态statusItem包含了eNotifyType 类型的优先级
    int GetStatusItemPriority(IdleStatusItem & statusItem, StateNotifyType eNotifyType);

    //从状态statusItem清除eNotifyType 类型
    void ClearNotifyTypeFlag(VecNotifyTypePriority & vecNotify, unsigned int iNotifyType);

public:

#if IS_CODE_TEST
    //测试用接口
    void reset();
#endif

#ifdef IF_FEATURE_PSTN
    void GetAddPstnIdList(YLVector<int> &vecID);

    void AddPstnIdToList(int nAccountId);

    void RemovePstnIdFromList(int nAccountId);

    void ClearPstnIdList();
#endif // IF_FEATURE_PSTN

private:
    bool                        m_bIsXwin;
    bool                        m_isOn;             //是否开启了Status管理, 不开启则不弹窗.
    const int                   m_NotifyTimer;      //定时通知定时器
    bool                        m_bTimerStatus;     //定时器是否运行
    int                         m_iNeedNotifyType;  //标识需要更新类型
    StatusConfigureMap          m_mapStatusCfg;     // 状态配置.
    NonFixedStatusList          m_listStatus;       //状态列表.
    YLVector<StatusNotifyInfo>  m_vecNotifyFunc;    //通知回调函数
#ifdef IF_FEATURE_PSTN
    YLVector<int>               m_listAddPstnId;
#endif // IF_FEATURE_PSTN
#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    FixedStatusVector m_vecFixedStatus;  // 固定状态列表.
#endif
};  // Class CStatusManager

#define _StatusManager (CStatusManager::GetInstance())

#endif // IF_FEATURE_IDLESCREEN
#endif  // COMMONUNITS_STATUSMANAGER_H_
