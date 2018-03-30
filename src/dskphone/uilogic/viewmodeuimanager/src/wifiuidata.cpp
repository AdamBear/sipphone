#ifdef IF_SUPPORT_WIFI
#include "wifiuidata.h"
#include "wireless/wifi/include/modwifi.h"
#include "translateiddefine.h"
#include "viewmodeuimanager/include/uicommon.h"
#include "commonapi/stringhelperapi.h"
#include <devicelib/phonedevice.h>

#define _CASE_TO_STRING(x)  case x: return #x;
#define WIFI_ENCRYPT "Lock"

// wifi列表图标
#if IS_COLOR
#define PIC_WIFI_LIST_SIGNAL_1      "Theme#wifi@wifi_2.png"
#define PIC_WIFI_LIST_SIGNAL_2      "Theme#wifi@wifi_3.png"
#define PIC_WIFI_LIST_SIGNAL_3      "Theme#wifi@wifi_4.png"
#define PIC_WIFI_LIST_SIGNAL_4      "Theme#wifi@wifi_5.png"
#else
#define PIC_WIFI_LIST_SIGNAL_1      "wifi_list_signal1.dob"
#define PIC_WIFI_LIST_SIGNAL_2      "wifi_list_signal2.dob"
#define PIC_WIFI_LIST_SIGNAL_3      "wifi_list_signal3.dob"
#define PIC_WIFI_LIST_SIGNAL_4      "wifi_list_signal4.dob"
#define PIC_WIFI_CONNECTED          "wifi_connect_hook.dob"
#define PIC_WIFI_CONNECTING         "wifi_connecting.dob"
#define PIC_WIFI_ENCRYPT            "wifi_encryption.dob"
#endif
const char * s_pWifiListIcon[] = { PIC_WIFI_LIST_SIGNAL_1, PIC_WIFI_LIST_SIGNAL_2,
                                   PIC_WIFI_LIST_SIGNAL_3, PIC_WIFI_LIST_SIGNAL_4
                                 };


CWifiUIData::CWifiUIData()
{
    m_bEnable = WIFI_IsEnable();
    m_bIsInScan = WIFI_IsScan();
}

CWifiUIData::~CWifiUIData()
{

}

// 获取title
yl::string CWifiUIData::GetTitle(int iUIType)
{
    yl::string strTitle("");

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_WIFI_MENU:
        strTitle = TRID_WIFI;
        break;
    case WIRELESS_UI_TYPE_WIFI_PROFILE:
        strTitle = TRID_STORAGE_NETWORK;
        break;
    case WIRELESS_UI_TYPE_WIFI_SCAN:
        strTitle = TRID_AVAILABLE_WIRELESS_DEVICE;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_STATUS:
        strTitle = TRID_WIRELESS_STATUS;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL:
        strTitle = TRID_NETWORK_DETAILS;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_EDIT:
        strTitle = TRID_EDIT_NETWORK;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_ADD:
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD:
        strTitle = TRID_ADD_NETWORK;
        break;
    case WIRELESS_UI_TYPE_WIFI_PIN_INPUT:
        strTitle = TRID_ENTER_PASSWORD;
        break;
    case WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK:
        strTitle = TRID_WIFI_AUTHENTICATION;
        break;
    default:
        break;
    }

    return UIConmonAPI_Lang(strTitle);
}

// 获取数据列表
bool CWifiUIData::GetDataList(VecViewInfo & vecInfo, int iUIType,
                              int iPageIndex, int iLastFocusDataID,
                              int iStartPos /*= 0*/, int iNumber /*= -1*/)
{
    bool bHandle = true;
    ViewInfo viewInfo;
    WiFiNetInfo netInfo;

    m_vecData.clear();

    // 将所需数据打包
    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_WIFI_MENU:
        PacketMenuUIData();
        break;
    case WIRELESS_UI_TYPE_WIFI_PROFILE:
    case WIRELESS_UI_TYPE_WIFI_SCAN:
        PacketListUIData(WIRELESS_UI_TYPE_WIFI_SCAN == iUIType);
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_STATUS:
        PacketStatusUIData();
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL:
        PacketDetailsData(iLastFocusDataID);
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_EDIT:
    case WIRELESS_UI_TYPE_WIFI_NET_ADD:
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD:
        PacketDevAddAndEditUIData(iUIType, iLastFocusDataID);
        break;
    case WIRELESS_UI_TYPE_WIFI_PIN_INPUT:
        viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_PASSWORD);
        m_vecData.push_back(viewInfo);
        break;
    case WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK:
        viewInfo.iDataID = WIFI_ITEM_DATA_ID_EDIT_EAP_NAME;
        viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_USER_NAME);
        m_vecData.push_back(viewInfo);
        viewInfo.iDataID = WIFI_ITEM_DATA_ID_EDIT_PSK;
        viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_PASSWORD);
        m_vecData.push_back(viewInfo);
        break;
    default:
        bHandle = false;
        break;
    }

    // 按区间截取数据
    bHandle = GetDataList(vecInfo, iStartPos, iNumber);
    WIRE_INFO("Get data list.UI[%d], start[%d], num[%d] size[%d], last data id[%d]",
              iUIType, iStartPos, iNumber, vecInfo.size(), iLastFocusDataID);

    return bHandle;
}

