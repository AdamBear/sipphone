///////////////talkui/////////////////////
#include "dlgtalkuiframe.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkui_define.h"
#include "talkuicommonfunction.h"
#include "talkuixmlitemname.h"
#include "talkuiframemanager.h"
#include "conflictmanager.h"

//////////////talkuilogic//////////////////
//#include "talkuilogic/talkuilogiccommon.h"
#include "talk/talklogic/include/modtalklogic.h"

//////////////uimanager//////////////////
#include "uimanager/moduimanager.h"
#include "uimanager/modvolumebar.h"

///////////////xwindow////////////////
#include "xwindow/xWindowManager.h"
#include "xwindow/xSoftkeyBar.h"

#include "settinguilogic/include/modsetuilogic.h"
#include "baseui/include/baseuicommon.h"
#include "imagecache/include/modimagecache.h"

namespace talkui
{

#define TALK_UI_RECT_NORMAL_LEFT        14
#define TALK_UI_RECT_NORMAL_WIDTH   292

#define TALK_UI_RECT_NARROW_LEFT        43
#define TALK_UI_RECT_NARROW_WIDTH   234

#define TALK_UI_RECT_TOP                            3
#define TALK_UI_RECT_HEIGHT                     169


//取得实例
CBaseDialog * CDlgTalkUIFrame::CreateInstance()
{
    CDlgTalkUIFrame * pTalkUIFrame = new CDlgTalkUIFrame();

    if (NULL != pTalkUIFrame)
    {
        pTalkUIFrame->loadTalkContent();
    }

    return pTalkUIFrame;
}

CDlgTalkUIFrame::CDlgTalkUIFrame()
    : CBaseDialog(this, eWT_Talk)
    , m_pTalkUI(NULL)
{

}

void CDlgTalkUIFrame::loadTalkContent()
{
    loadContent(TALKUI_DLGTALKUIFRAME_XML_PATH);

    m_TitleBar.LoadWidget(this);

    m_MenuTitle.LoadWidget(this);

    m_PublicWidget.LoadWidget(this);
}

CDlgTalkUIFrame::~CDlgTalkUIFrame()
{

}

// 设置子窗口指针
void CDlgTalkUIFrame::SetTalkUI(CDlgBaseTalkUI * pTalkUI)
{
    //当前的焦点窗口  窗口不一样，才进行切换
    if (m_pTalkUI == pTalkUI)
    {
        return;
    }

    if (m_pTalkUI)
    {
        //talkuiframe是当前焦点窗口 那么才需要调用该函数
        if (UIManager_GetTopWindow() == this)
        {
            // 画新的窗口前,旧窗口处理焦点失去事件
            m_pTalkUI->OnFocusChanged(false);
        }

        // 隐藏旧界面
        m_pTalkUI->hide(true);

        m_pTalkUI->Uninit();

        m_PublicWidget.popTalkUIChild(m_pTalkUI);

        //处理完后  把当前的通话子窗口置空
        m_pTalkUI = NULL;
    }

    //窗口指针赋值
    m_pTalkUI = pTalkUI;

    //窗口改变时重置softkey页码
    resetSoftkeyPageIndex();

    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->Init();

#if IF_TALKUI_BG_PICTURE
        m_PublicWidget.setTalkUIBackGround(m_pTalkUI);
#endif

        m_PublicWidget.addTalkUIChild(m_pTalkUI);

        m_pTalkUI->show(xView::SHOW_CMD_BRING_TO_TOP);        //显示子窗口

        //talkuiframe是当前焦点窗口 那么才需要调用该函数
        if (UIManager_GetTopWindow() == this)
        {
            // 画新的窗口前,旧窗口处理焦点失去事件
            m_pTalkUI->OnFocusChanged(true);
        }

        //小窗口获取焦点
        //修正bug http://10.2.1.199/Bug.php?BugID=31728 使焦点在通过窗体上
        //这个bug 因为在TalkUIFrame中已经不判断是否子窗口了  所以这个设置焦点的可以去除
        //m_pTalkUI->setFocus();
    }
}

// 获取子窗口指针
CDlgBaseTalkUI * CDlgTalkUIFrame::GetTalkUI()
{
    return m_pTalkUI;
}

//给TalkUIFrame设置数据
void CDlgTalkUIFrame::SetData(CUILogicToTalkUIData & dataUILogicToTalkUI)
{
    //暂时不做处理

}

