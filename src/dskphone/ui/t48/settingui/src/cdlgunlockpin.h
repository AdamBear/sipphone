#ifndef CDLG_UNLOCK_PIN_H
#define CDLG_UNLOCK_PIN_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

namespace
{
enum
{
    ENTER_CHANGE_PHONE_LOCK_PIN_BY_NORMAL = 0,
    ENTER_CHANGE_PHONE_LOCK_PIN_BY_CHANGE_UNLOCK_INFO,
};
}


class CDlgUnlockPIN : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgUnlockPIN(QWidget * parent = 0);
    ~CDlgUnlockPIN();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgUnlockPIN();
    }

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);
    // 设置子页面后的操作
    virtual void AfterSetSubPage(void * pData);

    virtual bool OnAction(const QString & strAction);

    virtual void UninstallAllIME();

private:
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

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

private:
    PasswordData m_pageData;

    QLineEdit * m_pEditCurrentPassword;
    QLineEdit * m_pEditNewPassword;
    QLineEdit * m_pEditConfirmPassword;

    bool m_bHaveOriginPassword;
    int         m_nEnterType;
};

#endif // CDLGSETPASSWORD_H
