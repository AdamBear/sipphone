#include "talkuiframemanager.h"
#include "modtalkui.h"

#include "dlgbasetalkui.h"
#include "dlgtalkuiframe.h"
#include "dlgdialingui.h"
#include "talkuidelegate.h"

#if IF_USE_CALLMANAGER
#include "callmanager/modcallmanager.h"
#endif

#include "mainwnd/mainwnd.h"
#include "talkuimanager.h"
#include "baseui/t4xpicpath.h"

#include "voice/include/modvoice.h"
#include "mainwnd/autovolumebar.h"
#include "mainwnd/modmainwnd.h"

CTalkUIFrameManager & CTalkUIFrameManager::GetInstance()
{
    static CTalkUIFrameManager s_cTalkUIFrameManager;

    return s_cTalkUIFrameManager;
}

CTalkUIFrameManager::CTalkUIFrameManager(void)
{
    m_pDlgTalkUIFrame   = NULL;
    m_frameType = FRAME_TYPE_NONE;

    //通道图标相关
    //初始化是否显示音量条标识
    m_bIsShowVolume = false;
    //初始化当前通道标识
    m_eCurrentChannel = CHANNEL_NULL;

}

CTalkUIFrameManager::~CTalkUIFrameManager(void)
{

}

// 初始化
bool CTalkUIFrameManager::Init()
{
    m_pDlgTalkUIFrame   = NULL;
    m_frameType = FRAME_TYPE_NONE;

    return false;
}

// 释放数据,将窗口清空回复到初始化状态
void CTalkUIFrameManager::Uninit()
{
    //清除TalkUI
    ClearTalkUI();
}