bool CWifiUIData::GetSoftkeyList(int iUIType, int iDataID,
                                 int iLastFocusDataID, YLVector<int> & vecSoftkey)
{
    WiFiNetInfo netInfo;
    bool bHandle = true;
    bool bIsScanUI = false;
    UI_WIFI_NETINFO_LIST listNetInfo;

    vecSoftkey.clear();
    for (int i = 0; i < 4; ++i)
    {
        vecSoftkey.push_back(ST_EMPTY);
    }

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_WIFI_MENU:
        {
            vecSoftkey[0] = ST_BACK;
            if (WIFI_ITEM_DATA_ID_MENU_SWITCH == iDataID)
            {
                vecSoftkey[2] = ST_SWITCH;
                vecSoftkey[3] = ST_SAVE;
            }
            else
            {
                vecSoftkey[3] = ST_ENTER;
            }

            if (!m_bEnable)
            {
                break;
            }

            vecSoftkey[1] = ST_WPS;
            if (WIRELESS_UI_TYPE_WIFI_PROFILE == iDataID)
            {
                vecSoftkey[2] = ST_ADD;
            }
            else if (WIRELESS_UI_TYPE_WIFI_SCAN == iDataID)
            {
                vecSoftkey[2] = ST_SCAN;
            }
        }
        break;
    case WIRELESS_UI_TYPE_WIFI_PROFILE:
    case WIRELESS_UI_TYPE_WIFI_SCAN:
        {
            bIsScanUI = iUIType == WIRELESS_UI_TYPE_WIFI_SCAN ? true : false;

            if (bIsScanUI)
            {
                vecSoftkey[0] = ST_BACK;
                vecSoftkey[2] = ST_SCAN;
            }
            else
            {
                vecSoftkey[0] = ST_BACK;
                vecSoftkey[2] = ST_ADD;
            }

            // 获取网络信息
            if (GetDevListSize(bIsScanUI ? WIFI_DEV_LIST_SCAN : WIFI_DEV_LIST_PAIRED)
                    && WIFI_GetNetworkInfoByID(iDataID, netInfo))
            {
                vecSoftkey[1] = bIsScanUI ? ST_DETAIL : ST_OPTION;
                if (netInfo.eNetState == WIRE_DEVICE_CONNECTED_STATE)
                {
                    vecSoftkey[3] = ST_DISCONNECT;
                }
                else if (netInfo.eNetState == WIRE_DEVICE_INIT_STATE)
                {
                    vecSoftkey[3] = ST_CONNECT;
                }
                else
                {
                    vecSoftkey[3] = ST_CANCEL;
                }
            }
        }
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_EDIT:
    case WIRELESS_UI_TYPE_WIFI_NET_ADD:
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD:
        {
            vecSoftkey[0] = ST_BACK;
            vecSoftkey[3] = ST_SAVE;
            if (WIFI_ITEM_DATA_ID_EDIT_PROFILE_NAME == iDataID
                    || WIFI_ITEM_DATA_ID_EDIT_SSID == iDataID
                    || WIFI_ITEM_DATA_ID_EDIT_PSK == iDataID
                    || WIFI_ITEM_DATA_ID_EDIT_EAP_NAME == iDataID)
            {
                vecSoftkey[1] = ST_IMECHANGE;
                vecSoftkey[2] = ST_DELETE;
            }
        }
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_STATUS:
        vecSoftkey[0] = ST_BACK;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL:
        {
            vecSoftkey[0] = ST_BACK;
            vecSoftkey[1] = ST_ADD;

            WIFI_GetNetworkInfoByID(iLastFocusDataID, netInfo);
            vecSoftkey[3] = WIRE_DEVICE_CONNECTED_STATE == netInfo.eNetState ?
                            ST_DISCONNECT : ST_CONNECT;
        }
        break;
    case WIRELESS_UI_TYPE_WIFI_PIN_INPUT:
    case WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK:
        {
            vecSoftkey[0] = ST_BACK;
            vecSoftkey[1] = ST_IMECHANGE;
            vecSoftkey[2] = ST_DELETE;
            vecSoftkey[3] = ST_OK;
        }
        break;
    default:
        bHandle = true;
        break;
    }

    WIRE_INFO("Get softkey list.UI[%d], data id[%d]", iUIType, iDataID);

    return bHandle;
}

