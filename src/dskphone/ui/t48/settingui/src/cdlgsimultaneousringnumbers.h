#ifndef CDlgSimultaneousRingNumbers_H
#define CDlgSimultaneousRingNumbers_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "settinguicommon.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"

class qCheckBox;

class CDlgSimultaneousRingNumbers : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgSimultaneousRingNumbers(QWidget * parent = 0);
    ~CDlgSimultaneousRingNumbers();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSimultaneousRingNumbers();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:

    //获取SimultaneousRing号码列表
    void GetSimultaneousRingNumbers();

    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void InitData();

    // 设置页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 从服务器下载数据
    void DownloadSimData();

    // 窗口初始化完成后，需要额外的操作，比如一些阻塞或同步的动作
    virtual void ExtraDialogInitData();

    void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);
    // 密码框取消响应函数
    void OnCancelInputXSIPassword();

protected:
    virtual void showEvent(QShowEvent * e);
    bool ProcessMsgBoxPassword(int nLineId);

private:
    virtual bool OnAction(const QString & strAction);

private:
    void OnEditSimultaneousRingNumber();
    void OnDeleteAllSimultaneousRingNumber();

private:
    qCheckBox * m_pStatusCheckBox;

    SSimultaneousRing m_RingNumbers;

    int        m_iAccountId;

private:

    EMessageBoxType m_eMessageBoxType;
};

#endif // CDlgSimultaneousRingNumbers_H
