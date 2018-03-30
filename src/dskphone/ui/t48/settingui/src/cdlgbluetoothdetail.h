#ifndef CDLGBLUETOOTH_DETAIL_H
#define CDLGBLUETOOTH_DETAIL_H

#include "settingui/src/cdlgbasesubpage.h"
#include "settingui/src/bluetoothuicommon.h"
#ifdef BLUETOOTH_USED_OLD_VERSION
#include "wireless/bluetoothOld/include/btdefines.h"
#else
#include "wireless/bluetoothNew/include/btdefines.h"
#endif // IF_OLD_BLUETOOTH

class CDlgBluetoothDetail : public CDlgBaseSubPage
{
    Q_OBJECT
public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgBluetoothDetail();
    }

public:
    //blooth详情页面的类型
    enum BloothDetailType
    {
        BDT_PairedDevice, //已连接设备详情
        BDT_Local   //本地设备详情
    };
public:
    CDlgBluetoothDetail(QWidget * parent = 0);
    ~CDlgBluetoothDetail();

public:
    //页面自己加载数据,用于设置本地详情数据
    bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();

    // 初始化数据
    void Init();

    // 释放数据
    void Uninit();

    //外部设置数据，用于设置一匹配设备的详情信息
    virtual void SetData(void * pData = NULL);

    //外部设置数据，用于设置一匹配设备的详情信息
    bool SetPageData(QString * pStrMac);

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //setSubPage之前做的操作，会在LoadPageData之前调用
    //virtual void BeforeSetSubPage(void *pData);

    //softkey按下后的响应
    virtual bool OnAction(const QString & strAction);

    void ConnectStatusChanged();

    void AcceptMessageBox();

    //处理ETL消息
    void OnETLMsg(int wParam, int lParam);

    //界面当前进行的操作代码
    int GetOpCode() const
    {
        return m_eOpCode;
    }

protected:
    //连接当前编辑的设备
    void OnConnect();

    void OnDisconnect();

private:
    //初始化数据，设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();
    //取得当前页面数据
    void GetCurrentPageData(DeviceInfo & currentPageData);
    void OnBtnIMEClick();
    void OnBtnDeleteClick();
    void UpdateSoftkeyFN4(int iIndex = -1);

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

private Q_SLOTS:
    //页面的选中行改变时的处理
    void OnCurrentIndexChange(int iIndex);

private:
    DeviceInfo m_DeviceData;
    QLineEdit * m_pEditDeviceName;
    QLineEdit * m_pEditMACAddr;
    QComboBox * m_pCmbBoxAutoConnect;
    QString m_strDevMac;

    int m_eOpCode; //操作状态

    //本页面的类型
    int m_eDetailType;

    bool m_bAsynchReq;
};

#endif

