#ifndef CDLGMULTIFORWARDSETTING_H
#define CDLGMULTIFORWARDSETTING_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>


class CDlgMultiForward : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMultiForward(QWidget * parent = 0);
    ~CDlgMultiForward();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMultiForward();
    }

public:

    //从V层读取当前界面的值
    bool GetPageData(ForwardData * pData);

    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    virtual bool OnAction(const QString & strAction);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

private slots:
    void OnRefreshSoftKey(int nIndex);

private:

    QLineEdit * m_pEditForward;     // alwaysForward  号码

    int        m_iKeyIndex;         // 当前配置的Dsskey ID
};

#endif // CDLGMULTIFORWARDSETTING_H
