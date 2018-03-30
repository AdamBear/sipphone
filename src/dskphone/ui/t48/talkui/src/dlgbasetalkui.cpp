//mod-base
#include "dlgbasetalkui.h"
#include "qtcommon/qmisc.h"

//mod-VoiceChannel
#include "voice/include/modvoice.h"

//mod-Talk
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
// #include "talk/talklogic_inc.h"
// #include "talk/talklogicmanager.h"
#include "contactinfoframe.h"

//mod-UIKernel
#include "uikernel/qsoftkeybar.h"
#include "uikernel/languagehelper.h"

//mod-this(TalkUI)
#include "talkuimanager.h"
#include "talkuidelegate.h"

//mod-InputMethod
#include "inputmethod/inputmethod.h"

#include "xmlparser/xmlparser.hpp"
// mod-keyGuard
#include "keyguard/include/modkeyguard.h"

// mod-Menu
#include "setting/include/modmenu.h"

#include "uimanager/rectdefine.h"
#include "baseui/t4xpicpath.h"

#include "directoryui/include/moddirectoryui.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "uicommon/messagebox/optionmessagebox.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "talkuicommon.h"


#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "commonfunction.h"
#include "record/include/modrecord.h"
#endif

#include "voice/include/modvoice.h"

// CDlgBaseTalkUI
// 两个swap的间隔时间
#define TALK_ACTION_SWAP_INTERVAL 600
#define SECURITY_MSGBOX_EXIT_TIME 60000
// 联系人信息控件位置
const QRect RECT_CONNECT_INFO(6, 0, RECT_TALK_FRAME.width() - 8, RECT_TALK_FRAME.height());

#ifndef _T2X

bool CDlgBaseTalkUI::m_bLockSwap = false;

#endif

//构造函数
CDlgBaseTalkUI::CDlgBaseTalkUI(const yl::string & strTalkUIName, TALK_UI_TYPE eTalkUIType)
    : m_strTalkUIName(strTalkUIName)
    , m_eTalkUIType(eTalkUIType)
    , m_iSessionID(-1)
    , m_nCallID(-1)
//#ifdef _T48
//, m_pConInfoFrame(NULL)
//#endif
    , m_eSessionState(SESSION_UNKNOWN)
    , m_bLoadContactPhoto(false)
{
    //设置TalkWidget 指针

//#ifdef _T48
//  m_pConInfoFrame = new CContactInfoFrame(this);
//  if (NULL != m_pConInfoFrame)
//  {
//      m_pConInfoFrame->setGeometry(RECT_CONNECT_INFO);
//  }
//#else
//
//#endif
}

CDlgBaseTalkUI::~CDlgBaseTalkUI(void)
{
    m_strTalkUIName = "";
    m_eSessionState = SESSION_UNKNOWN;
    m_eTalkUIType = TALK_UI_UNKNOWN;
    m_iSessionID = -1;
    m_nCallID = -1;
}

// 初始化
void CDlgBaseTalkUI::Init()
{
    m_iSessionID = -1;
    m_eSessionState = SESSION_UNKNOWN;
    m_vecContactCustomImage.clear();
    UnlockSwap();

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgBaseTalkUI::Uninit()
{
    m_iSessionID = -1;
    m_eSessionState = SESSION_UNKNOWN;
    UnlockSwap();

//#ifdef _T48
//  if (NULL != m_pConInfoFrame)
//  {
//      m_pConInfoFrame->Clear();
//  }
//#endif

}

void CDlgBaseTalkUI::SetLogicData(DataLogic2UI & objData)
{
    //这边设置 SessionState
    SetSessionState(objData.eSessionState);

    //然后 UI 设置数据
    SetTalkData(objData.pData);

    //在这边 获取softkey
    GetSoftkeyData(m_SoftkeyData, objData.pData);
}

void CDlgBaseTalkUI::SetTalkData(DataLogic2UIBase * pData)
{

}

//通道键处理
bool CDlgBaseTalkUI::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

    if (IsChannelKeyToExit(iKey))
    {
        bHandled = true;
        SendUIRequestAction(TALK_ACTION_CANCEL, 1, 0);

        // 如果由通道键引发的退出，且还有通话存在，则切换通道
        if (talklogic_SessionExist())
        {
            voice_SwitchChannel();
        }
    }
    else
    {
        // 呼叫中心模式，免提模式下按免提键可以切换成耳麦通道
        if (voice_IsCallCenterMode() && iKey == PHONE_KEY_HANDFREE)
        {
            if (!voice_GetHandfreeStatus())
            {
                if (voice_GetHandsetStatus())
                {
                    voice_SwitchChannel(CHANNEL_2HEADSET);
                }
                else
                {
                    voice_SwitchChannel(CHANNEL_HEADSET);
                }
                bHandled = true;
            }
        }
    }

    //处理完后更新一下通道图标  统一由外部去更新
    //  _TalkUIManager.UpdateChannelIcon();

    return bHandled;

}

//功能键处理
bool CDlgBaseTalkUI::OnFunckeyPress(int iKey, bool bEXP)
{
    switch (iKey)
    {
    case PHONE_KEY_CANCEL:
        {
            //Cancel按键
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
bool CDlgBaseTalkUI::OnDigitkeyPress(int iKey)
{
    return false;
}

//按键弹起处理
bool CDlgBaseTalkUI::OnKeyRelease(int iKey)
{
    return false;
}

// softkey处理  GetSoftKeyActionByType统一转换成string来处理
bool CDlgBaseTalkUI::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    //转换成字符串类型的softkey来处理
    //yl::string v_strSoftkey = GetSoftKeyActionByType(eSoftkey);
    yl::string v_strSoftkey = "";

    return OnUIAction(toQString(v_strSoftkey));

}

//获取TalkUI名称
yl::string CDlgBaseTalkUI::GetTalkUIName()
{
    return m_strTalkUIName;
}

//设置TalkUI名称
void CDlgBaseTalkUI::SetTalkUIName(yl::string strTalkUIName)
{
    m_strTalkUIName = strTalkUIName;
}

//设置关联的SessionId
void CDlgBaseTalkUI::SetSessionId(int iSessionId)
{
    m_iSessionID = iSessionId;
}

//获取关联的SessionId
int CDlgBaseTalkUI::GetSessionId()
{
    return m_iSessionID;
}

//获取通话UI类型
TALK_UI_TYPE CDlgBaseTalkUI::GetTalkUIType() const
{
    return m_eTalkUIType;
}

#ifdef _T48
void CDlgBaseTalkUI::SetSoftkeyIcon(CArraySoftKeyBarData & objSoftkeyData)
{
    for (int i = 0; i < objSoftkeyData.size(); ++i)
    {
        objSoftkeyData[i].m_strIconPath = GetSoftkeyIconByAction(objSoftkeyData[i].m_strSoftkeyAction);
    }
}
#endif

void CDlgBaseTalkUI::BindSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData)
{
#ifdef _T48
    // 设置softkey的Icon
    SetSoftkeyIcon(objSoftkeyData);
#endif

    for (int nIndex = 0; nIndex < objSoftkeyData.size(); ++nIndex)
    {
        //操作不为空，才添加响应函数
        if (objSoftkeyData[nIndex].m_strSoftkeyAction != "")
        {
            //添加热键栏的响应槽函数
            objSoftkeyData[nIndex].m_pReceiver = this;
            objSoftkeyData[nIndex].m_strSlotName = SLOT(OnSoftkeyClicked(qSoftButton *));
        }
        else
        {
            //清空热键栏的响应槽函数
            objSoftkeyData[nIndex].m_pReceiver = NULL;
            objSoftkeyData[nIndex].m_strSlotName = "";
        }
    }
}

void CDlgBaseTalkUI::SetSessionState(SESSION_STATE eSessionState)
{
    m_eSessionState = eSessionState;
}

//显示security的弹出框
void CDlgBaseTalkUI::ShowSecurityBox(int nAccount)
{
    YLList<yl::string> lstAllowLevels;
    talklogic_GetAccountAllowSecurityLevels(lstAllowLevels, nAccount);

    if (lstAllowLevels.size() == 0)
    {
        return;
    }

    vecOptionMsgBoxItem vecOption;
    vecOption.clear();
    OptionMsgBoxItem optionItem;

    yl::string strLevel = talklogic_GetAccountOverrideLevel(nAccount);

    YLList<yl::string>::ListIterator iter = lstAllowLevels.begin();
    int nSelIndex = -1;
    for (int nIndex = 0; iter != lstAllowLevels.end(); ++iter, ++nIndex)
    {
        QString strDisp = toQString(*iter);
        optionItem.m_strAction = strDisp;
        strDisp = LANG_TRANSLATE(strDisp);
        if (strLevel == (*iter))
        {
            strDisp += LANG_TRANSLATE(TRID_ASSIGNED);
            nSelIndex = nIndex;
        }
        optionItem.m_strTitle = strDisp;

        vecOption.push_back(optionItem);
    }

    MessageBox_ShowOptionBox(this, vecOption, MSGBOX_ID_SECURITY, QVariant(), true, nSelIndex,
                             SECURITY_MSGBOX_EXIT_TIME);
}

//设置Softkey数据Action值
void CDlgBaseTalkUI::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData,
                                    DataLogic2UIBase * pData /* = NULL */)
{
    // 清空softkey数据
    objSoftkeyData.clear();

    // 先获取配置的softkey
    bool bUseCustomSoftkey = false;
    if (IsAllowDefineSoftkey()
            && (bool)configParser_GetConfigInt(kszCustomSoftkeySwitch))
    {
        yl::string strFile;
        yl::string strStatus;
        GetCustomSoftkeyData(strFile, strStatus);
        bUseCustomSoftkey = LoadCustomSoftkey(objSoftkeyData, strFile, strStatus);
    }

#if IF_FEATURE_CUSTOM_SOFTKEY
    if (bUseCustomSoftkey
            && configParser_GetConfigInt(kszCustomDynamicSoftkey) == 1)
    {
        CArraySoftKeyBarData tmpSoftkeyData;
        GetDynamicSoftkey(tmpSoftkeyData, pData);
        InsertSoftKey(objSoftkeyData, tmpSoftkeyData);
    }
    AdjustCustomSoftkey(objSoftkeyData, pData);
#endif

    //从子窗口获取softkey
    if (!bUseCustomSoftkey)
    {
        GetSoftkeyAction(objSoftkeyData, pData);
    }

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    AddUSBRecordSoftkey(objSoftkeyData);
#endif

    BindSoftkeyAction(objSoftkeyData);
}

