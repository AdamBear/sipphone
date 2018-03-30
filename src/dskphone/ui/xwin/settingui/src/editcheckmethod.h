#ifndef __EDIT_CHECK_METHOD_H__
#define __EDIT_CHECK_METHOD_H__

#include "ETLLib/ETLLib.hpp"
#include "devicelib/networkserver.h"

// 指定IP是否是IPV6的IP
BOOL netIsValidIP_ipv6(LPCSTR lpszIpAddress);

// 判断是否是正确的IPV6 gateway
BOOL netIsValidGateWay_ipv6(LPCSTR lpszIpAddress);

bool IsValidIP(const yl::string & strValue);

bool IsValidIPv4(const yl::string & strValue);

bool IsValidIPv6(const yl::string & strValue);

bool IsValidIPv4NetMask(const yl::string & strValue);

bool IsValidIPv6GateWay(const yl::string & strValue);

bool isInRange(const yl::string & strValue, int min, int max);

bool IsValidIPv6Prefix(const yl::string & strValue);

bool IsValidPort(const yl::string & strValue);

bool IsValidDTMF(const yl::string & strValue);

bool IsValidNumber(const yl::string & strValue);

bool IsValidAES(const yl::string & strValue);

bool IsContentEmpty(const yl::string & strValue);

/*
国际域名格式:
域名由各国文字的特定字符集、英文字母、数字及“-”(即连字符或减号)任意组合而成, 但开头及结尾均不能含有“-”。
域名中字母不分大小写。域名最长可达67个字节
*/
bool IsValidURLServer(const yl::string & strValue);

bool IsValidServer(const yl::string & strValue);

bool IsValidServerName(const yl::string & strValue);

bool IsValidAccountName(const yl::string & strValue);

bool IsValidPassword(const yl::string & strValue);

bool IsValidQos(const yl::string & strValue);

bool IsValidVideoMTU(const yl::string & strValue);

bool IsValidVlanVID(const yl::string & strValue);

bool IsValidPacketInterval(const yl::string & strValue);

bool IsValidServerExpires(const yl::string & strValue);

bool IsValidUdpPort(const yl::string & strValue);

bool IsValidConferencePwd(const yl::string & strValue);



#endif // __EDIT_CHECK_METHOD_H__

