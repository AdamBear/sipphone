#ifndef __BLUETOOTH_MSG_DEFINE_H_
#define __BLUETOOTH_MSG_DEFINE_H_

#include "innermsgdefine.h"

enum BluetoothMessageDefine
{
    BLUETOOTH_MESSAGE_BEGIN = INNER_MSG_BLUETOOTH_MESSAGE_BEGIN,
    // 蓝牙回调事件消息
    BLUETOOTH_MSG_CALLBACK,
    // 蓝牙耳机连接与取消连接
    BLUETOOTH_MSG_CONNECT_STATE,    //wParam:1:connect 0:disconnect
    // PIN码输入
    BLUETOOTH_MSG_REQ_PIN,
    // 刷新界面
    BLUETOOTH_MSG_UPDATE_LIST,
    // play or stop ring
    BLUETOOTH_MSG_PLAY_RING,        //wparam 1=play,0=stop
    // set enable or disable
    BLUETOOTH_MSG_SET_ENABLE,       //wparam 1=enable,0=disable
    // start or stop scan
    BLUETOOTH_MSG_START_SCAN,       //wparam 1=start,0=stop
    // answer pin
    BLUETOOTH_MSG_ANSWER_PIN,
    // connect or disconnect device
    BLUETOOTH_MSG_CONNECT_DEVICE,   //wparam 1=connect,0=disconnect
    // delete device
    BLUETOOTH_MSG_DELETE_DEVICE,
    // set device name
    BLUETOOTH_MSG_SET_DEVICE_NAME,
    // adjust device
    BLUETOOTH_MSG_ADJUST_DEVICE,
    // set auto connect
    BLUETOOTH_MSG_SET_AUTOCONNECT,
    // call out
    BLUETOOTH_MSG_CALLOUT,
    // call end
    BLUETOOTH_MSG_ENDCALL,
    // answer call
    BLUETOOTH_MSG_ANSWERCALL,
    // callin
    BLUETOOTH_MSG_CALLIN,
    // 设备初始化完成消息
    BLUETOOTH_MSG_DEVICE_POWER,
    // 蓝牙发给主线程进行重拨，因为重拨会刷新界面，qt不允许在子线程刷界面
    BLUETOOTH_MSG_BLUETOOTH_REDIAL,
    // 蓝牙发来挂断消息
    BLUETOOTH_MSG_BLUETOOTH_TERMINAL,
    // 蓝牙状态改变
    BLUETOOTH_MSG_STATE_CHANGE,         //wparam 1:ENABLE 0:DISABLE
    // DSSKEY 自动设置开关
    BLUETOOTH_MSG_SETDSSKEY,            //wparam 1:ENABLE 0: DISABLE
    // 查看蓝牙模块状态消息
    BLUETOOTH_MSG_GETSTATUS,
    // 联系人自动同步开关
    BLUETOOTH_MSG_REQ_SYNC,             //wparam:device type lparam:device handle extra data:Mac
    // 联系人同步完成
    BLUETOOTH_MSG_DIR_DONE,             //wParam 1=success 0=failed 2=remove mobilecontact 3=add mobilecontact group;extra:file path(if success)
    // 显示PIN码
    BLUETOOTH_MSG_SHOW_PIN,             //wparam device type,lparam:device handle extra data:pin
    // 蓝牙手机通话消息
    BLUETOOTH_MSG_MOBILE_STATECHANGE,   //wparam 1:call state change 0:connect state change, lparam connect state,extra:btkit_msg_t
    // 声道切换消息
    BLUETOOTH_MSG_CHANNEL_CHANGE,       //wParam:0 lParam:0
    // dongle状态变更
    BLUETOOTH_MSG_DONGLE_CHANGE,        //wParam 0:移除 1:插入
    // 蓝牙音乐播放操作
    BLUETOOTH_MSG_A2DP_PLAY_ACTION,     //wParam 0=play 1=stop 2=resume 3=pause
    // 蓝牙音乐开关变更
    BLUETOOTH_MSG_A2DP_STATE_CHANGE,    //wParam 0=disable 1=enable

    BLUETOOTH_MESSAGE_END = INNER_MSG_BLUETOOTH_MESSAGE_END,
};

// 当前操作类型
enum OPERATION_RESULT
{
    OP_NONE = 0,
    OP_UPDATE_LIST,             // 更新列表     1: 列表新增设备/0:列表设备减少
    OP_PAIR_RESULT,             // 配对结果
    OP_CONNECT_RESULT,          // 连接结果
    OP_SCAN_RESULT,             // 扫描结果
    OP_DISCONNECT_RESULT,       // 断开结果
    OP_DOWNLOAD_RESULT,         // 下载结果
    OP_UPDATE_DEVICE,           // 设备变更
};

// 蓝牙音乐播放操作
enum A2DP_PLAY_ACTION
{
    A2DP_PLAY_START = 0,
    A2DP_PLAY_STOP,
    A2DP_PLAY_RESUME,
    A2DP_PLAY_PAUSE
};

#endif  // __BLUETOOTH_MSG_DEFINE_H_
