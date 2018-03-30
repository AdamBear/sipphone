#ifndef CDLGIPMODESTATUS_H
#define CDLGIPMODESTATUS_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include "uikernel/qscrolltextlabel.h"
#include <QDialog>

class CDlgIPModeStatus : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgIPModeStatus(QWidget * parent = 0);
    ~CDlgIPModeStatus();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgIPModeStatus();
    }

public:
    //加载数据
    virtual bool LoadPageData();

    //获取数据状态是否改变
    virtual bool IsStatusChanged();

    //保存数据
    virtual bool SavePageData();

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);

private:
    //界面初始化
    void InitPageData();

    //添加内容
    void AddPageContent();
private:
    enum IP_STATUS_PAGE_TYPE
    {
        IP_STATUS_IPV4 = 0,
        IP_STATUS_IPV6 = 1,
    };

private:
    QLabel   *  m_pLabWANType;
    qScrollTextLabel    *   m_pLabWANIP;
    qScrollTextLabel    *   m_pLabWANMask;
    qScrollTextLabel    *   m_pLabGateway;
    qScrollTextLabel    *   m_pLabPriDNS;
    qScrollTextLabel    *   m_pLabSecDNS;
    IP_STATUS_PAGE_TYPE m_ePageType;
};

#endif // CDLGIPMODESTATUS_H
