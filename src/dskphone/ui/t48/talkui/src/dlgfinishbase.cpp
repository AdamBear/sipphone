// mod-this(TalkUI)
#include "dlgfinishbase.h"
#include "commonwidget.h"
#include "contactinfoframe.h"
#include "talkui_define.h"
#include "talkuidelegate.h"

// mod-InputMethod
#include "inputmethod/inputmethod.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uimanager/rectdefine.h"

//mod-CommonAPI
#include "commonapi/stringhelperapi.h"
#include "configparser/modconfigparser.h"

// mod-MainWnd
#include "uimanager/rectdefine.h"

#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif
#include "talk/talklogic/include/callinfo.h"

#include "talkuicommon.h"
#include "voice/include/modvoice.h"

//控件位置定义
const QRect CONTACT_INFO_FRAME_GEOMETRY(6, 0, RECT_TALK_FRAME.width() - 8,
                                        RECT_TALK_FRAME.height());
const QRect TIP_INFO_LABEL_GEOMETRY(12, 140, 450, 40);

//构造函数
CDlgFinishedBase::CDlgFinishedBase()
    : CDlgNormalTalkBase(DLG_Finished, TALK_UI_FINISHED)
    , m_pAutoRedialTimer(NULL)
    , m_iRedialInterval(0)
    , m_iRemainRedialTimes(0)
{
    InitData();
}

//析构函数
CDlgFinishedBase::~CDlgFinishedBase()
{

}

// 初始化页面的数据
void CDlgFinishedBase::InitData()
{

}

// 初始化数据
void CDlgFinishedBase::Init()
{
    CDlgNormalTalkBase::Init();

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgFinishedBase::Uninit()
{
    if (NULL != m_pAutoRedialTimer)
    {
        m_pAutoRedialTimer->stop();
    }

    CDlgNormalTalkBase::Uninit();
}

void CDlgFinishedBase::OnAutoRedial()
{
    TALKUI_INFO("timer OnAutoRedial");
    // 计数减一
    m_iRedialInterval--;
    if (m_iRedialInterval > 0)
    {
        //界面更改一下时间
        UpdateAutoRedialNote();
    }
    else
    {
        if (NULL != m_pAutoRedialTimer)
        {
            // 计数到,清除定时器
            m_pAutoRedialTimer->stop();
        }
        // 向logic发送OK事件
        SendUIRequestAction(TALK_ACTION_OK);
    }
}

//设置通话数据内容
void CDlgFinishedBase::SetTalkData(DataLogic2UIBase * pData)
{
    //基类执行设置数据
    CDlgNormalTalkBase::SetTalkData(pData);

    DataCallFinished * pFinishData = dynamic_cast<DataCallFinished *>(pData);
    if (NULL == pFinishData)
    {
        return;
    }

    // 设置页面的Title
    yl::string strTitle("");

    if (pFinishData->bError)
    {
        //通话失败情况
        //设置提示文本
        strTitle = yl::string(TRID_CALL_FAIL);
        _TalkUIDelegate.UpdateTitleInfo(strTitle);
    }
    else
    {
        //正常通话结束情况
        //设置提示文本
        strTitle = yl::string(TRID_CALL_FINISH);
        _TalkUIDelegate.UpdateTitleInfo(strTitle);
        //emit ChangeTitle(strTitle);

        if (pFinishData->bAllowAutoRedial)
        {
            if (pFinishData->redialData.eStatus == AUTOREDIAL_COUNTDOWN)
            {
                if (m_pAutoRedialTimer == NULL)
                {
                    m_pAutoRedialTimer = new QTimer(this);
                    connect(m_pAutoRedialTimer, SIGNAL(timeout()), this, SLOT(OnAutoRedial()));
                    // 保存超时秒数
                    m_iRedialInterval = pFinishData->redialData.iRedialInterval;
                }

                m_iRemainRedialTimes = pFinishData->redialData.iRemainRedialTimes;

                //更新一下autoredial 相关信息
                UpdateAutoRedialNote();

                // 每隔1秒更新一次界面
                m_pAutoRedialTimer->start(1000);
            }

        }
    }

}

// 获取配置softkey的文件名和结点名
void CDlgFinishedBase::GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus)
{
    strFile = "CallFailed.xml";
    strStatus = "CallFailed";
}

//Finish界面使用
// 更新自动重拨提示语

// 是否允许自定义softkey
bool CDlgFinishedBase::IsAllowDefineSoftkey()
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTFINISHED)
    {
        return false;
    }

#endif

    return true;
}

void CDlgFinishedBase::GetBluetoothSoftkey(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData/* = NULL*/)
{
    DataCallFinished * pFinishData = dynamic_cast<DataCallFinished *>(pData);

    if (pFinishData == NULL)
    {
        return;
    }

    if (!pFinishData->bAllowNewCall
            || pFinishData->bMaxSessionError)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_NEWCALL);
    }
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, "");

    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);
}

//设置Softkey数据Action值
void CDlgFinishedBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
                                        DataLogic2UIBase * pData/* = NULL*/)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取蓝牙的softkey
    if (GetSessionState() == SESSION_BTFINISHED)
    {
        GetBluetoothSoftkey(objSoftkeyData, pData);

        return ;
    }
