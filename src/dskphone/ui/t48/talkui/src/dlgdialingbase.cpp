// mod-Qt
#include <QTextEdit>

// mod-this(TalkUI)
#include "dlgdialingbase.h"
#include "modtalkui.h"

// mod-InputMethod
#include "inputmethod/inputmethod.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "uikernel/qpswlineedit.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"
#include "searchwidget.h"
#include "uimanager/rectdefine.h"

// #include "keyguard/modkeyguard.h"
#include "settingui/include/modsettingui.h"

#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"
// #include "dsskeyui/moddsskeyui.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "contactinfoframe.h"
#include "account/include/accountmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "commonfunction.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "keyguard/include/modkeyguard.h"

#include "talk/talklogic/include/commonfunctions.h"
#include "autopcontrol/include/modautopcontrol.h"

#include "talkui/src/talkuimanager.h"
#include "talkuidelegate.h"
#include "talkuiframemanager.h"

#include "contacts/directory/include/directorymessagedefine.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "dialexecutiveselect.h"
#include "directoryui/src/transfernumselector.h"
#include "qtcommon/qmisc.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "uicommon/messagebox/selectmessagebox.h"

#include "talkuicommon.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/modrecord.h"
#endif
#include "voice/include/modvoice.h"

LRESULT OnDialingMessage(msgObject & refMessage)
{
    CBaseDialog * pDlgBase = _TalkUIFrameMananger.GetTalkUI();
    if (NULL != pDlgBase)
    {
        if (pDlgBase->inherits(DLG_Dial))
        {
            CDlgDialingBase * pDialing = (CDlgDialingBase *)pDlgBase;
            return pDialing->OnMessage(refMessage);
        }

    }

    return -1;
}

LRESULT OnSearchMessage(msgObject & refMessage)
{
    if (refMessage.message != SUPER_SEARCH_RESULT)
    {
        return FALSE;
    }

    bool suc = (1 == refMessage.lParam);

    if (!suc)
    {
        return TRUE;
    }

    //如果顶层窗口 不是通话窗口 那么直接返回  （有可能是联系人界面）
    //有可能是 从 dialingui 切换到 selectline 的时候会刷 所以这边还多判断一个 selectline 界面
    CBaseDialog * pTopDlg = UIManager_GetTopWindow();

    //这边判断 CDlgSelectPageUI 是因为在t29 的时候  有账号选择界面可能有搜索返回
    if (!UIManager_IsInCallManager() && !(pTopDlg->inherits("CDlgSelectPageUI")))
    {
        return TRUE;
    }

    yl::string strKey = (char *)refMessage.GetExtraData();

    CDlgBaseTalkUI * pDlgBaseTalkUI = _TalkUIFrameMananger.GetTalkUI();

    //如果获取的指针不为空
    if (NULL != pDlgBaseTalkUI)
    {
        //获取的对话框为拨号对话框 才要继续往下执行
        if (pDlgBaseTalkUI->inherits(DLG_Dial))
        {
            CDlgDialingBase * pDlgDialingBase = (CDlgDialingBase *)pDlgBaseTalkUI;

            pDlgDialingBase->DealSuperSearchMessage(refMessage);

            return TRUE;
        }
    }

    return FALSE;
}


LRESULT CDlgDialingBase::OnMessage(msgObject & refMessage)
{
    if (ACCOUNT_DEFAULT_CHANGED == refMessage.message
            && UIManager_ExistCallManager()
            && m_iCurrentAccountId != acc_GetDefaultAccount())
    {
        //dlgbasetalkui 发起
        //t46账号改变需要呼出， t48 账号改变不需要呼出
        //t46 改变账号时在 talkpool界面改变的 而t48 直接是在mainwnd的title中改变的。

        ChangeAccount(acc_GetDefaultAccount(), false);
    }

    return 0;
}

//LRESULT CDlgDialingBase::DealSuperSearchMessage(msgObject& refMessage)
//{
//  //显示第一页数据
//  if(NULL != m_pSearchTable)
//  {
//      m_pSearchTable->ShowSearchPage(0);
//  }
//
//  return 0;
//}


//构造函数
CDlgDialingBase::CDlgDialingBase()
    : CDlgNormalTalkBase(DLG_Dial, TALK_UI_NORMAL_DIAL)
    , m_eDialUIType(DIAL_UI_UNKNOWN)
    , m_strTitleInfo("")
    , m_iCurrentAccountId(-1)
    , m_bFirstTextChange(true)
    , m_strSearchData("")
    , m_eTalkState(TS_UNKNOW)
#if IF_FEATURE_DIAL_ON_HOLD
    , m_bAllowTran(false)           //DIAL_UI_ON_HOLD 状态下 是否允许transfer
#endif
    , m_pTransferNumSelector(NULL)
    , m_bSearchTransfer(false)
{
    InitData();
}

//析构函数
CDlgDialingBase::~CDlgDialingBase()
{

}

// 初始化页面的数据
void CDlgDialingBase::InitData()
{
    //设置属性为拒绝返回idle
    SetRejectReturnToIdle();
}

// 初始化
void CDlgDialingBase::Init()
{
    //基类先初始化
    CDlgNormalTalkBase::Init();

    m_eTalkState = TS_UNKNOW;

#if IF_FEATURE_DIAL_ON_HOLD
    m_bAllowTran = false;           //DIAL_UI_ON_HOLD 状态下 是否允许transfer
#endif

    m_bSearchTransfer = false;

    m_strTitleInfo = "";

    m_iCurrentAccountId = -1;

    //初始化 拨号界面类型
    m_eDialUIType = DIAL_UI_UNKNOWN;

    m_bFirstTextChange = true;

    m_strSearchData = "";


    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, OnDialingMessage);

    //注册超级搜索结果消息
    etl_RegisterMsgHandle(SUPER_SEARCH_RESULT,
                          SUPER_SEARCH_RESULT,
                          OnSearchMessage);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgDialingBase::Uninit()
{
    m_iCurrentAccountId = -1;

    m_bFirstTextChange = true;

    //拨号界面 没有这些图标
    // 删除加密图标
//  AddOrDeleteIcon(PIC_ICON_ENCRYPT, false);

    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, OnDialingMessage);

    //注销超级搜索回调
    etl_UnregisterMsgHandle(SUPER_SEARCH_RESULT, SUPER_SEARCH_RESULT, OnSearchMessage);

    ReleaseSelector();

    //基类反初始化
    CDlgNormalTalkBase::Uninit();
}

//更新窗口
void CDlgDialingBase::UpdateWnd()
{
    //更新通道图标
    _TalkUIDelegate.UpdateChannelIcon();

#ifdef _T48
    //在这更新title
    _TalkUIDelegate.UpdateTitleInfo(m_strTitleInfo);
#endif

}

QPixmap CDlgDialingBase::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_DIALING_BACKGROUND);
}