// 设置子窗口指针
void CTalkUIFrameManager::SetTalkUI(CDlgBaseTalkUI * pBaseTalkUI, void * pData/* = NULL */)
{
    //清除 talkui 对话框架
    if (NULL == pBaseTalkUI)
    {
        if (_MainWND.GetVolumeBar() != NULL)
        {
            disconnect(_MainWND.GetVolumeBar(), SIGNAL(visibleChanged(bool)), this,
                       SLOT(OnVolumeBarVisibleChanged(bool)));
            disconnect(_MainWND.GetVolumeBar(), SIGNAL(valueChanged(int)), this, SLOT(OnVolumeChanged(int)));
        }

        if (GetCurrentFrameType() == FRAME_TYPE_TALKUIFRAME)
        {
            if (NULL != m_pDlgTalkUIFrame)
            {
                m_pDlgTalkUIFrame->SetTalkUI(NULL);
            }
        }
#if IF_USE_CALLMANAGER
        else if (GetCurrentFrameType() == FRAME_TYPE_CALLMANAGER)
        {
            //这边不用做任何动作就行  因为 CDlgDialingUI是缓存在 CDlgCallManager中的
            //这边还是把 CallManager 退出来
            //先把对话框指针设置为空  //这边只需要设置为空就可以了  不能退出因为 可能还有dir 和 calllog 数据需要暂时保存的
            callmanager_SetPageDlgByName(DLG_Dial, NULL);
        }
#endif

        //把活动的框架设置为空
        SetCurrentFrameType(FRAME_TYPE_NONE);

        return ;
    }

    //设置通话子窗口
    //如果是与当前的子窗口相同 那么刷新窗口直接返回
    if (GetTalkUI() == pBaseTalkUI)
    {
        //如果是属于TalkUIFrame 先设置数据  在更新窗口
        if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
        {
            if (NULL != m_pDlgTalkUIFrame)
            {
                m_pDlgTalkUIFrame->SetData(pData);

                if (m_pDlgTalkUIFrame == UIManager_GetTopWindow())
                {
                    //设置数据后要更新窗口 主要是为了刷softkey等
                    UIManager_UpdateWnd(m_pDlgTalkUIFrame);
                }
            }
        }
#if IF_USE_CALLMANAGER
        else if (FRAME_TYPE_CALLMANAGER == GetCurrentFrameType())
        {
            _MainWND.SetVolumeBarVisible(false);

            //子窗口 没有变化 如果当前是在拨号界面  那么也要去除所有弹出框
            if (_TalkUIDelegate.GetMessageBoxCount() > 0)
            {
                _TalkUIDelegate.RemoveBaseTalkuiMessageBox(pBaseTalkUI);
                //_TalkUIDelegate.RemoveAllMessageBox();
            }

            //先进行数据设置
            DataLogic2UI * v_pLogicData = (DataLogic2UI *)pData;

            if (NULL != v_pLogicData)
            {
                //更换新设置数据方式
                //pBaseTalkUI->SetTalkData(v_pLogicData->pData, v_pLogicData->eUpateReason);
                pBaseTalkUI->SetLogicData(*v_pLogicData);
            }

            //设置窗口  这边直接显示就好了  因为 子窗口都是相同的
            //  callmanager_SetPageDlgByName(DLG_Dial, pBaseTalkUI);
            //显示窗口  内部会自己进行刷新

            if (callmanager_GetCurrentPageName() == DLG_Dial
                    // kszIncomingCallPriorityWhenDial==0时不打断操作
                    || configParser_GetConfigInt(kszIncomingCallPriorityWhenDial) != 0)
            {
                callmanager_EnterPage(DLG_Dial);

                //要刷新 窗口
                //  CallManager_EnterPage(CALLMGR_DIALING);

                //刷新通道
                _TalkUIManager.UpdateChannelIcon();
            }

        }
#endif

        return ;
    }

    //连接volumeBar的信号槽
    if (_MainWND.GetVolumeBar() != NULL)
    {
        disconnect(_MainWND.GetVolumeBar(), SIGNAL(visibleChanged(bool)), this,
                   SLOT(OnVolumeBarVisibleChanged(bool)));
        disconnect(_MainWND.GetVolumeBar(), SIGNAL(valueChanged(int)), this, SLOT(OnVolumeChanged(int)));

        connect(_MainWND.GetVolumeBar(), SIGNAL(visibleChanged(bool)), this,
                SLOT(OnVolumeBarVisibleChanged(bool)));
        connect(_MainWND.GetVolumeBar(), SIGNAL(valueChanged(int)), this, SLOT(OnVolumeChanged(int)));
    }

#if IF_USE_CALLMANAGER

    //子窗口 有变化 那么 要去除所有弹出框
    if (_TalkUIDelegate.GetMessageBoxCount() > 0)
    {
        _TalkUIDelegate.RemoveAllMessageBox();
    }

    //如果是设置 拨号界面
    if (DLG_Dial == pBaseTalkUI->GetTalkUIName())
    {
        //先把CDlgTalkUIFrame的设置为空
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->SetTalkUI(NULL);
            //这边不能把 TalkUIFrame 给出栈 了， 因为 可能主动退出拨号界面的那么这个时候就不会刷 TalkUIFrame了
        }

        //音量条不显示
        _MainWND.SetVolumeBarVisible(false);

        //先进行数据设置 然后在刷新窗口
        //设置数据
        DataLogic2UI * v_pLogicData = (DataLogic2UI *)pData;

        if (NULL != v_pLogicData)
        {
            //更换新设置方式
            //pBaseTalkUI->SetTalkData(v_pLogicData->pData, v_pLogicData->eUpateReason);
            pBaseTalkUI->SetLogicData(*v_pLogicData);
        }

        //当前活动通话框架设置为CDlgCallManager
        SetCurrentFrameType(FRAME_TYPE_CALLMANAGER);
        //这个函数内部会做 显示窗口的操作  这边不需要再强制刷新了 因为强制刷新 主要是为了更新softkey等
        //设置窗口指针
        callmanager_SetPageDlgByName(DLG_Dial, pBaseTalkUI);
        //显示拨号界面
        callmanager_EnterPage(DLG_Dial);

        //CallManager_EnterPage(CALLMGR_DIALING);

        //刷新通道
        _TalkUIManager.UpdateChannelIcon();

        return ;
    }

