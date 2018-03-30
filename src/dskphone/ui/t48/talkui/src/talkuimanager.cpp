//mod-base
#include "talkui_define.h"
#include "talkuimanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

//mod-UIKernel
#include "uikernel/languagehelper.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"

//mod-this(TalkUI)
#include "dlgtalkuiframe.h"
#include "dlgbasetalkui.h"

#include "qtcommon/qmisc.h"

//mod-dsskey
#include "dsskey/include/moddsskey.h"

//call center
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonapi/stringhelperapi.h"

// Bluetooth
#include "extended/bluetoothmessagedefine.h"

// PopupBoxManager
#include "uicommon/messagebox/modmessagebox.h"

#include "talkuifactory.h"
#include "talkuidelegate.h"
#include "talkuiframemanager.h"


#include "callmanager/modcallmanager.h"
#include "commonfunction.h"
#include "dlgdialingui.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/modrecord.h"
#include "record/include/recordmessagedefine.h"
#include "idlescreen/include/modidlescreen.h"
#endif
#include "application/include/modapplication.h"
#include "voice/include/modvoice.h"
//DND消息头文件
#include "talk/fwd_dnd/include/fwddndmsgdefine.h"

static LRESULT OnTalkUIMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case ACD_MESSAGE_QUEUE_STATUS:
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
        _TalkUIManager.ShowACDQueueStatus();
#endif
        break;
    case BLUETOOTH_MSG_CONNECT_STATE:
        {
            _TalkUIManager.RefreshBlueToothIcon();
            _TalkUIManager.UpdateChannelIcon();     //蓝牙连接更新 可能导致通道更新
        }
        break;
    case VOICE_CHANGE_CHANNEL:
        {
            _TalkUIManager.UpdateChannelIcon();
        }
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case RECORD_STORAGE_STATE_CHANGE:
    case RECORD_STORAGE_VOLUME_WARNING:
    case RECORD_STORAGE_SPACE_NOTENOUGH:
    case RECORD_STORAGE_RECORD_RESULT:
        {
            _TalkUIManager.OnTalkUIUSBRecordMsg(objMessage);
        }
        break;
#endif
#ifdef IF_FEATURE_ENTRANCE_GUARD
    case ENTRANCE_GUARD_MESSAGE_START:
    case ENTRANCE_GUARD_MESSAGE_STOP:
    case ENTRANCE_GUARD_MESSAGE_PAUSE:
    case ENTRANCE_GUARD_MESSAGE_REFRESH:
    case ENTRANCE_GUARD_MESSAGE_TIME_OUT:
    case ENTRANCE_GUARD_MESSAGE_ERROR:
        {
            _TalkUIFrameMananger.OnDoorLineMessage(objMessage);
        }
        break;
#endif
    //http://10.2.1.199/Bug.php?BugID=94102 状态改变logic发送消息提示刷新通道图标
    case BLUETOOTH_MSG_CHANNEL_CHANGE:
        {
            _TalkUIManager.UpdateChannelIcon();
        }
        break;
    //DND变化消息
    case DND_MESSAGE_STATUS_CHANGED:
        {
            if (!configParser_GetConfigInt(kszKeepDNDAfterTalk))
            {
                _TalkUIFrameMananger.RefreshDNDIcon();
            }
            else
            {
                //关闭配置,不刷新DND图标
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

//构造函数
CTalkUIManager::CTalkUIManager(void)
    : m_pCurrentTalkUI(NULL)
{

}

//析构函数
CTalkUIManager::~CTalkUIManager(void)
{

}

//初始化管理对象
bool CTalkUIManager::InitManager()
{
    //当前的通话子界面设置为空
    m_pCurrentTalkUI = NULL;
    //初始化TalkUIFactory
    _TalkUIFactory.Init();
    //初始化显示框架
    _TalkUIFrameMananger.Init();

    etl_RegisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS, OnTalkUIMsg);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, OnTalkUIMsg);
    etl_RegisterMsgHandle(VOICE_CHANGE_CHANNEL, VOICE_CHANGE_CHANNEL, OnTalkUIMsg);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_RECORD_RESULT, OnTalkUIMsg);
#endif
#ifdef IF_FEATURE_ENTRANCE_GUARD
    etl_RegisterMsgHandle(ENTRANCE_GUARD_MESSAGE_BEGIN, ENTRANCE_GUARD_MESSAGE_END, OnTalkUIMsg);
