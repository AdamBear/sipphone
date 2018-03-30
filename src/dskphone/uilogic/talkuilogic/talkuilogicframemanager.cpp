#include "talkuilogicframemanager.h"
#include "talkuilogic_inc.h"
#include "talkuiheaders.h"
#include "include/translateiddefine.h"
#include "talkuilogicdelegate.h"
#include "sessioncachemanager.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "broadsoft/callcenter/include/ccmessagedefine.h"
#include "extended/bluetoothmessagedefine.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//mod-bluetooth
#include "wireless/bluetoothNew/include/modbluetooth.h"
#endif

#if IF_TALKUI_USB_ENABLE
//录制相关头文件
#include "record/include/recordmessagedefine.h"
#include "record/include/modrecord.h"
#endif

#include "talk/fwd_dnd/include/fwddndmsgdefine.h"

#include "talkingprocessor.h"
#include "confprocessor.h"


IMPLEMENT_GETINSTANCE(CTalkUILogicFrameManager)


static LRESULT OnTalkUIACDQueueMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case ACD_MESSAGE_QUEUE_STATUS:
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
        _TalkUILogicFrameManager.UpdateACDQueueStatus();
#endif
        break;
    default:
        break;
    }

    return 0;
}

#if IF_TALKUI_BLUETOOTH_ENABLE
static LRESULT OnTalkUIBlueToothMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case BLUETOOTH_MSG_CONNECT_STATE:
        {
            _TalkUILogicFrameManager.UpdateBlueToothIcon();
            _TalkUILogicFrameManager.UpdateChannelIcon();       //蓝牙连接更新 可能导致通道更新
        }
        break;
    default:
        break;
    }

    return 0;
}
#endif

#if IF_TALKUI_WIFI_ENABLE
static LRESULT OnTalkUIWifiMsg(msgObject & objMessage)
{
//  switch (objMessage.message)
//  {
#ifndef _WIN32
#warning wifimessage
#endif
//  case TALK_MSG_UPDATESTATS:
//      {
//          //http://10.2.1.199/Bug.php?BugID=79914 当消息类型为TALK_MSG_UPDATESTATS时，
//          //wParam:(当前通话ID), lParam(1:表示丢包率大于5%，信号差 0:信号正常，连续3次丢包率小于5)
//          int nIsShow = (int)objMessage.lParam;
//          _TalkUILogicFrameManager.SetWifiNotify(nIsShow);
//      }
//      break;
//  default:
//      break;
//  }

    return 0;
}
#endif

#if IF_TALKUI_USB_ENABLE
//USB录制消息回调函数
static LRESULT OnTalkUIUSBRecordMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    // wparam： 0=断开,1=连接上,2=正在检测,3=无法识别
    case RECORD_STORAGE_STATE_CHANGE:
        {
            _TalkUILogicFrameManager.OnUSBRecordMsgStateChange(objMessage.wParam);
        }
        break;
    // waparm： 警告值（单位秒） （实际是秒， 定义上注释有误）
    case RECORD_STORAGE_VOLUME_WARNING:
        {
            _TalkUILogicFrameManager.OnUSBRecordMsgWarning(objMessage.wParam);
        }
        break;
    // 录制 直到 容量满
    case RECORD_STORAGE_SPACE_NOTENOUGH:
        {
            _TalkUILogicFrameManager.OnUSBRecordMsgNotenough();
        }
        break;
    // waparm：0=开始录制,1=停止录制,2=截图  lparam：成功与否(0/1)
    case RECORD_STORAGE_RECORD_RESULT:
        {
            //录制截图 提醒
            //waparm：0=开始录制,1=停止录制,2=截图  lparam：成功与否(0/1)
            _TalkUILogicFrameManager.OnRecordStorageRecordResult(objMessage.wParam, objMessage.lParam);
        }
        break;
    default:
        {

        }
        break;
    }

    _TalkUILogicFrameManager.UpdateTalkUISoftkey();

    return 0;
}
#endif

static LRESULT OnDNDStatusChange(msgObject & objMessage)
{
    if (!configParser_GetConfigInt(kszKeepDNDAfterTalk))
    {
        _TalkUILogicFrameManager.UpdateTalkUIIcons();
    }

    return FALSE;
}

