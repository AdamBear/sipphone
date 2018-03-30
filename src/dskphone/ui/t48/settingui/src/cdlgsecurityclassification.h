#ifndef CDLGSECURITYCLASSIFICATION_H
#define CDLGSECURITYCLASSIFICATION_H

#include "cdlgbasesubpage.h"

class CDlgSecurityClassification : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgSecurityClassification(QWidget * parent = 0);
    ~CDlgSecurityClassification();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSecurityClassification();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    //界面初始化
    void InitPageData();
    virtual void OnReShow();
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);
};

#endif // CDLGSECURITYCLASSIFICATION_H