#endif
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CHANNEL_CHANGE, BLUETOOTH_MSG_CHANNEL_CHANGE, OnTalkUIMsg);
    //DND消息注册
    etl_RegisterMsgHandle(DND_MESSAGE_STATUS_CHANGED, DND_MESSAGE_STATUS_CHANGED, OnTalkUIMsg);
    return true;
}

//注销管理对象
void CTalkUIManager::UninitManager()
{
    etl_UnregisterMsgHandle(ACD_MESSAGE_QUEUE_STATUS, ACD_MESSAGE_QUEUE_STATUS, OnTalkUIMsg);
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, OnTalkUIMsg);
    etl_UnregisterMsgHandle(VOICE_CHANGE_CHANNEL, VOICE_CHANGE_CHANNEL, OnTalkUIMsg);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_RECORD_RESULT, OnTalkUIMsg);
#endif
#ifdef IF_FEATURE_ENTRANCE_GUARD
    etl_UnregisterMsgHandle(ENTRANCE_GUARD_MESSAGE_BEGIN, ENTRANCE_GUARD_MESSAGE_END, OnTalkUIMsg);
#endif
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CHANNEL_CHANGE, BLUETOOTH_MSG_CHANNEL_CHANGE, OnTalkUIMsg);
    //DND消息注销
    etl_UnregisterMsgHandle(DND_MESSAGE_STATUS_CHANGED, DND_MESSAGE_STATUS_CHANGED, OnTalkUIMsg);
    //当前子界面设置为空
    m_pCurrentTalkUI = NULL;
    //反初始化TalkUIFrameMananger  先于TalkUIFactory执行 因为通话子界面实例指针 实际是放在talkuifactory中的
    _TalkUIFrameMananger.Uninit();
    //反初始化TalkUIFactory
    _TalkUIFactory.UnInit();
}

CTalkUIManager & CTalkUIManager::GetInstance()
{
    static CTalkUIManager s_TalkUIManager;
    return s_TalkUIManager;
}

//清理TalkUI映射表
void CTalkUIManager::ClearTalkUIMap()
{
    m_pCurrentTalkUI = NULL;

    _TalkUIFrameMananger.ClearTalkUI();
    _TalkUIFactory.ClearAllTalkUI();
}

//根据SessionId与UI名查找TalkUI指针，当存在多个匹配SessionId的UI时，同时UI名为空时，返回列表中第一个窗口指针
CDlgBaseTalkUI * CTalkUIManager::FindTalkUIBySessionId(int nSessionId, yl::string strUIName /* = */)
{
    return _TalkUIFactory.FindTalkUIBySessionId(nSessionId, strUIName);
}


//根据SessionId从映射表移除TalkUI
bool CTalkUIManager::EraseTalkUIFromMap(int nSessionId)
{
    //这边先判断一下是不是当前正在显示的 通话  如果是的话 要先把通话框架中的对话框设置为空
    if (NULL != m_pCurrentTalkUI)
    {
        if (m_pCurrentTalkUI->GetSessionId() == nSessionId)
        {
            //当前的通话子界面要设置为空
            m_pCurrentTalkUI = NULL;
            //显示的要设置为空
            _TalkUIFrameMananger.SetTalkUI(NULL);
        }
    }

    //在通话子界面管理器中 将对话框子界面清除
    _TalkUIFactory.ClearTalkUIBySessionID(nSessionId);

    return true;

    //bool bResult = false;
    ////查找TalkUI
    //ListTalkUI::iterator itTarget = m_listTalkUI.begin();
    //for (; itTarget != m_listTalkUI.end();)
    //{
    //  CDlgBaseTalkUI* pTalkUI = (CDlgBaseTalkUI*)*itTarget;
    //  if (pTalkUI != NULL
    //      && pTalkUI->GetSessionId() == nSessionId)
    //  {
    //      //查找到对应SessionId的TalkUI指针，从列表中移除
    //      if (m_pTalkUIFrame != NULL
    //          && m_pTalkUIFrame->GetTalkUI() == pTalkUI)
    //      {
    //          m_pTalkUIFrame->SetTalkUI(NULL);
    //      }

    //      itTarget = m_listTalkUI.erase(itTarget);
    //      //释放UI窗口内容
    //      ReleaseTalkUI(pTalkUI);//--
    //      //itTarget = m_listTalkUI.erase(itTarget);
    //      bResult = true;
    //  }
    //  else
    //  {
    //      ++itTarget;
    //  }
    //}

    ////查找不到，返回失败
    //return bResult;
}

