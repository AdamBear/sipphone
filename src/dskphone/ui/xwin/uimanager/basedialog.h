#ifndef __BASE_DIALOG__
#define __BASE_DIALOG__

#include "dialogviewcontroller.h"
#include "dialogfactory.h"

#include "xwindow/xWindow.h"
#include "messagebox/messageboxcallback.h"

///////////////////////////////////////////////////////
//使用时，定义的界面应同时继承xView和CBaseDialog，如下所示
//class CHomeScreen
//  : public xLinearLayout
//  , public CBaseDialog
//在CBaseDialog创建后，基类会去调用createWindow，因此不需要再手动调用
///////////////////////////////////////////////////////

// 将类名和创建类实例的函数加入到Factory Map中
#define REGISTER_DLG(className) \
    g_DlgFactory.RegisterDialog(#className, className::CreateInstance);

// 顶层窗口改变信息数据
struct CTopWndChangeInfo
{
    bool            m_bBeforeTopWndChange;  // 标示在顶层窗口改变前还是改变后
    CBaseDialog  *  m_dlgPreTop;            // 顶层窗口改变前的界面
    CBaseDialog  *  m_dlgNewTop;            // 顶层窗口改变后的界面

    CTopWndChangeInfo()
        : m_bBeforeTopWndChange(false)
        , m_dlgPreTop(NULL)
        , m_dlgNewTop(NULL)
    {

    }

    CTopWndChangeInfo(bool bBeforeTopWndChange,
                      CBaseDialog * dlgPreTop,
                      CBaseDialog * dlgNewTop)
        : m_bBeforeTopWndChange(bBeforeTopWndChange)
        , m_dlgPreTop(dlgPreTop)
        , m_dlgNewTop(dlgNewTop)
    {

    }

    ~CTopWndChangeInfo()
    {

    }
};
// 顶层窗口改变时候的回调函数
typedef void(*ON_TOPWND_CHANGE_PROC)(const CTopWndChangeInfo & topWndChangeInfo);

///////////////////////////////////////////

// 所有小窗口的基类，所有模块的小窗口必须从该类继承
// 以实现统一的Init Uninit接口
class CBaseDialog
    : public CDialogViewController
    , public CMsgBoxCallBackAction
{

public:
    CBaseDialog(xView * pDialogView, EWindowType eWndType);
    virtual ~CBaseDialog();

    ///////////////UIManager//////////////////////
    void SetDialogName(const yl::string & strDialogName);
    const yl::string & GetDialogName() const;

    // 初始化
    virtual void Init() = 0;
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit() = 0;

    virtual void UpdateWnd(bool bUIReturn = false) {}

    virtual void RefreshTitle() {};

    virtual void RefreshSoftkey();

    // 顶层改变
    // | bFocused | [in] 为将要改变的顶层状态. true：界面要成为顶层界面，false为从处于顶层的状态退出
    virtual void OnFocusChanged(bool bFocused) {}

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle()
    {
        return false;
    }

    //回到Idle前的处理函数
    virtual void OnReturnToIdle() {}

    virtual void OnReturnToIdleAction() {}

    //获取窗体类型
    EWindowType GetWindowType() const
    {
        return m_eWndType;
    }
    ////////////////////////////////////////////////////

    ////////////////////音量条///////////////////////
    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix);

    //////////////////////////////////////////////////
    virtual bool IsShowDsskey(bool & bExpend);

    virtual bool OnDsskeyLengthChange()
    {
        return false;
    }

    // Dsskey显示类型
    virtual DSSKEY_DISPLAY_MODE GetDsskeyDisplayMode()
    {
        return DDM_HIDE;
    }

    virtual bool IsHideOtherWnd()
    {
        return true;
    }

    // 是否重叠属性，默认为true;
    virtual bool GetOverlap()
    {
        return true;
    }
    ///////////////////////////////////////////////

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);
    ///////////////////////////////////////////////

    ///////////////处理按键事件//////////////////
    //一般由uimanager钩回给顶层窗口处理
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress)
    {
        return false;
    }
    ///////////////////////////////////////////////

    ////////////////处理弹出框回调///////////////
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) {}
    /////////////////////////////////////////////////

    ////////////////////////////////////////////////
    //该窗口是否是可以用于入栈的大窗口
    virtual bool IsTopWnd()
    {
        return true;
    }
    ////////////////////////////////////////////////

    ////////////////////////////////////////////////
    virtual xView * GetFocusWidget()
    {
        return NULL;
    }
    ////////////////////////////////////////////////

    //////////界面能否进入屏保：默认为false/////////
    virtual bool IsAllowScreenSaverCount()
    {
        return false;
    }

protected:
    // 设置窗体类型
    void SetWindowType(EWindowType eWndType)
    {
        m_eWndType = eWndType;
    }

    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

protected:
    //窗体类型
    EWindowType             m_eWndType;

    yl::string                      m_strDialogName;
};

#endif // __BASE_DIALOG__
