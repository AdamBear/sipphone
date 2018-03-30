#include "dircommonfunc.h"
#include "../include/commondata.h"
//#include <algorithm>

// 生成数据记录
BaseDirDataRecord * CreateDataEntry(DIR_DATA_ENTRY_TYPE type)
{
    BaseDirDataRecord * pRet = NULL;
    switch (type)
    {
    case DIR_TYPE_GROUP://联系人组
        pRet = new DirGroupRecord();
        break;

    case DIR_TYPE_LOCAL_CONTACT://本地联系人
        pRet = new LocalDirContactRecord();
        break;

    case DIR_TYPE_BT_CONTACT:
        pRet = new BlueToothDirContactRecord();
        break;
    case DIR_TYPE_LOCALLOG_CALLLOG: //本地历史记录
        pRet = new LocalLogItemRecord();
        break;

    case DIR_TYPE_BSFT_CALLLOG: // BSFT历史记录
        pRet = new BSFTCallLogRecord();
        break;

    case DIR_TYPE_BSFT_LOG_GROUP:
        pRet = new BSFTLogGroupRecord();
        break;

    case DIR_TYPE_BROADSOFT_MENU://BraodSoft联系人组
        pRet = new ContactBroadSoftMenu();
        break;

    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        pRet = new ContactBroadSoftCommon;
        break;

    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        pRet = new ContactBroadSoftCustom;
        break;

    case DIR_TYPE_BROADSOFT_ITEM_PERSONAL:
        pRet = new ContactBroadSoftPersonal;
        break;

    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        pRet = new ContactBroadSoftGroup;
        break;

    case DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        pRet = new ContactBroadSoftEnterprise;
        break;

    case DIR_TYPE_REMOTE_MENU:
        pRet = new ContactRemoteMenu;
        break;

    case DIR_TYPE_REMOTE_UNIT:
        pRet = new ContactRemoteUnit;
        break;

    case DIR_TYPE_REMOTE_HOTKEY:
        pRet = new ContactRemoteHotKey;
        break;
    case DIR_TYPE_CLOUD_ITEM_ENTERPRISE:
        pRet = new ContactCloudEnterprise;
        break;
#if IF_FEATURE_GBBOOK
    case DIR_TYPE_GB_MENU:
        pRet = new ContactGBMenu;
        break;
    case DIR_TYPE_GB_UNIT:
        pRet = new ContactGBBookUnit;
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case DIR_TYPE_MTSW_CALLLOG: // MTSW历史记录
        pRet = new MTSWCallLogRecord();
        break;

    case DIR_TYPE_MTSW_LOG_GROUP:
        pRet = new MTSWLogGroupRecord();
        break;
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIR_TYPE_METASWITCH_MENU:
        pRet = new ContactMetaSwitchMenu;
        break;

    case DIR_TYPE_METASWITCH_ITEM_CONTACTLIST:
        pRet = new ContactMetaSwitchContactList;
        break;

    case DIR_TYPE_METASWITCH_ITEM_EXTENTION:
        pRet = new ContactMetaSwitchExtention;
        break;

    case DIR_TYPE_METASWITCH_ITEM_MLHG:
        pRet = new ContactMetaSwitchMLHG;
        break;
#endif

    default://默认返回空
        break;
    }
    if (pRet)
    {
        pRet->m_eType = type;
    }
    return pRet;
}


bool ParseStringBySeparator(const yl::string & strInput, const char Separator,
                            YLList<yl::string> & OutPutList)
{
    if (strInput.empty())
    {
        return false;
    }

    OutPutList.clear();

    yl::string InputCopy(strInput);
    int nFound = InputCopy.find_first_of(Separator);
    while (nFound != yl::string::npos)
    {
        yl::string strSub = InputCopy.substr(0, nFound);
        InputCopy = InputCopy.substr(nFound + 1);

        OutPutList.push_back(strSub);
        nFound = InputCopy.find_first_of(Separator);
    }

    if (!InputCopy.empty())
    {
        OutPutList.push_back(InputCopy);
    }
    return true;
}

bool OpenBSFTXML(const char * lpFile, xml_document & doc)
{
    if (NULL == lpFile
            || 0 == strlen(lpFile))
    {
        return false;
    }

    xml_parse_result ret = doc.load_file(lpFile, parse_default | parse_declaration);

    xml_node nodeXml = doc.child("xml");

    if (!nodeXml.empty())
    {
        xml_attribute AttrEncoding =  nodeXml.attribute("encoding");
        if (!AttrEncoding.empty())
        {
            yl::string strValue;
            strValue.assign(AttrEncoding.value(), strlen(AttrEncoding.value()));
            if (strValue.compare("ISO-8859-1") == 0)
            {
                ret = doc.load_file(lpFile, parse_default | parse_declaration, encoding_latin1);
            }
        }
    }

    return (status_ok == ret.status);
}


#if IF_FEATURE_METASWITCH_CALLLOG
int TransformMonth2Int(const char * pszMonth)
{
    if (pszMonth == NULL)
    {
        return -1;
    }
    const char * strMonth[13] = {"", TRID_JANUARY_SHORT, TRID_FEBRUARY_SHORT, TRID_MARCH_SHORT, TRID_APRIL_SHORT, TRID_MAY, TRID_JUNE_SHORT, TRID_JULY_SHORT, TRID_AUGUST_SHORT, TRID_SEPTEMBER_SHORT, TRID_OCTOBER_SHORT, TRID_NOVEBER_SHORT, TRID_DECEMBER_SHORT};
    for (int i = 0; i < 13; ++i)
    {
        if (strcmp(pszMonth, strMonth[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

yl::string TransformTime(char * strCallLog)
{
    //18 Nov 14 21:12:09
    int iYear = 0;
    int iMonth = 0;
    char cMonth[16] = "";
    int iDay = 0;
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;

    sscanf(strCallLog, "%d %s %d %d:%d:%d", &iDay, cMonth, &iYear, &iHour, &iMinute, &iSecond);

    iMonth = TransformMonth2Int(cMonth);

    if (iYear < 100)
    {
        iYear += 2000;
    }

    yl::string strTime = commonAPI_FormatString("%d-%02d-%02d %02d:%02d:%02d",
                         iYear, iMonth, iDay,
                         iHour, iMinute, iSecond);
    return strTime;
}
#endif