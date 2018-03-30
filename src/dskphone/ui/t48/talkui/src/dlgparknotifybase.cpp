// mod-this(TalkUI)
#include "dlgparknotifybase.h"
#include "contactinfoframe.h"
#include "commonwidget.h"

// mod-UIKernel
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

// mod-CallPark
#include "broadsoft/callpark/include/callparkcommon.h"

// mod-CommonUnits
#include "commonunits/modcommonunits.h"

// mod-MainWnd
#include "uimanager/rectdefine.h"

#include "imagemanager/modimagemanager.h"
#include "devicelib/networkserver.h"

// mod-StyleUI
#ifdef _T4X
#include "baseui/t4xpicpath.h"
#else
#include "T3XStyleUI/T3XPicPath.h"
#endif
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"

#include "talkuidelegate.h"
#include "voice/include/modvoice.h"

CDlgParkNotifyBase::CDlgParkNotifyBase()
    : CDlgNormalTalkBase(DLG_ParkNotifyUI, TALK_UI_PARK_NOTIFY)
{
    // 初始化控件
    InitData();
}

CDlgParkNotifyBase::~CDlgParkNotifyBase()
{

}

// 初始化
void CDlgParkNotifyBase::Init()
{
    CDlgNormalTalkBase::Init();

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgParkNotifyBase::Uninit()
{
    CDlgNormalTalkBase::Uninit();
}

void CDlgParkNotifyBase::InitData()
{

}

void CDlgParkNotifyBase::GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData,
        DataLogic2UIBase * pData/* = NULL*/)
{
#ifdef _T48
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RETRIEVE_PARK);
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL, LANG_TRANSLATE(TRID_EXIT));
#else
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_RETRIEVE_PARK);
    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, "");
    AddSoftkey(objSoftkeyData, TALK_UI_ACTION_CANCEL, LANG_TRANSLATE(TRID_EXIT));
#endif
}

//获取通话时间内容
QCustomTime CDlgParkNotifyBase::GetTalkTime()
{
    return CDlgNormalTalkBase::GetTalkTime();
}

//设置通话数据内容
void CDlgParkNotifyBase::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgNormalTalkBase::SetTalkData(pData);


    yl::string strTitle = yl::string(TRID_CALL_PARK);
    _TalkUIDelegate.UpdateTitleInfo(strTitle);

}

// 事件过滤
bool CDlgParkNotifyBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    return false;
}

// 通道键处理
bool CDlgParkNotifyBase::OnChannelKeyPress(int iKey)
{
    BOOL bHandled = FALSE;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
    case PHONE_KEY_EHS_OPEN:
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            //  http://10.2.1.199/Bug.php?BugID=36135
            // park notify界面按免提不需要去取回通话，而是要进入预拨号界面
            bHandled = TRUE;
            talklogic_EnterDialUI("", AUTO_SELECT_LINE_ID);
            voice_SwitchChannel();
        }
        break;
    default:
        break;
    }

    if (!bHandled)
    {
        bHandled = CDlgNormalTalkBase::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

// 功能键处理
bool CDlgParkNotifyBase::OnFunckeyPress(int iKey, bool bEXP)
{
    switch (iKey)
    {
    case PHONE_KEY_OK:
        {
            OnUIAction(TALK_UI_ACTION_OK);
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

    return CDlgNormalTalkBase::OnFunckeyPress(iKey, bEXP);
}

//数字键处理
bool CDlgParkNotifyBase::OnDigitkeyPress(int iKey)
{
    return false;
}

//处理UI事件
bool CDlgParkNotifyBase::OnUIAction(const QString & strAction)
{
    if (strAction == TALK_UI_ACTION_RETRIEVE_PARK
            || strAction == TALK_UI_ACTION_OK)
    {
        SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK);
    }
    else if (strAction == TALK_UI_ACTION_CANCEL)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else
    {
        chWARNING2(false, "CDlgParkNotifyUI OnUIAction string error!");
        return false;
    }

    return true;
}