#endif

    //非设置拨号界面

    //判断TalkUIFrame是否为空
    if (NULL == m_pDlgTalkUIFrame)
    {
        CreateTalkUIFrame();

        //第一次进入通话页面
        UIManager_ReturnToIdle();

        //当创建TalkUIFrame的时候难免需要调用 UIMananger_UpdateWnd 两次  因为 第二次 在设置talkui的时候 还要冲刷 softkey
    }

    //当前活动框架类型设置为 TalkUIFrame
    SetCurrentFrameType(FRAME_TYPE_TALKUIFRAME);

    //退出拨号界面显示  设置窗口指针未NULL
    callmanager_SetPageDlgByName(DLG_Dial, NULL);

    //显示其他通话对话框  要退出拨号界面显示
    callmanager_PutPage(DLG_Dial);

//  CallManager_ExitModule();

    if (NULL != m_pDlgTalkUIFrame)
    {
        //这边不用清除CallManager的子窗口了 只需要给TalkUIFrame设置就行
        m_pDlgTalkUIFrame->SetTalkUI(pBaseTalkUI);

        //先设置数据
        m_pDlgTalkUIFrame->SetData(pData);

        //如果获取顶层窗口相同 只要刷新就可以了
        if (m_pDlgTalkUIFrame == UIManager_GetTopWindow())
        {
            //主要是为了刷新  softkey 等
            UIManager_UpdateWnd(m_pDlgTalkUIFrame);
        }
    }

}

//把焦点的session数据设置无效
void CTalkUIFrameManager::SetFocusSessionDataInvalid()
{

}

// 获取子窗口指针
CDlgBaseTalkUI * CTalkUIFrameManager::GetTalkUI() const
{
    //如果当前状态为 FRAME_TYPE_NONE 说明没有窗口
    if (GetCurrentFrameType() == FRAME_TYPE_TALKUIFRAME)
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            return m_pDlgTalkUIFrame->GetTalkUI();
        }

        return NULL;
    }
#if IF_USE_CALLMANAGER
    else if (GetCurrentFrameType() == FRAME_TYPE_CALLMANAGER)
    {
        //获取当前的拨号对话框
        CCallManagerBaseDlg * pBaseDlg = callmanager_GetPageByUIName(DLG_Dial);

        if (NULL != pBaseDlg
                && pBaseDlg->inherits(DLG_Dial))
        {
            return static_cast<CDlgBaseTalkUI *>(pBaseDlg);
        }

    }
#endif

    return NULL;
}

//输入输入框内容，并返回当前输入内容
bool CTalkUIFrameManager::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
#if IF_USE_CALLMANAGER
    CCallManagerBaseDlg * pBaseDlg = callmanager_GetCurrentPage();
    if (NULL != pBaseDlg
            && pBaseDlg->inherits(DLG_Dial))
    {
        CDlgDialingUI * pDialingUI = (CDlgDialingUI *)pBaseDlg;
        return pDialingUI->GetInputContent(strNum, strDisplayname);
    }
#else
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            CDlgBaseTalkUI * pDlgBaseTalkUI = m_pDlgTalkUIFrame->GetTalkUI();

            if (NULL != pDlgBaseTalkUI)
            {
                if (pDlgBaseTalkUI->inherits(DLG_Dial))
                {
                    CDlgDialingUI * pDialingUI = (CDlgDialingUI *)pDlgBaseTalkUI;

                    return pDialingUI->GetInputContent(strNum, strDisplayname);

                }
            }

        }
    }

#endif

    return false;
}

int CTalkUIFrameManager::GetInputCallID()
{
#if IF_USE_CALLMANAGER
    CCallManagerBaseDlg * pBaseDlg = callmanager_GetCurrentPage();
    if (NULL != pBaseDlg
            && pBaseDlg->inherits(DLG_Dial))
    {
        CDlgDialingUI * pDialingUI = (CDlgDialingUI *)pBaseDlg;
        return pDialingUI->GetShttleCallID();
    }
#else
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            CDlgBaseTalkUI * pDlgBaseTalkUI = m_pDlgTalkUIFrame->GetTalkUI();

            if (NULL != pDlgBaseTalkUI)
            {
                if (pDlgBaseTalkUI->inherits(DLG_Dial))
                {
                    CDlgDialingUI * pDialingUI = (CDlgDialingUI *)pDlgBaseTalkUI;

                    return pDialingUI->GetShttleCallID();

                }
            }

        }
    }

#endif

    return -1;
}

