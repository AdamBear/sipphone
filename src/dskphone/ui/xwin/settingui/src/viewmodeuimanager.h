#if defined (IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)
#ifndef _VIEWMODEUI_MANAGER_H_
#define _VIEWMODEUI_MANAGER_H_

#include "viewmodeuimanager/include/viewmodeuimgrbase.h"
#include "viewmodeuimanager/include/viewmodectrler.h"

#include "singletonclassdefine.h"
#include "dlgsettingui.h"
#include "messagebox/messageboxcallback.h"
#include "messagebox/selectmessagebox.h"


typedef YLHashMap<int, yl::string>::iterator    Iter_UIType_2_PageName;
typedef YLHashMap<int, yl::string>              MAP_UIType_2_PageName;

class CViewModeUIMgr: public CViewModeUIMgrBase, public CMsgBoxCallBackAction
{
    DECLARE_SINGLETON_CLASS(CViewModeUIMgr)

    // MsgBox 标识
    enum MsgBoxID
    {
        MSGBOX_ID_OPTION = -2,
    };

public:
    // 更新页面
    virtual void UpdateUI(bool bResetFocus = false);
    // 更新title
    virtual bool UpdateTitle(yl::string strTitle);
    // 更新item
    virtual bool UpdateListInfo(bool bResetFocus = false);
    // 更新softkey
    virtual bool UpdateSoftKet(int iUIType = VIEW_MODE_UI_TYPE_BEGIN);
    // 更新动画
    virtual bool UpdateAnimation(bool bEnable);
    // 弹出消息框
    virtual bool PopupMsgBox(int iID, yl::string strTitle,
                             yl::string strContent, int iStyle, int iTimeOut,
                             bool bCountDown = false, bool bQuitEnable = true);
    // 移除消息框
    virtual void RemoveMsgBox(int iID);
    // 进入页面
    virtual bool EnterPage(int iUIType, int iFocusIndex = 0);
    // 退出页面
    virtual bool ExitPage();
    // 获取焦点数据ID
    virtual int  GetFocusDataID();
    // 获取顶层页面类型
    virtual int  GetTopUIType();
    // 显示option
    virtual bool ShowOption(bool bShow = true);
    // 通过页面类型获取页面名称
    yl::string GetPageName(int iUIType);
    // 通过页面名称获取页面类型
    int GetUITypeByPagenName(const yl::string & strPageName);
    //
    int  GetNextFocusIndex()
    {
        return m_NextFocusIndex;
    }
    void SetNextFocusIndex(int iFocusIndex)
    {
        m_NextFocusIndex = iFocusIndex;
    }

    virtual bool SetDataListFocus(int iDataID);

public:
    yl::string GetSoftKeyTitleByType(int iType);
    yl::string GetOptioTitleByOptionType(int iOptionType);
    int TransformToMyMsgBoxStyle(int eStyle);
    int TransformToMsgBoxResult(int iStyle);

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    void OptionMsgBoxCallBack(CSelectMessageBox * pOptionMsgBox);

private:
    CDlgSettingUI * GetSettingUIDlg();

private:
    MAP_UIType_2_PageName       m_mapUIType2PageName;   // ui type映射到page name

};

#define _ViewModeUIMgr GET_SINGLETON_INSTANCE(CViewModeUIMgr)

#endif // _VIEWMODEUI_MANAGER_H_
#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
