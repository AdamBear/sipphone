#ifndef _DLG_ZERO_TOUCH_H_
#define _DLG_ZERO_TOUCH_H_

#include "settingui/src/dlgsettingui.h"
#include "settingmsgboxmgr.h"

class xEditTextView;
class xTextView;

// ZeroSpTouch页面类型
enum ZeroPageType
{
    ZERO_PAGE_NONE = -1,                //  错误的页面模式
    ZERO_PAGE_MAIN = 0,                 //  主界面

    ZERO_PAGE_NETWORK_SETTING,          //  网络设置界面

    ZERO_PAGE_NOTE_UPDATING,            //  正在更新中
    ZERO_PAGE_NOTE_UPDATE_FAIL,         //  更新新失败
    ZERO_PAGE_NOTE_UPTADE_FINISH,       //  更新完成

    ZERO_PAGE_NOTE_NETWORK_CHECK,       //  网络检查界面
    ZERO_PAGE_NOTE_NETWORK_FAIL,        //  网络异常

    ZERO_PAGE_NOTE_REBOOTING,           //  重启中

    ZERO_PAGE_REDIRECTOR_NOTE,          //  重定向提示界面
    ZERO_PAGE_REDIRECTOR_SUBMIT,        //  重定向鉴权界面
    ZERO_PAGE_REDIRECTOR_FAILED,        //  重定向失败提示界面
    ZERO_PAGE_REDIRECTOR_FINISH,        //  重定向完成提示界面

    ZERO_PAGE_STATUS,                   //  状态查看界面

    ZERO_PAGE_AUTOP,                    //  AutoP
};

enum DELAY_OPERATOR_TYPE
{
    DOT_NONE,
    DOT_NETWORK_FAIL,
    DOT_UPDATE_FAIL,
    DOT_UPDATE_FINISH,
};

enum ZERO_SOFTKEY_TYPE
{
    ZERO_SOFTKEY_TYPE_PROMPT,
    ZERO_SOFTKEY_TYPE_CONTROL,
    ZERO_SOFTKEY_TYPE_CANCEL,
};

class CDlgZeroTouch : public CDlgSettingUI, public CAsyncOperateAction, public xTimerHandler
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgZeroTouch();
    virtual ~CDlgZeroTouch();
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual void AsyncOperateCallBack();
    virtual void loadChildrenElements(xml_node & node);
    virtual void RefreshUI(bool bReLoadData = false);
    virtual bool LoadPageData(bool bReLoadData = false);
    virtual bool onKeyPressedEvent(int key);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    //回到Idle前的处理函数
    virtual void OnReturnToIdle();
    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();

    // 设置页面的类型
    void SetPageType(ZeroPageType ePageType);
    // 设置页面的类型
    void SetPageType(const yl::string & strPageAction);
    // 将相应的页面Action转换为Type
    ZeroPageType ActionToType(const yl::string & strPageAction);
    // 刷新界面
    void RefreshPage();

private:
    void SetSoftkeyType(ZERO_SOFTKEY_TYPE sType);
    void SetRemainPrompt();
    void SetPrompt(const yl::string & strPromp);
    // 开启倒计时
    void StartRemain();
    // 停止倒计时
    void StopRemain();
    void OnRemainTimer(xThreadTimer* timer, LPVOID pExtraData);
    // 延时进行某些操作
    void DelayOperator(DELAY_OPERATOR_TYPE eType);
    // 延时操作后的释放资源等操作
    void OnDelayOperator(xThreadTimer* timer, LPVOID pExtraData);
    // 网络异常界面超时相应槽
    void OnNetworkFailPageTimeout();
    // 处理OK按键
    bool ProcessOK();


protected:
    ZeroPageType        m_ePageType;    // 页面的模式
    DELAY_OPERATOR_TYPE m_eDelayType;
    int                 m_nRemainTime;
    xTextView     *     m_pTxtPrompt;
    yl::string          m_strPageAction;
    xThreadTimer        m_timerRemain;
    xThreadTimer        m_timerDelayOperate;
};

#endif //_DLG_ZERO_TOUCH_H_
