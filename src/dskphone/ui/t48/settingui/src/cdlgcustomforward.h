#ifndef CDLGCUSTOMFORWARD_H
#define CDLGCUSTOMFORWARD_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include <list>

class CDlgCustomForward : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgCustomForward(QWidget * parent = 0);
    ~CDlgCustomForward();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgCustomForward();
    }

    //从V层读取当前界面的值
    bool GetPageData(int & nIndex);

    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

protected:
    virtual void showEvent(QShowEvent * e);

private:
    virtual bool OnAction(const QString & strAction);

private:
    struct CustomForwardData
    {
        int        iAccountID;
        QLabel  *  pLabel;
        int        nIndex;
    };

    typedef std::list<CustomForwardData>  ListForwardData;
    typedef ListForwardData::iterator     IterForwardData;
private:
    ListForwardData m_listForwardData;

    bool            m_bEnableAccount;
};

#endif // CDLGCUSTOMFORWARD_H
