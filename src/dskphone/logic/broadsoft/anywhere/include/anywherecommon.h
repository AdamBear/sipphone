/************************************************************************
 * FileName  : AnywhereCommon.h (header file)
 * Purpose   :
 * Date      : 2012/08/10 16:04:33
 ************************************************************************/

#ifndef _ANYWHERECOMMON_H_
#define _ANYWHERECOMMON_H_

#include <ylstring.h>
#include <ylvector.h>

#define Elem_Name_BroadWorksAnywhere                "BroadWorksAnywhere"
#define Elem_Name_BroadWorksAnywhereLocation    "BroadWorksAnywhereLocation"
#define Elem_Name_Locations                             "locations"
#define Elem_Name_Location                                  "location"
#define Elem_Name_PhoneNumber                           "phoneNumber"
#define Elem_Name_OutboundAlternateNumber       "outboundAlternateNumber"
#define Elem_Name_Description                               "description"
#define Elem_Name_LocationUri                               "locationUri"
#define Elem_Name_Active                                        "active"
#define Elem_Name_BroadworksCallControl             "broadworksCallControl"
#define Elem_Name_UseDiversionInhibitor             "useDiversionInhibitor"
#define Elem_Name_AnswerConfirmationRequired    "answerConfirmationRequired"
#define Elem_Name_AlertAllLocationsForClickToDialCalls  "alertAllLocationsForClickToDialCalls"
#define Elem_Name_CriteriaActivations "criteriaActivations"
#define Elem_Name_CriteriaActivation "criteriaActivation"

#define Attrib_Name_CountryCode             "countryCode"
#define Attrib_Name_xmlns                       "xmlns"

#define Attrib_Value_xmlns                          "http://schema.broadsoft.com/xsi"


typedef YLVector<yl::string> CStringVector;

struct SAYWLocation
{
    yl::string  m_strLocationUri;
    yl::string  m_strPhoneNum;
    yl::string  m_strCountryCode;
    yl::string  m_strDescription;
    yl::string  m_strOutBoundPhoneNum;
    bool    m_bActive;
    bool m_bEnableDiversionInhibitor;
    bool m_bRequireAnswerConfirmation;
    bool m_bUseBbasedCallControlServices;

    bool operator==(const yl::string & strPhoneNumber) const
    {
        return strPhoneNumber == m_strPhoneNum;
    }

    void clear()
    {
        m_strLocationUri.clear();
        m_strPhoneNum.clear();
        m_strCountryCode.clear();
        m_strDescription.clear();
        m_strOutBoundPhoneNum.clear();
        m_bActive = false;
        m_bEnableDiversionInhibitor = false;
        m_bRequireAnswerConfirmation = false;
        m_bUseBbasedCallControlServices = false;
    }
};

struct SAYWLocationArray
{
    SAYWLocationArray() : m_bAlertAllLocatForClickToDialCalls(false) {}
    typedef YLVector<SAYWLocation> LocationArray;

    //member
    bool m_bAlertAllLocatForClickToDialCalls;
    bool m_bAlertAllLocationsForGroupPagingCalls;
    LocationArray m_locatArr;

    void push_back(const SAYWLocation & value)
    {
        m_locatArr.push_back(value);
    }

    size_t size()
    {
        return m_locatArr.size();
    }

    void clear()
    {
        m_bAlertAllLocatForClickToDialCalls = false;
        m_bAlertAllLocationsForGroupPagingCalls = false;
        m_locatArr.clear();
    }
};

#endif
