#ifndef CDLGSETPASSWORD_H
#define CDLGSETPASSWORD_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class CDlgSetPassword : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgSetPassword(QWidget * parent = 0);
    ~CDlgSetPassword();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSetPassword();
    }

private:

    //读取当前界面的值
    bool GetPageData(PasswordData * pData);

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    //增加密码过于简单的提示
    bool PopPasswordNotify(const PasswordData * pData);

    void OnConfirmSave();

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

private:
    PasswordData m_pageData;

    QLineEdit * m_pEditCurrentPassword;
    QLineEdit * m_pEditNewPassword;
    QLineEdit * m_pEditConfirmPassword;
};

#endif // CDLGSETPASSWORD_H
