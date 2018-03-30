#include "dialprocessor.h"
#include "talkuilogiccommon.h"
#include "talkuilogicframemanager.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "feature/include/modfeature.h"
#include "uilogicdialuibase.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//mod-bluetooth
#ifdef IF_OLD_BLUETOOTH
#include "wireless/bluetoothOld/include/modbluetooth.h"
#else
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "talk/talklogic/src/bttalkmanager.h"
#endif // IF_OLD_BLUETOOTH
#endif

#if IF_TALKUI_PSTN
#include "account/include/modaccount.h"
#endif

#if IF_BUG_27560
#include "extended/actionuri_url/include/modactionuri_url.h"
#endif

namespace
{
#define LINE_SELECT_ID          100
#define CALL_INITIATION_ID          101
}

//处理默认账号改变消息
LRESULT OnDialDefaultAccountChangeMessage(msgObject & refMessage);

LRESULT OnDialDefaultAccountChangeMessage(msgObject & refMessage)
{
    CBaseTalkUIProcessor * pTalkUIProcessor = _TalkUILogicFrameManager.GetCurrentTalkUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        CDialProcessor * pDialProcessor = (CDialProcessor *)pTalkUIProcessor;

        return pDialProcessor->OnDefaultAccountChange(refMessage);

    }

    return 0;
}


CDialProcessor::CDialProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_DIAL)
{
    //上次保存的 搜索字符串变换数据
    m_strLastKey = "";

    m_pTalkUI = NULL;

    m_bGPickUpSoftkeySet = false;
    m_bRetrieveSoftkeySet = false;
    m_bDPickUpSoftkeySet = false;
    m_bZPickUpSoftkeySet = false;
}

CDialProcessor::~CDialProcessor()
{
    TALKUI_INFO("CDialProcessor::~CDialProcessor()  this=0x%x", (int)this);
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CDialProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    //上次保存的 搜索字符串变换数据
    m_strLastKey = "";

    m_objDialUIData.Reset();

    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                          OnDialDefaultAccountChangeMessage);
}

//释放前解除UI绑定前调用
void CDialProcessor::Uninit()
{
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                            OnDialDefaultAccountChangeMessage);

    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CDialProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicDialUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CDialProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取拨号界面类型
DIAL_UI_TYPE CDialProcessor::GetDialUIType()
{
    return m_objDialUIData.m_eDialUIType;
}

//transfer 状态下 获取当前通话联系人信息
CContactInfo CDialProcessor::GetTransferContactInfo()
{
    return m_objDialUIData.m_objTransferContactInfo;
}

//获取保存的数据
//保存的UI数据
CDialUIData * CDialProcessor::GetDialUIData()
{
    return &m_objDialUIData;
}


//获取当前通话账号
int CDialProcessor::GetSipAccount()
{
    return m_objDialUIData.m_iCurrentAccountID;
}

//处理 删除
void CDialProcessor::DealDelete()
{
    //发送消息给逻辑层
    SendUIRequestAction(TALK_ACTION_KEYPRESS, PHONE_KEY_BACKSPACE, 0);
}

//处理cancel   可以给talkuidial调用
void CDialProcessor::DealCancel()
{
    int nProcessorSize = _TalkUIProcessorList.GetProcessorCount();
    if (nProcessorSize > 1)
    {
        SendUIRequestAction(TALK_ACTION_CANCEL);
    }
}

//处理 ui 一些action
void CDialProcessor::DealUIAction(yl::string strUIAction)
{
    if (TALK_UI_ACTION_DELETE == strUIAction)
    {
        //判断需要播放 softkey 的按键音
        if (IsSoftkeyNeedToPlayKeyTone(ST_DELETE))
        {
            //播放按键音
            _TalkUILogicDelegate.PlayKeyTone(ST_DELETE);
        }

        DealDelete();

    }
    else if (TALK_UI_ACTION_SEND == strUIAction
             || TALK_UI_ACTION_ADIAL == strUIAction)     //send
    {
        //判断需要播放 softkey 的按键音
        if (IsSoftkeyNeedToPlayKeyTone(ST_SEND))
        {
            //播放按键音
            _TalkUILogicDelegate.PlayKeyTone(ST_SEND);
        }

        OnSend(TALK_UI_ACTION_ADIAL == strUIAction);
    }
    else if (TALK_UI_ACTION_TRANSFER == strUIAction)
    {
        OnTransfer();
    }
    else if (TALK_UI_ACTION_DPICKUP == strUIAction) //DPickUp
    {
        OnDPickUp();
    }
    else if (TALK_UI_ACTION_GPICKUP == strUIAction) //GPickUp
    {
        OnGPickUp();
    }
    else if (TALK_UI_ACTION_RETRIEVE_PARK == strUIAction)
    {
        OnRetrievePark();
    }
}

//是否允许切换账号
//t48 / t49 在拨号和预拨号界面是允许切换账号的， 其他状态下不允许切换   就是左上角点击会出现账号列表
//其他机型也有切换账号的功能， 是在softkey 上体现
bool CDialProcessor::IsAllowChangeAccount()
{
    if ((DIAL_UI_DIALING == m_objDialUIData.m_eDialUIType)
            || (DIAL_UI_PREDIAL == m_objDialUIData.m_eDialUIType)
       )
    {
        return true;
    }

    return false;
}

//处理账号改变消息
LRESULT CDialProcessor::OnDefaultAccountChange(msgObject & refMessage)
{
    if (ACCOUNT_DEFAULT_CHANGED == refMessage.message
            //  && UIManager_ExistCallManager()
            && m_objDialUIData.m_iCurrentAccountID != acc_GetDefaultAccount())
    {
#if IF_TALKUI_DIAL_CHANGEACCOUNT_CALLOUT
        //dlgbasetalkui 发起
        //t46账号改变需要呼出， t48 账号改变不需要呼出
        //t46 改变账号时在 talkpool界面改变的 而t48 直接是在mainwnd的title中改变的。
        ChangeAccount(acc_GetDefaultAccount(), true);
        //只要发送这个消息给逻辑层就可以了  逻辑层会再次刷新拨号界面
#else
        ChangeAccount(acc_GetDefaultAccount(), false);
#endif
    }

    return 0;
}

//设置数据
void CDialProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
//  m_objDialUIData.Reset();

    DataDial * pDialData = (DataDial *)(pData);

    //拨号UI的类型改变
    m_objDialUIData.m_eDialUIType = pDialData->eDialUIType;

    //设置穿梭数据
    m_objDialUIData.m_listTranCallInfo = pDialData->listTranCallInfo;
    //会议数据
    m_objDialUIData.listConfCallInfo = pDialData->listConfCallInfo;

    //设置拨号号码
    m_objDialUIData.m_strNumber = pDialData->strNumber;

    //设置拨号按键值
    m_objDialUIData.m_iKeyCode =
        pDialData->iKeyCode;       //将特定按键长按后转换为另一字符的需求中，需要用到该成员

#if IF_FEATURE_DIAL_ON_HOLD
    m_objDialUIData.m_bAllowTran = pDialData->bAllowTran;
#endif

    if (NULL != pDialData->pCallInfo)
    {
        //赋值账号ID
        m_objDialUIData.m_iCurrentAccountID = pDialData->pCallInfo->GetAccountID();

        //设置  主要是用于在 callparkchannelkey 函数中处理
        m_objDialUIData.m_eTalkState = pDialData->pCallInfo->m_eTalkState;

        m_objDialUIData.m_eCallLineType = pDialData->pCallInfo->GetCallLineType();
    }
}

