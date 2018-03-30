#ifndef _BWCOMMON_H_
#define _BWCOMMON_H_

#include <ETLLib.hpp>
#include <ylstring.h>
#include <ylvector.h>

struct SRemoteOffice
{
    //是否开启
    bool bActive;
    //remote phone number
    yl::string strPhoneNumber;

    SRemoteOffice()
    {
        bActive = false;
        strPhoneNumber = "";
    }
};

struct SSimRingLocation
{
    //Answer confirmation required
    bool bAnswerConfirmationRequired;

    //Phone Number / SIP-URI
    yl::string strAddress;

    SSimRingLocation()
    {
        bAnswerConfirmationRequired = false;
        strAddress = "";
    }
};

struct SSimultaneousRing
{
    //是否开启
    bool bActive;

    //Do not ring my Simultaneous Ring Numbers if I'm already on a call
    bool bDoNotRingOnACall;

    //号码列表
    YLVector<SSimRingLocation> simRingLocationArry;

    SSimultaneousRing()
    {
        bActive = false;
        bDoNotRingOnACall = true;
    }

    //重置
    void Reset()
    {
        bActive = false;
        bDoNotRingOnACall = true;
        simRingLocationArry.clear();
    }
};

struct SCallForwardNotReachable
{
    //是否开启
    bool bActive;
    //call forward phone number
    yl::string strPhoneNumber;
};

//Mobility的PhoneToRing类型
static const char * MobilityPhoneToRingType[] = {"Fixed", "Mobile", "Both"};

struct SMobility
{
    bool bActive;
    int nPhoneToRing;
    yl::string strMobilePhoneNumber;
    bool    bAlertClickToDialCalls;
    bool    bAlertGroupPagingCalls;
    bool    bEnableDiversionInhibitor;
    bool    bRequireAnswerConfirmation;
    bool    bBroadworksCallControl;

    SMobility()
    {
        Reset();
    }

    void Reset()
    {
        bActive = false;
        nPhoneToRing = 0;
        bAlertClickToDialCalls = false;
        bAlertGroupPagingCalls = false;
        bEnableDiversionInhibitor = false;
        bRequireAnswerConfirmation = false;
        bBroadworksCallControl = false;
        strMobilePhoneNumber.clear();
    }
};

struct SGroupNightForward
{
    yl::string strUserNightFwdMode;
    yl::string strHolidayScheduleName;
    yl::string strHolidayScheduleLevel;
    yl::string strBusinessHoursName;
    yl::string strBusinessHoursLevel;
    yl::string strForwarTarget;

    void Reset()
    {
        strUserNightFwdMode.clear();
        strHolidayScheduleName.clear();
        strHolidayScheduleLevel.clear();
        strBusinessHoursName.clear();
        strBusinessHoursLevel.clear();
        strForwarTarget.clear();
    }
};

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
struct SForwardSelectiveDataCriteriaItem
{
    bool            bActive;
    yl::string      strName;
    yl::string      strURI;
};

struct SForwardSelectiveData
{
    typedef YLVector<SForwardSelectiveDataCriteriaItem> VecCriteriaItem;

    bool                bActive;
    bool                bPlayRingReminder;
    yl::string          strForwardToPhoneNumber;
    VecCriteriaItem     vecCriteria;

    SForwardSelectiveData() : bActive(false), bPlayRingReminder(false)
    {
    }

    void clear()
    {
        bActive = false;
        bPlayRingReminder = false;
        strForwardToPhoneNumber.clear();
        vecCriteria.clear();
    }
};
#endif
#endif  //_BWCOMMON_H_
