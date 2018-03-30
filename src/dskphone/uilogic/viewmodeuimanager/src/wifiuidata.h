#ifdef IF_SUPPORT_WIFI
#ifndef _WIFIUI_DATA_H_
#define _WIFIUI_DATA_H_

#include "viewmodeuimanager/include/viewmodeuidefine.h"
#include "common/common_data_define.h"
#include "wireless/wifi/include/wifidefines.h"

enum DEV_LIST_TYPE
{
    WIFI_DEV_LIST_SCAN,
    WIFI_DEV_LIST_PAIRED,
    WIFI_DEV_LIST_ALL
};

enum NET_FIELD
{
    NET_FIELD_STATUS = 0,
    NET_FIELD_PROFILE_NAME,
    NET_FIELD_SSID,
    NET_FIELD_SIGNAL,
    NET_FIELD_CHANNEL,
    NET_FIELD_AP_MAC,
    NET_FIELD_SECURE_MODE,
    NET_FIELD_ENCRYPT_TYPE,
    NET_FIELD_TRANSMIT_RATE,
    NET_FIELD_FREQUENCE,
    NET_FIELD_QOS,
    NET_FIELD_WPA_SHARE_KEY,    // 普通网络密码和eap网络密码共用
    NET_FIELD_EAP_NAME,         // eap网络用户名
};

// 编辑界面的返回结果
struct EditResult
{
    int iResult;
    int iFocusDataID;

    EditResult()
    {
        iResult = 0;
        iFocusDataID = DATA_ID_INVALID;
    }
};

class CWifiUIData
{
public:
    CWifiUIData();
    ~CWifiUIData();
    // 获取title
    yl::string GetTitle(int iUIType);
    // 获取数据列表
    bool GetDataList(VecViewInfo & vecInfo, int iUIType, int iPageIndex,
                     int iLastFocusDataID, int iStartPos = 0, int iNumber = -1);
    // 获取softkey列表
    bool GetSoftkeyList(int iUIType, int iDataID,
                        int iLastFocusDataID, YLVector<int> & vecSoftkey);
    // 获取Option softkey列表
    bool GetOptionSoftkeyList(int iUIType, int iFocusIndex,
                              YLVector<OPTION_TYPE> & vecOptionSoftkey);
    // 获取数据长度
    int  GetListSize(int iUIType);
    // 获取数据在链表的焦点
    int  GetFocusInDataList(int iDataID);
    // 校正数据
    virtual bool CorrectViewInfo(int iUIType,
                                 const VecItemDataID2Info & vecItemDataInfo,
                                 VecViewInfo & vecInfo);

    // 获取设备链表
    bool GetDevList(UI_WIFI_NETINFO_LIST & listNetInfo,
                    DEV_LIST_TYPE eType, int iStartPos = 0, int iNum = 911);
    bool GetDevList(VecViewInfo & vecInfo,
                    DEV_LIST_TYPE eType, int iStartPos = 0, int iNum = 911);
    // Logic数据转换为UI显示数据
    void LogicData2UI(WiFiNetInfo & netInfo, ViewInfo & devViewInfo);
    // 获取设备链表大小
    int GetDevListSize(DEV_LIST_TYPE eType);
    // 获取安全模式
    yl::string GetNetSercureModeStr(WIFI_SERCURE_MODE eMode);
    // 获取加密方式
    yl::string GetNetEncryptTypeStr(WIFI_ENCRYPT_TYPE eType);
    // 添加网络或者修改网络
    // 返回值:　[-1]失败  [0]编辑成功  [1]设备个数上限  [2]ssid为空  [3]psk为空
    int EditNetInfo(int & iErrorDataID, bool bAdd,
                    VecItemDataID2Info & itemDataInfo, int iNetID = WIFI_NET_ID_INVAILED);
    bool GetUISwitchEnable()
    {
        return m_bEnable;
    }
    void SetUISwitchEnable(bool bEnable)
    {
        m_bEnable = bEnable;
    }
    bool GetUIScanState()
    {
        return m_bIsInScan;
    }
    void SetUIScanState(bool bScan)
    {
        m_bIsInScan = bScan;
    }
private:
    // 封装menu显示数据
    void PacketMenuUIData();
    // 封装网络列表显示数据
    void PacketListUIData(bool bIsScanUI);
    // 封装status显示数据
    void PacketStatusUIData();
    // 封装detail显示数据
    void PacketDetailsData(int iNetID);
    // 封装edit/add显示数据
    void PacketDevAddAndEditUIData(int iUIType, int iNetID);
    int  GetDataList(VecViewInfo & vecInfo, int iStartPos = 0, int iNum = 911);
    // 通过网络字段获取显示信息
    bool GetViewInfoByNetField(ViewInfo & viewInfo, NET_FIELD eField,
                               WiFiNetInfo & netInfo, int iDataID = DATA_ID_INVALID);

private:
    bool                m_bEnable;          // 记录UI开关状态
    bool                m_bIsInScan;        // 记录UI扫描状态
    VecViewInfo         m_vecData;          // 封装完整的item显示数据
};


#endif // _WIFIUI_DATA_H_
#endif // IF_SUPPORT_WIFI
