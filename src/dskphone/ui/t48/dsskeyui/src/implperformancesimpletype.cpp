#include "implperformancesimpletype.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyperformancemanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "edk/include/modedk.h"
#include "dsskey/include/moddsskey.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"

#define  MAX_BUF_LEN            256

////////////////DT_VOICEMAIL/////////////////////////
ImplDsskeyPerformanceVoiceMail::ImplDsskeyPerformanceVoiceMail()
    : ImplDsskeyPerformanceBase(DT_VOICEMAIL)
{

}

ImplDsskeyPerformanceVoiceMail::~ImplDsskeyPerformanceVoiceMail()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceVoiceMail::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_VOICE_MAIL;
    return true;
}

///////////////DT_CUSTOM/////////////////////////
ImplDsskeyPerformanceDTMF::ImplDsskeyPerformanceDTMF()
    : ImplDsskeyPerformanceBase(DT_CUSTOM)
{

}

ImplDsskeyPerformanceDTMF::~ImplDsskeyPerformanceDTMF()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceDTMF::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_DTMF;
    return true;
}

///////////////DT_XMLBROWSER/////////////////////////
ImplDsskeyPerformanceXmlBrowser::ImplDsskeyPerformanceXmlBrowser()
    : ImplDsskeyPerformanceBase(DT_XMLBROWSER)
{

}

ImplDsskeyPerformanceXmlBrowser::~ImplDsskeyPerformanceXmlBrowser()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceXmlBrowser::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_XML_BROWSER;
#ifdef IF_FEATURE_GENBAND
    switch (logicData.iStatus)
    {
    case DS_XMLBROSER_LOAD_FINISH:
        {
            //绿色图标
            //to do -syl
            IconData.strIconPath = PIC_DSS_XML_BROWSER_HTTP_SUC;
            IconData.eRefreshType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    case DS_XMLBROSER_LOAD_ERROE:
        {
            //红色图标
            //to do -syl
            IconData.strIconPath = PIC_DSS_XML_BROWSER_HTTP_FAIL;
            IconData.eRefreshType = REFRESH_TYPE_FLASH;
        }
        break;
    default:
        {
            IconData.eRefreshType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    }
#endif

    return true;
}

bool ImplDsskeyPerformanceXmlBrowser::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    switch (logicData.iStatus)
    {
    case DS_XMLBROSER_LOAD_FINISH:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            LigntData.eLightFlashType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    case DS_XMLBROSER_LOAD_ERROE:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_RED;
            LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
        }
        break;
    default:
        {
            LigntData.eLightColorType = LED_COLOR_OFF;
            LigntData.eLightFlashType = REFRESH_TYPE_NO_FLASH;
        }
        break;
    }

    return true;
}

#if IF_BUG_31595
///////////////DT_XMLBROWSER_INFO/////////////////////////
ImplDsskeyPerformanceXmlBrowserInfo::ImplDsskeyPerformanceXmlBrowserInfo()
    : ImplDsskeyPerformanceBase(DT_XMLBROWSER_INFO)
{

}

ImplDsskeyPerformanceXmlBrowserInfo::~ImplDsskeyPerformanceXmlBrowserInfo()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceXmlBrowserInfo::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_XML_BROWSER;
    return true;
}

bool ImplDsskeyPerformanceXmlBrowserInfo::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    return true;
}
#endif

#if IF_BUG_14856
///////////////DT_MISSED_CALL/////////////////////////
ImplDsskeyPerformanceMissedCall::ImplDsskeyPerformanceMissedCall()
    : ImplDsskeyPerformanceBase(DT_MISSED_CALL)
{

}

ImplDsskeyPerformanceMissedCall::~ImplDsskeyPerformanceMissedCall()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMissedCall::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_LOCALGROUP;
    return true;
}
#endif


///////////////DT_LDAP/////////////////////////
ImplDsskeyPerformanceLDAP::ImplDsskeyPerformanceLDAP()
    : ImplDsskeyPerformanceBase(DT_LDAP)
{

}