// 焦点切换
void CDlgDialingBase::OnFocusChanged(bool bFocus)
{
    CDlgNormalTalkBase::OnFocusChanged(bFocus);
    //在UpdateWnd 中更新通道了  这边就不更新了

    //发送焦点改变的消息给逻辑层
    SendUIRequestAction(TALK_ACTION_WND_FOCUSE_CHANGE, (int)bFocus);

}

bool CDlgDialingBase::IsShowTitle(TitleLayoutInfo & objInfo)
{
    //拨号、预拨号状态下 需要显示 选择账号 所有要用  TITLE_MODE_CALL_MANAGER 模式
    //非拨号、预拨号状态 只需要显示  当前号码就可以了
    objInfo.eLeftType = IsCanSelectLineBtn() ? TITLE_MODE_CALL_MANAGER : TITLE_MODE_TALK_INFO;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (modRecord_IsRecordPause(RECORD_AUDIO)
            || modRecord_IsRecording(RECORD_AUDIO))
    {
        objInfo.eMidType = TITLE_MODE_USB_RECORDING;
    }
    else
    {
        objInfo.eMidType = TITLE_MODE_TITLE;
    }
#else
    objInfo.eMidType  = TITLE_MODE_TITLE;
#endif
    objInfo.eRightType = TITLE_MODE_TIME;
    return true;
}

void CDlgDialingBase::AfterShowTitle()
{
    //更新通道图标
    _TalkUIDelegate.UpdateChannelIcon();
}

//设置通话数据内容
void CDlgDialingBase::SetTalkData(DataLogic2UIBase * pData)
{
    //基类设置数据
    CDlgNormalTalkBase::SetTalkData(pData);

    DataDial * pDialData = dynamic_cast<DataDial *>(pData);
    if (pDialData != NULL)
    {
        //拨号UI类型
        m_eDialUIType = pDialData->eDialUIType;

#if IF_FEATURE_DIAL_ON_HOLD
        m_bAllowTran = pDialData->bAllowTran;           //DIAL_UI_ON_HOLD 状态下 是否允许transfer
#endif

        m_strTitleInfo = "";

        if (pDialData->pCallInfo != NULL)
        {
            // 刷新高清图标
            m_iCurrentAccountId = pDialData->pCallInfo->GetAccountID();

            //设置  主要是用于在 callparkchannelkey 函数中处理
            m_eTalkState = pDialData->pCallInfo->m_eTalkState;
        }

        yl::string strTitle("");

        switch (m_eDialUIType)
        {
        case DIAL_UI_DIALING:
        case DIAL_UI_PREDIAL:
            {
                //拨号预拨号界面显示当前所用账号的信息
                if (pDialData->pCallInfo != NULL)
                {
#ifdef _T48
                    //只需要在 拨号预拨号界面就可以  因为其他界面是不允许改变账号的
                    //这边是因为  t48 MainWnd 需要在下拉的时候 显示账号列表信息  需要给当前选择的账号加上背景，并改变字体颜色
                    //设置账号信息
                    _MainWND.SetTitleSelectAccountID(pDialData->pCallInfo->GetAccountID());
#endif

                    //优先获取hoteling 账号
                    yl::string strTemp = _TalkUIDelegate.GetHotelingUserName(pDialData->pCallInfo->GetAccountID());

                    //如果获取到的 hoteling 账号还是为空
                    if (strTemp.empty())
                    {
#if IF_FEATURE_32591
                        strTemp = acc_GetStatusBarLabel(pDialData->pCallInfo->GetAccountID());
                        if (strTemp.empty())
#endif
                        {
                            int iDisplayMethodOnDialing = _TalkUIDelegate.GetDisplayMethodOnDialing();

                            //优先显示Label
                            if (1 == iDisplayMethodOnDialing)
                            {
                                strTemp = GetLabel(pDialData->pCallInfo->GetAccountID());

                            }
                            else if (2 == iDisplayMethodOnDialing)
                            {
                                strTemp = GetDisplayName(pDialData->pCallInfo->GetAccountID());

                            }
                            else
                            {
                                strTemp = GetUserName(pDialData->pCallInfo->GetAccountID());
                            }

                            //如果获取的UserName 是空的 那么就不应该显示  有的时候 账号Label、DisplayName有填  但是UserName 没写 这个时候按dsskey可能还是可以进入拨号界面的
                            //这个时候title 就不应该显示有内容

                            if (GetUserName(pDialData->pCallInfo->GetAccountID()).empty())
                            {
                                strTemp = "";
                            }
                        }
                    }

                    //这边就不需要翻译了  因为不是人名就是号码
                    strTitle = strTemp;
#if IF_BUG_25367
                    commonUnits_FormatNumber(strTitle);
#endif
                    //  _MainWND.SetTitleAccountDisplayName(toQString(acc_GetLabel(pDialData->pCallInfo->GetAccountID()).c_str()));
                }
            }
            break;
        case DIAL_UI_RINGFORWARD:
            {
                //forward to界面
                strTitle = yl::string(TRID_FORWARD_TO);
            }
            break;
        case DIAL_UI_TRANSFER:
            {
                //Transfer to界面
                strTitle = yl::string(TRID_TRANSFER_TO);
            }
            break;
        case DIAL_UI_CONFERENCE:
            {
                //会议界面
                strTitle = yl::string(TRID_CONFERENCE_WITH);

            }
            break;
        case DIAL_UI_CALLPARK:
            {
                //Park通话
                strTitle = yl::string(TRID_PARK_CALL_TO);

            }
            break;
        case DIAL_UI_GROUPCALLPARK:
            {
                strTitle = yl::string(TRID_GROUPPARK_CALL_TO);
            }
            break;
        case DIAL_UI_RETRIEVE_CALLPARK:
            {
                //取回Park
                strTitle = yl::string(TRID_RETRIEVE_PARK);

            }
            break;
        case DIAL_UI_DPICKUP:
            {
                //Pickup状态
                strTitle = yl::string(TRID_CALL_PICKUP);

            }
            break;
        default:
            {
                //未知类型
                chWARNING2(FALSE, commonAPI_FormatString("Dial ui type(%d) is unknown!",
                           pDialData->eDialUIType).c_str());
                strTitle = "";
            }
            break;
        }

#ifdef _T48

        //T48 在拨号和预拨号状态才需要改变title 其他状态 用默认的title 就可以了   默认的为 当前号码
        if (DIAL_UI_DIALING == m_eDialUIType || DIAL_UI_PREDIAL == m_eDialUIType)
        {
            m_strTitleInfo = strTitle;
            //  _TalkUIDelegate.UpdateTitleInfo(strTitle);
        }
#else
        _TalkUIDelegate.UpdateTitleInfo(strTitle);
#endif

    }


}

