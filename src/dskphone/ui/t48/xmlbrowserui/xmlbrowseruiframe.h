#ifndef XML_BROWSER_UIFRAME_H
#define XML_BROWSER_UIFRAME_H

#include <vector>
#include <QFrame>
#include "cuiregionbase.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#include <QTimer>

class CXMLBrowserUIFrame : public QFrame
{
    Q_OBJECT

public:
    CXMLBrowserUIFrame(QWidget * parent = 0);
    ~CXMLBrowserUIFrame();

    // 更新界面
    void UpdateUI(const XMLLogicToUIData & objData);

    // 当softkey点击时调用的函数
    void OnSoftkeyBtnClick(const SoftkeyParam & objParam);

    // 重写事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

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

    bool MyKeypadHook(int nKeyCode, bool bPress);

    bool ProcessDsskey(int iIndex);

    void OnKeyboardShow();
    void OnKeyboardHide();

private:
    // 根据类型创建ElementUI
    static CUIRegionBase * CreateUIByType(XML_ELEMENT_TYPE eType);

    // 初始化
    void InitData();

    // 清空界面数据
    void ClearData();

    // 启动计时器
    void RestartTimer(int iTime);

    // 重新设置计时器
    void ReSetTimer(int iTime);

    // 相应softkey的操作
    // softkey操作:回退一格
    void OnClickSoftKeyBackSpace();

    // softkey操作:添加.
    void OnClickSoftKeyDot();

    // softkey操作:向下一格
    void OnClickSoftKeyNextSpace();

    // softkey操作:切换输入法
    void OnClickSoftKeyChangeIME();

    // softkey操作:提交
    void OnClickSoftKeySubmit();

signals:
    // 退出Frame的信号
    void QuitFrame();

    // 改变softkey的信号
    void ChangeSoftkey();

public slots:
    // 处理RegionAction
    void OnRegionAction(REGION_ACTION_TYPE eAction);

private slots:
    // 当退出页面计时器到时调用的槽函数
    void OnQuitTimerTimeOut();

private:
    std::vector<CUIRegionBase *> m_ArrpRegion;    // 存放Frame所有的指针
    CUIRegionBase       *       m_pCurrentRegion; // 当前Region的Index
    XB_UI_TYPE      m_ePageType;  // UIFrame所在Dlg的页面类型
    int             m_iTimeOut;   // 不操作退出界面的时间
    QTimer          m_quitTimer; // 计算退出界面时间的计时器
};

#endif // XML_BROWSER_UIFRAME_H
