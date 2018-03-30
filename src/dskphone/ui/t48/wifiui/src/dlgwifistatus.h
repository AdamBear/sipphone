#ifndef DLGWIFISTATUS_H
#define DLGWIFISTATUS_H

#include "settingui/src/cdlgbasesubpage.h"

class WifiSignalItem;
class WifiStatusDelegate;
class QTimer;

class CDlgWifiStatus : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgWifiStatus();
    }

public:
    CDlgWifiStatus(QWidget * parent = NULL);
    ~CDlgWifiStatus(void);

    void InitData();

    /*************************************** 继承CListSubPage *****************************************/
    virtual bool LoadPageData();
    virtual bool IsStatusChanged()
    {
        return false;
    }
    virtual bool SavePageData()
    {
        return true;
    }

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData
#ifdef _T49
                                , QObject * pSignalReceiver
#endif
                               );

    /**************************************** used only in class *******************************************/
protected:
    // clean label text
    void CleanLabelText();

    void RefreshPageData();
    void DoRelayout();

    bool JudgeWidgetNULL();
#ifdef _T49
public slots:
    void OnTimeOut();
#endif

private:
    QLabel       *      m_pLabWirelessStatus;
    QLabel       *      m_pLabProfileName;
    QLabel       *      m_pLabSSID;
#ifdef _T49
    WifiSignalItem   *  m_pSignalItem;
#else
    QLabel       *      m_pLabSignalStrength;
#endif
    QLabel       *      m_pLabChannel;
    QLabel       *      m_pLabAPMacAddress;
    QLabel       *      m_pLabSecurityMode;
    QLabel       *      m_pLabEncryptionType;
    QLabel       *      m_pLabTransmitRate;
    QLabel       *      m_pLabFrequency;
    QLabel       *      m_pLabQos;

    //WifiStatusDelegate*   m_pWifiStatusDelegate;
#ifdef _T49
    QTimer       *      m_pTimer;
#endif
    int                 m_iSignalLevel;
};

#endif