ImplDsskeyPerformanceLDAP::~ImplDsskeyPerformanceLDAP()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceLDAP::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_LDAP;
    return true;
}

///////////////DT_FAVORITES/////////////////////////
ImplDsskeyPerformanceFavorite::ImplDsskeyPerformanceFavorite()
    : ImplDsskeyPerformanceBase(DT_FAVORITES)
{

}

ImplDsskeyPerformanceFavorite::~ImplDsskeyPerformanceFavorite()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceFavorite::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_FAVORITES;
    return true;
}

///////////////DT_CONF/////////////////////////
ImplDsskeyPerformanceConference::ImplDsskeyPerformanceConference()
    : ImplDsskeyPerformanceBase(DT_CONF)
{

}

ImplDsskeyPerformanceConference::~ImplDsskeyPerformanceConference()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceConference::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_CONFERENCE;
    return true;
}

///////////////DT_TRAN/////////////////////////
ImplDsskeyPerformanceTransfer::ImplDsskeyPerformanceTransfer()
    : ImplDsskeyPerformanceBase(DT_TRAN)
{

}

ImplDsskeyPerformanceTransfer::~ImplDsskeyPerformanceTransfer()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceTransfer::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_TRANSFER;
    return true;
}

///////////////DT_HOLD/////////////////////////
ImplDsskeyPerformanceHold::ImplDsskeyPerformanceHold()
    : ImplDsskeyPerformanceBase(DT_HOLD)
{

}

ImplDsskeyPerformanceHold::~ImplDsskeyPerformanceHold()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceHold::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_HOLD;
    IconData.strIconPath = PIC_DSS_HOLD;
    return true;
}

///////////////DT_REDIAL/////////////////////////
ImplDsskeyPerformanceRedial::ImplDsskeyPerformanceRedial()
    : ImplDsskeyPerformanceBase(DT_REDIAL)
{

}

ImplDsskeyPerformanceRedial::~ImplDsskeyPerformanceRedial()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceRedial::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_REDIAL;
    return true;
}

///////////////DT_CALLRETURN/////////////////////////
ImplDsskeyPerformanceCallReturn::ImplDsskeyPerformanceCallReturn()
    : ImplDsskeyPerformanceBase(DT_CALLRETURN)
{

}

ImplDsskeyPerformanceCallReturn::~ImplDsskeyPerformanceCallReturn()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceCallReturn::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_CALLRETURN;
    return true;
}

///////////////DT_SMS/////////////////////////
ImplDsskeyPerformanceSMS::ImplDsskeyPerformanceSMS()
    : ImplDsskeyPerformanceBase(DT_SMS)
{

}

ImplDsskeyPerformanceSMS::~ImplDsskeyPerformanceSMS()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceSMS::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_SMS;
    IconData.strIconPath = PIC_DSS_SMS;
    return true;
}

#if IF_FEATURE_METASWITCH_VOICEMAIL
///////////////DT_MESSAGE_LIST/////////////////////////
ImplDsskeyPerformanceMessageList::ImplDsskeyPerformanceMessageList()
    : ImplDsskeyPerformanceBase(DT_MESSAGE_LIST)
{

}

ImplDsskeyPerformanceMessageList::~ImplDsskeyPerformanceMessageList()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMessageList::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_MTSW_MESSAGELIST;
    return true;
}
#endif

///////////////DT_GROUPLISTEN/////////////////////////
ImplDsskeyPerformanceGroupListen::ImplDsskeyPerformanceGroupListen()
    : ImplDsskeyPerformanceBase(DT_GROUPLISTEN)
{

}

ImplDsskeyPerformanceGroupListen::~ImplDsskeyPerformanceGroupListen()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceGroupListen::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_GROUP_LISTENING;
    return true;
}

///////////////DT_PUBLICHOLD/////////////////////////
ImplDsskeyPerformancePublicHold::ImplDsskeyPerformancePublicHold()
    : ImplDsskeyPerformanceBase(DT_PUBLICHOLD)
{

}

