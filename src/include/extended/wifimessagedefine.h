#ifndef _WIFIMESSAGEDEFINE__INCLUDE_H_
#define _WIFIMESSAGEDEFINE__INCLUDE_H_

#include "innermsgdefine.h"

enum WifiMessageDefine
{
    WIFI_MESSAGE_BEGIN = INNER_MSG_WIFI_MESSAGE_BEGIN,
    WIFI_MSG_AUTHORIZE,                      //输入密码  wParam:Net id, lParam:eap network or normal, extradata:AuthInfo
    WIFI_MSG_UPDATE_UI,                      //刷新界面(wparam:操作类型,lparam:操作结果,ExtraData:数据内容)
    WIFI_MSG_GETSTATUS,                      //获取WIFI 模块状态
    WIFI_MSG_TEST,                           //测试消息
    WIFI_MESSAGE_END = INNER_MSG_WIFI_MESSAGE_END,
};

//操作类型
enum WIFI_UI_OPERATOR
{
    OP_UPDATE_WIFI_DATA = 0,         //刷新数据
    OP_WIFI_OPEN_RESULT,             //WIFI开启结果 iOperationResult 0/1/2/3 success/no dongle/cable connected/failed
    OP_WPS_OPEN,                     //WPS-PBC功能开启
    OP_WPSCONNECT_RESULT,            //WPS-PBC连接结果 iOperationResult 0/1 failed/success
    OP_SCAN_WIFI_RESULT,             //扫描结果 iOperationResult : 0/1 failed/success
    OP_CONNECT_WIFI_RESULT,          //连接结果  iOperationResult :0/1/2/3/4 success/wrong pwd/failed/connecting/cancle connect
    OP_ENTER_WIFI_PWD,               //输入网络的密码 iOperationResult :0/1 输入密码/输入eap用户名和密码
    OP_DISCONNECT_WIFI_RESULT,       //断开连接结果iOperationResult:0/1 failed/success
    OP_WPSPINCONNECT_RESULT,         //WPS-PIN连接结果 iOperationResult 0/1 failed/success
    OP_AUTOCONNECT_TIMEROUT,         //自动连接超时
    OP_WIFI_CLOSE_RESULT,            //WIFI 关闭结果
    OP_WIFI_ADD_RESULT,              //添加结果 iOperationResult： 1:列表已满
    OP_WIFI_CONNECT_MODIFY,          //连接profile 有差异的网络
    OP_WIFI_DONGLE_STATUS_MODIFY,    //WIFI dongle 状态改变 iOperationResult:0/1/2 add/remove/cancel message box
    OP_WIFI_SIGNAL_UPDATE            //信号图标更新
};

//开关结果
enum WiFiOpenResult
{
    WIRELESS_OP_OPEN_RESULT_SUCCESS = 0,                 //成功
    WIRELESS_OP_OPEN_RESULT_NODONGLE = 1,                //无适配器
    WIRELESS_OP_OPEN_RESULT_CABLE_CONNECTED = 2,         //网线已接入
    WIRELESS_OP_OPEN_RESULT_FAILED = 3,                  //失败
    WIRELESS_OP_OPEN_RESULT_ING = 4                      //执行中
};

//连接结果
enum WiFiConnectResult
{
    WIRELESS_OP_CONNECT_SUCCESS = 0,              //连接成功
    WIRELESS_OP_CONNECT_WRONGPWD = 1,             //密码错误
    WIRELESS_OP_CONNECT_FAILED = 2,               //连接失败
    WIRELESS_OP_CONNECTING = 3,                   //连接中
    WIRELESS_OP_CONNECT_CANCLE = 4                //取消连接
};

//dongle状态改变
enum WiFiDongleModify
{
    WIRELESS_OP_DONGLE_ADD = 0,                 //wifi dongle插入消息(开关为开的情况下插入dongle)
    WIRELESS_OP_DONGLE_REMOVE,                  //wifi dongle移除消息
    WIRELESS_OP_DONGLE_ADD_IGNORE_SWITCH        //wifi dongle插入消息(忽略开关状态)
};

#define  WIRELESS_OP_WPSCONNECT_SUCCESS            1
#define  WIRELESS_OP_WPSCONNECT_FAILED             0
#define  WIRELESS_OP_SCAN_RESULT_SUCCESS           1
#define  WIRELESS_OP_SCAN_RESULT_FAILED            0

#define  WIRELESS_OP_DISCONNECT_FAILED             0
#define  WIRELESS_OP_DISCONNECT_SUCCESS            1
#define  WIRELESS_OP_ADD_LISTFULL                  1

//操作参数
typedef struct WIRELESS_OP_MSG
{
    int        iOperationType;                  //操作类型
    int        iOperationSubType;               //子类型
    int        iOperationResult;               //操作结果
    long       lSessionID;                     //连接的ID
    WIRELESS_OP_MSG()
    {
        iOperationType = OP_UPDATE_WIFI_DATA;
        iOperationSubType = OP_UPDATE_WIFI_DATA;
        iOperationResult = WIRELESS_OP_OPEN_RESULT_SUCCESS;
        lSessionID = -1;
    }
} WirelessOperationMsg;

#endif //(_WIFIMESSAGEDEFINE__INCLUDE_H_)