//调用完设置数据 SetData 后会调用该函数
//刷新窗口
void CDlgTalkUIFrame::UpdateTalkUI(bool bNewTalkUI)
{
    if (bNewTalkUI)
    {
        //子窗口改变 那么dtmf信息要清除
        m_PublicWidget.ResetDtmf();

        // 通话窗口变化的话要隐藏音量条!!!!
        //音量条隐藏
        Volumebar_SetVolumeBarVisible(false);

        m_PublicWidget.RefreshWidget();
    }
}

void CDlgTalkUIFrame::UpdateTalkUITitle(yl::string strTitle)
{
    if (NULL != m_pTalkUI)
    {
        switch (m_pTalkUI->GetTitleType())
        {
        case TALKUI_TITLE_TYPE_NO_TITLE:
            {
                m_TitleBar.Hide();
                m_MenuTitle.Hide();
            }
            break;
        case TALKUI_TITLE_TYPE_MENU:
            {
                m_TitleBar.Hide();

                m_MenuTitle.Init();
                m_MenuTitle.Show();
                m_MenuTitle.SetTitle(strTitle);
            }
            break;
        case TALKUI_TITLE_TYPE_NORMAL:
        default:
            {
                m_MenuTitle.Hide();

                m_TitleBar.Init();
                m_TitleBar.Show();

                if (m_bStationName)
                {
                    m_TitleBar.SetTitle(m_strStationName);
                    m_TitleBar.SetTitleScroll(m_bStationNameScroll);
                    m_TitleBar.SetChannelIcon("", false);
                }
                else
                {
                    m_TitleBar.SetTitle(strTitle);
                    m_TitleBar.SetTitleScroll(true);
                }

#if IF_TALKUI_USB_ENABLE
                m_TitleBar.SyncRecordTime();
#endif
            }
            break;
        }
    }
}

//改变通道图标
void CDlgTalkUIFrame::UpdateChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon)
{
    if (NULL != m_pTalkUI)
    {
        switch (m_pTalkUI->GetTitleType())
        {
        case TALKUI_TITLE_TYPE_NO_TITLE:
            {
                //nothing
            }
            break;
        case TALKUI_TITLE_TYPE_MENU:
            {
                //nothing
            }
            break;
        case TALKUI_TITLE_TYPE_NORMAL:
        default:
            {
                if (TALKUI_CHANNEL_ICON_UNKNOWN == eTalkUIChannelIcon
                        || m_bStationName)
                {
#if IF_TITLE_ICON_FIXED
                    ResetFixedChannelIcon();
#else
                    m_TitleBar.SetChannelIcon("", false);
#endif
                }
                else
                {
#if IF_TITLE_ICON_FIXED
                    ShowFixedChannelIcon(eTalkUIChannelIcon);
#else
                    m_TitleBar.SetChannelIcon(GetChannelIconPicByChannelIconType(eTalkUIChannelIcon));
#endif
                }
            }
            break;
        }
    }
}

//刷新pagenum显示
//iCurrentIndex是实际的序号  开始序号会在iCurrentIndex的基础上加1
void CDlgTalkUIFrame::UpdateTalkUIPageNum(int iCurrentIndex, int iSessionCount)
{
    if (NULL == m_pTalkUI)
    {
        return;
    }

    bool bShowScrollBar = true;

    switch (m_pTalkUI->GetTitleType())
    {
    case TALKUI_TITLE_TYPE_NO_TITLE:
        {
            //nothing
        }
        break;
    case TALKUI_TITLE_TYPE_MENU:
        {
            bShowScrollBar = false;
        }
        break;
    case TALKUI_TITLE_TYPE_NORMAL:
    default:
        {
            //需要显示  pagenum
#if IF_TALKUI_ONE_PAGE_SHOW_NOTE
            if (iSessionCount > 0)
#else
            if (iSessionCount > 1)
#endif
            {
                m_TitleBar.SetPageNum(iCurrentIndex + 1, iSessionCount);
            }
            else    //隐藏pagenum
            {
                m_TitleBar.HidePageNum();
            }
        }
        break;
    }

    //显示滚动条
    //需要显示滚动条才显示
    if (bShowScrollBar
            && (iSessionCount > 1)
            && (m_pTalkUI->IsShowSessionScrollBar())
       )
    {
        m_PublicWidget.SetScrollBar(1, iSessionCount, iCurrentIndex);
        m_PublicWidget.ShowScrollBar(true);
    }
    else
    {
        m_PublicWidget.ShowScrollBar(false);
    }
}