// 获取Option softkey列表
bool CWifiUIData::GetOptionSoftkeyList(int iUIType, int iFocusIndex,
                                       YLVector<OPTION_TYPE> & vecOptionSoftkey)
{
    bool bHandle = true;

    if (WIRELESS_UI_TYPE_WIFI_PROFILE == iUIType)
    {
        vecOptionSoftkey.push_back(OPTION_EDIT);
        vecOptionSoftkey.push_back(OPTION_MOVE_UP);
        vecOptionSoftkey.push_back(OPTION_MOVE_DOWN);
        vecOptionSoftkey.push_back(OPTION_DELETE);
        vecOptionSoftkey.push_back(OPTION_DELETE_ALL);
    }
    else if (WIRELESS_UI_TYPE_WIFI_MENU == iUIType)
    {
        vecOptionSoftkey.push_back(OPTION_WPS);
        vecOptionSoftkey.push_back(OPTION_WPS_PIN);
    }
    else
    {
        bHandle = false;
    }

    return bHandle;
}

// 获取数据长度
int CWifiUIData::GetListSize(int iUIType)
{
    int iSize = 0;

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_WIFI_MENU:
        iSize = m_bEnable ? 4 : 2;
        break;
    case WIRELESS_UI_TYPE_WIFI_PROFILE:
    case WIRELESS_UI_TYPE_WIFI_SCAN:
        iSize = GetDevListSize(iUIType == WIRELESS_UI_TYPE_WIFI_SCAN ?
                               WIFI_DEV_LIST_SCAN : WIFI_DEV_LIST_PAIRED);
        iSize = iSize == 0 ? 1 : iSize;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_STATUS:
        iSize = 11;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL:
        iSize = 7;
        break;
    case WIRELESS_UI_TYPE_WIFI_NET_EDIT:
    case WIRELESS_UI_TYPE_WIFI_NET_ADD:
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD:
        iSize = 6;
        break;
    case WIRELESS_UI_TYPE_WIFI_PIN_INPUT:
        iSize = 1;
        break;
    case WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK:
        iSize = 2;
        break;
    default:
        break;
    }

    return iSize;
}

// 获取数据在链表的焦点
int CWifiUIData::GetFocusInDataList(int iDataID)
{
    return -1;
}

// 校正数据
bool CWifiUIData::CorrectViewInfo(int iUIType,
                                  const VecItemDataID2Info & vecItemDataInfo,
                                  VecViewInfo & vecInfo)
{
    if (WIRELESS_UI_TYPE_WIFI_NET_EDIT == iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_ADD == iUIType
            || WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD == iUIType)
    {
        for (int i = 0; i < vecItemDataInfo.size(); ++i)
        {
            ItemDataID2Info itemDataInfo = vecItemDataInfo[i];
            if (WIFI_ITEM_DATA_ID_EDIT_SECTURE != itemDataInfo.iDataID
                    || !itemDataInfo.bIsFocus)
            {
                continue;
            }

            ViewInfo viewInfo;
            viewInfo.iDataID = WIFI_ITEM_DATA_ID_EDIT_CIPHER;
            viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_CIPHER_TYPE);
            viewInfo.strValue = "0";

            WIFI_SERCURE_MODE eSercureMode = (WIFI_SERCURE_MODE)atoi(itemDataInfo.strCustomValue.c_str());
            switch (eSercureMode)
            {
            case SERCURE_NONE:
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_NONE));
                viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_NONE));
                break;
            case SERCURE_WEP: // 对应cipher为WEP, 定义中暂时没有对应的类型
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WEP));
                viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_W40));
                break;
            case SERCURE_WPA_PSK:
            case SERCURE_WPA2_PSK:
            case SERCURE_WPA_EAP:
            case SERCURE_WPA2_EAP:
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_TKIP));
                viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_TKIP));
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_AES));
                viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_CCMP));
                viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_TKIP_AES));
                viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_AUTO));
                break;
            default:
                break;
            }
            vecInfo.push_back(viewInfo);
            WIRE_INFO("Correct data success. size[%d]", vecInfo.size());
        }

        return true;
    }

    return false;
}

// 获取设备链表
bool CWifiUIData::GetDevList(UI_WIFI_NETINFO_LIST & listNetInfo,
                             DEV_LIST_TYPE eType,
                             int iStartPos /*= 0*/, int iNum /*= 911*/)
{
    UI_WIFI_NETINFO_LIST listNet;
    if (WIFI_DEV_LIST_SCAN == eType)
    {
        WIFI_GetScanNetworkList(listNet, iStartPos, iStartPos + iNum);

        WiFiNetInfo netConnectedInfo;
        WIFI_GetCurrentNetConnectInfo(netConnectedInfo);

        // 将已连接网络从列表删除
        for (UI_WIFI_NETINFO_LIST::iterator iter = listNet.begin();
                iter != listNet.end();
                ++ iter)
        {
            if (!WIFI_IsConnected())
            {
                break;
            }

            WiFiNetInfo & netInfo = *iter;
            if (WIRE_DEVICE_CONNECTED_STATE == netInfo.eNetState)
            {
                iter = listNet.erase(iter);
                // 将已连接网络插入到链表开始
                listNet.insert(listNet.begin(), netConnectedInfo);
                break;
            }
        }

        listNetInfo = listNet;
    }
    else if (WIFI_DEV_LIST_PAIRED == eType)
    {
        // 判断范围合法性
        if (iStartPos < 0 || iNum <= 0 || iStartPos > WIFI_GetSavedNetworkNum())
        {
            return false;
        }

        WIFI_GetSavedNetworkList(listNet);

        int iIndex = 0;
        for (UI_WIFI_NETINFO_LIST::iterator iter = listNet.begin();
                iter != listNet.end();
                ++iter, ++iIndex)
        {
            WiFiNetInfo & netInfo = *iter;

            if (iIndex < iStartPos)
            {
                continue;
            }

            if (iNum <= listNetInfo.size())
            {
                break;
            }

            // 保存列表网络同步扫描列表的信号强度
            /*WiFiNetInfo netScanInfo;
            if (WIFI_GetNetworkInfoByID(netInfo.iNetID, netScanInfo))
            {
                netInfo.iSignal = netScanInfo.iSignal;
            }*/

            listNetInfo.push_back(netInfo);
        }
    }

    return listNetInfo.size() > 0;
}