CTalkUILogicFrameManager::CTalkUILogicFrameManager()
    : m_pCurrentProcessor(NULL)
    , m_pTalkUIFrameManagerBase(NULL)
{

}

CTalkUILogicFrameManager::~CTalkUILogicFrameManager()
{

}

//初始化
bool CTalkUILogicFrameManager::Init()
{
    g_objTalkUISessionCacheManager.Init();

    //注册 队列、蓝牙 消息
    etl_RegisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS, OnTalkUIACDQueueMsg);

    //蓝牙
#if IF_TALKUI_BLUETOOTH_ENABLE
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                          OnTalkUIBlueToothMsg);
#endif

#if IF_TALKUI_USB_ENABLE
    //USB相关
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_RECORD_RESULT,
                          OnTalkUIUSBRecordMsg);
#endif

#if IF_TALKUI_WIFI_ENABLE
    etl_RegisterMsgHandle(TALK_MSG_UPDATESTATS, TALK_MSG_UPDATESTATS, OnTalkUIWifiMsg);
#endif

    etl_RegisterMsgHandle(DND_MESSAGE_STATUS_CHANGED, DND_MESSAGE_STATUS_CHANGED, OnDNDStatusChange);

    return true;
}

//反初始化
void CTalkUILogicFrameManager::Uninit()
{
    //反注册 队列、蓝牙消息
    etl_UnregisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS, OnTalkUIACDQueueMsg);

#if IF_TALKUI_BLUETOOTH_ENABLE
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE,
                            OnTalkUIBlueToothMsg);
#endif

#if IF_TALKUI_USB_ENABLE
    //USB相关
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_SPACE_NOTENOUGH,
                            OnTalkUIUSBRecordMsg);
#endif

#if IF_TALKUI_WIFI_ENABLE
    etl_UnregisterMsgHandle(TALK_MSG_UPDATESTATS, TALK_MSG_UPDATESTATS, OnTalkUIWifiMsg);
#endif

    etl_UnregisterMsgHandle(DND_MESSAGE_STATUS_CHANGED, DND_MESSAGE_STATUS_CHANGED, OnDNDStatusChange);

    g_objTalkUISessionCacheManager.Uninit();

    ClearTalkProcessor();
}

// 根据逻辑层上传数据 刷新UI
bool CTalkUILogicFrameManager::UpdateUI(DataLogic2UI * pDataLogic2UI)
{
    if (NULL == pDataLogic2UI)
    {
        return false;
    }

    //如果sessionID 是非法的   那么说明是退出整个通话框架
    if (-1 >= pDataLogic2UI->iSessionID)
    {
        //清除session缓存数据
        g_objTalkUISessionCacheManager.ClearAllSessionCache();

#if IF_TALKUI_USB_ENABLE
        //逻辑层会在session开启和关闭的时候进行控制，不需要ui来控制
//      //如果当前有录制 需要停止录制
//      if(modRecord_IsRecording())
//      {
//          modRecord_RecordStop();
//      }
#endif

        //设置没显示音量条
        CBaseTalkUIProcessor::S_SetIsShowVolume(false);

        //调用清除 通话框架的函数
        ClearTalkProcessor();

        //退出整个通话时清空缓存
        ClearTalkUICache();

        return true;
    }

    //获取ProcessorName名称并且生成新的数据
    yl::string strProcessorName = GetProcessorNameFromData(pDataLogic2UI, m_objDataLogic2UI);

    //从session数据缓存中获取要显示的processorname
    //主要用于uilogic同状态刷新
    yl::string strShowProcessorName = g_objTalkUISessionCacheManager.GetProcessorNameTrans(
                                          pDataLogic2UI->iSessionID, strProcessorName);

    //如果获取到的processorname不为空 那么要替换
    if (!strShowProcessorName.empty())
    {
        strProcessorName = strShowProcessorName;
    }

    TALKUI_INFO("CTalkUILogicFrameManager-----processorname = %s", strProcessorName.c_str());

    //return UpdateUIByProcessorName(pDataLogic2UI, strProcessorName);
    return UpdateUIByProcessorName(strProcessorName);
}

