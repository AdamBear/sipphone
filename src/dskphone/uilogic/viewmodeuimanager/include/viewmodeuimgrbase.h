#if (defined(IF_SUPPORT_BLUETOOTH ) || defined(IF_SUPPORT_WIFI))
#ifndef _VIEWMODEUI_MGRBASE_H_
#define _VIEWMODEUI_MGRBASE_H_

#include "viewmodectrler.h"

class CViewModeUIMgrBase
{
public:
    CViewModeUIMgrBase();
    virtual ~CViewModeUIMgrBase();
    // 设置控制器
    bool SetControler(int iUIType);
    // 释放控制器
    void ReleaseControler();
    // 获取控制器
    CViewModeControler * GetControler();
    // 更新页面
    virtual void UpdateUI(bool bResetFocus = false) {}
    // 更新title
    virtual bool UpdateTitle(yl::string strTitle)
    {
        return false;
    }
    // 更新item
    virtual bool UpdateListInfo(bool bResetFocus = false)
    {
        return false;
    }
    // 更新softkey
    virtual bool UpdateSoftKet(int iUIType = VIEW_MODE_UI_TYPE_BEGIN)
    {
        return false;
    }
    // 更新动画
    virtual bool UpdateAnimation(bool bEnable)
    {
        return false;
    }
    // 弹出消息框
    virtual bool PopupMsgBox(int iID, yl::string strTitle,
                             yl::string strContent, int iStyle, int iTimeOut,
                             bool bCountDown = false, bool bQuitEnable = true)
    {
        return false;
    }
    // 移除消息框
    virtual void RemoveMsgBox(int iID) {}
    // 进入页面
    virtual bool EnterPage(int iUIType, int iFocusIndex = 0)
    {
        return false;
    }
    // 退出页面
    virtual bool ExitPage()
    {
        return false;
    }
    // 获取焦点数据ID
    virtual int  GetFocusDataID()
    {
        return 0;
    }
    // 获取顶层页面类型
    virtual int  GetTopUIType()
    {
        return 0;
    }
    // 显示option
    virtual bool ShowOption(bool bShow = true)
    {
        return false;
    }
    // 获取页面数据
    virtual int  GetItemDataID2Info(VecItemDataID2Info & vecItemDataID2Info)
    {
        return false;
    };
    // 设置数据列表焦点
    virtual bool SetDataListFocus(int iDataID)
    {
        return false;
    };

protected:
    CViewModeControler       *      m_pViewModeControler;   // 控制器
    int                             m_NextFocusIndex;
};

#endif // _VIEWMODEUI_MGRBASE_H_
#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