// 获取设备链表
bool CWifiUIData::GetDevList(VecViewInfo & vecInfo,
                             DEV_LIST_TYPE eType,
                             int iStartPos /*= 0*/, int iNum /*= 911*/)
{
    UI_WIFI_NETINFO_LIST listNet;

    if (!GetDevList(listNet, eType, iStartPos, iNum))
    {
        return false;
    }

    ViewInfo viewInfo;
    for (UI_WIFI_NETINFO_LIST::iterator iter = listNet.begin();
            iter != listNet.end();
            ++ iter)
    {
        viewInfo.Reset();
        WiFiNetInfo & netInfo = *iter;
        LogicData2UI(netInfo, viewInfo);
        vecInfo.push_back(viewInfo);
    }

    return true;
}

// Logic数据转换为UI显示数据
void CWifiUIData::LogicData2UI(WiFiNetInfo & netInfo, ViewInfo & devViewInfo)
{
    devViewInfo.Reset();

    devViewInfo.iDataID = netInfo.iNetID;
    devViewInfo.strDisplayName = netInfo.strProfileName.empty() ?
                                 netInfo.strSSID : netInfo.strProfileName;

    if (netInfo.iSignal > 0 && netInfo.iSignal <= 4)
    {
#if IS_COLOR
        if (PT_T52 == devicelib_GetPhoneType())
        {
            yl::string strSignal = commonAPI_FormatString("%d", netInfo.iSignal);
            devViewInfo.strVecExtraValue.push_back(strSignal);
        }
        else
#endif
        {
            devViewInfo.strVecExtraValue.push_back(s_pWifiListIcon[netInfo.iSignal - 1]);
        }
    }

    // 连接状态
    yl::string strTemp("");
    if (WIRE_DEVICE_CONNECTED_STATE == netInfo.eNetState)
    {
        strTemp = UIConmonAPI_Lang(TRID_CONNECTED);
    }
    else if (WIRE_DEVICE_CONNECTING_STATE == netInfo.eNetState)
    {
        strTemp = UIConmonAPI_Lang(TRID_CONNECTING);
    }
    devViewInfo.strVecCustomValue.push_back(strTemp);
    // 加密状态
    devViewInfo.strVecCustomValue.push_back(SERCURE_NONE != netInfo.iSercureMode ? WIFI_ENCRYPT :
                                            "");
}

// 获取设备列表大小
int CWifiUIData::GetDevListSize(DEV_LIST_TYPE eType)
{
    int iSize = 0;
    UI_WIFI_NETINFO_LIST listNet;

    if (WIFI_DEV_LIST_SCAN == eType)
    {
        WIFI_GetScanNetworkList(listNet);
        iSize = listNet.size();
    }
    else if (WIFI_DEV_LIST_PAIRED == eType)
    {
        iSize = WIFI_GetSavedNetworkNum();
    }

    return iSize;
}

// 获取安全模式
yl::string CWifiUIData::GetNetSercureModeStr(WIFI_SERCURE_MODE eMode)
{
    yl::string strSecuryMode("");
    switch (eMode)
    {
    case SERCURE_NONE:
        strSecuryMode = TRID_NONE;
        break;
    case SERCURE_WEP:
        strSecuryMode = TRID_WEP;
        break;
    case SERCURE_WPA_PSK:
        strSecuryMode = TRID_WPA_PSK;
        break;
    case SERCURE_WPA2_PSK:
        strSecuryMode = TRID_WPA2_PSK;
        break;
    case SERCURE_WPA_EAP:
        strSecuryMode = TRID_WPA_EAP;
        break;
    case SERCURE_WPA2_EAP:
        strSecuryMode = TRID_WPA2_EAP;
        break;
    default:
        break;
    }

    return UIConmonAPI_Lang(strSecuryMode);
}

