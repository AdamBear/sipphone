#ifndef _DLG_CALLMANAGER_BASE_H
#define _DLG_CALLMANAGER_BASE_H

#include "callmanagerstruct.h"
#include "callmanagerbarbase.h"
//#include "softkeybarbase.h"


class CDlgCallManagerBase: public CBaseDialog
{
public:
    CDlgCallManagerBase(void);
    virtual ~CDlgCallManagerBase(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);

public:
    // 是否显示Title
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo);

    virtual void AfterShowTitle();

    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    virtual void UpdateWnd();

    // 返回该窗口指针
    static CBaseDialog * CreateInstance();

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    //设置Softkey数据
    void SetSoftKey(const CArraySoftKeyBarData & objSoftkeyData);

    // 是否显示Softkey，默认认为显示
    virtual bool IsShowSoftkey();

    // 是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

    ////获取窗体类型
    //EWindowType GetWindowType() const;

    //设置Title
    void SetWindowTitle(const QString & strTitle);

    //获取Title
    const QString GetWindowTitle() const;

    // 是否允许鼠标事件通过
    virtual bool AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent);

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    // 子界面响应Back按钮的操作
    virtual void OnBackButtonClick();

    // 子界面响应Home按钮的操作
    virtual void OnHomeButtonClick();

    //callmanager 栏点击事件
    virtual void OnCallManagerBarClick(const yl::string & strUIName);

public:
    //给UI 插入子界面信息  主要是为了 设置 显示的item名称 和选中背景图片等等
    virtual void SetCallMGRListInfo(YLList<CCallMGRInfo *> & listCallMGRInfo);

    //根据信息显示界面
    void EnterPage(CCallMGRInfo * pCallMGRInfo);

    //退出显示界面
    void PutPage(CCallMGRInfo * pCallMGRInfo);

    //设置对话框为空
    void SetPageDlgNULL(CCallMGRInfo * pCallMGRInfo);

    //当前是否是returntoidle 退出
    bool IsReturnToIdle();

protected:
    // 这个函数 需要重载
    //创建softkey bar 实例
    virtual void CreateSoftkeyBar() = 0;

protected:
    CCallMGRInfo * m_pCurrentCallMGRInfo;           //当前显示界面的信息
    //标识是否是returntoidle 动作的
    bool m_bIsReturnToIdle;
    //底排按钮栏
    CCallManagerBarBase * m_pCallManagerBar;

};


#endif