//刷新当前窗口
void CTalkUILogicFrameManager::UpdateCurrentWnd(UPDATE_REASON eReason)
{
    if (NULL == m_pCurrentProcessor)
    {
        return ;
    }

    //把数据设置到UI子界面去
    m_pCurrentProcessor->SetTalkUIData(eReason);

    //收集给 talkUI 的数据  给m_dataUILogicToTalkUI 设置数据
    CollectTalkUIData(m_objDataLogic2UI);

    //刷新显示UI
    RefreshUI();

    //这边检测网络
    //检测网络 与 更新title和图标等不同， 因为UI在刷新布局的时候 这些都有可能变化， 但是网络检测肯定是talkuilogic会刷新
    //所以在这调用就可以了  以免重复刷新
    CheckNetwork();

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //更新ACDQueue队列信息
    UpdateACDQueueStatus();
#endif
}


//根据数据和名称更新窗口
//bool CTalkUILogicFrameManager::UpdateUIByProcessorName(DataLogic2UI* pDataLogic2UI, yl::string& strProcessorName)
bool CTalkUILogicFrameManager::UpdateUIByProcessorName(const yl::string & strProcessorName)
{
    //根据SessionID和名称创建通话UI逻辑处理实例
    CBaseTalkUIProcessor * pProcessor = CreateTalkUIByData(m_objDataLogic2UI.iSessionID,
                                        strProcessorName);

    //获取到的 实例为空 返回false
    if (NULL == pProcessor)
    {
        return false;
    }

    //设置当前的通话逻辑实例
    m_pCurrentProcessor = pProcessor;

    //这两个函数的调用 移到 UpdateCurrentWnd 中去
    pProcessor->SetSessionState(m_objDataLogic2UI.eSessionState);
    ////先处理一些共用数据 如title数据
    //往子界面设置数据 还是放在这  不要放在刷新界面的流程中  因为后面逻辑层的数据可能有改变  此时可能UI的数据和logic是同步的
    //但是 和UILogic的数据是不同步的    那么主动发起刷新流程中就不能给UI设置数据
    pProcessor->DealSetTalkData(m_objDataLogic2UI.pData, m_objDataLogic2UI.eUpateReason);

    ////更新通话UI数据
    //pProcessor->SetTalkData(m_objDataLogic2UI.pData, m_objDataLogic2UI.eUpateReason);


    //收集给 talkUI 的数据  给m_dataUILogicToTalkUI 设置数据
    CollectTalkUIData(m_objDataLogic2UI);

    //设置当前的通话逻辑实例
    m_pCurrentProcessor = pProcessor;


    //刷新显示UI
    RefreshUI();

    /*
    //调用talkui 接口函数 刷新显示UI
    if(m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateUI(pProcessor->GetBindUI(), m_dataUILogicToTalkUI);
    }

    */

    //这边检测网络
    //检测网络 与 更新title和图标等不同， 因为UI在刷新布局的时候 这些都有可能变化， 但是网络检测肯定是talkuilogic会刷新
    //所以在这调用就可以了  以免重复刷新
    CheckNetwork();

    //根据数据刷新安全等级
    CheckSecurityClassification();

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //更新ACDQueue队列信息
    UpdateACDQueueStatus();
#endif

    //清除这个sessionID 对应的子对话框
    _TalkUIProcessorList.CheckProcessorBySessionIDAndName(m_objDataLogic2UI.iSessionID,
            strProcessorName);

    //清除SessionID 错误的子对话框
    _TalkUIProcessorList.ClearInvalidTalkUI();

    return true;
}

//刷新当前界面
void CTalkUILogicFrameManager::RefreshUI()
{
    //调用talkui 接口函数 刷新显示UI
    if (m_pTalkUIFrameManagerBase)
    {
        if (m_pCurrentProcessor)
        {
            m_pTalkUIFrameManagerBase->UpdateUI(m_pCurrentProcessor->GetBindUI(), m_dataUILogicToTalkUI);
        }
    }
}

void CTalkUILogicFrameManager::SessionExit(int iSessionID)
{
    EraseTalkUIProcessor(iSessionID);
}