// 获取加密方式
yl::string CWifiUIData::GetNetEncryptTypeStr(WIFI_ENCRYPT_TYPE eType)
{
    yl::string strEncryptType("");
    switch (eType)
    {
    case CIPHER_NONE:
        strEncryptType = TRID_NONE;
        break;
    case CIPHER_W40:
    case CIPHER_W104:
        strEncryptType = TRID_WEP;
        break;
    case CIPHER_TKIP:
        strEncryptType = TRID_TKIP;
        break;
    case CIPHER_CCMP:
        strEncryptType = TRID_AES;
        break;
    case CIPHER_AUTO:
        strEncryptType = TRID_TKIP_AES;
        break;
    default:
        break;
    }

    return strEncryptType;
}

// 添加网络或者修改网络
// 返回值:　[-1]失败  [0]编辑成功  [1]设备个数上限  [2]ssid为空  [3]psk为空
int CWifiUIData::EditNetInfo(int & iErrorDataID, bool bAdd,
                             VecItemDataID2Info & vecItemDataInfo,
                             int iNetID /*= WIFI_NET_ID_INVAILED*/)
{
    WiFiNetInfo netInfo;
    yl::string strPassword = "";

    for (int iIndex = 0; iIndex < vecItemDataInfo.size(); ++iIndex)
    {
        ItemDataID2Info & itemData = vecItemDataInfo[iIndex];
        if (WIFI_ITEM_DATA_ID_EDIT_SECTURE == itemData.iDataID)
        {
            netInfo.iSercureMode = (WIFI_SERCURE_MODE)atoi(itemData.strCustomValue.c_str());
        }
        else if (WIFI_ITEM_DATA_ID_EDIT_PROFILE_NAME == itemData.iDataID)
        {
            netInfo.strProfileName = itemData.strValue;
        }
        else if (WIFI_ITEM_DATA_ID_EDIT_SSID == itemData.iDataID)
        {
            netInfo.strSSID = itemData.strValue;
        }
        else if (WIFI_ITEM_DATA_ID_EDIT_CIPHER == itemData.iDataID)
        {
            netInfo.iEncrypt = (WIFI_ENCRYPT_TYPE)atoi(itemData.strCustomValue.c_str());
        }
        else if (WIFI_ITEM_DATA_ID_EDIT_PSK == itemData.iDataID)
        {
            strPassword = itemData.strValue;
        }
        else if (WIFI_ITEM_DATA_ID_EDIT_EAP_NAME == itemData.iDataID)
        {
            netInfo.strEapName = itemData.strValue;
        }
    }

    // 设备信息合法性判断
    if (netInfo.strSSID.empty())
    {
        iErrorDataID = WIFI_ITEM_DATA_ID_EDIT_SSID;
        return 2;
    }
    else if ((SERCURE_WEP == netInfo.iSercureMode
              || SERCURE_WPA_PSK == netInfo.iSercureMode
              || SERCURE_WPA2_PSK == netInfo.iSercureMode)
             && strPassword.empty())
    {
        iErrorDataID = WIFI_ITEM_DATA_ID_EDIT_PSK;
        return 3;
    }
    else if ((SERCURE_WPA_EAP == netInfo.iSercureMode
              || SERCURE_WPA2_EAP == netInfo.iSercureMode)
             && netInfo.strEapName.empty())
    {
        iErrorDataID = WIFI_ITEM_DATA_ID_EDIT_EAP_NAME;
        return 4;
    }

    // profile name为空使用ssid填充
    if (netInfo.strProfileName.empty())
    {
        netInfo.strProfileName = netInfo.strSSID;
    }

    // 密码框需要区分是否是EAP网络
    if (SERCURE_WPA_EAP == netInfo.iSercureMode
            || SERCURE_WPA2_EAP == netInfo.iSercureMode)
    {
        netInfo.strEapPwd = strPassword;
    }
    else
    {
        netInfo.strEapName.clear();
        netInfo.strPsk = strPassword;
    }

    int bResult = -1;
    if (bAdd)
    {
        // 添加网络需判断列当前保存个数
        bResult = 1;
        if (WIFI_GetSavedNetworkNum() <= WIFI_SAVED_MAX)
        {
            bResult = WIFI_AddNetwork(&netInfo) ? 0 : -1;

#if WIFI_DEBUG_LOG
            WIRE_INFO("Net(%d) sec[%d] ssid[%s] pro[%s] enc[%d] psk[%s] eap name[%s], eap pwd[%s]",
                      netInfo.iNetID,
                      netInfo.iSercureMode, netInfo.strSSID.c_str(), netInfo.strProfileName.c_str(),
                      netInfo.iEncrypt, netInfo.strPsk.c_str(), netInfo.strEapName.c_str(), netInfo.strEapPwd.c_str());
#endif
        }
    }
    else
    {
        bResult = 0;

        // 网络信息发生改变才进行修改
        WiFiNetInfo netSavedNetInfo;
        if (WIFI_GetNetworkInfoByID(iNetID, netSavedNetInfo)
                && (netInfo.iSercureMode != netSavedNetInfo.iSercureMode
                    || netInfo.strProfileName != netSavedNetInfo.strProfileName
                    || netInfo.strSSID != netSavedNetInfo.strSSID
                    || netInfo.iEncrypt != netSavedNetInfo.iEncrypt
                    || netInfo.strEapName != netSavedNetInfo.strEapName
                    || netInfo.strEapPwd != netSavedNetInfo.strEapPwd
                    || netInfo.strPsk != netSavedNetInfo.strPsk))
        {
            netInfo.iNetID = iNetID;
            bResult = WIFI_ModifyNetwork(iNetID, &netInfo) ? 0 : -1;

#if WIFI_DEBUG_LOG
            WIRE_INFO("Net(%d) [%d][%s][%s][%d][%s] to (%d) [%d][%s][%s][%d][%s]",
                      netSavedNetInfo.iNetID, netSavedNetInfo.iSercureMode, netSavedNetInfo.strProfileName.c_str(),
                      netSavedNetInfo.strSSID.c_str(), netSavedNetInfo.iEncrypt, netSavedNetInfo.strPsk.c_str(),
                      netInfo.iNetID, netInfo.iSercureMode, netInfo.strProfileName.c_str(),
                      netInfo.strSSID.c_str(), netInfo.iEncrypt, netInfo.strPsk.c_str());
#endif
        }
    }

    return bResult;
}