ImplDsskeyPerformancePublicHold::~ImplDsskeyPerformancePublicHold()
{

}

// 获取图标显示
bool ImplDsskeyPerformancePublicHold::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_HOLD;
    IconData.strIconPath = PIC_DSS_PUBLIC_HOLD;
    return true;
}

///////////////DT_PRIVATEHOLD/////////////////////////
ImplDsskeyPerformancePrivateHold::ImplDsskeyPerformancePrivateHold()
    : ImplDsskeyPerformanceBase(DT_PRIVATEHOLD)
{

}

ImplDsskeyPerformancePrivateHold::~ImplDsskeyPerformancePrivateHold()
{

}

// 获取图标显示
bool ImplDsskeyPerformancePrivateHold::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_HOLD;
    IconData.strIconPath = PIC_DSS_PRIVATE_HOLD;
    return true;
}

///////////////DT_ZERO/////////////////////////
ImplDsskeyPerformanceZeroTouch::ImplDsskeyPerformanceZeroTouch()
    : ImplDsskeyPerformanceBase(DT_ZERO)
{

}

ImplDsskeyPerformanceZeroTouch::~ImplDsskeyPerformanceZeroTouch()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceZeroTouch::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_ZERO_TOUCH;
    return true;
}

///////////////DT_MEET_ME_CONFERENCE/////////////////////////
ImplDsskeyPerformanceMeetMe::ImplDsskeyPerformanceMeetMe()
    : ImplDsskeyPerformanceBase(DT_MEET_ME_CONFERENCE)
{

}

ImplDsskeyPerformanceMeetMe::~ImplDsskeyPerformanceMeetMe()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMeetMe::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_MEET_ME;
    return true;
}

///////////////DT_ACD_TRACE/////////////////////////
ImplDsskeyPerformanceACDTrace::ImplDsskeyPerformanceACDTrace()
    : ImplDsskeyPerformanceBase(DT_ACD_TRACE)
{

}

ImplDsskeyPerformanceACDTrace::~ImplDsskeyPerformanceACDTrace()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceACDTrace::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_ACD_TRACE;
    IconData.strIconPath = PIC_DSS_ACD_TRACE;
    return true;
}

///////////////DT_DISPCODE/////////////////////////
ImplDsskeyPerformanceDispcode::ImplDsskeyPerformanceDispcode()
    : ImplDsskeyPerformanceBase(DT_DISPCODE)
{

}

ImplDsskeyPerformanceDispcode::~ImplDsskeyPerformanceDispcode()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceDispcode::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_DISPCODE;
    IconData.strIconPath = PIC_DSS_DISPCODE;
    return true;
}

///////////////DT_ESCALATE/////////////////////////
ImplDsskeyPerformanceEscalate::ImplDsskeyPerformanceEscalate()
    : ImplDsskeyPerformanceBase(DT_ESCALATE)
{

}

ImplDsskeyPerformanceEscalate::~ImplDsskeyPerformanceEscalate()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceEscalate::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_ESCALATE;
    IconData.strIconPath = PIC_DSS_ESCALATE;
    return true;
}

///////////////DT_KEYPAD_LOCK/////////////////////////
ImplDsskeyPerformanceKeypadLock::ImplDsskeyPerformanceKeypadLock()
    : ImplDsskeyPerformanceBase(DT_KEYPAD_LOCK)
{

}

ImplDsskeyPerformanceKeypadLock::~ImplDsskeyPerformanceKeypadLock()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceKeypadLock::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_KEYPAD_LOCK;
    return true;
}

///////////////DT_BSFT_RETRIEVE_PARK/////////////////////////
ImplDsskeyPerformanceRetrievePark::ImplDsskeyPerformanceRetrievePark()
    : ImplDsskeyPerformanceBase(DT_BSFT_RETRIEVE_PARK)
{

}

