// mod-this(TalkUI)
#include "dlgringingbase.h"

// mod-InputMethod
#include "inputmethod/inputmethod.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

#include "commonwidget.h"
#include "contactinfoframe.h"
#include "talkui_define.h"
#include "talkuimanager.h"
#include "qpictureanimation.h"

//mod-Call center
#include "broadsoft/callcenter/include/modcallcenter.h"

//mod-MainWnd
#include "uimanager/rectdefine.h"

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif
#include "talk/talklogic/include/callinfo.h"

#include "talk/talklogic/include/modtalklogic.h"

#include "talkuidelegate.h"
#include "broadsoft/xsiactionhelper/include/modxsicommon.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "talk/fwd_dnd/include/modfwddnd.h"

#include "talkuicommon.h"
#include "voice/include/modvoice.h"
#include "application/include/modapplication.h"

// 动画控件的位置
const QRect RECT_CONNECT_ANIMATION(67, 131, 133, 26);

//构造函数
CDlgRingingBase::CDlgRingingBase()
    : CDlgNormalTalkBase(DLG_Ringing, TALK_UI_RING)
    , m_bIsSilence(false)
    , m_nCallID(0)
    , m_bIsRecalling(false)
    , m_bCallCenterInfoAvaliable(false)
    , m_bAnswered(false)
    , m_bCallDeclineEnable(false)
    , m_bIsAllowForward(true)
#ifdef IF_FEATURE_EMERGENCY
    , m_bEmergencyCalled(false)
#endif
{
    InitData();
}

//析构函数
CDlgRingingBase::~CDlgRingingBase()
{

}

// 初始化页面的数据
void CDlgRingingBase::InitData()
{

}

// 初始化
void CDlgRingingBase::Init()
{
    CDlgNormalTalkBase::Init();


    m_bIsSilence = false;

    // 是否recall状态
    m_bIsRecalling = false;
    // 是否显示CallCenter Call Information
    m_bCallCenterInfoAvaliable = false;

    m_bAnswered = false;

    m_bIsAllowForward = true;
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgRingingBase::Uninit()
{
    CDlgNormalTalkBase::Uninit();

}

// 焦点切换
void CDlgRingingBase::OnFocusChanged(bool bFocus)
{

}


void CDlgRingingBase::OnTipChange(TALK_TIP_TYPE eTipType)
{

}

bool CDlgRingingBase::IsShowTip()
{
    return true;
}

//设置通话数据内容
void CDlgRingingBase::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgNormalTalkBase::SetTalkData(pData);

    // 入口参数判断
    DataRing * pRingingtData = dynamic_cast<DataRing *>(pData);
    if (pRingingtData == NULL)
    {
        return;
    }
    // 设置联系人信息
    m_bCallDeclineEnable = pRingingtData->bCallDeclineEnable;

    m_bIsRecalling = (m_eSessionState == SESSION_RECALL)
                     || Xsi_IsExecutiveCallPushRecall(GetSessionId());


#ifdef IF_FEATURE_EMERGENCY
    m_bEmergencyCalled = pRingingtData->bEmergencyCalled;
#endif


    //这个 主要是用于 获取softkey时候是否显示CallInfo 的softkey使用的
    // V80 中 pRingingtData->bShowCallCenterInfo 与  pRingData->pCCInfo 一起判断是显示ACDInfo 界面还是 Ringing界面 而不是用于 判断是否显示TALK_UI_ACTION_ACD_CALL_INFO 的softkey了
    // 是否显示 TALK_UI_ACTION_ACD_CALL_INFO 的softkey 判断是 用 pRingingtData->bCallCenterInfoAvaliable 来判断
    m_bCallCenterInfoAvaliable = pRingingtData->bCallCenterInfoAvaliable;
//  m_bShowCallCenterInfo = pRingingtData->bShowCallCenterInfo;

    // 设置静音图标
    m_bIsSilence = pRingingtData->bShowSilence;

    m_bIsAllowForward = pRingingtData->bAllowFwd;

    // 设置页面的Title
    yl::string strTitle("");
    if (m_bIsRecalling)
    {
        strTitle = yl::string(TRID_RECALL);
    }
    else
    {
        strTitle = yl::string(TRID_RINGING);
    }

    _TalkUIDelegate.UpdateTitleInfo(strTitle);

//  emit ChangeTitle(strTitle.toUtf8().data());

    // 设置静音图标
    _TalkUIDelegate.AddTitleIcon(PIC_ICON_TALK_MUTE, pRingingtData->bShowSilence
                                 || voice_GetVolume(VT_RING) == 0);

    //m_bIsSilence = pRingingtData->bShowSilence;
    //TALKUI_INFO("m_bIsSilence is %d.", m_bIsSilence);
    //AddOrDeleteIcon(PIC_ICON_TALK_MUTE, pRingingtData->bShowSilence);

}

//设置Softkey数据Action值
void CDlgRingingBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
                                       DataLogic2UIBase * pData/* = NULL*/)
{
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (application_IsDoorLineCall(GetSessionId()))
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_OPENDOOR);
    }