// 解析号码与名字
bool ParseDialNumber(LPCSTR lpszDial, DataDial2Logic & objData)
{
    if (NULL == lpszDial)
    {
        return false;
    }

    int nLen = strlen(lpszDial);
    LPCSTR lpszTmp = strrchr(lpszDial, '(');
    char cTmp = lpszDial[nLen - 1];
    // 存在 ‘(’且最后一个为‘)’
    if (NULL == lpszTmp
            || cTmp != ')')
    {
        objData.strNumber = lpszDial;
        objData.strName = "";
        return true;
    }
    // name
    char szArrayLeft[128] = "";
    strncpy(szArrayLeft, lpszDial, strlen(lpszDial) - strlen(lpszTmp));
    objData.strName = szArrayLeft;
    // number
    char szArrayRight[128] = "";
    strncpy(szArrayRight, lpszTmp + 1, strlen(lpszTmp) - 2);
    objData.strNumber = szArrayRight;

    return true;
}

// 通道键处理
bool CDlgDialingBase::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

#ifdef IF_FEATURE_HEADSETKEY_IN_TALK
    // http://192.168.1.99/Bug.php?BugID=19141
    // 焦点是通话界面，且耳麦打开的情况下不再处理channel按键
    if (configParser_GetConfigInt(kszHeadSetKeyInTalkSwitch) == 0
            && voice_GetHeadsetStatus()
            && (DIAL_UI_CALLPARK == m_eDialUIType)       //CALL PARK和 通话时同状态
       )
    {
        //通道键处理最后强制更新通道图标
        _TalkUIDelegate.UpdateChannelIcon();

        return FALSE;
    }
#endif

    // 这边不能再交给voice 处理了  因为在qT46Application::PreKeyHookBegin 函数中会处理一次！！！！！！
    //  bHandled = voice_OnKeyPress(iKey);

    // 双耳麦模式，摘挂机只更改通道，不再继续往下传
    if (voice_IsCallCenterMode()
            && (iKey == PHONE_KEY_HANDSET_OFF_HOOK
                || iKey == PHONE_KEY_HANDSET_ON_HOOK))
    {
        bHandled = true;
    }
    else
    {
        //如果是forward 那么要走forward的channelkey 流程
        if (DIAL_UI_RINGFORWARD == m_eDialUIType)
        {
            bHandled = OnForwardChannelKeyPress(iKey) || bHandled;
        }
        else if (DIAL_UI_CALLPARK == m_eDialUIType)
        {
            bHandled = OnCallParkChannelKeyPress(iKey) || bHandled;
        }
        else
        {
            bHandled = OnNormalChannelKeyPress(iKey) || bHandled;
        }
    }


    //通道键处理最后强制更新通道图标
    _TalkUIDelegate.UpdateChannelIcon();

    //这个要放在 通话处理之后， 因为这个时候可能 已经退出了通话界面了， 避免退到Idle后还出现音量条
    //需要放在更新通道 图标之后 因为 更新通道图标会把m_bIsShowVolume 的状态值 刷新
    if (PHONE_KEY_HANDFREE == iKey
            || PHONE_KEY_HANDSET == iKey
            || PHONE_KEY_HANDSET_OFF_HOOK == iKey
            || PHONE_KEY_HANDSET_ON_HOOK == iKey
            || PHONE_KEY_HEADSET_CTRL == iKey
       )
    {
        if (UIManager_IsInCallManager()
                && _TalkUIFrameMananger.GetCurrentFrameType() == FRAME_TYPE_CALLMANAGER)
        {
            TALK_UI_TYPE eType = GetTalkUIType();
            //摘手柄或者按免提导致声道切换，需要显示切换后声道的音量
            //通过模拟按键显示音量条
            if (eType != TALK_UI_RING)
            {
                if (_TalkUIFrameMananger.GetIsShowVolume())
                {
                    inputHook_SendKey(PHONE_KEY_VOLUME_DECREASE, true);

                    _TalkUIFrameMananger.SetShowVolumeStatus(false);
                }
            }
        }
    }

    return bHandled;

}

