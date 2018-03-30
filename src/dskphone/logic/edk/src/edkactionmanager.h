#ifndef __EDK_ACTIOND_STREAM_MANAGER_H__
#define __EDK_ACTIOND_STREAM_MANAGER_H__
#include "edk_def.h"
#include <singletonclassdefine.h>
#include <ETLLib.hpp>

// 向前声明
class CEdkActionBase;

typedef YLHashMap<int, EdkActionInfo> MAP_ID_TO_EDK_ACTION;
typedef MAP_ID_TO_EDK_ACTION::iterator MAP_ID_TO_EDK_ACTION_ITER;

class CEdkActionManager
{
    DECLARE_SINGLETON_CLASS(CEdkActionManager)

public:

    // 初始化工作
    void Init();

    // 执行命令
    bool DoAction(EdkActionInfo & objAction);
    // 执行命令流
    bool DoAction(int iActionId);

    // 取消后面所有的命令 如弹出框被取消后，后面的操作就不执行
    bool CancelAction(int iActionId, bool bClearAll = false);

    // 反馈数据给EDKManager --给完数据，继续执行下面的命令TODO
    bool FeedBackData(int iActionId, yl::string & strPrompt);

    // 传递消息
    LRESULT OnMessage(EDKEXCUTETYPE eType, msgObject & msg);

    // 获取当前执行的Action list
    //bool GetActionList(listEdkAction& listAction) { listAction = m_listAction; }

    // 处理 预处理Action 的反馈结果
    bool DealActionFeedBack(int iActionId, const yl::string & strData);

    // 各个类型特有的解析规则
    bool ParseActionByType(edk_action_data_t & tEdkAction);

    // 获取dsskey的显示数据
    bool GetEdkCtrlDsskeyDispData(int iDsskeyId, EdkCtrlDsskeyDispData & objDisplayData);

    // 设置action定时器
    bool SetTimer(bool bEnable, int iActionId);

    bool OnTimer(UINT uTimer);
private:
    // 获取顶层命令
    bool GetTopAction(EdkActionInfo & objAction, edk_action_data_t & tEdkAction);

    // 命令出栈
    void PopAction(EdkActionInfo & objAction, edk_action_data_t & tEdkAction);

    // 处理预处理命令
    bool DoPreAction(int iActionId);

    // 获取预处理顶层命令
    bool GetTopPreAction(EdkActionInfo & objAction, edk_preaction_data_t & tEdkAction);

    // 预处理命令出栈
    void PopPreAction(EdkActionInfo & objAction, edk_preaction_data_t & tEdkAction);

    // 获取命令执行类
    CEdkActionBase * GetActionExcute(EDKEXCUTETYPE eActionTye);

    // 获取命令执行类型
    EDKEXCUTETYPE GetActionExcuteType(EDKACTIONTYPE eActionType);

    // 释放资源
    void Release();

    // 创建执行类
    CEdkActionBase * CreateEdkActionByType(EDKEXCUTETYPE eType);

    // 调试用
    yl::string MapActionString(EDKACTIONTYPE eActionType);

    // 执行具体的命令
    bool ExecAction(int iActionId, edk_action_data_t & tEdkAction);

    // 替换命令
    void ReplaceCommand(int iActionId, const yl::string & strAction);

    // 根据actionid获取命令结构体
    bool GetActionInfoById(int iActionId, EdkActionInfo & objAction);

private:

    //listEdkAction m_listAction;           // edk宏执行流链表
    MAP_ID_TO_EDK_ACTION m_mapAction;
    YLList<CEdkActionBase *> m_listActionExcute;    // 命令执行类链表
};

#define _EdkActionManager GET_SINGLETON_INSTANCE(CEdkActionManager)

#endif  // __EDK_ACTIOND_STREAM_MANAGER_H__