// 通道键处理
bool CDialProcessor::OnChannelKeyPress(int iKey)
{
    //如果是forward 那么要走forward的channelkey 流程
    if (DIAL_UI_RINGFORWARD == m_objDialUIData.m_eDialUIType)
    {
        return OnForwardChannelKeyPress(iKey);
    }
    else if (DIAL_UI_CALLPARK == m_objDialUIData.m_eDialUIType)
    {
        return OnCallParkChannelKeyPress(iKey);
    }
    else
    {
        return OnNormalChannelKeyPress(iKey);
    }

    return false;
}

// 功能键处理
bool CDialProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
#if IF_TALKUI_BLUETOOTH_ENABLE
#ifdef _CP920
    case PHONE_KEY_BLUETOOTH:
        {
            TALKUI_INFO("KEY_BLUETOOTH press, Change to Bt account");

            bool bBtAccountAvailable = (Bluetooth_IsConnected() && Bluetooth_IsMobileConnected());
            bool bCurrentBtAccount = (Bluetooth_GetBtAccountID() == m_objDialUIData.m_iCurrentAccountID);
            if (bBtAccountAvailable && !bCurrentBtAccount)
            {
                TALKUI_INFO("Change to Bt account ID");
                _BTMobileManager.EnterDialUI("");
            }
            return true;
        }
        break;
#endif
#endif
    case PHONE_KEY_OK:
        {
            //这边主要是 因为有穿梭数据的存在所以要区分    如果都是调用TALK_UI_ACTION_SEND 的话就没有 穿梭数据 了
            //在OnUIAction中已经有对穿梭数据进行判断了 所以这边就不需要判断了
            //呼出号码
            OnSend();
            return true;
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            //Callpark 和 Forward 的时候 都不响应REDIAL键
            if (DIAL_UI_CALLPARK != m_objDialUIData.m_eDialUIType
                    && DIAL_UI_RINGFORWARD != m_objDialUIData.m_eDialUIType)
            {
                //重拨事件
                ////----确认
                SendUIRequestAction(TALK_ACTION_REDIAL);
            }

            return true;
        }
        break;
    case PHONE_KEY_TRANSFER:
        {
            //Callpark 和 Forward 的时候 都不响应
            if (DIAL_UI_CALLPARK != m_objDialUIData.m_eDialUIType
                    && DIAL_UI_RINGFORWARD != m_objDialUIData.m_eDialUIType)
            {
                //Transfer按键
                OnTransfer();
            }

            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            TALKUI_INFO("PHONE_KEY_CANCEL");

            //x键复用为 mute
            if (_TalkUILogicDelegate.IsMuteReplaceX())
            {
                SendUIRequestAction(TALK_ACTION_MUTE);

                return true;
            }

            //Cancel按键
            //Cancel退出事件
            SendUIRequestAction(TALK_ACTION_CANCEL);
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
            if (DIAL_UI_CALLPARK != m_objDialUIData.m_eDialUIType
                    && DIAL_UI_RINGFORWARD != m_objDialUIData.m_eDialUIType)
            {
                SendUIRequestAction(TALK_ACTION_SMS);
            }

            //不管执行成不成功 都返回true 不继续往下执行
            return true;
        }
        break;

    case PHONE_KEY_MUTE:
        {
            SendUIRequestAction(TALK_ACTION_MUTE);
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

// 数字键处理
bool CDialProcessor::OnDigitkeyPress(int iKey)
{
    yl::string strNumber = "";
    yl::string strDisplayName = "";

    //是发送的按键
    if (_TalkUILogicDelegate.IsDigitkeyAsSend(iKey))
    {
        //在transfer 状态  看看是否有穿梭数据
        if (DIAL_UI_TRANSFER == m_objDialUIData.m_eDialUIType)
        {
            int iShuttleCallID = GetShuttleCallID();
            if (iShuttleCallID > 0)
            {
                //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
                InputLineEditData();

                //调用send 函数
                OnSend();

                //      SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);

                return true;
            }
        }

        //联系人控件中有选择联系人  直接呼出
        if (IsSearchTableSelectItem())
        {
            OnSend();
            //  OnUIAction(TALK_UI_ACTION_SEND);

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
#ifndef _WIN32
#warning feature_MatchAutoPCodePrefix
#endif
                    /*
                    if (feature_MatchAutoPCodePrefix(strNumber, (char)('0'+iKey-PHONE_KEY_0)) >= 0
                        || commonUnits_IsTestModePrefix(strNumber + GetDigitKeyString(iKey)))
                    {
                        return false;
                    }
                    */
                }

                OnSend();
                //      OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }

        }
        else        //联系人号码为空
        {
            if (DIAL_UI_CALLPARK ==
                    m_objDialUIData.m_eDialUIType)  //如果是在park  界面  可以按 呼出键 直接park到本机
                //如果用户需要按以keyassend 开头的特征码，那么必须先把keyassend的功能关闭
            {
                OnSend();
                //  OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }
            else if (DIAL_UI_RETRIEVE_CALLPARK ==
                     m_objDialUIData.m_eDialUIType) //为空的时候 可能是取回park 自己号码上的通话
            {
                OnSend();
                //  OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }
            else if (DIAL_UI_DPICKUP == m_objDialUIData.m_eDialUIType)
            {
                //这边pickup 的时候 即使编辑框为空发送DPICKUP事件 也不会有什么问题  而且PickUp 可能有默认的 code 所以为空的时候需要往下发
                //V73 可能在逻辑层对PICK拨号的SEND事件有做区分，但是V80在逻辑层没区分所以必须在UI区分

                OnSend();
                //  OnUIAction(TALK_UI_ACTION_SEND);

                return true;
            }
        }
    }

    return false;
}

//softKey 按键处理
bool CDialProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    switch (eSoftkey)
    {
#ifdef IF_FEATURE_DIAL_ON_HOLD
    case ST_NEWCALL:
#endif
    case ST_SEND:
    case ST_OK:
    case ST_DIAL:
#ifndef _WIN32
#warning
#endif
//  case ST_CONSULT:
        {
            //把transfer穿梭数据的逻辑也放到这来
            OnSend();

            bHandled = true;
        }
        break;
    case ST_IMECHANGE:
        {
            if (m_pTalkUI)
            {
                m_pTalkUI->DealIMEAction();
            }
            bHandled = true;
        }
        break;
    case ST_DELETE:
        {
            if (m_pTalkUI)
            {
                m_pTalkUI->DealDeleteAction();

#if IF_FEATURE_DIAL_ON_HOLD
                if (m_objDialUIData.m_eDialUIType == DIAL_UI_ON_HOLD)
                {
                    yl::string strNumber = "";
                    yl::string strDisplayName = "";
                    bool bResult = GetInputContent(strNumber, strDisplayName);
                    if (bResult)
                    {
                        if (strNumber.empty())
                        {
                            SendUIRequestAction(TALK_ACTION_CANCEL);
                        }
                    }
                }
#endif
            }
            bHandled = true;
        }
        break;
    case ST_POOL:
        {
            if (m_pTalkUI)
            {
                m_pTalkUI->DealPool();
            }

            bHandled = true;
        }
        break;
    case ST_TRANSFER:
#ifndef _WIN32
#warning
#endif
//  case ST_DIRECT:
        {
            OnTransfer();

            bHandled = true;
        }
        break;
    case ST_CANCEL:
    case ST_ENDCALL:
        {
            SendUIRequestAction(TALK_ACTION_CANCEL);

            bHandled = true;
        }
        break;
    case ST_PARK:
        {
            OnPark();

            bHandled = true;
        }
        break;
    case ST_GROUP_PARK:
        {
            OnGPark();

            bHandled = true;
        }
        break;
    case ST_RETRIEVE_PARK:
        {
            OnRetrievePark();

            bHandled = true;

        }
        break;
    case ST_DPICKUP:
        {
            OnDPickUp();

            bHandled = true;

        }
        break;
    case ST_GPICKUP:
        {
            OnGPickUp();

            bHandled = true;

        }
        break;

#if IF_BUG_27796
    case ST_Z_PICKUP:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_PREFIX_PARK);
        }
        break;
#endif

    case ST_LINE:
        {
            SelectSelectLine();
        }
        break;

#if IF_TALKUI_PSTN
    case ST_SWITCH:
        {
            // 切换账号
            ListAccounts listAccounts;
            if (IsPSTNAccountAvailable())
            {
                listAccounts = acc_GetAvailableAccountList();
            }
            else
            {
                listAccounts = acc_GetAvailableAccountList(ACCOUNT_TYPE_SIP);
            }

            bool bBtAccountAvailable = false;
#if IF_TALKUI_BLUETOOTH_ENABLE
            if (IsBTAccountAvailable())
            {
                bBtAccountAvailable = (Bluetooth_IsConnected() && Bluetooth_IsMobileConnected());
            }
#endif

            if (listAccounts.size() + (bBtAccountAvailable ? 1 : 0) <= 1)
            {
                break;
            }
            bool bCurrentBtAccount = false;
#if IF_TALKUI_BLUETOOTH_ENABLE
            bCurrentBtAccount = (Bluetooth_GetBtAccountID() == m_objDialUIData.m_iCurrentAccountID);
#endif
            ListAccounts::iterator it = listAccounts.begin();
            if (bCurrentBtAccount)
            {
                TALKUI_INFO("Change from BtAccount to sip account");
                talklogic_EnterDialUI("", *it);
            }
            else
            {
                for (; it != listAccounts.end(); ++it)
                {
                    if (*it == m_objDialUIData.m_iCurrentAccountID)
                    {
                        break;
                    }
                }
                if (it == listAccounts.end())
                {
                    // 无法找到当前账号！
                    break;
                }
                ++it;
                if (it == listAccounts.end())
                {
#if IF_TALKUI_BLUETOOTH_ENABLE
                    if (bBtAccountAvailable)
                    {
                        TALKUI_INFO("Change to Bt account ID: %d", Bluetooth_GetBtAccountID());
                        _BTMobileManager.EnterDialUI("");
                    }
                    else
#endif
                    {
                        it = listAccounts.begin();
                        TALKUI_INFO("Change to next account ID: %d", *it);
                        SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE, *it, 1);
                    }
                }
                else
                {
                    TALKUI_INFO("Change to next account ID: %d", *it);
                    SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE, *it, 1);
                }
            }

            bHandled = true;
        }
        break;
