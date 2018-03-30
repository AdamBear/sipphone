#ifndef DLGRINGTONE_H
#define DLGRINGTONE_H

#include "cdlgbasesubpage.h"

class CDlgRingtone : public CDlgBaseSubPage
{
public:
    CDlgRingtone(QWidget * parent = 0);
    ~CDlgRingtone(void);

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgRingtone();
    }

public:
    // 查看页面状态是否改变
    virtual bool IsStatusChanged()
    {
        return false;
    }
    // 保存页面数据
    virtual bool SavePageData()
    {
        return true;
    }

    virtual bool LoadPageData()
    {
        return true;
    }

    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool OnAction(const QString & strAction);

private:
    //页面数据初始化
    void InitData();

private:
    std::vector<int> m_vecAccount;
};

#endif // DLGRINGTONE_H
