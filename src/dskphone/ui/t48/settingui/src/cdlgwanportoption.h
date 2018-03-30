#ifndef CDLGWANPORTOPTION_H
#define CDLGWANPORTOPTION_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

class CDlgWanPortOption : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgWanPortOption(QWidget * parent = 0);
    ~CDlgWanPortOption();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgWanPortOption();
    }



protected slots:
    //根据IP Mode选项觉得是否显示Preference(IPv4/IPv6)选择框
    void IsShowPreference(int nIndex);
public:

    //从V层读取当前界面的值
    bool GetPageData(int & nIPMode, int & nPreference);

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
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);

private:
    virtual bool OnAction(const QString & strAction);

private:
    QComboBox * m_pCmbBoxIPMode;
    QComboBox * m_pCmbBoxPreference;
    //新增只有选择（IPV4&IPV6）项时出现的Preference项

    int        m_iIPMode;
    int        m_iPreference;
};

#endif // CDLGWANPORTOPTION_H
