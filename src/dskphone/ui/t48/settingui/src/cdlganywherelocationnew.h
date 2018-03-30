#ifndef CDLGANYWHERELOCATIONNEW_H
#define CDLGANYWHERELOCATIONNEW_H

#include "broadsoft/anywhere/include/anywherecommon.h"
#include "cdlgbasesubpage.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"

class QLineEdit;

class CDlgAnyWhereLocationNew : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAnyWhereLocationNew(QWidget * parent = 0);
    ~CDlgAnyWhereLocationNew();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAnyWhereLocationNew();
    }

public:
    //号码已存在时的提示
    void NotifyLocationAlreadyExist();

    //获取号码
    QString GetNumber();

    //获取页面数据
    SAYWLocation GetPageData();

    //////////基类纯虚函数 该界面未用///////////////
    virtual bool LoadPageData();

    virtual bool IsStatusChanged();
    ////////////////////////////////////////////////

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid();

    virtual void InitData();

    virtual void
    OnCustomBack();            // 用于自定义退出界面操作（只回退一个界面）

private:
    QLineEdit * m_pEditName;
    QLineEdit * m_pEditNumber;

};

#endif // CDLGANYWHERELOCATIONNEW_H