#endif

    case ST_HISTORY:
        {
            if (NULL != m_pTalkUI)
            {
                m_pTalkUI->DealHistory();
            }
        }
        break;

#if IF_TALKUI_EXECUTIVE
    case ST_CALLINITIATION:
        {
            int iAccount = m_objDialUIData.m_iCurrentAccountID;
            int iCount = BWExeAssis_GetExecutiveListCount(iAccount);
            if (iCount > 1)
            {
                SelectCallInitiationLine();
            }
            else if (iCount == 1)
            {
                bHandled = SendUIRequestAction(TALK_ACTION_CALL_INITIATION, 1);
            }
        }
        break;
#endif

    default:
        break;
    }

    return bHandled;
}

//设置title
void CDialProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    DataDial * pDialData = (DataDial *)(pData);
    yl::string strTitle("");

    //根据拨号类型判断
    switch (pDialData->eDialUIType)
    {
    case DIAL_UI_CONFERENCE:
#if (IF_TALKUI_DSSKEY || IF_BUG_26666)
        //有dsskey的机型，可以在标题上显示conf的状态
        //没有dsskey的机型，则应优先显示账号
        {
            //会议界面
            strTitle = _UILOGIC_LANG_TRAN(TRID_CONFERENCE_WITH);
        }
        break;
#endif

    case DIAL_UI_DIALING:
    case DIAL_UI_PREDIAL:
    case DIAL_UI_ON_HOLD:
        {
            strTitle = GetAccountTitleText(pDialData);
        }
        break;
    case DIAL_UI_RINGFORWARD:
        {
            //forward to界面
            strTitle = _UILOGIC_LANG_TRAN(TRID_FORWARD_TO);
        }
        break;
    case DIAL_UI_TRANSFER:
        {
            //Transfer to界面
            strTitle = _UILOGIC_LANG_TRAN(TRID_TRANSFER_TO);
        }
        break;
    case DIAL_UI_CALLPARK:
        {
            //Park通话
            strTitle = _UILOGIC_LANG_TRAN(TRID_PARK_CALL_TO);
        }
        break;
    case DIAL_UI_GROUPCALLPARK:
        {
            strTitle = _UILOGIC_LANG_TRAN(TRID_GROUPPARK_CALL_TO);
        }
        break;
    case DIAL_UI_RETRIEVE_CALLPARK:
        {
            //取回Park
            strTitle = _UILOGIC_LANG_TRAN(TRID_RETRIEVE_PARK);
        }
        break;
    case DIAL_UI_DPICKUP:
        {
            //Pickup状态
            strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_PICKUP);
        }
        break;
    case DIAL_UI_BLUETOOTH_DIALING:     //蓝牙拨号状态  http://10.2.1.199/Bug.php?BugID=69647
#ifndef _WIN32
#warning
#endif
        //case DIAL_UI_BLUETOOTH_PREDIAL:
        {
            strTitle = _UILOGIC_LANG_TRAN(TRID_MOBILE_ACCOUNT);
        }
        break;

    default:
        {
            //未知类型
            chWARNING2(FALSE, commonAPI_FormatString("Dial ui type(%d) is unknown!", pDialData->eDialUIType).c_str());
            strTitle = "";
        }
        break;
    }

    m_strTitle = strTitle;
}

//设置图标列表
void CDialProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    DataDial * pDialData = (DataDial *)(pData);

    //这边显示的是信令加密
    if (true == pDialData->bEncrypt)
    {
        //添加sip加密图标
        AddTalkUIIcon(TALKUI_ICON_SIPENCRY, true);
    }

}

//给子界面设置数据
void CDialProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objDialUIData, eReason);
    }
}

void CDialProcessor::GetAlwaysBeforeSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    m_bGPickUpSoftkeySet = false;
    m_bRetrieveSoftkeySet = false;
    m_bDPickUpSoftkeySet = false;
    m_bZPickUpSoftkeySet = false;
}

void CDialProcessor::RelayoutSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    for (int i = 0; i < VecSoftkey.size();)
    {
        if (ST_GPICKUP == VecSoftkey[i])
        {
            m_bGPickUpSoftkeySet = true;
        }
        else if (ST_RETRIEVE_PARK == VecSoftkey[i])
        {
            m_bRetrieveSoftkeySet = true;
        }
        else if (ST_DPICKUP == VecSoftkey[i])
        {
            m_bDPickUpSoftkeySet = true;
        }
#if IF_BUG_27796
        else if (ST_Z_PICKUP == VecSoftkey[i])
        {
            m_bZPickUpSoftkeySet = true;
        }
#endif

        ++i;
    }
}

void CDialProcessor::GetAlwaysAfterSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (m_bUseCustomSoftkey
            && !_TalkUILogicDelegate.IsCustomDynamicSoftkeyOn())
    {
        return;
    }