#if IF_FEATURE_CUSTOM_SOFTKEY
void CDlgBaseTalkUI::InsertSoftKey(CArraySoftKeyBarData & oldSoftkeyData,
                                   CArraySoftKeyBarData & newSoftkeyData)
{
    // 若不存在该Softkey则插入到Softkey末尾
    for (int i = 0; i < newSoftkeyData.size(); i++)
    {
        bool bExist = false;
        for (int j = 0; j < oldSoftkeyData.size(); j++)
        {
            if (newSoftkeyData[i].m_strSoftkeyAction == oldSoftkeyData[j].m_strSoftkeyAction)
            {
                bExist = true;
                break;
            }
        }

        if (!bExist)
        {
            oldSoftkeyData.push_back(newSoftkeyData[i]);
        }
    }
}

bool CDlgBaseTalkUI::IsSoftkeyAllowToShow(const yl::string & strSoftkeyAction,
        DataLogic2UIBase * pData/* = NULL*/)
{
    //http://bugfree.yealink.com/Bug.php?BugID=134148 之前遗漏了对TALK_UI_ACTION_SWITCH的判断
    if (TALK_UI_ACTION_SWAP == strSoftkeyAction || TALK_UI_ACTION_SWITCH == strSoftkeyAction)
    {
        if (talklogic_GetAmountOfSession() <= 1)
        {
            return false;
        }
    }
    else if (TALK_UI_ACTION_NEWCALL == strSoftkeyAction && !talklogic_BeAllowNewCall())
    {
        return false;
    }
    else if (TALK_UI_ACTION_REJECT == strSoftkeyAction
             && 0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
    {
        return false;
    }

    return true;
}

void CDlgBaseTalkUI::AdjustCustomSoftkey(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData)
{
    if (objSoftkeyData.empty())
    {
        return;
    }
    CArraySoftKeyBarData::iterator it = objSoftkeyData.begin();
    for (; it != objSoftkeyData.end();)
    {
        if (!IsSoftkeyAllowToShow(it->m_strSoftkeyAction, pData))
        {
            // erase的返回值是删除元素下一个元素的迭代器
            it = objSoftkeyData.erase(it);
        }
        else
        {
            it++;
        }
    }
}
#endif

// 根据CallInfo获取record图标类型
ICON_TYPE CDlgBaseTalkUI::GetRecordIconType(CCallInfo * pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return IT_UNKNOWN;
    }

    ICON_TYPE eRecordIcon = IT_UNKNOWN;
#if IF_BUG_22457_Record
    if (pCallInfo->IsRecordOn(INF_RECORD_SIP)
            || pCallInfo->m_eSIPRecordStatus != SIP_RECORD_STATUS_UNKNOWN)
    {
        switch (pCallInfo->m_eSIPRecordStatus)
        {
        case SIP_RECORD_STATUS_OFF_FAILED:
            eRecordIcon = IT_CANNOT_BE_STOPPED;
            break;
        case SIP_RECORD_STATUS_OFF_SUCCESS:
            eRecordIcon = IT_UNKNOWN;
            break;
        case SIP_RECORD_STATUS_ON_FAILED:
            eRecordIcon = IT_CANNOT_BE_STARTED;
            break;
        case SIP_RECORD_STATUS_ON_SUCCESS:
            eRecordIcon = IT_RECORD;
            break;
        default:
            eRecordIcon = IT_UNKNOWN;
            break;
        }
    }
#else
    if (pCallInfo->IsRecordOn(INF_RECORD_SIP | INF_RECORD_VOICE))
    {
        eRecordIcon = IT_RECORD;
    }
#endif
    if (pCallInfo->IsRecordOn(INF_RECORD_URL)
            || pCallInfo->m_eURLRecordError != URE_NONE)
    {
        switch (pCallInfo->m_eURLRecordError)
        {
        case URE_NONE:
            eRecordIcon = IT_RECORD;
            break;
        case URE_CANNOT_BE_RECORDED:
            eRecordIcon = IT_CANNOT_BE_RECORDED;
            break;
        case URE_CANNOT_BE_STOPPED:
            eRecordIcon = IT_CANNOT_BE_STOPPED;
            break;
        case URE_CANNOT_BE_STARTED:
            eRecordIcon = IT_CANNOT_BE_STARTED;
            break;
        case URE_FULL:
            eRecordIcon = IT_RECORD_FULL;
            break;
        default:
            break;
        }
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (pCallInfo->m_eRecordStatus != CALL_RECORD_UNKNOWN)
    {
        switch (pCallInfo->m_eRecordStatus)
        {
        case CALL_RECORD_NORMAL:
            eRecordIcon = IT_RECORD_NORMAL;
            break;
        case CALL_RECORD_START:
            eRecordIcon = IT_RECORD;
            break;
        case CALL_RECORD_PAUSE:
            eRecordIcon = IT_RECORD_PAUSE;
            break;
        default:
            break;
        }
    }
#endif

    return eRecordIcon;
}

// 焦点切换
void CDlgBaseTalkUI::OnFocusChanged(bool bFocus)
{

}

void CDlgBaseTalkUI::OnTipChange(TALK_TIP_TYPE eTipType)
{
    // None
}

bool CDlgBaseTalkUI::GetLoadCustomImage(int nIndex)
{
    int nSize = m_vecContactCustomImage.size();
    if (nSize < 1)
    {
        return false;
    }

    //当配置为2的时候如果存在一个头像是显示自定义头像的则代表有显示头像的布局
    bool bResult = false;
    for (int nIndex = 0; nIndex < nSize; ++nIndex)
    {
        if (m_vecContactCustomImage.at(nIndex))
        {
            bResult = true;
        }
    }

    return bResult;
}

#ifdef _T48
void CDlgBaseTalkUI::SetTalkUIRect(const QRect & rect)
{
    setGeometry(rect);

    AdjustLayout();
}
#endif

//设置通话UI类型
void CDlgBaseTalkUI::SetTalkUIType(TALK_UI_TYPE eTalkUIType)
{
    m_eTalkUIType = eTalkUIType;
}

bool CDlgBaseTalkUI::IsExistBySoftkeyArrayData(CArraySoftKeyBarData & objSoftkeyData, \
        const yl::string & strTalkUIAction, \
        const QString & strTitle)
{
    bool bResult = false;

    CArraySoftKeyBarData::iterator it = objSoftkeyData.begin();
    for (; it != objSoftkeyData.end(); ++it)
    {
        if (it->m_strSoftkeyAction == strTalkUIAction && \
                it->m_strSoftkeyTitle == strTitle)
        {
            return true;
        }
    }

    return bResult;
}

// 添加特定类型的softkey

bool CDlgBaseTalkUI::IsDisplayContactPhoto(int nIndex/* = 0*/)
{
    if (configParser_GetConfigInt(kszDisplayContactPhoto) == 1)
    {
        return true;
    }
    else if (configParser_GetConfigInt(kszDisplayContactPhoto) == 2)
    {
        return GetLoadCustomImage(nIndex);
    }

    return false;
}

//是否存在显示的头像
bool CDlgBaseTalkUI::HasDisplayContactImage()
{
    return IsDisplayContactPhoto(0);
}

// 添加特定类型的softkey
void CDlgBaseTalkUI::AddSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                                const yl::string & strTalkUIAction,
                                const QString & strTitle/* = ""*/)
{
    if (IsExistBySoftkeyArrayData(objSoftkeyData, strTalkUIAction, strTitle))
    {
        return;
    }

    //这个 不能不设置  如果不设置就少了一个softkey了  有些softkeybar的布局 是需要有空的
    //当strTalkUIAction为空时不设置

#ifdef _T48
    if (strTalkUIAction.empty())
    {
        return;
    }
#endif

    // 拼装softkey数据
    CSoftKeyBarData objData;
    objData.m_strSoftkeyAction = strTalkUIAction;

    if (!strTalkUIAction.empty())
    {
        objData.m_pReceiver = this;
        objData.m_strSlotName = SLOT(OnSoftKeyAction(qSoftButton *));
    }
    else
    {
        objData.m_pReceiver = NULL;
    }

    // 设置显示的Title
    if (strTitle.isEmpty())
    {
        objData.m_strSoftkeyTitle  = SoftkeyActionToTitle(strTalkUIAction);
    }
    else
    {
        objData.m_strSoftkeyTitle = strTitle;
    }

    // 添加softkey数据
    objSoftkeyData.push_back(objData);
}