//更新 其他图标  静音、加密、HD、蓝牙、录音
void CTalkUIFrameManager::AddTitleIcon(const yl::string & strIconPath, bool bAdd)
{
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->OnAddTitleIcon(strIconPath, bAdd);
        }
    }
}

//这个在t46 中使用   t48 直接在talkuimanager 就刷新了

//刷新通道图标
void CTalkUIFrameManager::UpdateChannelIcon()
{
    //通道图标字符串
    yl::string strChannelIcon = "";

    //获取通道类型
    CHANNEL_MODE eChannelMode = voice_GetCurrentChannel();
    if (m_eCurrentChannel != eChannelMode)
    {
        if (m_eCurrentChannel == CHANNEL_HANDFREE
                || m_eCurrentChannel == CHANNEL_GROUP
                || m_eCurrentChannel == CHANNEL_HANDSET
                || m_eCurrentChannel == CHANNEL_HEADSET)
        {
            m_bIsShowVolume = true;
        }

        m_eCurrentChannel = eChannelMode;
    }

    switch (eChannelMode)
    {
    case CHANNEL_HANDFREE:
    case CHANNEL_GROUP:
    {
        //免提
        strChannelIcon = PIC_ICON_HANDFREE;
        //  m_pTopBar->SetChannelIcon(PIC_ICON_HANDFREE);
    }
    break;
    case CHANNEL_HANDSET:
    {
        //手柄
        strChannelIcon = PIC_ICON_HANDSET;
        //  m_pTopBar->SetChannelIcon(PIC_ICON_HANDSET);
    }
    break;
    case CHANNEL_HEADSET:
    case CHANNEL_2HEADSET:
    {
        //耳麦
        strChannelIcon = PIC_ICON_HEADSET;
        //  m_pTopBar->SetChannelIcon(PIC_ICON_HEADSET);
    }
    break;
    case CHANNEL_RING:
    {
        if (voice_IsRingHeadset())
        {
            // 耳麦响铃
            strChannelIcon = PIC_ICON_HEADSET;
            //  m_pTopBar->SetChannelIcon(PIC_ICON_HEADSET);
        }
        else
        {
            // 免提响铃
            strChannelIcon = PIC_ICON_HANDFREE;
            //  m_pTopBar->SetChannelIcon(PIC_ICON_HANDFREE);
        }
    }
    break;
    default:
    {
        //未知
        strChannelIcon = "";
        //  m_pTopBar->SetChannelIcon("");
    }
    break;
    }

    _TalkUIDelegate.SetChannelIcon(strChannelIcon);

    ////m_pDlgTalkUIFrame存在 不一定代表 当前就是显示 talkuiframe
    ////需要根据  GetCurrentFrameType() 来判断
    //if(FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    //{
    //  if(NULL != m_pDlgTalkUIFrame)
    //  {
    //      m_pDlgTalkUIFrame->UpdateChannelIcon();
    //  }
    //}
}


void CTalkUIFrameManager::UpdateRecordIcon(ICON_TYPE eIconType)
{
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->SetRecordTip(eIconType);
        }
    }

}

void CTalkUIFrameManager::RefreshBlueToothIcon()
{
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->RefreshBlueToothIcon();
        }
    }
}


void CTalkUIFrameManager::RefreshDNDIcon()
{
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->RefreshDNDIcon();
        }
    }
}

void CTalkUIFrameManager::UpdateQueueStatus(const yl::string & strQueueStatus)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->UpdateQueueStatus(strQueueStatus);
        }
    }

#endif

}

//获取是否显示音量条状态
bool CTalkUIFrameManager::GetIsShowVolume()
{
    return m_bIsShowVolume;
}

//设置是否显示音量条状态
void CTalkUIFrameManager::SetShowVolumeStatus(bool bIsShowVolume)
{
    m_bIsShowVolume = bIsShowVolume;
}

