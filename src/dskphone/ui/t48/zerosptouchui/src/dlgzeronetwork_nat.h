#ifndef DLGZERONETWORKNAT_H
#define DLGZERONETWORKNAT_H

#include "settingui/src/cdlgbasesubpage.h"
#include "setting/include/common.h"

class qCheckBox;

class CDlgZeroNetwork_NAT : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgZeroNetwork_NAT();
    }

    CDlgZeroNetwork_NAT(QWidget * parent = 0);
    ~CDlgZeroNetwork_NAT();

public:
    virtual void InitData();
    virtual bool LoadPageData();
    virtual bool IsStatusChanged();
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);
    virtual bool SavePageData();

signals:
    void ReLayout(CBaseDialog * parent);

public slots:
    void OnChkBoxEnableNATChanged(bool bOn);

protected:
    bool GetPageData(NetworkStunData & refData);

private:
    qCheckBox * m_pCheckBoxNatStatus;
    QLineEdit * m_pEditStunServerAddr;
    QLineEdit * m_pEditStunServerPort;
};

#endif