#ifdef _T48
const char * CDlgBaseTalkUI::GetSoftkeyIconByAction(const yl::string & strTalkUIAction)
{
    if (strTalkUIAction == TALK_UI_ACTION_SECURITY)
    {
        return PIC_SOFTKEY_SECURITY;
    }
    if (strTalkUIAction == TALK_UI_ACTION_IME)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_DELETE)
    {
        return PIC_SOFTKEY_DELETE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_LINE)
    {
        return "Line";
    }
    if (strTalkUIAction == TALK_UI_ACTION_POOL)
    {
        return PIC_SOFTKEY_DIRECTORY;
    }
    if (strTalkUIAction == TALK_UI_ACTION_TEXT_CHANGE)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_SEND)
    {
        return PIC_SOFTKEY_SEND;
    }
    if (strTalkUIAction == TALK_UI_ACTION_REDIAL)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_CANCEL)
    {
        return PIC_SOFTKEY_CANCEL;
    }
    if (strTalkUIAction == TALK_UI_ACTION_ENDCALL)
    {
        return PIC_SOFTKEY_ENDCALL;
    }
    if (strTalkUIAction == TALK_UI_ACTION_NEWCALL)
    {
        return PIC_SOFTKEY_NEWCALL;
    }
    if (strTalkUIAction == TALK_UI_ACTION_ANSWER)
    {
        return PIC_SOFTKEY_ANSWER;
    }
    if (strTalkUIAction == TALK_UI_ACTION_REJECT)
    {
        return PIC_SOFTKEY_REJECT;
    }
    if (strTalkUIAction == TALK_UI_ACTION_DECLINE)
    {
        return PIC_SOFTKEY_DECLINE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_DECLINE_IGNORE)
    {
        return PIC_SOFTKEY_REJECT;
    }
    if (strTalkUIAction == TALK_UI_ACTION_IGNORE)
    {
        return PIC_SOFTKEY_SILENCE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_FORWARD)
    {
        return PIC_SOFTKEY_FORWARD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_SWAP)
    {
        return PIC_SOFTKEY_SWAP;
    }
    if (strTalkUIAction == TALK_UI_ACTION_HOLD)
    {
        return PIC_SOFTKEY_HOLD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_RESUME)
    {
        return PIC_SOFTKEY_RESUME;
    }
    if (strTalkUIAction == TALK_UI_ACTION_MUTE_VOICE)
    {
        return PIC_SOFTKEY_TALK_MUTE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_TRANSFER)
    {
        return PIC_SOFTKEY_TRANSFER;
    }
    if (strTalkUIAction == TALK_UI_ACTION_FAIL_TRANSFER)
    {
        return PIC_SOFTKEY_TRANSFER;
    }
    if (strTalkUIAction == TALK_UI_ACTION_CONFERENCE)
    {
        return PIC_SOFTKEY_CONFERENCE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_SPLIT)
    {
        return PIC_SOFTKEY_SPLIT;
    }
    if (strTalkUIAction == TALK_UI_ACTION_REMOVE)
    {
        return "Remove";
    }
    if (strTalkUIAction == TALK_UI_ACTION_AUTOREDIAL)
    {
        return PIC_SOFTKEY_TALK_REDIAL;
    }
    if (strTalkUIAction == TALK_UI_ACTION_PARK)
    {
        return PIC_SOFTKEY_PACK;
    }
    if (strTalkUIAction == TALK_UI_ACTION_GROUP_PARK)
    {
        return PIC_SOFTKEY_GPACK;
    }
    if (strTalkUIAction == TALK_UI_ACTION_RETRIEVE_PARK)
    {
        return PIC_SOFTKEY_RETRIEVE_PACK;
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_CALL_INFO)
    {
        return PIC_SOFTKEY_CALLINFO;
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_DISPCODE)
    {
        return PIC_SOFTKEY_DISPCODE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_TRACE)
    {
        return PIC_SOFTKEY_ACD_TRACE;
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_EMERGESCAL)
    {
        return PIC_SOFTKEY_EMERGENCY;
    }
    if (strTalkUIAction == TALK_UI_ACTION_DPICKUP)
    {
        return PIC_SOFTKEY_DPICKUP;
    }
    if (strTalkUIAction == TALK_UI_ACTION_GPICKUP)
    {
        return PIC_SOFTKEY_GPICKUP;
    }
    if (strTalkUIAction == TALK_UI_ACTION_PRIHOLD)
    {
        return PIC_SOFTKEY_PRIVATE_HOLD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_SWITCH)
    {
        return PIC_SOFTKEY_SWITCH;
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENHISTORY)
    {
        return PIC_SOFTKEY_HISTORY;
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENDIRECTORY)
    {
        return  PIC_SOFTKEY_DIRECTORY;
    }
    if (strTalkUIAction == TALK_UI_ACTION_RTP_STATUS)
    {
        return  PIC_SOFTKEY_RTP_STATUS;
    }
    if (strTalkUIAction == TALK_UI_ACTION_RTP_STATUS_SWITCH)
    {
        return  PIC_SOFTKEY_RTP_STATUS_SWITCH;
    }
