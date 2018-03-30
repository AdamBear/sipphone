#include "autoredialprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "uilogicautoredialuibase.h"

CAutoRedialProcessor::CAutoRedialProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_AUTOREDIAL)
{
    m_pTalkUI = NULL;
    m_bIsCountDown = false;
}

CAutoRedialProcessor::~CAutoRedialProcessor()
{
    m_pTalkUI = NULL;
    m_bIsCountDown = false;
}

//构造完成后 绑定了UI后调用
void CAutoRedialProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objAutoRedialUIData.Reset();

    //如果定时器在跑 先关闭定时器
    if (m_objAutoRedialTimer.IsTimerRuning())
    {
        m_objAutoRedialTimer.KillTimer();
    }
    //设置成不在倒计时状态
    m_bIsCountDown = false;
}

//释放前解除UI绑定前调用
void CAutoRedialProcessor::Uninit()
{
    //如果定时器在跑 先关闭定时器
    if (m_objAutoRedialTimer.IsTimerRuning())
    {
        m_objAutoRedialTimer.KillTimer();
    }

    //设置成不在倒计时状态
    m_bIsCountDown = false;

    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CAutoRedialProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicAutoRedialUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CAutoRedialProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取数据
CAutoRedialUIData * CAutoRedialProcessor::GetAutoRedialUIData()
{
    return &m_objAutoRedialUIData;
}

//设置数据
void CAutoRedialProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    DataCallFinished * pFinishData = (DataCallFinished *)pData;

    //获取联系人信息
    GetContactInfoFromCallInfo(m_objAutoRedialUIData.objContactInfo, pFinishData->pCallInfo);

    //赋值状态
    m_objAutoRedialUIData.eAutoRedialStatus = pFinishData->redialData.eStatus;

    //先把两个时间设置为0
    m_objAutoRedialUIData.nRedialInterval = 0;
    m_objAutoRedialUIData.nRemainRedialTimes = 0;

    //设置的信息置为空
    m_objAutoRedialUIData.strInfo = "";

    //倒计时
    if (AUTOREDIAL_COUNTDOWN == pFinishData->redialData.eStatus)
    {
        // 防止正在倒计时时进行的界面更新,导致界面显示错误,如挂机
        //就是防止再次刷新
        if (false == m_bIsCountDown) //不是在倒计时状态 才需要设置
        {
            //设置成是在倒计时状态
            m_bIsCountDown = true;

            //保存自动重拨的秒数
            m_objAutoRedialUIData.nRedialInterval = pFinishData->redialData.iRedialInterval;
            //保存自动重拨的次数
            m_objAutoRedialUIData.nRemainRedialTimes = pFinishData->redialData.iRemainRedialTimes;

            //没有启动定时器 才需要启动
            if (!m_objAutoRedialTimer.IsTimerRuning())
            {
                m_objAutoRedialTimer.SetTimer(1000, NULL, MK_MSG_ENTRY(this, CAutoRedialProcessor::OnTimer));
            }
        }

    }
    else
    {
        m_objAutoRedialUIData.strInfo = _UILOGIC_LANG_TRAN(TRID_AUTO_REDIAL) + yl::string("?");
    }

    //还是更新一下  以免ui没有更新数据
    UpdateAutoRedialNote();
}

// 通道键处理
bool CAutoRedialProcessor::OnChannelKeyPress(int iKey)
{
    // 自动重拨中,摘机就新建一路
    if (iKey == PHONE_KEY_HANDSET_OFF_HOOK || iKey == PHONE_KEY_HANDFREE)
    {
        // http://10.2.1.199/Bug.php?BugID=61258
        // 通过免提键新起话路时，强制将通道切成免提
        if (iKey == PHONE_KEY_HANDFREE)
        {
            voice_SwitchChannel(CHANNEL_HANDFREE);
        }

        SendUIRequestAction(TALK_ACTION_NEWCALL);

        return true;
    }
    else if (PHONE_KEY_HANDSET_ON_HOOK != iKey
#if IF_TALKUI_ONHOOK_EXITCALL
             || true
#endif
            )
    {
        return CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return false;
}

// 功能键处理
bool CAutoRedialProcessor::OnFunckeyPress(int iKey)
{
    if (iKey == PHONE_KEY_OK)
    {
        OnRedial();
    }
    else if (iKey == PHONE_KEY_CANCEL)
    {
        OnCancel();
    }
    else
    {
        return false;
    }

    return false;
}

// 数字键处理
bool CAutoRedialProcessor::OnDigitkeyPress(int iKey)
{
    talklogic_EnterPredialUI(iKey);
    return true;
}

//softKey 按键处理
bool CAutoRedialProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    if (eSoftkey == ST_CANCEL)
    {
        OnCancel();
    }
    else if (eSoftkey == ST_OK)
    {
        OnRedial();
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

//设置title
void CAutoRedialProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //  http://10.3.5.199/Bug.php?BugID=63029
    m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_FINISH);
}

//设置图标列表
void CAutoRedialProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    //不需要显示icons

}

//给子界面设置数据
void CAutoRedialProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objAutoRedialUIData, eReason);
    }
}


//获取softkey数据信息
void CAutoRedialProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    VecSoftkey.push_back(ST_OK);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_CANCEL);
}

// softkey处理(自定义类型)
void CAutoRedialProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
//不允许自定义softkey
}

// 是否允许自定义softkey
//不允许自定义softkey
bool CAutoRedialProcessor::IsAllowDefineSoftkey()
{
    return false;
}

//定时函数
BOOL CAutoRedialProcessor::OnTimer(msgObject & msg)
{
    // 时间计数减一
    m_objAutoRedialUIData.nRedialInterval--;
    if (m_objAutoRedialUIData.nRedialInterval > 0)
    {
        //界面更改一下时间
        UpdateAutoRedialNote();
    }
    else
    {
        //呼出
        OnRedial();
    }

    return TRUE;
}

//重拨、呼出
void CAutoRedialProcessor::OnRedial()
{
    //先要关闭定时器
    if (m_objAutoRedialTimer.IsTimerRuning())
    {
        m_objAutoRedialTimer.KillTimer();
    }

    //设置成没有在跑
    m_bIsCountDown = false;

    // 向logic发送OK事件
    SendUIRequestAction(TALK_ACTION_OK);

}

//取消 redial
void CAutoRedialProcessor::OnCancel()
{
    //先要关闭定时器
    if (m_objAutoRedialTimer.IsTimerRuning())
    {
        m_objAutoRedialTimer.KillTimer();
    }

    //设置成没有在跑
    m_bIsCountDown = false;

    //发送取消事件
    SendUIRequestAction(TALK_ACTION_CANCEL);

}

//更新界面提示语
void CAutoRedialProcessor::UpdateAutoRedialNote()
{
    //调用UI的接口来更新
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->UpdateAutoRedialNote(m_objAutoRedialUIData.eAutoRedialStatus,
                                        m_objAutoRedialUIData.strInfo, m_objAutoRedialUIData.nRemainRedialTimes,
                                        m_objAutoRedialUIData.nRedialInterval);
    }
}



