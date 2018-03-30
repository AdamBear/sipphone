#pragma once

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include <QDialog>

class CDlgSilentAlerting : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgSilentAlerting(QWidget * parent = 0);
    ~CDlgSilentAlerting();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSilentAlerting();
    }

public:

    // 初始化
    virtual void Init();
    // 反初始化
    virtual void Uninit();
    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取FWDNotReachData数据
    void GetSilentAlertingData();

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    // 处理密码弹窗
    bool ProcessMsgBoxPassword(int nLineId);

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    void OnCancelInputXSIPassword();

    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);


private:
    bool       m_bIsSilent;
    int        m_iAccountId;

    //表示是加载还是保存
    bool m_bIsInLoad;


private:
    QComboBox * m_pCmbBoxStatus;
};