#if IF_BUG_23548
    if (strTalkUIAction == TALK_UI_ACTION_CALL_BACK)
    {
        return PIC_SOFTKEY_CALL_BACK;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (strTalkUIAction == TALK_UI_ACTION_RECORD)
    {
        return  PIC_SOFTKEY_RECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_STARTRECORD)
    {
        return  PIC_SOFTKEY_RECORD_STARTRECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_STOPRECORD)
    {
        return  PIC_SOFTKEY_RECORD_STOPRECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_PAUSERECORD)
    {
        return  PIC_SOFTKEY_RECORD_PAUSERECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_RESUMERECORD)
    {
        return  PIC_SOFTKEY_RECORD_RESUMERECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_BACK)
    {
        return  PIC_SOFTKEY_BACK;
    }
#endif
    if (strTalkUIAction == TALK_UI_ACTION_CALL_INITIATION)
    {
        return  PIC_SOFTKEY_INITIATION;
    }
    if (strTalkUIAction == TALK_UI_ACTION_CALL_PUSH)
    {
        return  PIC_SOFTKEY_CALLPUSH;
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (strTalkUIAction == TALK_UI_ACTION_USB_BEGIN_RECORD)
    {
        return  PIC_SOFTKEY_RECORD_STARTRECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_USB_STOP_REDORD)
    {
        return  PIC_SOFTKEY_RECORD_STOPRECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_USB_PAUSE_RECORD)
    {
        return  PIC_SOFTKEY_RECORD_PAUSERECORD;
    }
    if (strTalkUIAction == TALK_UI_ACTION_USB_RESUME_REDORD)
    {
        return  PIC_SOFTKEY_RECORD_RESUMERECORD;
    }
#endif
    if (strTalkUIAction == TALK_UI_ACTION_OPENDOOR)
    {
        return PIC_SOFTKEY_OPENDOOR;
    }
    return "";
}
#endif

bool CDlgBaseTalkUI::LoadCustomSoftkey(CArraySoftKeyBarData & objSoftkeyData,
                                       const yl::string & strFile,
                                       const yl::string & strStatus)
{
    // 读取配置文件
    xml_document xFile;
    //!!!!! 文件存放路径待定
    yl::string strPath = CONFIG_PATH"softkey/" + strFile;
    xml_parse_result objResult = xFile.load_file(strPath.c_str());
    if (objResult.status != status_ok)
    {
        TALKUI_INFO("CDlgBaseTalkUI::LoadSoftkey load file[%s] error", strFile.c_str());
        return false;
    }

    xml_node nodeXmlNode = xFile.child(strStatus.c_str());

    yl::string strNewFile = strFile;
    yl::string strNewStatus = strStatus;

    if (nodeXmlNode.empty())
    {
        TALKUI_INFO("CDlgBaseTalkUI::LoadSoftkey load status[%s] error", strNewStatus.c_str());
#if IF_FEATURE_CUSTOM_SOFTKEY
        if (commonUnits_GetBackupSoftkey(strNewFile, strNewStatus))
        {
            strPath = CONFIG_PATH"softkey/" + strNewFile;
            objResult = xFile.load_file(strPath.c_str());

            if (objResult.status != status_ok)
            {
                TALKUI_INFO("CDlgBaseTalkUI::LoadSoftkey load file[%s] error", strNewFile.c_str());
                return false;
            }

            nodeXmlNode = xFile.child(strNewStatus.c_str());
            if (nodeXmlNode.empty())
            {
                return false;
            }
        }
        else
#endif
        {
            return false;
        }
    }

#if IF_FEATURE_CUSTOM_SOFTKEY
    TALKUI_INFO("CDlgBaseTalkUI::LoadCustomSoftkey strFile[%s] strStatus[%s]", strNewFile.c_str(),
                strNewStatus.c_str());
    if (!commonUnits_IsSoftkeyEnable(strNewFile, strNewStatus))
    {
        return false;
    }
#endif

    nodeXmlNode = nodeXmlNode.child("Enable");
    if (nodeXmlNode.empty())
    {
        chWARNING2(false, "CDlgBaseTalkUI::LoadSoftkey load enable status error");
        return false;
    }

    if (!nodeXmlNode.empty())
    {
        xml_node nodeKeyNode = nodeXmlNode.child("Key");
        while (!nodeKeyNode.empty())
        {
            AddSoftkey(objSoftkeyData, SoftkeyTypeToAction(nodeKeyNode.attribute("Type").value()));
            nodeKeyNode = nodeKeyNode.next_sibling();
        }

        // 如果配置的softkey为空则插入空的softkey
        if (objSoftkeyData.empty())
        {
            for (int i = 0; i != SOFTKEY_NUM; ++i)
            {
                AddSoftkey(objSoftkeyData, "");
            }
        }
    }

    return true;
}

const char * CDlgBaseTalkUI::SoftkeyTypeToAction(const yl::string & strType)
{
    TALKUI_INFO("CDlgBaseTalkUI::SoftkeyTypeToAction(%s)", strType.c_str());

    if (strType == "Send")
    {
        return TALK_UI_ACTION_SEND;
    }
    else if (strType == "IME")
    {
        return TALK_UI_ACTION_IME;
    }
    else if (strType == "Delete")
    {
        return TALK_UI_ACTION_DELETE;
    }
    else if (strType == "Line")
    {
        return TALK_UI_ACTION_LINE;
    }
    else if (strType == "Cancel")
    {
        return TALK_UI_ACTION_CANCEL;
    }
    else if (strType == "End Call")
    {
        return TALK_UI_ACTION_ENDCALL;
    }
    else if (strType == "Answer")
    {
        return TALK_UI_ACTION_ANSWER;
    }
    else if (strType == "Forward")
    {
        return TALK_UI_ACTION_FORWARD;
    }
    else if (strType == "Reject")
    {
        return TALK_UI_ACTION_REJECT;
    }
    else if (strType == "Transfer")
    {
        return TALK_UI_ACTION_TRANSFER;
    }
    else if (strType == "NewCall")
    {
        return TALK_UI_ACTION_NEWCALL;
    }
    else if (strType == "Hold")
    {
        return TALK_UI_ACTION_HOLD;
    }
    else if (strType == "Conference")
    {
        return TALK_UI_ACTION_CONFERENCE;
    }
    else if (strType == "Resume")
    {
        return TALK_UI_ACTION_RESUME;
    }
    else if (strType == "Split")
    {
        return TALK_UI_ACTION_SPLIT;
    }
    else if (strType == "Directories")
    {
        return TALK_UI_ACTION_POOL;
    }
    else if (strType == "Park")
    {
        return TALK_UI_ACTION_PARK;
    }
    else if (strType == "GPark")
    {
        return TALK_UI_ACTION_GROUP_PARK;
    }
    else if (strType == "Retrieve")
    {
        return TALK_UI_ACTION_RETRIEVE_PARK;
    }
    else if (strType == "DPickup")
    {
        return TALK_UI_ACTION_DPICKUP;
    }
    else if (strType == "GPickup")
    {
        return TALK_UI_ACTION_GPICKUP;
    }
    else if (strType == "Mute")
    {
        return TALK_UI_ACTION_MUTE_VOICE;
    }
    else if (strType == "History")
    {
        return TALK_UI_ACTION_OPENHISTORY;
    }
    else if (strType == "Directory")
    {
        return TALK_UI_ACTION_OPENDIRECTORY;
    }
    else if (strType == "Favorite")
    {
        return TALK_UI_ACTION_POOL;
    }
    else if (strType == "Directories")
    {
        return TALK_UI_ACTION_POOL;
    }
    else if (strType == "Silence")
    {
        return TALK_UI_ACTION_IGNORE;
    }
    else if (strType == "SWAP")
    {
        return TALK_UI_ACTION_SWAP;
    }
    else if (strType == "Switch")
    {
        return TALK_UI_ACTION_SWITCH;
    }
    else if (strType == "PriHold")
    {
        return TALK_UI_ACTION_PRIHOLD;
    }
    else if (strType == "Manager")
    {
        return TALK_UI_ACTION_CONF_MGR;
    }
    else if (strType == "RTP Status")
    {
        return TALK_UI_ACTION_RTP_STATUS;
    }
    else if (strType == "Security")
    {
        return TALK_UI_ACTION_SECURITY;
    }
#if IF_BUG_23548
    else if (strType == "Call Back")
    {
        return TALK_UI_ACTION_CALL_BACK;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    else if (strType == "Record")
    {
        return TALK_UI_ACTION_RECORD;
    }
#endif
    else if (strType == "Call Initiation")
    {
        return TALK_UI_ACTION_CALL_INITIATION;
    }
    else if (strType == "Call Push")
    {
        return TALK_UI_ACTION_CALL_PUSH;
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    else if (strType == "beginrecord")
    {
        return TALK_UI_ACTION_USB_BEGIN_RECORD;
    }
    else if (strType == "stoprecord")
    {
        return TALK_UI_ACTION_USB_STOP_REDORD;
    }
    else if (strType == "pauserecord")
    {
        return TALK_UI_ACTION_USB_PAUSE_RECORD;
    }
    else if (strType == "resumerecord")
    {
        return TALK_UI_ACTION_USB_RESUME_REDORD;
    }
#endif
    else if (strType == "Open Door")
    {
        return TALK_UI_ACTION_OPENDOOR;
    }
    else if (strType == "Decline")
    {
        return TALK_UI_ACTION_DECLINE;
    }
    else
    {
        return "";
    }

    //!!!!! 待确认
    //  else if (strType == "Silence")      return ST_SILENCE;
    //  else if (strType == "SWAP")         return ST_SWAP;
    //  else if (strType == "Ok")           return ST_OK;
    //  else if (strType == "Switch")       return ST_SWITCH;
    //  else if (strType == "CC")           return ST_CCOMPLETION;
    //  else if (strType == "Delete All")   return ST_DELETEALL;
    //  else if (strType == "Login")        return ST_ACD_LOGIN;
    //  else if (strType == "Logout")       return ST_ACD_LOGOUT;
    //  else if (strType == "Avail")        return ST_ACD_AVAILABLE;
    //  else if (strType == "Unavail")      return ST_ACD_UNAVAILABLE;
    //  else if (strType == "GuestIn")      return ST_HOTELING_GUESTIN;
    //  else if (strType == "GuestOut")     return ST_HOTELING_GUESTOUT;
    //  else if (strType == "Disp Code")    return ST_ACD_DISPCODE;
    //  else if (strType == "Trace")        return ST_ACD_TRACE;
    //  else if (strType == "Call Info")    return ST_ACD_CALLINFO;
    //  else if (strType == "Emergcy")      return ST_ACD_EMERGENCY;
    //  else if (strType == "Favorite")     return ST_FAVORITE;
    //  else if (strType == "PriHold")      return ST_PRIVATEHOLD;
    //  else if (strType == "Back")         return ST_BACK;
    //  else if (strType == "Enter")        return ST_ENTER;
    //  else if (strType == "Select")       return ST_SELECT;
    //  else if (strType == "Exit")         return ST_EXIT;
    //  else if (strType == "Connect")      return ST_CONNECT;
    //  else if (strType == "View")         return ST_VIEW;
    //  else if (strType == "Confirm")      return ST_CONFIRM;
    //  else if (strType == "Save")         return ST_SAVE;
    //  else if (strType == "DPickup")      return ST_DPICKUP;
    //  else if (strType == "GPickup")      return ST_GPICKUP;
    //  else if (strType == "Dial")         return ST_DIAL;
    //  else if (strType == "Barge In")     return ST_BARGEIN;
    //  else if (strType == "Block")        return ST_BLOCK;
    //  else if (strType == "Pickup")       return ST_PICKUP;
    //  else if (strType == "Add")          return ST_ADD;
    //  else if (strType == "Remove")       return ST_REMOVE;
    //  else if (strType == "PriChat")      return ST_PRI_CHAT;
    //  else if (strType == "ExitPri")      return ST_EXIT_PRICHAT;
}

QString CDlgBaseTalkUI::SoftkeyActionToTitle(const yl::string & strTalkUIAction)
{
    if (strTalkUIAction == TALK_UI_ACTION_IME)
    {
        return qGetCurrentIMETitle(focusWidget());
    }
    if (strTalkUIAction == TALK_UI_ACTION_DELETE)
    {
        return LANG_TRANSLATE(TRID_DELETE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_LINE)
    {
        return LANG_TRANSLATE(TRID_LINE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_POOL)
    {
        return LANG_TRANSLATE(TRID_FAVOURITE_DIRECTORIES);
    }
    if (strTalkUIAction == TALK_UI_ACTION_TEXT_CHANGE)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_SEND)
    {
        return LANG_TRANSLATE(TRID_SEND);
    }
    if (strTalkUIAction == TALK_UI_ACTION_REDIAL)
    {
        return LANG_TRANSLATE(TRID_REDIAL);
    }
    if (strTalkUIAction == TALK_UI_ACTION_CANCEL)
    {
        return LANG_TRANSLATE(TRID_CANCEL);
    }
    if (strTalkUIAction == TALK_UI_ACTION_ENDCALL)
    {
        return LANG_TRANSLATE(TRID_END_CALL);
    }
    if (strTalkUIAction == TALK_UI_ACTION_NEWCALL)
    {
        return LANG_TRANSLATE(TRID_NEW_CALL);
    }
    if (strTalkUIAction == TALK_UI_ACTION_ANSWER)
    {
        return LANG_TRANSLATE(TRID_ANSWER);
    }
    if (strTalkUIAction == TALK_UI_ACTION_REJECT)
    {
        return LANG_TRANSLATE(TRID_REJECT);
    }
    if (strTalkUIAction == TALK_UI_ACTION_DECLINE)
    {
        return LANG_TRANSLATE(TRID_DECLINE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_DECLINE_IGNORE)
    {
        return LANG_TRANSLATE(TRID_IGNORE);
    }
#ifdef _VPX
    if (strTalkUIAction == TALK_UI_ACTION_IGNORE)
    {
        return LANG_TRANSLATE(TRID_IGNORE);
    }
#else
    if (strTalkUIAction == TALK_UI_ACTION_IGNORE)
    {
        return LANG_TRANSLATE(TRID_SILENCE);
    }
#endif
    if (strTalkUIAction == TALK_UI_ACTION_FORWARD)
    {
        return LANG_TRANSLATE(TRID_FORWARD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_SECURITY)
    {
        return LANG_TRANSLATE(TRID_SECURITY);
    }
    if (strTalkUIAction == TALK_UI_ACTION_SWAP)
    {
        return LANG_TRANSLATE(TRID_SWAP);
    }
    if (strTalkUIAction == TALK_UI_ACTION_HOLD)
    {
        return LANG_TRANSLATE(TRID_HOLD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_RESUME)
    {
        return LANG_TRANSLATE(TRID_RESUME);
    }
    if (strTalkUIAction == TALK_UI_ACTION_MUTE_VOICE)
    {
        return LANG_TRANSLATE(TRID_MUTE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_TRANSFER)
    {
        return LANG_TRANSLATE(TRID_TRANSFER);
    }
    if (strTalkUIAction == TALK_UI_ACTION_FAIL_TRANSFER)
    {
        return LANG_TRANSLATE(TRID_TRANSFER);
    }
    if (strTalkUIAction == TALK_UI_ACTION_CONFERENCE)
    {
        return LANG_TRANSLATE(TRID_CONFERENCE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_SPLIT)
    {
        return LANG_TRANSLATE(TRID_SPLIT);
    }
    if (strTalkUIAction == TALK_UI_ACTION_REMOVE)
    {
        return LANG_TRANSLATE(TRID_REMOVE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_RESIZE_VIDEO)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_STOP_VIDEO)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_START_VIDEO)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENDOOR)
    {
        return LANG_TRANSLATE(TRID_OPEN_DOOR);
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENFAVORITE)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENHISTORY)
    {
        return LANG_TRANSLATE(TRID_HISTORY);
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENDIRECTORY)
    {
        return LANG_TRANSLATE(TRID_DIRECTORY);
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPEN_REMOTE_DIRECTORY)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPEN_LDAP_DIRECTORY)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPEN_BST_DIRECTORY)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPEN_BST_HISTORY)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_OPENDSSKEY)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_AUTOREDIAL)
    {
        return LANG_TRANSLATE(TRID_OK);
    }
    if (strTalkUIAction == TALK_UI_ACTION_OK)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_SMS)
    {
        return "";
    }
    if (strTalkUIAction == TALK_UI_ACTION_CONF_MGR)
    {
        return LANG_TRANSLATE(TRID_MANAGE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_PARK)
    {
        return LANG_TRANSLATE(TRID_PARK_CALL);
    }
    if (strTalkUIAction == TALK_UI_ACTION_GROUP_PARK)
    {
        return LANG_TRANSLATE(TRID_GROUP_CALL_PARK);
    }
    if (strTalkUIAction == TALK_UI_ACTION_RETRIEVE_PARK)
    {
        return LANG_TRANSLATE(TRID_RETRIEVE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_CALL_INFO)
    {
        return LANG_TRANSLATE(TRID_CALL_INFO);
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_DISPCODE)
    {
        return LANG_TRANSLATE(TRID_ACD_DISPOSITION_CODE_SHORT);
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_TRACE)
    {
        return LANG_TRANSLATE(TRID_ACD_TRACE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_ACD_EMERGESCAL)
    {
        return LANG_TRANSLATE(TRID_EMERGENCY);
    }
    if (strTalkUIAction == TALK_UI_ACTION_DPICKUP)
    {
        return LANG_TRANSLATE(TRID_DIRECT_PICKUP);
    }
    if (strTalkUIAction == TALK_UI_ACTION_GPICKUP)
    {
        return LANG_TRANSLATE(TRID_GROUP_CALL_PICKUP);
    }
    if (strTalkUIAction == TALK_UI_ACTION_PRIHOLD)
    {
        return LANG_TRANSLATE(TRID_PRI_HOLD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_SWITCH)
    {
        return LANG_TRANSLATE(TRID_SWITCH);
    }
    if (strTalkUIAction == TALK_UI_ACTION_RTP_STATUS)
    {
        return LANG_TRANSLATE(TRID_RTP_STATUS);
    }
    if (strTalkUIAction == TALK_UI_ACTION_RTP_STATUS_SWITCH)
    {
        return LANG_TRANSLATE(TRID_SWITCH);
    }
#if IF_BUG_23548
    if (strTalkUIAction == TALK_UI_ACTION_CALL_BACK)
    {
        return LANG_TRANSLATE(TRID_CALL_BACK);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (strTalkUIAction == TALK_UI_ACTION_RECORD)
    {
        return LANG_TRANSLATE(TRID_RECORD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_BACK)
    {
        return LANG_TRANSLATE(TRID_BACK);
    }
    if (strTalkUIAction == TALK_UI_ACTION_STARTRECORD)
    {
        return LANG_TRANSLATE(TRID_RECORD_START);
    }
    if (strTalkUIAction == TALK_UI_ACTION_STOPRECORD)
    {
        return LANG_TRANSLATE(TRID_RECORD_STOP);
    }
    if (strTalkUIAction == TALK_UI_ACTION_PAUSERECORD)
    {
        return LANG_TRANSLATE(TRID_RECORD_PAUSE);
    }
    if (strTalkUIAction == TALK_UI_ACTION_RESUMERECORD)
    {
        return LANG_TRANSLATE(TRID_RECORD_RESUME);
    }
#endif
    if (strTalkUIAction == TALK_UI_ACTION_CALL_INITIATION)
    {
        return LANG_TRANSLATE(TRID_DSSKEY_CALL_INITIATION);
    }
    if (strTalkUIAction == TALK_UI_ACTION_CALL_PUSH)
    {
        return LANG_TRANSLATE(TRID_DSSKEY_CALL_PUSH);
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (strTalkUIAction == TALK_UI_ACTION_USB_BEGIN_RECORD)
    {
        return LANG_TRANSLATE(TRID_START_RECORD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_USB_STOP_REDORD)
    {
        return LANG_TRANSLATE(TRID_STOP_RECORD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_USB_PAUSE_RECORD)
    {
        return LANG_TRANSLATE(TRID_PAUSE_RECORD);
    }
    if (strTalkUIAction == TALK_UI_ACTION_USB_RESUME_REDORD)
    {
        return LANG_TRANSLATE(TRID_RESUME_RECORD);
    }
#endif
    return "";
}

//获取通话时间内容
QCustomTime CDlgBaseTalkUI::GetTalkTime()
{
    QCustomTime timeTalk(0, 0, 0);
    return timeTalk;
}

//是否显示Dtmf条
bool CDlgBaseTalkUI::IsShowDtmfLine() const
{
    //默认不显示
    return false;
}

//是否显示音量条
bool CDlgBaseTalkUI::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                     QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = TalkUIlogic_IsAllowSilenceDuringCall() ? 0 : 1;
    iMax = 15;

#ifdef _T48
    rtVolume = RECT_VOLUMEBAR;
    QRect rectTalkUI = geometry();
    rtVolume.moveBottom(rectTalkUI.bottom() - 6);// 离底边6像素
#endif

    //默认显示
    return true;
}

// 是否允许键盘事件通过
bool CDlgBaseTalkUI::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    // 键盘锁开启的时候才进行判断
    if (!keyGuard_IsLock())
    {
        return true;
    }

    // 音量声道按键不做限制
    if (nKeyCode == PHONE_KEY_VOLUME_INCREASE
            || nKeyCode == PHONE_KEY_VOLUME_DECREASE
            || nKeyCode == PHONE_KEY_HEADSET_CTRL)
    {
        return true;
    }

    // 判断是否允许按键通过
    bool bAllow = false;
    switch (keyGuard_GetKeyGuardType())
    {
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            bAllow = (nKeyCode == PHONE_KEY_OK
                      || nKeyCode == PHONE_KEY_HANDFREE
                      || nKeyCode == PHONE_KEY_HANDSET
                      || nKeyCode == PHONE_KEY_HANDSET_ON_HOOK
                      || nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                      //|| nKeyCode == PHONE_KEY_TRANSFER
                      //|| nKeyCode == PHONE_KEY_HOLD
                      //|| nKeyCode == PHONE_KEY_CONFERENCE
                      || nKeyCode == PHONE_KEY_HEADSET_OUT
                      || nKeyCode == PHONE_KEY_HEADSET_IN
                      || nKeyCode == PHONE_KEY_EHS_OPEN
                      || nKeyCode == PHONE_KEY_EHS_CLOSE
                      || nKeyCode == PHONE_KEY_STAR
                      || nKeyCode == PHONE_KEY_POUND
                      || nKeyCode == PHONE_KEY_CANCEL
                      || nKeyCode == PHONE_KEY_BACKSPACE
                      || nKeyCode == Qt::Key_Space
                      || nKeyCode == PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD
                      || (PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9)
                      || (PHONE_KEY_LINE1 <= nKeyCode && nKeyCode <= PHONE_KEY_LINE10)
                      //|| (PHONE_KEY_LEFT <= nKeyCode && nKeyCode <= PHONE_KEY_DOWN)
                      || (PHONE_KEY_FN1 <= nKeyCode && nKeyCode <= PHONE_KEY_FN4));
        }
        break;
    case KG_MENU_KEY:
        {
            bAllow = true;
        }
        break;
    default:
        {
            bAllow = false;
        }
        break;
    }

    return bAllow;
}

// 判断是否需要播放按键音
bool CDlgBaseTalkUI::IsNeedToPlayKeyTone(int iKey)
{
    return false;
}

// 判断Softkey是否需要播放按键音
bool CDlgBaseTalkUI::IsSoftkeyNeedToPlayKeyTone(yl::string strSoftkeyAction)
{
    return false;
}

#ifdef _T48
bool CDlgBaseTalkUI::AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent)
{
    // 默认允许鼠标事件通过
    // TalkUI模块用过滤Action的方式上锁
    return true;
}
#endif

bool CDlgBaseTalkUI::IsAllowSendAction(TALK_ACTION_TYPE eActionType)
{
    // 键盘锁未上锁,或者处于MenuKey的状态
    // 允许所有Action通过
    if (!keyGuard_IsLock()
            || keyGuard_GetKeyGuardType() == KG_MENU_KEY)
    {
        return true;
    }

    // 在Allkeys、Funkey开启的时候只允许接听呼出和接听通话
    return (eActionType == TALK_ACTION_SEND
            || eActionType == TALK_ACTION_CANCEL
            || eActionType == TALK_ACTION_ANSWER
            || eActionType == TALK_ACTION_KEYPRESS
            || eActionType == TALK_ACTION_TEXT_CHANGE
            || eActionType == TALK_ACTION_NEWCALL
            || eActionType == TALK_ACTION_REJECT
            || eActionType == TALK_ACTION_FOCUSE_CHANGE
            || eActionType == TALK_ACTION_SCREEN_CLICK
            || eActionType == TALK_ACTION_VOLUME_CHANGE
            || eActionType == TALK_ACTION_VOLUMEBAR_SHOW
            || eActionType == TALK_ACTION_WND_FOCUSE_CHANGE
            || eActionType == TALK_ACTION_EXIT_FAILD_INFO
            //在通话过程中按数字键属于DTMF
            || eActionType == TALK_ACTION_DTMF
            //不允许hold但运行resume，因为进入line界面后会自动hold
            || eActionType == TALK_ACTION_RESUME
            || eActionType == TALK_ACTION_ACCOUNT_CHANGE);
}

// 是否允许键盘事件通过
bool CDlgBaseTalkUI::AllowSoftkeyResponseEvent(const QString & strAction)
{
    // 判断是否允许按键通过
    bool bAllow = false;
    switch (keyGuard_GetKeyGuardType())
    {
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            bAllow = (TALK_UI_ACTION_IME == strAction
                      || TALK_UI_ACTION_DELETE == strAction
                      || TALK_UI_ACTION_LINE == strAction
                      || TALK_UI_ACTION_TEXT_CHANGE == strAction
                      || TALK_UI_ACTION_SEND == strAction
                      || TALK_UI_ACTION_REDIAL == strAction
                      || TALK_UI_ACTION_CANCEL == strAction
                      || TALK_UI_ACTION_ENDCALL == strAction
                      || TALK_UI_ACTION_NEWCALL == strAction
                      || TALK_UI_ACTION_ANSWER == strAction
                      || TALK_UI_ACTION_REJECT == strAction
                      || TALK_UI_ACTION_IGNORE == strAction
                      || TALK_UI_ACTION_RESUME == strAction
                      || TALK_UI_ACTION_OK == strAction);
        }
        break;
    case KG_MENU_KEY:
        {
            bAllow = true;
        }
        break;
    default:
        {
            bAllow = false;
        }
        break;
    }

    return bAllow;
}

// 是否允许softkey事件通过
bool CDlgBaseTalkUI::AllowSoftKeyEventOnKeyLock(const QString & strAction)
{
    // 键盘锁未上锁,或者处于MenuKey的状态
    // 允许所有Action通过
    //这边要怎么改？键盘锁涉及业务
    if (!keyGuard_IsLock()
            || keyGuard_GetKeyGuardType() == KG_MENU_KEY)
    {
        return true;
    }

    if (!AllowSoftkeyResponseEvent(strAction))
    {
        //如果不允许，则调用该接口弹出解锁框（并非是要判断是否允许dsskey通过）
        if (!keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY))//functionkey时调用该函数必然返回false
        {
            return false;
        }
    }

    return true;
}


//是否显示加密图标
bool CDlgBaseTalkUI::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return false;

}

#ifdef _T48
void CDlgBaseTalkUI::ExitTalkUI()
{
    OnUIAction(TALK_UI_ACTION_ENDCALL);
}
#endif

bool CDlgBaseTalkUI::IsChannelKeyToExit(int iKey)
{
    bool bExit = FALSE;

    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (!voice_GetHandfreeStatus() && !voice_GetHandsetStatus())
            {
                bExit = TRUE;
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!voice_GetHandfreeStatus() && !voice_GetHandsetStatus()
                    && voice_GetCurrentChannel() != CHANNEL_HEADSET)
            {
                bExit = TRUE;
            }
        }
        break;
    case PHONE_KEY_EHS_CLOSE:
        {
            if (voice_GetCurrentChannel() == CHANNEL_HEADSET)
            {
                // 如果耳麦打开则退出
                bExit = TRUE;
            }
        }
        break;
    case PHONE_KEY_HEADSET_CTRL:
        {
            if (configParser_GetConfigInt(kszHeadsetCtrlCall) == 1)
            {
                bExit = !voice_GetHeadsetStatus() && !voice_GetHandsetStatus();
            }
#ifdef IF_FEATURE_HEADSET_AS_CANCEL
            else if (configParser_GetConfigInt(kszHeadsetAsCancel) != 0)
            {
                if (!voice_GetHeadsetStatus() && !voice_GetHandsetStatus())
                {
                    bExit = TRUE;
                    // 耳麦键退出时，需要将通道切到耳麦，比如耳麦通话中，按耳麦退出，要求通道还是耳麦
                    voice_SwitchChannel(CHANNEL_HEADSET);
                }
            }
#endif
        }
        break;
    default:
        break;
    }

    return bExit;
}

