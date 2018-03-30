///////////////talkui//////////////
#include "talkuiframemanager.h"
#include "talkuicommonfunction.h"
#include "talkuimsgboxmanager.h"
#include "talkuipath.h"

#include "talkuifactory.h"

#include "dlgdialingui.h"
#include "dlgtalkingui.h"
#include "dlgringui.h"
#include "dlgacdcallinfoui.h"
#include "dlgconnectui.h"
#include "dlgfinishui.h"
#include "dlgautoredialui.h"
#include "dlgparknotifyui.h"
#include "dlgconfui.h"
#include "dlgconfmanagerui.h"
#include "dlgrtpstatusui.h"

////////////uimanager//////////////
#include "uimanager/moduimanager.h"

//各通话子界面
#include "dlgdialingui.h"
#include "dlgtalkingui.h"

#include "talkuilogic/talkuilogicprocessorname.h"
#include "uimanager/modvolumebar.h"


namespace talkui
{


CTalkUIFrameManager & CTalkUIFrameManager::GetInstance()
{
    static CTalkUIFrameManager s_objTalkUIFrameManager;

    return s_objTalkUIFrameManager;
}

CTalkUIFrameManager::CTalkUIFrameManager()
    : m_pTalkUIFrame(NULL)
    , m_frameType(FRAME_TYPE_NONE)
    , m_bIsTalkUIFrameNew(false)
{

}

CTalkUIFrameManager::~CTalkUIFrameManager()
{

}

//初始化
bool CTalkUIFrameManager::Init()
{
    //给talkuilogicframemanager 设置UI指针
    _TalkUILogicFrameManager.SetTalkUIFrameManager(this);
    _TalkUIFactory.init();

    m_pTalkUIFrame = NULL;
    m_frameType = FRAME_TYPE_NONE;

    return true;
}

//反初始化
void CTalkUIFrameManager::Unint()
{
    //先清除 talkuilogicframemanager 中保存的指针
    _TalkUILogicFrameManager.SetTalkUIFrameManager(NULL);

    ClearTalkUI();

    m_pTalkUIFrame = NULL;
    m_frameType = FRAME_TYPE_NONE;
}

//这边pBaseTalkUI 可为NULL， 表示退出当前通话UI显示  ， 但是不表示 退出通话框架
void CTalkUIFrameManager::UpdateUI(CUILogicBaseTalkUI * pUILogicBaseTalkUI,
                                   CUILogicToTalkUIData & dataUILogicToTalkUI)
{
    OnUpdateUI(pUILogicBaseTalkUI, dataUILogicToTalkUI);
}

//实际处理 切换显示 通话UI
//这边pBaseTalkUI 可为NULL， 表示退出当前通话UI显示  ， 但是不表示 退出通话框架  , 暂时是不会有对话框为NULL的情况
void CTalkUIFrameManager::OnUpdateUI(CUILogicBaseTalkUI * pUILogicBaseTalkUI,
                                     CUILogicToTalkUIData & dataUILogicToTalkUI)
{
    TALKUI_INFO("CTalkUIFrameManager::UpdateUI  pBaseTalkUI=0x%x", pUILogicBaseTalkUI);

    //清除 talkui 对话框架
    if (NULL == pUILogicBaseTalkUI)
    {
        //不显示通话界面  那么跟通话相关的弹框要清除掉
        //主要是去除confmanager 界面的弹出框
        //子窗口 有变化 那么 要去除所有弹出框
        g_objTalkUIMsgboxManager.RemoveMessageBox(NULL, TALKUI_MSGBOX_TYPE_TALKUIFRAME);

        if (NULL != m_pTalkUIFrame)
        {
            m_pTalkUIFrame->SetTalkUI(NULL);
        }

        //把活动的框架设置为空
        SetCurrentFrameType(FRAME_TYPE_NONE);

        return ;
    }

    //强转子通话界面
    CDlgBaseTalkUI * pBaseTalkUI = _TalkUIFactory.UILogicTalkUIToBaseTalkUI(pUILogicBaseTalkUI->GetProcessorName(),
                                   pUILogicBaseTalkUI);
    //这边是有可能转换失败的
    if (NULL == pBaseTalkUI)
    {
        return ;
    }

    //设置通话子窗口
    //如果是与当前的子窗口相同 那么刷新窗口直接返回
    if (GetTalkUI() == pBaseTalkUI)
    {
        if (pBaseTalkUI->NeedReUpdate())
        {
            if (NULL != m_pTalkUIFrame)
            {
                //调用接口刷新通话UI子界面
                m_pTalkUIFrame->UpdateTalkUI(false);

                //talkuiframe还是要更新一下数据的
                m_pTalkUIFrame->SetData(dataUILogicToTalkUI);

                //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
                m_pTalkUIFrame->RefreshTalkUIData();

                //不是顶层窗口就不需要刷新界面了也不要置顶
                if (UIManager_GetTopWindow() == m_pTalkUIFrame)
                {
                    //设置数据后要更新窗口 主要是为了刷softkey等
                    UIManager_UpdateWnd(m_pTalkUIFrame);
                }
            }
        }
        else
        {
            //不是顶层窗口就不需要刷新界面了也不要置顶
            if (UIManager_GetTopWindow() == m_pTalkUIFrame)
            {
                //设置数据后要更新窗口 主要是为了刷softkey等
                UIManager_UpdateWnd(m_pTalkUIFrame);
            }
        }

        return ;
    }

    //清除通话中的弹框
    //主要是去除confmanager 界面的弹出框
    //子窗口 有变化 那么 要去除所有弹出框
    g_objTalkUIMsgboxManager.RemoveMessageBox(NULL, TALKUI_MSGBOX_TYPE_TALKUI);

    //判断TalkUIFrame是否为空
    if (NULL == m_pTalkUIFrame)
    {
        CreateTalkUIFrame();

        //设置talkuiframe是新new出来的
        m_bIsTalkUIFrameNew = true;

        //第一次进入通话页面 需要退出其他界面
        UIManager_ReturnToIdle();

        //当创建TalkUIFrame的时候难免需要调用 UIMananger_UpdateWnd 两次  因为 第二次 在设置talkui的时候 还要冲刷 softkey
    }
    else
    {
        m_bIsTalkUIFrameNew = false;    //设置talkuiframe不是新new出来的
    }

    //当前活动框架类型设置为 TalkUIFrame
    SetCurrentFrameType(FRAME_TYPE_TALKUIFRAME);

    //显示talkuiframe
    if (NULL != m_pTalkUIFrame)
    {
        //这边不用清除CallManager的子窗口了 只需要给TalkUIFrame设置就行
        m_pTalkUIFrame->SetTalkUI(pBaseTalkUI);

        //调用接口刷新通话UI子界面
        m_pTalkUIFrame->UpdateTalkUI(true);

        //先设置数据  talkuiframe还是要设置数据的
        m_pTalkUIFrame->SetData(dataUILogicToTalkUI);

        //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
        m_pTalkUIFrame->RefreshTalkUIData();


        //如果获取顶层窗口相同 只要刷新就可以了
        if (UIManager_GetTopWindow() == m_pTalkUIFrame)
        {
            //主要是为了刷新  softkey 等
            UIManager_UpdateWnd(m_pTalkUIFrame);
        }
        else if (true == m_bIsTalkUIFrameNew)   //如果talkuiframe是新new出来的 那么是需要置顶
        {
            UIManager_SetTopWindow(m_pTalkUIFrame);
        }
    }
}

//把焦点的session数据设置无效
//clearsessiondata 和 sessionExit 的是当前的焦点对话框的时候会调用这个函数
void CTalkUIFrameManager::SetFocusSessionDataInvalid()
{
//这边暂时不需要做什么处理
}

//清除ui窗口缓存
void CTalkUIFrameManager::ClearTalkUICache()
{
    _TalkUIFactory.ReleaseAllUI();
}

//清除子窗口 UILogic 收到sessionID为 -1 的时候 会调用该函数
void CTalkUIFrameManager::ClearTalkUI()
{
    g_objTalkUIMsgboxManager.RemoveMessageBox(NULL, TALKUI_MSGBOX_TYPE_TALKUIFRAME);

    //先将子窗口设置为NULL  该函数内会对各状态进行设置
    CUILogicToTalkUIData objUILogicToTalkUIData;

    //先把窗口设置为空
    UpdateUI(NULL, objUILogicToTalkUIData);

    if (NULL != m_pTalkUIFrame)
    {
        UIManager_PutPrivateDlg(m_pTalkUIFrame);
        m_pTalkUIFrame = NULL;
    }
}

//显示/刷新 错误信息
void CTalkUIFrameManager::UpdateErrorInfo(const yl::string & strErrorTip, int nTime/* = TALK_MSGBOX_SHORT_TIME*/)
{
    g_objTalkUIMsgboxManager.ShowTalkUINoteMsgBox(strErrorTip, TALKUI_MSGBOX_TYPE_TALKUIFRAME);
}

//显示弹框提示
void CTalkUIFrameManager::UpdateMsgboxNoteNotify(const yl::string strMsgboxNoteNotify, int nTime/* = TALK_MSGBOX_SHORT_TIME*/)
{
    g_objTalkUIMsgboxManager.ShowTalkUINoteMsgBox(strMsgboxNoteNotify, TALKUI_MSGBOX_TYPE_TALKUIFRAME);
}

//显示 错误信息， 这个是在talkuiframe 的 标题栏 中显示的, 在拨号界面不会显示
void CTalkUIFrameManager::UpdateOtherNotify(yl::string strOtherNotify)
{
    //无论是否在 talkuiframe 都显示  因为如果是在拨号界面  那么退出拨号界面的时候回到talkuiframe 还要显示出来

    //t46 暂时没有该提示
    //if(NULL != m_pTalkUIFrame)
    //{
    //  m_pTalkUIFrame->UpdateOtherNotify(strOtherNotify);
    //}

}

////显示talking 界面失败提示
//void CTalkUIFrameManager::UpdateTalkingFailedNotify(CDlgBaseTalkUI* pTalkUI, yl::string strTalkingFailedNotify)
//{
//  //if(m_pTalkUIFrame)
//  //{
//  //  m_pTalkUIFrame->UpdateTalkingFailedNotify(pTalkUI, strTalkingFailedNotify);
//  //}
//
//}

// 获取子窗口指针
CDlgBaseTalkUI * CTalkUIFrameManager::GetTalkUI() const
{
    if (m_pTalkUIFrame)
    {
        return m_pTalkUIFrame->GetTalkUI();
    }

    return NULL;
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
//更新ACD队列状态
void CTalkUIFrameManager::UpdateACDQueueStatus(yl::string strACDQueueStatus)
{
    //无论是否在 talkuiframe 都显示  因为如果是在拨号界面  那么退出拨号界面的时候回到talkuiframe 还要显示出来
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateACDQueueStatus(strACDQueueStatus);
    }
}
#endif

//更新网络连接状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
void CTalkUIFrameManager::UpdateNetworkStatus(TALK_NETWORK_STATUS eStatus)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateNetworkStatus(eStatus);
    }
}