#endif
    //移植T49蓝牙通话模块的功能
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //是蓝牙通话 走蓝牙 获取softkey流程
    if (GetSessionState() == SESSION_BTRINGING)
    {
        GetBlueToothRingSoftkey(objSoftkeyData);

        return ;
    }
#endif

    // 设置二级菜单界面对应SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ANSWER);

    if (m_bCallDeclineEnable)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_DECLINE_IGNORE);
    }

    if (!m_bIsRecalling)
    {
        if (m_bIsAllowForward)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_FORWARD);
        }
        else
        {
            AddSoftkey(objSoftkeyData, "");
        }
    }
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

    if (CallCenter_IsCallInfoOn()
            && m_bCallCenterInfoAvaliable
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && !m_bIsRecalling)
    {
        // 设置二级菜单界面对应SoftKeybar的数据
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_CALL_INFO);
    }
    else
    {
        // 设置二级菜单界面对应SoftKeybar的数据
        if (m_bIsSilence || (0 == voice_GetVolume(VT_RING)))
        {
            AddSoftkey(objSoftkeyData, "");
        }
        else
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_IGNORE);
        }
    }
#ifdef IF_FEATURE_EMERGENCY
    //如果是紧急号码回呼情况，则进行屏蔽拒接按钮
    if (IsRecall() && m_bEmergencyCalled)
    {
        AddSoftkey(objSoftkeyData, "");
    }
    else
#endif
        if (m_bCallDeclineEnable)
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_DECLINE);
        }
        else if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
                )
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_REJECT);
        }
        else
        {
            AddSoftkey(objSoftkeyData, "");
        }
}

#if IF_FEATURE_CUSTOM_SOFTKEY
void CDlgRingingBase::GetDynamicSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                                        DataLogic2UIBase * pData/* = NULL*/)
{
    if (CallCenter_IsCallInfoOn()
            && m_bCallCenterInfoAvaliable
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && !m_bIsRecalling)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ACD_CALL_INFO);
    }
}

bool CDlgRingingBase::IsSoftkeyAllowToShow(const yl::string & strSoftkeyAction,
        DataLogic2UIBase * pData/* = NULL*/)
{
    if (TALK_UI_ACTION_IGNORE == strSoftkeyAction)
    {
        // 设置二级菜单界面对应SoftKeybar的数据
        if (m_bIsSilence || (0 == voice_GetVolume(VT_RING)))
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_FORWARD == strSoftkeyAction)
    {
        if (m_bIsRecalling
                || !m_bIsAllowForward)
        {
            return false;
        }
    }
    else if (!CDlgBaseTalkUI::IsSoftkeyAllowToShow(strSoftkeyAction, pData))
    {
        return false;
    }

    return true;
}
#endif

//是否显示音量条
bool CDlgRingingBase::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                      QRect & rtVolume, yl::string & strBGPath) const
{
    rtVolume = RECT_VOLUMEBAR;
    QRect rectTalkUI = geometry();
    rtVolume.moveBottom(rectTalkUI.bottom() - 6);// 离底边6像素

//     (talklogic_IsAllSessionRinging() || TalkUIlogic_IsAllowSilenceDuringCall()) ? iMin = 0 : iMin = 1;
    iMin = TalkUIlogic_IsAllowSilenceDuringCall() ? 0 : 1;
    iMax = 15;
    return true;
}

//是否显示DTMF条
bool CDlgRingingBase::IsShowDtmfLine() const
{
    return false;
}

//是否显示加密图标,rectEncrypt hold the location of encrypt label
bool CDlgRingingBase::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return true;
}

//处理UI事件
bool CDlgRingingBase::OnUIAction(const QString & strAction)
{
    TALKUI_INFO("RingingUI-OnUIAction(%s)", strAction.toUtf8().data());

    if (strAction == TALK_UI_ACTION_ANSWER)
    {
        SendUIRequestAction(TALK_ACTION_ANSWER);
        ProcessAnswerIncomingCall();
    }
    else if (strAction == TALK_UI_ACTION_FORWARD)
    {
        if (!m_bIsRecalling)
        {
            SendUIRequestAction(TALK_ACTION_FWD_TO);
        }
    }
    else if (strAction == TALK_UI_ACTION_IGNORE)
    {
        //Ignore
        SendUIRequestAction(TALK_ACTION_SILENCE);
    }
    else if (strAction == TALK_UI_ACTION_REJECT
             || strAction == TALK_UI_ACTION_CANCEL)
    {
        if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)

           )
        {
            SendUIRequestAction(TALK_ACTION_REJECT);
        }
    }
    else if (strAction == TALK_UI_ACTION_ACD_CALL_INFO)
    {
        if (!m_bIsRecalling)
        {
            SendUIRequestAction(TALK_ACTION_CALLINFO);
        }
    }
    else if (strAction == TALK_UI_ACTION_MUTE_VOICE)
    {
        SendUIRequestAction(TALK_ACTION_MUTE);
    }
    else if (strAction == TALK_UI_ACTION_DECLINE_IGNORE)
    {
        if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)

           )
        {
            SendUIRequestAction(TALK_ACTION_CALL_DECLINE);
        }
    }
    else if (strAction == TALK_UI_ACTION_DECLINE)
    {
        if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)

           )
        {
            SendUIRequestAction(TALK_ACTION_CALL_DECLINE, TRUE, 0, NULL);
        }
    }
    else
    {
        return CDlgNormalTalkBase::OnUIAction(strAction);
    }
    return true;
}