// 功能键处理
bool CDlgDialingBase::OnFunckeyPress(int iKey, bool bEXP)
{
    //播放按键音
    if (IsNeedToPlayKeyTone(iKey))
    {
        _TalkUIDelegate.PlayKeyTone(iKey);
    }

    switch (iKey)
    {
    case PHONE_KEY_UP:
        {
            //响应 按键Up 事件
            OnKeyEventUp();

//非T48 需要刷新softkey
#ifndef _T48

            RefreshSoftKey();
#endif
            return true;
        }
        break;
    case PHONE_KEY_DOWN:
        {
            //响应按键 down 事件
            OnKeyEventDown();
//非T48 需要刷新softkey
#ifndef _T48

            RefreshSoftKey();
#endif
            return true;

        }
        break;
    case PHONE_KEY_OK:
        {
            //这边主要是 因为有穿梭数据的存在所以要区分    如果都是调用TALK_UI_ACTION_SEND 的话就没有 穿梭数据 了
            //在OnUIAction中已经有对穿梭数据进行判断了 所以这边就不需要判断了

            //呼出号码
            OnUIAction(TALK_UI_ACTION_SEND);
            return true;
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            //Callpark 和 Forward 的时候 都不响应REDIAL键
            if (DIAL_UI_CALLPARK != m_eDialUIType && DIAL_UI_RINGFORWARD != m_eDialUIType)
            {
                OnUIAction(TALK_UI_ACTION_REDIAL);
            }

            return true;
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            //Callpark 和 Forward 的时候 都不响应
            if (DIAL_UI_CALLPARK != m_eDialUIType && DIAL_UI_RINGFORWARD != m_eDialUIType)
            {
                //Transfer按键
                OnUIAction(TALK_UI_ACTION_TRANSFER);
            }

            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            TALKUI_INFO("PHONE_KEY_CANCEL");
            //Cancel按键
            OnUIAction(TALK_UI_ACTION_CANCEL);
            return true;
        }
        break;
    case PHONE_KEY_MESSAGE:
        {
            ////呼出配置的默认语音帐号号码（如果帐号未配置，则进入默认账号的语音信箱设置）
            //SettingUI_VoiceMailCallOut(m_iCurrentAccountId);
            ////阻止mainWnd再处理message按下事件
            //return true;
            //呼出配置的默认语音帐号号码（如果帐号未配置，则进入默认账号的语音信箱设置）

            //Callpark 和 Forward 的时候 都不响应
            if (DIAL_UI_CALLPARK != m_eDialUIType && DIAL_UI_RINGFORWARD != m_eDialUIType)
            {
                OnUIAction(TALK_UI_ACTION_SMS);
            }

            //不管执行成不成功 都返回true 不继续往下执行
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
bool CDlgDialingBase::OnDigitkeyPress(int iKey)
{
    //播放按键音
    if (IsNeedToPlayKeyTone(iKey))
    {
        _TalkUIDelegate.PlayKeyTone(iKey);
    }


    yl::string strNumber = "";
    yl::string strDisplayName = "";

    //是发送的按键
    if (_TalkUIDelegate.IsDigitkeyAsSend(iKey))
    {
        //在transfer 状态  看看是否有穿梭数据
        if (DIAL_UI_TRANSFER == m_eDialUIType)
        {
            int iShuttleCallID = GetShttleCallID();
            if (iShuttleCallID > 0)
            {
                //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
                InputLineEditData();

                SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);

                return true;
            }
        }

        //联系人控件中有选择联系人  直接呼出
        if (IsSearchTableSelectItem())
        {
            OnUIAction(TALK_UI_ACTION_SEND);

            return true;
        }

        //获取相关数据
        //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
        InputLineEditData();

        //获取联系人号码和姓名
        GetInputContent(strNumber, strDisplayName);

        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;

        //不确定是否在这实现
        //如果编辑框有字符 # 号直接呼出
        //编辑框字符大于0 才需要判断是否是呼出键
        if (strNumber.length() > 0)
        {
            if (IsSendDigitKey(strNumber, iKey))
            {
                //如果是 测试模式 和 autop 模式需要  直接返回false 能够继续输入
                if (SESSION_PREDIAL == m_eSessionState)
                {
                    if (Autop_MatchAutoPCodePrefix(strNumber, (char)('0' + iKey - PHONE_KEY_0)) >= 0
                            || commonUnits_IsTestModePrefix(strNumber + GetDigitKeyString(iKey)))
                    {
                        return false;
                    }
                }

                OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }

        }
        else        //联系人号码为空
        {
            if (DIAL_UI_CALLPARK ==
                    m_eDialUIType)  //如果是在park  界面  可以按 呼出键 直接park到本机
                //如果用户需要按以keyassend 开头的特征码，那么必须先把keyassend的功能关闭
            {
                OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }
            else if (DIAL_UI_RETRIEVE_CALLPARK ==
                     m_eDialUIType) //为空的时候 可能是取回park 自己号码上的通话
            {
                OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }
            else if (DIAL_UI_DPICKUP == m_eDialUIType)
            {
                //这边pickup 的时候 即使编辑框为空发送DPICKUP事件 也不会有什么问题  而且PickUp 可能有默认的 code 所以为空的时候需要往下发
                //V73 可能在逻辑层对PICK拨号的SEND事件有做区分，但是V80在逻辑层没区分所以必须在UI区分
                OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }
        }
    }

    //交给UI处理 ， 输出字符
    if (OnUIDigitPress(iKey))
    {
        return true;
    }
    //if(NULL != m_pTalkWidget)
    //{
    //  if(m_pTalkWidget->OnDigitkeyPress(iKey))
    //  {
    //      return true;
    //  }
    //}

    return CDlgNormalTalkBase::OnDigitkeyPress(iKey);

}

//普通拨号界面通道键处理
bool CDlgDialingBase::OnNormalChannelKeyPress(int iKey)
{
    bool bHandled = FALSE;
    bool bCallOut = FALSE;

    yl::string strNumber = "";
    yl::string strDisplayName = "";

    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:                // 免提键
    case PHONE_KEY_HANDSET_OFF_HOOK:        // 摘机
    case PHONE_KEY_EHS_OPEN:
#ifdef IF_FEATURE_HEADSET_AS_CANCEL
    case PHONE_KEY_HEADSET_CTRL:
#endif
        {
            if (iKey == PHONE_KEY_HEADSET_CTRL
                    && configParser_GetConfigInt(kszHeadsetAsCancel) == 0
                    && configParser_GetConfigInt(kszHeadsetCtrlCall) == 0)
            {
                break;
            }

            if ((iKey == PHONE_KEY_HANDFREE
                    || configParser_GetConfigInt(kszHeadsetCtrlCall) == 1
                    && iKey == PHONE_KEY_HEADSET_CTRL)
                    && SESSION_PREDIAL != m_eSessionState)
            {
                // 预拨号界面的免提键才有呼出的作用，拨号界面的免提键改成退出拨号界面
                break;
            }

            //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
            InputLineEditData();
            //获取号码
            GetInputContent(strNumber, strDisplayName);

            // 如果号码不为空则呼出
            if (strNumber.length() > 0)
            {
                bCallOut = TRUE;

                // http://10.2.1.199/Bug.php?BugID=37176
                // 通过免提键呼出时，强制将通道切成免提
                if (iKey == PHONE_KEY_HANDFREE)
                {
                    voice_SwitchChannel(CHANNEL_HANDFREE);
                }
            }

        }
        break;
    default:
        break;
    }

    if (bCallOut)
    {
        //使用这个函数 呼出号码
        OnUIAction(TALK_UI_ACTION_SEND);
        //bHandled = TRUE;
        //DataDial2Logic objDialData;
        //if(NULL != m_pPswLineEdit)
        //{
        //  ParseDialNumber(m_pPswLineEdit->text().toUtf8().data(), objDialData);
        //}
        //
        //SendUIRequestAction(TALK_ACTION_SEND, 0, 0, &objDialData);
    }
    else
    {
        bHandled = CDlgNormalTalkBase::OnChannelKeyPress(iKey);

        //这边因为 BaseTalkUI不更新了 所以 这边要更新 TalkUIFrame也有更新 但是 这个窗口不在TalkUIFrame内 所以 这边必须更新

    }

    return bHandled;
}

//Forward 状态下 按键处理
bool CDlgDialingBase::OnForwardChannelKeyPress(int iKey)
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

            SendUIRequestAction(TALK_ACTION_ANSWER, 1);
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            //问题：Wildix T28\T26\T22 手柄不能挂掉还没应答的通话
            //修改：来电界面挂机时不执行 ExitRoutine()操作
            //http://10.2.1.199/Bug.php?BugID=16809

            // http://10.2.1.199/Bug.php?BugID=46003
            // 挂机不挂断通话，但要切换通道
            voice_SwitchChannel();
        }
        break;
    case PHONE_KEY_EHS_OPEN:
        {
            // 来电时打开EHS则接起来电
            SendUIRequestAction(TALK_ACTION_ANSWER, 1);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

//callpark 状态下 按键处理
bool CDlgDialingBase::OnCallParkChannelKeyPress(int iKey)
{
    bool bHandled = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (voice_IsCallCenterMode())
            {
                // callCenterMode交给基类处理
                bHandled = false;
            }
            else
            {
                // 通话中摘手柄时按免提键，则打开群听模式
                if (voice_GetHandsetStatus() && voice_GetHandfreeStatus())
                {
                    if (m_eTalkState == TS_TALK)
                    {
                        TALKUI_INFO("CTalkingUI : Open GroupListening");
                        if (1 == configParser_GetConfigInt(kszGroupListenInTalkingEnable))
                        {
                            voice_SwitchChannel(CHANNEL_GROUP);
                        }
                        else
                        {
                            voice_SwitchChannel(CHANNEL_HANDFREE);
                        }
                        bHandled = true;
                    }
                }
                else
                {
                    bHandled = false;
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            if (!IsChannelKeyToExit(iKey))
            {
                bHandled = true;
                voice_SwitchChannel();
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }
    if (!bHandled)
    {
        bHandled = CDlgNormalTalkBase::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

// 是否*、#呼出键
bool CDlgDialingBase::IsSendDigitKey(yl::string strEditContent, int iKey)
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

// 判断是否需要播放按键音
bool CDlgDialingBase::IsNeedToPlayKeyTone(int iKey)
{
    yl::string strEditData = "";

    //先从界面类中获取编辑框字符串
    strEditData = GetLineEditData();

    return IsNeedToPlayKeyTone(strEditData, iKey);
}

//实际判断硬按键是否播放按键音
bool CDlgDialingBase::IsNeedToPlayKeyTone(yl::string strEditContent, int iKey)
{
    // 根据配置判断呼出键是否要播放按键音
    if (strEditContent.length() > 0)
    {
        BOOL bSendKey = FALSE;

        if (iKey == PHONE_KEY_OK)
        {
            bSendKey = TRUE;
        }
        else if (IsSendDigitKey(strEditContent, iKey))
        {
            // *#呼出键
            bSendKey = TRUE;
        }

        // 是否播放呼出键按键音
        if (bSendKey)
        {
            return _TalkUIDelegate.IsVolumeSendKeySoundOn();
        }
    }

    // 拨号界面只播放数字键、删除键的按键音
    if (IsDigitKey(iKey))
    {
        return TRUE;
    }

    return FALSE;
}

// 判断Softkey是否需要播放按键音
bool CDlgDialingBase::IsSoftkeyNeedToPlayKeyTone(yl::string strSoftkeyAction)
{
    //如果是删除键
    if (TALK_UI_ACTION_DELETE == strSoftkeyAction)
    {
        yl::string strText = "";

        //这边获取编辑框中的字符就可以了 因为 是判断删除键是否响铃
        strText = GetLineEditData();

        //并且编辑框有数据 那么要响铃声
        if (strText.length() > 0)
        {
            return true;
        }

    }

    //并且是 呼出键
    if (TALK_UI_ACTION_SEND == strSoftkeyAction)
    {
        yl::string strNumber = "";
        yl::string strDisplayName = "";


        GetInputContent(strNumber, strDisplayName);

        //如果呼出键不需要响按键音 直接返回false
        if (!_TalkUIDelegate.IsVolumeSendKeySoundOn())
        {
            return false;
        }
        //如果有数据
        if (strNumber.length() > 0)
        {
            return true;
        }
        else if (GetShttleCallID() > -1 || IsSearchTableSelectItem())
        {
            return true;
        }
        else
        {
            if (DIAL_UI_CALLPARK ==
                    m_eDialUIType)  //如果是在park  界面  可以按 呼出键 直接park到本机
                //如果用户需要按以keyassend 开头的特征码，那么必须先把keyassend的功能关闭
            {
                return true;
            }
            else if (DIAL_UI_RETRIEVE_CALLPARK ==
                     m_eDialUIType) //为空的时候 可能是取回park 自己号码上的通话
            {
                return true;
            }
            else if (DIAL_UI_DPICKUP == m_eDialUIType)
            {
                return true;
            }
        }
    }

    return false;
}

//void CDlgDialingUI::showEvent(QShowEvent *event)
//{
//  // 刷新声道图标
//  _MainWND.SetTitleAccountIcon(::GetChannelIcon());
//
//  if (NULL != m_pPswLineEdit)
//  {
//      OnSearchTabNoFocus();
//  }
//}

//是否显示音量条
bool CDlgDialingBase::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                      QRect & rtVolume, yl::string & strBGPath) const
{
    iMin = TalkUIlogic_IsAllowSilenceDuringCall() ? 0 : 1;
    iMax = 15;
    return true;
}

//是否显示DTMF条
bool CDlgDialingBase::IsShowDtmfLine() const
{
    return false;
}

//是否显示加密图标,rectEncrypt hold the location of encrypt label
bool CDlgDialingBase::IsShowEncryptTip(void * pData, QRect & rectEncrypt)
{
    return true;
}

//输入所有输入框内容
bool CDlgDialingBase::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    return false;
}

void CDlgDialingBase::OnBackButtonClick()
{
    OnUIAction(TALK_UI_ACTION_ENDCALL);
}

//这个会重载
bool CDlgDialingBase::IsChangeSession()
{
    return true;
}

bool CDlgDialingBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL || pEvent == NULL)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            //http://10.3.5.199/Bug.php?BugID=65290
            //把删除弹出框清除
            //if (_TalkUIDelegate.GetMessageBoxCount() > 0)
            //{
            //  _TalkUIDelegate.RemoveAllMessageBox();
            //}

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();

            ////判断是否要响按键音
            //if(IsNeedToPlayKeyTone(nKey))
            //{
            //  voice_PlayKeyTone(nKey);
            //}

            // 判断Logic是否已处理过按键且要求不再继续往下传递
            SendUIRequestAction(TALK_ACTION_KEYPRESS, nKey, 0, NULL);


            //判断是否是通道图标 如果是通道图标处理完后直接返回true 不需要继续了
            if (voice_IsChannelKey(nKey))
            {
                return OnChannelKeyPress(nKey);
            }

            if (IsDigitKey(nKey))
            {
                return OnDigitkeyPress(nKey);
            }

            return OnFunckeyPress(nKey);
        }
        break;
    case QEvent::MouseButtonPress:
        {
            //将点击事件传递给具体的Logic层处理
            SendUIRequestAction(TALK_ACTION_SCREEN_CLICK);
        }
        break;
    default:
        {
            //none
        }
        break;
    }

    return false;
}

