#ifndef CDLG_FORWARDSELECTIVE_H
#define CDLG_FORWARDSELECTIVE_H

#include "config.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include "uikernel/qcheckbox.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>


class CDlgForwardSelective : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgForwardSelective(QWidget * parent = 0);
    virtual ~CDlgForwardSelective();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgForwardSelective();
    }

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

public:

    //收到logic层返回的消息
    void OnReceiveMsg(msgObject & objMessage);

    //获取FWDNotReachData数据
    void OnGetForwardSelectiveData();

    virtual bool LoadPageData();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual bool IsStatusChanged()
    {
        return true;
    }

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

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

    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

private:
    void GetPageData();

private:
    bool                    m_bIsInLoad;
    int                     m_iAccountId;
    SForwardSelectiveData   m_sFsInfo;
};
#endif
#endif // CDLGMOBILITY_H