//根据通话Id清空UI对应的数据指针
void CTalkUILogicFrameManager::ClearSessionData(int iSessionID)
{
    //清除该SessionID对应的缓存数据
    g_objTalkUISessionCacheManager.ClearSessionCache(iSessionID);

    EraseTalkUIProcessor(iSessionID);
}

// 功能  ： 根据通话Id获得通话时间数据
int CTalkUILogicFrameManager::GetTalkTimeBySessionId(int nSessionId)
{
    //从缓存中获取通话时间 如果没有获取到 返回-1
    //这边因为每次都是 talking conf parknotify 在最前面 所以获取的会是正确的
    //比较科学的做法应该是判断界面是不是 talking、conf 、parknotify
    yl::string strProcessorName = "";
    CBaseTalkUIProcessor * pProcessor = _TalkUIProcessorList.FindProcessorBySessionId(nSessionId,
                                        strProcessorName);

    if (NULL != pProcessor)
    {
        return pProcessor->GetTalkTime();
    }

    return 0;
}

//根据Logic层所提供的数据进行错误信息更新，若有错误信息
void CTalkUILogicFrameManager::UpdateErrorInfo(void * pData)
{
    if (NULL == pData)
    {
        return ;
    }

    Error_Info * pErrorInfo = (Error_Info *)pData;

    yl::string strErrorTip = "";//objErrorInfo.strErrorTip;

    switch (pErrorInfo->eErrorType)
    {
    case EIT_INVALID_CODEC_TOCONF:
        {
            strErrorTip = TRID_AUDIO_CODECS_DIFFERENT_CANNOT_SETUP_CONFERENCE;
        }
        break;
    case EIT_NOT_AVALIABLE_G722_SESSION_TOCONF:
        {
            //Conference Codec不一致
            strErrorTip = TRID_G722_CODEC_IS_NOT_AVAILABLE_IN_FOUR_WAY_CONFERENCE;
        }
        break;
    case EIT_NOT_AVALIABLE_OPUS_SESSION_TOCONF:
        {
            //Conference Codec不一致 UI:评审
            strErrorTip = TRID_OPUS_CODEC_IS_NOT_AVAILABLE_IN_FOUR_WAY_CONFERENCE;
        }
        break;
    case EIT_NOT_AVALIABLE_SRTP_SESSION_TOCONF:
        {
            strErrorTip = TRID_SRTP_IS_NOT_AVAILABLE_IN_FOUR_WAY_CONFERENCE;
        }
        break;
    default:
        break;
    }

    if (m_pTalkUIFrameManagerBase)
    {
        yl::string strErrorInfo = _UILOGIC_LANG_TRAN(strErrorTip.c_str());
        m_pTalkUIFrameManagerBase->UpdateErrorInfo(strErrorInfo);
    }
}

//显示弹框提示
void CTalkUILogicFrameManager::UpdateMsgboxNoteNotify(const yl::string strMsgboxNoteNotify)
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateMsgboxNoteNotify(strMsgboxNoteNotify);
    }
}

bool CTalkUILogicFrameManager::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    //当前有保存逻辑处理实例 返回当前的  否则返回false
    if (NULL != m_pCurrentProcessor)
    {
        return m_pCurrentProcessor->GetInputContent(strNum, strDisplayname);
    }

    return false;
}

#if IF_TALKUI_USB_ENABLE
//处理usb消息 连接状态改变
//iState USB状态
LRESULT CTalkUILogicFrameManager::OnUSBRecordMsgStateChange(int iState)
{
    TALKUI_INFO("CTalkUILogicFrameManager::OnUSBRecordMsgStateChange, iState %d", iState);

    if (m_pCurrentProcessor)
    {
        //设置icon相关的信息
        m_pCurrentProcessor->DealSetTalkData(m_objDataLogic2UI.pData);
    }

    if (m_pTalkUIFrameManagerBase)
    {
        //刷新icon
        m_pTalkUIFrameManagerBase->UpdateTalkUIIcons();
        UpdateBlueToothIcon();

        //后面有统一的地方刷新softkey
        m_pTalkUIFrameManagerBase->OnUSBRecordMsgStateChange(iState);

    }

    return 0;
}

