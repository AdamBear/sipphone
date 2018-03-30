#ifndef __WIRELESS_COMMON_DATA_H
#define __WIRELESS_COMMON_DATA_H

// refresh interface with record offset
#define FRESH_WINDOW_WITH_MIN_CHANGED

// Item Note Type
enum Item_Note_Type
{
    INT_NONE = 0x0001,
    INT_ANIMATION,          // display with animation
    INT_DISAPPEAR,          // display 2 second then disappear
    INT_FOREVER,            // display forever

    INT_END
};

// 调用UI逻辑层接口返回值类型
enum Operatoer_Reslut
{
    OR_NONE = 0x0001,
    OR_NO_DONGLE,
    OR_HAVE_CONNECTED,
    OR_INTERFACE_FAIL,          // 调用Logic或其他接口返回false
    OR_INTERFACE_SUCCESS,       // 调用Logic或其他接口返回true
    OR_MOBILE_CONNECT,

    OR_END
};

// Wireless Search Item Data
struct WirelessSearchStatisticsData
{
    Item_Note_Type  s_eType;
    QString         s_strInfo;

    WirelessSearchStatisticsData() : s_eType(INT_NONE), s_strInfo("") {}
    WirelessSearchStatisticsData(Item_Note_Type eType, QString strInfo)
        : s_eType(eType), s_strInfo(strInfo) {}
    ~WirelessSearchStatisticsData() {}
};

#endif