#if IF_TALKUI_EXECUTIVE
    if (m_objDialUIData.m_eDialUIType == DIAL_UI_DIALING
            || m_objDialUIData.m_eDialUIType == DIAL_UI_PREDIAL
            || m_objDialUIData.m_eDialUIType == DIAL_UI_TRANSFER
            || m_objDialUIData.m_eDialUIType == DIAL_UI_CONFERENCE)
    {
        if (BWExeAssis_IsCallInitiationEnable(GetLineId()))
        {
            VecSoftkey.push_back(ST_CALLINITIATION);
        }
    }
#endif

    if (IsAllowGPickUpSoftkey())
    {
        m_bGPickUpSoftkeySet = true;
        VecSoftkey.push_back(ST_GPICKUP);
    }

    // 已经添加了Retrieve则不再添加
    if (IsAllowRetrieveParkSoftkey())
    {
        m_bRetrieveSoftkeySet = true;
        VecSoftkey.push_back(ST_RETRIEVE_PARK);
    }

    if (IsAllowDPickUpSoftkey())
    {
        m_bDPickUpSoftkeySet = true;
        VecSoftkey.push_back(ST_DPICKUP);
        //  AddSoftkey(objSoftkeyData, TAKL_UI_ACTION_DPICKUP);
    }
}

//获取softkey数据信息
void CDialProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //是否只有一个可用帐号
    bool bIsOnlyOneValidAccount = (acc_GetRegisteredAccountNum() <= 1);

    // 是否有Delete键
    //不需要判断 联系人列表是否有焦点  如果有选择联系人列表中的联系人  按delete的softkey还是可以删除编辑框中的字符
    bool bHaveDel = !IsEditLineEmpty();
    // 是否处于编辑模式
    //选择项时，也是处于编辑模式
    //输入框内容为空，则处于正常模式
    //bool bInNormal = (m_pSearchTable != NULL
    //  && !m_pSearchTable->IsSelectItem())
    //  && (m_pPswLineEdit != NULL
    //  && m_pPswLineEdit->text().isEmpty());

    //编辑框为空  而且 联系人控件没有选择联系人
    bool bInNormal = IsEditLineEmpty() && (!IsSearchTableSelectItem());

    switch (m_objDialUIData.m_eDialUIType)
    {
    case DIAL_UI_DPICKUP:
        {
            // 特殊处理DPickup呼出
            VecSoftkey.push_back(ST_DPICKUP);
            m_bDPickUpSoftkeySet = true;
            VecSoftkey.push_back(ST_IMECHANGE);
            VecSoftkey.push_back(ST_DELETE);
            VecSoftkey.push_back(ST_CANCEL);

            return;
        }
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case DIAL_UI_CALLPARK:
        {
            // 添加RetrieveParkSoftkey
            //http://10.2.1.199/Bug.php?BugID=55588
            //根据这个bug需求 统一 softkey

            // http://10.3.5.199/Bug.php?BugID=71884
            //broadsoft 开启的时候才能显示相关 softkey
            //http://bugfree.yealink.com/Bug.php?BugID=124363
//          if (configParser_GetConfigInt(kszBroadsoftActive) != 0)
//          {
            VecSoftkey.push_back(ST_PARK);
//          }

            VecSoftkey.push_back(ST_IMECHANGE);
            //为空的情况下 Softkey 变成 Dir
            // http://10.2.1.199/Bug.php?BugID=57347
            if (true == bHaveDel)
            {
                VecSoftkey.push_back(ST_DELETE);
            }
            else
            {
                VecSoftkey.push_back(ST_POOL);
            }
            VecSoftkey.push_back(ST_CANCEL);

            return;
        }
        break;
    case DIAL_UI_RETRIEVE_CALLPARK:
        {
            // 添加RetrieveParkSoftkey
            // http://10.3.5.199/Bug.php?BugID=71884
            //broadsoft 开启的时候才能显示相关 softkey

            //http://bugfree.yealink.com/Bug.php?BugID=124363
            //相关 softkey不受broadsoft 开关控制
//          if (configParser_GetConfigInt(kszBroadsoftActive) != 0)
//          {
            VecSoftkey.push_back(ST_RETRIEVE_PARK);
            m_bRetrieveSoftkeySet = true;
//          }

            VecSoftkey.push_back(ST_IMECHANGE);
            VecSoftkey.push_back(ST_DELETE);
            VecSoftkey.push_back(ST_CANCEL);

            return;
        }
        break;
    case DIAL_UI_GROUPCALLPARK:
        {
            // 添加GroupParkSoftkey
            // http://10.3.5.199/Bug.php?BugID=71884
            //broadsoft 开启的时候才能显示相关 softkey
            if (configParser_GetConfigInt(kszBroadsoftActive) != 0)
            {
                VecSoftkey.push_back(ST_GROUP_PARK);
            }

            VecSoftkey.push_back(ST_IMECHANGE);
            if (true == bHaveDel)
            {
                VecSoftkey.push_back(ST_DELETE);
            }
            else
            {
                VecSoftkey.push_back(ST_EMPTY);
            }
            VecSoftkey.push_back(ST_CANCEL);

            return ;
        }
        break;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case DIAL_UI_TRANSFER:
        {
            //可能联系人列表有选择联系人  所以这边判断条件不是bHaveDel
            if (bInNormal)
            {
                VecSoftkey.push_back(ST_POOL);
            }
            else
            {
                VecSoftkey.push_back(ST_TRANSFER);
            }
        }
        break;
#if IF_FEATURE_DIAL_ON_HOLD
    case DIAL_UI_ON_HOLD:
        {
            if (m_objDialUIData.m_bAllowTran)
            {
                VecSoftkey.push_back(ST_TRANSFER);
            }
            else
            {
                VecSoftkey.push_back(ST_EMPTY);
            }

            VecSoftkey.push_back(ST_DELETE);
            VecSoftkey.push_back(ST_NEWCALL);
            VecSoftkey.push_back(ST_CANCEL);

            return;
        }
        break;
#endif

    default:
        {
            //可能联系人列表有选择联系人  所以这边判断条件不是bHaveDel
            if (bInNormal)
            {
                VecSoftkey.push_back(ST_POOL);
            }
            else
            {
                VecSoftkey.push_back(ST_SEND);
            }
        }
        break;
    }

    // 设置Softkey2
    VecSoftkey.push_back(ST_IMECHANGE);

    // 设置Softkey3
    if (bInNormal)
    {
        int iAccountNum = 0;

        if (IsPSTNAccountAvailable())
        {
            iAccountNum = acc_GetAvailableAccountList().size();
        }
        else
        {
            iAccountNum = acc_GetAvailableAccountList(ACCOUNT_TYPE_SIP).size();
        }

#if IF_TALKUI_BLUETOOTH_ENABLE
        //蓝牙连接 且蓝牙手机可用
        if (IsBTAccountAvailable()
                && Bluetooth_IsConnected()
                && Bluetooth_IsMobileConnected())
        {
            iAccountNum++;
        }
#endif

#if IF_TALKUI_PSTN
        if (iAccountNum > 1 && ACCOUNTID_OF_IPDIRECT_CALL != GetSipAccount())
        {
            // 无输入且多于一个账号则允许切换
            VecSoftkey.push_back(ST_SWITCH);
        }
#endif
    }

    if (IsAllowGPickUpSoftkey())
    {
        m_bGPickUpSoftkeySet = true;
        VecSoftkey.push_back(ST_GPICKUP);
    }
    else if (IsAllowRetrieveParkSoftkey())
    {
        m_bRetrieveSoftkeySet = true;
        VecSoftkey.push_back(ST_RETRIEVE_PARK);
    }
