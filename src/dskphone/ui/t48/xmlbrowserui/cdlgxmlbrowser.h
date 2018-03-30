#ifndef CDLGXMLBROWSER_H
#define CDLGXMLBROWSER_H

#include <QtGui/QDialog>
#include <ETLLib.hpp>
#include <vector>
#include "uimanager/basedialog.h"
#include "uikernel/quipushbutton.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"

#include "keymap.h"
#include "globalinputhook/inputhook_include.h"


// 前向声明
class CXMLBrowserUIFrame;
class qSoftButton;

class CDlgXMLBrowser : public CBaseDialog
{
    Q_OBJECT

public:
    CDlgXMLBrowser(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~CDlgXMLBrowser();

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 是否显示softkey
    virtual bool IsShowSoftkey();
    // 是否显示Title
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo);

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    bool MyKeypadHook(int nKeyCode, bool bPress);

    // 返回该窗口指针
    static CBaseDialog * CreateInstance();

    // 更新界面
    void UpdateUI(const XMLLogicToUIData & objData);

    // 事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 重写大小改变事件
    void resizeEvent(QResizeEvent * pEvent);

    // 逻辑层和UI层之间的softkey数据转换
    yl::string ToUISoftkeyAction(const SoftkeyParam & strLogicAction);
    SoftkeyParam ToLogicSoftkeyAction(const yl::string & strUIAction);

    virtual bool IsShowDsskey(bool & bExpend)
    {
        bExpend = false;
        return true;
    }

    virtual bool OnAction(const QString & strAction);

    // 设置背景
    void SetBackground();

    bool ProcessDsskey(int iIndex);

    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

    // 子界面响应Home按钮的操作
    virtual void OnHomeButtonClick();

    virtual bool IsShowBackButton();

    virtual bool IsShowHomeButton();

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();

private:
    // 初始化数据
    void InitData();

    // 更新softkey数据
    void UpdateSoftkey(VecSoftkeyParam & refSoftkeyData);

    void FilterIMESoftkey();

public slots:
    // 更新softkey数据
    void UpdateSoftkey();

protected Q_SLOTS:
    void OnSoftKeyAction(qSoftButton * pBtn);

    void OnXBFrameQuit();

private:
    CXMLBrowserUIFrame * m_pXBUIFrame;

    QPixmap             m_pmBG;
};

// 键盘回调函数
bool XMLKeypadHookCallbackFun(int nKeyCode, bool bPress);

#endif // CDLGXMLBROWSER_H
