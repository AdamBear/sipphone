#include "acdcallinfoprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uilogicacdcallinfouibase.h"

CACDCallInfoProcessor::CACDCallInfoProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_ACDCALLINFO)
{
    m_pTalkUI = NULL;
}

CACDCallInfoProcessor::~CACDCallInfoProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CACDCallInfoProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objACDCallInfoUIData.Reset();

    //如果定时器在跑 先关闭定时器
    StopACDCallInfoTimer();
}

//释放前解除UI绑定前调用
void CACDCallInfoProcessor::Uninit()
{
    //如果定时器在跑 关闭定时器
    StopACDCallInfoTimer();

    CBaseTalkUIProcessor::Uninit();
}

//设置绑定的UI
void CACDCallInfoProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicACDCallInfoUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CACDCallInfoProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取数据
CACDCallInfoUIData * CACDCallInfoProcessor::GetACDCallInfoUIData()
{
    return &m_objACDCallInfoUIData;
}

// 焦点改变.
// | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
void CACDCallInfoProcessor::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        //有焦点要开启定时器
        StartACDCallInfoTimer();
    }
    else
    {
        //没有焦点要关闭定时器
        StopACDCallInfoTimer();
    }

    CBaseTalkUIProcessor::OnFocusChanged(bFocused);

}

//设置数据
void CACDCallInfoProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //设置数据 需要重新启动定时器
    //如果定时器在跑 先关闭定时器
//  StopACDCallInfoTimer();

    //开关定时器统一放到 onfocuschanged中处理
    //这边也在做一次 防止在刷新的时候 不会重置定时器
    //开启定时器
    StartACDCallInfoTimer();


    // 设置联系人信息
    CallCenterInfo * pCallInfoData = (CallCenterInfo *)pData;

    char szData[256] = {0};

    //Wait Time
    yl::string strName = _UILOGIC_LANG_TRAN(TRID_WAIT_TIME);
    sprintf(szData, "%d", pCallInfoData->m_nWaitTime);
    m_objACDCallInfoUIData.strWaitTime = strName + yl::string(": ") + yl::string(
            szData) + yl::string(" s");

    //CC Name
    strName = _UILOGIC_LANG_TRAN(TRID_CALL_CENTER_NAME);
    m_objACDCallInfoUIData.strCCName = strName + yl::string(": ") + pCallInfoData->m_strCallCenterName;

    //CC UID
    strName = _UILOGIC_LANG_TRAN(TRID_CALL_CENTER_USER_ID);
    m_objACDCallInfoUIData.strCCUID = strName + yl::string(": ") + pCallInfoData->m_strUserId;

    //Queue
    strName = _UILOGIC_LANG_TRAN(TRID_QUEUE);
    sprintf(szData, "%d", pCallInfoData->m_nCountInQueue);
    m_objACDCallInfoUIData.strQueue = strName + yl::string(": ") + yl::string(szData);

    //longest
    if (pCallInfoData->m_nLongestTime != -1)
    {
        strName = _UILOGIC_LANG_TRAN(TRID_LONGEST_CALL_DURATION);
        sprintf(szData, "%d", pCallInfoData->m_nLongestTime);
        m_objACDCallInfoUIData.strCallsLongest = strName + yl::string(": ") + yl::string(
                    szData) + yl::string(" s");

    }
    else    //值是错误的
    {
        //这边一定要设置为空
        m_objACDCallInfoUIData.strCallsLongest = "";
    }


}

