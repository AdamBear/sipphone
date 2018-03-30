#include "ledlampstruct.h"

/* 初始化映射表 */
void _InitKmap()
{
    kmapNameTypeCache.clear();

    //固定按键灯
    kmapNameTypeCache[kstrSMS] = LAMP_EVENTTYPE_SMS;
    kmapNameTypeCache[kstrHeadset] = LAMP_EVENTTYPE_HEADSET;
    kmapNameTypeCache[kstrSpeaker] = LAMP_EVENTTYPE_SPEAKER;
    kmapNameTypeCache[kstrMute] = LAMP_EVENTTYPE_MUTE;

    //电源灯时间
    kmapNameTypeCache[kstrInCall] = LAMP_EVENTTYPE_INCALL;
    kmapNameTypeCache[kstrTalking] = LAMP_EVENTTYPE_TALKING;
    kmapNameTypeCache[kstrVoiceMail] = LAMP_EVENTTYPE_MESSAGE;
    kmapNameTypeCache[kstrTextMessage] = LAMP_EVENTTYPE_MESSAGE;
    kmapNameTypeCache[kstrAcdAlert] = LAMP_EVENTTYPE_ACDALERT;
    kmapNameTypeCache[kstrCommon] = LAMP_EVENTTYPE_COMMON;
    kmapNameTypeCache[kstrHold] = LAMP_EVENTTYPE_HOLD;
    kmapNameTypeCache[kstrMutePowerLight] = LAMP_EVENTTYPE_MUTE_POWER_LIGHT;
    kmapNameTypeCache[kstrBackLight] = LAMP_EVENTTYPE_BACKLIGHT;

    //DECT
#ifdef _DECT
    kmapNameTypeCache[kstrTCPTest] = LAMP_EVENTTYPE_TCPTEST;
    kmapNameTypeCache[kstrnormalNetwork] = LAMP_EVENTTYPE_NORMALNETWORK;
    kmapNameTypeCache[kstrUpgrading] = LAMP_EVENTTYPE_UPGRADING;
    kmapNameTypeCache[kstrRegisterMode] = LAMP_EVENTTYPE_REGISTERMODE;
    kmapNameTypeCache[kstrPaging] = LAMP_EVENTTYPE_PAGING;
    kmapNameTypeCache[kstrTestMode] = LAMP_EVENTTYPE_TESTMODE;
    kmapNameTypeCache[kstrFactory] = LAMP_EVENTTYPE_FACTORY;
    kmapNameTypeCache[kstrHandsetRegisted] = LAMP_EVENTTYPE_HANDSETREGISTED;
    kmapNameTypeCache[kstrNoHandsetRegisted] = LAMP_EVENTTYPE_NOHANDSETREGISTED;
    kmapNameTypeCache[kstrTBRTest] = LAMP_EVENTTYPE_TBRTEST;
    kmapNameTypeCache[kstrAccountRegistered] = LAMP_EVENTTYPE_ACCOUNTREGISTERED;
    kmapNameTypeCache[kstrPowerOn] = LAMP_EVENTTYPE_POWER_ON;
    kmapNameTypeCache[kstrabnormalNetwork] = LAMP_EVENTTYPE_ABNORMALNETWORK;
    kmapNameTypeCache[kstrnormalNetwork] = LAMP_EVENTTYPE_NORMALNETWORK;
    kmapNameTypeCache[kstrDialing] = LAMP_EVENTTYPE_DIALING;
    kmapNameTypeCache[kstrConneting] = LAMP_EVENTTYPE_CONNETING;
    kmapNameTypeCache[kstrFailAccount] = LAMP_EVENTTYPE_FAILACCOUNT;
    kmapNameTypeCache[kstrMissCall] = LAMP_EVENTTYPE_MISSCALL;
    kmapNameTypeCache[kstrForwardCall] = LAMP_EVENTTYPE_FORWARDCALL;
#else
    kmapNameTypeCache[kstrMissCall] = LAMP_EVENTTYPE_MISSCALL;
#endif // _DECT

    //Dsskey
    kmapNameTypeCache[kstrLineKey1] = LAMP_EVENTTYPE_LINEKEY1;
    kmapNameTypeCache[kstrLineKey2] = LAMP_EVENTTYPE_LINEKEY2;
    kmapNameTypeCache[kstrLineKey3] = LAMP_EVENTTYPE_LINEKEY3;
    kmapNameTypeCache[kstrLineKey4] = LAMP_EVENTTYPE_LINEKEY4;
    kmapNameTypeCache[kstrLineKey5] = LAMP_EVENTTYPE_LINEKEY5;
    kmapNameTypeCache[kstrLineKey6] = LAMP_EVENTTYPE_LINEKEY6;
    kmapNameTypeCache[kstrMemoryKeyRed1] = LAMP_EVENTTYPE_MEMORYKEYRED1;
    kmapNameTypeCache[kstrMemoryKeyRed2] = LAMP_EVENTTYPE_MEMORYKEYRED2;
    kmapNameTypeCache[kstrMemoryKeyRed3] = LAMP_EVENTTYPE_MEMORYKEYRED3;
    kmapNameTypeCache[kstrMemoryKeyRed4] = LAMP_EVENTTYPE_MEMORYKEYRED4;
    kmapNameTypeCache[kstrMemoryKeyRed5] = LAMP_EVENTTYPE_MEMORYKEYRED5;
    kmapNameTypeCache[kstrMemoryKeyRed6] = LAMP_EVENTTYPE_MEMORYKEYRED6;
    kmapNameTypeCache[kstrMemoryKeyRed7] = LAMP_EVENTTYPE_MEMORYKEYRED7;
    kmapNameTypeCache[kstrMemoryKeyRed8] = LAMP_EVENTTYPE_MEMORYKEYRED8;
    kmapNameTypeCache[kstrMemoryKeyRed9] = LAMP_EVENTTYPE_MEMORYKEYRED9;
    kmapNameTypeCache[kstrMemoryKeyRed10] = LAMP_EVENTTYPE_MEMORYKEYRED10;
    kmapNameTypeCache[kstrMemoryKeyGreen1] = LAMP_EVENTTYPE_MEMORYKEYGREEN1;
    kmapNameTypeCache[kstrMemoryKeyGreen2] = LAMP_EVENTTYPE_MEMORYKEYGREEN2;
    kmapNameTypeCache[kstrMemoryKeyGreen3] = LAMP_EVENTTYPE_MEMORYKEYGREEN3;
    kmapNameTypeCache[kstrMemoryKeyGreen4] = LAMP_EVENTTYPE_MEMORYKEYGREEN4;
    kmapNameTypeCache[kstrMemoryKeyGreen5] = LAMP_EVENTTYPE_MEMORYKEYGREEN5;
    kmapNameTypeCache[kstrMemoryKeyGreen6] = LAMP_EVENTTYPE_MEMORYKEYGREEN6;
    kmapNameTypeCache[kstrMemoryKeyGreen7] = LAMP_EVENTTYPE_MEMORYKEYGREEN7;
    kmapNameTypeCache[kstrMemoryKeyGreen8] = LAMP_EVENTTYPE_MEMORYKEYGREEN8;
    kmapNameTypeCache[kstrMemoryKeyGreen9] = LAMP_EVENTTYPE_MEMORYKEYGREEN9;
    kmapNameTypeCache[kstrMemoryKeyGreen10] = LAMP_EVENTTYPE_MEMORYKEYGREEN10;

    kmapNameTypeCache[kstrDssKeyRed1] = LAMP_EVENTTYPE_DSSKEYRED1;
    kmapNameTypeCache[kstrDssKeyRed2] = LAMP_EVENTTYPE_DSSKEYRED2;
    kmapNameTypeCache[kstrDssKeyRed3] = LAMP_EVENTTYPE_DSSKEYRED3;
    kmapNameTypeCache[kstrDssKeyRed4] = LAMP_EVENTTYPE_DSSKEYRED4;
    kmapNameTypeCache[kstrDssKeyRed5] = LAMP_EVENTTYPE_DSSKEYRED5;
    kmapNameTypeCache[kstrDssKeyRed6] = LAMP_EVENTTYPE_DSSKEYRED6;
    kmapNameTypeCache[kstrDssKeyRed7] = LAMP_EVENTTYPE_DSSKEYRED7;
    kmapNameTypeCache[kstrDssKeyRed8] = LAMP_EVENTTYPE_DSSKEYRED8;
    kmapNameTypeCache[kstrDssKeyRed9] = LAMP_EVENTTYPE_DSSKEYRED9;
    kmapNameTypeCache[kstrDssKeyRed10] = LAMP_EVENTTYPE_DSSKEYRED10;
    kmapNameTypeCache[kstrDssKeyGreen1] = LAMP_EVENTTYPE_DSSKEYGREEN1;
    kmapNameTypeCache[kstrDssKeyGreen2] = LAMP_EVENTTYPE_DSSKEYGREEN2;
    kmapNameTypeCache[kstrDssKeyGreen3] = LAMP_EVENTTYPE_DSSKEYGREEN3;
    kmapNameTypeCache[kstrDssKeyGreen4] = LAMP_EVENTTYPE_DSSKEYGREEN4;
    kmapNameTypeCache[kstrDssKeyGreen5] = LAMP_EVENTTYPE_DSSKEYGREEN5;
    kmapNameTypeCache[kstrDssKeyGreen6] = LAMP_EVENTTYPE_DSSKEYGREEN6;
    kmapNameTypeCache[kstrDssKeyGreen7] = LAMP_EVENTTYPE_DSSKEYGREEN7;
    kmapNameTypeCache[kstrDssKeyGreen8] = LAMP_EVENTTYPE_DSSKEYGREEN8;
    kmapNameTypeCache[kstrDssKeyGreen9] = LAMP_EVENTTYPE_DSSKEYGREEN9;
    kmapNameTypeCache[kstrDssKeyGreen10] = LAMP_EVENTTYPE_DSSKEYGREEN10;
    kmapNameTypeCache[kstrDssKeyOrange1] = LAMP_EVENTTYPE_DSSKEYORANGE1;
    kmapNameTypeCache[kstrDssKeyOrange2] = LAMP_EVENTTYPE_DSSKEYORANGE2;
    kmapNameTypeCache[kstrDssKeyOrange3] = LAMP_EVENTTYPE_DSSKEYORANGE3;
    kmapNameTypeCache[kstrDssKeyOrange4] = LAMP_EVENTTYPE_DSSKEYORANGE4;
    kmapNameTypeCache[kstrDssKeyOrange5] = LAMP_EVENTTYPE_DSSKEYORANGE5;
    kmapNameTypeCache[kstrDssKeyOrange6] = LAMP_EVENTTYPE_DSSKEYORANGE6;
    kmapNameTypeCache[kstrDssKeyOrange7] = LAMP_EVENTTYPE_DSSKEYORANGE7;
    kmapNameTypeCache[kstrDssKeyOrange8] = LAMP_EVENTTYPE_DSSKEYORANGE8;
    kmapNameTypeCache[kstrDssKeyOrange9] = LAMP_EVENTTYPE_DSSKEYORANGE9;
    kmapNameTypeCache[kstrDssKeyOrange10] = LAMP_EVENTTYPE_DSSKEYORANGE10;

    kmapNameTypeCache[kstrAutopFinish] = LAMP_EVENTTYPE_AUTOPFINISH;

    kmapNameTypeCache[kstrVideoInCall] = LAMP_EVENTTYPE_VIDEO_INCALL;
    kmapNameTypeCache[kstrVideoTalking] = LAMP_EVENTTYPE_VIDEO_TALKING;
    kmapNameTypeCache[kstrVideoHold] = LAMP_EVENTTYPE_VIDEO_HOLD;
    kmapNameTypeCache[kstrVideoMute] = LAMP_EVENTTYPE_VIDEO_MUTE;
    kmapNameTypeCache[kstrVideoIdle] = LAMP_EVENTTYPE_VIDEO_IDLE;

    kmapNameTypeCache[kstrPowerSaving] = LAMP_EVENTTYPE_POWERSAVING;

}


/* 根据字符串, 从映射表中获取相应事件类型 */
LAMP_EVENTTYPE _GetEventTypeByStr(const yl::string & strName)
{
    MAPNAMETYPE::iterator it =  kmapNameTypeCache.find(strName);
    if (it != kmapNameTypeCache.end())
    {
        return it->second;
    }
    return LAMP_EVENTTYPE_NONE;
}
