#ifndef DLGSTATUSPHONE_H
#define DLGSTATUSPHONE_H

#include "setting/include/common.h"
#include "uikernel/qscrolltextlabel.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

struct PhoneStatusData;

class CDlgStatusPhone : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgStatusPhone(QWidget * parent = 0);
    ~CDlgStatusPhone();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgStatusPhone();
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
    // 获取当前基于MAC的SSL证书的状态
    bool GetDeviceCertStatus();

private:
    QLabel   *  m_pLabModel;
    QLabel   *  m_pLabHardware;
    QLabel   *  m_pLabFirmware;
    QLabel   *  m_pLabProductID;
    QLabel   *  m_pLabMAC;
    QLabel   *  m_pLabWifiMAC;
    QLabel   *  m_pLabBluetoothMAC;
    QLabel   *  m_pLabCertificate;              // 证书
    QLabel   *  m_pLabAMRLicense;
#if IF_BUG_34374
    qScrollTextLabel * m_pLabCfgVerValue;
#endif

};

#endif // DLGSTATUSPHONE_H