void CTalkUIFrameManager::CreateTalkUIFrame()
{
    //不为空 直接返回
    if (NULL != m_pDlgTalkUIFrame)
    {
        return ;
    }

    //获取通话窗口框架对象指针，由于其应为唯一，因此，不需传入ObjectName进行标识
    //TalkUIFrame为顶层窗口
    CBaseDialog * pDialog = UIManager_CreatePrivateDlg(DLG_TalkUIFrame);
    if (pDialog != NULL)
    {
        //判断该获得的窗口指针是否为通话窗口框架对象指针
        if (pDialog->inherits(DLG_TalkUIFrame))
        {
            //将窗口指针赋值
            m_pDlgTalkUIFrame = static_cast<CDlgTalkUIFrame *>(pDialog);

        }
        else    //主要是为了容错
        {
            //非通话窗口框架对象指针
            //将生成的对象指针释放返回
            UIManager_PutPrivateDlg(pDialog);
            m_pDlgTalkUIFrame = NULL;
        }
    }
}

//清除子窗口
void CTalkUIFrameManager::ClearTalkUI()
{
    //先将子窗口设置为NULL  该函数内会对各状态进行设置
    SetTalkUI(NULL);

    if (NULL != m_pDlgTalkUIFrame)
    {
        UIManager_PutPrivateDlg(m_pDlgTalkUIFrame);
        m_pDlgTalkUIFrame = NULL;
    }

#if IF_USE_CALLMANAGER
    //退出拨号界面显示
    callmanager_PutPage(DLG_Dial);
#endif

    //  SetFrameType(FRAME_TYPE_NONE);

    m_bIsShowVolume = false;
}

void CTalkUIFrameManager::SetCurrentFrameType(FRAME_TYPE frameType)
{
    m_frameType = frameType;
}

FRAME_TYPE CTalkUIFrameManager::GetCurrentFrameType() const
{
    return m_frameType;
}

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
void CTalkUIFrameManager::USBRecordStateChange()
{
    if (FRAME_TYPE_TALKUIFRAME == GetCurrentFrameType())
    {
        if (NULL != m_pDlgTalkUIFrame)
        {
            m_pDlgTalkUIFrame->USBRecordStateChange();
            if (m_pDlgTalkUIFrame == UIManager_GetTopWindow())
            {
                UIManager_UpdateWnd(m_pDlgTalkUIFrame);
            }
        }
    }
}
#endif

bool CTalkUIFrameManager::OnDoorLineMessage(msgObject & refMessage)
{
#ifdef IF_FEATURE_ENTRANCE_GUARD
    if (NULL != m_pDlgTalkUIFrame)
    {
        return m_pDlgTalkUIFrame->OnDoorLineMessage(refMessage);
    }
#endif

    return false;
}

void CTalkUIFrameManager::OnVolumeChanged(int nValue)
{
    CDlgBaseTalkUI * pTalkUI = GetTalkUI();
    if (talkui_IsInTalk() && pTalkUI != NULL)
    {
        //发送音量改变的消息给Logic层
        pTalkUI->SendUIRequestAction(TALK_ACTION_VOLUME_CHANGE, nValue);

        pTalkUI->OnTalkVolumnValueChange();
    }
}

void CTalkUIFrameManager::OnVolumeBarVisibleChanged(bool bVisible)
{
    CDlgBaseTalkUI * pTalkUI = GetTalkUI();
    CAutoVolumeBar * pVolumeBar = const_cast<CAutoVolumeBar *>(_MainWND.GetVolumeBar());

    if (talkui_IsInTalk() && NULL != pTalkUI && NULL != pVolumeBar && NULL != m_pDlgTalkUIFrame)
    {
        if (bVisible)
        {
            // 显示音量条
            //!!!!!
            //todo:这个是因为在_MainWND.SetVolumeBarVisible里VolumeBar还会再发出visibleChanged
            //导致OnVolumeBarVisibleChanged槽被循环调用。
            pVolumeBar->blockSignals(true);
            _MainWND.SetVolumeBarVisible(true);
            pVolumeBar->blockSignals(false);

            OnVolumeChanged(pVolumeBar->GetValue());

            //发送音量条的消息给Logic层
            pTalkUI->SendUIRequestAction(TALK_ACTION_VOLUMEBAR_SHOW);

            m_pDlgTalkUIFrame->SetDTMFLineVisible(false);
            m_pDlgTalkUIFrame->VisibleCCStatusLabel(false);
            m_pDlgTalkUIFrame->ProcessTalkTipType(TALK_TIP_VOLUMEBAR);
        }
        else
        {
            m_pDlgTalkUIFrame->SetTipRegionVisiblePolicy();
        }
    }
}