// 封装menu显示数据
void CWifiUIData::PacketMenuUIData()
{
    ViewInfo viewInfo;
    WiFiNetInfo netInfo;

    // 1.
    viewInfo.Reset();
    viewInfo.iDataID = WIFI_ITEM_DATA_ID_MENU_SWITCH;
    viewInfo.strDisplayName += UIConmonAPI_Lang(TRID_WIFI);
    viewInfo.strValue = m_bEnable ? "1" : "0";
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_OFF));
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_ON));
    m_vecData.push_back(viewInfo);

    // 2.
    viewInfo.Reset();
    viewInfo.iDataID = WIRELESS_UI_TYPE_WIFI_NET_STATUS;
    viewInfo.strDisplayName += UIConmonAPI_Lang(TRID_WIRELESS_STATUS);
    if (m_bEnable
            && WIFI_GetCurrentNetConnectInfo(netInfo)
            && WIRE_DEVICE_CONNECTED_STATE == netInfo.eNetState)
    {
        viewInfo.strDisplayName += ": ";
        viewInfo.strDisplayName += netInfo.strProfileName.empty() ? netInfo.strSSID :
                                   netInfo.strProfileName;
        viewInfo.strDisplayName += " " + UIConmonAPI_Lang(TRID_CONNECTED);
    }

    m_vecData.push_back(viewInfo);

    if (!m_bEnable)
    {
        return;
    }

    // 3.
    viewInfo.Reset();
    viewInfo.iDataID = WIRELESS_UI_TYPE_WIFI_PROFILE;
    viewInfo.strDisplayName += UIConmonAPI_Lang(TRID_THE_STORAGE_NETWORK);
    m_vecData.push_back(viewInfo);

    // 4.
    viewInfo.Reset();
    viewInfo.iDataID = WIRELESS_UI_TYPE_WIFI_SCAN;
    if (m_bIsInScan)
    {
        viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_SCANING);
    }
    else
    {
        viewInfo.strDisplayName = commonAPI_FormatString("%d ", GetDevListSize(WIFI_DEV_LIST_SCAN));
        viewInfo.strDisplayName += UIConmonAPI_Lang(TRID_AVAILABLE_NETWORK);
    }
    m_vecData.push_back(viewInfo);
}

// 封装网络列表显示数据
void CWifiUIData::PacketListUIData(bool bIsScanUI)
{
    ViewInfo viewInfo;
    WiFiNetInfo netInfo;

    if (GetDevListSize(bIsScanUI ? WIFI_DEV_LIST_SCAN : WIFI_DEV_LIST_PAIRED) <= 0
            || !GetDevList(m_vecData, bIsScanUI ? WIFI_DEV_LIST_SCAN : WIFI_DEV_LIST_PAIRED, 0, 911))
    {
        // 没有数据则显示空
        viewInfo.iDataID = DATA_ID_NONE;
        viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_NONE);
        m_vecData.push_back(viewInfo);
    }
}

// 封装status显示数据
void CWifiUIData::PacketStatusUIData()
{
    ViewInfo viewInfo;
    WiFiNetInfo netInfo;
    WIFI_GetCurrentNetConnectInfo(netInfo);

    // 获取数据
    GetViewInfoByNetField(viewInfo, NET_FIELD_STATUS, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_PROFILE_NAME, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_SSID, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_SIGNAL, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_CHANNEL, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_AP_MAC, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_SECURE_MODE, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_ENCRYPT_TYPE, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_TRANSMIT_RATE, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_FREQUENCE, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_QOS, netInfo);

    // 未连接设备, 则除了状态之外的信息都置空
    for (int iIndex = 0; iIndex < m_vecData.size(); ++ iIndex)
    {
        if (!WIFI_IsConnected() && 0 != iIndex)
        {
            m_vecData[iIndex].strValue = "";
        }
    }
}

