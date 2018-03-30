#ifndef __BASE_DIALOG__
#define __BASE_DIALOG__

#include "uimanager_header.h"
#include "uimanager_common.h"
#include "dialogfactory.h"
#include "globalinputhook/inputhook_include.h"
#include "uikernel/qsoftkeybar.h"

#include "uicommon/messagebox/messageboxcallback.h"

// 将类名和创建类实例的函数加入到Factory Map中
#define REGISTER_DLG(className) \
    CDialogFactory::RegisterDialog(#className, className::CreateInstance);

#define ACTION_BACK "Back"

enum RETURN_IDLE_TYPE
{
    RETURN_IDLE_TYPE_NORMAL,
    RETURN_IDLE_TYPE_TIME_OUT,
    RETURN_IDLE_TYPE_HOME_PRESS,
    RETURN_IDLE_TYPE_CHANNAL,

    RETURN_IDLE_TYPE_TIME_LAST,
};


////////////////////////////softkeyBar数据类
class CSoftKeyBarData
{
public:
    CSoftKeyBarData()
    {
        InitData();
    }

    ~CSoftKeyBarData() {}

    // 初始化Softkey数据
    void InitData();

    // 重载赋值操作符
    CSoftKeyBarData & operator=(const CSoftKeyBarData & other);

public:
    bool        m_bDisplay;  // softkeyBar是否显示
    yl::string  m_strSoftkeyAction;
    QString     m_strSoftkeyTitle;
    yl::string  m_strSlotName;
    QString     m_strIconPath;  // 按钮图标的路径
    QObject  *  m_pReceiver; //添加信号接收对象指针成员变量
};

// 重定义类型
typedef std::vector<CSoftKeyBarData> CArraySoftKeyBarData;

class qSoftButton;
// 所有小窗口的基类，所有模块的小窗口必须从该类继承
// 以实现统一的Init Uninit接口
class CBaseDialog : public QDialog, public CMsgBoxCallBackAction
{
    Q_OBJECT

public:
    CBaseDialog(QWidget * parent = NULL);
    virtual ~CBaseDialog();

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) { }


    // 初始化
    virtual void Init() = 0;
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit() = 0;

    virtual void UpdateWnd();

    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    QRect GetDlgGeometry();

    void SetDlgGeometry(const QRect & rect);

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    //设置Softkey数据
    void SetSoftKey(const CArraySoftKeyBarData & objSoftkeyData);

    // 是否显示Title
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo)
    {
        return true;
    }

    virtual bool ProcessTitleBarSelectAccount(int nAccountID)
    {
        return false;
    }

    virtual void AfterShowTitle() { }

    // 是否显示Dsskey，默认认为不显示 bExpend
    virtual bool IsShowDsskey(bool & bExpend)
    {
        bExpend = false;
        return false;
    }
    // 是否显示Softkey，默认认为显示
    virtual bool IsShowSoftkey()
    {
        return true;
    }

    // 当前界面是否允许键盘锁的计时器计时，比如idle允许，通话不允许
    virtual bool IsAllowKeyguardCount()
    {
        return false;
    }

    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // rtVolume | [out] 音量条的位置
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const
    {
        return false;
    }

    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

    // 初始化所有的Softkey
    void InitSoftKeyData();

    //获取窗体类型
    EWindowType GetWindowType() const
    {
        return m_eWndType;
    }

    //设置Title
    void SetWindowTitle(const QString & strTitle)
    {
        m_strTitle = strTitle;
    }

    //获取Title
    virtual const QString GetWindowTitle() const
    {
        return m_strTitle;
    }

    //设置窗口的提示模式命令
    virtual void SetHintTitle(const QString & strHint)
    {
        m_strHint = strHint;
    }

    //获取窗口提示模式命令
    virtual const QString GetWindowHint() const
    {
        return m_strHint;
    }

    // 是否为顶层窗体
    bool IsTopWnd()
    {
        return (m_nDlgFlag & DIALOG_TOPWND) != 0;
    }

    // 是否跳过Return To Idle，包括通话、Idle
    // eReturnIdle发起回退idle的原因
    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL)
    {
        return (m_nDlgFlag & DIALOG_REJECT_RETURN_TO_IDLE) != 0;
    }

    void SetEnableReturnToIdle()
    {
        m_nDlgFlag = m_nDlgFlag & ~DIALOG_REJECT_RETURN_TO_IDLE;
    }
    // 是否允许鼠标事件通过
    virtual bool AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent);

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    virtual QWidget * GetFocusWidget()
    {
        return NULL;
    }

    // 子界面响应Back按钮的操作
    virtual void OnBackButtonClick();

    virtual bool IsShowBackButton()
    {
        return true;
    }

    ////---- 是否转化为事件
    // 子界面响应Home按钮的操作
    virtual void OnHomeButtonClick();

    virtual bool IsShowHomeButton()
    {
        return true;
    }

    // 设置软键图标显示类型
    void SetSoftkeyType(SoftkeyType eIconType);

    // 获取软键图标显示类型
    SoftkeyType GetSoftkeySoftkeyType();

    // 方便子界面派生以做调整
    virtual void SetRect(const QRect & rect)
    {
        setGeometry(rect);
    }

    //仅提供给EDKUI直接跳转功能使用
    virtual bool DoAction(const QString & strAction);

    virtual bool IsUseTransparency()
    {
        return false;
    }

    // 当全键盘显示时调用的函数
    virtual void OnKeyboardShow() { }

    // 当全键盘隐藏时调用的函数
    virtual void OnKeyboardHide() { }

protected:
    virtual bool OnAction(const QString & strAction)
    {
        return false;
    }

    // 设置窗体类型
    void SetWindowType(EWindowType eWndType)
    {
        m_eWndType = eWndType;
    }

    // 设置为顶层窗体
    void SetTopWnd()
    {
        m_nDlgFlag = m_nDlgFlag | DIALOG_TOPWND;
    }

    // 设置跳过Return To Idle
    void SetRejectReturnToIdle()
    {
        m_nDlgFlag = m_nDlgFlag | DIALOG_REJECT_RETURN_TO_IDLE;
    }

protected:
    virtual void paintEvent(QPaintEvent *);

public slots:
    virtual void OnSoftKeyAction(qSoftButton * pBtn);

protected:
    //窗体类型
    EWindowType             m_eWndType;
    //窗体标志
    int                     m_nDlgFlag;
    // softkey
    CArraySoftKeyBarData    m_SoftkeyData;
    // 显示类型
    SoftkeyType             m_eSoftkeyType;
    // 对应的Title的名字
    QString                 m_strTitle;
    // 窗体位置
    QRect                   m_dlgRect;
    //窗口提示模式命令
    QString             m_strHint;
};

// 顶层窗口改变时候的回调函数
typedef void (*ON_TOPWND_CHANGE_PROC)(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop);

#endif // __BASE_DIALOG__