// 通道键处理
bool CACDCallInfoProcessor::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

    // 双耳麦模式，摘挂机只更改通道，不再继续往下传
    if (voice_IsCallCenterMode()
            && (iKey == PHONE_KEY_HANDSET_OFF_HOOK
                || iKey == PHONE_KEY_HANDSET_ON_HOOK))
    {
        return bHandled;
    }

    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
    case PHONE_KEY_HEADSET_CTRL:
    case PHONE_KEY_HANDFREE:
        {
            // 耳麦键接起来电时，需要将通道切到耳麦，比如耳麦通话中新来电，按耳麦接起
            if (iKey == PHONE_KEY_HEADSET_CTRL)
            {
                if (voice_IsCallCenterMode()
                        && voice_GetHandsetStatus())
                {
                    // 摘机状态打开双耳麦模式
                    voice_SwitchChannel(CHANNEL_2HEADSET);
                }
                else
                {
                    voice_SwitchChannel(CHANNEL_HEADSET);
                }
            }

            //http://10.3.5.199/Bug.php?BugID=65286
            AnswerIncomingCall(true);
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            //问题：Wildix T28\T26\T22 手柄不能挂掉还没应答的通话
            //修改：来电界面挂机时不执行 ExitRoutine()操作
            //http://10.2.1.199/Bug.php?BugID=16809
            if (m_objACDCallInfoUIData.bAnswer)
            {
                //如果定时器在跑 先关闭定时器
                StopACDCallInfoTimer();

                //这边不能调用ExitACDCallInfo函数  因为 参数不一样
                SendUIRequestAction(TALK_ACTION_REJECT, 1, 0, NULL);

                // 如果由通道键引发的退出，且还有通话存在，则切换通道
                if (talklogic_SessionExist())
                {
                    voice_SwitchChannel();
                }
            }
            else
            {
                // http://10.2.1.199/Bug.php?BugID=46003
                // 挂机不挂断通话，但要切换通道
                voice_SwitchChannel();
            }
        }
        break;
    case PHONE_KEY_EHS_OPEN:
        {
            // 来电时打开EHS则接起来电
            AnswerIncomingCall(true);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 功能键处理
bool CACDCallInfoProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_OK:
    case PHONE_KEY_ANSWER:
        {
            //Answer, OK键需要接起来电
            //参数设置成不是通道键接起的来电
            AnswerIncomingCall(false);

            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            //退出该界面
            ExitACDCallInfo();
            //  SendUIRequestAction(TALK_ACTION_CANCEL);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

// 数字键处理
//不处理数字键
bool CACDCallInfoProcessor::OnDigitkeyPress(int iKey)
{
    return true;
}

//softKey 按键处理
bool CACDCallInfoProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    switch (eSoftkey)
    {
    case ST_ANSWER:
        {
            //不是通道键接起的来电
            AnswerIncomingCall(false);
            bHandled = true;
        }
        break;
    case ST_CANCEL:
    case ST_REJECT:
    case ST_EXIT:
        {
            //退出该界面
            ExitACDCallInfo();
            bHandled = true;
            //  bHandled = SendUIRequestAction(TALK_ACTION_CANCEL);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;

}

//设置title
void CACDCallInfoProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_INFORMATION);
}

//设置图标列表
void CACDCallInfoProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    //acdcallinfo 没有图标
}

//给子界面设置数据
void CACDCallInfoProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objACDCallInfoUIData, eReason);
    }
}

//获取softkey数据信息
void CACDCallInfoProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    // 设置二级菜单界面对应SoftKeybar的数据
    VecSoftkey.push_back(ST_ANSWER);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EXIT);
}

// softkey处理(自定义类型)
void CACDCallInfoProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    //不允许自定义softkey
}

// 是否允许自定义softkey
bool CACDCallInfoProcessor::IsAllowDefineSoftkey()
{
    return false;
}

// 应答来电
void CACDCallInfoProcessor::AnswerIncomingCall(bool bChannelKey)
{
    //如果定时器在跑 先关闭定时器
    StopACDCallInfoTimer();

    //修改应答的标志位
    m_objACDCallInfoUIData.bAnswer = true;
    //如果是通道键接起那么需要发送1  因为逻辑层还需要设置一下通道
    SendUIRequestAction(TALK_ACTION_ANSWER, bChannelKey ? 1 : 0);
}

//退出ACDCallInfo界面
void CACDCallInfoProcessor::ExitACDCallInfo()
{
    //如果定时器在跑 先关闭定时器
    StopACDCallInfoTimer();

    SendUIRequestAction(TALK_ACTION_CANCEL);
}

//开启定时器功能
void CACDCallInfoProcessor::StartACDCallInfoTimer()
{
    //如果定时器在跑 先关闭定时器
    StopACDCallInfoTimer();

    m_objExitTimer.SetTimer(CallCenter_GetCallInfoShowTime() * 1000 + 1, NULL, MK_MSG_ENTRY(this,
                            CACDCallInfoProcessor::OnTimer));
}

//关闭定时器功能
void CACDCallInfoProcessor::StopACDCallInfoTimer()
{
    //如果定时器在跑 关闭定时器
    if (m_objExitTimer.IsTimerRuning())
    {
        m_objExitTimer.KillTimer();
    }
}

//定时函数
BOOL CACDCallInfoProcessor::OnTimer(msgObject & msg)
{
    //退出定时函数
    //if(&m_objExitTimer == msg)
    //{
    //  ExitACDCallInfo();
    //}
    //暂时只有这个退出的在用这个定时  所以不用判断参数 所以直接退出
    ExitACDCallInfo();

    return TRUE;

}