#endif

    // 设置二级菜单界面对应SoftKeybar的数据
    DataCallFinished * pFinishData = dynamic_cast<DataCallFinished *>(pData);

    //http://10.3.5.199/Bug.php?BugID=63118
    // 没有newcall softkey的时候 需要显示cancel
    if ((NULL == pFinishData)
            || pFinishData->bInvalidAccount
            || pFinishData->bMaxSessionError
            || (!pFinishData->bAllowNewCall && !pFinishData->bAllowAutoRedial)
       )
    {
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, "");
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);

        return ;
    }

    if (pFinishData != NULL
            && pFinishData->pCallInfo != NULL)
    {
        //使用逻辑层接口来判断
        if (talklogic_BeAllowNewCall() && pFinishData->bAllowNewCall)
        {
            if (acc_IsAccountAvailable(pFinishData->pCallInfo->GetAccountID()))
            {
                //若帐号为注册上且话路未达到最大话路,则允许呼出
                AddSoftkey(objSoftkeyData, TALK_UI_ACTION_NEWCALL);
            }
            else
            {
                AddSoftkey(objSoftkeyData, "");
            }
        }
        else if (pFinishData->bAllowAutoRedial)
        {
            //若允许auto dial，F1应该为OK
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_AUTOREDIAL);
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

    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, "");

    //  http://10.3.5.199/Bug.php?BugID=62052
    //统一成都显示cancel
    //http://10.3.5.199/Bug.php?BugID=56032
    //如果是call failed 那么显示的softkey 和 默认的softkey 相同
    //if (pFinishData != NULL
    //  && true == pFinishData->bError)
    //{
    //  AddSoftkey(objSoftkeyData, "");
    //}
    //else
    //{
    // 设置二级菜单界面对应SoftKeybar的数据
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);
    //}
}

//是否显示音量条
bool CDlgFinishedBase::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                       QRect & rtVolume, yl::string & strBGPath) const
{
    rtVolume = RECT_VOLUMEBAR;
    QRect rectTalkUI = geometry();
    rtVolume.moveBottom(rectTalkUI.bottom() - 6);// 离底边6像素

    iMin = TalkUIlogic_IsAllowSilenceDuringCall() ? 0 : 1;
    iMax = 15;
    return true;
}

//是否显示DTMF条
bool CDlgFinishedBase::IsShowDtmfLine() const
{
    return false;
}

//是否显示加密图标,rectEncrypt hold the location of encrypt label
bool CDlgFinishedBase::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return true;
}

bool CDlgFinishedBase::IsShowTip()
{
    return CDlgNormalTalkBase::IsShowTip();
}

//处理UI事件
bool CDlgFinishedBase::OnUIAction(const QString & strAction)
{
    TALKUI_INFO("FinishedUI-OnUIAction(%s)", strAction.toUtf8().data());
    if (strAction == TALK_UI_ACTION_CANCEL)
    {
        //Cancel退出通话
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_NEWCALL)
    {
        //New Call新起一路通话
        SendUIRequestAction(TALK_ACTION_NEWCALL);
    }
    else if (strAction == TALK_UI_ACTION_AUTOREDIAL)
    {
        SendUIRequestAction(TALK_ACTION_OK);
    }
    else
    {
        return CDlgNormalTalkBase::OnUIAction(strAction);
    }

    return true;
}

//事件过滤器
bool CDlgFinishedBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    return false;
}

// 通道键处理
bool CDlgFinishedBase::OnChannelKeyPress(int iKey)
{
    //这边将 一些key 相同状态的处理 也分开 是为了后续较容易理解
    //状态是finished
    if (SESSION_FINISHED == m_eSessionState)
    {
        return CDlgNormalTalkBase::OnChannelKeyPress(iKey);
    }
    else    //状态是 autoredial
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

            OnUIAction(TALK_UI_ACTION_NEWCALL);
            //  SendUIRequestAction(TALK_ACTION_NEWCALL);
            return true;
        }
        else if (PHONE_KEY_HANDSET_ON_HOOK != iKey)
        {
            return CDlgNormalTalkBase::OnChannelKeyPress(iKey);
        }

        return false;
    }

    return false;
}

bool CDlgFinishedBase::OnFunckeyPress(int iKey, bool bEXP)
{
    //这边将 一些key 相同状态的处理 也分开 是为了后续较容易理解
    //状态是finished
    if (SESSION_FINISHED == m_eSessionState)
    {
        switch (iKey)
        {
        case PHONE_KEY_CANCEL:
            {
                SendUIRequestAction(TALK_ACTION_CANCEL);
                return true;
            }
            break;
        default:
            break;
        }

        return false;
    }
    else    //状态是 autoredial
    {
        switch (iKey)
        {
        case PHONE_KEY_CANCEL:
            {
                OnUIAction(TALK_UI_ACTION_CANCEL);
                return true;
            }
            break;
        case PHONE_KEY_OK:
            {
                OnUIAction(TALK_UI_ACTION_AUTOREDIAL);
                return true;
            }
            break;
        default:
            break;
        }
    }

    return false;
}

//数字键处理
bool CDlgFinishedBase::OnDigitkeyPress(int iKey)
{
    //这边将 一些key 相同状态的处理 也分开 是为了后续较容易理解
    //状态是finished
    if (SESSION_FINISHED == m_eSessionState)
    {
        return false;
    }
    else    //状态是 autoredial
    {
        talklogic_EnterPredialUI(iKey);
        return true;
    }

    return false;
}