ImplDsskeyPerformanceRetrievePark::~ImplDsskeyPerformanceRetrievePark()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceRetrievePark::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_RETRIEVE_PARK;
    return true;
}

///////////////DT_HOTDESKING/////////////////////////
ImplDsskeyPerformanceHotDesk::ImplDsskeyPerformanceHotDesk()
    : ImplDsskeyPerformanceBase(DT_HOTDESKING)
{

}

ImplDsskeyPerformanceHotDesk::~ImplDsskeyPerformanceHotDesk()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceHotDesk::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_HOTDESKING;
    return true;
}

///////////////DT_NETWORK_FAVORITE/////////////////////////
ImplDsskeyPerformanceNetworkFavorite::ImplDsskeyPerformanceNetworkFavorite()
    : ImplDsskeyPerformanceBase(DT_NETWORK_FAVORITE)
{

}

ImplDsskeyPerformanceNetworkFavorite::~ImplDsskeyPerformanceNetworkFavorite()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceNetworkFavorite::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (1 == configParser_GetConfigInt(kszEnableUC)
            && configParser_GetConfigInt(kszBroadsoftActive) == 1)
    {
        IconData.strIconPath = PIC_DSS_FAVORITES;
    }
    return true;
}

///////////////DT_BUDDIES/////////////////////////
ImplDsskeyPerformanceBuddies::ImplDsskeyPerformanceBuddies()
    : ImplDsskeyPerformanceBase(DT_BUDDIES)
{

}

ImplDsskeyPerformanceBuddies::~ImplDsskeyPerformanceBuddies()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceBuddies::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (1 == configParser_GetConfigInt(kszEnableUC)
            && configParser_GetConfigInt(kszBroadsoftActive) == 1)
    {
        IconData.strIconPath = PIC_DSS_BUDDIES;
    }
    return true;
}

///////////////DT_MY_STATUS/////////////////////////
ImplDsskeyPerformanceMyStatus::ImplDsskeyPerformanceMyStatus()
    : ImplDsskeyPerformanceBase(DT_MY_STATUS)
{

}

ImplDsskeyPerformanceMyStatus::~ImplDsskeyPerformanceMyStatus()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMyStatus::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (1 == configParser_GetConfigInt(kszEnableUC)
            && configParser_GetConfigInt(kszBroadsoftActive) == 1)
    {
        IconData.strIconPath = PIC_DSS_STATUS;
    }
    return true;
}

///////////////DT_PAGING_LIST/////////////////////////
ImplDsskeyPerformancePagingList::ImplDsskeyPerformancePagingList()
    : ImplDsskeyPerformanceBase(DT_PAGING_LIST)
{

}

ImplDsskeyPerformancePagingList::~ImplDsskeyPerformancePagingList()
{

}

// 获取图标显示
bool ImplDsskeyPerformancePagingList::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_PAGING;
    return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

///////////////DT_PREFIX/////////////////////////
ImplDsskeyPerformancePrefix::ImplDsskeyPerformancePrefix()
    : ImplDsskeyPerformanceBase(DT_PREFIX)
{

}

ImplDsskeyPerformancePrefix::~ImplDsskeyPerformancePrefix()
{

}

// 获取图标显示
bool ImplDsskeyPerformancePrefix::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_PREFIX_ICON;
    return true;
}

// 获取文字显示
bool ImplDsskeyPerformancePrefix::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    bool bExt = GetExtendFlag();
    if (logicData.strLabel.empty())
    {

        // 预拨号由于分2段显示，在这边做处理，无须判断逻辑的翻译长度值
        char szBuffer[MAX_BUF_LEN] = "";
        sprintf(szBuffer, "%s:%s", (LANG_TRANSLATE_DSSKEY_LABEL(TRID_PREFIX, bExt)).toUtf8().data(),
                logicData.strValue.c_str());
        LabelData.strFirstRowText = szBuffer;
    }
    else
    {
        LabelData.strFirstRowText = logicData.strLabel;
    }
    return true;
}

