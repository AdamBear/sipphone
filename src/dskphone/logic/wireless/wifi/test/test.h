#ifdef IF_WIFI_TEST_DRIVER
#ifndef CTESTTHREAD__INCLUDED_H
#define CTESTTHREAD__INCLUDED_H
#include "wifi_inc.h"

#define WIFI_MSG_TEST_THREAD WIFI_MESSAGE_END - 1

class MyWiFiTestThread
{
    DECLARE_SINGLETON_CLASS(MyWiFiTestThread)
public:
    // 回调函数，遍历消息函数表
    BOOL OnMessage(msgObject & objMessage);

protected:
    void _outputSaveNetworkInfo(WiFiNetInfo & wifiInfo);

    void PrintfSavedList(UI_WIFI_NETINFO_LIST & listWiFi);

    void _outputScanNetInfo(WiFiNetInfo & wifiInfo);

    void PrintfScanNetInfo(UI_WIFI_NETINFO_LIST & listWiFi);

    void GetInt(int & iNumber);

    void GetBaseNetInfo(WiFiNetInfo & netInfo);

    void GetString(yl::string & strParam);

    int GetRandomNum(void);
};

#define _TestThread GET_SINGLETON_INSTANCE(MyWiFiTestThread)

#endif //CTESTTHREAD__INCLUDED_H
#endif // IF_WIFI_TEST_DRIVER
