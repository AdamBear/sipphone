#include "exp50msgprocess.h"

#ifdef _EXP_COLOR_SUPPORT
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif

CExp50MsgProcess::CExp50MsgProcess()
{
    m_bPageKeyCheck = false;
    m_nCheckCount = 0;
    m_nLastCode = 0;
    m_uPageKeyAntiShakeTimer = 0;
}

CExp50MsgProcess::~CExp50MsgProcess()
{
}

void CExp50MsgProcess::KeyProcess(int iKeyStatus, msgObject & msg)
{
    DsskeyID dssId;
    DsskeyID dssIdLast;
    ExpID expId;
    int iKeyCode;

    if (EXP_KEY_STATUS_TEST ==  iKeyStatus)
    {
        int nWaram = msg.wParam;
        iKeyCode = nWaram % 100;
        iKeyStatus = (iKeyCode / 30) == 0 ? 1 : 0;
        iKeyCode = iKeyCode % 30;
        expId = nWaram / 100;
        dssId = GetDsskeyIdByData(iKeyCode, expId, DT_EXP_50);
    }
    else
    {
        dssId = msg.wParam;
        expId = GetExpIndexFromDssKeyID(dssId);
        iKeyCode = GetKeyCodeFromDsskeyID(dssId);
    }

    dssIdLast = dssId;

    EXP_INFO("EXP50 KeyProcess: dssId[%d] expId[%d] keycode[%d]", dssId, expId, iKeyCode);
    // T4X测试模式需要获取原始的dsskeyid，开关打开时不转换Page键
    bool bPageKey = false;
    int nPage = 0;

    if (EXP_KEY_STATUS_PRESS == iKeyStatus)//按键按下
    {
        //已经有按键按下，不再响应按键
        if (GetRegDsskeyID() != EXP_KEY_NONE)
        {
            return ;
        }

        if (!IsTestMode())
        {
            if (iKeyCode == EXP_KEY_PAGE1)
            {
                nPage = 0;
                bPageKey = true;
            }
            else if (iKeyCode == EXP_KEY_PAGE2)
            {
                nPage = 1;
                bPageKey = true;
            }
            else if (iKeyCode == EXP_KEY_PAGE3)
            {
                nPage = 2;
                bPageKey = true;
            }
        }

        if (bPageKey)
        {
            PostMsgToApp(DEV_MSG_EXP_PAGE_KEY, 0, 0);
            SetPageKeyTimer();
            if (m_bPageKeyCheck)
            {
                m_nCheckCount++;
                m_nLastCode = dssIdLast;
            }
            else
            {
                if (GetCurrentExpPage(expId) != nPage)
                {
                    EXP_INFO("[Exp-Info] exp50 expIndex[%d] has logically switched to pageIndex[%d] ", expId, nPage);
                    SetCurrentExpPage(expId, nPage);
                    PostMsgToApp(EXP_MESSAGE_SWITCH_PAGE, 0, dssIdLast);
                }
            }

            if (IsTestMode())
            {
                PostMsgToApp(DEV_MSG_EXP_KEYDOWN, dssId, iKeyStatus);
            }
            else
            {
                m_bPageKeyCheck = true;
            }
        }
        else
        {
            RegisterDsskeyID(dssId);
            SetKeyLongPressTimer();
            PostMsgToApp(EXP_MSG_EXP_KEY_EVENT, AddPageInfoForDsskeyID(dssId), iKeyStatus);
            PostMsgToApp(DEV_MSG_EXP_KEYDOWN, AddPageInfoForDsskeyID(dssId), iKeyStatus);

        }
    }
    else if (EXP_KEY_STATUS_RELEASE == iKeyStatus)
    {
        if (iKeyCode == EXP_KEY_PAGE1
                || iKeyCode == EXP_KEY_PAGE2
                || iKeyCode == EXP_KEY_PAGE3)
        {
            if (IsTestMode())
            {
                PostMsgToApp(DEV_MSG_EXP_KEYUP, dssId, iKeyStatus);
            }
        }
        //当抬起按键与按下的相等时才响应
        else if (GetRegDsskeyID() == dssId)
        {
            RegisterDsskeyID(EXP_KEY_NONE);
            KillKeyLongPressTimer();
            PostMsgToApp(EXP_MSG_EXP_KEY_EVENT, AddPageInfoForDsskeyID(dssId), iKeyStatus);
            PostMsgToApp(DEV_MSG_EXP_KEYUP, AddPageInfoForDsskeyID(dssId), iKeyStatus);
        }
    }
    // 长按之后的弹起消息单独发送给MetaSwitch
    if (IsKeyReleaseAfterLongPress(iKeyStatus))
    {
        PostMsgToApp(DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE, AddPageInfoForDsskeyID(dssId), iKeyStatus);
        EXP_INFO("[EXP-INFO]:DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE post to gui");
        m_bKeyReleaseAfterLongPress = false;
    }
    return ;

}

void CExp50MsgProcess::ExpPluginProcess(msgObject & msg)
{

    ExpID expId = msg.wParam;
    DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);

    PostMsgToApp(DEV_MSG_EXP_INSERT, expId, expType);

    ExpCountsIncrease();

    //exp 50数量增加，发送事件通知
    event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
}

bool CExp50MsgProcess::OnTimer(msgObject & msg)
{
    CBaseExpMsgProcess::OnTimer(msg);

    UINT iTimerId = (UINT)msg.wParam;

    if (iTimerId == (UINT)&m_uPageKeyAntiShakeTimer)
    {
        OnPageKeyTimer();
    }
    return true;
}

bool CExp50MsgProcess::OnPageKeyTimer()
{
	EXP_INFO("EXP50 page key anti shake timer, responsing");
	KillPageKeyTimer();
	if (m_bPageKeyCheck && m_nCheckCount > 0)
	{
		int expId = GetExpIndexFromDssKeyID(m_nLastCode);
		int iPageIndex = GetKeyCodeFromDsskeyID(m_nLastCode) - EXP_KEY_PAGE1;

		if (GetCurrentExpPage(expId) != iPageIndex)
		{
			EXP_INFO("[Exp-Info] exp50 expIndex[%d] has logically switched to pageIndex[%d] ", expId,
				iPageIndex);
			SetCurrentExpPage(expId, iPageIndex);
			PostMsgToApp(EXP_MESSAGE_SWITCH_PAGE, 0, m_nLastCode);
		}
	}
	m_nCheckCount = 0;
	m_nLastCode = 0;
	m_bPageKeyCheck = false;
	return true;
}

bool CExp50MsgProcess::SetPageKeyTimer()
{
	timerSetThreadTimer((UINT)&m_uPageKeyAntiShakeTimer, EXP_50_PAGE_KEY_ANTI_SHAKE_TIME);
	m_uPageKeyAntiShakeTimer = GetMsgProcessType();
	EXP_INFO("EXP50 page key anti shake timer has been setted");
	return true;
}

bool CExp50MsgProcess::KillPageKeyTimer()
{
	timerKillThreadTimer((UINT)&m_uPageKeyAntiShakeTimer);
	EXP_INFO("EXP50 page key anti shake timer has been killed");
	return true;
}

#endif