//更新WIFI信号状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
void CTalkUIFrameManager::UpdateWifiStatus(bool bIsReady)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateWifiStatus(bIsReady);
    }
}

#if IF_TALKUI_USB_ENABLE
//处理usb消息 连接状态改变
//iState USB状态
LRESULT CTalkUIFrameManager::OnUSBRecordMsgStateChange(int iState)
{
    // iState： 0=断开,1=连接上,2=正在检测,3=无法识别

    //无论是否在 talkuiframe 都显示  因为如果是在拨号界面  那么退出拨号界面的时候回到talkuiframe 还要显示出来
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateRecordIcon(iState);
    }

    return 0;
}

//录制警告
// waparm： 警告值（单位秒）
LRESULT CTalkUIFrameManager::OnUSBRecordMsgWarning(int iTime)
{
    //无论是否在 talkuiframe 都显示  因为如果是在拨号界面  那么退出拨号界面的时候回到talkuiframe 还要显示出来
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->OnUSBRecordMsgWarning(iTime);
    }

    return 0;
}

//容量满 提醒
LRESULT CTalkUIFrameManager::OnUSBRecordMsgNotenough()
{
    //无论是否在 talkuiframe 都显示  因为如果是在拨号界面  那么退出拨号界面的时候回到talkuiframe 还要显示出来
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->OnUSBRecordMsgNotenough();
    }

    return 0;
}

