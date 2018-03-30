#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiManager.h
//  Implementation of the Class CWifiManager
//  Created on:      15-五月-2014 9:17:48
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CWIFIMANAGER__INCLUDED_H
#define CWIFIMANAGER__INCLUDED_H

#include "taskaction/taskactioncallbackbase.h"
#include "taskaction/taskaction.h"
#include "wifidata.h"
#include "wifisession.h"
#include "wireless/common/wirelesssessionmanager.h"

#define  _WiFiData  _WiFiManager.GetWifiDataInstance()

class CWiFiManager : public NS_TA::CTaskActionCallbackBase
{

    DECLARE_SINGLETON_CLASS(CWiFiManager)

public:
    //初始化
    bool Init();

    //获取保存网络个数
    int  GetSavedNetworkNum();

    //获取WPS-PIN 码
    bool GetWpsPinCode(yl::string & strPinCode);

    //是否在搜索状态
    bool IsInScan();

    //是否已连接网络
    bool IsConnected(int iNetID = WIFI_NET_ID_INVAILED);

    //网络是否正在连接
    bool IsConnecting(int iNetID = WIFI_NET_ID_INVAILED);

    //是否适配器可盈
    bool IsDongleValid(bool bCheckNow = false);

    //wifi是否可用
    bool IsEnable();

    //开关会话
    bool SetEnable(bool bEnable, bool bNotify = false, bool SetConfig = true);

    //更新网络信息
    bool RefreshData();

    //处理扫描的用户消息
#if IF_SUPPORT_WIFI_CLYCLESCAN
    bool ProcessScan(bool bFirstScan = true);
#else
    bool ProcessScan();
#endif
    //处理停止扫描
    bool ProcessStopScan();

    //处理连接的用户消息
    bool ProcessConnect(int iNetID);

    //处理断开连接
    bool ProcessDisconnect(int iNetID);

    //处理取消连接
    bool ProcessCancelConnect(int iNetID);

    //处理WPS-PIN连接消息
    bool ProcessWPSConnect(WIRELESS_SESSION_STATE eState);

    //处理停止WPS
    bool ProcessCancelWPSConnect(WIRELESS_SESSION_STATE eState = SESSION_UNINIT_STATE);

    //通知UI更新
    bool UpdateUI(WIFI_UI_OPERATOR iOperatorType  = OP_UPDATE_WIFI_DATA, int iOperatorResult  = 0,
                  int iNetID = WIFI_NET_ID_INVAILED);

    //接收用户事件消息
    bool OnRecieveMessage(msgObject & objMsg);

    //注销某个状态的会话
    bool DestorySessionByState(WIRELESS_SESSION_STATE eState, bool bEndConnect = false)
    {
        return _WirelessSessionManager.DestorySessionByState(eState, WIFI_SESSION, bEndConnect);
    }

    //删除网络
    bool DeleteNetwork(int iNetID, bool bNotify = true);

    CWiFiData & GetWifiDataInstance()
    {
        return m_instData;
    }

    //处理自动连接成功后添加到保存列表
    bool ProcessAutoAdd(int iNetID, const AuthInfo & autoInfo, const WiFiNetInfo * pNetInfo = NULL);

    //获取已连接网络的信息
    bool GetConnectedNetInfo(WiFiNetInfo & netInfo);

    //网络鉴权
    bool AuthNetwork(int iNetId, const AuthInfo& autoInfo, bool bEapNetwork = false);

protected:

    //连接结果消息处理
    bool ProcessConnectStatus(msgObject & objMsg);

    //定时器处理
    bool ProcessTimer(int  iTimerID);

    //适配器插入
    bool ProcessDongleAdded(bool bIsCallback);

    //适配器拔出
    bool ProcessDongleRemoved();

    /***
     * @brief : Action执行结束的回调
     * @param : |pTaskAction| 执行结束的action
     */
    virtual void OnTaskActionCallback(NS_TA::CTaskAction* pTaskAction);

    //设置wps硬按键等待定时器
    bool SetWPSWaittingTimer(bool bEnable);

    //设置自动连接超时定时器
    bool SetAutoConnectTimer(bool bEnable);

    //做初始化wifi的工作
    bool DoInit(bool bSucc, bool bNotify);

    //做关闭wifi的工作
    bool DoClose(bool bSucc);

    //处理断开连接之后的工作
    bool ProcessConnected();

    //设置WIFI开关状态
    void SetWifiStatus(bool bEnable, bool bSetConfig = true);

    //删除会话
    bool DeleteSession(CWiFiSession * pSession, bool bEndConnect = false)
    {
        return _WirelessSessionManager.DeleteSession(pSession, bEndConnect);
    }

    //新建并进入一路会话
    bool InitSession(WIRELESS_SESSION_STATE eState, int iNetID = WIFI_NET_ID_INVAILED,
                     void * pUserData = NULL)
    {
        return _WirelessSessionManager.InitSession(eState, WIFI_SESSION, iNetID, pUserData);
    }

    CWiFiSession * AddSession()
    {
        CSessionBase * pSession = _WirelessSessionManager.AddSession(WIFI_SESSION);
        return dynamic_cast<CWiFiSession *>(pSession);
    }

    //通过ID获取session指针
    CWiFiSession * GetSessionByNetID(int iNetID)
    {
        CSessionBase * pSession = _WirelessSessionManager.GetSessionByHandle(iNetID, WIFI_SESSION);
        return dynamic_cast<CWiFiSession *>(pSession);
    }

    //通过状态获取session指针
    CWiFiSession * GetSessionByState(WIRELESS_SESSION_STATE eState)
    {
        CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(eState, WIFI_SESSION);
        return dynamic_cast<CWiFiSession *>(pSession);
    }

    //通过state传callback消息给session
    bool SendCallBackMsgToSessionByState(WIRELESS_SESSION_STATE eState, msgObject & objMsg)
    {
        return _WirelessSessionManager.SendCallBackMsgToSessionByState(eState, objMsg, WIFI_SESSION);
    }

    //通过ID传消息ui给session
    bool SendUserMsgToSessionByID(int iNetID, msgObject & objMsg)
    {
        return _WirelessSessionManager.SendUserMsgToSessionByHandle(iNetID, objMsg, WIFI_SESSION);
    }

    //通过ID传callback消息给session
    bool SendCallBackMsgToSessionByID(int iNetID, msgObject & objMsg)
    {
        return _WirelessSessionManager.SendCallBackMsgToSessionByHandle(iNetID, objMsg, WIFI_SESSION);
    }

private:
    //wifi数据类
    CWiFiData    m_instData;

    //wps硬按键等待定时器
    const int  m_iWpsTimer;

    //自动连接定时器
    const int m_iAutoConnectTimer;

    //wifi 开关
    bool m_bEnable;
};

#define _WiFiManager GET_SINGLETON_INSTANCE(CWiFiManager)

#endif // !defined(CWIFIMANAGER__INCLUDED_H)
#endif // IF_SUPPORT_WIFI