//录制警告
// waparm： 警告值（单位秒）
LRESULT CTalkUILogicFrameManager::OnUSBRecordMsgWarning(int iTime)
{
    TALKUI_INFO("CTalkUILogicFrameManager::OnUSBRecordMsgWarning, iTime %d", iTime);

    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->OnUSBRecordMsgWarning(iTime);
    }
    return 0;
}

//容量满 提醒
LRESULT CTalkUILogicFrameManager::OnUSBRecordMsgNotenough()
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->OnUSBRecordMsgNotenough();
    }
    return 0;
}

//录制截图 提醒
// iType：0=开始录制,1=停止录制,2=截图,3=暂停录制,4=恢复录制  iTypeResult：成功与否(0/1)
LRESULT CTalkUILogicFrameManager::OnRecordStorageRecordResult(int iType, int iTypeResult)
{
    TALKUI_INFO("CTalkUILogicFrameManager::OnRecordStorageRecordResult, iType %d, iTypeResult %d",
                iType, iTypeResult);

    if (m_pCurrentProcessor)
    {
        m_pCurrentProcessor->DealSetTalkData(m_objDataLogic2UI.pData, UPDATE_ICON);
    }

    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateTalkUIIcons();
        UpdateBlueToothIcon();

        TALK_RECORD_STATUS eStatus = TALK_RECORD_STATUS_NONE;

        // iType：0=开始录制,1=停止录制,2=截图,3=暂停录制,4=恢复录制  iTypeResult：成功与否(0/1)
        if ((iType == 0) || (iType == 4)
                && iTypeResult == 1
           )
        {
            eStatus = TALK_RECORD_STATUS_START;
        }
        else if (iType == 3 && iTypeResult == 1)
        {
            eStatus = TALK_RECORD_STATUS_PAUSE;
        }
        else if ((iType == 0 && iTypeResult == 0)
                 || (iType == 1 && iTypeResult != 0))
        {
            eStatus = TALK_RECORD_STATUS_STOP;
        }

        m_pTalkUIFrameManagerBase->OnRecordStorageRecordResult(eStatus);
    }

    return 0;
}
#endif

//创建通话UI
CUILogicBaseTalkUI * CTalkUILogicFrameManager::CreateTalkUI(const yl::string & strName)
{
    if (m_pTalkUIFrameManagerBase)
    {
        return m_pTalkUIFrameManagerBase->CreateTalkUI(strName);
    }

    return NULL;
}

//释放通话UI
void CTalkUILogicFrameManager::ReleaseTalkUI(const yl::string & strProcessorName,
        CUILogicBaseTalkUI * pTalkUI)
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->ReleaseTalkUI(strProcessorName, pTalkUI);
    }

}

//刷新softkey
void CTalkUILogicFrameManager::UpdateTalkUISoftkey()
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateTalkUISoftkey();
    }
}

#if IF_TALKUI_CONTACT_PHOTO
//获取会议联系人头像
yl::string CTalkUILogicFrameManager::GetPicContactsConference()
{
    if (m_pTalkUIFrameManagerBase)
    {
        return m_pTalkUIFrameManagerBase->GetPicContactsConference();
    }

    return "";
}

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//获取蓝牙联系人头像
yl::string CTalkUILogicFrameManager::GetPicBluetoothContact()
{
    if (m_pTalkUIFrameManagerBase)
    {
        return m_pTalkUIFrameManagerBase->GetPicBluetoothContact();
    }

    return "";
}
#endif

yl::string CTalkUILogicFrameManager::GetPicCloudContacts()
{
    if (m_pTalkUIFrameManagerBase)
    {
        return m_pTalkUIFrameManagerBase->GetPicCloudContact();
    }

    return "";
}
#endif

void CTalkUILogicFrameManager::UpdateTalkUITitle(yl::string strTitle)
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateTalkUITitle(strTitle);
    }
}

void CTalkUILogicFrameManager::UpdateTalkUIPageNum()
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateTalkUIPageNum(m_dataUILogicToTalkUI.iSessionIndex,
                m_dataUILogicToTalkUI.iSessionCount);
    }
}

