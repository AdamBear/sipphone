#include "edkactionmanager.h"
#include "edkparse.h"
#include "talkaction.h"
#include "promptresolveraction.h"
#include "commonaction.h"
#include "entermenuaction.h"
#include "edkkeyaction.h"
#include "timeraction.h"
#include "edkledcontrolaction.h"
#include "edkmanager.h"

#if IF_FEATURE_EDK
IMPLEMENT_GETINSTANCE(CEdkActionManager)

static LRESULT OnEdkActionMesssage(msgObject & msg)
{
    if (msg.message != TM_TIMER)
    {
        return -1;
    }

    if (!_EDKManager.OnTimer(msg.wParam))
    {
        return _EdkActionManager.OnTimer(msg.wParam);
    }

    return true;
}

CEdkActionManager::CEdkActionManager()
{
    Init();
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnEdkActionMesssage);
}

CEdkActionManager::~CEdkActionManager()
{
    Release();
}

/*
**生成执行类链表
*/
void CEdkActionManager::Init()
{
    CEdkActionBase * pAction = NULL;
    EDKEXCUTETYPE arr_ExcuteAction[] = { EDK_EXCUTEPROMPT,
                                         EDK_EXCUTETALK,
                                         EDK_EXCUTETIMER,
                                         EDK_EXCUTECOMMON,
                                         EDK_EXCUTEENTERMENU,
                                         EDK_EXCUTEKEY,
                                         EDK_EXCUTELED
                                       };
    for (int i = 0; i < sizeof(arr_ExcuteAction) / sizeof(EDKEXCUTETYPE); i++)
    {
        pAction = CreateEdkActionByType(arr_ExcuteAction[i]);
        if (pAction != NULL)
        {
            m_listActionExcute.push_back(pAction);
        }
    }
}

// 获取预处理顶层命令
bool CEdkActionManager::GetTopPreAction(EdkActionInfo & objAction,
                                        edk_preaction_data_t & tEdkAction)
{
    if (objAction.m_listPreAction.size() == 0)
    {
        return false;
    }

    tEdkAction = objAction.m_listPreAction.front();
    return true;
}

// 预处理命令出栈
void CEdkActionManager::PopPreAction(EdkActionInfo & objAction, edk_preaction_data_t & tEdkAction)
{
    for (LISTEDKPREACTIONITER iter = objAction.m_listPreAction.begin();
            iter != objAction.m_listPreAction.end(); ++iter)
    {
        objAction.m_listPreAction.erase(iter);
        return;
    }
}

// 处理预处理命令
bool CEdkActionManager::DoPreAction(int iActionId)
{
    MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.find(iActionId);
    if (iter == m_mapAction.end())
    {
        EDK_INFO("-----Warning Action not find -----");
        return false;
    }

    EdkActionInfo & objAction = iter->second;
    if (objAction.m_listPreAction.size() == 0)
    {
        EDK_INFO("Edk Pre Action is finished");
        return true;
    }

    edk_preaction_data_t objPreAction;
    if (GetTopPreAction(objAction, objPreAction))
    {
        ExecAction(objAction.m_iActionId, objPreAction.m_sActionData);
    }
    return false;
}

// 执行命令
bool CEdkActionManager::DoAction(EdkActionInfo & objAction)
{
    MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.find(objAction.m_iActionId);
    if (iter == m_mapAction.end())
    {
        m_mapAction.insert(objAction.m_iActionId, objAction);
        return DoAction(objAction.m_iActionId);
    }
    return false;
}

// 执行具体的命令
bool CEdkActionManager::ExecAction(int iActionId, edk_action_data_t & tEdkAction)
{
    EDK_INFO("CEdkActionManager::DoAction [iActionId:%d][Type:%s][Action:%s]", iActionId,
             MapActionString(tEdkAction.m_eActionType).c_str(), tEdkAction.m_strAction.c_str());

    CEdkActionBase * pEdkActionExcute = NULL;
    EDKEXCUTETYPE eExcuteType;
    pEdkActionExcute = GetActionExcute(GetActionExcuteType(tEdkAction.m_eActionType));

    bool bResult = true;        // 命令执行结果
    // 命令执行类去做action，返回值为是否继续下条命令
    if (pEdkActionExcute != NULL
            && !pEdkActionExcute->DoAction(iActionId, tEdkAction, bResult))
    {
        if (!bResult)
        {
            CancelAction(iActionId);
        }

        return false;
    }
    return true;
}