///////////////DT_FWD/////////////////////////
ImplDsskeyPerformanceForward::ImplDsskeyPerformanceForward()
    : ImplDsskeyPerformanceBase(DT_FWD)
{

}

ImplDsskeyPerformanceForward::~ImplDsskeyPerformanceForward()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceForward::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
#if IF_BUG_30107//Lydis 特质图标
    if (logicData.iStatus == DS_FWD_USING)
    {
        IconData.strIconPath = PIC_DSS_FORWARD_USING;
    }
    else
#endif
    {
        IconData.strIconPath = PIC_DSS_FORWARD;
    }
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceForward::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    if (logicData.iStatus == DS_FWD_USING)
    {
#if IF_FEATURE_32591
        LigntData.eLightColorType = LED_COLOR_TYPE_RED;
#else
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
#endif
    }
    return true;
}

///////////////DT_DND/////////////////////////
ImplDsskeyPerformanceDND::ImplDsskeyPerformanceDND()
    : ImplDsskeyPerformanceBase(DT_DND)
{

}

ImplDsskeyPerformanceDND::~ImplDsskeyPerformanceDND()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceDND::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_DND;
#if IF_FEATURE_32591
    if (logicData.iStatus == DS_DND_USING)
    {
        IconData.strIconPath = PIC_DSS_DND;
    }
    else
    {
        IconData.strIconPath = PIC_DSS_DND_OFF;
    }
#else
    IconData.strIconPath = PIC_DSS_DND;
#endif
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceDND::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    if (logicData.iStatus == DS_DND_USING)
    {
#if IF_FEATURE_32591
        LigntData.eLightColorType = LED_COLOR_TYPE_RED;
#else
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
#endif
    }
    return true;
}

///////////////DT_RECORD or DT_URLRECORD/////////////////////////
ImplDsskeyPerformanceRecord::ImplDsskeyPerformanceRecord(DssKey_Type type)
    : ImplDsskeyPerformanceBase(type)
{

}

ImplDsskeyPerformanceRecord::~ImplDsskeyPerformanceRecord()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceRecord::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (DS_RECORD_USING == logicData.iStatus)
    {
        IconData.strIconPath = PIC_DSS_RECORDING;
    }
    else
    {
        IconData.strIconPath = PIC_DSS_RECORD;
    }
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceRecord::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    if (DS_RECORD_USING == logicData.iStatus)
    {
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
    }
    return true;
}

///////////////DT_ACD/////////////////////////
ImplDsskeyPerformanceACD::ImplDsskeyPerformanceACD()
    : ImplDsskeyPerformanceBase(DT_ACD)
{

}

ImplDsskeyPerformanceACD::~ImplDsskeyPerformanceACD()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceACD::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    switch (logicData.iStatus)
    {
    case DS_ACD_AVAILABLE:
        {
            IconData.strIconPath = PIC_DSS_ACD_AVAILABLE;
        }
        break;
    case DS_ACD_UNAVAILABLE:
        {
            IconData.strIconPath = PIC_DSS_ACD_UNAVAILABLE;
        }
        break;
    case DS_ACD_WRAPUP:
        {
            IconData.strIconPath = PIC_DSS_ACD_WARP_UP;
        }
        break;
    case DS_ACD_IDLE:
        {
            IconData.strIconPath = PIC_DSS_ACD_LOGOUT;
        }
        break;
    default:
        {
            IconData.strIconPath = PIC_DSS_ACD_LOGOUT;
        }
        break;
    }

    IconData.eIconType = DK_ICON_ACD;
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceACD::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    switch (logicData.iStatus)
    {
    case DS_ACD_AVAILABLE:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
        }
        break;
    case DS_ACD_UNAVAILABLE:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
            LigntData.eLightFlashType = REFRESH_TYPE_FLASH;
        }
        break;
    case DS_ACD_WRAPUP:
        {
            LigntData.eLightColorType = LED_COLOR_TYPE_RED;
        }
        break;
    case DS_ACD_IDLE:
    default:
        break;
    }
    return true;
}

