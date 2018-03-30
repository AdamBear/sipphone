#ifndef DLGSTATUSNETWORK_H
#define DLGSTATUSNETWORK_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class CDlgStatusNetwork : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgStatusNetwork(QWidget * parent = 0);
    ~CDlgStatusNetwork();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgStatusNetwork();
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

    //状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);

    // 过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

private:
    virtual bool OnAction(const QString & strAction);
    void SetViewByIPMode(int eMode);

private:
#ifndef _T4X
    QLabel   *  m_pLabLanType;
    QLabel   *  m_pLabLanIP;
    QLabel   *  m_pLabLanMask;
#endif
    QLabel   *  m_pLabIPMode;
    QLabel   *  m_pLabMAC;
    QLabel   *  m_pLabWifiMAC;
    int         m_iIPMode;
};

#endif // DLGSTATUSNETWORK_H
