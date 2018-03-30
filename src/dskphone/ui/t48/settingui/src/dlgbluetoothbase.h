#ifndef DLGBLUETOOTHBASE_H
#define DLGBLUETOOTHBASE_H

#include "cdlgbasesubpage.h"
#include "bluetoothitem.h"
#include "settingui/src/bluetoothuicommon.h"
#include <QPointer>
#ifdef BLUETOOTH_USED_NEW_VERSION
#include "wireless/bluetoothNew/include/btdefines.h"
#include "wifiui/include/wirelesscommondata.h"
#else
#include "wireless/bluetoothOld/include/btdefines.h"
#endif // BLUETOOTH_USED_NEW_VERSION

class BluetoothDelegate;
class qCheckBox;
class CSettingBluetoothItem;
class BluetoothDetailSelector;
class qMultiPicAnimation;

class CDlgBluetoothBase : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgBluetoothBase();
    }

    static bool GetSyncPinMac(QString & strPinMac);

public:
    CDlgBluetoothBase(QWidget * parent = 0);

    virtual ~CDlgBluetoothBase();

    /***********************************from CDlgBaseSubPage*******************************/
public:
    virtual void Init();
    virtual void Uninit();

    //继承自CDlgBaseSubPage
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);
    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();
    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged()
    {
        return false;
    }
    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData()
    {
        return false;
    }
    //回到Idle前的处理函数
    virtual void OnReturnToIdle();
    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    // deal with action
    virtual bool OnAction(const QString & strAction);

    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    // 蓝牙界面Item属性不同，不需要统一转换
    virtual void AdjustWgtPair(QWPVector & vec, bool bAddIndex, bool bAddMark
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
                               , bool bChangeStyleOnPress
#endif
                               , bool bChangeStyleOnSelect
#endif
                              ) {};

    // 处理消息.
    LRESULT OnUIMessage(msgObject & refMessage);

    /**************************************************************************************/
    // set bluetooth
    void SetBluetooth(bool bBluetoothOpen = false);

    void DoReLayout(bool bBtState);

    //取消询问
    void ResetAskStatus();

    void AcceptMessageBox();

    void ShowPasswordMesaageBox();

    void UpdateDeviceList(int wParam, int lParam, const char * pMac = "");

    void SetPinMac(const char * pMac)
    {
        m_strAskPinMac = pMac;
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    bool CheckSwitchChange();

    bool IsOnScanning()
    {
        return m_bInScanning;
    }
    void PreDealWithScan();
#endif

    bool IsPairedDeivces(enum WIRELESS_DEVICE_STATE eState);

    // 测试模式预留
    inline bool GetOpenByTestMode()
    {
        return false;
    }

    /****************************************对内接口***************************************/
protected:
    // load all device list(include scan and paired)
    void LoadDeviceList(bool bClearStatus = true);
    // load paired device list
    void LoadPairedDevice();
    // add bluetooth devices to the view
    void AddPairedDevice(UI_LIST_BT_DEVICE & listDeviceInfo);
    void AddNotPairedDevice(UI_LIST_BT_DEVICE & listDeviceInfo);
    // 进入或退出BluetoothDeviceList界面，删除未配对蓝牙，防止未扫描且有未配对的蓝牙（之前扫描到的）显示
    void DeleteNotPairedDevice(UI_LIST_BT_DEVICE & listDeviceInfo);

    void SetDetailMac(QString strDetailMac);

    inline QString GetConnectName()
    {
        return m_strConnectedDeviceName;
    }

    inline bool GetClearStatus()
    {
        return m_bClearStatus;
    }

    // 获取配对蓝牙个数
    int GetNumOfPaired()
    {
        return m_iPairedBtCounts;
    }

    // get current Bluetooth Device Mac
    QString GetCurrentBluetoothMac();

    void OnStartScan();
    void OnStopScan();

    bool RequestStartScan();
    bool RequestStopScan();

private Q_SLOTS:
    //on/off值变化
    void OnSwitchDeviceStatus(bool);

    void OnItemDetailAction(CListItem * pItem);

#ifndef BLUETOOTH_USED_NEW_VERSION
    void AfterDetail();
    // 是否是移动操作
    int GetMoveUpOrNot();
#endif

    void OnSubmitBTPassword(const yl::string & strPswd);
    void OnCancelBTPassword();

    // 检查蓝牙Dongle是否存在，及是否有弹框提示语
    bool CheckDongle(bool bPopDlgTip = true);

private:
    void OnBtnSaveClick();
    void OnBtnEnterClick();
    void OnBtnConnect(const QString & strMac);
    void OnBtnDisconnect(const QString & strMac);
    void OnBtnScanClick(bool bClearStatus);
    void OnBtnDelete(const QString & strMac);
    void OnBtnDeleteAll();
    void OnBtnMoveUp(const QString & strMac);
    void OnBtnMoveDown(const QString & strMac);

    // deal with detail operator like : move up/down , set auto connect , delete bluetooth device
    void DealWithDetailOperator(const int iOp, const QString & strMac);

private:
    void InitData();

    void LoadMenuList(bool bBtState);

    void SetBluetoothItemNote(const QString & strNote);
#ifdef BLUETOOTH_USED_NEW_VERSION
    // 更新连接结果提示语   0 : connect 1 : disconnect
    void SetConnectResultNote(int iResult, QString strMac = "");
    void SetDisConnectResultNote(int iResult, QString strMac = "");

    // set bluetooth item ui saved data
    void SetBlueItemSavedDataByDataIndex(int iDataIdx, std::string strNote, Item_Note_Type eType
                                         , int eItemType = SETTING_ITEM_BLUETOOTH, const QString & strAction = ""
                                         , bool bConnected = false);

    void RegisterMsg();
    void UnRegisterMsg();

    void InitResidentWgtPair(bool bBTSwitchOn = false);
    bool FillBtWgtPairByBtDevice(QWidgetPair & qPair, DeviceInfo * pDeviceInfo);
    int GetResidentItemCount();
    void GetSearchTitleText(QString & strTitle);
    bool IsBlueboothdeviceScan() const;

    // get/set current operator device MAC
    void SetCurOperatorMAC(const QString & strMac);
    QString GetCurOperatorMAC();
    // set/get detail MAC
    QString GetDetailMAC();
#else
    // 获取Detail操作是否是设置自动连接
    int GetAutoConnet()
    {
        return m_iAutoConnet;
    }
#endif

private:
    qCheckBox         *         m_pCheckBox;
    BluetoothDelegate     *     m_pBlueDelegate;
    int                         m_iPairedBtCounts;      //配对蓝牙数目
//#warning 当前是否还有必要
    bool                        m_bClearStatus;     //清空列表的所有状态信息
    bool                        m_bAskingPwd;       //是否在询问密码
    QString                     m_strConnectedMac;
    QString                     m_strConnectedDeviceName;
    QString                     m_strAskPinMac;     // record current PIN MAC
    QString                     m_strDetailMac;
    QWPVector                   m_vecBlueWgtPair;   //存放页面控件数据
#ifdef BLUETOOTH_USED_NEW_VERSION
    QString                     m_strCurOperatorMac;// current operator MAC
    std::map<QString, int>      m_mapMACIndex;      // MAC对应Item的DateIndex
    bool                        m_bInScanning;
    bool                        m_bInConnecting;
#else
    int                         m_iConnectState;
    int                         m_iAutoConnet;
    int                         m_iMoveUp;          //是否上移操作
    QPointer<CSettingBluetoothItem> m_pItem;
#endif

    BluetoothDetailSelector  *  m_pBlueDetailSelector;          // bluetooth detail delegate
    qMultiPicAnimation     *    m_pPicAnimation;                // Scan animation

private:
    static QString              s_strPinMac;
};

#endif