#if IF_BUG_27796
    else if (IsAllowZPickUpSoftkey())
    {
        m_bZPickUpSoftkeySet = true;
        vecSoftkey.push_back(ST_Z_PICKUP);
    }
#endif
    else
    {
        if (VecSoftkey.size() <= 2)
        {
            if (bHaveDel)
            {
                VecSoftkey.push_back(ST_DELETE);
            }
            else
            {
                VecSoftkey.push_back(ST_EMPTY);
            }
        }
        else if (bHaveDel)
        {
            VecSoftkey.push_back(ST_DELETE);
        }
    }

#if IF_TALKUI_MULTY_ACCOUNT
    // 设置Softkey4
    if (m_objDialUIData.m_eDialUIType != DIAL_UI_TRANSFER
            && m_objDialUIData.m_eDialUIType != DIAL_UI_CONFERENCE
            && m_objDialUIData.m_eDialUIType !=
            DIAL_UI_RINGFORWARD         //http://10.3.5.199/Bug.php?BugID=61423   在forward to 界面不显示Line 的softkey
            && !bIsOnlyOneValidAccount
#if IF_FEATURE_BLOCK_LINE
            && configParser_GetConfigInt(kszBlockLine) != 1
#endif
            && !bHaveDel
       )
    {
        VecSoftkey.push_back(ST_LINE);
    }
    else
    {
        VecSoftkey.push_back(ST_CANCEL);
    }
#else
    VecSoftkey.push_back(ST_CANCEL);
#endif //IF_TALKUI_MULTY_ACCOUNT
}

// softkey处理(自定义类型)
void CDialProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
#if IF_FEATURE_CUSTOM_SOFTKEY
    strFile = "Dialing.xml";

    switch (m_objDialUIData.m_eDialUIType)
    {
    case DIAL_UI_TRANSFER:
        strStatus = "DialTrans";
        break;
    case DIAL_UI_CONFERENCE:
        strStatus = "DialConference";
        break;
    case DIAL_UI_DPICKUP:
        strStatus = "DialDPickup";
        break;
    case DIAL_UI_RETRIEVE_CALLPARK:
        strStatus = "DialRetrieve";
        break;
    case DIAL_UI_RINGFORWARD:
        strStatus = "DialForward";
        break;
#if IF_FEATURE_DIAL_ON_HOLD
    case DIAL_UI_ON_HOLD:
        strStatus = "DialOnHold";
        return;
#endif
    case DIAL_UI_CALLPARK:
        strStatus = "DialCallPark";
        break;
    case DIAL_UI_GROUPCALLPARK:
        strStatus = "DialGroupCallPark";
        break;
    default:
        strStatus = "Dialing";
        break;
    }

    bool bInputEmpty = IsEditLineEmpty();
    if (bInputEmpty)
    {
        strStatus.append("Empty");
    }
#else
    if (DIAL_UI_TRANSFER == m_objDialUIData.m_eDialUIType)
    {
        strFile = "Talking.xml";
        strStatus = "PreTrans";
    }
    else
    {
        strFile = "Dialing.xml";
        strStatus = "Dialing";
    }
#endif
}

// 是否允许自定义softkey
bool CDialProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_PSTN
    if (acc_ExistAvailableAccount(
                ACCOUNT_TYPE_PSTN)) // 存在可用PSTN账号，需要显示Switch，因此显示固定Layout
    {
        return false;
    }
#endif

    if (DIAL_UI_RINGFORWARD == m_objDialUIData.m_eDialUIType
            || DIAL_UI_CONFERENCE == m_objDialUIData.m_eDialUIType
            || DIAL_UI_DPICKUP == m_objDialUIData.m_eDialUIType
            || DIAL_UI_RETRIEVE_CALLPARK == m_objDialUIData.m_eDialUIType
            || DIAL_UI_CALLPARK == m_objDialUIData.m_eDialUIType
            || DIAL_UI_GROUPCALLPARK == m_objDialUIData.m_eDialUIType)
    {
        return false;
    }

    return true;
}

