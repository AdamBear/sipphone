#ifndef _TALKUILOGIC_ENUM_H
#define _TALKUILOGIC_ENUM_H

#include "talkuilogic/talkuilogiccondition.h"

//普通图标类型
enum TALKUI_ICON
{
    TALKUI_ICON_NONE,
    TALKUI_ICON_SIPENCRY,       //sip协议加密
    TALKUI_ICON_VOICEENCRY,     //语音加密
    TALKUI_ICON_SILENCE,        //来电界面的静音图标
    TALKUI_ICON_HD,             //高清通话图标
    TALKUI_ICON_AMR,
    TALKUI_ICON_OPUS,
    TALKUI_ICON_RECORD,         //录音图标
    TALKUI_ICON_CANNOT_BE_RECORDED, //无法录音
    TALKUI_ICON_CANNOT_BE_STOPPED_RECORD,   //无法停止录音
    TALKUI_ICON_CANNOT_BE_STARTED_RECORD,   //无法开始录音
    TALKUI_ICON_RECORD_FULL,        //录音已满
//  TALKUI_ICON_BLUETOOTH_CONNECTED,        //蓝牙连接图标
//  TALKUI_ICON_BLUETOOTH_UNCONNECTED,      //蓝牙未连接图标
//  TALKUI_ICON_BLUETOOTH_DISABLE,          //蓝牙未开启
    TALKUI_ICON_CALL_RECORD_NORMAL,         //录音
    TALKUI_ICON_CALL_RECORD_PAUSE,          //录音暂停

    TALKUI_ICON_DND,
};

//通道图标类型
enum TALKUI_CHANNEL_ICON
{
    TALKUI_CHANNEL_ICON_UNKNOWN,                //未知
    TALKUI_CHANNEL_ICON_HANDFREE,           //免提图标
    TALKUI_CHANNEL_ICON_HANDSET,            //手柄
    TALKUI_CHANNEL_ICON_HEADSET,            //耳麦

};

//蓝牙图标类型
enum TALKUI_BLUETOOTH_ICON
{
    TALKUI_BLUETOOTH_ICON_UNKNOWN,          //未知
//  TALKUI_BLUETOOTH_ICON_DISABLE,          //蓝牙未开启
    TALKUI_BLUETOOTH_ICON_UNCONNECTED,      //蓝牙未连接图标
    TALKUI_BLUETOOTH_ICON_MOBILE,           //蓝牙手机图标
    TALKUI_BLUETOOTH_ICON_HEADSET,          //蓝牙耳机图标

};

//通话信息显示方式
// enum SHOW_CONTACTDETAIL_TYPE
// {
//  SHOW_NAME_AND_NUMBER = 0,
//  SHOW_NUMBER_AND_NAME = 1,
//  SHOW_ONLY_NAME = 2,
//  SHOW_ONLY_NUMBER = 3,
//  SHOW_FULL_CONTACT_INFO = 4,
// };


#endif          //_TALKUILOGIC_ENUM_H
