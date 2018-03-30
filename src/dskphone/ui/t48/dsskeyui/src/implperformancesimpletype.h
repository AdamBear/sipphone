#ifndef _IMPLEMENT_DSSKEY_PERFORMANCE_SIMPLE_TYPE_H_
#define _IMPLEMENT_DSSKEY_PERFORMANCE_SIMPLE_TYPE_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "commonfunc.h"
#include "impldsskeyperformance.h"

////////////////DT_VOICEMAIL/////////////////////////
class  ImplDsskeyPerformanceVoiceMail : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceVoiceMail();

    virtual ~ImplDsskeyPerformanceVoiceMail();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_DTMF/////////////////////////
class  ImplDsskeyPerformanceDTMF : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceDTMF();

    virtual ~ImplDsskeyPerformanceDTMF();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_XMLBROWSER/////////////////////////
class  ImplDsskeyPerformanceXmlBrowser : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceXmlBrowser();

    virtual ~ImplDsskeyPerformanceXmlBrowser();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

};

#if IF_BUG_31595
////////////////DT_XMLBROWSER_INFO/////////////////////////
class  ImplDsskeyPerformanceXmlBrowserInfo : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceXmlBrowserInfo();

    virtual ~ImplDsskeyPerformanceXmlBrowserInfo();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

};
#endif

#if IF_BUG_14856
////////////////DT_MISSED_CALL/////////////////////////
class  ImplDsskeyPerformanceMissedCall : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMissedCall();

    virtual ~ImplDsskeyPerformanceMissedCall();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};
#endif

////////////////DT_LDAP/////////////////////////
class  ImplDsskeyPerformanceLDAP : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceLDAP();

    virtual ~ImplDsskeyPerformanceLDAP();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_FAVORITES/////////////////////////
class  ImplDsskeyPerformanceFavorite : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceFavorite();

    virtual ~ImplDsskeyPerformanceFavorite();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_CONF/////////////////////////
class  ImplDsskeyPerformanceConference : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceConference();

    virtual ~ImplDsskeyPerformanceConference();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_TRAN/////////////////////////
class  ImplDsskeyPerformanceTransfer : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceTransfer();

    virtual ~ImplDsskeyPerformanceTransfer();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_HOLD/////////////////////////
class  ImplDsskeyPerformanceHold : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceHold();

    virtual ~ImplDsskeyPerformanceHold();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_REDIAL/////////////////////////
class  ImplDsskeyPerformanceRedial : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceRedial();

    virtual ~ImplDsskeyPerformanceRedial();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_CALLRETURN/////////////////////////
class  ImplDsskeyPerformanceCallReturn : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceCallReturn();

    virtual ~ImplDsskeyPerformanceCallReturn();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_SMS/////////////////////////
class  ImplDsskeyPerformanceSMS : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceSMS();

    virtual ~ImplDsskeyPerformanceSMS();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

#if IF_FEATURE_METASWITCH_VOICEMAIL
////////////////DT_MESSAGE_LIST/////////////////////////
class  ImplDsskeyPerformanceMessageList : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMessageList();

    virtual ~ImplDsskeyPerformanceMessageList();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};
#endif

////////////////DT_GROUPLISTEN/////////////////////////
class  ImplDsskeyPerformanceGroupListen : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceGroupListen();

    virtual ~ImplDsskeyPerformanceGroupListen();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_PUBLICHOLD/////////////////////////
class  ImplDsskeyPerformancePublicHold : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformancePublicHold();

    virtual ~ImplDsskeyPerformancePublicHold();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_PRIVATEHOLD/////////////////////////
class  ImplDsskeyPerformancePrivateHold : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformancePrivateHold();

    virtual ~ImplDsskeyPerformancePrivateHold();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_ZERO/////////////////////////
class  ImplDsskeyPerformanceZeroTouch : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceZeroTouch();

    virtual ~ImplDsskeyPerformanceZeroTouch();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_MEET_ME_CONFERENCE/////////////////////////
class  ImplDsskeyPerformanceMeetMe : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMeetMe();

    virtual ~ImplDsskeyPerformanceMeetMe();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_ACD_TRACE/////////////////////////
class  ImplDsskeyPerformanceACDTrace : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceACDTrace();

    virtual ~ImplDsskeyPerformanceACDTrace();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_DISPCODE/////////////////////////
class  ImplDsskeyPerformanceDispcode : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceDispcode();

    virtual ~ImplDsskeyPerformanceDispcode();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_ESCALATE/////////////////////////