// 封装detail显示数据
void CWifiUIData::PacketDetailsData(int iNetID)
{
    ViewInfo viewInfo;
    WiFiNetInfo netInfo;
    WIFI_GetNetworkInfoByID(iNetID, netInfo);

    // 获取数据
    GetViewInfoByNetField(viewInfo, NET_FIELD_PROFILE_NAME, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_SSID, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_AP_MAC, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_SIGNAL, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_SECURE_MODE, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_ENCRYPT_TYPE, netInfo);
    GetViewInfoByNetField(viewInfo, NET_FIELD_FREQUENCE, netInfo);
}

// 封装edit/add显示数据
void CWifiUIData::PacketDevAddAndEditUIData(int iUIType, int iNetID)
{
    ViewInfo viewInfo;
    WiFiNetInfo netInfo;

    if (WIRELESS_UI_TYPE_WIFI_NET_EDIT == iUIType)
    {
        WIFI_GetSavedNetInfoByID(iNetID, netInfo);
    }
    else if (WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD == iUIType)
    {
        WIFI_GetNetworkInfoByID(iNetID, netInfo);
    }

    // 安全模式
    viewInfo.Reset();
    viewInfo.iDataID = WIFI_ITEM_DATA_ID_EDIT_SECTURE;
    viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_SECURITY_MODE) + ":";
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_NONE));
    viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", SERCURE_NONE));
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WEP));
    viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", SERCURE_WEP));
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WPA_PSK));
    viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", SERCURE_WPA_PSK));
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WPA2_PSK));
    viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", SERCURE_WPA2_PSK));
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WPA_EAP));
    viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", SERCURE_WPA_EAP));
    viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WPA2_EAP));
    viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", SERCURE_WPA2_EAP));

    // 设置secure位置, 目前开放的安全模式如下:
    int iIndex = 0;
    switch (netInfo.iSercureMode)
    {
    case SERCURE_NONE:
        iIndex = 0;
        break;
    case SERCURE_WEP:
        iIndex = 1;
        break;
    case SERCURE_WPA_PSK:
        iIndex = 2;
        break;
    case SERCURE_WPA2_PSK:
        iIndex = 3;
        break;
    case SERCURE_WPA_EAP:
        iIndex = 4;
        break;
    case SERCURE_WPA2_EAP:
        iIndex = 5;
        break;
    default:
        break;
    }
    viewInfo.strValue = commonAPI_FormatString("%d", iIndex);
    m_vecData.push_back(viewInfo);

    GetViewInfoByNetField(viewInfo,
                          NET_FIELD_PROFILE_NAME, netInfo, WIFI_ITEM_DATA_ID_EDIT_PROFILE_NAME);
    GetViewInfoByNetField(viewInfo,
                          NET_FIELD_SSID, netInfo, WIFI_ITEM_DATA_ID_EDIT_SSID);

    // 加密类型与安全模式对应
    viewInfo.Reset();
    viewInfo.iDataID = WIFI_ITEM_DATA_ID_EDIT_CIPHER;
    viewInfo.strDisplayName = UIConmonAPI_Lang(TRID_CIPHER_TYPE) + ":";

    switch (netInfo.iSercureMode)
    {
    case SERCURE_NONE:
        viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_NONE));
        viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_NONE));
        break;
    case SERCURE_WEP: // 对应cipher为WEP, 定义中暂时没有对应的类型
        viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_WEP));
        viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_W40));
        break;
    case SERCURE_WPA_PSK:
    case SERCURE_WPA2_PSK:
    case SERCURE_WPA_EAP:
    case SERCURE_WPA2_EAP:
        viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_TKIP));
        viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_TKIP));
        viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_AES));
        viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_CCMP));
        viewInfo.strVecExtraValue.push_back(UIConmonAPI_Lang(TRID_TKIP_AES));
        viewInfo.strVecCustomValue.push_back(commonAPI_FormatString("%d", CIPHER_AUTO));
        break;
    default:
        break;
    }

    iIndex = 0;
    if (WIRELESS_UI_TYPE_WIFI_NET_ADD != iUIType)
    {
        // 设置cipher位置,
        switch (netInfo.iEncrypt)
        {
        case CIPHER_NONE:
        case CIPHER_W40:
        case CIPHER_W104:
        case CIPHER_TKIP:
            iIndex = 0;
            break;
        case CIPHER_CCMP:
            iIndex = 1;
            break;
        case CIPHER_AUTO:
            iIndex = 2;
            break;
        }
    }
    viewInfo.strValue = commonAPI_FormatString("%d", iIndex);

    m_vecData.push_back(viewInfo);

    GetViewInfoByNetField(viewInfo,
                          NET_FIELD_EAP_NAME, netInfo, WIFI_ITEM_DATA_ID_EDIT_EAP_NAME);
    GetViewInfoByNetField(viewInfo,
                          NET_FIELD_WPA_SHARE_KEY, netInfo, WIFI_ITEM_DATA_ID_EDIT_PSK);
}

