#ifndef CDLGREMOTEOFFICENEW_H
#define CDLGREMOTEOFFICENEW_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

class qCheckBox;

class CDlgRemoteOfficeNew : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgRemoteOfficeNew(QWidget * parent = 0);
    ~CDlgRemoteOfficeNew();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgRemoteOfficeNew();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:

    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取RemoteOffice数据
    void GetRemoteOfficeData();

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 窗口初始化完成后，需要额外的操作，比如一些阻塞或同步的动作
    virtual void ExtraDialogInitData();

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    void OnCancelInputXSIPassword();


protected:
    // 检查鉴权
    bool ProcessMsgBoxPassword(int nLineId);

private:
    qCheckBox * m_pCheckStatus;
    QLineEdit * m_pEditNumber;
    int        m_iAccountId;

    //表示是加载还是保存
    bool m_bIsInLoad;
};

#endif // CDLGREMOTEOFFICENEW_H