//清除对应sessionid 的数据
bool CTalkUIManager::ClearSessionData(int iSessionID)
{
    //这边先判断一下是不是当前正在显示的 通话  如果是的话 TalkUIFrameManager要 处理一下SessionID
    if (NULL != m_pCurrentTalkUI)
    {
        if (m_pCurrentTalkUI->GetSessionId() == iSessionID)
        {
            _TalkUIFrameMananger.SetFocusSessionDataInvalid();
        }
    }

    //先将 talkuifactory 的sessionID 处理  然后再处理talkuiFramemanager 的sessionID
    //通话子界面缓存  设置一下 sessionID 为错误的
    _TalkUIFactory.SetSessionDataInvalid(iSessionID);

    //TalkUIFrameManager 处理一下SessionID
    //_TalkUIFrameMananger.SetFocusSessionDataInvalid();

    return true;
}

//根据逻辑层数据更新UI内容
void CTalkUIManager::UpdateContentByData(DataLogic2UI & objData)
{
    //判断逻辑层数据内容
    if (objData.iSessionID == -1)
    {
        //SessionId为-1或UI名为空，则代表通话列表为空
        TALKUI_INFO("TalkUI: Session id or ui name is empty, return to idle!");
        //清理TalkUIMap

        // 退出通话界面的时候先清除所有的弹出框
        //MsgBox_RemoveAllPopupBox();
        //清空TalkUI的时候如果 这个时候有存在通话那么 才移除所有弹框
        if (_TalkUIFactory.GetTalkUICount() > 0)
        {
            _TalkUIDelegate.RemoveAllMessageBox();
        }

        m_pCurrentTalkUI = NULL;

        _TalkUIFrameMananger.ClearTalkUI();
        _TalkUIFactory.ClearAllTalkUI();

        return;
    }

    //根据所传递的逻辑数据获取窗口名，同时重新对相关联的逻辑数据进行缓存赋值
    yl::string strUIName = GetDlgNameAndCheckLogicData(objData, &m_tLogicData);

    //根据数据获取TalkUI对象指针
    //这边使用的必须是objData ， 因为 m_tLogicData已经和 objData不一样了
    CDlgBaseTalkUI * pTalkUI = GetTalkUIByData(objData);

    if (NULL == pTalkUI)
    {
        //初次创建TalkUI，调用UIManager的接口生成数据指针
        TALKUI_INFO("CTalkUIManager::UpdateContentByData Failed For Create TalkUI Failed TalkUIName = %s",
                    strUIName.c_str());
        return ;
    }

    //设置当前显示的talkUI
    m_pCurrentTalkUI = pTalkUI;

    //给通话显示框架类设置子窗口 和 数据
    _TalkUIFrameMananger.SetTalkUI(pTalkUI, &m_tLogicData);

    //清除无效的通话窗口
    _TalkUIFactory.ClearInvalidTalkUI();

    return ;
}

//内部刷新界面接口
void CTalkUIManager::UpdateWndByData(DataLogic2UI & objData, const yl::string & strUIName)
{
    CDlgBaseTalkUI * v_pTalkUI = _TalkUIFactory.GetTalkUIBySessionIDAndUIName(objData.iSessionID,
                                 strUIName);

    //指针为空 直接返回
    if (NULL == v_pTalkUI)
    {
        return ;
    }

    //给通话显示框架类设置子窗口 和 数据
    _TalkUIFrameMananger.SetTalkUI(v_pTalkUI, &objData);

    //清除无效的通话窗口
    _TalkUIFactory.ClearInvalidTalkUI();
}

//根据逻辑层数据获取TalkUI指针
CDlgBaseTalkUI * CTalkUIManager::GetTalkUIByData(const DataLogic2UI & objData)
{
    TALKUI_INFO("GetTalkUIByData(%d, %d)!", objData.iSessionID, objData.eSessionState);
    yl::string strUIName = GetDlgNameAndCheckLogicData(objData);

    CDlgBaseTalkUI * v_pTalkUI = _TalkUIFactory.GetTalkUIBySessionIDAndUIName(objData.iSessionID,
                                 strUIName);

    return v_pTalkUI;

}

//刷新通道图标
void CTalkUIManager::UpdateChannelIcon()
{
    _TalkUIDelegate.UpdateChannelIcon();
//#ifdef _T48
//
//  if(_TalkUIFactory.GetTalkUICount() > 0)
//  {
//      _MainWND.SetTitleAccountIcon(::GetChannelIcon());
//  }
//
//#else
//  //处于通话界面
//  _TalkUIFrameMananger.UpdateChannelIcon();
//
//#endif
}