// 判断action是否需要播放按键音
//硬按键也需要播放按键音
//如果硬按键 和action 都判断要播放按键音的 那么 硬按键不播放了  统一由action播放
//目前该函数 仅有 拨号界面使用
bool CDialProcessor::IsSoftkeyNeedToPlayKeyTone(SoftKey_TYPE softkeyType)
{
    // 配置项控制 预拨号是否需要播放按键音
    if (GetSessionState() == SESSION_PREDIAL
            && 0 == configParser_GetConfigInt(kszPlayDialToneInPreDialEnable))
    {
        return false;
    }

//     if (1 == configParser_GetConfigInt(kszTouchSound))
    {
        //如果是删除键
        if (ST_DELETE == softkeyType)
        {
            //并且编辑框有数据 那么要响铃声
            if (!IsEditLineEmpty())
            {
                return true;
            }
        }

        //并且是 呼出键
        if (ST_SEND == softkeyType)
        {
            //如果呼出键不需要响按键音 直接返回false
            if (!_TalkUILogicDelegate.IsVolumeSendKeySoundOn())
            {
                return false;
            }

            yl::string strNumber = "";
            yl::string strDisplayName = "";

            //获取拨号数据
            GetInputContent(strNumber, strDisplayName);

            //如果有数据
            if (strNumber.length() > 0)
            {
                return true;
            }
            else if (GetShuttleCallID() > -1 || IsSearchTableSelectItem())
            {
                return true;
            }
            else
            {
                if (DIAL_UI_CALLPARK ==
                        m_objDialUIData.m_eDialUIType)  //如果是在park  界面  可以按 呼出键 直接park到本机
                    //如果用户需要按以keyassend 开头的特征码，那么必须先把keyassend的功能关闭
                {
                    return true;
                }
                else if (DIAL_UI_RETRIEVE_CALLPARK ==
                         m_objDialUIData.m_eDialUIType) //为空的时候 可能是取回park 自己号码上的通话
                {
                    return true;
                }
                else if (DIAL_UI_DPICKUP == m_objDialUIData.m_eDialUIType)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//实际判断硬按键是否播放按键音
bool CDialProcessor::IsNeedToPlayKeyTone(yl::string strEditContent, int iKey)
{
    // 配置项控制 预拨号是否需要播放按键音
    if (GetSessionState() == SESSION_PREDIAL
            && 0 == configParser_GetConfigInt(kszPlayDialToneInPreDialEnable))
    {
        return FALSE;
    }

    if (_TalkUILogicDelegate.IsJoyce())
    {
        if ((strEditContent.length() > 0)
                && (strEditContent.at(0) != '*')
                && (strEditContent.at(0) != '#')
                && IsSendDigitKey(strEditContent, iKey))
        {
            return TRUE;
        }
    }

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
            return _TalkUILogicDelegate.IsVolumeSendKeySoundOn();
        }
    }

    // 拨号界面只播放数字键、删除键的按键音
    if (IsDigitKey(iKey))
    {
        return TRUE;
    }

    return FALSE;
}

// 判断是否需要播放按键音
//action 也需要判断是否播放按键音
//如果硬按键 和action 都判断要播放按键音的 那么 硬按键不播放了  统一由action播放
bool CDialProcessor::IsNeedToPlayKeyTone(int iKey)
{
    //先从界面类中获取编辑框字符串
    yl::string strEditData = GetLineEditData();

    return IsNeedToPlayKeyTone(strEditData, iKey);
}

//普通拨号界面通道键处理
bool CDialProcessor::OnNormalChannelKeyPress(int iKey)
{
    TALKUI_INFO("OnNormalChannelKeyPress  this=0x%x", (int)this);

    bool bHandled = false;
    bool bCallOut = false;

    yl::string strNumber = "";
    yl::string strDisplayName = "";

    switch (iKey)
    {
#if IF_TALKUI_OK_TO_REDIAL
    case PHONE_KEY_HANDSET_OFF_HOOK:        // 摘机
        {
            //某些机型没有redial键，在输入框没有输入内容的情况下，摘机键当成redial键使用

            yl::string strNumber = "";
            yl::string strDisplayName = "";

            //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
            InputLineEditData();
            //获取号码
            GetInputContent(strNumber, strDisplayName);

            //Callpark 和 Forward 的时候 都不响应REDIAL键
            if (!strNumber.length() > 0)
            {
                //重拨事件
                ////----确认
                SendUIRequestAction(TALK_ACTION_REDIAL);
            }
            else
            {
                bCallOut = true;
            }
        }
        break;
#else
    case PHONE_KEY_HANDSET_OFF_HOOK:        // 摘机
#endif

    case PHONE_KEY_HANDFREE:                // 免提键
    case PHONE_KEY_EHS_OPEN:
#ifdef IF_FEATURE_HEADSET_AS_CANCEL
    case PHONE_KEY_HEADSET_CTRL:
#endif
        {
            if (iKey == PHONE_KEY_HEADSET_CTRL
                    && configParser_GetConfigInt(kszHeadsetAsCancel) == 0)
            {
                break;
            }

            if (
                //CP的情况下，免提不能用于退出拨号界面
#if IF_TALKUI_HANDFREE_DIAL
                false &&
#endif
                iKey == PHONE_KEY_HANDFREE
                && SESSION_PREDIAL != m_eSessionState
            )
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
                bCallOut = true;

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
        OnSend();
    }
    else
    {
        bHandled = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);

        //这边因为 BaseTalkUI不更新了 所以 这边要更新 TalkUIFrame也有更新 但是 这个窗口不在TalkUIFrame内 所以 这边必须更新
    }

    TALKUI_INFO("CDialProcessor::OnNormalChannelKeyPress( end this=0x%x", this);

    return bHandled;
}

//Forward 状态下 按键处理
bool CDialProcessor::OnForwardChannelKeyPress(int iKey)
{
    bool bHandled = true;
    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
    case PHONE_KEY_HEADSET_CTRL:
    case PHONE_KEY_HANDFREE:
        {
            // 转移界面 声道按键是否当作转移键来使用
            if (configParser_GetConfigInt(kszFwdCallByChannelKey) != 0)
            {
                OnSend();
                break;
            }

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
#if IF_TALKUI_ONHOOK_EXITCALL
            SendUIRequestAction(TALK_ACTION_CANCEL);
#else
            //问题：Wildix T28\T26\T22 手柄不能挂掉还没应答的通话
            //修改：来电界面挂机时不执行 ExitRoutine()操作
            //http://10.2.1.199/Bug.php?BugID=16809

            // http://10.2.1.199/Bug.php?BugID=46003
            // 挂机不挂断通话，但要切换通道
            voice_SwitchChannel();
#endif
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
bool CDialProcessor::OnCallParkChannelKeyPress(int iKey)
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
                    if (m_objDialUIData.m_eTalkState == TS_TALK)
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
        bHandled = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

// 解析号码与名字
bool CDialProcessor::ParseDialNumber(LPCSTR lpszDial, DataDial2Logic & objData)
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

//获取转接通话ID
int CDialProcessor::GetShuttleCallID()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->GetShuttleCallID();
    }

    return -1;

}

//编辑框是否为空
bool CDialProcessor::IsEditLineEmpty()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->IsEditLineEmpty();
    }

    //没有通话UI 那么就认为是空的
    return true;
}

//获取编辑框数据
yl::string CDialProcessor::GetLineEditData()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->GetLineEditData();
    }

    return "";
}

//联系人控件是否有选中联系人
bool CDialProcessor::IsSearchTableSelectItem()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->IsSearchTableSelectItem();
    }

    return false;
}

//让UI输出预编辑字符
void CDialProcessor::InputLineEditData()
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->InputLineEditData();
    }

}

//获取号码 和联系人名称
bool CDialProcessor::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->GetInputContent(strNum, strDisplayname);
    }

    return false;

}

//把联系人控件中的联系人信息设置到编辑框中
void CDialProcessor::SetLineEditDataBySearchTable()
{
    if (NULL != m_pTalkUI)
    {
        yl::string strDisplayname = "";
        yl::string strNumber = "";

        // http://10.2.1.199/Bug.php?BugID=76119
        //获取搜索框内容成功
        if (m_pTalkUI->GetSearchTableContent(strNumber, strDisplayname))
        {
            // http://10.2.1.199/Bug.php?BugID=76119
            //且获取到的号码不为空
            if (!strNumber.empty())
            {
                m_pTalkUI->SetLineEditDataBySearchTable();
            }
        }
    }
}

//处理 发送事件
void CDialProcessor::OnSend(bool bAudioDial)
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    //在transfer 状态
    if (DIAL_UI_TRANSFER == m_objDialUIData.m_eDialUIType)
    {
        int iShuttleCallID = GetShuttleCallID();
        if (iShuttleCallID > 0)
        {
            //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
            InputLineEditData();

            SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);

            return;
            //  return true;
        }
    }
    else if (DIAL_UI_CONFERENCE == m_objDialUIData.m_eDialUIType)
    {
        int iShuttleCallID = GetShuttleCallID();
        if (iShuttleCallID > 0)
        {
            //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
            InputLineEditData();

            SendUIRequestAction(TALK_ACTION_CONF, iShuttleCallID);

            return;
            //  return true;
        }
    }

    //获取相关数据
    //把预编辑字符输出
    InputLineEditData();
    //联系人控件 有选择联系人 要赋值到编辑框中
    if (true == IsSearchTableSelectItem())
    {
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
    }
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
    if (DIAL_UI_CALLPARK == m_objDialUIData.m_eDialUIType)
    {
        SendUIRequestAction(TALK_ACTION_SEND, bAudioDial ? 1 : 0, 0, &objDialData);
    }
    else if (DIAL_UI_RETRIEVE_CALLPARK ==
             m_objDialUIData.m_eDialUIType) //为空的时候 可能是取回park 自己号码上的通话
    {
        SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK, 0, 0, &objDialData);
    }
    else if (DIAL_UI_GROUPCALLPARK == m_objDialUIData.m_eDialUIType)
    {
        SendUIRequestAction(TALK_ACTION_SEND, bAudioDial ? 1 : 0, 0, &objDialData);
    }
    else if (DIAL_UI_DPICKUP == m_objDialUIData.m_eDialUIType)
    {
        //这边pickup 的时候 即使编辑框为空发送DPICKUP事件 也不会有什么问题  而且PickUp 可能有默认的 code 所以为空的时候需要往下发
        //V73 可能在逻辑层对PICK拨号的SEND事件有做区分，但是V80在逻辑层没区分所以必须在UI区分

        SendUIRequestAction(TALK_ACTION_DPICKUP, 0, 0, &objDialData);
    }
    else if (!objDialData.strNumber.empty())
    {
        SendUIRequestAction(TALK_ACTION_SEND, bAudioDial ? 1 : 0, 0, &objDialData);
    }
}

