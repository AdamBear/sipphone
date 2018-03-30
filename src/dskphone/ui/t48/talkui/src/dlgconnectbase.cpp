//mod-this(Talk-UI)
#include "dlgconnectbase.h"
#include "commonwidget.h"
//#include "contactinfoframe.h"
#include "talkui_define.h"

//mod-UIKernel
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"

//mod-Talk
// #include "talk/talklogic_inc.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
// #include "talk/talklogicmanager.h"
#include "uimanager/rectdefine.h"
#include "baseui/t4xpicpath.h"
//#include "qpictureanimation.h"

#include "talk/talklogic/include/commonfunctions.h"
#include "talkuidelegate.h"

//const QRect RECT_CONNECT_ANIMATION(67, 131, 136, 11);// 动画控件的位置

CDlgConnectBase::CDlgConnectBase()
    : CDlgNormalTalkBase(DLG_Connect, TALK_UI_CONNECT)
{
    //初始化数据
    InitData();
}

CDlgConnectBase::~CDlgConnectBase()
{

}


// 释放数据，将窗口清空回复到初始化状态
void CDlgConnectBase::Init()
{
    CDlgNormalTalkBase::Init();

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConnectBase::Uninit()
{
    CDlgNormalTalkBase::Uninit();

}

////---- 确认无用后删除
// 焦点切换
void CDlgConnectBase::OnFocusChanged(bool bFocus)
{
    CDlgNormalTalkBase::OnFocusChanged(bFocus);
}

void CDlgConnectBase::InitData()
{

}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgConnectBase::OnTipChange(TALK_TIP_TYPE eTipType)
{

}

void CDlgConnectBase::SetTalkData(DataLogic2UIBase * pData)
{
    //基类先执行一次
    CDlgNormalTalkBase::SetTalkData(pData);

    DataConnect * pConnectData = dynamic_cast<DataConnect *>(pData);
    if (pConnectData != NULL)
    {

        // 更新加密图标
        _TalkUIDelegate.AddTitleIcon(PIC_ICON_ENCRYPT, pConnectData->bEncrypt);

        CCallInfo * pCallInfo = pConnectData->pCallInfo;
        if (pCallInfo != NULL)
        {
            // 设置页面的Title
            yl::string strTitle("");
            if (pConnectData->bConnectByTran)
            {
                strTitle = TRID_TRANSFERING;
            }
            else
            {
                strTitle = TRID_CALLING;
            }

            _TalkUIDelegate.UpdateTitleInfo(strTitle);

            //http://10.2.1.199/Bug.php?BugID=61033
            //在连接的时候不判断显示 高清图标
            // 更新HD图标
            //_TalkUIDelegate.AddTitleIcon(PIC_ICON_HD, pCallInfo->IsHDTalking());
        }
    }


}

void CDlgConnectBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
                                       DataLogic2UIBase * pData /* = NULL */)
{
    //http://10.2.1.199/Bug.php?BugID=59075
    //Transfer Connecting状态下，服务器还没有回复180回铃信息，此时Transfer按键无效
    //  DataConnect* pConnectData = (DataConnect*)pData;
    //  if (pConnectData != NULL && pConnectData->bConnectByTran)
    if (SESSION_RINGBACK_TRAN == m_eSessionState)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_TRANSFER);
    }
#if IF_BUG_23548
    else if (SESSION_RINGBACK == m_eSessionState
             && configParser_GetConfigInt(kszCallCompletionCallBackSwitch) == 1)
    {
        AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CALL_BACK);
    }
#endif
    else
    {
        AddSoftkey(objSoftkeyData, "");
    }

    // 设置SoftKeybar1
    AddSoftkey(objSoftkeyData, "");

    // 设置SoftKeybar2
    AddSoftkey(objSoftkeyData, "");

    // 设置SoftKeybar3
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL);
}

