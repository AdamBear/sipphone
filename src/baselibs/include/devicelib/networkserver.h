/**********************************************************************************************//**
 * @file    networkserver.h
 *
 * @brief   Declares the networkserver class.
 **************************************************************************************************/

#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__
#include <netmsgtypedefine.h>


// 由于之前T3X/VPX都有声明出C接口，外部模块已经这样依赖
// ////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  __cplusplus
extern "C" {
#endif

// simple interface
WAN_PORT_TYPE       netGetNetworkType();
WAN_PORT_STA        netGetNetworkSta();
WAN_6_Port_Type     netGetIPv6Type();
WAN_PORT_STA        netGetIPv6Status();

NETWORK_ERROR_CODE  netGetNetworkErrorCode();

BOOL                netIsCableConnected();
BOOL                netIsIPConflictting();
BOOL                netIsIPv6Conflictting();

// BOOL             IsValidIPV4(LPCSTR lpszIpAddress);
// BOOL             IsValidIPV6(LPCSTR lpszIpAddress);

BOOL                GetIPV6ByProc(char * pstrIp, int iLen, int * prefix);

in_addr             netGetLocalIPv4();          // inet_ntoa() to chStringA
in_addr             netGetWanNetmask();
in_addr             netGetWanGateway();
in_addr             netGetWanPrimaryDNS();
in_addr             netGetWanSecondaryDNS();

LPCSTR              netGetMACText(ACHAR cSplit);
yl::string          netGetMacText();
yl::string          netGetMacByHardWare(char szPlit);
BOOL                netIsIPV6Enable();

BOOL                netIsValidIP(LPCSTR lpszIpAddress);
BOOL                netIsValidIPv4(LPCSTR lpszIpAddress);
BOOL                netIsValidIPV6(LPCSTR lpszIpAddress);
BOOL                netIsValidIPV6GateWay(LPCSTR lpszIpAddress);

BOOL                netIsTrustedLocalTime();
//  BOOL                netIsAutoUpgrading();

time_t              netGetLocalTime();
BOOL                netSetLocalTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);

BOOL                netSendTimeMsg();
BOOL                netFlushNetworkConfig();
in_addr             netGetHostByName(LPCSTR lpszHostName);
in_addr             netGetHostByNameEx(LPCSTR lpszHostName, int nTimeout,
                                       BOOL bForceUseProxyThread);

// BOOL             netGetIPV6ByProc(char* pstrIp, int iLen);
BOOL                netLookupHostname(const char * hostname, struct in_addr * padd4, int * piGotV4,
                                      struct in6_addr * padd6, int * piGotV6, int nRetryTimes, int nTimeout, IP_Addr_Mode addrMode);
// if the vlan is available. depend on pppoe and vid
BOOL                netVlanAvailable();

#ifdef  __cplusplus
}; // extern "C"
#endif








/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ interface
#ifdef  __cplusplus

/**********************************************************************************************//**
 * @fn  yl::string netAddr2String(const in_addr& addr);
 *
 * @brief   Net address 2 string.
 *
 * @param   addr    The address.
 *
 * @return  addr string
 **************************************************************************************************/

yl::string          netAddr2String(const in_addr & addr);

/**********************************************************************************************//**
 * @fn  yl::string netAddr2String(const in6_addr& addr);
 *
 * @brief   Net address 2 string.
 *
 * @param   addr    The address.
 *
 * @return  addr string
 **************************************************************************************************/

yl::string          netAddr2String(const in6_addr & addr);

