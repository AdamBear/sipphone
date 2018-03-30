#ifndef CDLGFWDNOTREACH_H
#define CDLGFWDNOTREACH_H

#include "globalmodel.h"
#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

class qCheckBox;


class CDlgFWDNotReach : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgFWDNotReach(QWidget * parent = 0);
    ~CDlgFWDNotReach();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgFWDNotReach();
    }


public:

    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取FWDNotReachData数据
    void GetFWDNotReachData();

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

public Q_SLOTS:
    void OnSubmitXSIPassword(const yl::string & strPswd);
    void OnCancelInputXSIPassword();
    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

private:
    qCheckBox * m_pCheckboxCfnrStatus;
    QLineEdit * m_pEditNumber;
    int        m_iAccountId;
#if IF_BUG_22686
    //表示是加载还是保存
    bool m_bIsInLoad;
#endif
};

#endif // CDLGFWDNOTREACH_H