/*
** 执行命令流
*/
bool CEdkActionManager::DoAction(int iActionId)
{
    MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.find(iActionId);
    if (iter == m_mapAction.end())
    {
        EDK_INFO("Warning Edk Action not find!!!! ");
        return false;
    }

    EdkActionInfo & objAction = iter->second;

    edk_action_data_t tEdkAction;

    // 先执行预处理命令链表,只有预处理做完了，才执行真正命令
    if (!DoPreAction(objAction.m_iActionId))
    {
        EDK_INFO("Edk Pre Action is Executing!");
        return false;
    }

    EDK_INFO("CEdkActionManager::DoAction....m_mapAction size:%d", m_mapAction.size());
    // 获取顶层命令去执行，执行完把该命令弹出栈
    while (GetTopAction(objAction, tEdkAction))
    {
        bool bContinue = ExecAction(objAction.m_iActionId, tEdkAction);

        PopAction(objAction, tEdkAction);

        if (!bContinue)
        {
            return false;
        }
    }
    _EDKDataMgr.ClearTempPromt();

    // 执行完，把命令删除
    CancelAction(iActionId);


    return true;
}

/*
**获取顶层命令
*/
bool CEdkActionManager::GetTopAction(EdkActionInfo & objAction, edk_action_data_t & tEdkAction)
{
    /*EDK_INFO("CEdkActionManager::GetTopAction ActionList [Size = %d]", objAction.m_listAction.size());
    if (objAction.m_listAction.size() == 0)
    {
        return false;
    }

    tEdkAction = objAction.m_listAction.front();
    return true;*/

    // 采用类似数组下标的访问方式来获取链表节点, 链表较长的时候性能相对会差一些
    int iIndex = 1;
    for (LISTEDKACTIONITER iter = objAction.m_listAction.begin();
            iter != objAction.m_listAction.end();
            iter ++)
    {
        if (iIndex == objAction.m_iExcuteActionIndex)
        {
            EDK_INFO("CEdkActionManager::GetTopAction ActionList [index = %d]", iIndex);
            tEdkAction = *iter;
            objAction.m_iExcuteActionIndex ++;
            return true;
        }

        iIndex ++;
    }

    return false;
}

/*
**命令出栈
*/
void CEdkActionManager::PopAction(EdkActionInfo & objAction, edk_action_data_t & tEdkAction)
{
    // 这里释放链表节点容易产生内存碎片,因此不释放,等待一整个链表执行完,整块内存释放
    // http://10.2.1.199/Bug.php?BugID=89046
    EDK_INFO("CEdkActionManager::PopAction. NULL");
    return ;

    /*if (objAction.m_listAction.size() == 0)
    {
        return;
    }

    LISTEDKACTIONITER iter = objAction.m_listAction.begin();

    if (tEdkAction == *iter)
    {
        objAction.m_listAction.erase(iter);
        EDK_INFO("PopAction [%s]", MapActionString(tEdkAction.m_eActionType).c_str());
    }*/
}

// 获取命令执行类型
EDKEXCUTETYPE CEdkActionManager::GetActionExcuteType(EDKACTIONTYPE eActionType)
{
    EDKEXCUTETYPE eType = EDK_EXCUTENON;
    switch (eActionType)
    {
    case EDK_POPUP:
        {
            eType = EDK_EXCUTEPROMPT;
        }
        break;
    case EDK_PAUSE:
        {
            eType = EDK_EXCUTETIMER;
        }
        break;
    case EDK_URL:
    case EDK_LABEL:
        {
            eType = EDK_EXCUTECOMMON;
        }
        break;
    case EDK_INVITE:
    case EDK_REFER:
    case EDK_DTMF:
    case EDK_WC:
    case EDK_HANG:
    case EDK_HOLD:
    case EDK_DIGIT:
    case EDK_INTERCOM:
        {
            eType = EDK_EXCUTETALK;
        }
        break;
    case EDK_FUNCTION:
        {
            eType = EDK_EXCUTECOMMON;
        }
        break;
    case EDK_ENTER_MENU:
        {
            eType = EDK_EXCUTEENTERMENU;
        }
        break;
    case EDK_HARDKEY:
    case EDK_SOFTKEY:
        {
            eType = EDK_EXCUTEKEY;
        }
        break;
    case EDK_LED_CONTROL:
        {
            eType = EDK_EXCUTELED;
        }
        break;
    default:
        break;
    }

    return eType;
}
/*
**获取命令执行类
*/
CEdkActionBase * CEdkActionManager::GetActionExcute(EDKEXCUTETYPE eActionType)
{
    CEdkActionBase * pActionExcute = NULL;
    for (YLList<CEdkActionBase *>::ListIterator iter = m_listActionExcute.begin();
            iter != m_listActionExcute.end(); iter++)
    {
        pActionExcute = *iter;
        if (pActionExcute == NULL)
        {
            continue;
        }

        if (pActionExcute->GetActionType() == eActionType)
        {
            return pActionExcute;
        }
    }

    return NULL;
}