bool CDlgConnectBase::OnUIAction(const QString & strAction)
{
    TALKUI_INFO("ConnectUI-OnUIAction(%s)", strAction.toUtf8().data());
    if (strAction == TALK_UI_ACTION_CANCEL)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_TRANSFER)
    {
        SendUIRequestAction(TALK_ACTION_TRAN);
    }
#if IF_BUG_23548
    else if (strAction == TALK_UI_ACTION_CALL_BACK)
    {
        SendUIRequestAction(TALK_ACTION_CALL_BACK);
    }
#endif
    else if (strAction == TALK_UI_ACTION_MUTE_VOICE)
    {
        //mute事件
        SendUIRequestAction(TALK_ACTION_MUTE);
    }
    else
    {
        return CDlgNormalTalkBase::OnUIAction(strAction);
    }

    return true;
}

//是否显示加密图标
bool CDlgConnectBase::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return false;
}

//void CDlgConnectUI::AdjustLayout()
//{
//  if(NULL != m_pTalkWidget)
//  {
//      QRect rectWidget(0, 0, this->width(), this->height());
//      m_pTalkWidget->SetWidgetArea(rectWidget);
//  }
//  //if (NULL != m_pPicAnimation)
//  //{
//  //  m_pPicAnimation->setGeometry((this->width() - RECT_CONNECT_ANIMATION.width()) / 2 + 8,
//  //                               this->height() - RECT_CONNECT_ANIMATION.height() - 25,
//  //                               RECT_CONNECT_ANIMATION.width(),
//  //                               RECT_CONNECT_ANIMATION.height());
//  //}
//}

bool CDlgConnectBase::IsShowTip()
{
    return true;
}

// 获取配置softkey的文件名和结点名
void CDlgConnectBase::GetCustomSoftkeyData(yl::string & strFile, yl::string & strStatus)
{
    switch (m_eSessionState)
    {
    case SESSION_RINGBACK:
    case SESSION_RINGBACK_TRAN:
        {
            strFile = "RingBack.xml";

            if (m_eSessionState == SESSION_RINGBACK)
            {
                strStatus = "RingBack";
            }
            else
            {
                strStatus = "SemiAttendTransBack";
            }
        }
        break;
    case SESSION_CONNECTING:
    case SESSION_CONNECTING_TRAN:
    case SESSION_NETWORKCONF_CONNECT:
    case SESSION_BLACONNECTING:
        {
            strFile = "Connecting.xml";

            if (m_eSessionState == SESSION_CONNECTING_TRAN)
            {
                strStatus = "SemiAttendTrans";
            }
            else
            {
                strStatus = "Connecting";
            }
        }
        break;
    default:
        break;
    }
}

//事件过滤器
bool CDlgConnectBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    //判断所传入事件响应对象是否为本窗口的子窗口或父窗口

    return false;
}

//通道键处理
bool CDlgConnectBase::OnChannelKeyPress(int iKey)
{
    return CDlgNormalTalkBase::OnChannelKeyPress(iKey);
}

// 功能键处理
bool CDlgConnectBase::OnFunckeyPress(int iKey, bool bEXP)
{
    switch (iKey)
    {
    case PHONE_KEY_TRANSFER:
        {
            //Transfer按键
            OnUIAction(TALK_UI_ACTION_TRANSFER);
            return true;
        }
        break;
    //拨号mute需求
    case PHONE_KEY_MUTE:
        {
            //Mute按键 如果持续mute开启则该处不响应
            if (configParser_GetConfigInt(kszMuteMode) != 1)
            {
                OnUIAction(TALK_UI_ACTION_MUTE_VOICE);
            }

            return true;
        }
    default:
        break;
    }

    return CDlgNormalTalkBase::OnFunckeyPress(iKey, bEXP);
}

// 数字键处理
bool CDlgConnectBase::OnDigitkeyPress(int iKey)
{
    SendUIRequestAction(TALK_ACTION_DTMF, iKey);

    return true;
}

