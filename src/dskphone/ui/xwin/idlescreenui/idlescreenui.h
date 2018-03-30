#ifndef __IDLE_SCREEN_UI_H__
#define __IDLE_SCREEN_UI_H__

#include "uimanager/basedialog.h"
#include <idleuilogic/include/idledefine.h>
#include "idleuilogic/include/baseidleui.h"

#include "messagebox/messageboxbase.h"
#include <settinguilogic/include/settingdefine.h>
#include "baseui/include/baseuidefine.h"
#include <idlescreenui/idlescreenuidefine.h>
#include "extended/xmlbrowser/include/logicanduicommon.h"

class xListView;
class xDateTimeView;
class xFlashingBox;
class xAnimationBox;
class CMessageBoxBase;
class CTextStatusAdapter;

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
struct ST_IDLE_WIDGET_RECT
{
    chRect  rcDateTime;
    chRect  rcAccount;
    chRect  rcCustomNotify;
#if IF_FEATURE_DND_LARGE_ICON
    chRect  rcLargeDND;
#endif
};
#endif

#if IF_SUPPORT_ICON_PARAMETER
struct ST_ICON_PARAMETER
{
    int     nIconSize;
    bool    bLeft2Right;
};
#endif

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET || IF_SUPPORT_ICON_PARAMETER || IF_SUPPORT_EXTRA_IDLE_NOTE
class xIdleFrameHandle : public xViewHandler
{
    friend class xDetailFrame;
public:
    xIdleFrameHandle();
    virtual ~xIdleFrameHandle();

    static xIdleFrameHandle* GetIdleFrameHandle(xViewHandler* pHandle);

public:
#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
    void GetIdleWidgetRect(ST_IDLE_WIDGET_RECT& stWidgetRect, bool bNormal);
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    int GetDateTimeOffsetY()
    {
        return m_nOffsetYDateTime;
    }
#endif
#if IF_SUPPORT_ICON_PARAMETER
    void GetIconParam(ST_ICON_PARAMETER& stIconParam)
    {
        stIconParam = m_stIconParam;
    }
#endif
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    void GetStatusIconRect(chRect& rcIcon, bool bRight);
#endif
public:
    virtual void loadAttributes(xml_node& node);

protected:
#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
    ST_IDLE_WIDGET_RECT m_stWidgetRectNormal;
    ST_IDLE_WIDGET_RECT m_stWidgetRectShort;
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    int                 m_nOffsetYDateTime;
#endif
#if IF_SUPPORT_ICON_PARAMETER
    ST_ICON_PARAMETER   m_stIconParam;
#endif
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    chRect              m_rcIconStatusRight;
    chRect              m_rcIconStatusCenter;
#endif
};
#endif

class CIdleScreenUI : public xFrameLayout, public CBaseDialog, public CBaseIdleUI
{
public:
    //取得实例
    static CBaseDialog* CreateInstance();
    static CIdleScreenUI* GetInstance();

protected:
    CIdleScreenUI();
    virtual ~CIdleScreenUI();
#if IF_SUPPORT_WALLPAPER
    static LRESULT OnIdleScreenMsg(msgObject& msg);
#endif
public:
    enum
    {
        IDLEUI_LAYOUT_SHOW_NONE = 0,                     // Invalid date time format type.
#if IF_SUPPORT_TEXT_NOTIFY
        IDLEUI_LAYOUT_SHOW_PROMPT_TEXT = 0x1 << 1,       // 显示Prompt提示文本
#endif
#if IF_FEATURE_DND_LARGE_ICON
        IDLEUI_LAYOUT_SHOW_LARGE_DND = 0x1 << 2,         // DND大图标
#endif
        IDLEUI_LAYOUT_SHOW_CUSTOM_TEXT = 0x1 << 3,       // 显示自定义提示文本（Xmlbrowser/ACD queue）
#if IF_SUPPORT_EXTRA_IDLE_NOTE
        IDLEUI_LAYOUT_SHOW_EXTRA_TEXT = 0x1 << 4,       // 显示自定义提示文本（Xmlbrowser/ACD queue）
#endif
        IDLEUI_LAYOUT_SHOW_MAX
    };
public:
    virtual void Init();
    virtual void Uninit();
    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    virtual void OnFocusChanged(bool bFocused);

    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

    virtual LRESULT OnConfigChangeMsg(msgObject& msg);

public:
    virtual void loadChildrenElements(xml_node& node);

#if IF_SUPPORT_ICON_PARAMETER
    virtual void loadAttributes(xml_node& node);
#endif

    virtual bool onKeyPressedEvent(int key);
    virtual bool onKeyReleasedEvent(int key);
    virtual bool onKeyLongPressedEvent(int key);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

    virtual bool IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix);

public:
    virtual bool UpdateSoftkey(VEC_SOFTKET_TYPE& softkey);
#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
    virtual yl::string GetSoftkeyBG();
#endif

#if IF_SUPPORT_TEXT_NOTIFY
    virtual bool UpdatePromptText(const ST_TextNotifyData& stNotifyText);