//处理transfer
void CDialProcessor::OnTransfer()
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    //在transfer 状态  穿梭数据
    if (DIAL_UI_TRANSFER == m_objDialUIData.m_eDialUIType)
    {
        int iShuttleCallID = GetShuttleCallID();
        if (iShuttleCallID > 0)
        {
            //有预编辑字符 就先让预编辑字符输出  避免出现其他问题
            InputLineEditData();

            SendUIRequestAction(TALK_ACTION_TRAN, iShuttleCallID);

            return ;
            //  return true;
        }
    }

    //获取相关数据
    //把预编辑字符输出
    InputLineEditData();
    //联系人控件 有选择联系人 要赋值到编辑框中
    if (true == IsSearchTableSelectItem())
    {
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
    }

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

//处理park
void CDialProcessor::OnPark()
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    bool bPrefixPark = false;
#if IF_BUG_27796
    //根据账号服务器类型，选择进行Park操作
    if (SST_BROADSOFT != configParser_GetCfgItemIntValue(kszAccountSIPServerType,
            m_objDialUIData.m_iCurrentAccountID))
    {
        bPrefixPark = true;
    }
#endif

    if (bPrefixPark)
    {
        SendUIRequestAction(TALK_ACTION_PREFIX_PARK);
    }
    else
    {
        //Park
        InputLineEditData();
        //联系人控件 有选择联系人 要赋值到编辑框中
        if (true == IsSearchTableSelectItem())
        {
            //把联系人控件中的联系人信息 赋值到编辑框中
            SetLineEditDataBySearchTable();
        }

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
}

//处理GPark
void CDialProcessor::OnGPark()
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    InputLineEditData();
    //联系人控件 有选择联系人 要赋值到编辑框中
    if (true == IsSearchTableSelectItem())
    {
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
    }

    GetInputContent(strNumber, strDisplayName);


    //呼出事件
    DataDial2Logic objDialData;
    objDialData.strNumber = strNumber;
    objDialData.strName = strDisplayName;
    //GetDataToLogic(objDialData);

    SendUIRequestAction(TALK_ACTION_GROUP_PARK, 0, 0, &objDialData);
}

//处理RETRIEVE_PARK
void CDialProcessor::OnRetrievePark()
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    InputLineEditData();
    //联系人控件 有选择联系人 要赋值到编辑框中
    if (true == IsSearchTableSelectItem())
    {
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
    }

    GetInputContent(strNumber, strDisplayName);


    //呼出事件
    DataDial2Logic objDialData;
    objDialData.strNumber = strNumber;
    objDialData.strName = strDisplayName;
    //GetDataToLogic(objDialData);

    SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK, 0, 0, &objDialData);
}

//处理DPickUp
void CDialProcessor::OnDPickUp()
{
    yl::string strNumber = "";          //联系人号码
    yl::string strDisplayName = "";     //联系人姓名

    //把预编辑字符输出
    InputLineEditData();
    //联系人控件 有选择联系人 要赋值到编辑框中
    if (true == IsSearchTableSelectItem())
    {
        //把联系人控件中的联系人信息 赋值到编辑框中
        SetLineEditDataBySearchTable();
    }

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

//处理GPickUp
void CDialProcessor::OnGPickUp()
{
    // 从逻辑层拿到数据
    SendUIRequestAction(TALK_ACTION_GPICKUP);
}

//编辑框字符变化处理函数
void CDialProcessor::OnTextChange(yl::string strText)
{
#if IF_BUG_27560
    if (actURI_IsHideTalkInfo())
    {
        return;
    }
#endif

    //如果是由UI发起的刷新 不赋值那么就会有问题 所以这边先赋值，保证后面不会出错
    m_objDialUIData.m_strNumber = strText;

    ///////////先刷新 softkey/////////////////////
    if ((strText.empty() && !m_strLastKey.empty()) || (!strText.empty() && m_strLastKey.empty()))
    {
        //分开来赋值比较合适  因为有可能会在刷新softkey的时候用到这个值 （当前没用到  不代表以后不用）
        m_strLastKey = strText;
        //先更新softkey， 防止出现 输入一定好吗自动呼出的问题， 如果拨号不缓存有可能造成死机
        //有删除的softkey 之分
        UpdateTalkSoftkey();
    }
    else        //分开来赋值比较合适  因为有可能会在刷新softkey的时候用到这个值 （当前没用到  不代表以后不用）
    {
        //分开来赋值比较合适  因为有可能会在刷新softkey的时候用到这个值 （当前没用到  不代表以后不用）
        m_strLastKey = strText;
    }

    ///////////先刷新 softkey/////////////////////


    DataDial2Logic objDial2LogicData;

    objDial2LogicData.strNumber = strText;
    objDial2LogicData.strName = "";

    SendUIRequestAction(TALK_ACTION_TEXT_CHANGE, 0, 0, &objDial2LogicData);

}

void CDialProcessor::SelectSelectLine()
{
    CUILogicBaseTalkUI * pBaseTalkUI = GetBindUI();
    if (NULL == pBaseTalkUI)
    {
        return;
    }

    // 创建列表
    m_vecLineSelectData.clear();
    LINE_SELECT_DATA objData;

    ListAccounts listAccount = acc_GetAvailableAccountList();
    ListAccounts::const_iterator iter = listAccount.begin();

    for (; iter != listAccount.end(); ++iter)
    {
        int iAccount = *iter;

        //http://10.2.1.199/Bug.php?BugID=93528 MCA添加限制条件 如果不能再起一路呼出的账号就不显示
        if (!talklogic_IsAllowNewCallByAccount(iAccount))
        {
            continue;
        }

        objData.m_strName = acc_GetUsername(iAccount) + "@" + acc_GetServerName(iAccount);
        objData.m_strContent = "";
        objData.m_nAccount = iAccount;

        m_vecLineSelectData.push_back(objData);
    }

    pBaseTalkUI->OpenLineSelectPage(m_vecLineSelectData,
                                    _UILOGIC_LANG_TRAN(TRID_LINE_SELECT), true, LINE_SELECT_ID);
}

#if IF_TALKUI_EXECUTIVE
void CDialProcessor::SelectCallInitiationLine()
{
    CUILogicBaseTalkUI * pBaseTalkUI = GetBindUI();
    if (NULL == pBaseTalkUI)
    {
        return;
    }

    // 创建列表
    m_vecCallInitiationData.clear();
    CAssCommon dataInfo;
    LINE_SELECT_DATA objData;
    int iAccount = GetSipAccount();
    if (iAccount >= 0 && BWExeAssis_GetData(&dataInfo, iAccount))
    {
        for (ExeAssisListIt it = dataInfo.listData.begin(); it != dataInfo.listData.end(); ++it)
        {
            objData.m_strName = it->ItemName.strUserId;
            objData.m_strContent = it->ItemName.strExt;

            m_vecCallInitiationData.push_back(objData);
        }
    }

    objData.m_strName = _UILOGIC_LANG_TRAN(TRID_CANCEL_SELECT);
    objData.m_strContent = "";

    m_vecCallInitiationData.push_back(objData);

    pBaseTalkUI->OpenLineSelectPage(m_vecCallInitiationData,
                                    _UILOGIC_LANG_TRAN(TRID_DSSKEY_CALL_INITIATION), true, CALL_INITIATION_ID);
}
#endif

void CDialProcessor::OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID)
{
    if (LINE_SELECT_RESULT_SELECT != eResult)
    {
        return;
    }

    if (LINE_SELECT_ID == nSelectID)
    {
        if (nSelectIndex < 0 || nSelectIndex > m_vecLineSelectData.size())
        {
            return;
        }

        //http://10.3.5.199/Bug.php?BugID=66254
        //t46 改变账号需要呼出  后面的参数改成非0 就是呼出
        talklogic_UIEvent(GetSessionID(), TALK_ACTION_ACCOUNT_CHANGE, m_vecLineSelectData[nSelectIndex].m_nAccount, 1);
    }
#if IF_TALKUI_EXECUTIVE
    else if (CALL_INITIATION_ID == nSelectID)
    {
        int iAccount = GetSipAccount();
        int iSize = m_vecCallInitiationData.size();

        if (nSelectIndex < 0 || iAccount < 0 || nSelectIndex >= iSize)
        {
            return;
        }

        yl::string strTarget = m_vecCallInitiationData[nSelectIndex].m_strContent;
        BWExeAssis_SetCallInitiationNumber(strTarget, iAccount);

        SendUIRequestAction(TALK_ACTION_CALL_INITIATION, nSelectIndex == iSize - 1 ? 0 : 1);
    }
#endif
}

