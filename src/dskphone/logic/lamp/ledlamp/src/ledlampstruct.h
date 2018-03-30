#ifndef _LEDLAMPSTRUCTH_
#define _LEDLAMPSTRUCTH_
#include <ylstring.h>
#include <ylhashmap.h>
#include <yllist.h>
#include <ylvector.h>
#include <utility>
#include "modledlamp.h"
#include "path_define.h"
// #ifdef _T3X
// #include "t3xledlampdefine.h"
// #endif
// #ifdef _T4X
// #include "t4xledlampdefine.h"
// #endif

/* 定义事件类型对应的字符串通 */
#define kstrInCall "InCall"
#define kstrDialing "Dialing"
#define kstrConneting "Conneting"
#define kstrTalking "Talking"
#define kstrHold "Hold"
#define kstrMute "Mute"
#define kstrFailAccount "FailAccount"
#define kstrNewMessage "NewMessage"
#define kstrMissCall "MissCall"
#define kstrForwardCall "ForwardCall"
#define kstrFailUpgrade "FailUpgrade"
#define kstrabnormalNetwork "abnormalNetwork"
#define kstrUpgrading "Upgrading"
#define kstrSMS "sms"
#define kstrPower "power"
#define kstrHeadset "headset"
#define kstrSpeaker "speaker"
#define kstrCustom "Custom"
#define kstrPowerDown "PowerDown"
#define kstrAcdAlert "AcdAlert"
#define kstrVoiceMail "VoiceMail"
#define kstrTextMessage "TextMessage"
#define kstrPowerOn "PowerOn"
#define kstrPowerOff "PowerOff"
#define kstrCommon "Common"
#define kstrHold "Hold"
#define kstrMutePowerLight "MutePowerLight"
#define kstrBackLight "BackLight"
#define kstrMissCall "MissCall"
#define kstrTCPTest         "TCPTest"
#define kstrnormalNetwork   "normalNetwork"
#define kstrUpgrading       "Upgrading"
#define kstrRegisterMode    "RegisterMode"
#define kstrPaging          "Paging"
#define kstrTestMode        "TestMode"
#define kstrFactory         "Factory"
#define kstrHandsetRegisted "HandsetRegisted"
#define kstrNoHandsetRegisted  "NoHandsetRegisted"
#define kstrTBRTest          "TBRTest"
#define kstrAccountRegistered "AccountRegistered"
#define kstrPowerSaving "PowerSaving"

#define kstrDssKeyRed1 "DsskeyRed1"
#define kstrDssKeyRed2 "DsskeyRed2"
#define kstrDssKeyRed3 "DsskeyRed3"
#define kstrDssKeyRed4 "DsskeyRed4"
#define kstrDssKeyRed5 "DsskeyRed5"
#define kstrDssKeyRed6 "DsskeyRed6"
#define kstrDssKeyRed7 "DsskeyRed7"
#define kstrDssKeyRed8 "DsskeyRed8"
#define kstrDssKeyRed9 "DsskeyRed9"
#define kstrDssKeyRed10 "DsskeyRed10"
#define kstrDssKeyGreen1 "DsskeyGreen1"
#define kstrDssKeyGreen2 "DsskeyGreen2"
#define kstrDssKeyGreen3 "DsskeyGreen3"
#define kstrDssKeyGreen4 "DsskeyGreen4"
#define kstrDssKeyGreen5 "DsskeyGreen5"
#define kstrDssKeyGreen6 "DsskeyGreen6"
#define kstrDssKeyGreen7 "DsskeyGreen7"
#define kstrDssKeyGreen8 "DsskeyGreen8"
#define kstrDssKeyGreen9 "DsskeyGreen9"
#define kstrDssKeyGreen10 "DsskeyGreen10"
#define kstrDssKeyOrange1 "DsskeyOrange1"
#define kstrDssKeyOrange2 "DsskeyOrange2"
#define kstrDssKeyOrange3 "DsskeyOrange3"
#define kstrDssKeyOrange4 "DsskeyOrange4"
#define kstrDssKeyOrange5 "DsskeyOrange5"
#define kstrDssKeyOrange6 "DsskeyOrange6"
#define kstrDssKeyOrange7 "DsskeyOrange7"
#define kstrDssKeyOrange8 "DsskeyOrange8"
#define kstrDssKeyOrange9 "DsskeyOrange9"
#define kstrDssKeyOrange10 "DsskeyOrange10"