#endif
    virtual bool UpdateIcon(const VEC_ICON_INFO& Statuslist);
    virtual bool UpdateXmlBrowser(VEC_CUSTOM_NOTIFY& listRefreshData);
    virtual bool CreateMessageBox(const PopupBoxInfo& popData);
    virtual bool RemoveIdleMessageBox(int nID);
    virtual bool UpdateAccountInfo(const yl::string& strName, ICON_TYPE eIcon);
    virtual void RefreshSoftkey();
    //更新自定义显示内容
    virtual bool UpdateCustomNotify(const VEC_STATUS_ITEM& refCusNotify, bool bCusNotify);
    virtual void EnterPasswordCheckPage();
    virtual void SetLongPressTimeByKey(int nKey, int nTime);
    virtual void ResetLongPressTimeByKey(int nKey);
    virtual bool IsExistMessageBox();
    virtual int GetDsskeyIDByKeyCode(int nKey);
    virtual void EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType);
#if IF_SUPPORT_CENTER_ACD_STATUS
    virtual void UpdateCenterNote(const yl::string& strNote);
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    virtual void UpdateExtraNote(const yl::string& strNote);
#endif

public:
    // 显示弹框
    void OnShowMessageBox(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    //音量条显示改变
    void OnVolumeVisibleChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    //音量值改变
    void OnVolumeChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
#if IF_IDLE_SUPPORT_LOGO
    void ShowLogo(bool bShow, const yl::string& strPic);
#endif

#if IF_SUPPORT_WALLPAPER
    //设置整个大背景的图片（不是界面自身的背景）
    virtual xBrush GetBackGroundBrush();
    void UpdateBackGroundBrush();
#endif

#if IF_FEATURE_STATION_NAME
    // 刷新station name
    void RefreshStationName();
#endif

    bool IsSupportDsskeyShorten();
#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    bool IsShowDefaultAccount();
#endif
    // 是否需要显示Idle内容，如Acd文本、xmlBrowser文本、DND大图标等
    bool IsIdleContentShow();

    virtual bool OnDsskeyLengthChange();

    // Dsskey显示类型
    virtual DSSKEY_DISPLAY_MODE GetDsskeyDisplayMode();

    // 改变Dsskey显示类型
    void ChangeDsskeyDispalyMode(bool bUpdate = false);

#if IF_SUPPORT_DYNAMIC_IDLE_WIDGET
    void ChangeContentSizeByMode(DSSKEY_DISPLAY_MODE eDsskeyMode);
    void UpdateIdleContent(const ST_IDLE_WIDGET_RECT& stWidgetRect);
    void UpdateWidgetLayoutInfo(xView* pView, const chRect& rcWidget);
#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    void UpdateStatusIconLayout(bool bRight);
#endif
#endif
    //////////界面能否进入屏/////////
    virtual bool IsAllowScreenSaverCount()
    {
        return true;
    }


protected:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

private:
    void MapSoftkeyString(SoftKey_TYPE stype, yl::string& strSoftkey);
    BOOL TransSoftkeyToKeyCode(SoftKey_TYPE eSoftKey, PHONE_KEY_CODE& nKeyCode);
    SoftKey_TYPE ResultType2SoftkeyType(MESSAGEBOX_RESULT resulttype);
    MESSAGEBOX_RESULT SoftkeyType2ResultType(SoftKey_TYPE softtype);

    bool ClearIcon();
    void UpdateDynamicContent();

protected:
    xDateTimeView*  m_pDateTimeView;
    xLinearLayout*  m_pIconList;
#if IF_SHOW_ACOUNT_ICON
    xImageView*     m_pAccountPic;
#endif
    xLinearLayout*  m_pAccountLayout;
    xTextView*      m_pAccountText;
#if IF_FEATURE_STATION_NAME
    xTextView*      m_pStationText;
#endif
    xFrameLayout*   m_pFrameIdle;
#if IF_IDLE_SUPPORT_LOGO
    xImageView*     m_pLogoPic;
#endif
#if IF_FEATURE_DND_LARGE_ICON
    xLinearLayout*  m_pLargeDND;
#endif
    xLinearLayout*  m_pNotifyLayout;
    xImageView*     m_pNotifyPic;
    xTextView*      m_pNotifyText;
    xListView*      m_pListXMLStatus;
    CTextStatusAdapter* m_pAdapterXMLStatus;
#if IF_SUPPORT_TEXT_NOTIFY
    xLinearLayout*  m_pPromptLayout;
    xFlashingBox*   m_pPromptFlashBox;
    xAnimationBox*  m_pPromptAnimationBox;
    xTextView*      m_pPromptText;
#endif
    int             m_nLayoutShowFlag;
    int             m_nDateTimeType;
    int             m_nTimeShowType;
#if IF_SUPPORT_CENTER_ACD_STATUS
    xTextView*      m_pCenterNote;
#endif
#if IF_IDLE_HIDE_DATE_BY_ICON
    int             m_nDateTimeWidth;
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    xTextView*      m_pExtraNote;
#endif

#if IF_SUPPORT_IDLE_MIDDLE_CLOCK
    xDateTimeView*  m_pMiddleDateTimeView;
#endif

#if IF_SUPPORT_WALLPAPER
    xPixmap      m_pmBkGround;
#endif

};

#define  g_pIdleScreenUI (CIdleScreenUI::GetInstance())

#endif
