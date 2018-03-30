#include "baseexpmsgprocess.h"

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif

CBaseExpMsgProcess::CBaseExpMsgProcess()
{
    m_MsgProcessType = DT_INVALID;
    m_uInsertDelayTimeId = 0;
    m_uKeyLongPressTimerId = 0;
    m_InsertId = -1;
    m_DssId = -1;
    m_bTestMode = false;
    m_bKeyReleaseAfterLongPress = false;
    m_iExpCounts = 0;
    m_bIsInsertDelayTimerRunning = false;
    m_bIsLongPressTimerRunning = false;

    for (int i = 0; i < MAX_EXP_NUM; i++)
    {
        m_nCurrentPage[i] = 0;
    }

}

CBaseExpMsgProcess::~CBaseExpMsgProcess()
{

}

void CBaseExpMsgProcess::ExpPluginProcess(msgObject & msg)
{

    ExpID expId = msg.wParam;
    DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);
    // 记录一下这次插入的exp的index
    m_InsertId = expId;
    SetExpInsertDelayTimer(expType);
}

void CBaseExpMsgProcess::ExpRemoveProcess(msgObject & msg)
{

    ExpID expId = msg.wParam;
    DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);
    m_nCurrentPage[expId] = 0;
    ExpCountsDecrease();

    //exp数量减少，发送事件通知
    event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);

    PostMsgToApp(DEV_MSG_EXP_REMOVE, expId, expType);
}

void CBaseExpMsgProcess::AutoSetKillLongPressTimer(int iKeyStatus)
{
    if (iKeyStatus == EXP_KEY_STATUS_PRESS)
    {
        SetKeyLongPressTimer();
    }
    else if (iKeyStatus == EXP_KEY_STATUS_RELEASE)
    {
        KillKeyLongPressTimer();
    }
}

DsskeyID CBaseExpMsgProcess::AddPageInfoForDsskeyID(DsskeyID dssId)
{
    int iPageIndex = GetCurrentExpPage(GetExpIndexFromDssKeyID(dssId));

    dssId += EXP_KEYNUM_PAGE * iPageIndex;

    return dssId;
}

bool CBaseExpMsgProcess::SetExpInsertDelayTimer(DEVICE_TYPE_DEF expType)
{
    if (!m_bIsInsertDelayTimerRunning)
    {
        m_uInsertDelayTimeId = expType;
        m_MsgProcessType = expType;

        m_bIsInsertDelayTimerRunning = true;

        timerSetThreadTimer((UINT)&m_uInsertDelayTimeId, EXP_INSERT_DELAY_RESPONSE_TIME);

        return true;
    }
    return false;
}

void CBaseExpMsgProcess::KillExpInsertDelayTimer()
{
    timerKillThreadTimer((UINT)&m_uInsertDelayTimeId);
    m_bIsInsertDelayTimerRunning = false;
}

bool CBaseExpMsgProcess::SetKeyLongPressTimer()
{
    if (!m_bIsLongPressTimerRunning)
    {
        m_uKeyLongPressTimerId = GetMsgProcessType();

        m_bIsLongPressTimerRunning = true;

        EXP_INFO("EXP long press timer has been setted");

        timerSetThreadTimer((UINT)&m_uKeyLongPressTimerId, EXP_KEY_LONG_PRESS_TIME);

        return true;
    }
    return false;
}

bool CBaseExpMsgProcess::OnKeyLongPressTimer(UINT iTimerId)
{
    EXP_INFO("EXP long press timer timed out,try to response dssId[HEX:%x]",
             AddPageInfoForDsskeyID(m_DssId));

    KillKeyLongPressTimer();
    PostMsgToApp(DEV_MSG_ELP_KEY, 1, AddPageInfoForDsskeyID(m_DssId));
    PostMsgToApp(EXP_MSG_EXP_KEY_EVENT, AddPageInfoForDsskeyID(m_DssId), EXP_KEY_STATUS_RELEASE);
    // 长按定时器到了，清空记录dssID
    // 弹起时发现和记录的dssID 不一致，不会发送按键弹起的消息
    m_DssId = EXP_INVALID_KEY_ID;

    m_bKeyReleaseAfterLongPress = true;
    return true;
}

bool CBaseExpMsgProcess::IsKeyReleaseAfterLongPress(int iKeyStatus)
{
    if (iKeyStatus == EXP_KEY_STATUS_PRESS)
    {
        return false;
    }

    if (iKeyStatus == EXP_KEY_STATUS_RELEASE
            && m_bKeyReleaseAfterLongPress)
    {
        return true;
    }

    return false;
}

bool CBaseExpMsgProcess::OnTimer(msgObject & msg)
{
    UINT iTimerId = (UINT)msg.wParam;

    if (iTimerId == (UINT)&m_uInsertDelayTimeId)
    {
        OnExpInsertDelayTimer(iTimerId);
    }
    else if (iTimerId == (UINT)&m_uKeyLongPressTimerId)
    {
        OnKeyLongPressTimer(iTimerId);
    }
    return true;
}

void CBaseExpMsgProcess::KillKeyLongPressTimer()
{
    timerKillThreadTimer((UINT)&m_uKeyLongPressTimerId);
    m_bIsLongPressTimerRunning = false;
}


bool CBaseExpMsgProcess::OnExpInsertDelayTimer(UINT iTimerId)
{
    KillExpInsertDelayTimer();

    //TODO. 这个处理比较危险
    DEVICE_TYPE_DEF expType = GetExpTypeFromTimerID(iTimerId);

    for (ExpID i = m_iExpCounts; i <= m_InsertId; i++)
    {
        PostMsgToApp(DEV_MSG_EXP_INSERT, i, expType);

        ExpCountsIncrease();
    }

    //exp数量增加，发送事件通知
    event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);

    return true;
}

void CBaseExpMsgProcess::SetCurrentExpPage(ExpID expId, int iPage)
{
	if (expId < 0
		|| expId >= MAX_EXP_NUM)
	{
		EXP_ERR("Set page:ExpIndex error![%d]", expId);
		return;
	}
	m_nCurrentPage[expId] = iPage;
}

int CBaseExpMsgProcess::GetCurrentExpPage(ExpID expId)
{
	if (expId < 0
		|| expId >= MAX_EXP_NUM)
	{
		EXP_ERR("get page:ExpIndex error![%d]", expId);
		return 0;
	}

	return m_nCurrentPage[expId];
}

void CBaseExpMsgProcess::RegisterKeyDown(DsskeyID dssId, int iKeyStaus)
{
	if (iKeyStaus == EXP_KEY_STATUS_PRESS && m_DssId == EXP_INVALID_KEY_ID)
	{
		m_DssId = dssId;
	}
	else if (iKeyStaus == EXP_KEY_STATUS_RELEASE)
	{
		m_DssId = EXP_INVALID_KEY_ID;
	}
}

void CBaseExpMsgProcess::SetTestMode(bool bEnable)
{
	if (bEnable != m_bTestMode)
	{
		m_bTestMode = bEnable;
	}
}

void CBaseExpMsgProcess::ExpCountsDecrease()
{
	if (m_iExpCounts > 0)
	{
		m_iExpCounts--;
	}
}

#endif