//录制截图 提醒
//waparm：0=开始录制,1=停止录制,2=截图  lparam：成功与否(0/1)
LRESULT CTalkUIFrameManager::OnRecordStorageRecordResult(TALK_RECORD_STATUS eStatus)
{
    // http://10.3.5.199/Bug.php?BugID=70579
    //无论是否在 talkuiframe 都显示  因为如果是在拨号界面  那么退出拨号界面的时候回到talkuiframe 还要显示出来
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateRecordStatus(eStatus);
    }

    return 0;
}
#endif

//改变通道图标
void CTalkUIFrameManager::UpdateChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateChannelIcon(eTalkUIChannelIcon);
    }
}

//刷新title
void CTalkUIFrameManager::UpdateTalkUITitle(yl::string strTitle)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateTalkUITitle(strTitle);
    }
}

//刷新pagenum显示
//iCurrentIndex是实际的序号 开始序号会在iCurrentIndex的基础上加1
void CTalkUIFrameManager::UpdateTalkUIPageNum(int iCurrentIndex, int iSessionCount)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateTalkUIPageNum(iCurrentIndex, iSessionCount);
    }
}

//刷新通话时间显示
void CTalkUIFrameManager::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateTalkUITime(iHour, iMin, iSecond);
    }
}

//刷新softkey
void CTalkUIFrameManager::UpdateTalkUISoftkey()
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateTalkUISoftkey();
    }
}

