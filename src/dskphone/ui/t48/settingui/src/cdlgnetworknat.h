#ifndef _CDLG_NETEORK_NAT_H_
#define _CDLG_NETEORK_NAT_H_

#include "cdlgbasesubpage.h"
#include "setting/include/common.h"

class qCheckBox;

class CDlgNetworkNAT : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetworkNAT();
    }

    CDlgNetworkNAT(QWidget * parent = 0);
    ~CDlgNetworkNAT();

public:
    virtual void InitData();
    virtual bool LoadPageData();
    virtual bool IsStatusChanged();
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);
    virtual bool SavePageData();

protected:
    bool GetPageData(NetworkStunData & refData);

private:
    qCheckBox * m_pCheckStunStatus;
    QLineEdit * m_pEditStunServerAddr;
    QLineEdit * m_pEditStunServerPort;
};

#endif  //_CDLG_NETEORK_NAT_H_
