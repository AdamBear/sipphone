#ifndef CDLGMOBILITY_H
#define CDLGMOBILITY_H

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include "uikernel/qcheckbox.h"

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>


class CDlgMobility : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMobility(QWidget * parent = 0);
    ~CDlgMobility();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMobility();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:

    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取FWDNotReachData数据
    void GetMobilityData();

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual void InitData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 设置子页面后的操作
    virtual void AfterSetSubPage(void * pData);

    // 处理密码弹窗
    bool ProcessMsgBoxPassword(int nLineId);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual void ExtraDialogInitData();

public Q_SLOTS:
    // 密码框提交响应函数
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // 密码框取消响应函数
    void OnCancelInputXSIPassword();

private:
    qCheckBox * m_pCbMobility;
    QComboBox * m_pCbRinging;
    qCheckBox * m_pCbDiverseInhibite;
    qCheckBox * m_pCbRequireConfirm;
    qCheckBox * m_pCbAlertClick;
    qCheckBox * m_pCbAlertPage;
    qCheckBox * m_pCbCallCenter;
    QLineEdit * m_pEditNumber;
    int        m_iAccountId;

    //表示是加载还是保存
    bool m_bIsInLoad;

};

#endif // CDLGMOBILITY_H
