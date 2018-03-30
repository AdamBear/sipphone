#ifndef CDLGALWAYSFORWARD_H
#define CDLGALWAYSFORWARD_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "cforworduilogic.h"

class qCheckBox;

class CDlgAlwaysForward
    : public CDlgBaseFwdPageUI
{
    Q_OBJECT

public:
    explicit CDlgAlwaysForward(QWidget * parent = 0);
    virtual ~CDlgAlwaysForward();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAlwaysForward();
    }

public:

    //从V层读取当前界面的值
    virtual bool GetPageData(ForwardData * pData, FWD_TYPE eFwdType);

    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //通过C层获取所有可用帐号的配置数据，与GetPageData值比较，有改动时返回true
    bool IsAnyLineStatusChanged();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    virtual bool OnAction(const QString & strAction);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    // 重写接受按钮消息
    virtual void OnExtraAction(int eActionType);
    // 写入联系人数据
    void writeData(QString name, QString number);

private:
    //将所有可用帐号的的Always的属性全部设置成当前页面的属性
    bool SavePageDataToAllLine();

private:
    //设置Forward Active对应的开关
    qCheckBox * m_qCheckBox;

    QLineEdit * m_pEditTarget;
    QLineEdit * m_pEditOnCode;
    QLineEdit * m_pEditOffCode;
};

#endif // CDLGALWAYSFORWARD_H