/**********************************************************************************************//**
 * @fn  BOOL netGetLanBridge();
 *
 * @brief   Determines if we can net get LAN bridge.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetLanBridge();

/**********************************************************************************************//**
 * @fn  BOOL netGetLanIP(yl::string & strResult);
 *
 * @brief   Net get LAN IP.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetLanIP(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetLanNetmask(yl::string &strResult);
 *
 * @brief   Net get LAN netmask.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetLanNetmask(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetLocalIPv4(yl::string& strResult);
 *
 * @brief   Net get local IPv4.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetLocalIPv4(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanNetmaskIPv4(yl::string& strResult);
 *
 * @brief   Net get WAN netmask IPv4.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanNetmaskIPv4(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanGatewayIPv4(yl::string& strResult);
 *
 * @brief   Net get WAN gateway IPv4.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanGatewayIPv4(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanPrimaryDNSIPv4(yl::string& strResult);
 *
 * @brief   Net get WAN primary DNS IPv4.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanPrimaryDNSIPv4(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanSecondaryDNSIPv4(yl::string& strResult);
 *
 * @brief   Net get WAN secondary DNS IPv4.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanSecondaryDNSIPv4(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanPrefixIPv4(yl::string& strResult);
 *
 * @brief   Net get WAN prefix IPv4.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanPrefixIPv4(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetLocalIPv6(yl::string& strResult);
 *
 * @brief   Net get local IPv6.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetLocalIPv6(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanGatewayIPv6(yl::string& strResult);
 *
 * @brief   Net get WAN gateway IPv6.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanGatewayIPv6(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanPrimaryDNSIPv6(yl::string& strResult);
 *
 * @brief   Net get WAN primary DNS IPv6.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanPrimaryDNSIPv6(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanSecondaryDNSIPv6(yl::string& strResult);
 *
 * @brief   Net get WAN secondary DNS IPv6.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanSecondaryDNSIPv6(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  BOOL netGetWanPrefixIPv6(yl::string& strResult);
 *
 * @brief   Net get WAN prefix IPv6.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanPrefixIPv6(yl::string & strResult);

/**********************************************************************************************//**
 * @fn  int netGetNetworkMode();
 *
 * @brief   Net get network mode.
 *
 * @return  An int.
 **************************************************************************************************/

int                 netGetNetworkMode();

/**********************************************************************************************//**
 * @fn  BOOL netIsOpenIPv4Mode();
 *
 * @brief   Determines if we can net is open IPv4 mode.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netIsOpenIPv4Mode();

/**********************************************************************************************//**
 * @fn  BOOL netIsOpenIPv6Mode();
 *
 * @brief   Determines if we can net is open IPv6 mode.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netIsOpenIPv6Mode();

/**********************************************************************************************//**
 * @fn  BOOL netGetWanIP(yl::string& strResult);
 *
 * @brief   Net get WAN IP.
 *
 * @param [in,out]  strResult   The result.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netGetWanIP(yl::string &
                                strResult);        //获取默认有效ip,同时有效ipv4优先。

/**********************************************************************************************//**
 * @fn  BOOL netIsNetworkReady();
 *
 * @brief   Determines if we can net is network ready.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL                netIsNetworkReady();//网络是否有效

/**********************************************************************************************//**
 * @fn  yl::string netGetHostIPString(const char *hostname, int nTimeout, IP_Addr_Mode addrMode);
 *
 * @brief   Net get host IP string.
 *
 * @param   hostname    The hostname.
 * @param   nTimeout    The timeout.
 * @param   addrMode    The address mode.
 *
 * @return  A yl::string.
 **************************************************************************************************/

yl::string          netGetHostIPString(const char * hostname, int nTimeout, IP_Addr_Mode addrMode);

/**********************************************************************************************//**
 * @fn  yl::string netGetVirtualAdapterName();
 *
 * @brief   get the network adaptername. depend on vlan config.
 *
 * @return  A yl::string.
 **************************************************************************************************/

yl::string          netGetVirtualAdapterName();

/**********************************************************************************************//**
 * @fn  yl::string netGetAdapterName();
 *
 * @brief   Net get adapter name.
 *
 * @return  A yl::string.
 **************************************************************************************************/

yl::string          netGetAdapterName();

/**********************************************************************************************//**
 * @fn  BOOL netStartCapture(int iTimeout);
 *
 * @brief   Net start capture.
 *
 * @param   iTimeout    Zero-based index of the timeout.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL netStartCapture(int iTimeout);

/**********************************************************************************************//**
 * @fn  BOOL netStopCapture(int iTimeout);
 *
 * @brief   Net stop capture.
 *
 * @param   iTimeout    Zero-based index of the timeout.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL netStopCapture(int iTimeout);

/**********************************************************************************************//**
 * @fn  BOOL netExportCapture(const char* pFilePath, int iLen, int iTimeout);
 *
 * @brief   Net export capture.
 *
 * @param   pFilePath   Full pathname of the file.
 * @param   iLen        Zero-based index of the length.
 * @param   iTimeout    Zero-based index of the timeout.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

BOOL netExportCapture(const char * pFilePath, int iLen, int iTimeout);

/**********************************************************************************************//**
 * @fn  int netGetCaptureState(int iTimeout);
 *
 * @brief   Net get capture state.
 *
 * @param   iTimeout    Zero-based index of the timeout.
 *
 * @return  An int.
 **************************************************************************************************/

int netGetCaptureState(int iTimeout);

#endif  //end of c++ interface

#endif // __NETWORK_SERVER_H__

