#ifndef _DLGDIALING_BASE_H_
#define _DLGDIALING_BASE_H_

//mod-Talk
// #include "talk/talklogic_inc.h"
#include <ETLLib/ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"

//mod-this(TalkUI)
#include "dlgnormaltalkbase.h"
#include "numberpad.h"

class CTransferNumSelector;

class CDlgDialingBase : public CDlgNormalTalkBase
{
    Q_OBJECT

public:
    //构造函数
    CDlgDialingBase();

    //析构函数
    virtual ~CDlgDialingBase();

public:
    //设置通话数据内容
    virtual void SetTalkData(DataLogic2UIBase * pData);

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //更新窗口
    virtual void UpdateWnd();

    // 是否显示Title
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo);

    virtual void AfterShowTitle();


    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;

    //是否显示加密图标,rectEncrypt hold the location of encrypt label
    virtual bool IsShowEncryptTip(void * pData, QRect & rectEncrypt);

    //输入所有输入框内容
    virtual bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);

    // 子界面响应Back按钮的操作
    virtual void OnBackButtonClick();

    // 焦点切换
    virtual void OnFocusChanged(bool bFocus);

    //处理UI事件
    virtual bool OnUIAction(const QString & strAction);

    // 事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 是否允许切换Session
    virtual bool IsChangeSession();

    // 预拨号界面的TIP显示区域默认都被使用
    virtual bool IsShowTip()
    {
        return true;
    }

    //处理消息函数
    virtual LRESULT OnMessage(msgObject & refMessage);

    ////处理超级搜索消息函数
    //LRESULT DealSuperSearchMessage(msgObject& refMessage);

    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();


    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey, bool bEXP = FALSE);
    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //普通拨号界面通道键处理
    virtual bool OnNormalChannelKeyPress(int iKey);
    //Forward 状态下 按键处理
    virtual bool OnForwardChannelKeyPress(int iKey);
    //callpark 状态下 按键处理
    virtual bool OnCallParkChannelKeyPress(int iKey);

    //实际判断硬按键是否播放按键音
    bool IsNeedToPlayKeyTone(yl::string strEditContent, int iKey);

    // 判断是否需要播放按键音
    virtual bool IsNeedToPlayKeyTone(int iKey);

    int GetLineId()
    {
        return m_iCurrentAccountId;
    }

    CTransferNumSelector * GetTransferNumSelector();

    void ReleaseSelector();

protected:

    virtual bool IsSendDigitKey(yl::string strEditContent, int iKey);


    // 判断Softkey是否需要播放按键音
    virtual bool IsSoftkeyNeedToPlayKeyTone(yl::string strSoftkeyAction);




protected:

    virtual void GetSoftkeyAction(CArraySoftKeyBarData & objSoftkeyData, DataLogic2UIBase * pData = NULL) {}

    // 初始化页面的数据
    void InitData();

    //// 重载showEvent事件
    //virtual void showEvent(QShowEvent *event);

    // 是否运行自定义softkey
    bool IsAllowDefineSoftkey();

    // 是否显示切换话路的滚动条
    virtual bool IsShowSessionScrollBar()
    {
        return false;
    }

    // 是否可以切换默认话路的按钮
    bool IsCanSelectLineBtn();

protected slots:
    // Text变化的时候响应的槽
    virtual void OnTextChanged(const QString & sText);
    //搜索延时定时器响应槽
    void OnSearch();
    // 延迟处理文字变化事件
    void DelayProcessTextChanged();

public slots:
    //因为在CallMananger 是直接调用setGeometry的 所以这边槽函数中需要给dlgdialingwidget 设置位置
    void resizeEvent(QResizeEvent *);

private:
    //拨号界面按上下键选择搜索控件中的联系人时改变Softkey（第一个Pool改为Send）
    void RefreshSoftKey();

protected:
    //------------------拨号界面使用------------------

    //获取当前 LineEidt 编辑框中的数据  不强制输出预编辑字符
    virtual yl::string GetLineEditData() = 0;

    //把联系人控件中的联系人信息设置到编辑框中  参数bTextChange为是否要触发OnTextChange 函数  默认不触发
    virtual void SetLineEditDataBySearchTable() = 0;
    //把编辑框中的预编辑字符输入到编辑框中  参数bTextChange为是否要触发OnTextChange 函数  默认不触发
    virtual void InputLineEditData() = 0;
    //联系人控件是否有选中联系人
    virtual bool IsSearchTableSelectItem() = 0;
    //获取当前保存的 转接通话ID
    virtual int GetShttleCallID() = 0;
    //联系人控件 往上翻一页
    virtual void OnKeyEventUp() = 0;
    //联系人控件 往下翻一页
    virtual void OnKeyEventDown() = 0;
    //UI处理 数字键按下
    virtual bool OnUIDigitPress(int iKey) = 0;

    //搜索联系人
    virtual void SearchContact(const QString & strSearchData) = 0;

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    //搜索联系人结果返回
    virtual LRESULT DealSuperSearchMessage(msgObject & refMessage);

public:
    //执行 UI 按钮按下操作
    virtual void OnUIActionPress(yl::string strAction);
    ////Widget 回调该逻辑使用
    //virtual void OnTalkWidgetAction(TALKWIDGET_CALLBACK_TYPE talkWidgetCallBackType);

public:
    //拨号界面的类型
    DIAL_UI_TYPE    m_eDialUIType;

#if IF_FEATURE_DIAL_ON_HOLD
    bool m_bAllowTran;          //DIAL_UI_ON_HOLD 状态下 是否允许transfer
#endif

    // 是否是点击超级搜索进行Transfer
    bool m_bSearchTransfer;
protected:
    yl::string m_strTitleInfo;

private:
    //当前账号
    int             m_iCurrentAccountId;
    // 第一次文字改变
    bool            m_bFirstTextChange;
    //搜索key
    QString         m_strSearchData;

    //当前通话routine的状态   主要是用于callparkchannelkey处理
    TALK_STATE m_eTalkState;

    CTransferNumSelector * m_pTransferNumSelector;
};

#endif