bool CDialProcessor::IsBTAccountAvailable()
{
    //蓝牙不能参与转接与park等行为
    if (m_objDialUIData.m_eDialUIType == DIAL_UI_DIALING
            || m_objDialUIData.m_eDialUIType == DIAL_UI_PREDIAL
            || m_objDialUIData.m_eDialUIType == DIAL_UI_CONFERENCE
            || m_objDialUIData.m_eDialUIType == DIAL_UI_BLUETOOTH_DIALING
#ifndef _WIN32
#warning
#endif
//      || m_objDialUIData.m_eDialUIType == DIAL_UI_BLUETOOTH_PREDIAL
#if IF_FEATURE_DIAL_ON_HOLD
            || m_objDialUIData.m_eDialUIType == DIAL_UI_ON_HOLD
#endif
       )
    {
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

bool CDialProcessor::IsPSTNAccountAvailable()
{
    //根据当前类型，判断是要获取pstn账号，还是只能获取sip账号

    //pstn不能参与转接与park等行为
    if (m_eSessionState != SESSION_PRETRAN
            && (m_objDialUIData.m_eDialUIType == DIAL_UI_DIALING
                || m_objDialUIData.m_eDialUIType == DIAL_UI_PREDIAL
                || m_objDialUIData.m_eDialUIType == DIAL_UI_CONFERENCE
#if IF_FEATURE_DIAL_ON_HOLD
                || m_objDialUIData.m_eDialUIType == DIAL_UI_ON_HOLD
#endif
                || m_objDialUIData.m_eDialUIType == DIAL_UI_BLUETOOTH_DIALING
#ifndef _WIN32
#warning
#endif
//      || m_objDialUIData.m_eDialUIType == DIAL_UI_BLUETOOTH_PREDIAL
               ))
    {
        return true;
    }
    else
    {
        return false;
    }

    return false;
}

yl::string CDialProcessor::GetAccountTitleText(DataDial * pDialData)
{
    yl::string strTitle = "";

#if IF_FEATURE_32591
    strTitle = acc_GetStatusBarLabel(pDialData->pCallInfo->GetAccountID());
    if (!strTitle.empty())
    {
        return strTitle;
    }
#endif

    //拨号预拨号界面显示当前所用账号的信息
    if (NULL == pDialData->pCallInfo)
    {
        return "";
    }

	int iAccountID = pDialData->pCallInfo->GetAccountID();
    //优先获取hoteling 账号
    yl::string strTemp = _TalkUILogicDelegate.GetHotelingUserName(iAccountID);

    //如果获取到的 hoteling 账号还是为空
    if (strTemp.empty())
    {
        int iDisplayMethodOnDialing = _TalkUILogicDelegate.GetDisplayMethodOnDialing();

        //优先显示Label
        if (1 == iDisplayMethodOnDialing)
        {
            strTemp = GetLabel(iAccountID);

        }
        else if (2 == iDisplayMethodOnDialing)
        {
            strTemp = GetDisplayName(iAccountID);

        }
        else
        {
            strTemp = GetUserName(iAccountID);
        }

        //如果获取的UserName 是空的 那么就不应该显示  有的时候 账号Label、DisplayName有填  但是UserName 没写 这个时候按dsskey可能还是可以进入拨号界面的
        //这个时候title 就不应该显示有内容
        if (GetUserName(iAccountID).empty()
#if IF_TALKUI_PSTN
                //PSTN无账号不处理
                && !acc_IsPstnAccount(iAccountID)
#endif
           )
        {
            strTemp = "";
        }
    }

    //这边就不需要翻译了  因为不是人名就是号码
    strTitle = strTemp;

    return strTitle;
}

bool CDialProcessor::NeedReUpdate()
{
#if IF_FEATURE_37893
    if (SESSION_PRETRAN == m_eSessionState
            || SESSION_PREDIAL == m_eSessionState
            || SESSION_DIALING == m_eSessionState
            || SESSION_BTDIALING == m_eSessionState
            || SESSION_PRE_RETRIEVEPARK == m_eSessionState)
    {
        return false;
    }
#endif

    return true;
}

bool CDialProcessor::AllowSoftkeyShow(SoftKey_TYPE eType)
{
    if (ST_GPICKUP == eType)
    {
        return IsAllowGPickUpSoftkey();
    }
    else if (ST_DPICKUP == eType)
    {
        return IsAllowDPickUpSoftkey();
    }
    else if (ST_RETRIEVE_PARK == eType)
    {
        return IsAllowRetrieveParkSoftkey();
    }

    return CBaseTalkUIProcessor::AllowSoftkeyShow(eType);
}

bool CDialProcessor::IsAllowGPickUpSoftkey()
{
    if (IsEditLineEmpty()
            && !m_bGPickUpSoftkeySet
            && configParser_GetConfigInt(kszGPickupSwitch) != 0
#if IF_TALKUI_PSTN
            && m_objDialUIData.m_eCallLineType != CALL_LINE_PSTN
#endif
       )
    {
        return true;
    }

    return false;
}


bool CDialProcessor::IsAllowDPickUpSoftkey()
{
    if (IsEditLineEmpty()
            && !m_bDPickUpSoftkeySet
            && configParser_GetConfigInt(kszDPickupSwitch) != 0
#if IF_TALKUI_PSTN
            && CALL_LINE_PSTN != m_objDialUIData.m_eCallLineType
#endif
       )
    {
        return true;
    }

    return false;
}

bool CDialProcessor::IsAllowZPickUpSoftkey()
{
    if ((SESSION_DIALING == m_eSessionState
            || SESSION_PREDIAL == m_eSessionState)
            && !m_bZPickUpSoftkeySet
#if IF_TALKUI_PSTN
            && CALL_LINE_PSTN != m_objDialUIData.m_eCallLineType
#endif
       )
    {
        return true;
    }

    return false;
}

bool CDialProcessor::IsAllowRetrieveParkSoftkey()
{
    // 添加RetrieveParkSoftkey
    // http://10.3.5.199/Bug.php?BugID=71884
    //broadsoft 开启的时候才能显示相关 softkey

    //http://bugfree.yealink.com/Bug.php?BugID=124363
    //相关 softkey不受broadsoft 开关控制

    if (!m_bRetrieveSoftkeySet
            && IsEditLineEmpty()
            && (configParser_GetConfigInt(kszCallParkSwitch) != 0
                || configParser_GetConfigInt(kszGroupCallParkSwitch) != 0)
            //      && configParser_GetConfigInt(kszBroadsoftActive) != 0
#if IF_TALKUI_PSTN
            && CALL_LINE_PSTN != m_objDialUIData.m_eCallLineType
#endif
       )
    {
        return true;
    }

    return false;
}