// 是否*、#呼出键
bool CDlgBaseTalkUI::IsSendDigitKey(yl::string strEditContent, int iKey)
{
    // 0--Disable，1--#，2--*
    int iSendKey = _TalkUIDelegate.GetKeyAsSend();
    // 拨号界面能够输入##（就算#设置为呼出键，此时也要能在拨号界面输入##） 主要是为了 呼出##
    if ((iSendKey == 1 && iKey == PHONE_KEY_POUND
            && (!_TalkUIDelegate.IsEnableSendPoundKey()  //这个配置的意思 应该是能不能呼出## 这么理解
                || yl::string("#") != strEditContent))
            || iSendKey == 2 && iKey == PHONE_KEY_STAR)
    {
        return true;
    }

    return false;
}

//// 来电显示方式 有两个显示控件 一个 名称控件  一个 号码 控件
////执行该函数后  strName 用于给名称控件 赋值    strNumber用于给号码控件赋值
//void CDlgBaseTalkUI::ChangeNameAndNumberByInfoMethod(yl::string& strName, yl::string& strNumber, yl::string& strFullContactInfo, SHOW_CONTACTDETAIL_TYPE showContactDetailType, bool bIsMergerNameAndNumber /*= true*/)
//{
//  ////暂时不做任何操作  直接返回
//  //if(bIsMergerNameAndNumber)
//  //{
//  //if(strName == strNumber)
//  //{
//  //  strNumber = "";
//  //}
//  //}
//  //return ;
//
//  //三个当中 至少有一个 有值的时候 先保证 strName 有值
//  if(strName.empty())
//  {
//      strName = strNumber;
//
//      if(strName.empty())
//      {
//          strName = strFullContactInfo;
//
//          //这边如果是3个联系人信息全是为空 那么直接返回就好了
//          if(strName.empty())
//          {
//              return ;
//          }
//      }
//  }
//
//  //这三个 信息中strName 有用于最后赋值给 名称控件    strNumber 有用于赋值给 号码控件   strFullContactInfo 没有用于赋值给任何控件
//  //所以 其实只是给 strName 和 strNumber 两个变量赋值就行了
//
//  //  int nDisplayMethod = configParser_GetConfigInt(kszDisplayMethod);
//  if(showContactDetailType == SHOW_NAME_AND_NUMBER)
//  {
//      //如果是选择 显示 name + number  那么不需要做任何操作即可
//  }
//  else if (showContactDetailType == SHOW_NUMBER_AND_NAME)
//  {
//      //如果 是选择 number + name  那么 要对调一下
//      //但是首先得保证 strNumber 不为空
//      if(!strNumber.empty())
//      {
//          yl::string strTemp = strName;
//          strName = strNumber;
//          strNumber = strTemp;
//      }
//  }
//  else if (showContactDetailType == SHOW_ONLY_NAME)
//  {
//      //如果只显示 名称  那么把号码赋值为空
//      strNumber = "";
//  }
//  else if (showContactDetailType == SHOW_ONLY_NUMBER)
//  {
//      //如果只显示号码  那么把号码赋值给 名称变量  然后号码变量设置为空
//      //但是首先要保证 号码不为空  否则仍然显示号码
//      if(!strNumber.empty())
//      {
//          strName = strNumber;
//          strNumber = "";
//      }
//  }
//  else if (showContactDetailType == SHOW_FULL_CONTACT_INFO)
//  {
//      //如果只显示 FullContactInfo 那么把 FullContactInfo 赋值个 名称 并且 号码 赋值为空
//      strNumber = "";
//
//      //但是首先要保证FullContactInfo 不为空
//      if(!strFullContactInfo.empty())
//      {
//          strName = strFullContactInfo;
//      }
//  }
//
//  //如果需要合并号码和名称
//  if(bIsMergerNameAndNumber)
//  {
//      //名称和号码相同才需要合并
//      if(strName == strNumber)
//      {
//          //把 号码赋值为空就可以了
//          strNumber = "";
//      }
//  }
//
//}