//处理UI事件
bool CDlgDialingBase::OnUIAction(const QString & strAction)
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    TALKUI_INFO("DialUI-OnUIAction(%s)", strAction.toUtf8().data());
    if (strAction == TALK_UI_ACTION_TEXT_CHANGE)
    {
        //获取相关数据
        //获取联系人号码和姓名
        GetInputContent(strNumber, strDisplayName);

        //文本改变事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;

        SendUIRequestAction(TALK_ACTION_TEXT_CHANGE, 0, 0, &objDialData);
    }
    else if (strAction == TALK_UI_ACTION_SEND)
    {
        //在transfer 状态
        if (DIAL_UI_TRANSFER == m_eDialUIType)
        {
            int iShuttleCallID = GetShttleCallID();
            if (iShuttleCallID > 0)
            {
                //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
                InputLineEditData();

                SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);

                return true;
            }
        }

        //获取相关数据
        //把预编辑字符输出
        InputLineEditData();
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
        //获取联系人号码和姓名
        GetInputContent(strNumber, strDisplayName);


        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;

        //暂时先保留
        //// 上锁后需要判断是否密码拨号
        //if (keyGuard_IsLock()
        //  && !talklogic_IsEmergency(objDialData.strNumber.c_str()))
        //{
        //  // 用于防抖,防止用户多次操作触发多个弹出框
        //  static bool bPopupNote = false;

        //  if (!bPopupNote)
        //  {
        //      bPopupNote = true;
        //      MsgBox_PopupMessageBox(LANG_TRANSLATE("Not Emergency Number!"), PB_TYPE_NOTE, SHORT_TIME);
        //      bPopupNote = false;
        //  }
        //  return false;
        //}

        //为park的时候 无论输入框是否为空 都需要发送send 事件  因为park 的时候可能有一个默认的code
        if (DIAL_UI_CALLPARK == m_eDialUIType)
        {
            SendUIRequestAction(TALK_ACTION_SEND, 0, 0, &objDialData);
        }
        else if (DIAL_UI_RETRIEVE_CALLPARK ==
                 m_eDialUIType) //为空的时候 可能是取回park 自己号码上的通话
        {
            SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK, 0, 0, &objDialData);
        }
        else if (DIAL_UI_GROUPCALLPARK == m_eDialUIType)
        {
            SendUIRequestAction(TALK_ACTION_SEND, 0, 0, &objDialData);
        }
        else if (DIAL_UI_DPICKUP == m_eDialUIType)
        {
            //这边pickup 的时候 即使编辑框为空发送DPICKUP事件 也不会有什么问题  而且PickUp 可能有默认的 code 所以为空的时候需要往下发
            //V73 可能在逻辑层对PICK拨号的SEND事件有做区分，但是V80在逻辑层没区分所以必须在UI区分

            SendUIRequestAction(TALK_ACTION_DPICKUP, 0, 0, &objDialData);
        }
        else if (!objDialData.strNumber.empty())
        {
            SESSION_STATE state = talklogic_GetSessionStateBySessionID(talklogic_GetFocusedSessionID());

            if (SESSION_PRETRAN == state && m_bSearchTransfer)
            {
                m_bSearchTransfer = false;

                CTransferNumSelector * pNumberSelector = GetTransferNumSelector();

                if (NULL == pNumberSelector)
                {
                    return false;
                }

                pNumberSelector->Init(TRAN_NUM_OTHER);
                pNumberSelector->SetName(strDisplayName);
                pNumberSelector->SetNumber(strNumber);

                MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
                MsgBoxSoftkey skSend(LANG_TRANSLATE(TRID_SEND), MessageBox_Send);
                MsgBoxSoftkey skBTransfer(LANG_TRANSLATE(TRID_BLIND_TRANSFER), MessageBox_BTransfer);

                VecMsgBoxSoftkey vecSoftKey;

                vecSoftKey.push_back(skBTransfer);
                vecSoftKey.push_back(skSend);
                vecSoftKey.push_back(skCancel);

                QString strTitle = toQString(strDisplayName, true);


                CMessageBoxBase * pSelectBox = MessageBox_ShowSelectBox(this,
                                               vecSoftKey,
                                               SELECT_POP_BOX_TYPE_NORMAL,
                                               pNumberSelector,
                                               strTitle,
                                               DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE);

                if (NULL != pSelectBox)
                {
                    pSelectBox->SetEventFilter(pNumberSelector);
                }
            }
            else
            {
                SendUIRequestAction(TALK_ACTION_SEND, 0, 0, &objDialData);
            }
        }
    }
    else if (strAction == TALK_UI_ACTION_TRANSFER)
    {
        //在transfer 状态  穿梭数据
        if (DIAL_UI_TRANSFER == m_eDialUIType)
        {
            int iShuttleCallID = GetShttleCallID();
            if (iShuttleCallID > 0)
            {
                //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
                InputLineEditData();
                SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);

                return true;
            }
        }

        //获取相关数据
        //把预编辑字符输出
        InputLineEditData();
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
        //获取联系人号码和姓名
        GetInputContent(strNumber, strDisplayName);

        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;

        ////Transfer事件
        //DataDial2Logic objDialData;
        //objDialData.strNumber = qReturnEdit(m_pPswLineEdit).toUtf8().data();

        //if (m_pSearchTable != NULL
        //  && m_pSearchTable->IsSelectItem())
        //{
        //  //焦点为搜索栏，获取焦点数据
        //  SearchItemData tSearchItemData = m_pSearchTable->GetCurrentItemContent();
        //  bool bOk = false;
        //  int nShuttleCallId = tSearchItemData.varData.toInt(&bOk);
        //  if (bOk && nShuttleCallId > 0)
        //  {
        //      //穿梭
        //      TALKUI_INFO("Dial UI: Enter shuttle!");
        //      SendUIRequestAction(TALK_ACTION_TRAN, nShuttleCallId);
        //      return true;
        //  }
        //  objDialData.strName = tSearchItemData.strDisplay.toUtf8().data();
        //  objDialData.strNumber = tSearchItemData.strNumber.toUtf8().data();
        //}

        // 号码不为空的时候进行Transfer操作  盲转
        if (!objDialData.strNumber.empty())
        {
            SendUIRequestAction(TALK_ACTION_TRAN, 0, 0, &objDialData);
        }
    }
    else if (strAction == TALK_UI_ACTION_CONFERENCE)
    {
        if (DIAL_UI_CONFERENCE == m_eDialUIType)
        {
            int iShuttleCallID = GetShttleCallID();
            if (iShuttleCallID > 0)
            {
                //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
                InputLineEditData();
                SendUIRequestAction(TALK_ACTION_CONF, iShuttleCallID);

                return true;
            }
        }

        //把联系人控件中的数据放到编辑框中
        SetLineEditDataBySearchTable();

        GetInputContent(strNumber, strDisplayName);


        //Conference事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;

        //  GetDataToLogic(objDialData);
        // 号码不为空的时候进行Conference操作
        if (!objDialData.strNumber.empty())
        {
            SendUIRequestAction(TALK_ACTION_CONF, 0, 0, &objDialData);
        }
    }
    else if (strAction == TALK_UI_ACTION_REDIAL)
    {
        //重拨事件
        ////----确认
        SendUIRequestAction(TALK_ACTION_REDIAL);
    }
    //UI去实现
    //else if (strAction == TALK_UI_ACTION_IME)
    //{
    //  //切换输入法
    //  qShift2NextIME(m_pPswLineEdit);

    //  ResetIMEText();
    //}
    //else if (strAction == TALK_UI_ACTION_DELETE)
    //{
    //  //因为qOnBackspaceClicked 会触发OnTextChange， 所以在qOnBackspaceClicked之前先调用 SendUIRequestAction(TALK_ACTION_KEYPRESS
    //  //向Logic层发出删除键的按键
    //  //这个动作当前没有用处， 暂时仍然保留
    //  SendUIRequestAction(TALK_ACTION_KEYPRESS, PHONE_KEY_BACKSPACE, 0);

    //  //调用 几面类进行处理
    //  OnUIActionPress(TALK_UI_ACTION_DELETE);
    //}
    else if (strAction == TALK_UI_ACTION_CANCEL)
    {
        //Cancel退出事件
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
    else if (strAction == TALK_UI_ACTION_LINE)
    {
        //输出预编辑字符
        InputLineEditData();
        //把联系人控件中的联系人号码 设置到编辑框中
        SetLineEditDataBySearchTable();


        //if (m_pSearchTable != NULL
        //  && m_pSearchTable->IsSelectItem())
        //{
        //  m_pPswLineEdit->setText(m_pSearchTable->GetCurrentItemNum());
        //}
        ////TalkUI_OpenSelectPage(this->GetSessionId());
    }
    else if (strAction == TALK_UI_ACTION_POOL)
    {
        talkui_OpenPool();
    }
    else if (strAction == TALK_UI_ACTION_SMS)
    {
        SendUIRequestAction(TALK_ACTION_SMS);
    }
    else if (strAction == TALK_UI_ACTION_PARK)
    {
        //Park
        InputLineEditData();
        SetLineEditDataBySearchTable();
        GetInputContent(strNumber, strDisplayName);


        //qReturnEdit(m_pPswLineEdit);

        //if (m_pSearchTable != NULL
        //  && m_pSearchTable->IsSelectItem())
        //{
        //  m_pPswLineEdit->setText(m_pSearchTable->GetCurrentItemNum());
        //}

        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;
        //GetDataToLogic(objDialData);

        SendUIRequestAction(TALK_ACTION_PARK, 0, 0, &objDialData);
    }
    else if (TALK_UI_ACTION_GROUP_PARK == strAction)
    {
        InputLineEditData();
        SetLineEditDataBySearchTable();
        GetInputContent(strNumber, strDisplayName);


        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;
        //GetDataToLogic(objDialData);

        SendUIRequestAction(TALK_ACTION_GROUP_PARK, 0, 0, &objDialData);
    }
    else if (strAction == TALK_UI_ACTION_RETRIEVE_PARK)
    {
        //Retrieve Park
        //qReturnEdit(m_pPswLineEdit);

        //if (m_pSearchTable != NULL
        //  && m_pSearchTable->IsSelectItem())
        //{
        //  m_pPswLineEdit->setText(m_pSearchTable->GetCurrentItemNum());
        //}

        InputLineEditData();
        SetLineEditDataBySearchTable();
        GetInputContent(strNumber, strDisplayName);


        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;
        //GetDataToLogic(objDialData);

        SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK, 0, 0, &objDialData);
    }
    else if (strAction == TALK_UI_ACTION_DPICKUP)
    {
        //把预编辑字符输出
        InputLineEditData();
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
        //获取联系人号码和姓名
        GetInputContent(strNumber, strDisplayName);


        //qReturnEdit(m_pPswLineEdit);

        //if (m_pSearchTable != NULL
        //  && m_pSearchTable->IsSelectItem())
        //  {
        //      m_pPswLineEdit->setText(m_pSearchTable->GetCurrentItemNum());
        //  }

        //呼出事件
        DataDial2Logic objDialData;
        objDialData.strNumber = strNumber;
        objDialData.strName = strDisplayName;
        //GetDataToLogic(objDialData);

        // 号码不为空的时候呼出, 或者当前非Dpickup模式，而变为Pickup模式
        //if (!objDialData.strNumber.empty()
        //  || (m_eDialUIType != DIAL_UI_DPICKUP))
        //{
        //这边pickup 的时候 即使编辑框为空发送DPICKUP事件 也不会有什么问题  而且PickUp 可能有默认的 code 所以为空的时候需要往下发
        //V73 可能在逻辑层对PICK拨号的SEND事件有做区分，但是V80在逻辑层没区分所以必须在UI区分
        SendUIRequestAction(TALK_ACTION_DPICKUP, 0, 0, &objDialData);
        //}
    }
    else if (strAction == TALK_UI_ACTION_GPICKUP)
    {
        // 从逻辑层拿到数据
        SendUIRequestAction(TALK_ACTION_GPICKUP);
    }
    else if (strAction == TALK_UI_ACTION_OPENDSSKEY)
    {


#ifdef _T48
        if (keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY))
        {
            dsskeyUI_OpenFullDsskeyDlg();
        }