//事件过滤器
bool CDlgRingingBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    return false;
}

// 通道键处理
bool CDlgRingingBase::OnChannelKeyPress(int iKey)
{
    bool bHandled = TRUE;
    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
    case PHONE_KEY_HEADSET_CTRL:
    case PHONE_KEY_HANDFREE:
        {
            //          AnswerIncomingCall(TRUE);
            //逻辑层会根据 第二个参数 是否为TRUE判断是否还要再次切换通道
            SendUIRequestAction(TALK_ACTION_ANSWER, 1, iKey);
            ProcessAnswerIncomingCall();
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            //问题：Wildix T28\T26\T22 手柄不能挂掉还没应答的通话
            //修改：来电界面挂机时不执行 ExitRoutine()操作
            //http://10.2.1.199/Bug.php?BugID=16809
            if (m_bAnswered)
            {
                // http://10.2.1.199/Bug.php?BugID=87362
                // 通话接起后, 挂断应该是cancel而不是reject
                SendUIRequestAction(TALK_ACTION_CANCEL, 1, 0, NULL);
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
            //  AnswerIncomingCall(true);
            //逻辑层会根据 第二个参数 是否为TRUE判断是否还要再次切换通道
            SendUIRequestAction(TALK_ACTION_ANSWER, 1, iKey);
            ProcessAnswerIncomingCall();
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

bool CDlgRingingBase::OnFunckeyPress(int iKey, bool bEXP)
{
    switch (iKey)
    {
    case PHONE_KEY_OK:
    case PHONE_KEY_ANSWER:
        {
            //Answer, OK键需要接起来电
            OnUIAction(TALK_UI_ACTION_ANSWER);
            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            OnUIAction(TALK_UI_ACTION_CANCEL);
            return true;
        }
        break;
#ifdef _T4X
    case PHONE_KEY_TRANSFER:
        {
            //来电Transfer按键当做Forward
            //非Recall
            if (SESSION_RECALL != m_eSessionState)
            {
                OnUIAction(TALK_UI_ACTION_FORWARD);
            }

            return true;
        }
        break;
#endif
    case PHONE_KEY_MUTE:
        {
            OnUIAction(TALK_UI_ACTION_MUTE_VOICE);
            return true;
        }
        break;
    default:
        break;
    }
    return CDlgNormalTalkBase::OnFunckeyPress(iKey, bEXP);
}

//数字键处理
bool CDlgRingingBase::OnDigitkeyPress(int iKey)
{
    return false;
}

//音量条变化调用函数
void CDlgRingingBase::OnTalkVolumnValueChange()
{
    //加上Mute 静音图标 因为这边有按了 音量条 所以 只要用音量大小判断
//  emit AddTitleIcon(PIC_ICON_RINGMUTE, voice_GetVolume(VT_RING) == 0 );
    _TalkUIDelegate.AddTitleIcon(PIC_ICON_TALK_MUTE, m_bIsSilence || voice_GetVolume(VT_RING) == 0);

    GetSoftkeyData(m_SoftkeyData, NULL);

    //刷新softkey  因为 silence 的softkey要变
    emit RequestRefreshSoftkey();
}

void CDlgRingingBase::GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus)
{
    strFile = "CallIn.xml";
    strStatus = "CallIn";
}

//获取蓝牙来电 softkey
void CDlgRingingBase::GetBlueToothRingSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        void * pData/* = NULL*/)
{
    //只需要显示3个softkey
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ANSWER);
    //普通通话显示 ST_FORWARD
    AddSoftkey(objSoftkeyData, "");

    //当只有一路来电时，m_objRingUIData.m_bIsSilence || (voice_GetVolume(VT_RING) == 0)条件已经可以满足
    //当已有一路通话时又有来电，当前情况下silence是针对来电话路的，而此时按音量键，调整的则是通话中话路的音量，因此按音量键不能影响此处的判断

    // 设置二级菜单界面对应SoftKeybar的数据
    if (m_bIsSilence || (0 == voice_GetVolume(VT_RING)))
    {
        AddSoftkey(objSoftkeyData, "");
    }
    else
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_IGNORE);
    }

    if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
       )
    {

        //  VecSoftkey.push_back(ST_SILENCE);
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_REJECT);
    }
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

}

// 是否允许自定义softkey
bool CDlgRingingBase::IsAllowDefineSoftkey()
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTRINGING)
    {
        return false;
    }
#endif

    return true;

}

void CDlgRingingBase::ProcessAnswerIncomingCall()
{
    if (configParser_GetConfigInt(kszAnswerByHand) == 0)
    {
        m_bAnswered = TRUE;
    }
    else
    {
        m_bAnswered = talklogic_IsRingingSessionAnswered(GetSessionId());
    }
}
