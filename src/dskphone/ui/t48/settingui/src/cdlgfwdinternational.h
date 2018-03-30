#ifndef CDLGFWDINTERNATIONAL_H
#define CDLGFWDINTERNATIONAL_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class qCheckBox;
struct FWDInternationalData;

class CDlgFWDInternational : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgFWDInternational(QWidget * parent = 0);
    ~CDlgFWDInternational();

public:

    //读取当前界面的值
    //bool GetPageData(* pData);

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgFWDInternational();
    }

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

private:
    //比较页面前后数据
    bool ComparePageData(const FWDInternationalData & beforePageData,
                         const FWDInternationalData & afterPageData) const;
    //取得当前页面数据
    FWDInternationalData & GetCurrentPageData(FWDInternationalData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(FWDInternationalData & currentPageData);

private:
    //设置开关对应的滑块
    QComboBox * m_pCmbBoxFWDInternational_Active;
};

#endif // CDLGFWDINTERNATIONAL_H
