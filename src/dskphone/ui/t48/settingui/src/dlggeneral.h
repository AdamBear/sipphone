#ifndef DLGGENERAL_H
#define DLGGENERAL_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "setting/src/advancedcallfeaturecontroller.h"

class qCheckBox;

// 表示未启用kes as send功能，值与combobox的索引值相对应
// #define DISABLED_SYMBOL 0
// 表示启用#为key as send功能，值与combobox的索引值相对应
// #define CROSSHATCH_SYMBOL 1
// 表示启用*为key as send功能，值与combobox的索引值相对应
//#define STAR_SYMBOL 2

////////////////////////////////////////////////////////////////////////////////
class CDlgGeneral : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgGeneral(QWidget * parent = 0);
    ~CDlgGeneral();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgGeneral();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();

private:
    //界面初始化
    void InitPageData();
    //拷贝页面数据
    void CopyPageData(AdvancedCallFeatureGeneralData & dest_GeneralPageData,
                      const AdvancedCallFeatureGeneralData & src_GeneralPageData);
    //比较页面前后数据
    bool ComparePageData(const AdvancedCallFeatureGeneralData & before_GeneralPageData,
                         const AdvancedCallFeatureGeneralData & after_GeneralPageData) const;
    //取得当前页面数据
    AdvancedCallFeatureGeneralData & GetCurrentPageData(AdvancedCallFeatureGeneralData &
            current_GeneralPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(AdvancedCallFeatureGeneralData & current_GeneralPageData);


    /*private:*/
public:
    AdvancedCallFeatureGeneralData m_generalPageData;
    qCheckBox   *   m_qCheckBoxHistoryRecord;
    QComboBox   *   m_pCmbBoxKeyAsSend;
};

#endif // DLGGENERAL_H
