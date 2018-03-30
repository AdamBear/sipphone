#ifndef XML_BROWSER_UIFRAME_H
#define XML_BROWSER_UIFRAME_H

#include "ylvector.h"
#include "cuiregionbase.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#include "xwindow-kernel/include/xsignal.h"

#if XML_BROWSER_SUPPORT_UI

typedef YLVector<CUIRegionBase *>            VEC_UIREGION;

class xXMLBrowserUIFrame : public xFrameLayout, public xSlotHandler, public xTimerHandler
{

public:
    xXMLBrowserUIFrame();
    ~xXMLBrowserUIFrame();

    // 更新界面
    void UpdateUI(const XMLLogicToUIData & objData);

    bool OnSoftkeyPress(int nKeyIndex);

    // 获取当前的softkey
    VecSoftkeyParam & GetCurrentUISoftkey();

    // 停止计时器
    void StopTimer();

    // 设置Frame所在的页面类型
    inline void SetPageType(XB_UI_TYPE ePageType)
    {
        m_ePageType = ePageType;
    }

    // 获取Frame所在的页面类型
    inline XB_UI_TYPE GetPageType()
    {
        return m_ePageType;
    }

    bool ProcessKeyEvent(int nKeyCode, bool bPress);

    // 初始化
    void InitData();

    // 刷新界面
    void RefreshUI();

    // 退出指定UI
    void ExitXmlUI(int nElementID);

private:
    // 根据类型创建ElementUI
    static CUIRegionBase * CreateUIByType(XML_ELEMENT_TYPE eType);

    CUIRegionBase* GetUIByElementID(int nElementID);

    void UnbindOldRegion();

    // 当softkey点击时调用的函数
    void OnSoftkeyBtnClick(const SoftkeyParam & objParam);

    // 清空界面数据
    void ClearData();

    // 启动计时器
    void RestartTimer(int iTime);

    // 重新设置计时器
    void ReSetTimer(int iTime);

    // softkey操作:提交
    void OnClickSoftKeySubmit();

    // 处理RegionAction
    void OnRegionAction(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    // 当退出页面计时器到时调用的槽函数
    void OnQuitTimerTimeOut(xThreadTimer* timer, LPVOID pExtraData);
public:
    xSignal                     m_signalFocusChange;

private:
    VEC_UIREGION                m_ArrpRegion;     // 存放Frame所有的指针
    CUIRegionBase       *       m_pCurrentRegion; // 当前Region的Index
    XB_UI_TYPE                  m_ePageType;  // UIFrame所在Dlg的页面类型
    int                         m_iTimeOut;   // 不操作退出界面的时间
    xThreadTimer                m_timerQuit; // 计算退出界面时间的计时器
};

#endif

#endif // XML_BROWSER_UIFRAME_H
