#include "talklogic_inc.h"

CAutoRedialRoutine::CAutoRedialRoutine()
{
}

CAutoRedialRoutine::~CAutoRedialRoutine()
{
}

// 创建Routine之后的处理
void CAutoRedialRoutine::AfterRoutineCreate()
{
    if (AUTOREDIAL_IDLE == _AutoRedialManager.GetStatus())
    {
        // 重置,重新读入数据
        _AutoRedialManager.Init();
        // 保存号码和账号
        _AutoRedialManager.SetStatus(AUTOREDIAL_TIP);
    }
}

// Routine的定时器响应
bool CAutoRedialRoutine::OnTimer(UINT uTimerID)
{
    return CBaseRoutine::OnTimer(uTimerID);
}

// 处理ui发送过来的Action事件
bool CAutoRedialRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_CANCEL:
        {
            bHandled = false;
            // 退出时,需重新加载配置
            _AutoRedialManager.StopAutoRedial();
        }
        break;
    case TALK_ACTION_SEND:
    case TALK_ACTION_OK:
        {
            // UI只在呼出时发Ok或者Send消息
            bHandled = true;
            // 如果当前为提示界面,需更改为倒计时界面
            if (AUTOREDIAL_TIP == _AutoRedialManager.GetStatus())
            {
                // 更新状态为倒计时
                _AutoRedialManager.SetStatus(AUTOREDIAL_COUNTDOWN);
            }
            else if (AUTOREDIAL_COUNTDOWN == _AutoRedialManager.GetStatus())
            {
                // 先保存通话记录
                //m_pCallSession->SaveCallLog(GetCallInfo());
                // 重拨次数减一
                _AutoRedialManager.DescreaseTimers();
                // 需要重新获取callid
                m_pCallInfo->SetCallID(modVoip_NewCallId());

                _TalklogicManager.ProcessCallOut(m_pCallSession, m_pCallInfo->m_tRemoteInfo.sSIPName.c_str(), "");
            }
        }
        break;
    default:
        break;
    }

    return bHandled;
}
