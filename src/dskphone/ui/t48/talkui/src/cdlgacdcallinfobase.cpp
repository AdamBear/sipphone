/************************************************************************
 * FileName  : CDlgACDCallInfo.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/24 9:50:32
 ************************************************************************/

#include "cdlgacdcallinfobase.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "talkuimanager.h"
#include "uikernel/qsoftkeybar.h"
#include "qtcommon/qmisc.h"
#include "uimanager/rectdefine.h"
#include "contactinfoframe.h"


#include "talkuidelegate.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "voice/include/modvoice.h"

#ifdef _T4X
const int   ITEM_HEIGHT = RECT_TALK_PAGE.height() / 5;
const QRect RECT_WAITTIME_INFO(2, 0, RECT_TALK_PAGE.width() - 2, ITEM_HEIGHT);
#endif

CDlgACDCallInfoBase::CDlgACDCallInfoBase()
    : CDlgNormalTalkBase(DLG_CDlgACDCallInfo, TALK_UI_RING)
    , m_bAnswered(false)
    , m_pQuitTimer(NULL)
{
    InitData();
}

CDlgACDCallInfoBase::~CDlgACDCallInfoBase()
{
}

// 初始化页面数据
void CDlgACDCallInfoBase::InitData()
{
    // 自动退出
    m_pQuitTimer = new QTimer(this);
    connect(m_pQuitTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));

}

// 初始化
void CDlgACDCallInfoBase::Init()
{
    CDlgNormalTalkBase::Init();

    m_bAnswered = false;

    //进入界面的时候要启动
    if (NULL != m_pQuitTimer)
    {
        m_pQuitTimer->stop();

        m_pQuitTimer->start(CallCenter_GetCallInfoShowTime() * 1000);
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgACDCallInfoBase::Uninit()
{
    //只需要执行一次定时器
    if (NULL != m_pQuitTimer)
    {
        m_pQuitTimer->stop();
    }

    CDlgNormalTalkBase::Uninit();
}

//设置通话数据内容
void CDlgACDCallInfoBase::SetTalkData(DataLogic2UIBase * pData)
{
    CDlgNormalTalkBase::SetTalkData(pData);

    //进入界面的时候要启动
    if (NULL != m_pQuitTimer)
    {
        m_pQuitTimer->stop();

        m_pQuitTimer->start(CallCenter_GetCallInfoShowTime() * 1000);
    }

    // 入口参数判断
    if (pData == NULL)
    {
        return;
    }

    // 设置页面的Title
    yl::string strTitle = TRID_CALL_INFORMATION;

    _TalkUIDelegate.UpdateTitleInfo(strTitle);

}

// 事件过滤器
bool CDlgACDCallInfoBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    return false;
}

// 通道键处理
bool CDlgACDCallInfoBase::OnChannelKeyPress(int iKey)
{
    bool bHandled = TRUE;
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
            //          AnswerIncomingCall(TRUE);
            SendUIRequestAction(TALK_ACTION_ANSWER, 1);
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            //问题：Wildix T28\T26\T22 手柄不能挂掉还没应答的通话
            //修改：来电界面挂机时不执行 ExitRoutine()操作
            //http://10.2.1.199/Bug.php?BugID=16809
            if (m_bAnswered)
            {
                //如果不为拒接模式
                if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
                   )
                {
                    SendUIRequestAction(TALK_ACTION_REJECT, 1, 0, NULL);
                    // 如果由通道键引发的退出，且还有通话存在，则切换通道
                    if (talklogic_SessionExist())
                    {
                        voice_SwitchChannel();
                    }
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
            SendUIRequestAction(TALK_ACTION_ANSWER, 1);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

// 功能键处理
bool CDlgACDCallInfoBase::OnFunckeyPress(int iKey, bool bEXP)
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
    default:
        break;
    }

    return false;
}

//数字键处理
bool CDlgACDCallInfoBase::OnDigitkeyPress(int iKey)
{
#ifdef TALKUI_DEBUG_TEST
    if (PHONE_KEY_POUND == iKey)
    {
        _TalkUIManager.ShowACDQueueStatus();

        return true;
    }
    //测试使用  主动更新 QueueStatus
#endif


    //ACDCallInfo 不处理数字键
    return false;

}


//设置Softkey数据Action值
void CDlgACDCallInfoBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData/* = NULL*/)
{
    // 设置二级菜单界面对应SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_ANSWER);
    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL, LANG_TRANSLATE(TRID_EXIT));
}

//处理UI事件
bool CDlgACDCallInfoBase::OnUIAction(const QString & strAction)
{
    TALKUI_INFO("CDlgACDCallInfo::OnUIAction(%s)", strAction.toUtf8().data());
    if (strAction == TALK_UI_ACTION_ANSWER)
    {
        m_bAnswered = true;

        SendUIRequestAction(TALK_ACTION_ANSWER);

        return true;
    }
    else if (strAction == TALK_UI_ACTION_CANCEL)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);

        return true;
    }
    else
    {
        chWARNING2(false, "DialUI OnAction string error!");
        return false;
    }

    return CDlgNormalTalkBase::OnUIAction(strAction);
}

void CDlgACDCallInfoBase::OnTimer()
{
    //只需要执行一次定时器
    if (NULL != m_pQuitTimer)
    {
        m_pQuitTimer->stop();
    }

    //时间到退出
    OnUIAction(TALK_UI_ACTION_CANCEL);
}