class  ImplDsskeyPerformanceEscalate : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceEscalate();

    virtual ~ImplDsskeyPerformanceEscalate();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_KEYPAD_LOCK/////////////////////////
class  ImplDsskeyPerformanceKeypadLock : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceKeypadLock();

    virtual ~ImplDsskeyPerformanceKeypadLock();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_BSFT_RETRIEVE_PARK/////////////////////////
class  ImplDsskeyPerformanceRetrievePark : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceRetrievePark();

    virtual ~ImplDsskeyPerformanceRetrievePark();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_HOTDESKING/////////////////////////
class  ImplDsskeyPerformanceHotDesk : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceHotDesk();

    virtual ~ImplDsskeyPerformanceHotDesk();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_NETWORK_FAVORITE/////////////////////////
class  ImplDsskeyPerformanceNetworkFavorite : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceNetworkFavorite();

    virtual ~ImplDsskeyPerformanceNetworkFavorite();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_BUDDIES/////////////////////////
class  ImplDsskeyPerformanceBuddies : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceBuddies();

    virtual ~ImplDsskeyPerformanceBuddies();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_MY_STATUS/////////////////////////
class  ImplDsskeyPerformanceMyStatus : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMyStatus();

    virtual ~ImplDsskeyPerformanceMyStatus();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

////////////////DT_PAGING_LIST/////////////////////////
class  ImplDsskeyPerformancePagingList : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformancePagingList();

    virtual ~ImplDsskeyPerformancePagingList();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

////////////////DT_PREFIX/////////////////////////
class  ImplDsskeyPerformancePrefix : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformancePrefix();

    virtual ~ImplDsskeyPerformancePrefix();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);
};

////////////////DT_FWD/////////////////////////
class  ImplDsskeyPerformanceForward : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceForward();

    virtual ~ImplDsskeyPerformanceForward();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);
};

////////////////DT_DND/////////////////////////
class  ImplDsskeyPerformanceDND : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceDND();

    virtual ~ImplDsskeyPerformanceDND();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);
};

////////////////DT_RECORD or DT_URLRECORD/////////////////////////
class  ImplDsskeyPerformanceRecord : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceRecord(DssKey_Type type);

    virtual ~ImplDsskeyPerformanceRecord();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);
};

////////////////DT_ACD/////////////////////////
class  ImplDsskeyPerformanceACD : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceACD();

    virtual ~ImplDsskeyPerformanceACD();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);
};

////////////////DT_HOTELING/////////////////////////
class  ImplDsskeyPerformanceHoteling : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceHoteling();

    virtual ~ImplDsskeyPerformanceHoteling();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);
};

////////////////DT_EDK/////////////////////////
class  ImplDsskeyPerformanceEDK : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceEDK();

    virtual ~ImplDsskeyPerformanceEDK();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

};


////////////////DT_BTLINE/////////////////////////
class  ImplDsskeyPerformanceBTLINE: public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceBTLINE();

    virtual ~ImplDsskeyPerformanceBTLINE();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
    virtual bool GetBkgroundPerformance(const DssKeyLogicData & logicData, DsskeyBkgroundData & BkData);
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

};

#ifdef IF_FEATURE_GETBALANCE
////////////////DT_GET_BALANCE/////////////////////////
class  ImplDsskeyPerformanceGETBALANCE : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceGETBALANCE();

    virtual ~ImplDsskeyPerformanceGETBALANCE();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};
#endif

#if IF_FEATURE_METASWITCH
////////////////DT_MTSW_LOGOUT/////////////////////////
class  ImplDsskeyPerformanceMTSWLogout : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMTSWLogout();

    virtual ~ImplDsskeyPerformanceMTSWLogout();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
////////////////DT_MTSWDIR/////////////////////////
class  ImplDsskeyPerformanceMTSWDir : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMTSWDir();

    virtual ~ImplDsskeyPerformanceMTSWDir();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};
#endif

#if IF_FEATURE_METASWITCH_CALLPARK
////////////////DT_MTSW_Callpark/////////////////////////
class  ImplDsskeyPerformanceMTSWCallPark : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceMTSWCallPark();

    virtual ~ImplDsskeyPerformanceMTSWCallPark();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
////////////////DT_Calllist/////////////////////////
class  ImplDsskeyPerformanceHistory : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceHistory();

    virtual ~ImplDsskeyPerformanceHistory();

public:

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);
};
#endif

#endif //_IMPLEMENT_DSSKEY_PERFORMANCE_SIMPLE_TYPE_H_