#endif
    }
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    else if (strAction == TALK_UI_ACTION_CALL_INITIATION)
    {
        InputLineEditData();
        SetLineEditDataBySearchTable();

        int iCount = BWExeAssis_GetExecutiveListCount(m_iCurrentAccountId);
        if (iCount > 1)
        {
            //用静态变量
            static CExecutiveSelector selector;
            selector.Init(m_iCurrentAccountId, GetSessionId());

            QString strTitle = LANG_TRANSLATE(TRID_SELECT_EXECUTIVE_NUMBER);

            //更换弹出框
            VecMsgBoxSoftkey vecSoftKey;
            vecSoftKey.push_back(MsgBoxSoftkey(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel));
            vecSoftKey.push_back(MsgBoxSoftkey(LANG_TRANSLATE(TRID_SELECT), MessageBox_OK));

            MessageBox_ShowSelectBox(&selector, vecSoftKey, SELECT_POP_BOX_TYPE_NORMAL, &selector, strTitle);
        }
        else if (iCount == 1)
        {
            SendUIRequestAction(TALK_ACTION_CALL_INITIATION, 1);
        }
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

CTransferNumSelector * CDlgDialingBase::GetTransferNumSelector()
{
    if (NULL != m_pTransferNumSelector)
    {
        return m_pTransferNumSelector;
    }
    m_pTransferNumSelector = new CTransferNumSelector;

    return m_pTransferNumSelector;
}

void CDlgDialingBase::ReleaseSelector()
{
    if (m_pTransferNumSelector)
    {
        delete m_pTransferNumSelector;
        m_pTransferNumSelector = NULL;
    }
}

void CDlgDialingBase::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetType())
    {
    case MESSAGEBOX_SELECT:
        {
            int nAction = pMessageBox->GetID();

            if (DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE == nAction)
            {
                // transfer 类型
                CTransferNumSelector * pNumberSelector = GetTransferNumSelector();

                if (NULL == pNumberSelector)
                {
                    break;
                }

                yl::string strNumber;

                if (!pNumberSelector->GetNumber(strNumber))
                {
                    break;
                }

                DataDial2Logic objDialData;
                objDialData.strNumber = strNumber;
                objDialData.strName = pNumberSelector->GetName();
                if (MessageBox_Send == pMessageBox->GetResult())
                {
                    SendUIRequestAction(TALK_ACTION_SEND, 0, 0, &objDialData);
                }
                else if (MessageBox_BTransfer == pMessageBox->GetResult())
                {
                    SendUIRequestAction(TALK_ACTION_TRAN, 0, 0, &objDialData);
                }
                else if (MessageBox_Cancel == pMessageBox->GetResult())
                {
                    UpdateWnd();
                }
                else
                {
                    break;
                }
            }
        }
    default:
        break;
    }
}

