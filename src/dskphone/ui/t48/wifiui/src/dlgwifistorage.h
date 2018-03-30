#ifndef DLG_WIFISTORAGE_H
#define DLG_WIFISTORAGE_H

#include "settingui/src/cdlgbasesubpage.h"
#include <map>
#include "wifiui/include/wificommondata.h"

class WifiSavedDelegate;
class WifiDetailSelector;

class CDlgWifiStorage : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgWifiStorage();
    }

public:
    CDlgWifiStorage(QWidget * parent = NULL);
    ~CDlgWifiStorage(void);

    void InitData();

    /*************************************** 继承CListSubPage *****************************************/
    virtual bool LoadPageData()
    {
        return true;
    }
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

    virtual bool OnAction(const QString & strAction);

    //窗口显示到界面上时的操作
    virtual void OnReShow();

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    /********************************************************************************************/
    void DoRelayout();

    // 更新连接结果提示语
    void SetConnectResultNote(int iResult, int iCurID = -1);
    void SetDisConnectResultNote(int iResult, int iCurID = -1);
    // 检测Wifi是否存在
    bool IsWifiExist(int iNetID = -1);

    void PopPwdDialog(bool bAuthentication = false);

    void SetWifiPopId(int nPopWifiId);

    // 处理弹出框回调
    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

private Q_SLOTS:
    void OnSubmitNameAndPassword(const yl::string & strName, const yl::string & strPswd);
    void OnSubmitPassword(const yl::string & strPswd);
    void OnCancelPassword();

protected:
    void CreatePageByString(const char * strPageAction, ENUM_WIFI_PAGE_TYPE eType, int iCurId = -1);

private slots:
    void OnItemDetailAction(CListItem * pItem);

private:
    int                 m_iPopWifiID;
    int                 m_iPreWifiID;
    std::map<int, int>  m_mpIDDataIndex;
    bool                m_bConnecting;
    int                 m_iCurDetailID;
    WifiSavedDelegate * m_pWifiSavedDelegate;
#ifdef _T49
    WifiDetailSelector * m_pWifiDetailSelector;
#endif
};

#endif
