#include "directorycommon.h"
#include "translateiddefine.h"
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include <resource/modresource.h>
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "talk/talklogic/include/talklogic_common.h"


// 定义联系人属性名
const char * DIR_ATTR_NAME = "name";
const char * DIR_ATTR_OFFICENUMBER = "officenumber";
const char * DIR_ATTR_MOBILENUMBER = "mobilenumber";
const char * DIR_ATTR_OTHERNUMBER = "othernumber";
const char * DIR_ATTR_NUMBER = "number";
const char * DIR_ATTR_LINE = "line";
const char * DIR_ATTR_RING = "ring";
const char * DIR_ATTR_GROUP = "group";
const char * DIR_ATTR_FAVORITE_INDEX = "favoriteindex";

// 定义broadsoft联系人属性名
const char * DIR_ATTR_BSFT_LASTNAME = "lastname";
const char * DIR_ATTR_BSFT_FIRSTNAME = "firstname";
const char * DIR_ATTR_BSFT_USERID = "userid";
const char * DIR_ATTR_BSFT_GROUPID = "groupid";
const char * DIR_ATTR_BSFT_EXTENSION = "extension";
const char * DIR_ATTR_BSFT_NUMBER = "number";
const char * DIR_ATTR_BSFT_MOBILE = "mobile";
const char * DIR_ATTR_BSFT_YAHOOID = "yahooid";
const char * DIR_ATTR_BSFT_PAGER = "pager";
const char * DIR_ATTR_BSFT_EMAIL = "email";
const char * DIR_ATTR_BSFT_IMPID = "impid";
const char * DIR_ATTR_BSFT_LOCATION = "location";
const char * DIR_ATTR_BSFT_TITLE = "title";
const char * DIR_ATTR_BSFT_ADDRESS1 = "address1";
const char * DIR_ATTR_BSFT_ADDRESS2 = "address2";
const char * DIR_ATTR_BSFT_CITY = "city";
const char * DIR_ATTR_BSFT_STATE = "state";
const char * DIR_ATTR_BSFT_COUNTRY = "country";
const char * DIR_ATTR_BSFT_CODE = "code";
const char * DIR_ATTR_BSFT_DEPARTMENT = "department";
const char * DIR_ATTR_BSFT_PASSWORD = "password";

const char * DIR_ATTR_MTSW_FIRST_NAME = "mtswfirstname";
const char * DIR_ATTR_MTSW_LAST_NAME = "mtswlastname";
const char * DIR_ATTR_MTSW_NICK_NAME = "mtswnickname";
const char * DIR_ATTR_MTSW_JOBTITLE = "mtswjobtitle";
const char * DIR_ATTR_MTSW_ORGANISATION = "mtsworganisation";
const char * DIR_ATTR_MTSW_HOMENUMBER = "mtswhomenumber";
const char * DIR_ATTR_MTSW_WORKNUMBER = "mtswworknumber";
const char * DIR_ATTR_MTSW_MOBILENUMBER = "mtswmobilenumber";
const char * DIR_ATTR_MTSW_FAXNUMBER = "mtswfaxnumber";
const char * DIR_ATTR_MTSW_OTHERNUMBER = "mtswothernumber";
const char * DIR_ATTR_MTSW_DEPARTMENT = "mtswdepartment";
const char * DIR_ATTR_MTSW_EXTENTION = "mtswextention";
const char * DIR_ATTR_MTSW_NAME = "name";
const char * DIR_ATTR_MTSW_NUMBER = "number";
const char * DIR_ATTR_MTSW_EXTENSION = "extension";

#define CONTACT_RES_TYPE_IMAGE "contact"
typedef YLSmartPtr<BaseResItem> CResDataPtr;
typedef YLList<CResDataPtr> LIST_RESOURCE;


bool Res_GetResType(int nResType, yl::string & strResType)
{
    if (DIR_RES_TYPE_RING == nResType)
    {
        strResType = RING_TYPE;

        return true;
    }
    else if (DIR_RES_TYPE_PHOTO == nResType)
    {
        strResType = CONTACT_RES_TYPE_IMAGE;

        return true;
    }

    return false;
}

bool Res_GetNameByPath(int nResType, const yl::string & strPath, yl::string & strName)
{
    yl::string strResType;

    if (!Res_GetResType(nResType, strResType))
    {
        return false;
    }

    LIST_RESOURCE listRes;

    if (0 != res_getResItemsByResType(listRes, strResType.c_str()))
    {
        return false;
    }

    LIST_RESOURCE::iterator iter = listRes.begin();

    for (; iter != listRes.end(); iter++)
    {
        CResDataPtr & pItem = *iter;

        if (pItem.IsEmpty())
        {
            continue;
        }

        if (strPath == pItem->resPath)
        {
            strName = pItem->resItemName;
            return true;
        }
    }

    return false;
}