//// 来电显示方式 只有一个名称控件
////执行该函数后  strName 用于给名称控件 赋值
//void CDlgBaseTalkUI::ChangeNameAndNumberByInfoMethodOnlyShowName(yl::string& strName, yl::string& strNumber, yl::string& strFullContactInfo, SHOW_CONTACTDETAIL_TYPE showContactDetailType, bool bIsMergerNameAndNumber /*= true*/)
//{
//  ////暂时不做任何操作  直接返回
//  //if(bIsMergerNameAndNumber)
//  //{
//  //if(strName == strNumber)
//  //{
//  //  strNumber = "";
//  //}
//  //}
//  //return ;
//
//  //三个当中 至少有一个 有值的时候 先保证 strName 有值
//  if(strName.empty())
//  {
//      strName = strNumber;
//
//      if(strName.empty())
//      {
//          strName = strFullContactInfo;
//
//          //这边如果是3个联系人信息全是为空 那么直接返回就好了
//          if(strName.empty())
//          {
//              return ;
//          }
//      }
//  }
//
//  //这三个 信息中strName 有用于最后赋值给 名称控件
//  //所以 其实只是给 strName 变量赋值就行了
//
//  //  int nDisplayMethod = configParser_GetConfigInt(kszDisplayMethod);
//  if(showContactDetailType == SHOW_NAME_AND_NUMBER)
//  {
//      //号码不为空的时候才需要进行操作
//      if(!strNumber.empty())
//      {
//          //如果需要合并名称和号码
//          if(bIsMergerNameAndNumber)
//          {
//              if(strName == strNumber)
//              {
//                  //名称和号码相同不需要做任何操作 只需要显示名称就行了
//
//              }
//              else
//              {
//                  strName = strName + " ";
//                  strName = strName + strNumber;
//              }
//          }
//          else
//          {
//              strName = strName + " ";
//              strName = strName + strNumber;
//          }
//      }
//      //号码为空 直接就显示 名称就行了
//  }
//  else if (showContactDetailType == SHOW_NUMBER_AND_NAME)
//  {
//      //号码不为空 才继续
//      if(!strNumber.empty())
//      {
//          //先对调一下 两个变量的值
//          yl::string strTemp = strName;
//          strName = strNumber;
//          strNumber = strTemp;
//
//          //如果需要合并名称和号码
//          if(bIsMergerNameAndNumber)
//          {
//              if(strName == strNumber)
//              {
//                  //名称和号码相同不需要做任何操作 只需要显示名称就行了
//
//              }
//              else
//              {
//                  strName = strName + " ";
//                  strName = strName + strNumber;
//              }
//          }
//          else
//          {
//              strName = strName + " ";
//              strName = strName + strNumber;
//          }
//      }
//
//      //号码为空 直接就显示 名称就行了
//  }
//  else if (showContactDetailType == SHOW_ONLY_NAME)
//  {
//      //如果只显示 名称  不需要做任何处理
//
//  }
//  else if (showContactDetailType == SHOW_ONLY_NUMBER)
//  {
//      //如果只显示号码  那么把号码赋值给 名称变量
//      //但是首先要保证 号码不为空  否则仍然显示号码
//      if(!strNumber.empty())
//      {
//          strName = strNumber;
//      }
//  }
//  else if (showContactDetailType == SHOW_FULL_CONTACT_INFO)
//  {
//      //如果只显示 FullContactInfo 那么把 FullContactInfo 赋值给 名称
//      //但是首先要保证FullContactInfo 不为空
//      if(!strFullContactInfo.empty())
//      {
//          strName = strFullContactInfo;
//      }
//  }
//
//}

