#ifndef __WIFI_COMMON_DATA_H
#define __WIFI_COMMON_DATA_H

#include "wirelesscommondata.h"

// temp solution
//#define TEMP_SLIP_SOLUTION
// 优化
#define WIFI_BASE_OPTIMIZATION_ONE

//#define TEST_T48_WIFI

// time to refresh
#define TIME_TO_FRESH_PAGE_DATA         3000//15000

#define MAX_SAVED_NUM                   5

struct WifiAddData
{
    int             iLeftIcon;
    int             iSecurity;
    int             iNetID;
    bool            bShowRightBtnIcon;
    bool            bShowNote;
    bool            bAutoPair;              // auto connect list
    bool            bConnectDevice;
    std::string     strNote;
    std::string     strRightBtnIcon;
    QString     strDisName;
    Item_Note_Type  eType;                  // 提示语显示状态


    WifiAddData() : iLeftIcon(0), strDisName(""), iSecurity(1), strNote(""), strRightBtnIcon(""),
        bShowNote(false)
        , iNetID(-1), eType(INT_NONE), bConnectDevice(false), bAutoPair(false), bShowRightBtnIcon(true) {}
    WifiAddData(QString sDisName, int iSecu, std::string sNote, bool bShow = false, int iLeft = 0,
                std::string sRightBtnIcon = "", int iID = -1, bool bPd = false, bool bCDevice = false,
                bool bAPair = false, bool bSRBIcon = true)
        : strDisName(sDisName), iSecurity(iSecu), strNote(sNote), bShowNote(bShow), iLeftIcon(iLeft),
          strRightBtnIcon(sRightBtnIcon), iNetID(iID), bConnectDevice(bCDevice), bAutoPair(bAPair),
          bShowRightBtnIcon(bSRBIcon)
    {}
};

struct WifiSignalData
{
    int             iSignalLevel;

    WifiSignalData() : iSignalLevel(0) {}
    WifiSignalData(int iLevel) : iSignalLevel(iLevel) {}
};

enum ENUM_WIFI_PAGE_TYPE
{
    ENUM_WIFI_NONE_PAGE_TYPE = 0x0001,
    ENUM_WIFI_ADD_PAGE_TYPE,
    ENUM_WIFI_ADD_PAGE_BY_WIFI_DETAIL_TYPE,
    ENUM_WIFI_EDIT_PAGE_TYPE,
    ENUM_WIFI_DETAIL_PAGE_TYPE,
    ENUM_WIFI_VIEW_PAGE_TYPE,

    ENUM_WIFI_MAX_PAGE_TYPE
};

#endif