void CDlgDialingBase::OnTextChanged(const QString & sText)
{
    //先保存搜索key

    m_strSearchData = sText;


    // 优化界面速度，第一次进入的时候延迟搜索
    if (m_bFirstTextChange)
    {
        m_bFirstTextChange = false;
        QTimer::singleShot(100, this, SLOT(DelayProcessTextChanged()));
    }
    else
    {
        DelayProcessTextChanged();
    }
}

void CDlgDialingBase::DelayProcessTextChanged()
{
    //先搜索
    OnSearch();
}

void CDlgDialingBase::resizeEvent(QResizeEvent * resizeEvent)
{
    //if(NULL != m_pTalkWidget)
    //{
    //  QRect rectWidget(0, 0, this->width(), this->height());
    //  m_pTalkWidget->SetWidgetArea(rectWidget);
    //}
}

//搜索延时定时器响应槽
void CDlgDialingBase::OnSearch()
{
    SearchContact(m_strSearchData);
}

//拨号界面按上下键选择搜索控件中的联系人时改变Softkey（第一个Pool改为Send）
void CDlgDialingBase::RefreshSoftKey()
{
    emit RequestRefreshSoftkey();
}

////Widget 回调该逻辑使用
//void CDlgDialingBase::OnTalkWidgetAction(TALKWIDGET_CALLBACK_TYPE talkWidgetCallBackType)
//{
//  switch(talkWidgetCallBackType)
//  {
//  case TCT_DIALING_DELETE:    //拨号界面按删除键
//      {
//          //判断需要播放 softkey 的按键音
//          if(IsSoftkeyNeedToPlayKeyTone(TALK_UI_ACTION_DELETE))
//          {
//              //播放按键音
//              PlaySoftKeyTone(TALK_UI_ACTION_DELETE);
//          }
//
//          OnUIAction(TALK_UI_ACTION_DELETE);
//
//          ////因为qOnBackspaceClicked 会触发OnTextChange， 所以在qOnBackspaceClicked之前先调用 SendUIRequestAction(TALK_ACTION_KEYPRESS
//          ////向Logic层发出删除键的按键
//          ////这个动作当前没有用处， 暂时仍然保留
//          //SendUIRequestAction(TALK_ACTION_KEYPRESS, PHONE_KEY_BACKSPACE, 0);
//
//          ////调用 几面类进行处理
//          //if(NULL != m_pTalkWidget)
//          //{
//          //  m_pTalkWidget->OnWidgetAction(TALK_UI_ACTION_DELETE);
//          //}
//      }
//      break;
//  case TCT_DIALING_DPICKUP:
//      {
//          OnUIAction(TALK_UI_ACTION_DPICKUP);
//      }
//      break;
//  case TCT_DIALING_GPICKUP:
//      {
//          OnUIAction(TALK_UI_ACTION_GPICKUP);
//      }
//      break;
//  case TCT_DIALING_SEND:
//      {
//          OnUIAction(TALK_UI_ACTION_SEND);
//      }
//      break;
//  case TCT_DIALING_OPENDSSKEY:
//      {
//          OnUIAction(TALK_UI_ACTION_OPENDSSKEY);
//      }
//      break;
//  case TCT_DIALING_TEXTCHANGE:
//      {
//          //调用OnTextChange 函数
//          OnTextChanged("");
//      }
//      break;
//  case TCT_DIALING_TRANSFERBYCALLID:      //根据穿梭数据 转接通话
//      {
//          int iShuttleCallID = GetShttleCallID();
//          if(iShuttleCallID > 0)
//          {
//              //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
//              InputLineEditData();
//              SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);
//          }
//
//      }
//      break;
//  default:
//      break;
//
//  }
//
//}