//yl::string TransTalkText(yl::string& strText)
//{
//  if (strText == "Anonymous"
//      || strText == "anonymous")
//  {
//      return LANG_TRANSLATE(strText.c_str()).toUtf8().data();
//  }
//
//  if((strText.find(kszReplaceCallParkCode) != yl::string::npos
//      || strText.find(kszReplaceCallRetriveCode) != yl::string::npos
//      || strText.find(kszReplaceGroupParkCode) != yl::string::npos
//      || strText.find(kszReplaceGPickUpCode) != yl::string::npos
//      || strText.find(kszReplaceDPickUpCode) != yl::string::npos
//      || strText.find(kszReplaceVoiceMailCode) != yl::string::npos
//      || strText.find(kszReplaceBargeInCode) != yl::string::npos
//      || strText.find(kszReplaceCallPullCode) != yl::string::npos)
//      && configParser_GetConfigInt(kszHideFeatureAccessCodes) != 0)
//  {
//      yl::string::size_type nPos = 0;
//      if ((nPos = strText.find(':')) != yl::string::npos)
//      {
//          yl::string strTemp = strText;
//          yl::string strFACValue = strTemp.substr(0, nPos);
//          strTemp = strTemp.substr(nPos);
//          strFACValue = LANG_TRANSLATE(strFACValue.c_str()).toUtf8().data();
//          strFACValue.append(strTemp);
//
//          return strFACValue;
//      }
//      else
//      {
//          return LANG_TRANSLATE(strText.c_str()).toUtf8().data();
//      }
//  }
//  else
//  {
//      return strText;
//  }
//}
//
//// 根据callinfo获取显示名称
//yl::string CDlgBaseTalkUI::GetNameFromCallInfo(CCallInfo* pCallInfo)
//{
//  if (pCallInfo != NULL)
//  {
//      if (pCallInfo->m_tRemoteInfo.sDisplayName.empty())
//      {
//          return TransTalkText(pCallInfo->m_tRemoteInfo.sSIPName);
//      }
//      return TransTalkText(pCallInfo->m_tRemoteInfo.sDisplayName);
//  }
//  return "";
//}
//
//// 根据callinfo获取显示号码
//yl::string CDlgBaseTalkUI::GetNumberFromCallInfo(CCallInfo* pCallInfo)
//{
//  if (pCallInfo != NULL)
//  {
//      return TransTalkText(pCallInfo->m_tRemoteInfo.sSIPName);
//  }
//  return "";
//}
//
//yl::string CDlgBaseTalkUI::GetFullContactInfoFromCallInfo(CCallInfo* pCallInfo)
//{
//  if(NULL != pCallInfo)
//  {
//      return pCallInfo->m_tRemoteInfo.sFullContactInfo;
//  }
//
//  return "";
//}

////获取TargetInfo 信息 主要是给 通话、会议 提供新来电 targetinfo 信息  获取的优先级为 Label、DisplayName、UserName
//yl::string CDlgBaseTalkUI::GetTargetInfo()
//{
//  yl::string strTargetInfo = "";
//
//  //需要显示targetinfo
//  if(_TalkUIDelegate.IsShowTargetInfo())
//  {
//      int iAccountID = _TalkUIDelegate.GetTalkingIncomingCallTargetAccountId();
//
//      strTargetInfo = _TalkUIDelegate.GetHotelingUserName(iAccountID);
//
//      //获取到的hoteling账号为空
//      if(strTargetInfo.empty())
//      {
//          strTargetInfo = GetLabel(iAccountID);
//      }
//  }
//
//  return strTargetInfo;
//}
//
////获取Label  Label 不存在那么 获取GetDisplayName 函数数据
//yl::string CDlgBaseTalkUI::GetLabel(int iAccountID)
//{
//  yl::string strLabel = _TalkUIDelegate.GetLabelFromLogic(iAccountID);
//
//  if(strLabel.empty())
//  {
//      strLabel = GetDisplayName(iAccountID);
//  }
//
//  return strLabel;
//}
//
////获取Displayname Display 如果获取不成功 那么用 GetUserName 函数数据
//yl::string CDlgBaseTalkUI::GetDisplayName(int iAccountID)
//{
//  yl::string strDisplayName = _TalkUIDelegate.GetDisplayNameFromLogic(iAccountID);
//
//  if(strDisplayName.empty())
//  {
//      strDisplayName = GetUserName(iAccountID);
//  }
//
//  return strDisplayName;
//}
//
////获取UserName 数据
//yl::string CDlgBaseTalkUI::GetUserName(int iAccountID)
//{
//  return _TalkUIDelegate.GetUserNameFromLogic(iAccountID);
//}

#ifdef _T48
// 调整控件布局
void CDlgBaseTalkUI::AdjustLayout()
{

}

#endif

// 显示安全级别
void CDlgBaseTalkUI::ChangeNameAndNumberBySecurityLevel(yl::string & strName,
        yl::string & strNumber)
{
    // 获取话路对应的等级
    yl::string strSecurityLev = fromQString(LANG_TRANSLATE(talklogic_GetTalkSecurityLevel(
            GetSessionId()).c_str()));
    if (strSecurityLev.empty())
    {
        return ;
    }

    // 多行显示的机型, 优先保证在第二行；
    // 本来第二行有显示信息，就插在它前面，中间加一个“-”；
    // 本来第二行没显示，就直接显示安全级别，也没有“-”；
    if (strName != strNumber && !strNumber.empty())
    {
        strNumber = strSecurityLev + "-" + strNumber;
    }
    else
    {
        strNumber = strSecurityLev;
    }
}

//音量条变化调用函数
void CDlgBaseTalkUI::OnTalkVolumnValueChange()
{

}

