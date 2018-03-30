#ifndef _SETTING_PAGE_H_
#define _SETTING_PAGE_H_

#include "uimanager/basedialog.h"
#include "uicommon.h"
#include <map>
#include "settinguicommon.h"
#include <ylstring.h>
#include "settinguifeaturesdefine.h"

#define  MAX_ITEM_NUM 20
// 前向声明
class CDlgBaseSubPage;
class CSettingWindow;
class CFrameList;
class CFrameListDelegate;
class CSettingPageHelper;
class CMenuviewDelegate;
typedef std::vector<CSettingWindow *> SettingWndVec;

// 设置界面最外层主框架类（顶层窗口）
class CSettingWindow : public CBaseDialog
{
    Q_OBJECT

public:
    // 重定义类型
    typedef std::map<yl::string, CDlgBaseSubPage *> MapSubPage;
#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    typedef MapSubPage::iterator               IterSubPage;
    typedef list<CDlgBaseSubPage *>             List_SubPage;
    typedef List_SubPage::iterator             pageListIiter;
#endif

public:
    //取得实例
    static CBaseDialog * CreateInstance();

    //获取已经存在的setting窗口的链表，以便进行全部刷新等操作
    static SettingWndVec GetSettingWndList();
public:
    explicit CSettingWindow(QWidget * parent = 0);
    virtual ~CSettingWindow();

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //virtual QPixmap GetCustomBackGround();

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    //设置数据，如列表的action
    void SetWndData(const SubMenuData & objSubMenuData);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    //注册消息
    void RegisterMsg();
    void UnRegisterMsg();

    // 是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    //刷新菜单
    void UpdateWndFromMSG();
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    void UpdateWndFromUSBMSG(msgObject & objMessage);
#endif

    //选择默认索引，bEnter表示是否进入
    void SelectItemIndex(int nIndex, bool bEnter = false);

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    //选中左侧菜单的某一项
    void SetCurrentItem(int iIndex);

    void SetCurrentItem(const QString & strItemClickAction);
#endif

    //提供接口设置,取得子页面
    CDlgBaseSubPage * GetCurrentSubPage() const;

    // 设置子窗口
    bool SetSubPage(const SubMenuData & pageInfo);

    // 设置子窗口
    virtual void SetSubPage(CDlgBaseSubPage * pSubPage, const yl::string & strSubPageAction,
                            const int nDefaultIndex = 0);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 获取页面的Action
    yl::string GetPageAction();

    void SetPageMode(PAGE_MODE eMode);

#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    //断开对左侧菜单切换的响应
    void DisconnectMenuView();

    //连接对左侧菜单切换的响应
    void ConnectMenuView();
#endif

    virtual void UpdateWnd();

    bool IsEmpty();

    virtual void OnBackButtonClick();
    virtual void OnHomeButtonClick();

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    void SetPageStyle(SETTING_PAGE_STYLE eStyle);
#endif

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();

public:
    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

    //设置窗口的提示模式命令
    virtual void SetHintTitle(const QString & strHint) ;

    //获取窗口提示模式命令
    virtual const QString GetWindowHint() const;

protected:
    virtual bool OnAction(const QString & strAction);
    virtual bool OnUIAction(const QString & strAction, void * pActionData = NULL);

private:
    // 处理回退事件
    void ProcessBack();

    // 弹出当前的子页面
    void TakeSubPage();

    // 释放缓存的子页面
    void ReleaseCacheDlg();

    // 保存缓存的页面数据
    bool SaveCacheDlgData();

    // 设置窗口的SoftKey数据
    void SetSoftKeyBarData();

    //添加常用的softkey的图标
    void AddSoftkeyIcon(CArraySoftKeyBarData & objWndData);

    //根据softkey类型获得图标
    QString GetIconByAction(yl::string strAction);

    //清空数据
    void ClearPageData();
#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    //子页面入栈
    void PushSubPage(CDlgBaseSubPage * pSubPage);
    //删除栈顶子页面
    void PopTopSubPage();
    //清除子页面堆栈
    void ClearSubPageStack();
#endif

    bool RefreshTitle();
    //检查页面是否需要拒绝回到idle
    void CheckReturnToIdleFlag();
    //检查页面是否需要拒绝开启回到idle的定时器
    void CheckCloseTimerFlag();

private:
    void OnBtnSaveClick();
    void OnSoftkeyBtnClick();
    void OnBtnIMEClick();
    void OnBtnDeleteClick();
    //添加一个SetttingWnd对象的指针到链表
    static void AddSettingWnd(CSettingWindow * pWnd);
    //从链表中删除一个SetttingWnd的指针
    static void EraseSetttingWnd(CSettingWindow * pWnd);
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    bool CreateMenuView();
#endif
    void DisconnectSubpage(CDlgBaseSubPage * pSubPage); //连接一个子页面的信号槽
    void ConnectSubpage(CDlgBaseSubPage * pSubPage); //断开一个子页面的信号槽

private Q_SLOTS:
    //响应选中Menu
    void OnProcessPageAction(const QString & strAction);

    void OnRefreshSoftkey(); // 刷新Softkey

    void OnRefreshTitle(); // 刷新Title

    void OnRefreshTimer(); // 刷新定时器状态
    void OnBtnBackClick();

    virtual void OnMenuViewCurrentItemChange(const QString &);

    bool OnBackAndEnterNewPage(const SubMenuData & pageInfo);

    //处理内部打开页面请求
    bool OnOpenpage(const SubMenuData & pageInfo);

    bool OnOpenpage(const yl::string & strAction);
    // 用新页面替换当前页面
    bool OnReplaceCurPage(const SubMenuData & subData);

protected:
    PAGE_MODE               m_ePageMode;
    yl::string              m_strMenuAction;
    yl::string              m_strCurrentSubPageAction;
    CDlgBaseSubPage    *    m_pSubPage;
#if IF_SETTINGUI_WND_SUPPORT_LEFT_MENU_STYLE
    CFrameList       *      m_pMenuList;// 左侧菜单栏显示
    CMenuviewDelegate   *   m_pMenuListDelegate;
#endif


private:
    MapSubPage             m_mapSubPage; // zero专用页面缓存
    //所有当前存在的settingwnd的链表，用于三级权限变化时刷新缓存的的SettingWnd
    static SettingWndVec SettingWndList;
    //记录当前是否处于非idle状态
    bool m_bIsIdleStatus;

#if IF_SETTINGUI_SUPPORT_MULTI_STOREY_SUBPAGE
    List_SubPage m_listSubPage;//setting子页面栈
#endif

    SETTING_PAGE_STYLE m_ePageStyle;    //风格：带不带左侧菜单
    CSettingPageHelper * m_pPageHelper;
};


#endif //_SETTING_PAGE_H_