// 获取列表数据
int CWifiUIData::GetDataList(VecViewInfo & vecInfo,
                             int iStartPos /*= 0*/, int iNum /*= 911*/)
{
    if (iStartPos < 0 || iNum <= 0 || iStartPos > m_vecData.size())
    {
        return 0;
    }

    int iIndex = 0;
    for (VecViewInfo::iterator iter = m_vecData.begin();
            iter != m_vecData.end();
            ++ iter, ++ iIndex)
    {
        ViewInfo & viewInfo = *iter;

        if (iIndex < iStartPos)
        {
            continue;
        }

        if (iNum <= vecInfo.size())
        {
            break;
        }
        vecInfo.push_back(viewInfo);
    }

    return vecInfo.size();
}

// 通过网络字段获取显示信息
bool CWifiUIData::GetViewInfoByNetField(ViewInfo & viewInfo, NET_FIELD eField,
                                        WiFiNetInfo & netInfo, int iDataID/* = DATA_ID_INVALID*/)
{
    bool bHandle = true;

    viewInfo.Reset();
    if (DATA_ID_INVALID != iDataID)
    {
        viewInfo.iDataID = iDataID;
    }

    switch (eField)
    {
    case NET_FIELD_STATUS:
        viewInfo.strDisplayName = TRID_WIRELESS_STATUS;
        if (!WIFI_IsDongleVaild())
        {
            viewInfo.strValue = TRID_DISABLE;
        }
        else
        {
            viewInfo.strValue = WIFI_IsConnected() ? TRID_CONNECTED : TRID_DISCONNECTED;
        }
        viewInfo.strValue = UIConmonAPI_Lang(viewInfo.strValue);
        break;
    case NET_FIELD_PROFILE_NAME:
        viewInfo.strDisplayName = TRID_PROFILE_NAME;
        viewInfo.strValue = netInfo.strProfileName;
        break;
    case NET_FIELD_SSID:
        viewInfo.strDisplayName = TRID_SSID;
        viewInfo.strValue = netInfo.strSSID;
        break;
    case NET_FIELD_SIGNAL:
        viewInfo.strDisplayName = TRID_SIGNAL_STRENGTH;
        viewInfo.strValue = commonAPI_FormatString("%d", netInfo.iSignal);
        break;
    case NET_FIELD_CHANNEL:
        viewInfo.strDisplayName = TRID_CHANNEL;
        viewInfo.strValue = commonAPI_FormatString("%d", netInfo.iChannel);
        break;
    case NET_FIELD_AP_MAC:
        viewInfo.strDisplayName = TRID_AP_MAC_ADDRESS;
        viewInfo.strValue = netInfo.strMac;
        break;
    case NET_FIELD_SECURE_MODE:
        viewInfo.strDisplayName = TRID_SECURITY_MODE;
        viewInfo.strValue = GetNetSercureModeStr(netInfo.iSercureMode);
        break;
    case NET_FIELD_ENCRYPT_TYPE:
        viewInfo.strDisplayName = TRID_ENCRYPTION_TYPE;
        viewInfo.strValue = GetNetEncryptTypeStr(netInfo.iEncrypt);
        break;
    case NET_FIELD_TRANSMIT_RATE:
        viewInfo.strDisplayName = TRID_TRANSMIT_RATE;
        viewInfo.strValue = commonAPI_FormatString("%d%s", netInfo.iSpeed, "M/s");
        break;
    case NET_FIELD_FREQUENCE:
        viewInfo.strDisplayName = TRID_FREQUENCY;
        viewInfo.strValue = commonAPI_FormatString("%d%s", netInfo.iFrequence, "MHz");
        break;
    case NET_FIELD_QOS:
        viewInfo.strDisplayName = TRID_QOS;
        viewInfo.strValue = commonAPI_FormatString("%d", netInfo.iQos);
        break;
    case NET_FIELD_EAP_NAME:
        viewInfo.strDisplayName = TRID_USER_NAME;
        viewInfo.strValue = netInfo.strEapName;
        // secure mode为eap类型才需要显示eap name
        viewInfo.bShow = SERCURE_WPA_EAP == netInfo.iSercureMode
                         || SERCURE_WPA2_EAP == netInfo.iSercureMode;
        break;
    case NET_FIELD_WPA_SHARE_KEY:
        viewInfo.strDisplayName = TRID_WPA_SHARED_KEY;
        if (netInfo.iSercureMode == SERCURE_WPA_EAP
                || netInfo.iSercureMode == SERCURE_WPA2_EAP)
        {
            viewInfo.strValue = netInfo.strEapPwd;
        }
        else
        {
            viewInfo.strValue = netInfo.strPsk;
        }
        break;
    default:
        break;
    }

    viewInfo.strDisplayName = UIConmonAPI_Lang(viewInfo.strDisplayName) + ":";
    m_vecData.push_back(viewInfo);

    return bHandle;
}

#endif // IF_SUPPORT_WIFI