//刷新通话时间显示
void CTalkUILogicFrameManager::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateTalkUITime(iHour, iMin, iSecond);
    }
}

//刷新通话图标
//暂时是只有来电界面使用到
void CTalkUILogicFrameManager::UpdateTalkUIIcons()
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateTalkUIIcons();
    }

}

//设置talkuiframemanager实例指针
void CTalkUILogicFrameManager::SetTalkUIFrameManager(CTalkUIFrameManagerBase *
        pTalkUIFrameManagerBase)
{
    m_pTalkUIFrameManagerBase = pTalkUIFrameManagerBase;
}

//获取当前的焦点通话Processor
CBaseTalkUIProcessor * CTalkUILogicFrameManager::GetCurrentTalkUIProcessor()
{
    return m_pCurrentProcessor;
}

//获取通道图标
TALKUI_CHANNEL_ICON CTalkUILogicFrameManager::GetChannelIcon()
{
    return _TalkUILogicDelegate.GetChannelIcon();
}

//获取蓝牙图标
//TALKUI_BLUETOOTH_ICON CTalkUILogicFrameManager::GetBlueToothIcon()
//{
//  return _TalkUILogicDelegate.GetBlueToothIcon();
//}

//刷新蓝牙连接状态
void CTalkUILogicFrameManager::UpdateBlueToothIcon()
{
#ifndef WIN32

#if IF_TALKUI_BLUETOOTH_ENABLE

    //蓝牙没开 不刷蓝牙图标
    if (!Bluetooth_IsEnable())
    {
        return;
    }

    TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon = TALKUI_BLUETOOTH_ICON_UNKNOWN;

    //蓝牙没连接
    if (!Bluetooth_IsConnected())
    {
        eTalkUIBlueToothIcon = TALKUI_BLUETOOTH_ICON_UNCONNECTED;
    }
    //蓝牙连接
    else
    {
#ifndef _WIN32
#warning bluetooth mod
#endif
        //连接了 蓝牙手机
        if (Bluetooth_IsMobileConnected())
        {
            eTalkUIBlueToothIcon = TALKUI_BLUETOOTH_ICON_MOBILE;
        }
        //连接了 蓝牙耳机
//      else if (Bluetooth_IsHeadsetConnected())
//      {
//          eTalkUIBlueToothIcon = TALKUI_BLUETOOTH_ICON_HEADSET;
//      }
        else
        {
            eTalkUIBlueToothIcon = TALKUI_BLUETOOTH_ICON_UNKNOWN;
        }
    }

    //调用UI接口刷新 蓝牙图标
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateBluetoothIcon(eTalkUIBlueToothIcon);
    }

#endif

#endif

}

//更新通道图标
void CTalkUILogicFrameManager::UpdateChannelIcon()
{
    if (NULL != m_pTalkUIFrameManagerBase)
    {
        if (NULL != m_pCurrentProcessor)
        {
            TALKUI_CHANNEL_ICON eIconType;
            if (m_pCurrentProcessor->GetCustomChannelIcon(eIconType))
            {
                m_pTalkUIFrameManagerBase->UpdateChannelIcon(eIconType);
                return;
            }
        }

        m_pTalkUIFrameManagerBase->UpdateChannelIcon(GetChannelIcon());
    }
}

//刷新其他提示
void CTalkUILogicFrameManager::UpdateOtherNotify(yl::string strOtherNotify)
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateOtherNotify(strOtherNotify);
    }

}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
//更新ACD队列状态
void CTalkUILogicFrameManager::UpdateACDQueueStatus()
{
    //获取queuestatus 数据
    yl::string strDisplay = _TalkUILogicDelegate.GetQueueStatus();

    //调用更新
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateACDQueueStatus(strDisplay);
    }
}
#endif

