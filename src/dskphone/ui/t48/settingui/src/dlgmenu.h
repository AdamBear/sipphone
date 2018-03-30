#ifndef DLGMENU_H
#define DLGMENU_H

#include <vector>
#include "uimanager/basedialog.h"
#include "baseui/scrollctrl.h"

#define  MAX_FLOOR_MENU 20

// 窗体模板类前向声明
class CIconView;
class qUIPushButton;

class CDlgMenu : public CBaseDialog
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgMenu();
    }

public:
    CDlgMenu(QWidget * parent = NULL);

    virtual ~CDlgMenu();
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    // 初始化数据
    void InitData();
    //刷新菜单
    void UpdateMenuFromMSG();

    void SetTitle(const QString & strTitle);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    // 刷新翻页键
    void RefreshPageCtrl();

    // 是否显示Softkey，默认认为显示
    virtual bool IsShowSoftkey()
    {
        return false;
    }

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

public:
    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected:
    virtual bool OnAction(const QString & strAction);

private Q_SLOTS:
    //点击菜单响应操作
    void EnterMenuAction();

    //点击Back时回到上一级菜单
    void BackLastMenu();

    virtual void OnSoftKeyAction(qSoftButton * pbtn);

private:
    // 清空界面数据
    void ClearPageData();

    //存储Title的堆栈m_vecMenuTitle回退到上级菜单
    void BackLastTitle();

    //注册消息
    void RegisterMsg();

    // 取消消息的注册
    void UnRegisterMsg();

    void EnterPageByAction(const yl::string & strAction);

    void InitPageCtrlWidget();
    void SetPageFlag(int nRang, int nCurPage);

private slots:
    void OnClickLeftButton();
    void OnClickRightButton();

private:
    //用于显示菜单列表
    CIconView * m_pMenuIconView;
    //存放记忆上级菜单列表
    yl::string m_aStrMenuAction[MAX_FLOOR_MENU];
    //存放记忆上级一序列菜单列表所对应的page页面
    int m_aBackPage[MAX_FLOOR_MENU];
    //存放记忆上级一序列菜单的菜单名称(也就是displayName)
    std::vector<yl::string> m_vecMenuTitle;
    //存储记忆顶级菜单选中的Item的位置
    int m_nSelectedRow;
    //当前菜单
    yl::string m_strCurrentMenuAction;
    //记录回退时要用的icon View第几页
    int m_nBackPage;
    //记录当前是否处于非idle状态
    bool m_bIsIdleStatus;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    //记录USB是否连接上
    bool m_bUSBConnect;
#endif

    qUIPushButton   *   m_pBtnLeft;
    qUIPushButton   *   m_pBtnRight;
    QLabel       *      m_pPageFlag;
    bool                m_bPageCtrlInitSucceed;
};

#endif // DLGMENU_H
