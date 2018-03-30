#ifndef DLG_WIFIBASE_H
#define DLG_WIFIBASE_H

#include "settingui/src/cdlgbasesubpage.h"
#include <map>
#include "wifiui/include/wificommondata.h"
#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/wifidefines.h"
#endif

class qCheckBox;
class WifiDelegate;
enum SETTING_ITEM_TYPE;
class qMultiPicAnimation;
class QTimer;

class CDlgWifiBase : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgWifiBase();
    }

public:
    CDlgWifiBase(QWidget * parent = NULL);
    ~CDlgWifiBase(void);

    void InitData();

    void ReSetCurOffSet();

    // 处理消息
    LRESULT OnInterWifiBaseUIMessage(msgObject & refMessage);
    /********************************** 继承自CDlgBaseSubPage ***********************************/
public:
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData
#ifdef _T49
                                , QObject * pSignalReceiver
#endif
                               );

    virtual void Init();
    virtual void Uninit();

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();
    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged()
    {
        return false;
    }
    virtual bool SavePageData()
    {
        return false;
    }

    //窗口显示到界面上时的操作
    virtual void OnReShow();

    virtual bool OnAction(const QString & strAction);

    // 处理弹出框回调
    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    /********************************** 自身方法 ***********************************/
    // 重新加载
    void DoRelayout(bool bSwitchStatus = false);
    void CreateItemByType(std::string strDis, QWidget * pWgt, std::string strAction,
                          SETTING_ITEM_TYPE eType);
    void AddItem(std::string strDis, QWidget * pWgt, std::string strAction, SETTING_ITEM_TYPE eType);
    void AddWifiItem();
#ifdef IF_SUPPORT_WIFI
    void AddWifiItem(std::string strAction, const WiFiNetInfo & wifiScanNet, std::string strNote,
                     int iIdx);
#endif

    // 更新连接结果提示语   0 : connect 1 : disconnect
    void SetConnectResultNote(int iResult, int iCurID = -1);
    void SetDisConnectResultNote(int iResult, int iCurID = -1);
    // 输入密码框
    void PopPwdDialog(bool bAuthentication = false);

    // 设置开关状态
    void SetWifiOpenClose(bool bOpen = false);
    // 设置是否由Off切到On的Scan
    void SetOff2OnScan(bool bOnScan = false)
    {
        m_bOnScan = bOnScan;
    }
    // set wifi item ui saved data
    void SetWifiItemSavedDataByDataIndex(int iDataIdx, std::string strNote, Item_Note_Type eType
                                         , int eItemType = SETTING_ITEM_WIFI, const QString & strAction = ""
                                         , bool bConnected = false);

    bool CheckSwitchChange();

    /******************************* 测试模式用 **********************************/
    void SetOpenByTestMode(bool bTestModeOpen);

    inline bool GetOpenByTestMode()
    {
        return m_bTestModeOpen;
    }

private Q_SLOTS:
    void OnSubmitNameAndPassword(const yl::string & strName, const yl::string & strPswd);
    void OnSubmitPassword(const yl::string & strPswd);
    void OnCancelPassword();

protected:
    void CreatePageByString(const char * strPageAction, ENUM_WIFI_PAGE_TYPE eType,
                            CListItem * pItem = NULL);

private Q_SLOTS:
    //on/off值变化
    void OnSwitchDeviceChange(bool bStatus);
    void OnItemDetailAction(CListItem * pItem);
#ifdef _T49
    void OnTimeOut();
#endif

private:
    qCheckBox         *         m_pWifiSwitch;
    int                         m_iPopWifiID;                   // 弹密码框的ID
    int
    m_iPreWifiID;                   // 前一个连接的ID,m_iPreWifiID 业务较杂，单独创建一个控制项
    bool                        m_bOnScan;                      // 是否是Off切到On的Scan
    bool                        m_bConnecting;                  // 是否正在连接或断开连接中
    std::map<int, int>          m_mpIDIndex;                    // Wifi ID对应DataIndex
    WifiDelegate        *       m_pWifiDelegate;                // Wifi Delegate
    qMultiPicAnimation     *    m_pPicAnimation;                // Scan animation
    bool                        m_bTestModeOpen;                // mark open by Test Mode or not
#ifdef _T49
    QTimer           *          m_pTimer;
#endif
    bool                        m_bFirstCreate;
};

#endif