//刷新通话时间显示
void CDlgTalkUIFrame::UpdateTalkUITime(uint iHour, uint iMin, uint iSecond)
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->UpdateTalkUITime(iHour, iMin, iSecond);
    }
}

//刷新softkey
void CDlgTalkUIFrame::UpdateTalkUISoftkey()
{
    //当前是在通话 界面 那么才更新softkey
    if (this == UIManager_GetTopWindow())
    {
        RefreshSoftkey();
    }
}

//刷新蓝牙图标
void CDlgTalkUIFrame::UpdateBluetoothIcon(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon)
{
    //先删除蓝牙图标

    //先删除蓝牙图标
    m_TitleBar.DeleteIcon(PIC_TALK_ICON_BLUETOOTH_UNCONNECTED);
    m_TitleBar.DeleteIcon(PIC_TALK_ICON_BLUETOOTH_MOBILE);
    m_TitleBar.DeleteIcon(PIC_TALK_ICON_BLUETOOTH_HEADSET);

    yl::string strBluetoothIcon = GetBlueToothIconPathByBlueToothIconType(eTalkUIBlueToothIcon);

    if (!strBluetoothIcon.empty())
    {
        m_TitleBar.AddIcon(strBluetoothIcon);
    }
}

#if IF_TALKUI_USB_ENABLE
void CDlgTalkUIFrame::UpdateRecordStatus(TALK_RECORD_STATUS eStatus)
{
    switch (eStatus)
    {
    case TALK_RECORD_STATUS_START:
        {
            m_TitleBar.StartRecord();
        }
        break;
    case TALK_RECORD_STATUS_PAUSE:
        {
            m_TitleBar.PauseRecord();
        }
        break;
    case TALK_RECORD_STATUS_STOP:
        {
            m_TitleBar.StopRecord();
        }
        break;
    case TALK_RECORD_STATUS_NONE:
    default:
        {
            //
        }
        break;
    }
    //如果是停止，则需要弹框提示
    if (TALK_RECORD_STATUS_STOP == eStatus)
    {
        _TalkUIFrameManager.UpdateMsgboxNoteNotify(_UILOGIC_LANG_TRAN(TRID_RECORD_FILE_SAVED));
    }
}

void CDlgTalkUIFrame::UpdateRecordIcon(int iState)
{
    //nothing
}

void CDlgTalkUIFrame::OnUSBRecordMsgWarning(int iTime)
{
    //提示上来的是 以秒为单位的 所以 要除60
    int nTime = iTime / 60;
    yl::string strWarning = commonAPI_FormatString("%d", nTime);
    strWarning += " ";
    strWarning += _UILOGIC_LANG_TRAN(TRID_MINUTES_LEFT_FOR_CALL_RECORDING);

    _TalkUIFrameManager.UpdateMsgboxNoteNotify(strWarning, -1);
}

void CDlgTalkUIFrame::OnUSBRecordMsgNotenough()
{
    _TalkUIFrameManager.UpdateMsgboxNoteNotify(_UILOGIC_LANG_TRAN(TRID_SPACE_FULL_SAVE_FILE));
}
#endif

//更新通话图标
void CDlgTalkUIFrame::UpdateTalkUIIcons()
{
    if (NULL != m_pTalkUI)
    {
        //先清除所有图标
        m_TitleBar.ClearAllIcons();

#if IF_TITLE_ICON_FIXED
        ResetFixedOtherIcon();
#endif

        YLList<TALKUI_ICON> listIcons;
        m_pTalkUI->GetIconList(listIcons);

        //设置图标
        yl::string strIcon = "";
        YLList<TALKUI_ICON>::iterator iteorList = listIcons.begin();
        for (; listIcons.end() != iteorList; iteorList++)
        {
            TALKUI_ICON iconType = *iteorList;
#if IF_HD_ICON_DEPART
            if (TALKUI_ICON_HD == iconType
                    || TALKUI_ICON_AMR == iconType
                    || TALKUI_ICON_OPUS == iconType)
            {
                continue;
            }
#endif

#if IF_TITLE_ICON_FIXED
            if (ShowFixedOtherIcon(iconType))
            {
                continue;
            }
#endif

            strIcon = GetIconPathByIconType(iconType);
            m_TitleBar.AddIcon(strIcon);
        }
    }
}

