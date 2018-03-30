#ifndef DLGUSBBROWSESTORAGE_H
#define DLGUSBBROWSESTORAGE_H

#include "settingui/src/cdlgbasesubpage.h"
#include <QDialog>

class CDlgUSBBrowseStorage : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgUSBBrowseStorage(QWidget * parent = 0);
    ~CDlgUSBBrowseStorage();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgUSBBrowseStorage();
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
    //获取Softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);

private:
    QLabel   *  m_pLabTotal;
    QLabel   *  m_pLabUsed;
    QLabel   *  m_pLabFree;
    QLabel   *  m_pLabAvailableTime;
};

#endif
