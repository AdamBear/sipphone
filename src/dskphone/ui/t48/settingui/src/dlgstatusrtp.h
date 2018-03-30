#ifndef DLG_STATUS_RTP_H
#define DLG_STATUS_RTP_H

#include "cdlgbasesubpage.h"

class CDlgStatusRTP : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgStatusRTP(QWidget * parent = 0);
    ~CDlgStatusRTP();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgStatusRTP();
    }

public:
    //获取数据状态是否改变
    virtual bool IsStatusChanged()
    {
        return false;
    }
    //保存数据
    virtual bool SavePageData()
    {
        return true;
    }

    //加载数据
    virtual bool LoadPageData();

    //状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);
};

#endif  //DLG_STATUS_RTP_H
