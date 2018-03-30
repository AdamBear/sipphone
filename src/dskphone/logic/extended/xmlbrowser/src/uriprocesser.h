#ifndef __URI_PROCESSER_H__
#define __URI_PROCESSER_H__

#ifdef IF_FEATURE_XMLBROWSER

#include "lamp/ledlamp/include/modledlamp.h"
#include "lamp/ledlamp/include/led_data_define.h"
#include "dsskey/include/dsskey_enumtypes.h"

// 灯号
#define LAMP_EVENTTYPE_POWSER 100
enum LAMP_INDEX
{
    LAMP_SPEAKER = LAMP_EVENTTYPE_SPEAKER,    //8
    LAMP_HEADSET = LAMP_EVENTTYPE_HEADSET,    //9
    LAMP_SMS = LAMP_EVENTTYPE_SMS,            //10
    LAMP_MUTE = LAMP_EVENTTYPE_MUTE,          //11
    LAMP_BACKLIGHT = LAMP_EVENTTYPE_BACKLIGHT,//12
    LAMP_POWSER = LAMP_EVENTTYPE_COMMON,      //100
};

// URI的处理
class CURIProcesser
{
    friend class CElementDirectory;
public:
    // 对外接口处理URI
    void ProcessURI(const yl::string & strURI);
    // 处理拨号呼出类型的URI
    void ProcessDial(const char * szURI);

private:
    // 处理http(s)类型的URI
    void ProcessHttp(const char * szURI);
    // 处理灯控制类型的URI
    void ProcessLed(const char * szURI);
    // 处理发送按键类型的URI
    void ProcessKey(const char * szURI);
    // 处理播放音频类型的URI
    void ProcessPlayWave(const char * szURI);
    // 处理停止播放音频
    void ProcessStopWave();
    // 处理命令类型的URI
    void ProcessCommand(const char * szURI);

    // 检查URI是否为空
    bool IsURIEmpty(const char * szURI);

    // 对扩展台按键灯的处理
    void ProcessExpLed(const char * szLed, const char * szStatus);

    // 发送按键
    void SendKey(const yl::string & strKey);

    // 获取灯类型
    LIGHT_Type GetLightType(const LEDCOLOR_LTYE eColor, const LEDSTATUS_TYPE eStatus);

    // 获取灯状态
    LEDSTATUS_TYPE GetLightStatus(const char * szStatus);
    // 获取灯颜色
    LEDCOLOR_LTYE GetLightColor(const char * szColor);

    // 获取灯键值
    int GetLightIndex(const char * szLed);
    // 获取灯键值
    bool GetDsskeyLightInfo(const char * szLed, int & iKeyIndex, LEDCOLOR_LTYE & eColorType,
                            DSSKEY_MODULE_TYPE & dsskeyModule);
    // 获取不同机型对应的灯号
    int GetLampID(int iLightIndex);

#if IF_BUG_23718
    int UploadSystemInfo();
#endif
};

// 获取全局唯一的URI处理对象
#define _URIProcesser GetURIProcesserInstance()
CURIProcesser & GetURIProcesserInstance();

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // __URI_PROCESSER_H__
