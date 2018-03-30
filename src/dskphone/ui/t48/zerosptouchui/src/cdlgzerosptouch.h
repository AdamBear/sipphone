#ifndef CDLGZEROSPTOUCH_H
#define CDLGZEROSPTOUCH_H

#include "ui_cdlgzerosptouch.h"
#include "uimanager/basedialog.h"
#include <list>
#include "ylstring.h"

// 前向声明
class QTimer;
class qSoftButton;
class CWndTemplate;
class CDlgZeroNetwork;
class CDlgZeroStatus;
class DlgZeroAutoProvision;

// ZeroSpTouch页面类型
enum ZeroPageType
{
    ZERO_PAGE_NONE = -1,                //错误的页面模式
    ZERO_PAGE_MAIN = 0,                 //主界面

    ZERO_PAGE_NETWORK_SETTING,          //网络设置界面

    ZERO_PAGE_NOTE_UPDATING,            //正在更新中
    ZERO_PAGE_NOTE_UPDAT_FAIL,          //跟新失败
    ZERO_PAGE_NOTE_UPTADE_FINISH,       //更新完成

    ZERO_PAGE_NOTE_NETWORK_CHECK,   //网络检查界面
    ZERO_PAGE_NOTE_NETWORK_FAIL,        //网络异常

    ZERO_PAGE_NOTE_REBOOTING,           //重启中

    ZERO_PAGE_REDIRECTOR_NOTE,          //重定向提示界面
    ZERO_PAGE_REDIRECTOR_SUBMIT,        //重定向鉴权界面
    ZERO_PAGE_REDIRECTOR_FAILED,        //重定向失败提示界面
    ZERO_PAGE_REDIRECTOR_FINISH,        //重定向完成提示界面

    ZERO_PAGE_STATUS,                   //状态查看界面

    ZERO_PAGE_AUTOP,                    // AutoP
    ZERO_PAGE_UNAUTHORIZED,             //密码错误提示界面
    ZERO_PAGE_NOTE_AUTH_FAIL,           //鉴权失败界面
    ZERO_PAGE_AUTHENTICATION,       // 鉴权界面
    ZERO_PAGE_AUTOP_DIRECT,             // 直接显示输入用户名与密码界面，不显示倒计时与网络设置界面
};

class CDlgZeroSPTouch : public CBaseDialog
{
    Q_OBJECT

public:
    // 取得实例
    static CBaseDialog * CreateInstance();

public:
    CDlgZeroSPTouch();

    ~CDlgZeroSPTouch();

    // 初始化
    virtual void Init();

    // 释放数据,将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL)
    {
        return m_bRejectToIdle;
    }

    void SetRejectReturnToIdle(bool bRejectToIdle)
    {
        m_bRejectToIdle = bRejectToIdle;
    }

    // 初始化数据
    void InitData();

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);


    // 设置页面的类型
    void SetPageType(const yl::string & strPageAction);

    //事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool IsShowBackButton();
    virtual bool IsShowHomeButton()
    {
        return false;
    }

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();

    bool IsAuthPage();

    bool IsResultPage();
public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

private slots:
    // 鉴权失败的提示
    void OnAuthorizedIncorrectTimeout();

private:
    enum DELAY_OPERATOR_TYPE
    {
        DOT_NONE,
        DOT_NETWORK_FAIL,
        DOT_UPDATE_FAIL,
        DOT_UPDATE_FINISH,
        DOT_AUTH_FAIL,
    };

    // 刷新界面
    void RefreshPage();

    // 开启倒计时
    void StartRemain();

    // 停止倒计时
    void StopRemain();

    // 设置页面的类型
    void SetPageType(ZeroPageType ePageType);

    void ResetDelayOperatorTimer();

    // 将相应的页面Action转换为Type
    ZeroPageType ActionToType(const yl::string & strPageAction);

    // 获取页面的Softkey
    //void SetSoftkeyData();

    // 处理界面操作
    virtual bool OnAction(const QString & strAction);

    //更新标题
    void UpdateTitle();

    // 延时进行某些操作
    void DelayOperator(CDlgZeroSPTouch::DELAY_OPERATOR_TYPE eType);

    bool IsRpsOrZerotouch();

    void StartAuthTime();

private slots:
//  void on_editPassword_textChanged(const QString &);
    //主界面计时器到时的响应
    void OnRemainTimeOut();

    // 网络异常界面超时相应槽
    void OnNetworkFailPageTimeout();

    //更新成功自动退出
    void OnUpdateFinishPageTimeout();

    // 更新失败超时相应槽
    void OnUpdataFailPageTimeout();

    // 鉴权失败提示界面超时
    void OnAuthFailPageTimeout();

    // 延时操作后的释放资源等操作
    void OnDelayOperator();

    // 获取页面的Softkey
    void SetSoftkeyData();

private:
    Ui::CDlgZeroSPTouchClass ui;
    QTimer         *        m_pRemainTimer;
    int                     m_nRemainTime;      // 倒计时时间
    QTimer         *        m_pDelayTimer;      // 延时时间
    ZeroPageType            m_ePageType;    // 页面的模式
    yl::string              m_strPageAction;

    bool                    m_bShowTime;        //标记是否开启计时器
    bool                    m_bRejectToIdle;
    bool                    m_bExitAutoPWhenExitDlg;

    // 网络异常界面退出时间
    static const int WAITTING_EXIT_TIME = 3000;

    CDlgZeroNetwork    *    m_pDlgZeroNetwork;

    DlgZeroAutoProvision  * m_pDlgAutoP;

    DELAY_OPERATOR_TYPE     m_eDelayType;
};

#endif // CDLGZEROSPTOUCH_H