#define kstrLineKey1 "linekey1"
#define kstrLineKey2 "linekey2"
#define kstrLineKey3 "linekey3"
#define kstrLineKey4 "linekey4"
#define kstrLineKey5 "linekey5"
#define kstrLineKey6 "linekey6"
#define kstrMemoryKeyRed1 "memorykeyred1"
#define kstrMemoryKeyRed2 "memorykeyred2"
#define kstrMemoryKeyRed3 "memorykeyred3"
#define kstrMemoryKeyRed4 "memorykeyred4"
#define kstrMemoryKeyRed5 "memorykeyred5"
#define kstrMemoryKeyRed6 "memorykeyred6"
#define kstrMemoryKeyRed7 "memorykeyred7"
#define kstrMemoryKeyRed8 "memorykeyred8"
#define kstrMemoryKeyRed9 "memorykeyred9"
#define kstrMemoryKeyRed10 "memorykeyred10"
#define kstrMemoryKeyGreen1 "memorykeygreen1"
#define kstrMemoryKeyGreen2 "memorykeygreen2"
#define kstrMemoryKeyGreen3 "memorykeygreen3"
#define kstrMemoryKeyGreen4 "memorykeygreen4"
#define kstrMemoryKeyGreen5 "memorykeygreen5"
#define kstrMemoryKeyGreen6 "memorykeygreen6"
#define kstrMemoryKeyGreen7 "memorykeygreen7"
#define kstrMemoryKeyGreen8 "memorykeygreen8"
#define kstrMemoryKeyGreen9 "memorykeygreen9"
#define kstrMemoryKeyGreen10 "memorykeygreen10"

#define kstrAutopFinish "AutopFinish"

#define kstrVideoInCall "VideoInCall"
#define kstrVideoTalking "VideoTalking"
#define kstrVideoHold "VideoHold"
#define kstrVideoMute "VideoMute"
#define kstrVideoIdle "VideoIdle"

/*
<!- -规则 -->
<event priority="%d优先级" event="%s事件" lightTime="%d(ms)亮时间" darkTime="%d(ms)暗时间" lightID="\d位移|灯号"/>
*/

/* 定义事件类型枚举值与字串的映射表 */
typedef YLHashMap< yl::string, LAMP_EVENTTYPE, STRING_HASH> MAPNAMETYPE;
static MAPNAMETYPE kmapNameTypeCache;
/* 初始化映射表 */
void _InitKmap();
/* 根据字符串, 从映射表中获取相应事件类型 */
LAMP_EVENTTYPE _GetEventTypeByStr(const yl::string & strName);

//Led的颜色
enum LED_COLOR
{
    LED_COLOR_YELLOW = 0,
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_ORANGE,
};

//LedID--Value,
typedef YLHashMap<int, LED_COLOR> MPA_LED_COLOR;//Led需要的颜色的值

/* 事件单元( 优先级 + 亮时间 + 暗时间 + 灯号) */
struct tEventUnit
{
    int _npriority;  /* 优先级 */
    int _nLightTime; /* 亮时间 */
    int _nDarkTime; /* 暗时间 */
    int _nLampNum; /* Lamp ID*/
    MPA_LED_COLOR MapLed;

    // 添加默认值，以便对不存在的灯类型做判断
    tEventUnit()
    {
        _npriority = -1;
        _nLightTime = -1;
        _nDarkTime = -1;
        _nLampNum = -1;
    }
};
typedef YLHashMap< LAMP_EVENTTYPE, tEventUnit > MAPEVENTUNIT;

/* 灯的属性 (优先级 + 亮时间 + 暗时间 + 事件类型) */
struct tAttrStruct
{
    int nPriority;
    int nLightTime;
    int nDarkTime;
    LED_COLOR eLedColor;
    LAMP_EVENTTYPE eEventType;
};

typedef YLList< tAttrStruct > ATTRLIST;
struct tLightInfo // 与一个灯相关的结构体
{
    int nLightNum; /* 灯号 */
    ATTRLIST attrList; /* 属性列表 */
};

typedef YLVector<tLightInfo *> VECTORLIGHTINFO; /* 数组: 所有灯信息(灯号 + 属性) */

//---------------------kstrLedColorFilePath----------------
//每个颜色对应的LEDID
typedef YLHashMap<LED_COLOR, int> MAPCOLORVALUE;

//LedLampID-<Color,Ledid>
typedef YLHashMap<int, MAPCOLORVALUE> MAPLEDCOLORVALUE;


/* xml文件路径 */
static yl::string kstrXmlFilePath = RESOURCE_PATH "device-res/ledLamp/LedLamp.xml";
static yl::string kstrLedColorFilePath = RESOURCE_PATH "device-res/ledLamp/LedLampColor.xml";

#endif