DSSKEY_DISPLAY_MODE CDlgTalkUIFrame::GetDsskeyDisplayMode()
{
    if (devicelib_GetPhoneType() == PT_T52)
    {
        if (configParser_GetConfigInt(kszDsskeyLengthShorten) == 1)
        {
#if IF_TALKUI_BG_PICTURE
            m_PublicWidget.setTalkUIGeometry(TALK_UI_RECT_NARROW_LEFT
                                             , TALK_UI_RECT_TOP
                                             , TALK_UI_RECT_NARROW_WIDTH
                                             , TALK_UI_RECT_HEIGHT);
#endif

            return DDM_TWO_CHARS;
        }
        else
        {
#if IF_TALKUI_BG_PICTURE
            m_PublicWidget.setTalkUIGeometry(TALK_UI_RECT_NORMAL_LEFT
                                             , TALK_UI_RECT_TOP
                                             , TALK_UI_RECT_NORMAL_WIDTH
                                             , TALK_UI_RECT_HEIGHT);
#endif

            return DDM_NO_LABEL;
        }
    }

    return DDM_NORMAL;
}
// 音量条可见状态的槽函数
void CDlgTalkUIFrame::OnVolumeBarVisibleChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    bool bVisible = wParam;

    if (m_pTalkUI)
    {
        m_pTalkUI->OnTalkVolumnVisibleChange(bVisible);
    }

    _TalkConflictManager.setVolumebarVisible(bVisible);
//     _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_VOLUMEBAR, bVisible);
//     _TalkConflictManager.checkShowUnit();
}

//音量值改变
void CDlgTalkUIFrame::OnVolumeValueChanged(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    int nValue = wParam;

    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->OnTalkVolumnValueChange(nValue);
    }
}

//刷新通话UI
void CDlgTalkUIFrame::RefreshTalkUIData()
{
    if (NULL != m_pTalkUI)
    {
        //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
        m_pTalkUI->RefreshTalkUIData();
    }
}


bool CDlgTalkUIFrame::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    //判断顶层窗口 不是自己 直接返回
    if (this != UIManager_GetTopWindow())
    {
        return false;
    }

    int key = nKeyCode;

    if (NULL != m_pTalkUI)
    {
        m_PublicWidget.OnKeyEvent(m_pTalkUI, key, true);
        m_pTalkUI->OnKeyPreProcess(key);
    }

    if ((PHONE_KEY_UP == key) || (PHONE_KEY_DOWN == key))
    {
        if (NULL != m_pTalkUI)
        {
            if (m_pTalkUI->IsChangeSession(key))
            {
                if (PHONE_KEY_UP == key)
                {
                    //往上翻
                    ChangePrevSession();
                }
                else
                {
                    //往下翻
                    ChangeNextSession();
                }

                return true;
            }
        }
    }

    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->OnKeyPress(key);
    }

    return false;
}

bool CDlgTalkUIFrame::onKeyPressedEvent(int key)
{
    return false;
}

bool CDlgTalkUIFrame::onKeyReleasedEvent(int key)
{
    //判断顶层窗口 不是自己 直接返回
    if (this != UIManager_GetTopWindow())
    {
        return false;
    }

    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->OnKeyRelease(key);
    }

    return false;
}

bool CDlgTalkUIFrame::onFocusMoveEvent(xView * pNewFocus, xView * pOldFocus)
{
    //判断顶层窗口 不是自己 直接返回
    if (this != UIManager_GetTopWindow())
    {
        return false;
    }

    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->onFocusMove(pNewFocus, pOldFocus);
    }

    return false;
}

// 初始化
void CDlgTalkUIFrame::Init()
{
    CONFLICT_UNIT stUnit;

    stUnit.m_ShowFunction = NULL;
//     _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_VOLUMEBAR, stUnit);

    m_pTalkUI = NULL;
    m_TitleBar.Init();
    m_MenuTitle.Init();
    m_PublicWidget.Init();

    Volumebar_ConnectVisibleChangeSlot(this,
                                       static_cast<slotMethod>(&talkui::CDlgTalkUIFrame::OnVolumeBarVisibleChanged));
    Volumebar_ConnectVolumeChangeSlot(this,
                                      static_cast<slotMethod>(&talkui::CDlgTalkUIFrame::OnVolumeValueChanged));

    SetDefaultShowDsskey(true);

#if IF_TALKUI_BG_IDLE_STYLE
    InitBackground();
#endif

    GetStationName();
}