//根据逻辑层数据更新错误提示界面
void CTalkUIManager::UpdateErrorInfoByData(const Error_Info & objErrorInfo)
{
    string strErrorTip = "";//objErrorInfo.strErrorTip;
    switch (objErrorInfo.eErrorType)
    {
    case EIT_INVALID_CODEC_TOCONF:
        {
            strErrorTip =  TRID_AUDIO_CODECS_DIFFERENT_CANNOT_SETUP_CONFERENCE;
        }
        break;
    case EIT_NOT_AVALIABLE_G722_SESSION_TOCONF:
        {
            //Conference Codec不一致
            strErrorTip = TRID_G722_CODEC_IS_NOT_AVAILABLE_IN_FOUR_WAY_CONFERENCE;
        }
        break;
    case EIT_NOT_AVALIABLE_SRTP_SESSION_TOCONF:
        {
            strErrorTip = TRID_SRTP_IS_NOT_AVAILABLE_IN_FOUR_WAY_CONFERENCE;
        }
        break;
    case ETI_ALREADY_PARKED_TO_THIS_SLOT:
        {
            strErrorTip = TRID_CALL_ALREADY_PARKED_TO_THIS_SLOT;
        }
        break;
    default:
        break;
    }

    _TalkUIDelegate.ShowNoteMessageBox(LANG_TRANSLATE(strErrorTip.c_str()), objErrorInfo.iErrorShowTime == -1 ? SHORT_TIME : objErrorInfo.iErrorShowTime);
    //弹出提示框
//  MsgBox_PopupMessageBox(LANG_TRANSLATE(strErrorTip.c_str()), PB_TYPE_NOTE, SHORT_TIME);
}

//输入输入框内容，并返回当前输入内容
bool CTalkUIManager::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    return _TalkUIFrameMananger.GetInputContent(strNum, strDisplayname);

    //CBaseDialog* pBaseDlg = CallManager_GetCurrentPage();
    //if (NULL != pBaseDlg
    //  && pBaseDlg->inherits("CDlgDialingUI"))
    //{
    //  CDlgDialingUI* pDialingUI = (CDlgDialingUI*)pBaseDlg;
    //  return pDialingUI->GetInputContent(strNum, strDisplayname);
    //}

    //return false;
}