///////////////DT_HOTELING/////////////////////////
ImplDsskeyPerformanceHoteling::ImplDsskeyPerformanceHoteling()
    : ImplDsskeyPerformanceBase(DT_HOTELING)
{

}

ImplDsskeyPerformanceHoteling::~ImplDsskeyPerformanceHoteling()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceHoteling::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.eIconType = DK_ICON_HOTELING;
    if (DS_HOTELING_ON == logicData.iStatus)
    {
        IconData.strIconPath = PIC_DSS_HOTELING_LOGININ;
    }
    else
    {
        IconData.strIconPath = PIC_DSS_HOTELING_LOGINOUT;
    }
    return true;
}

// 获取灯显示
bool ImplDsskeyPerformanceHoteling::GetLightPerformance(const DssKeyLogicData & logicData,
        DsskeyLightData & LigntData)
{
    if (DS_HOTELING_ON == logicData.iStatus)
    {
        LigntData.eLightColorType = LED_COLOR_TYPE_GREEN;
    }
    return true;
}


///////////////DT_EDK/////////////////////////
ImplDsskeyPerformanceEDK::ImplDsskeyPerformanceEDK()
    : ImplDsskeyPerformanceBase(DT_CUSTOM_KEY)
{

}

ImplDsskeyPerformanceEDK::~ImplDsskeyPerformanceEDK()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceEDK::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    //http://10.2.1.199/Bug.php?BugID=86699 当EDK功能关闭的时候可能出现Type是DT_CUSTOM_KEY的特殊情况
    //兼容逻辑层这个特殊情况
    if (Edk_IsEnable())
    {
        IconData.strIconPath = PIC_DSS_EDK;
    }
    else
    {
        IconData.strIconPath = "";
    }

    return true;
}

// 获取文字显示
bool ImplDsskeyPerformanceEDK::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    yl::string strEDKLabel = Edk_GetDsskeyData(logicData.iDssKeyID, EDK_LABEL);

    if (!strEDKLabel.empty())
    {
        LabelData.strFirstRowText = strEDKLabel;
    }
    else
    {
        LabelData.strFirstRowText = fromQString(LANG_TRANSLATE(dsskey_GetDefaultLabel(
                DT_CUSTOM_KEY).c_str()));
    }

    LabelData.eTextMode = TEXT_MODE_FIRST_ROW;

    return true;
}

////////////////DT_BTLINE/////////////////////////
ImplDsskeyPerformanceBTLINE::ImplDsskeyPerformanceBTLINE()
    : ImplDsskeyPerformanceBase(DT_BTLINE)
{

}

ImplDsskeyPerformanceBTLINE::~ImplDsskeyPerformanceBTLINE()
{

}

bool ImplDsskeyPerformanceBTLINE::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    if (logicData.iStatus == DS_BTLINE_USING
            || logicData.iStatus == DS_BTLINE_ONLINE)
    {
        IconData.strIconPath = PIC_DSS_MOBILE_BLUE_GREEN;
    }
    else if (logicData.iStatus == DS_BTLINE_CONNECTTING)
    {
        IconData.strIconPath = PIC_DSS_MOBILE_BLUE_GREEN;
        IconData.eRefreshType = REFRESH_TYPE_FLASH;
    }
    else if (logicData.iStatus == DS_BTLINE_OFFLINE)
    {
        IconData.strIconPath = PIC_DSS_MOBILE_BLUE_RED;
    }

    return true;
}
bool ImplDsskeyPerformanceBTLINE::GetBkgroundPerformance(const DssKeyLogicData & logicData,
        DsskeyBkgroundData & BkData)
{
    if (logicData.iStatus == DS_BTLINE_USING)
    {
        //高亮显示
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_HIGHLIGHT;
    }
    else
    {
        //使用普通颜色
        BkData.eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
    }

    return true;
}

