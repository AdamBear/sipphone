#ifndef _DLGDIALINGUI_H_
#define _DLGDIALINGUI_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgdialingbase.h"
#include "numberpad.h"

// 前向声明
class qPswLineEdit;
class CSearchTable;
class qUIPushButton;
class DataDial;

class CDlgDialingUI: public CDlgDialingBase
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDialingUI();
    }

public:
    CDlgDialingUI();
    virtual ~CDlgDialingUI(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //更新窗口
    virtual void UpdateWnd();

    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);


    //事件过滤器
    bool eventFilter(QObject * pObject, QEvent * pEvent);

    //给界面设置数据
    virtual void SetTalkData(DataLogic2UIBase * pData);


public:
    //拨号界面使用
    // 是否允许切换Session
    virtual bool IsChangeSession();
    //获取号码和联系人
    virtual bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);
    //获取当前 LineEidt 编辑框中的数据  不强制输出预编辑字符
    virtual yl::string GetLineEditData();

    //把联系人控件中的联系人信息设置到编辑框中  参数bTextChange为是否要触发OnTextChange 函数  默认不触发
    virtual void SetLineEditDataBySearchTable();
    //把编辑框中的预编辑字符输入到编辑框中  参数bTextChange为是否要触发OnTextChange 函数  默认不触发
    virtual void InputLineEditData();
    //联系人控件是否有选中联系人
    virtual bool IsSearchTableSelectItem();
    //获取当前保存的 转接通话ID
    virtual int GetShttleCallID();
    //联系人控件 往上翻一页
    virtual void OnKeyEventUp();
    //联系人控件 往下翻一页
    virtual void OnKeyEventDown();
    //数字键处理
    virtual bool OnUIDigitPress(int iKey);
    //搜索联系人
    virtual void SearchContact(const QString & strSearchData);

    //搜索结果返回
    virtual LRESULT DealSuperSearchMessage(msgObject & refMessage);


    ////获取拨号传递给Logic数据
    //void GetDataToLogic(DataDial2Logic& objDialData, bool bInputEditText = true);

protected:
    //设置Softkey数据Action值
    void RefreshPickBtn(DataDial * pDialData);

    // 清空搜索栏
    void ClearSearchTable();

    //设置穿梭列表
    void SetShuttleList(YLList<CCallInfo *> listShuttleCall);

    // 显示DPickup按钮
    void ShowDPickupBtn(bool bShow);

    // 显示GPickup按钮
    void ShowGPickupBtn(bool bShow);

    //显示Retrieve按钮
    void ShowRetrieveParkBtn(bool bShow);

    //获取功能按钮左位移
    QRect GetFunctionBtnRect();

    // 重设IME按钮的文字
    void ResetIMEText();

protected:
    //构造函数调用初始化数据
    void InitData();
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    //获取长按数字键
    yl::string GetStringByDigitKeyLongPress(char chKey);
#endif

protected:
    void OnSuperSearch(const QString & sText);

    // 当搜索框失去焦点
    void OnSearchTabNoFocus();

protected slots:
    // Text变化的时候响应的槽
    virtual void OnTextChanged(const QString & sText);
    // 当DialPad点击的时候
    void OnBtnClicked(const QString & strAction);
    // 处理点击Item的事件
    void OnSearchItemClicked(const QString & strNumber, const QVariant & varData);

    //显示Initiation按钮
    void ShowInitiationBtn();
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    //长按数字键处理
    void OnExtendedDigitKeyPress();
#endif
protected:
    yl::string m_strSaveDisplayName;        //暂时保存的联系人的名称
    yl::string m_strSaveNumber;             //暂时保存的联系人号码

protected:
    qPswLineEdit  * m_pPswLineEdit;

    // 搜索控件
    CSearchTable  * m_pSearchTable;

    // 删除按钮
    qUIPushButton * m_pBtnDelete;

    // DPickup
    qUIPushButton * m_pBtnDPickup;

    // GPickup
    qUIPushButton * m_pBtnGPickup;

    // RetrieveBtn
    qUIPushButton * m_pBtnRetrieve;

    // Call Initiation
    qUIPushButton * m_pBtnInitiation;

    KeyPadDialPad         m_dialPad;

    // 长按数字键计时器
    QTimer m_timerLongPress;

    ////拨号界面的类型
    //DIAL_UI_TYPE  m_eDialUIType;
};


#endif