bool ParseFileName(const yl::string & strFilePath, yl::string & strFile)
{
    yl::string::size_type pos = strFilePath.find_last_of("/");

    if (yl::string::npos == pos)
    {
        return false;
    }

    strFile = strFilePath.substr(pos + 1);

    return true;
}

bool ParseFilePath(const yl::string & strFilePath, yl::string & strPath, yl::string & strFile)
{
    yl::string::size_type pos = strFilePath.find_last_of("/");

    if (yl::string::npos == pos)
    {
        return false;
    }

    strPath = strFilePath.substr(0, pos + 1);
    strFile = strFilePath.substr(pos + 1);

    return true;
}

bool IsBsftAllowSipAuth()
{
    return (1 == configParser_GetConfigInt(kszAllowSipAuth));
}

void TransBsftFACText(const yl::string & strValue, yl::string & strText)
{
    if (strValue.find(kszReplaceCallParkCode) != yl::string::npos
            || strValue.find(kszReplaceCallRetriveCode) != yl::string::npos
            || strValue.find(kszReplaceGroupParkCode) != yl::string::npos
            || strValue.find(kszReplaceGPickUpCode) != yl::string::npos
            || strValue.find(kszReplaceDPickUpCode) != yl::string::npos
            || strValue.find(kszReplaceVoiceMailCode) != yl::string::npos
            || strValue.find(kszReplaceBargeInCode) != yl::string::npos
            || strValue.find(kszReplaceCallPullCode) != yl::string::npos
#if IF_BUG_23548
            || strValue.find(kszReplaceCallBackCode) != yl::string::npos
#endif
       )
    {
        yl::string::size_type nPos = strValue.find(':');

        if (yl::string::npos != nPos)
        {
            yl::string strFACValue = strValue.substr(0, nPos);
            yl::string strTemp = strValue.substr(nPos);

            strText = _UILOGIC_LANG_TRAN(strFACValue.c_str()) + strTemp;
        }
        else
        {
            strText = _UILOGIC_LANG_TRAN(strValue.c_str());
        }
    }
    else
    {
        strText = strValue;
    }
}

// 从broadsoft时间中提取出月日和时间
time_t TransBsftCallLogTime(const char * pszCallLog, bool bHaveSecond)
{
    if (NULL == pszCallLog)
    {
        return 0;
    }

    //2010-09-01T04:18:24.491-04:00
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;
    int iZoneHour = 0;
    int iZoneMinute = 0;

    sscanf(pszCallLog, "%d-%d-%dT%d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);

    yl::string strLogTime = pszCallLog;
    yl::string strTimeZone = "";
    int nLogTimeLen = strLogTime.length();

    if (nLogTimeLen >= 6)
    {
        strTimeZone = strLogTime.substr(nLogTimeLen - 6, 6);
    }

    sscanf(strTimeZone.c_str(), "%d:%d", &iZoneHour, &iZoneMinute);

    //将时间转化成time_t
    tm tmWhen;

    tmWhen.tm_year = iYear - 1900;

    if (iMonth < 0)
    {
        iMonth = 0;
    }

    tmWhen.tm_mon = iMonth - 1;
    tmWhen.tm_mday = iDay;
    tmWhen.tm_hour = iHour ;
    tmWhen.tm_min = iMinute;

    if (bHaveSecond)
    {
        tmWhen.tm_sec = iSecond;
    }

    time_t tTime = mktime(&tmWhen);

    return tTime;
}

#if IF_FEATURE_METASWITCH_CALLLOG
// 从MTSW时间中提取出月日和时间
time_t TransMTSWCallLogTime(const char * pszCallLog, bool bHaveSecond)
{
    if (NULL == pszCallLog)
    {
        return 0;
    }

    //2014-11-18 21:12:38
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;

    sscanf(pszCallLog, "%d-%d-%d %d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);

    //将时间转化成time_t
    tm tmWhen;
    tmWhen.tm_year = iYear - 1900;
    if (iMonth < 0)
    {
        iMonth = 0;
    }
    tmWhen.tm_mon = iMonth - 1;
    tmWhen.tm_mday = iDay;
    tmWhen.tm_hour = iHour;
    tmWhen.tm_min = iMinute;
    if (bHaveSecond)
    {
        tmWhen.tm_sec = iSecond;
    }

    time_t tTime = mktime(&tmWhen);
    return tTime;
}
#endif
