#ifndef CDLGHIDENUMBER_H__
#define CDLGHIDENUMBER_H__

#include "cdlgbasesubpage.h"
class qCheckBox;

class CDlgHideNumber : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgHideNumber(QWidget * parent = 0);
    ~CDlgHideNumber();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgHideNumber();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:
    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 处理密码弹窗
    bool ProcessMsgBoxPassword(int nLineId);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    void OnCancelInputXSIPassword();

protected:
    virtual void ExtraDialogInitData();

private:
    qCheckBox * m_pCheckboxHNStatus;

    //表示是加载还是保存
    bool m_bIsInLoad;

    //当前账号ID
    int m_iAccountId;
};

#endif // CDLGHIDENUMBER_H__
