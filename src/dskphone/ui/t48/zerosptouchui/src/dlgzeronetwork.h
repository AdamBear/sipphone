#ifndef CDLGZERONETWORK_H
#define CDLGZERONETWORK_H

#include "settingui/src/cdlgbasesubpage.h"
#include "zerolistdelegate.h"

class CDlgZeroNetworkIPMode; //替换
class CDlgZeroNetwork_VLAN;
class CDlgZeroNetworkX_Mode;
class CDlgZeroNetwork_VPN;
class DlgZeroNetwork_VLANDHCP;
class CDlgZeroNetwork_LLDP;
class CDlgZeroNetwork_CDP;
class CDlgZeroNetwork_NAT;
class CWndPage;
class CBaseDialog;

class CDlgZeroNetwork : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    explicit CDlgZeroNetwork(QWidget * parent = NULL);
    ~CDlgZeroNetwork(void);

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

public:
    virtual bool LoadPageData();
    virtual bool SavePageData();

    virtual void Init();
    virtual void Uninit();

    bool SaveNetworkData();

    void ClearFocusMark();

    // 控件判空
    bool JudgetWgtNull();

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();

signals:
    void SoftkeyRefresh();

protected slots:
    //重新布局
    void DoReLayout(CBaseDialog * parent);

    //ipmode或wanType变化时调整布局,iSelectIndex [in] : 指定的布局后选中的行
    void DoReLayout(CBaseDialog * pDlg, int iSelectIndex);

    void DoFocusChanged(int nIndex);

private:
    bool CheckInputData();
    bool IsStatusChanged();

    void SetErrorFocus(QLineEdit * pLineEdit);

    void SetSoftKeyBarData();

    void ClearWgtpairData();
    // 设置子页面的SoftKey
    //virtual bool GetSoftkeyData(CArraySoftKeyBarData& objWndData);

    void GetWidgetPair(const yl::string & strID, CDlgBaseSubPage * pSubPage);

    void GetNetworkWidgetPair();

private slots:
    void OnRefreshSoftkey();

private:
    CDlgZeroNetworkIPMode   *   m_pDlgIPMode;
    CDlgZeroNetwork_VLAN    *   m_pDlgVLAN;
    CDlgZeroNetworkX_Mode   *   m_pDlgX_Mode;
    CDlgZeroNetwork_VPN    *    m_pDlgVPN;
    DlgZeroNetwork_VLANDHCP  *  m_pDlgVlanDHCP;
    CDlgZeroNetwork_LLDP    *   m_pDlgLLDP;
    CDlgZeroNetwork_CDP    *    m_pDlgCDP;
    CDlgZeroNetwork_NAT    *    m_pDlgNAT;

    ZeroListDelegate      *     m_pZeroListDelegate;

    int                         m_iFocusLine;
};

#endif  //CDLGZERONETWORK_H