// 是否运行自定义softkey
bool CDlgDialingBase::IsAllowDefineSoftkey()
{
    if (m_eDialUIType == DIAL_UI_RINGFORWARD
            || m_eDialUIType == DIAL_UI_CONFERENCE
            || m_eDialUIType == DIAL_UI_DPICKUP
            || m_eDialUIType == DIAL_UI_RETRIEVE_CALLPARK
            || m_eDialUIType == DIAL_UI_CALLPARK
            || m_eDialUIType == DIAL_UI_GROUPCALLPARK)
    {
        return false;
    }

    return true;
}

bool CDlgDialingBase::IsCanSelectLineBtn()
{
    return (m_eDialUIType == DIAL_UI_DIALING
            || m_eDialUIType == DIAL_UI_PREDIAL);
}

//搜索联系人结果返回
LRESULT CDlgDialingBase::DealSuperSearchMessage(msgObject & refMessage)
{
    return 0;
}

//dialingui 界面函数
//发给UI的动作
void CDlgDialingBase::OnUIActionPress(yl::string strAction)
{
    if (TALK_UI_ACTION_DELETE == strAction)
    {
        //判断需要播放 softkey 的按键音
        if (IsSoftkeyNeedToPlayKeyTone(TALK_UI_ACTION_DELETE))
        {
            //播放按键音
            PlaySoftKeyTone(TALK_UI_ACTION_DELETE);
        }

        //发送消息给逻辑层
        SendUIRequestAction(TALK_ACTION_KEYPRESS, PHONE_KEY_BACKSPACE, 0);
    }
    else if (TALK_UI_ACTION_SEND == strAction)      //send
    {
        //判断需要播放 softkey 的按键音
        if (IsSoftkeyNeedToPlayKeyTone(TALK_UI_ACTION_SEND))
        {
            //播放按键音
            PlaySoftKeyTone(TALK_UI_ACTION_SEND);
        }

        OnUIAction(TALK_UI_ACTION_SEND);
    }
    else if (TALK_UI_ACTION_TRANSFER == strAction)
    {
        OnUIAction(TALK_UI_ACTION_TRANSFER);
    }
    else if (TALK_UI_ACTION_DPICKUP == strAction)   //DPickUp
    {
        OnUIAction(TALK_UI_ACTION_DPICKUP);
    }
    else if (TALK_UI_ACTION_GPICKUP == strAction)   //GPickUp
    {
        OnUIAction(TALK_UI_ACTION_GPICKUP);
    }
    else if (TALK_UI_ACTION_RETRIEVE_PARK == strAction)
    {
        OnUIAction(TALK_UI_ACTION_RETRIEVE_PARK);
    }
    else if (TALK_UI_ACTION_IME == strAction)   //输入法  直接内部切换就可以了
    {
        OnUIAction(TALK_UI_ACTION_IME);
    }
    else if (TALK_UI_ACTION_OPENDSSKEY == strAction)    //打开DSSKEY
    {
        OnUIAction(TALK_UI_ACTION_OPENDSSKEY);
    }
    else if (TALK_UI_ACTION_CALL_INITIATION)
    {
        OnUIAction(TALK_UI_ACTION_CALL_INITIATION);
    }
}