int CTalkUIManager::GetInputCallID()
{
    return _TalkUIFrameMananger.GetInputCallID();
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
//显示ACD Queue Status
void CTalkUIManager::ShowACDQueueStatus()
{
    if (!UIManager_IsInTalk())
    {
        return;
    }

    yl::string strDisplay;
    if (CallCenter_IsEnableQueueStatus()
            && AS_IDLE != ACD_GetAcdStatus()
            && configParser_GetConfigInt(kszBroadsoftActive) != 0
            && ACD_BROADSOFT == Get_ACDType())
    {
        CallCenterQueueStatus ccQueueStatus = ACD_GetQueueStatus();
        strDisplay = ccQueueStatus.m_strName;
        strDisplay += ":";
        if (QUEUE_EMPTY == ccQueueStatus.m_eStatus)
        {
            strDisplay += fromQString(LANG_TRANSLATE(TRID_EMPTY));
        }
        else if (QUEUE_NORMAL == ccQueueStatus.m_eStatus)
        {
            strDisplay += fromQString(LANG_TRANSLATE(TRID_QUEUEING));
        }
        else if (QUEUE_THRESHOLD_EXCEEDED == ccQueueStatus.m_eStatus)
        {
            strDisplay += fromQString(LANG_TRANSLATE(TRID_ALERT));
        }
        else
        {
            strDisplay = "";
        }
    }
    else
    {
        strDisplay = "";
    }

    //调用框架显示 ACDStatus 信息
    _TalkUIFrameMananger.UpdateQueueStatus(strDisplay);

    //if (m_pTalkUIFrame != NULL)
    //{
    //  m_pTalkUIFrame->UpdateQueueStatus(strDisplay);
    //}
}
#endif

//更新录音图标
void CTalkUIManager::UpdateRecordIcon(ICON_TYPE eIconType)
{
    //处于通话界面
    _TalkUIFrameMananger.UpdateRecordIcon(eIconType);
    ////处于通话界面
    //if (m_pTalkUIFrame != NULL)
    //{
    //  //通话UI框架存在
    //  m_pTalkUIFrame->SetRecordTip(eIconType);
    //}
}

// 更新蓝牙图标
void CTalkUIManager::RefreshBlueToothIcon()
{
    _TalkUIDelegate.RefreshBlueToothIcon();
    //if (NULL != m_pTalkUIFrame)
    //{
    //  m_pTalkUIFrame->RefreshBlueToothIcon();
    //}
}

void CTalkUIManager::ExitDialingUI()
{
    CDlgBaseTalkUI * pTalkUI = _TalkUIFactory.GetTalkUIByTalkUIName(DLG_Dial);
    if (NULL != pTalkUI)
    {
        pTalkUI->ExitTalkUI();
    }

    //ListTalkUI listTemp;
    //ListTalkUI::iterator itTarget = m_listTalkUI.begin();
    //for (; itTarget != m_listTalkUI.end(); ++itTarget)
    //{
    //  CDlgBaseTalkUI* pTempUI = (CDlgBaseTalkUI*)*itTarget;
    //  if (pTempUI != NULL && pTempUI->inherits("CDlgDialingUI"))
    //  {
    //      listTemp.push_back(pTempUI);
    //  }
    //}

    //itTarget = listTemp.begin();
    //for (; itTarget != listTemp.end(); ++itTarget)
    //{
    //  (*itTarget)->ExitTalkUI();
    //}
}

//void CTalkUIManager::ReleaseTalkUI(CBaseDialog* pBaseDialog, bool bClearList)
//{
//  if (NULL == pBaseDialog)
//  {
//      return;
//  }
//
//  if (pBaseDialog->inherits(DLG_BaseTalkUI))
//  {
//      CDlgBaseTalkUI* pTalkUI = (CDlgBaseTalkUI*)pBaseDialog;
//      if (pTalkUI->GetUIName() == DLG_Dial)
//      {
//          bool bHasDial = false;
//          ListTalkUI::iterator it = m_listTalkUI.begin();
//          for ( ; it != m_listTalkUI.end(); ++it)
//          {
//              CDlgBaseTalkUI* pTempTalk = *it;
//              if (pTempTalk->GetUIName() == DLG_Dial)
//              {
//                  bHasDial = true;
//                  break;
//              }
//          }
//
//          if (!bHasDial || bClearList)
//          {
//              CallManager_PutPage(pTalkUI);
//          }
//          return;
//      }
//  }
//
//  UIManager_PutPrivateDlg(pBaseDialog);
//}

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
void CTalkUIManager::OnTalkUIUSBRecordMsg(msgObject & objMessage)
{
    if (!UIManager_IsInTalk())
    {
        return;
    }

    TALKUI_INFO("CTalkUIManager::OnTalkUIUSBRecordMsg[%d] [%d] [%d]", objMessage.message,
                objMessage.wParam, objMessage.lParam);

    switch (objMessage.message)
    {
    case RECORD_STORAGE_STATE_CHANGE:
        {
            if (objMessage.wParam == 1)
            {
                //yl::string strNote = fromQString(LANG_TRANSLATE(TRID_USB_DEVICE_CONNECTED));
                //strNote += "\n";
                //strNote += fromQString(LANG_TRANSLATE(TRID_RECORD_TIME));
                //strNote += ":";
                //yl::string strTime = modRecord_GetAvailableRecordingTime();
                //strNote.append(strTime.c_str());
                //MessageBox_ShowCommonMsgBox(NULL, toQString(strNote), MESSAGEBOX_NOTE, 2000);
                _TalkUIFrameMananger.USBRecordStateChange();
            }
            else if (objMessage.wParam == 0)
            {
                _TalkUIFrameMananger.USBRecordStateChange();
            }
        }
        break;
    case RECORD_STORAGE_VOLUME_WARNING:
        {
            //提示上来的是 以秒为单位的 所以 要除60
            int iTime = objMessage.wParam / 60;
            yl::string strWarning = commonAPI_FormatString("%d", iTime);
            strWarning += " ";
            strWarning += fromQString(LANG_TRANSLATE(TRID_MINUTES_LEFT_FOR_CALL_RECORDING));
            strWarning += ".";
            MessageBox_ShowCommonMsgBox(NULL, toQString(strWarning), MESSAGEBOX_Cancel);
        }
        break;
    case RECORD_STORAGE_SPACE_NOTENOUGH:
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SPACE_FULL_SAVE_FILE), MESSAGEBOX_Cancel);
        }
        break;
    case RECORD_STORAGE_RECORD_RESULT:
        {
            _TalkUIFrameMananger.USBRecordStateChange();
        }
        break;
    default:
        break;
    }
}
#endif
