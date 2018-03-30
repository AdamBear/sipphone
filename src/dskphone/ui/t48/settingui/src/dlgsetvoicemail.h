#ifndef DLGSETVOICEMAIL_H
#define DLGSETVOICEMAIL_H

#include <QWidget>
#include "cdlgbasesubpage.h"
#include <vector>

class CDlgSetVoiceMail : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgSetVoiceMail(QWidget * parent = 0);
    ~CDlgSetVoiceMail();
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgSetVoiceMail();
    }

public:
    // 读取页面数据
    virtual bool LoadPageData();
    // 查看页面状态是否改变
    virtual bool IsStatusChanged();
    // 保存页面数据
    virtual bool SavePageData();

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
    //根据账号id设置编辑框焦点
    void SetVoiceMailNumberFocus(int nAccountId);

    //判断输入是否合法
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual void SetDefaultIndex(int nIndex = 0, bool bEnter = false);

private:
    void InitData();

private:
    std::vector<QLineEdit *> aEditVoiceMailNumber;
};

#endif // DLGSETVOICEMAIL_H