//刷新蓝牙图标
void CTalkUIFrameManager::UpdateBluetoothIcon(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon)
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateBluetoothIcon(eTalkUIBlueToothIcon);
    }
}

//刷新通话图标
//暂时是只有来电界面使用到
//一般在实现这个接口的时候 最好是加上刷新蓝牙图标， 因为一般的中性版本普通图标和蓝牙图标是在一块显示的   这边容易把蓝牙图标也清除了
void CTalkUIFrameManager::UpdateTalkUIIcons()
{
    //拨号界面是不需要更新图标的
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->UpdateTalkUIIcons();
    }
}

#if IF_TALKUI_CONTACT_PHOTO
//获取会议联系人头像
yl::string CTalkUIFrameManager::GetPicContactsConference()
{
    return PIC_CONTACTS_CONFERENCE;
}

#if IF_TALKUI_BLUETOOTH_BT_ENABLE
//获取蓝牙联系人头像
yl::string CTalkUIFrameManager::GetPicBluetoothContact()
{
    return PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT;
}
#endif
#endif


//创建TalkUIFrame实例
void CTalkUIFrameManager::CreateTalkUIFrame()
{
    //不为空 直接返回
    if (NULL != m_pTalkUIFrame)
    {
        return ;
    }

    //获取通话窗口框架对象指针，由于其应为唯一，因此，不需传入ObjectName进行标识
    //TalkUIFrame为顶层窗口
    CBaseDialog * pDialog = UIManager_CreatePrivateDlg(DLG_TalkUIFrame);
    if (pDialog != NULL)
    {
        //将窗口指针赋值
        m_pTalkUIFrame = (CDlgTalkUIFrame *)pDialog;
    }
}

//创建通话UI实例, 这边的strName是PROCESSORNAME
CUILogicBaseTalkUI * CTalkUIFrameManager::CreateTalkUI(const yl::string & strName)
{
    return _TalkUIFactory.CreateTalkUI(strName);
}

//释放通话UI实例指针
void CTalkUIFrameManager::ReleaseTalkUI(const yl::string & strProcessorName,
                                        CUILogicBaseTalkUI * pTalkUI)
{
    _TalkUIFactory.ReleaseTalkUI(strProcessorName, pTalkUI);
}

void CTalkUIFrameManager::SetCurrentFrameType(FRAME_TYPE frameType)
{
    m_frameType = frameType;
}

FRAME_TYPE CTalkUIFrameManager::GetCurrentFrameType() const
{
    return m_frameType;
}

void CTalkUIFrameManager::UpdateSecurityClassification(bool bShowSecurity)
{
#if IF_TALKUI_CONTACTINFO_WITHOUT_TARGET
    //彩屏需要在title进行security的提示
#else
    //黑白屏直接合并在contactinfo中提示，此处不需要做处理
#endif
}

void CTalkUIFrameManager::UpdateVolumebar(bool bShow)
{
    Volumebar_SetVolumeBarVisible(bShow);
}


bool CTalkUIFrameManager::ShowErrorNote(CCallInfo * pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return false;
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CALL_RECORD_ACTION_RESULT eState = pCallInfo->m_eRecordResult;
    ShowErrorNote(eState);
    if (eState == CALL_RECORD_START_FAILED
            || eState == CALL_RECORD_STOP_FAILED
            || eState == CALL_RECORD_PAUSE_FAILED
            || eState == CALL_RECORD_RESUME_FAILED
       )
    {
        pCallInfo->m_eRecordResult = CALL_RECORD_NONE;
    }
#endif

    return true;
}

bool CTalkUIFrameManager::ShowErrorNote(CALL_RECORD_ACTION_RESULT eFailResult)
{
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (CALL_RECORD_START_FAILED == eFailResult)
    {
        _TalkUIFrameManager.UpdateErrorInfo(_UILOGIC_LANG_TRAN(TRID_START_RECORD_FAILED));
    }
    else if (CALL_RECORD_STOP_FAILED == eFailResult)
    {
        _TalkUIFrameManager.UpdateErrorInfo(_UILOGIC_LANG_TRAN(TRID_STOP_RECORD_FAILED));
    }
    else if (CALL_RECORD_PAUSE_FAILED == eFailResult)
    {
        _TalkUIFrameManager.UpdateErrorInfo(_UILOGIC_LANG_TRAN(TRID_PAUSE_RECORD_FAILED));
    }
    else if (CALL_RECORD_RESUME_FAILED == eFailResult)
    {
        _TalkUIFrameManager.UpdateErrorInfo(_UILOGIC_LANG_TRAN(TRID_RESUME_RECORD_FAILED));
    }
#endif

    return true;
}

}       //namespace talkui