// 释放数据，将窗.口清空回复到初始化状态
void CDlgTalkUIFrame::Uninit()
{
//     _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_VOLUMEBAR);

    m_pTalkUI = NULL;
    m_TitleBar.Uninit();
    m_MenuTitle.Uninit();
    m_PublicWidget.Uninit();

    Volumebar_DisconnectVisibleChangeSlot(this,
                                          static_cast<slotMethod>(&talkui::CDlgTalkUIFrame::OnVolumeBarVisibleChanged));
    Volumebar_DisconnectVolumeChangeSlot(this,
                                         static_cast<slotMethod>(&talkui::CDlgTalkUIFrame::OnVolumeValueChanged));

#if IF_TALKUI_BG_IDLE_STYLE
    m_brushBG.setPixmap(xPixmap());
#endif
}

// 刷新窗口
void CDlgTalkUIFrame::UpdateWnd(bool bUIReturn/* = false*/)
{
    if (NULL != m_pTalkUI)
    {
        RefreshSoftkey();

        m_pTalkUI->UpdateWnd(bUIReturn);
    }
}

// 焦点改变.
// | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
void CDlgTalkUIFrame::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        m_PublicWidget.SetFrameFocus();
    }

    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->OnFocusChanged(bFocused);
    }
}

// 是否跳过Return To Idle，包括通话、Idle
// bClickToIdle是否为点击IDLE发起的回退
bool CDlgTalkUIFrame::IsRejectReturnToIdle()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->IsRejectReturnToIdle();
    }

    return true;
}

//回到Idle前的处理函数
void CDlgTalkUIFrame::OnReturnToIdle()
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->OnReturnToIdle();
    }
}

void CDlgTalkUIFrame::OnReturnToIdleAction()
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->OnReturnToIdleAction();
    }
}

//获取窗体类型
EWindowType CDlgTalkUIFrame::GetWindowType() const
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->GetWindowType();
    }

    return m_eWndType;
}

// 获取当前页面音量条的数据
// 返回值：是否显示音量条,默认不显示音量条
// iVolumeType | [out] 音量条的类型
// iMin | [out] 音量条的最小值
// iMax | [out] 音量条的最大值
// strBGPath | [out] 音量条的背景图片
bool CDlgTalkUIFrame::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                      yl::string & strPix)
{
    iMin = 1;
    iMax = 15;

    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->IsShowVolumeBar(iVolumeType, iMin, iMax, strPix);
    }

    //默认显示
    return true;
}

//获取Softkey数据
void CDlgTalkUIFrame::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->GetSoftkey(objSoftkeyData);
    }
}

// 是否显示Softkey，默认认为显示
bool CDlgTalkUIFrame::IsShowSoftkey()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->IsShowSoftkey();
    }

    return false;
}

#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
yl::string CDlgTalkUIFrame::GetSoftkeyBG()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->GetSoftkeyBG();
    }

    return CBaseDialog::GetSoftkeyBG();
}
#endif

// about dsskey api
// 是否显示Dsskey，默认认为不显示 bExpend
bool CDlgTalkUIFrame::IsShowDsskey(bool & bExpend)
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->IsShowDsskey(bExpend);
    }

    return false;
}

#if IF_TALKUI_BG_IDLE_STYLE
void CDlgTalkUIFrame::InitBackground()
{
    xPixmap pix;
    yl::string strWallPath = SettingUILogic_GetWallpaperPath(WALLPAPER_Type_CURRENT);

    Image_GetImage(pix, strWallPath, IMG_CLASSIFY_WALLPAPER);
    Image_RemoveImage(strWallPath, IMG_CLASSIFY_WALLPAPER);

    if (!pix.isNull())
    {
        m_brushBG.setPixmap(pix);
    }
    else
    {
        strWallPath = SettingUILogic_GetWallpaperPath(WALLPAPER_Type_DEFAULT);
        Image_GetImage(pix, strWallPath, IMG_CLASSIFY_WALLPAPER);
        Image_RemoveImage(strWallPath, IMG_CLASSIFY_WALLPAPER);

        m_brushBG.setPixmap(pix);
    }
}
#endif

#if IF_TALKUI_BG_PICTURE
//由于xWindow实现改变，该处处理也需要跟着改变
////////////////背景///////////////////////////
//设置整个大背景的图片（不是界面自身的背景）
xBrush CDlgTalkUIFrame::GetBackGroundBrush()
{
    bool bResult = false;
    xBrush brush;
    if (NULL != m_pTalkUI)
    {
        brush = m_pTalkUI->GetBackGroundBrush(bResult);
    }

#if IF_TALKUI_BG_IDLE_STYLE
    if (!bResult)
    {
        return m_brushBG;
    }
#endif

    return brush;
}
///////////////////////////////////////////////
#endif