//检查网络，异常则给出提示，否则清除提示
void CTalkUILogicFrameManager::CheckNetwork()
{
    if (NULL != m_pCurrentProcessor
            && NULL != m_pTalkUIFrameManagerBase)
    {
        //先判断网络是否正常
        //网络不正常
        //processor没有判断的话就由netIsNetworkReady来判断
        if (TALK_NETWORK_STATUS_UNKNOWN == m_pCurrentProcessor->GetNetworkStatus())
        {
            if (FALSE == netIsNetworkReady())
            {
                m_pTalkUIFrameManagerBase->UpdateNetworkStatus(TALK_NETWORK_STATUS_FAIL);
            }
            else
            {
                m_pTalkUIFrameManagerBase->UpdateNetworkStatus(TALK_NETWORK_STATUS_READY);
            }
        }
        else
        {
            m_pTalkUIFrameManagerBase->UpdateNetworkStatus(m_pCurrentProcessor->GetNetworkStatus());
        }
    }
}

void CTalkUILogicFrameManager::UpdateVolumebar(bool bShow)
{
    if (NULL != m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->UpdateVolumebar(bShow);
    }
}

//检查wifi信号，异常则给出提示，否则清除提示
void CTalkUILogicFrameManager::SetWifiNotify(bool bIsReady)
{
    if (m_pTalkUIFrameManagerBase != NULL)
    {
        m_pTalkUIFrameManagerBase->UpdateWifiStatus(bIsReady);
    }
}

//清除所有的通话数据  收到sessionID 为-1 的时候会调用
void CTalkUILogicFrameManager::ClearTalkProcessor()
{
    TALKUI_INFO("CTalkUILogicFrameManager::ClearTalkProcessor begin");
    //先把 UI界面全部清除
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->ClearTalkUI();
    }
//  _TalkUIFrameManager.ClearTalkUI();
    //当前的 逻辑处理实例设置为空
    m_pCurrentProcessor = NULL;
    m_dataUILogicToTalkUI.Reset();
    //缓存清除所有数据
    _TalkUIProcessorList.ClearAllProcessor();

    TALKUI_INFO("CTalkUILogicFrameManager::ClearTalkProcessor end");

}

void CTalkUILogicFrameManager::ClearTalkUICache()
{
    if (m_pTalkUIFrameManagerBase)
    {
        m_pTalkUIFrameManagerBase->ClearTalkUICache();
    }
}

//清除一个session
void CTalkUILogicFrameManager::EraseTalkUIProcessor(int iSessionID)
{
    if (NULL != m_pCurrentProcessor)
    {
        //如果当前显示的sessionID 相同 那么要先把 UI中显示的给去除
        if (m_pCurrentProcessor->GetSessionID() == iSessionID)
        {
            if (m_pTalkUIFrameManagerBase)
            {
                m_pTalkUIFrameManagerBase->SetFocusSessionDataInvalid();
            }
        }
    }

    //通话子界面缓存  设置一下 sessionID 为错误的
    _TalkUIProcessorList.SetSessionDataInvalid(iSessionID);
}

// 根据逻辑层上传数据 创建UI
CBaseTalkUIProcessor * CTalkUILogicFrameManager::CreateTalkUIByData(int iSessionID,
        const yl::string & strName)
{
    CBaseTalkUIProcessor * pProcessor = _TalkUIProcessorList.GetProcessorBySessionIDAndName(iSessionID,
                                        strName);
    return pProcessor;
}

//收集 给TalkUI 的数据
void CTalkUILogicFrameManager::CollectTalkUIData(DataLogic2UI & dataLogic2UI)
{
    int iSessionNum = talklogic_GetAmountOfSession();
    m_dataUILogicToTalkUI.iSessionCount = iSessionNum;
    m_dataUILogicToTalkUI.iSessionID = dataLogic2UI.iSessionID;
    m_dataUILogicToTalkUI.sessionState = dataLogic2UI.eSessionState;
    m_dataUILogicToTalkUI.eUpateReason = dataLogic2UI.eUpateReason;

    //获取Sessionn的index
    m_dataUILogicToTalkUI.iSessionIndex = talklogic_GetFocusedSessionIndex();
}

void CTalkUILogicFrameManager::CheckSecurityClassification()
{
    if (m_pCurrentProcessor == NULL)
    {
        return;
    }

    m_pTalkUIFrameManagerBase->UpdateSecurityClassification(m_pCurrentProcessor->IsShowSecurityClassification());
}