/*
**释放资源
*/
void CEdkActionManager::Release()
{
    CEdkActionBase * pAction = NULL;
    for (YLList<CEdkActionBase *>::ListIterator iter = m_listActionExcute.begin();
            iter != m_listActionExcute.end(); iter++)
    {
        pAction = *iter;
        if (pAction != NULL)
        {
            delete pAction;
            pAction = NULL;
        }
    }
}

/*
**创建执行类
*/
CEdkActionBase * CEdkActionManager::CreateEdkActionByType(EDKEXCUTETYPE eType)
{
    CEdkActionBase * pAction = NULL;
    switch (eType)
    {
    case EDK_EXCUTEPROMPT:
        pAction = new CPromptResolverAction;
        break;
    case EDK_EXCUTETALK:
        pAction = new CTalkAction;
        break;
    case EDK_EXCUTETIMER:
        pAction = new CTimerAction;
        break;
    case EDK_EXCUTECOMMON:
        pAction = new CCommonAction;
        break;
    case EDK_EXCUTEENTERMENU:
        pAction = new CEnterMenuAction ;
        break;
    case EDK_EXCUTEKEY:
        pAction = new CKeyAction;
        break;
    case EDK_EXCUTELED:
        pAction = new CEdkLedControlAction;
        break;
    default:
        break;
    }

    return pAction;
}

/*
**取消后面所有的命令
*/
bool CEdkActionManager::CancelAction(int iActionId, bool bClearAll/* = false*/)
{
    EDK_INFO("CEdkActionManager::CancelAction [iActionId:%d] [Clear All:%d]", iActionId, bClearAll);

    if (!bClearAll)
    {
        // 清除命令链表
        MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.find(iActionId);
        if (iter != m_mapAction.end())
        {
            m_mapAction.erase(iter);
        }
    }
    else
    {
        m_mapAction.clear();
    }
    _EDKDataMgr.ClearTempPromt();

    return true;
}

bool CEdkActionManager::FeedBackData(int iActionId, yl::string & strPrompt)
{
    // TODO，是否需要判断结果？？
    EDK_INFO("DealActionFeedBack [iActionId:%d] [strData:%s]", iActionId, strPrompt.c_str());
    ReplaceCommand(iActionId, strPrompt);
    return DoAction(iActionId);
}

yl::string CEdkActionManager::MapActionString(EDKACTIONTYPE eActionType)
{
    yl::string strAction;
    switch (eActionType)
    {
    case EDK_DTMF:
        strAction = "EDK_DTMF";
        break;
    case EDK_HANG:
        strAction = "EDK_HANG";
        break;
    case EDK_HOLD:
        strAction = "EDK_HOLD";
        break;
    case EDK_INVITE:
        strAction = "EDK_INVITE";
        break;
    case EDK_REFER:
        strAction = "EDK_REFER";
        break;
    case EDK_WC:
        strAction = "EDK_WC";
        break;
    case EDK_PAUSE:
        strAction = "EDK_PAUSE";
        break;
    case EDK_URL:
        strAction = "EDK_URL";
        break;
    case EDK_POPUP:
        strAction = "EDK_POPUP";
        break;
    case EDK_FUNCTION:
        strAction = "EDK_FUNCTION";
        break;
    case EDK_DIGIT:
        strAction = "EDK_DIGIT";
        break;
    case EDK_SOFTKEY:
        strAction = "EDK_SOFTKEY";
        break;
    case EDK_HARDKEY:
        strAction = "EDK_HARDKEY";
        break;
    case EDK_ENTER_MENU:
        strAction = "EDK_ENTER_MENU";
        break;
    case EDK_LABEL:
        strAction = "EDK_LABEL";
        break;
    case EDK_LED_CONTROL:
        strAction = "EDK_LED_CONTROL";
        break;
    default:
        break;
    }

    return strAction;
}