// 是否允许键盘事件通过
bool CDlgTalkUIFrame::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->AllowKeyPadEventOnKeyLock(nKeyCode, bPress);
    }

    return false;
}


// 是否为顶层窗体
bool CDlgTalkUIFrame::IsTopWnd()
{
    return true;
}

////////////////////////////////////////////////
//获取焦点控件  用于输入法显示title使用
xView * CDlgTalkUIFrame::GetFocusWidget()
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->GetFocusWidget();
    }

    return NULL;
}
////////////////////////////////////////////////

bool CDlgTalkUIFrame::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (NULL != m_pTalkUI)
    {
        return m_pTalkUI->OnSoftkeyPress((SoftKey_TYPE)iType, bLongPress);
    }

    return false;
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
//更新ACD队列状态
void CDlgTalkUIFrame::UpdateACDQueueStatus(yl::string strACDQueueStatus)
{
    m_PublicWidget.RefreshCCStatus();
}
#endif

//更新网络连接状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
void CDlgTalkUIFrame::UpdateNetworkStatus(TALK_NETWORK_STATUS eStatus)
{
    m_PublicWidget.SetNetworkHint(eStatus);
}

//更新WIFI信号状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
void CDlgTalkUIFrame::UpdateWifiStatus(bool bIsReady)
{
// #warning "UpdateWifiStatus"
    //目前需求，通话中不需要显示wifi相关的图标
}

//刷新softkey
void CDlgTalkUIFrame::RefreshSoftkey()
{
    CArraySoftKeyBarData arraySoftkey;
    GetSoftkey(arraySoftkey);

    SetSoftKey(arraySoftkey);
}

// 切换到下一个Session
void CDlgTalkUIFrame::ChangeNextSession()
{
    TALKUI_INFO("CDlgTalkUIFrame::ChangeNextSession");
    //直接使用逻辑层的接口
    talklogic_SwitchFocusSession(true);
}

// 切换到上一个Session
void CDlgTalkUIFrame::ChangePrevSession()
{
    TALKUI_INFO("CDlgTalkUIFrame::ChangePrevSession");
    //直接使用逻辑层的接口
    talklogic_SwitchFocusSession(false);
}

#if IF_TITLE_ICON_FIXED
void CDlgTalkUIFrame::ResetFixedChannelIcon()
{
    SetFixedIcon(LCD_ICON_HS, false);
    SetFixedIcon(LCD_ICON_HF, false);
    SetFixedIcon(LCD_ICON_HEADS, false);
}

void CDlgTalkUIFrame::ShowFixedChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon)
{
    ResetFixedChannelIcon();
    LCD_ICON_TYPE eType = GetChannelIconTypeByChannelIconType(eTalkUIChannelIcon);
    SetFixedIcon(eType, true);
}

void CDlgTalkUIFrame::ResetFixedOtherIcon()
{
    SetFixedIcon(LCD_ICON_LOCK, false);
    SetFixedIcon(LCD_ICON_MUTE, false);
    SetFixedIcon(LCD_ICON_DND, false);
}

bool CDlgTalkUIFrame::ShowFixedOtherIcon(TALKUI_ICON iconType)
{
    if (TALKUI_ICON_SIPENCRY == iconType
            || TALKUI_ICON_VOICEENCRY == iconType)
    {
        SetFixedIcon(LCD_ICON_LOCK, true);
        return true;
    }
    else if (TALKUI_ICON_SILENCE == iconType)
    {
        SetFixedIcon(LCD_ICON_MUTE, true);
        return true;
    }
    else if (TALKUI_ICON_DND == iconType)
    {
        SetFixedIcon(LCD_ICON_DND, true);
        return true;
    }

    return false;
}

void CDlgTalkUIFrame::SetFixedIcon(LCD_ICON_TYPE eType, bool bShow)
{
    if (eType >= LCD_ICON_HS
            && eType < LCD_ICON_LAST)
    {
        devicelib_SetIcon(eType, bShow);
    }
}

#endif

void CDlgTalkUIFrame::GetStationName()
{
    m_strStationName = configParser_GetConfigString(kszStationName);
    if (m_strStationName.empty())
    {
        m_bStationName = false;
    }
    else
    {
        m_bStationName = true;

        if (0 != configParser_GetConfigInt(kszStationNameScrolling))
        {
            m_bStationNameScroll = true;
        }
        else
        {
            m_bStationNameScroll = false;
        }
    }
}

}       //namespace talkui
