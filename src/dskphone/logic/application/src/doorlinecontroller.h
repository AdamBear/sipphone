#pragma once

#include "config.h"
#include <ylstring.h>
#include "singletonclassdefine.h"
#include "taskaction/taskactioncallbackbase.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/networkfileaction.h"
#include "taskaction/common.h"

#ifdef IF_FEATURE_ENTRANCE_GUARD

#if 1
#include <map>
#else
#include <ylhashmap.h>
//#warning ylhashmap lack the necessary interfaces
#error door line should used in the color phone
#endif

using NS_TA::CTaskAction;
using NS_TA::CTaskActionCallbackBase;


enum EDoorLineAction
{
    EDLA_NONE,
    EDLA_CREATE,
    EDLA_DELETE,
    EDLA_START,
    EDLA_PAUSE,
};

class CEntranceGuardLogic
    :  public CTaskActionCallbackBase
{
public:
    struct EntranceGuardInfo
    {
        // 是否正处于下载状态
        bool            bInLoading;
        // 当前是否处于激活状态
        bool            bActived;
        // 失败之后的重试次数
        int             iTryCount;
        // 是否自动接起自动发送DTMF，因客户服务器不支持sip-info，需要我们做兼容，支持在来电时一键开门（即，接起+自动发DTMF+自动挂断）
        bool            bAutoFlow;

        // DTMF、Url、Local file
        yl::string      strDTMF;
        yl::string      strUrl;
        yl::string      strLocalFile;

        EntranceGuardInfo() : bInLoading(false), bActived(false), iTryCount(0), bAutoFlow(false) {}
        EntranceGuardInfo(int iTraceId, const yl::string& strDoorlineDtmf,
                          const yl::string& strstrDoorlineUrl);

        // 连续三次失败才认为真的失败了
        bool Failed()
        {
            return iTryCount >= 3;
        }

        ~EntranceGuardInfo();
    };

    typedef EntranceGuardInfo                 item_value_type;
#if 1
    typedef std::map<int, item_value_type>    mapSessionInfo;
    typedef mapSessionInfo::value_type        mapValueType;
#else
    typedef YLHashMap<int, item_value_type>   mapSessionInfo;
    typedef mapSessionInfo::TAssoc            mapValueType;
#endif
    typedef mapSessionInfo::iterator          mapSessionInfoIt;

    DECLARE_SINGLETON_CLASS(CEntranceGuardLogic)

public:
    // 创建任务
    bool CreateTask(int iTaskId, const yl::string& strDtmf, const yl::string& strUrl);

    // 获取Dtmf
    yl::string GetDTMFString(int iTaskId);

    // 获取焦点Task
    int GetFocusTaskId()
    {
        return m_iFocusId;
    }

    // 设置自动应答
    bool SetAutoAnswer(int iTaskId, bool bAuto = false);

    // 是否自动应答
    bool IsAutoAnswer(int iTaskId);

    //定义对应的消息处理函数接口
    bool DealSessionDestory(int iTaskId);

    bool DealTalkHide(int iTaskId, bool bHide);

    bool DealRingingEnter(int iTaskId);

    bool DealFocusChanged(int iTaskId, bool bFocusIn);

    bool DealDtmfFinshed(int iTaskId);

    bool DealTalkEnter(int iTaskId);

public:

    // 是否开启Doorline功能
    static bool IsEnable();

    // Msg注册函数
    static LRESULT OnMessage(msgObject& refObj);

    // 读配置，当前没有提供读double类型的函数
    static double GetConfigIntervar();

private:

    // 下载回调
    virtual void OnTaskActionCallback(CTaskAction* pTaskAction);

    // Msg分发处理函数
    bool OnMessgeProcess(msgObject& refObj);

    // 外部通话事件响应函数
    bool OnAction(EDoorLineAction eAction, int iTaskId);

    // 定时器响应
    bool OnTimer(UINT uTimerId);

    // 配置改变响应
    void OnConfigChange();

    // 下载函数
    bool ExeLoad(const mapValueType& mapItem);

    // 开始任务
    bool StartById(int iTaskId);

    // 结束任务
    bool StopById(int iTaskId, bool bRemoveTask = false);

    // 任务是否存在
    bool IsTaskIdValid(int iTaskId);

    // 通知失败
    bool NotifyFailMsg(int iTaskId, NS_TA::TaskActionErrorType eErrorType);

private:
    mapSessionInfo       m_mapInfo;          // Task缓存
    int                  m_iFocusId;         // 焦点Task
    int                  m_iRefreshInterval; // 刷新间隔
};

#define g_refEntranceGuard GET_SINGLETON_INSTANCE(CEntranceGuardLogic)

#endif //IF_FEATURE_ENTRANCE_GUARD