bool ImplDsskeyPerformanceBTLINE::GetLabelPerformance(const DssKeyLogicData & logicData,
        DsskeyLabelData & LabelData)
{
    yl::string strLabel = logicData.strLabel;
    int nBTLineStatus = logicData.iStatus;
    bool bExt = GetExtendFlag();

    if (DS_BTLINE_OFFLINE == nBTLineStatus
            || DS_BTLINE_CONNECTTING == nBTLineStatus)
    {
        if (strLabel.empty())
        {
            LabelData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_MOBILE_DSSKEY_LABEL,
                                         bExt)).toUtf8().data();
        }
        else
        {
            LabelData.strFirstRowText = strLabel;
        }
    }
    else if (DS_BTLINE_ONLINE == nBTLineStatus
             || DS_BTLINE_USING == nBTLineStatus)
    {
        if (strLabel == TRID_MOBILE_DSSKEY_LABEL
                || strLabel.empty())
        {
            LabelData.strFirstRowText = (LANG_TRANSLATE_DSSKEY_LABEL(TRID_BTLINE, bExt)).toUtf8().data();
        }
        else
        {
            LabelData.strFirstRowText = strLabel;
        }
    }
    else
    {
        LabelData.strFirstRowText = strLabel;
    }

    return true;
}

#ifdef IF_FEATURE_GETBALANCE
////////////////DT_GET_BALANCE/////////////////////////
ImplDsskeyPerformanceGETBALANCE::ImplDsskeyPerformanceGETBALANCE()
    : ImplDsskeyPerformanceBase(DT_GET_BALANCE)
{

}

ImplDsskeyPerformanceGETBALANCE::~ImplDsskeyPerformanceGETBALANCE()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceGETBALANCE::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_GET_BALANCE;
    return true;
}
#endif

#if IF_FEATURE_METASWITCH
///////////////DT_MTSW_LOGOUT/////////////////////////
ImplDsskeyPerformanceMTSWLogout::ImplDsskeyPerformanceMTSWLogout()
    : ImplDsskeyPerformanceBase(DT_MTSW_LOGOUT)
{

}

ImplDsskeyPerformanceMTSWLogout::~ImplDsskeyPerformanceMTSWLogout()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMTSWLogout::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_ACD_LOGOUT;
    return true;
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
///////////////DT_MTSWDIR/////////////////////////
ImplDsskeyPerformanceMTSWDir::ImplDsskeyPerformanceMTSWDir()
    : ImplDsskeyPerformanceBase(DT_MTSWDIR)
{

}

ImplDsskeyPerformanceMTSWDir::~ImplDsskeyPerformanceMTSWDir()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMTSWDir::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    /*IconData.eIconType = DK_ICON_SMS;*/
    IconData.strIconPath = PIC_DSS_LOCALGROUP;
    return true;
}
#endif

#if IF_FEATURE_METASWITCH_CALLPARK
///////////////DT_MTSW_Callpark/////////////////////////
ImplDsskeyPerformanceMTSWCallPark::ImplDsskeyPerformanceMTSWCallPark()
    : ImplDsskeyPerformanceBase(DT_MTSW_CALLPARK)
{

}

ImplDsskeyPerformanceMTSWCallPark::~ImplDsskeyPerformanceMTSWCallPark()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceMTSWCallPark::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_CALLPARK;
    return true;
}
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
///////////////History/////////////////////////
ImplDsskeyPerformanceHistory::ImplDsskeyPerformanceHistory()
    : ImplDsskeyPerformanceBase(DT_IDLE_CALLLIST)
{

}

ImplDsskeyPerformanceHistory::~ImplDsskeyPerformanceHistory()
{

}

// 获取图标显示
bool ImplDsskeyPerformanceHistory::GetIconPerformance(const DssKeyLogicData & logicData,
        DsskeyIconData & IconData)
{
    IconData.strIconPath = PIC_DSS_HISTORY;
    return true;
}
#endif