// 传递消息
LRESULT CEdkActionManager::OnMessage(EDKEXCUTETYPE eType, msgObject & msg)
{
    CEdkActionBase * pActionBase = GetActionExcute(eType);
    if (NULL == pActionBase)
    {
        return false;
    }

    return pActionBase->OnMessage(msg);
}

// 替换命令
void CEdkActionManager::ReplaceCommand(int iActionId, const yl::string & strAction)
{
    MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.find(iActionId);
    if (iter == m_mapAction.end()
            || strAction.length() <= 0)
    {
        return;
    }

    EdkActionInfo & objAction = iter->second;

    edk_preaction_data_t sPreActionData;
    // 取预处理命令栈顶的命令
    if (!GetTopPreAction(objAction, sPreActionData))
    {
        return;
    }

    int iIndex = 0;
    edk_action_data_t edkAction;

    // 遍历命令列表，替换执行索引的命令
    for (LISTEDKACTIONITER iter = objAction.m_listAction.begin();
            iter != objAction.m_listAction.end(); ++iter)
    {
        edk_action_data_t & edkAction = *iter;

        // 替换制定命令中的数据
        if (iIndex == sPreActionData.m_iIndex)
        {
            edkAction.m_strAction = edkAction.m_strAction + strAction;
            break;
        }

        iIndex++;
    }

    // 预处理反馈回来后，把该预处理命令弹出栈
    PopPreAction(objAction, sPreActionData);
}

// 根据actionid获取命令结构体
bool CEdkActionManager::GetActionInfoById(int iActionId, EdkActionInfo & objAction)
{
    MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.find(iActionId);
    if (iter != m_mapAction.end())
    {
        objAction = iter->second;
        return true;
    }
    return false;
}

// 处理 预处理Action 的反馈结果,然后继续执行接下去的命令
bool CEdkActionManager::DealActionFeedBack(int iActionId, const yl::string & strData)
{
    ReplaceCommand(iActionId, strData);
    return DoAction(iActionId);
}

// 各个类型特有的解析规则
bool CEdkActionManager::ParseActionByType(edk_action_data_t & tEdkAction)
{
    CEdkActionBase * pAction = GetActionExcute(GetActionExcuteType(tEdkAction.m_eActionType));
    if (NULL != pAction)
    {
        return pAction->ParseAction(tEdkAction);
    }
    return false;
}

// 获取dsskey的显示数据
bool CEdkActionManager::GetEdkCtrlDsskeyDispData(int iDsskeyId,
        EdkCtrlDsskeyDispData & objDisplayData)
{
    CEdkLedControlAction * pLedAction = dynamic_cast<CEdkLedControlAction *>(GetActionExcute(
                                            EDK_EXCUTELED));
    if (NULL != pLedAction)
    {
        return pLedAction->GetDsskeyDisDataById(iDsskeyId, objDisplayData);
    }
    return false;
}

// 设置action定时器
bool CEdkActionManager::SetTimer(bool bEnable, int iActionId)
{
    MAP_ID_TO_EDK_ACTION_ITER itr = m_mapAction.find(iActionId);
    if (itr != m_mapAction.end())
    {
        if (bEnable)
        {
            timerSetThreadTimer((UINT) & (itr->first), 100);
        }
        else
        {
            timerKillThreadTimer((UINT) & (itr->first));
        }
        return true;
    }
    return false;
}

bool CEdkActionManager::OnTimer(UINT uTimer)
{
    for (MAP_ID_TO_EDK_ACTION_ITER iter = m_mapAction.begin();
            iter != m_mapAction.end(); ++iter)
    {
        if (uTimer == (UINT) & (iter->first))
        {
            EDK_INFO("EDK Action Timer Back ,Continue to Exc Action [%d]", iter->first);
            SetTimer(false, iter->first);
            DoAction(iter->first);
            return true;
        }
    }
    return false;
}

#endif