bool CDlgBaseTalkUI::SendUIRequestAction(TALK_ACTION_TYPE eActionType,
        WPARAM wActionParam /* = 0 */,
        LPARAM lActionParam /* = 0 */, void * pExtraData /* = NULL */)
{
#ifndef _T2X
    if (TALK_ACTION_SWAP == eActionType)
    {
        if (IsSwapLock())
        {
            return true;
        }

        LockSwap();
        QTimer::singleShot(TALK_ACTION_SWAP_INTERVAL, this, SLOT(UnlockSwap()));
    }

#endif

    TALKUI_INFO("ProcessAction eActionType is %d wActionParam is %d", eActionType, wActionParam);
    return talklogic_UIEvent(GetSessionId(), eActionType, wActionParam, lActionParam, pExtraData);
}

////TalkWidget回调相关
//void CDlgBaseTalkUI::s_TALKWIDGET_CALLBACK(void* pData, TALKWIDGET_CALLBACK_TYPE talkWidgetCallBackType)
//{
//  CDlgBaseTalkUI* pTalkUI = (CDlgBaseTalkUI*)pData;
//
//  if(NULL != pTalkUI)
//  {
//      pTalkUI->OnTalkWidgetAction(talkWidgetCallBackType);
//  }
//}
//
//void CDlgBaseTalkUI::OnTalkWidgetAction(TALKWIDGET_CALLBACK_TYPE talkWidgetCallBackType)
//{
//
//}

// 切换通话账号
void CDlgBaseTalkUI::ChangeAccount(int iAccountID, bool bRefresh)
{
    SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE, iAccountID, bRefresh);
}

#ifdef _T48
void CDlgBaseTalkUI::AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd)
{

}

//删除所有图标
void CDlgBaseTalkUI::DeleteAllIcon()
{

}

#endif

#ifndef _T2X

bool CDlgBaseTalkUI::OnUIAction(const QString & strAction)
{
    //Switchsession 这边直接调用逻辑层接口
    if (strAction == TALK_UI_ACTION_SWITCH)
    {
        if (keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY))
        {
            talklogic_SwitchFocusSession(true);
        }
    }
    else if (strAction == TALK_UI_ACTION_OPENHISTORY)
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL);
    }
    else if (strAction == TALK_UI_ACTION_OPENDIRECTORY)
    {
#if IF_BUG_PLCMDIR
        if (configParser_GetConfigInt(kszPlcmPhoneEnable) == 1)
        {
            DM_EnterDirectoryUI(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
        }
        else
        {
            DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL);
        }
#else
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL);
#endif
    }
    else if (strAction == TALK_UI_ACTION_ENDCALL)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_CANCEL)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_OK)
    {
        SendUIRequestAction(TALK_ACTION_OK);
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    else if (strAction == TALK_UI_ACTION_USB_BEGIN_RECORD)
    {
        if (!modRecord_IsStorageConnect()
                || modRecord_IsRecording(RECORD_AUDIO))
        {
            return true;
        }

        CCallInfo * pCallInfo = talklogic_GetCallInfoBySessionID(m_iSessionID);
        if (m_iSessionID != -1 && NULL != pCallInfo)
        {
            return modRecord_RecordStart(RECORD_AUDIO, GetNameFromCallInfo(pCallInfo).c_str());
        }
    }
    else if (strAction == TALK_UI_ACTION_USB_STOP_REDORD)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO)
                && !modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordStop(RECORD_AUDIO);
    }
    else if (strAction == TALK_UI_ACTION_USB_PAUSE_RECORD)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordPause(RECORD_AUDIO);
    }
    else if (strAction == TALK_UI_ACTION_USB_RESUME_REDORD)
    {
        if (!modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return true;
        }

        return modRecord_RecordResume(RECORD_AUDIO);
    }
#endif
#ifdef IF_FEATURE_ENTRANCE_GUARD
    else if (strAction == TALK_UI_ACTION_OPENDOOR)
    {
        SendUIRequestAction(TALK_ACTION_OPENDOOR);
    }
#endif
    else
    {
        TALKUI_INFO("OnUIAction(%s) Action error", strAction.toUtf8().data());
        return false;
    }

    return true;
}

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
bool CDlgBaseTalkUI::OnUIUSBAction(const QString & strAction)
{
    if (strAction == TALK_UI_ACTION_USB_BEGIN_RECORD)
    {
        if (!modRecord_IsStorageConnect()
                || modRecord_IsRecording(RECORD_AUDIO))
        {
            return true;
        }

        CCallInfo * pCallInfo = talklogic_GetCallInfoBySessionID(m_iSessionID);
        if (m_iSessionID != -1 && NULL != pCallInfo)
        {
            modRecord_RecordStart(RECORD_AUDIO, GetNameFromCallInfo(pCallInfo).c_str());
        }
    }
    else if (strAction == TALK_UI_ACTION_USB_STOP_REDORD)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO)
                && !modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return true;
        }

        modRecord_RecordStop(RECORD_AUDIO);
    }
    else if (strAction == TALK_UI_ACTION_USB_PAUSE_RECORD)
    {
        if (!modRecord_IsRecording(RECORD_AUDIO))
        {
            return true;
        }

        modRecord_RecordPause(RECORD_AUDIO);
    }
    else if (strAction == TALK_UI_ACTION_USB_RESUME_REDORD)
    {
        if (!modRecord_IsRecordPause(RECORD_AUDIO))
        {
            return true;
        }

        modRecord_RecordResume(RECORD_AUDIO);
    }
    else
    {
        return false;
    }

    return true;
}
#endif

//softkey按下响应事件槽
void CDlgBaseTalkUI::OnSoftkeyClicked(qSoftButton * pSoftButton)
{
    if (pSoftButton != NULL)
    {
        //http://10.2.1.199/Bug.php?BugID=95331
        //http://10.2.1.199/Bug.php?BugID=95403
        //改造T46键盘锁流程 将之前分散流程统一到这里处理
        QString strAction = pSoftButton->m_strButtonName;

        if (!AllowSoftKeyEventOnKeyLock(strAction))
        {
            return;
        }

        //http://10.2.1.199/Bug.php?BugID=97835 把USB的响应提到主流程来响应
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
        if (OnUIUSBAction(strAction))
        {
            return;
        }
#endif
        OnUIAction(pSoftButton->m_strButtonName);
    }
}

#endif




#ifndef _T2X

bool CDlgBaseTalkUI::IsSwapLock()
{
    return m_bLockSwap;
}

// 对swap操作进行加锁操作
void CDlgBaseTalkUI::LockSwap()
{
    m_bLockSwap = true;
}

void CDlgBaseTalkUI::UnlockSwap()
{
    m_bLockSwap = false;
}


#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
void CDlgBaseTalkUI::AddUSBRecordSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    //http://bugfree.yealink.com/Bug.php?BugID=132700
    //http://bugfree.yealink.com/Bug.php?BugID=132701
    //rtpstatus界面不显示录音的softkey
    if (DLG_RTPStatusUI == GetTalkUIName())
    {
        return;
    }

    if (SESSION_TALKING == m_eSessionState
            || SESSION_LOCALCONF == m_eSessionState
            || SESSION_LINEDEVICE == m_eSessionState
            || SESSION_NETWORKCONF == m_eSessionState
            || SESSION_TRANSFERRED == m_eSessionState)
    {
        //先去掉录音softkey,否则把掉USB之后还会存在Record的softkey
        for (int i = 0; i < objSoftkeyData.size();)
        {
            if (objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_USB_STOP_REDORD
                    || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_USB_BEGIN_RECORD
                    || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_USB_PAUSE_RECORD
                    || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_USB_RESUME_REDORD)
            {
                objSoftkeyData.erase(objSoftkeyData.begin() + i);
            }
            else
            {
                i++;
            }
        }

        if (!modRecord_IsRecordEnable(RECORD_AUDIO))
        {
            return;
        }

        for (int i = 0; i < objSoftkeyData.size();)
        {
            //本地录制和blsf录制不共存
            if (objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_STARTRECORD
                    || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_STOPRECORD
                    || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_PAUSERECORD
                    || objSoftkeyData[i].m_strSoftkeyAction == TALK_UI_ACTION_RESUMERECORD)
            {
                objSoftkeyData.erase(objSoftkeyData.begin() + i);
            }
            else
            {
                i++;
            }
        }

        if (modRecord_IsRecording(RECORD_AUDIO))
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_USB_STOP_REDORD);
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_USB_PAUSE_RECORD);
        }
        else if (modRecord_IsRecordPause(RECORD_AUDIO))
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_USB_STOP_REDORD);
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_USB_RESUME_REDORD);
        }
        else
        {
            AddSoftkey(objSoftkeyData, TALK_UI_ACTION_USB_BEGIN_RECORD);
        }
    }
}

bool CDlgBaseTalkUI::StartUsbRecord()
{
    if (!modRecord_IsRecordEnable(RECORD_AUDIO))
    {
        TALKUI_WARN("Not allow recording!");
        return false;
    }

    if (modRecord_IsRecording(RECORD_AUDIO))
    {
        TALKUI_INFO("Recording already.");
        return true;
    }

    int iSessionID = -1;
    iSessionID = talklogic_GetFocusedSessionID();
    CCallInfo * pCallInfo = talklogic_GetCallInfoBySessionID(iSessionID);
    if (iSessionID == -1 || NULL == pCallInfo)
    {
        TALKUI_WARN("Cannot get call info.");
        return false;
    }
    return modRecord_RecordStart(RECORD_AUDIO, GetNameFromCallInfo(pCallInfo).c_str());
}
#endif

#endif
