#include "talkaction.h"
#include <etlmsghandler/modetlmsghandler.h>
#include "talk/talklogic/include/talkmsgdefine.h"
#include "edkactionmanager.h"
#include "talk/talklogic/include/modtalklogic.h"

#if IF_FEATURE_EDK
CTalkAction::CTalkAction()
    : CEdkActionBase(EDK_EXCUTETALK)
{
    // 注册消息
    etl_RegisterMsgHandle(TALK_MSG_TALK_ENTER, TALK_MSG_TALK_ENTER, &CTalkAction::OnTalkActionMessage);
    etl_RegisterMsgHandle(TALK_MSG_SESSION_DESTORY, TALK_MSG_SESSION_DESTORY,
                          &CTalkAction::OnTalkActionMessage);
    m_mapSessionId2EdkInfo.clear();
}

CTalkAction::~CTalkAction()
{
    etl_UnregisterMsgHandle(TALK_MSG_TALK_ENTER, TALK_MSG_TALK_ENTER,
                            &CTalkAction::OnTalkActionMessage);
    etl_UnregisterMsgHandle(TALK_MSG_SESSION_DESTORY, TALK_MSG_SESSION_DESTORY,
                            &CTalkAction::OnTalkActionMessage);
}

/*
**处理通话回来的消息，如通话建立，通话结束
*/
LRESULT CTalkAction::OnTalkActionMessage(msgObject & msg)
{
    return _EdkActionManager.OnMessage(EDK_EXCUTETALK, msg) ? 1 : 0;
}

// 处理消息
bool CTalkAction::OnMessage(msgObject & msg)
{
    MAP_ID2EDKINFO_ITER iter = m_mapSessionId2EdkInfo.find(msg.wParam);
    if (iter == m_mapSessionId2EdkInfo.end())
    {
        return false;
    }

    int iSessionId = iter->first;
    EdkTalkInfo & objTmp = iter->second;
    int iActionId = objTmp.m_iActionId;

    if (msg.wParam != iSessionId)
    {
        return false;
    }

    switch (msg.message)
    {
    case TALK_MSG_TALK_ENTER:
        {
            // 等待通话建立
            if (objTmp.m_bWaitTalkEstablish)
            {
                m_mapSessionId2EdkInfo.erase(iter);
                // 通话连接后，继续执行下面的命令
                return _EdkActionManager.DoAction(iActionId);
            }
        }
        break;
    case TALK_MSG_SESSION_DESTORY:
        {
            m_mapSessionId2EdkInfo.erase(iter);
            return _EdkActionManager.CancelAction(iActionId);
        }
        break;
    default:
        break;
    }

    return false;
}


bool CTalkAction::DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult)
{
    bool bContinue = true;
    switch (tEdkAction.m_eActionType)
    {
    case EDK_INVITE:
    case EDK_DIGIT:
    case EDK_INTERCOM:
        {
            // 设置呼出类型
            CallOut_Param pCallParam;
            pCallParam.bIntercom = EDK_INTERCOM == tEdkAction.m_eActionType ? true : false;
            pCallParam.eCallOutType = EDK_INTERCOM == tEdkAction.m_eActionType ?
                                      TCT_NORMAL : TCT_INTERCOM;

            // 选择呼出账号
            int iAccountID = talklogic_SessionExist() ?
                             talklogic_GetSessionAccount() : AUTO_SELECT_LINE_ID;

            // 进行拨号
            if (!talklogic_CallOut(tEdkAction.m_strAction, "", iAccountID, &pCallParam))
            {
                // 拨号失败，取消下面的命令
                _EdkActionManager.CancelAction(iActionId);
                return false;
            }

            // 记住自己创建的通话sessionid
            // InsertSessionID2EdkInfo(tEdkAction, iActionId);
        }
        break;
    // 等待该路通话被建立才继续下面的命令
    case EDK_WC:
        {
            // 如果没有建立通话，不执行该命令
            if (!talklogic_IsConnectingSessionExist()
                    && !talklogic_IsRingSessionExist())
            {
                // 取消所有的命令
                _EdkActionManager.CancelAction(iActionId);
                return false;
            }

            // session绑定失败, 取消命令, 避免内存泄漏
            if (!InsertSessionID2EdkInfo(tEdkAction, iActionId))
            {
                _EdkActionManager.CancelAction(iActionId);
                return false;
            }

            // 标识正在处理wc命令，等待通话建立
            int iSessionId = talklogic_GetFocusedSessionID();
            MAP_ID2EDKINFO_ITER iter = m_mapSessionId2EdkInfo.find(iSessionId);
            if (iter != m_mapSessionId2EdkInfo.end())
            {
                EdkTalkInfo & objTmp = iter->second;
                objTmp.m_bWaitTalkEstablish = true;
            }

            bContinue = false;
        }
        break;
    case EDK_DTMF:
        {
            // 是否存在通话
            if (!talklogic_SessionExist())
            {
                return true;
            }

            EDK_INFO("CTalkAction::DoAction Send Dtmf [%s]", tEdkAction.m_strAction.c_str());
            talklogic_SendDtmf(-1, tEdkAction.m_strAction);
        }
        break;
    case EDK_HOLD:
        {
            // Hold焦点通话session
            talklogic_HoldSession(talklogic_GetCallIdBySessionID(talklogic_GetFocusedSessionID()));
        }
        break;
    case EDK_HANG:
        {
            // 挂断焦点通话
            talklogic_ExitFocusSession();
        }
        break;
    case EDK_REFER:
        {
            // tranfer通话
            talklogic_BlindTransfer(tEdkAction.m_strAction);
        }
        break;
    default:
        break;
    }

    return bContinue;
}

bool CTalkAction::InsertSessionID2EdkInfo(const edk_action_data_t & tEdkAction, int iActionId)
{
    EdkTalkInfo objData;
    objData.m_iActionId = iActionId;

    int iSessionId = -1;
    if ((iSessionId = talklogic_GetFocusedSessionID()) == -1)
    {
        return false;
    }

    MAP_ID2EDKINFO_ITER iter = m_mapSessionId2EdkInfo.find(iSessionId);
    if (iter != m_mapSessionId2EdkInfo.end())
    {
        return false;
    }

    m_mapSessionId2EdkInfo.insert(iSessionId, objData);
    return true;
}
#endif
