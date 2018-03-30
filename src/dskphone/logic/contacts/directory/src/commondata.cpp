#include <stdlib.h>
#include "commondata.h"
#include "dirdefine.h"
#include "dircommonfunc.h"

#include "commonunits/modcommonunits.h"
#include "commonapi/stringhelperapi.h"
#include "ETLLib.hpp"
#include "localcontactmanager.h"
#include "remotecontactmanager.h"
#include "devicelib/networkserver.h"
#include <ylstringalgorithm.h>
#include "bluetoothcontactmanager.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"

#include "configiddefine.h"
#include "modconfigparser.h"

#if IF_BUG_27560
// 隐藏通话信息的字符串
#define HIDE_TALK_INFO_STRING "******"
#endif

//数据结构基类

BaseDirDataRecord::BaseDirDataRecord()
{
    m_strDisplayName.clear();
    m_nParentId = -1;
    m_eType = DIR_TYPE_NONE;
    m_eInheritType = DIR_INHERIT_BASE;
    m_nId = knInvalidRId;
}

BaseDirDataRecord::~BaseDirDataRecord()
{
}

bool BaseDirDataRecord::ParseFromXMLNode(const xml_node & xmlNode)
{
    return false;
}


SearchHitType BaseDirDataRecord::SearchHit(const yl::string & strSearchKey,
        SearchMethod searchMethod, StringMatchType eMatchtype/* = CONTAIN_MATCH*/)
{
    return HIT_TYPE_NONE;
}

bool BaseDirDataRecord::Compare(BaseDirDataRecord * pRecordToComm)
{
    if (pRecordToComm)
    {
        if (strcasecmp(m_strDisplayName.c_str(), pRecordToComm->m_strDisplayName.c_str()) < 0)
        {
            return true;
        }
    }
    return false;
}

const yl::string & BaseDirDataRecord::GetDisplayName()
{
    return m_strDisplayName;
}

bool BaseDirDataRecord::ToXMLNode(xml_node & xmlNode)
{
    return false;
}

SearchHitType BaseContactRecord::SearchHit(const yl::string & strSearchKey,
        SearchMethod searchMethod /*= SEARCH_BY_AUTO*/, StringMatchType eMatchtype /* = CONTAIN_MATCH */)
{
    if (searchMethod == SEARCH_BY_AUTO)
    {
        searchMethod = commonAPI_IsDigital(strSearchKey) ? SEARCH_BY_T9 : SEARCH_BY_NAME;
    }
    if (searchMethod == SEARCH_BY_T9)
    {
        for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
                iter++)
        {
            stNumber & stNum = *iter;

            if (commonAPI_StringMatch(stNum.strValue, strSearchKey, true, eMatchtype))
            {
                return HIT_TYPE_NUM;
            }
        }

        if (commonAPI_StringMatch(GetNameIndex(), strSearchKey, true, eMatchtype))
        {
            // 名字的索引匹配, 插入到搜索结果链表末尾.
            return HIT_TYPE_NAME;
        }
    }
    else if (searchMethod == SEARCH_BY_NAME)
    {
        // 默认搜索只匹配名字.
        if (commonAPI_StringMatch(GetDisplayName(), strSearchKey, false, eMatchtype))
        {
            // 名字的索引匹配.
            return HIT_TYPE_NAME;
        }
    }
    return HIT_TYPE_NONE;
}


yl::string BaseContactRecord::GetSearchShowNumber(const yl::string & strSearchKey)
{
    StringMatchType eMatchtype = CONTAIN_MATCH;
    if (1 == configParser_GetConfigInt(kszDirSearchMatchtype))
    {
        eMatchtype = LEAD_MATCH;
    }

    yl::string strFirstNum;
    for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
            iter++)
    {
        stNumber & stNum = *iter;

        if (stNum.strValue.empty())
        {
            continue;
        }

        if (strFirstNum.empty())
        {
            strFirstNum = stNum.strValue;
        }

        if (commonAPI_StringMatch(stNum.strValue, strSearchKey, true, eMatchtype))
        {
            return stNum.strValue;
        }
    }

    return strFirstNum;
}

bool BaseContactRecord::GetNumberByName(const yl::string & strName, yl::string & strNumber)
{
    for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
            iter++)
    {
        stNumber & stNum = *iter;
        if (stNum.strName.compare(strName) == 0)
        {
            strNumber = stNum.strValue;
            return true;
        }
    }
    return false;
}

const yl::string BaseContactRecord::GetNameIndex()
{
    return m_strNameIndex;
}

bool BaseContactRecord::GetNumberByIndex(int index, yl::string & strNumber)
{
    if (index >= m_listNumber.size())
    {
        return false;
    }
    int i = 0;
    for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
            iter++)
    {
        if (i == index)
        {
            stNumber & stNum = *iter;
            strNumber = stNum.strValue;
            return true;
        }
        i++;
    }

    return false;
}

const yl::string & BaseContactRecord::GetNumberByIndex(int index)
{
    if (index >= m_listNumber.size())
    {
        return kszEmptyString;
    }
    int i = 0;
    for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
            iter++)
    {
        if (i == index)
        {
            stNumber & stNum = *iter;
            return stNum.strValue;
        }
        i++;
    }

    return kszEmptyString;

}

const yl::string & BaseContactRecord::GetNameByIndex(int index)
{
    if (index >= m_listNumber.size())
    {
        return kszEmptyString;
    }
    int i = 0;
    for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
            iter++)
    {
        if (i == index)
        {
            stNumber & stNum = *iter;
            return stNum.strName;
        }
        i++;
    }

    return kszEmptyString;
}

const::yl::string & LocalLogItemRecord::GetNumber()
{
    return GetNumberByIndex(0);
}


time_t LocalLogItemRecord::GetAdjustTime()
{
    time_t tTime = atoi(m_strBornTick.c_str());
    sys_time_adust_time_offset(&tTime);
    return tTime;
}

const yl::string & LocalLogItemRecord::GetDisplayName()
{
#ifdef IF_BT_SUPPORT_PHONE
    const yl::string & strBtName = g_pBlueToothDirManager->GetContactNameByNumber(GetNumber());
    if (!strBtName.empty())
    {
        return strBtName;
    }
#endif

    const yl::string & strLocalName = g_pLocalDirManager->GetContactNameByNumber(GetNumber());

    if (!strLocalName.empty())
    {
        return strLocalName;
    }

    return m_strDisplayName;
}

const yl::string LocalLogItemRecord::GetNameIndex()
{
    return commonAPI_transT9String(GetDisplayName());
}

#if IF_BUG_27560
// 专门给UI获取号码，并判断是否隐藏
yl::string LocalLogItemRecord::GetNumberToUI()
{
    if (m_bHideSipName)
    {
        return HIDE_TALK_INFO_STRING;
    }
    else
    {
        return GetNumberByIndex(0);
    }
}

// 专门给UI获取号码，并判断是否隐藏
yl::string LocalLogItemRecord::GetDisplayNameToUI()
{
    yl::string strName = GetDisplayName();

    // 隐藏号码，如果显示名和号码相等且都为数字则隐藏显示名
    if (m_bHideSipName && strName == GetNumber())
    {
        return HIDE_TALK_INFO_STRING;
    }

    return strName;
}

#endif


void LocalLogItemRecord::SetRelationInfo(CALL_RELATION eRelation /*= CR_NONE*/, yl::string strRelation /*= ""*/)
{
    m_eRelation = eRelation;
    m_strRelationNum = strRelation;
}


const yl::string & LocalLogItemRecord::GetPhoto()
{
#ifdef IF_BT_SUPPORT_PHONE
    const yl::string & strBTPic = g_pBlueToothDirManager->GetContactImageByNumber(GetNumber());
    if (!strBTPic.empty())
    {
        return strBTPic;
    }
#endif

    const yl::string & strLocalPic = g_pLocalDirManager->GetContactImageByNumber(GetNumber());
    if (!strLocalPic.empty())
    {
        return strLocalPic;
    }

    return kszEmptyString;
}

void LocalLogItemRecord::SetNumber(const yl::string & strNumber)
{
    m_listNumber.clear();
    stNumber stNum;
    stNum.strName = "Number";
    stNum.strValue = strNumber;
    m_listNumber.push_back(stNum);
}

void LocalLogItemRecord::SetLogType(CALL_TYPE eType)
{
    switch (eType)
    {
    case CT_OUTGOING:
        m_nParentId = kszPlacedLogId;
        break;
    case CT_INCOMING:
        m_nParentId = kszReceivedLogId;
        break;
    case CT_MISSCALL:
        m_nParentId = kszMissLogId;
        break;
    case CT_FORWARDCALL:
        m_nParentId = kszForWardedLogId;
        break;
    default:
        m_nParentId = kszAllContactId;
    }
}

CALL_TYPE LocalLogItemRecord::GetLogType()
{
    if (m_nParentId == kszPlacedLogId)
    {
        return CT_OUTGOING;
    }
    else if (m_nParentId == kszReceivedLogId)
    {
        return CT_INCOMING;
    }
    else if (m_nParentId == kszMissLogId)
    {
        return CT_MISSCALL;
    }
    else if (m_nParentId == kszForWardedLogId)
    {
        return CT_FORWARDCALL;
    }
    else
    {
        return CT_UNKNOW;
    }
}

//本地通话记录数据记录
//从xml解析
bool LocalLogItemRecord::ParseFromXMLNode(const xml_node & node)
{
    xml_attribute attrTmp = node.attribute(kszAttrCallType);
    if (attrTmp)
    {
        // 读取通话类型(为CALL_TYPE枚举值)属性.
        m_nParentId = atoi(attrTmp.value());
    }

    attrTmp = node.attribute(kszAttrBornTick);
    if (attrTmp)
    {
        // 读取born_tick属性.
        m_strBornTick = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrLocalSipName);
    if (attrTmp)
    {
        // 读取local_sip_name属性.
        m_strLocalSipName = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrLocalSipServer);
    if (attrTmp)
    {
        // 读取local_sip_server属性.
        m_strLocalSipServer = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrRemoteSipName);
    if (attrTmp)
    {
        stNumber stNum;
        stNum.strName = "Number";
        stNum.strValue = attrTmp.value();
        m_listNumber.push_back(stNum);
    }

    attrTmp = node.attribute(kszAttrRemoteDisplayName);
    if (attrTmp)
    {
        // 读取remote_display_name属性.
        m_strDisplayName = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrRemoteSipServer);
    if (attrTmp)
    {
        // 读取remote_sip_server属性.
        m_strRemoteSipServer = attrTmp.value();
    }

    attrTmp = node.attribute(kszAttrDuration);
    if (attrTmp)
    {
        // 读取duration属性.
        m_strDuration = attrTmp.value();
    }


#ifdef IF_BT_SUPPORT_PHONE
    attrTmp = node.attribute(kszMobileCallLog);
    if (attrTmp)
    {
        //读取btcalllog属性
        if (strcmp("1", attrTmp.value()) == 0)
        {
            m_nLine = Bluetooth_GetBtAccountID();
        }
    }
#endif

#if IF_BUG_27560
    attrTmp = node.attribute(kszAttrHideSipName);
    if (attrTmp)
    {
        // 读取hide_sipName属性
        m_bHideSipName = attrTmp.value() == yl::string("1") ? true : false;
    }
#endif

#ifdef IF_SUPPORT_VIDEO
    attrTmp = node.attribute(kszAttrVideoCall);
    if (attrTmp)
    {
        m_bVideoCall = attrTmp.as_bool();
    }
#endif //#ifdef IF_SUPPORT_VIDEO

    attrTmp = node.attribute(kszAttrRelation);
    if (attrTmp)
    {
        int nRelation = attrTmp.as_int();
        if (nRelation >= 0)
        {
            m_eRelation = (CALL_RELATION)nRelation;
        }
    }

    attrTmp = node.attribute(kszAttrRelationNum);
    if (attrTmp)
    {
        m_strRelationNum = attrTmp.as_string();
    }

    attrTmp = node.attribute(kszAttrNumOfTimes);
    if (attrTmp)
    {
        m_nNumOfTimes = attrTmp.as_int();
    }

    return false;
}

//转化为xml节点
bool LocalLogItemRecord::ToXMLNode(xml_node & xmlNode)
{
    char szBuffer[knMaxBufferLen + 1] = {0,};
    // CALL_TYPE
    memset(szBuffer, 0, sizeof(szBuffer));
    snprintf(szBuffer, knMaxBufferLen, "%d", m_nParentId);
    szBuffer[knMaxBufferLen] = '\0';
    xmlNode.append_attribute(kszAttrCallType, szBuffer);


    // duration
    xmlNode.append_attribute(kszAttrDuration, m_strDuration.c_str());

    // local_sip_name
    xmlNode.append_attribute(kszAttrLocalSipName, m_strLocalSipName.c_str());
    // local_sip_server
    xmlNode.append_attribute(kszAttrLocalSipServer, m_strLocalSipServer.c_str());
    // remote_display_name
    xmlNode.append_attribute(kszAttrRemoteDisplayName, m_strDisplayName.c_str());
    // remote_sip_name
    xmlNode.append_attribute(kszAttrRemoteSipName, GetNumber().c_str());
    // remote_sip_server
    xmlNode.append_attribute(kszAttrRemoteSipServer, m_strRemoteSipServer.c_str());

    // born_tick
    xmlNode.append_attribute(kszAttrBornTick, m_strBornTick.c_str());

#ifdef IF_SUPPORT_VIDEO
    xmlNode.append_attribute(kszAttrVideoCall, m_bVideoCall ? 1 : 0);
#endif //#ifdef IF_SUPPORT_VIDEO

#ifdef IF_BT_SUPPORT_PHONE
    //btcalllog
    if (Bluetooth_GetBtAccountID() == m_nLine)
    {
        xmlNode.append_attribute(kszMobileCallLog, 1);
    }
#endif

#if IF_BUG_27560
    // 是否隐藏号码
    if (m_bHideSipName)
    {
        xmlNode.append_attribute(kszAttrHideSipName, "1");
    }
#endif

    if (m_eRelation != CR_NONE)
    {
        xmlNode.append_attribute(kszAttrRelation, m_eRelation);
    }
    if (!m_strRelationNum.empty())
    {
        xmlNode.append_attribute(kszAttrRelationNum, m_strRelationNum.c_str());
    }

    if (m_nNumOfTimes > 1)
    {
        xmlNode.append_attribute(kszAttrNumOfTimes, m_nNumOfTimes);
    }

    return true;
}

bool LocalLogItemRecord::IsCloudCallLog()
{
    yl::string strLogLocalServer = configParser_GetConfigString(kszCloudSipServer);
    if (m_strLocalSipServer == strLogLocalServer)
    {
        return true;
    }


    return false;
}

LocalLogItemRecord::LocalLogItemRecord()
    : m_strBornTick("")
    , m_strDuration("")
{
    m_eType = DIR_TYPE_LOCALLOG_CALLLOG;
    m_eRelation = CR_NONE;

    m_nLine = -1;
#ifdef IF_SUPPORT_VIDEO
    m_bVideoCall = true;
#endif //#ifdef IF_SUPPORT_VIDEO

    m_nNumOfTimes = 1;

    m_strLocalSipName.clear();
    m_strLocalSipServer.clear();
    m_strRemoteSipServer.clear();
    m_strForwardSipName.clear();
    m_strForwardSipServer.clear();
    m_StrForwardDisplayName.clear();
    m_strRelationNum.clear();
}

bool LocalLogItemRecord::IsMutable()
{
    CALL_TYPE nType = GetLogType();
    return (nType == CT_MISSCALL || nType == CT_INCOMING);
}

//本地联系人组

bool LocalDirGroupRecord::ParseFromXMLNode(const xml_node & xmlNode)
{
    xml_attribute attrTmp = xmlNode.attribute(kszGXmlAttrDisplayName);
    if (attrTmp)
    {
        // 读取display name属性.
        m_strDisplayName = attrTmp.value();
    }

    attrTmp = xmlNode.attribute(kszGXmlAttrRing);
    if (attrTmp)
    {
        // 读取铃声属性.
        m_strRing = attrTmp.value();
    }

    //TODO 隐藏铃声路径
    commonUnits_Hide2Real(m_strRing, PHFILE_TYPE_RING_FILE);

    return true;
}

//转化为xml节点
bool LocalDirGroupRecord::ToXMLNode(xml_node & xmlNode)
{
    xmlNode.append_attribute(kszGXmlAttrDisplayName, m_strDisplayName.c_str());
    //铃声目录隐藏
    yl::string strHide(m_strRing.c_str());
    commonUnits_Real2Hide(strHide, PHFILE_TYPE_RING_FILE);
    xmlNode.append_attribute(kszGXmlAttrRing, strHide.c_str());
    return true;
}

LocalDirGroupRecord::~LocalDirGroupRecord()
{

}

//本地联系人记录

//从xml解析
bool LocalDirContactRecord::ParseFromXMLNode(const xml_node & node)
{
    xml_attribute attrTmp = node.attribute(kszCXmlAttrDisplayName);
    if (attrTmp)
    {
        // 读取display name属性.
        m_strDisplayName = attrTmp.value();
    }

    attrTmp = node.attribute(kszOfficeNumber);
    if (attrTmp)
    {
        // 读取office_number属性.
        stNumber sNum;
        sNum.strName = kszOfficeNumber;
        sNum.strValue = attrTmp.value();
        m_listNumber.push_back(sNum);
        g_pLocalDirManager->InsertToNumMap(sNum.strValue, m_strDisplayName);
    }

    attrTmp = node.attribute(kszMobileNumber);
    if (attrTmp)
    {
        // 读取mobile_number属性.
        stNumber sNum;
        sNum.strName = kszMobileNumber;
        sNum.strValue = attrTmp.value();
        m_listNumber.push_back(sNum);
        g_pLocalDirManager->InsertToNumMap(sNum.strValue, m_strDisplayName);
    }

    attrTmp = node.attribute(kszOtherNumber);
    if (attrTmp)
    {
        // 读取other_number属性.
        stNumber sNum;
        sNum.strName = kszOtherNumber;
        sNum.strValue = attrTmp.value();
        m_listNumber.push_back(sNum);
        g_pLocalDirManager->InsertToNumMap(sNum.strValue, m_strDisplayName);
    }

    attrTmp = node.attribute(kszCXmlAttrLine);
    if (attrTmp)
    {
        // 读取line type属性.
        m_nLine = atoi(attrTmp.value());

    }

    attrTmp = node.attribute(kszCXmlAttrRing);
    if (attrTmp)
    {
        // 读取ring type属性.
        m_strRing = attrTmp.value();

        commonUnits_Hide2Real(m_strRing, PHFILE_TYPE_RING_FILE);

        if (!pathFileExist(m_strRing.c_str()))
        {
            m_strRing = "Auto";
        }
    }

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {

        attrTmp = node.attribute(kszCXmlAttrDefaultPhoto);
        if (attrTmp)
        {
            // 读取default_photo属性.
            m_strDefaultPhoto = attrTmp.value();
            //commonUnits_Hide2Real(m_strDefaultPhoto, PHFILE_TYPE_CONTACT_IMAGE);
        }

        attrTmp = node.attribute(kszCXmlAttrSelectPhoto);
        if (attrTmp)
        {
            // 读取selected_photo属性.
            m_nSelectedPhoto = atoi(attrTmp.value());
        }

    }



#if IF_FEATURE_FAVORITE_DIR
    attrTmp = node.attribute(kszCxmlAttrFavorite);
    if (attrTmp)
    {
        yl::string strTemp = attrTmp.value();
        if (!strTemp.empty())
        {
            m_nFavoriteIndex = atoi(attrTmp.value());
        }
        else
        {
            m_nFavoriteIndex = -1;
        }
    }
#endif

#ifdef IF_FEATURE_PSTN
    attrTmp = node.attribute(kszCXmlAttrFavorate);
    if (attrTmp)
    {
        yl::string strTemp = attrTmp.value();
        m_bIsFavorite = strTemp == "1" ? true : false;
    }
#endif

    attrTmp = node.attribute(kszCxmlAttrDivert);
    if (attrTmp)
    {
        m_strDivert = attrTmp.value();
    }


    return true;
}

//转化为xml节点
bool LocalDirContactRecord::ToXMLNode(xml_node & xmlNode)
{
    char szBuffer[knMaxBufferLen + 1] = {0,};
    xmlNode.append_attribute(kszCXmlAttrDisplayName, m_strDisplayName.c_str());

    for (ListNumber::iterator iter = m_listNumber.begin(); iter != m_listNumber.end();
            iter++)
    {
        stNumber & stNum = *iter;

        xmlNode.append_attribute(stNum.strName.c_str(), stNum.strValue.c_str());
    }

    memset(szBuffer, 0, sizeof(szBuffer));
    sprintf(szBuffer, "%d", m_nLine);
    szBuffer[knMaxBufferLen] = '\0';
    xmlNode.append_attribute(kszCXmlAttrLine, szBuffer);

    yl::string strHide(m_strRing.c_str());

    commonUnits_Real2Hide(strHide, PHFILE_TYPE_RING_FILE);

    xmlNode.append_attribute(kszCXmlAttrRing, strHide.c_str());

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        xmlNode.append_attribute(kszCXmlAttrDefaultPhoto, m_strDefaultPhoto.c_str());

        memset(szBuffer, 0, sizeof(szBuffer));
        snprintf(szBuffer, knMaxBufferLen, "%d", m_nSelectedPhoto);

        szBuffer[knMaxBufferLen] = '\0';
        xmlNode.append_attribute(kszCXmlAttrSelectPhoto, szBuffer);
    }

    yl::string strGroupName = g_pLocalDirManager->GetDisplayNameById(m_nParentId);

    if (strGroupName.empty())
    {
        strGroupName = kszGroupNameAll;
    }

    xmlNode.append_attribute(kszCXmlAttrGroupIdName, strGroupName.c_str());

#if IF_FEATURE_FAVORITE_DIR
    if (m_nFavoriteIndex > 0)
    {
        memset(szBuffer, 0, sizeof(szBuffer));
        snprintf(szBuffer, knMaxBufferLen, "%d", m_nFavoriteIndex);
        szBuffer[knMaxBufferLen] = '\0';
        xmlNode.append_attribute(kszCxmlAttrFavorite, szBuffer);
    }
#endif

#ifdef IF_FEATURE_PSTN
    xmlNode.append_attribute(kszCXmlAttrFavorate, m_bIsFavorite ? "1" : "0");
#endif

    if (!m_strDivert.empty())
    {
        xmlNode.append_attribute(kszCxmlAttrDivert, m_strDivert.c_str());
    }

    return true;
}

LocalDirContactRecord::~LocalDirContactRecord()
{

}

LocalDirContactRecord::LocalDirContactRecord()
{
    m_nId = -1;
    m_nLine = -1;  // Line.
    m_strDivert.clear();

    m_strDefaultPhoto.clear();
    m_nSelectedPhoto = 0;

#ifdef IF_FEATURE_PSTN
    m_bIsFavorite = false;
#endif
#if IF_FEATURE_FAVORITE_DIR
    m_nFavoriteIndex = -1;
#endif

    m_eType = DIR_TYPE_LOCAL_CONTACT;
}

//////////////////////////////////////////////////////////////////////////////
// 蓝牙联系人
BlueToothDirContactRecord::BlueToothDirContactRecord()
{
    m_eType = DIR_TYPE_BT_CONTACT;
}

BlueToothDirContactRecord::~BlueToothDirContactRecord()
{

}

//从xml解析
bool BlueToothDirContactRecord::ParseFromXMLNode(const xml_node & node)
{
    xml_attribute attrTmp = node.attribute(kszCXmlAttrDisplayName);
    if (attrTmp)
    {
        // 读取display name属性.
        m_strDisplayName = attrTmp.value();
    }

    attrTmp = node.attribute(kszOfficeNumber);
    if (attrTmp)
    {
        // 读取office_number属性.
        stNumber sNum;
        sNum.strName = kszOfficeNumber;
        sNum.strValue = attrTmp.value();
        m_listNumber.push_back(sNum);
#ifdef IF_BT_SUPPORT_PHONE
        g_pBlueToothDirManager->InsertToNumMap(sNum.strValue, m_strDisplayName);
#endif
    }

    attrTmp = node.attribute(kszMobileNumber);
    if (attrTmp)
    {
        // 读取mobile_number属性.
        stNumber sNum;
        sNum.strName = kszMobileNumber;
        sNum.strValue = attrTmp.value();
        m_listNumber.push_back(sNum);
#ifdef IF_BT_SUPPORT_PHONE
        g_pBlueToothDirManager->InsertToNumMap(sNum.strValue, m_strDisplayName);
#endif
    }

    attrTmp = node.attribute(kszOtherNumber);
    if (attrTmp)
    {
        // 读取other_number属性.
        stNumber sNum;
        sNum.strName = kszOtherNumber;
        sNum.strValue = attrTmp.value();
        m_listNumber.push_back(sNum);
#ifdef IF_BT_SUPPORT_PHONE
        g_pBlueToothDirManager->InsertToNumMap(sNum.strValue, m_strDisplayName);
#endif
    }

    attrTmp = node.attribute(kszCXmlAttrLine);
    if (attrTmp)
    {
        // 读取line type属性.
        m_nLine = atoi(attrTmp.value());

    }

    attrTmp = node.attribute(kszCXmlAttrRing);
    if (attrTmp)
    {
        // 读取ring type属性.
        m_strRing = attrTmp.value();

        commonUnits_Hide2Real(m_strRing, PHFILE_TYPE_RING_FILE);

        if (!pathFileExist(m_strRing.c_str()))
        {
            m_strRing = "Auto";
        }
    }
    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        attrTmp = node.attribute(kszCXmlAttrDefaultPhoto);
        if (attrTmp)
        {
            m_strDefaultPhoto = DEFULT_BT_IMAGE_PATH;
            commonUnits_Hide2Real(m_strDefaultPhoto, PHFILE_TYPE_CONTACT_IMAGE);
        }

        attrTmp = node.attribute(kszCXmlAttrSelectPhoto);
        if (attrTmp)
        {
            // 读取selected_photo属性.
            m_nSelectedPhoto = atoi(attrTmp.value());
        }
    }


    return true;
}

int ContactBroadSoftMenu::IsContactNameExist(const yl::string & stdName)
{
    IdChangeMap::iterator iter = m_mapNameToId.find(stdName);

    if (iter != m_mapNameToId.end())
    {
        return iter->second;
    }
    return -1;
}


void ContactBroadSoftMenu::CopyShowUIData(ContactBroadSoftMenu * pOriData)
{
    if (pOriData == NULL)
    {
        return;
    }

    m_eType = pOriData->m_eType;
    m_eInheritType = pOriData->m_eInheritType;
    m_strDisplayName = pOriData->m_strDisplayName;
    m_nParentId = pOriData->m_nParentId;
    m_nId = pOriData->m_nId;

    m_vChildrenId = pOriData->m_vChildrenId;
    m_strRing = pOriData->m_strRing;
    m_eStatus = pOriData->m_eStatus;
    m_bReqNext = pOriData->m_bReqNext;

    m_nCountPerPage = pOriData->m_nCountPerPage;
    m_nServerTotalCount = pOriData->m_nServerTotalCount;
}

bool ContactBroadSoftCommon::ParseFromXMLNode(const xml_node & node)
{
    xml_node nodeChild = node.first_child();
    yl::string strContent;
    yl::string strNodeName;

    for (; nodeChild; nodeChild = nodeChild.next_sibling())
    {
        strContent = nodeChild.child_value();
        if (strContent.empty())  // 结点文本为空, 略过.
        {
            continue;
        }

        strNodeName = nodeChild.name();
        if (strNodeName.compare(kszNodeName) == 0)
        {
            m_strDisplayName = strContent;
        }
        else if (strNodeName.compare(kszNodeNumber) == 0)
        {
            stNumber stNum;
            stNum.strName = kszBsftNumber;
            stNum.strValue = strContent;
            m_listNumber.push_back(stNum);
        }
    }  // Inner for.

    return true;
}

void ContactBroadSoftPersonal::SetNumber(const yl::string & strNumber)
{
    m_listNumber.clear();
    stNumber stNum;
    stNum.strName = kszBsftNumber;
    stNum.strValue = strNumber;
    m_listNumber.push_back(stNum);
}

bool ContactBroadSoftPersonal::ParseFromXMLNode(const xml_node & node)
{
    xml_node nodeChild = node.first_child();
    yl::string strContent;
    yl::string strNodeName;

    for (; nodeChild; nodeChild = nodeChild.next_sibling())
    {
        // 如果文本为空, 则略过.
        strContent = nodeChild.child_value();
        if (strContent.empty())
        {
            continue;
        }

        strNodeName = nodeChild.name();
        if (strNodeName.compare(kszNodeName) == 0)  // 读取名字结点.
        {
            m_strDisplayName = strContent;
        }
        else if (strNodeName.compare(kszNodeNumber) == 0)  // 读取号码结点.
        {
            stNumber stNum;
            stNum.strName = kszBsftNumber;
            stNum.strValue = strContent;
            m_listNumber.push_back(stNum);
        }
    }  // for.

    return true;
}

bool ContactBroadSoftCustom::ParseFromXMLNode(const xml_node & node)
{
    xml_node nodeChild = node.first_child();
    yl::string strContent;
    yl::string strNodeName;

    for (; nodeChild; nodeChild = nodeChild.next_sibling())
    {
        strContent = nodeChild.child_value();

        strNodeName = nodeChild.name();
        if (strNodeName.compare(kszNodeLastName) == 0)  // 读取姓结点.
        {
            m_strLastName = strContent;
        }
        else if (strNodeName.compare(kszNodeFirstName) == 0)  // 读取名结点.
        {
            m_strFirstName = strContent;
        }
        else if (strNodeName.compare(kszNodeUserId) == 0)  // 读取联系人user id.
        {
            m_strUserId = strContent;
        }

        else if (strNodeName.compare(kszNodeNumber) == 0)  // 读取联系人号码.
        {
            stNumber stNum;
            stNum.strName = kszBsftNumber;
            stNum.strValue = strContent;
            m_listNumber.push_back(stNum);
        }
        else if (strNodeName.compare(kszNodeExtension) == 0)  // 读取联系人扩展.
        {
            stNumber stNum;
            stNum.strName = kszBsftExtension;
            stNum.strValue = strContent;
            //Broadsoft特殊需要把Extention 放在最前面
            m_listNumber.push_front(stNum);
        }

        else if (strNodeName.compare(kszNodeMobile) == 0)  // 读取联系人手机号.
        {
            stNumber stNum;
            stNum.strName = kszBsftMobile;
            stNum.strValue = commonAPI_TrimString(strContent, " ", TRIM_ALL);
            m_listNumber.push_back(stNum);
        }

        else if (strNodeName.compare(kszNodeEmail) == 0)  // 读取联系人E-mail.
        {
            m_strEmail = strContent;
        }
        else if (strNodeName.compare(kszNodeName) == 0)  // 读取联系人Name.
        {
            m_strName = strContent;
        }
        else if (strNodeName.compare(kszNodeDepartment) == 0)
        {
            m_strDepartment = strContent;
        }
//      else if (strNodeName.compare(kszNodeIpmId) == 0)  // 读取Presence ID.
//      {
//          m_strImpId = strContent;
//      }
    }

    //http://192.168.1.99/Bug.php?BugID=35176
    if (configParser_GetConfigInt(kszBsftDirNameShowMode) == 0)
    {
        m_strDisplayName = m_strFirstName + " " + m_strLastName;
    }
    else
    {
        m_strDisplayName = m_strLastName + "," + m_strFirstName;
    }

    //m_strDisplayName = m_strFirstName + "," + m_strLastName;//排序比较用

    return true;
}

bool ContactBroadSoftGroup::ParseFromXMLNode(const xml_node & node)
{
    xml_node nodeChild = node.first_child();

    const char * strNodeName;

    for (; nodeChild; nodeChild = nodeChild.next_sibling())
    {
        strNodeName = nodeChild.name();
        if (strcmp(strNodeName, kszNodeLastName) == 0)  // 读取姓结点.
        {
            m_strLastName = nodeChild.child_value();
        }
        else if (strcmp(strNodeName, kszNodeFirstName) == 0) // 读取名结点.
        {
            m_strFirstName = nodeChild.child_value();
        }
//      else if (strcmp(strNodeName, kszNodeUserId) == 0)   // 读取联系人user id.
//      {
//          m_strUserId = nodeChild.child_value();
//      }
        //      else if (strNodeName.compare(kszNodeGroupId) == 0)  // 读取联系人group id.
        //      {
        //          m_strGroupId = strContent;
        //      }
        else if (strcmp(strNodeName, kszNodeNumber) == 0)   // 读取联系人号码.
        {
            stNumber stNum;
            stNum.strName = kszBsftNumber;
            stNum.strValue = nodeChild.child_value();
            m_listNumber.push_back(stNum);
        }
        else if (strcmp(strNodeName, kszNodeExtension) == 0)  // 读取联系人扩展.
        {
            stNumber stNum;
            stNum.strName = kszBsftExtension;
            stNum.strValue = nodeChild.child_value();
            //Broadsoft特殊需要把Extention 放在最前面
            m_listNumber.push_front(stNum);
        }
        else if (strcmp(strNodeName, kszNodeAddDetails) == 0)
        {
            xml_node nodeAddDetailsChild = nodeChild.first_child();
            for (; nodeAddDetailsChild; nodeAddDetailsChild = nodeAddDetailsChild.next_sibling())
            {
                const char * strChildNodeName = nodeAddDetailsChild.name();


                if (strcmp(strChildNodeName, kszNodeMobile) == 0) // 读取联系人手机号.
                {
                    stNumber stNum;
                    stNum.strName = kszBsftMobile;
                    stNum.strValue = nodeAddDetailsChild.child_value();
                    m_listNumber.push_back(stNum);
                }
                //              else if (strChildNodeName.compare(kszNodeYahooId) == 0)  // 读取联系人yahoo id.
                //              {
                //                  m_strYahooId = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodePaper) == 0)  // 读取联系人paper.
                //              {
                //                  m_strPaper = strChildNodeContent;
                //              }
                else if (strcmp(strChildNodeName, kszNodeEmail) == 0)   // 读取联系人E-mail.
                {
                    m_strEmail = nodeAddDetailsChild.child_value();
                }
                //              else if (strChildNodeName.compare(kszNodeTitle) == 0)  // 读取联系人头衔.
                //              {
                //                  m_strTitle = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeLocation) == 0)  // 读取联系人居住地.
                //              {
                //                  m_strLocation = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeAddressLine1) == 0)  // 读取联系人addressLine1.
                //              {
                //                  m_strAddressLine1 = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeAddressLine2) == 0)  // 读取联系人addressLine2.
                //              {
                //                  m_strAddressLine2 = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeCity) == 0)  // 读取联系人所在的城市.
                //              {
                //                  m_strCity = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeState) == 0)  // 读取联系人所在的镇.
                //              {
                //                  m_strState = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeZip) == 0)  // 读取联系人的ZIP号.
                //              {
                //                  m_StrZip = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeCountry) == 0)  // 读取联系人的国家.
                //              {
                //                  m_strCountry = strChildNodeContent;
                //              }
                else if (strcmp(strChildNodeName, kszNodeDepartment) == 0)   // 读取联系人的部门.
                {
                    m_strDepartment = nodeAddDetailsChild.child_value();
                }
            }
        }
    }  // Inner for.


    //http://192.168.1.99/Bug.php?BugID=35176
    if (configParser_GetConfigInt(kszBsftDirNameShowMode) == 0)
    {
        m_strDisplayName = m_strFirstName + " " + m_strLastName;
    }
    else
    {
        m_strDisplayName = m_strLastName + "," + m_strFirstName;
    }

    //m_strDisplayName = m_strFirstName + "," + m_strLastName;//排序比较用
    return true;
}

bool ContactBroadSoftEnterprise::ParseFromXMLNode(const xml_node & node)
{
    xml_node nodeChild = node.first_child();

    const char * strNodeName;

    for (; nodeChild; nodeChild = nodeChild.next_sibling())
    {

        strNodeName = nodeChild.name();
        if (strcmp(strNodeName, kszNodeLastName) == 0) // 读取姓结点.
        {
            m_strLastName = nodeChild.child_value();
        }
        else if (strcmp(strNodeName, kszNodeFirstName) == 0) // 读取名结点.
        {
            m_strFirstName = nodeChild.child_value();
        }
//      else if (strcmp(strNodeName, kszNodeUserId) == 0) // 读取联系人user id.
//      {
//          m_strUserId = nodeChild.child_value();
//      }
        //      else if (strNodeName.compare(kszNodeGroupId) == 0)  // 读取联系人group id.
        //      {
        //          m_strGroupId = strContent;
        //      }
        else if (strcmp(strNodeName, kszNodeNumber) == 0) // 读取联系人号码.
        {
            stNumber stNum;
            stNum.strName = kszBsftNumber;
            stNum.strValue = nodeChild.child_value();
            m_listNumber.push_back(stNum);
        }
        // http://10.2.1.199/Bug.php?BugID=94717 DUT在拨号界面输入云地址簿中A的号码，左侧显示两个联系人A
        else if (strcmp(strNodeName, kszNodeExtension) == 0
                 && m_eType != DIR_TYPE_CLOUD_ITEM_ENTERPRISE) // 读取联系人扩展.
        {
            stNumber stNum;
            stNum.strName = kszBsftExtension;
            stNum.strValue = nodeChild.child_value();
            //Broadsoft特殊需要把Extention 放在最前面
            m_listNumber.push_front(stNum);
        }
        else if (strcmp(strNodeName, kszNodeAddDetails) == 0)
        {
            xml_node nodeAddDetailsChild = nodeChild.first_child();
            for (; nodeAddDetailsChild; nodeAddDetailsChild = nodeAddDetailsChild.next_sibling())
            {
                const char * strChildNodeName = nodeAddDetailsChild.name();


                if (strcmp(strChildNodeName, kszNodeMobile) == 0) // 读取联系人手机号.
                {
                    stNumber stNum;
                    stNum.strName = kszBsftMobile;
                    stNum.strValue = nodeAddDetailsChild.child_value();
                    m_listNumber.push_back(stNum);
                }
                //              else if (strChildNodeName.compare(kszNodeYahooId) == 0)  // 读取联系人yahoo id.
                //              {
                //                  m_strYahooId = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodePaper) == 0)  // 读取联系人paper.
                //              {
                //                  m_strPaper = strChildNodeContent;
                //              }
                else if (strcmp(strChildNodeName, kszNodeEmail) == 0) // 读取联系人E-mail.
                {
                    m_strEmail = nodeAddDetailsChild.child_value();
                }
                //              else if (strChildNodeName.compare(kszNodeTitle) == 0)  // 读取联系人头衔.
                //              {
                //                  m_strTitle = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeLocation) == 0)  // 读取联系人居住地.
                //              {
                //                  m_strLocation = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeAddressLine1) == 0)  // 读取联系人addressLine1.
                //              {
                //                  m_strAddressLine1 = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeAddressLine2) == 0)  // 读取联系人addressLine2.
                //              {
                //                  m_strAddressLine2 = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeCity) == 0)  // 读取联系人所在的城市.
                //              {
                //                  m_strCity = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeState) == 0)  // 读取联系人所在的镇.
                //              {
                //                  m_strState = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeZip) == 0)  // 读取联系人的ZIP号.
                //              {
                //                  m_StrZip = strChildNodeContent;
                //              }
                //              else if (strChildNodeName.compare(kszNodeCountry) == 0)  // 读取联系人的国家.
                //              {
                //                  m_strCountry = strChildNodeContent;
                //              }
                else if (strcmp(strChildNodeName, kszNodeDepartment) == 0) // 读取联系人的部门.
                {
                    m_strDepartment = nodeAddDetailsChild.child_value();
                }
            }
        }
    }  // Inner for.

    //http://192.168.1.99/Bug.php?BugID=35176
    if (configParser_GetConfigInt(kszBsftDirNameShowMode) == 0)
    {
        m_strDisplayName = m_strFirstName + " " + m_strLastName;
    }
    else
    {
        m_strDisplayName = m_strLastName + "," + m_strFirstName;
    }

    //m_strDisplayName = m_strFirstName + "," + m_strLastName;//排序比较用
    return true;
}

bool ContactRemoteMenu::ParseFromXMLNode(const xml_node & node)
{
    if (node.empty())
    {
        return false;
    }

    //第一种格式
    // 获取名称属性的值.
    xml_attribute AttrName =  node.attribute(kszRemoteNodeName);
    if (!AttrName.empty())
    {
        m_strDisplayName.assign(AttrName.value(), strlen(AttrName.value()));
        return !m_strDisplayName.empty();
    }

    //第二种格式
    // 获取Name结点的值.
    xml_node NodeName = node.child(kszRemoteNodeName);
    if (!NodeName.empty())
    {
        m_strDisplayName.assign(NodeName.child_value(), strlen(NodeName.child_value()));
        // m_strDisplayName = commonAPI_TrimString(m_strDisplayName, " ");
    }

    // 读取URL地址.
    xml_node NodeUrl = node.child(kszRemoteNodeUrl);
    if (!NodeUrl.empty())
    {
        m_strURL.assign(NodeUrl.child_value(), strlen(NodeUrl.child_value()));
        //m_strURL = commonAPI_TrimString(m_strURL, " ");
    }

    bool isSucc = !m_strDisplayName.empty();

    return isSucc;
}

yl::string ContactRemoteMenu::GetUrl()
{
    if (!m_bSearchRootGroup)
    {
        return m_strURL;
    }

    // 变量定义.
    static const char kszMac[] = "#MAC";
    static const char kszIp[] = "#IP";
    static const char kszSearch[] = "#SEARCH";

    yl::string strUrl = m_strURL;

    // 替换ip地址.
    yl::string strIp;
    netGetWanIP(strIp);

    string_replace(strUrl, kszIp, strIp);

    yl::string strMac = netGetMACText(':');

    // 替换物理地址.
    string_replace(strUrl, kszMac, strMac);

    string_replace(strUrl, kszSearch, m_strSearchKey);

    DIR_INFO("After repalce:strUrl = %s", strUrl.c_str());

    return strUrl;
}

bool ContactRemoteHotKey::ParseFromXMLNode(const xml_node & node)
{
    if (node.empty())
    {
        return false;
    }

    // 获取Name结点的值.
    xml_node NodeName = node.child(kszRemoteNodeName);
    if (!NodeName.empty())
    {
        m_strKey.assign(NodeName.child_value(), strlen(NodeName.child_value()));
        //m_strKey = commonAPI_TrimString(m_strKey, " ");
    }

    // 读取URL地址.
    xml_node pNodeUrl = node.child(kszRemoteNodeUrl);
    if (!pNodeUrl.empty())
    {
        m_strURL.assign(pNodeUrl.child_value(), strlen(pNodeUrl.child_value()));
        //m_strURL = commonAPI_TrimString(m_strURL, " ");
    }

    bool isSucc = m_strKey.empty();

    return isSucc;
}

bool ContactRemoteUnit::ParseFromXMLNode(const xml_node & node)
{
#define kszNodePhone "Telephone"  // Phone结点.
#define kszNodeExtra "Extra"

#define kszAttrUnitPhone1 "Phone1"  // 具体联系人号码1.
#define kszAttrUnitPhone2 "Phone2"  // 具体联系人号码2.
#define kszAttrUnitPhone3 "Phone3"  // 具体联系人号码3.

#define kszAttrUnitLabel  "label"

    if (node.empty())
    {
        return false;
    }

    //第一种解析
    // 获取名称属性的值.
    xml_attribute AttrName = node.attribute(kszRemoteNodeName);
    if (!AttrName.empty())
    {
        m_strDisplayName.assign(
            AttrName.value(), strlen(AttrName.value()));

        m_strNameIndex = commonAPI_transT9String(m_strDisplayName);

        // 获取各个号码.
        yl::string strNumber;
        xml_attribute AttrNumber = node.attribute(kszAttrUnitPhone1);
        if (!AttrNumber.empty())
        {
            stNumber stNum;
            stNum.strName = kszNodePhone;
            stNum.strValue.assign(AttrNumber.value(), strlen(AttrNumber.value()));
            m_listNumber.push_back(stNum);
        }

        AttrNumber = node.attribute(kszAttrUnitPhone2);
        if (!AttrNumber.empty())
        {
            stNumber stNum;
            stNum.strName = kszNodePhone;
            stNum.strValue.assign(AttrNumber.value(), strlen(AttrNumber.value()));
            m_listNumber.push_back(stNum);
        }

        AttrNumber = node.attribute(kszAttrUnitPhone3);
        if (!AttrNumber.empty())
        {
            stNumber stNum;
            stNum.strName = kszNodePhone;
            stNum.strValue.assign(AttrNumber.value(), strlen(AttrNumber.value()));
            m_listNumber.push_back(stNum);
        }

        bool isSucc = !m_strDisplayName.empty();

        return isSucc;
    }

    //第二种解析
    // 获取Name结点的值.
    xml_node NodeName = node.child(kszRemoteNodeName);
    if (!NodeName.empty())
    {
        m_strDisplayName.assign(NodeName.child_value(), strlen(NodeName.child_value()));
        m_strNameIndex = commonAPI_transT9String(m_strDisplayName);
    }

    // 读取各个号码.
    for (xml_node NodePhone = node.child(kszNodePhone);
            !NodePhone.empty();
            NodePhone = NodePhone.next_sibling(kszNodePhone))
    {
        stNumber stNum;
        xml_attribute attrTmp = NodePhone.attribute(kszAttrUnitLabel);
        if (attrTmp)
        {
            stNum.strName = attrTmp.value();
        }
        else
        {
            stNum.strName = kszNodePhone;
        }
        stNum.strValue.assign(NodePhone.child_value(), strlen(NodePhone.child_value()));

        if (!stNum.strValue.empty())
        {
            m_listNumber.push_back(stNum);
        }
    }  // for.

    // 读取Extra
    for (xml_node NodePhone = node.child(kszNodeExtra);
            !NodePhone.empty();
            NodePhone = NodePhone.next_sibling(kszNodeExtra))
    {
        stNumber stExtra;
        stExtra.strValue.assign(NodePhone.child_value(), strlen(NodePhone.child_value()));
        if (stExtra.strValue.empty())
        {
            continue;
        }

        xml_attribute attrTmp = NodePhone.attribute(kszAttrUnitLabel);
        stExtra.strName = attrTmp ? attrTmp.value() : kszNodeExtra;

        m_listExtra.push_back(stExtra);
    }  // for.

    bool isSucc = !m_strDisplayName.empty();

    return isSucc;
}

//////////////////////////////////////////////////////////////////////////
//Broadsoft

BSFTLogGroupRecord::BSFTLogGroupRecord()
{
    m_eType = DIR_TYPE_BSFT_LOG_GROUP;
    m_strDisplayName = "";
    m_strServerUrl = "";
    m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
    m_nAccountID = -1;
}


//////////////////////////////////////////////////////////////////////////
//BSFTCallLogRecord
//从xml解析
bool BSFTCallLogRecord::ParseFromXMLNode(const xml_node & node)
{
    yl::string strNodeContent, strNodeName;

    xml_node nodeChild = node.first_child();
    for (; nodeChild; nodeChild = nodeChild.next_sibling())
    {
        // 如果文本为空, 则略过.
        strNodeContent = nodeChild.child_value();
        if (strNodeContent.empty())
        {
            continue;
        }
        strNodeName = nodeChild.name();
        if (strNodeName.compare(kszBSFTLogNodeName) == 0)
        {
            m_strDisplayName = strNodeContent;
        }
        else if (strNodeName.compare(kszBSFTLogNodeNumber) == 0)
        {
            stNumber stNum;
            stNum.strName = kszBsftNumber;
            stNum.strValue = strNodeContent;
            m_listNumber.push_back(stNum);
        }
        else if (strNodeName.compare(kszBSFTLogNodeTime) == 0)
        {
            m_strBornTick = strNodeContent;
        }
        else if (strNodeName.compare(kszBSFTLogCountryCode) == 0)
        {
            m_strCountryCode = strNodeContent;
        }
        else if (strNodeName.compare(kszBSFTLogNodeCallLogId) == 0)
        {
            m_strCallLogId = strNodeContent;
        }
    }

    // http://192.168.1.99/Bug.php?BugID=22677
    // 随便拨打一个不在话机地址薄里面的号码，结果话机的Call Log里面会显示Unvailable，正常直接显示这个拨打过的号码；
    // 这个问题是BSFT的问题，但是BSFT不肯修改，所以只能话机做修改；
    if (strcasecmp(m_strDisplayName.c_str(), "Unavailable") == 0)
    {
        m_strDisplayName = GetNumber();
    }

    return true;
}

CALL_TYPE BSFTCallLogRecord::GetLogType()
{
    //APP_INFO("--111111-----######################------type [%d]",m_eLogType);
    return m_eLogType;
}

const::yl::string & BSFTCallLogRecord::GetNumber()
{
    return GetNumberByIndex(0);
}

bool BSFTCallLogRecord::IsMutable()
{
    CALL_TYPE nType = GetLogType();
    return (nType == CT_MISSCALL || nType == CT_INCOMING);
}

bool DirectoryShortcutData::Compare(DirectoryShortcutData & reflIter,
                                    DirectoryShortcutData & refrIter)
{
    return reflIter.m_nPriority <= refrIter.m_nPriority;
}

//从xml解析
bool DirectoryShortcutData::ParseFromXMLNode(const xml_node & xmlNode)
{
    static const char kszAttrIdName[] = "id_name";  // ID名.
    static const char kszAttrDispName[] = "display_name";  // 显示名.
    static const char kszAttrPriority[] = "priority";  // 优先级.
    static const char kszAttrEnable[] = "enable";  // 是否可用.

    xml_attribute attrTmp = xmlNode.attribute(kszAttrIdName);
    if (attrTmp)
    {
        m_strIdName  = attrTmp.value();
    }

    attrTmp = xmlNode.attribute(kszAttrDispName);
    if (attrTmp)
    {
        // 读取office_number属性.
        m_strDisplayName = attrTmp.value();
    }

    attrTmp = xmlNode.attribute(kszAttrEnable);
    if (attrTmp)
    {
        // 读取mobile_number属性.
        m_isEnable = (attrTmp.as_int() == 1);
    }

    // 如果没有开启, 则将权限设置为0, 否则读取配置.
    if (m_isEnable)
    {
        attrTmp = xmlNode.attribute(kszAttrPriority);
        if (attrTmp)
        {
            m_nPriority = attrTmp.as_int();

        }
    }
    else
    {
        m_nPriority = knInvalidPriority;
    }

    return true;
}

bool RemoteIdCmp(const SuperDirData & reflData, const SuperDirData & refrData)
{
    return g_pRemoteDirManager->GetCompResult(reflData.m_nID, refrData.m_nID) < 0;
}

bool RemoteContactIdCmp(int nLeftId, int nRightId)
{
    return g_pRemoteDirManager->GetCompResult(nLeftId, nRightId) < 0;
}


#if IF_FEATURE_GBBOOK
/*        <multiRef id="id3" soapenc:root="0" soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xsi:type="ns2:AddressBookEntryDO" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:ns2="addrbook.ws.nortelnetworks.com">
<name xsi:type="xsd:string">test123</name>
<buddy xsi:type="xsd:boolean">false</buddy>
<businessPhoneNumber xsi:type="xsd:string" xsi:nil="true"/>
<emailAddress xsi:type="xsd:string" xsi:nil="true"/>
<fax xsi:type="xsd:string" xsi:nil="true"/>
<firstName xsi:type="xsd:string">test1</firstName>
<group xsi:type="ns3:AddrBookGroupNaturalKeyDO" xsi:nil="true" xmlns:ns3="addrbook.data.ws.nortelnetworks.com"/>
<homePhoneNumber xsi:type="xsd:string" xsi:nil="true"/>
<lastName xsi:type="xsd:string" xsi:nil="true"/>
<mobile xsi:type="xsd:string" xsi:nil="true"/>
<pager xsi:type="xsd:string" xsi:nil="true"/>
<photoURL xsi:type="xsd:string" xsi:nil="true"/>
<primaryContact xsi:type="xsd:string">55555@yealink.siot45.gbiot</primaryContact>
</multiRef>*/
bool ContactGBBookUnit::ParseFromXMLNode(const xml_node & node)
{
    for (xml_node childNode = node.first_child(); !childNode.empty();
            childNode = childNode.next_sibling())
    {
        yl::string strName = childNode.name();
        yl::string strValue = childNode.child_value();

        if (strName.compare("name") == 0)
        {
            m_strDisplayName = strValue;
        }
        else if (strName.compare("emailAddress") == 0)
        {
            m_strEmail = strValue;
        }
        else if (strName.compare("firstName") == 0)
        {
            m_strFirstName = strValue;
        }
        else if (strName.compare("lastName") == 0)
        {
            m_strLastName = strValue;
        }
        else if (strName.compare("group") == 0)
        {
            xml_attribute href = childNode.attribute("href");

            if (!href.empty())
            {
                m_strGroupFlag = href.as_string();
                m_strGroupFlag = commonAPI_TrimString(m_strGroupFlag, "#", TRIM_LEFT);
            }
        }
        else if (strName.compare(kszGbBookFax) == 0
                 || strName.compare(kszGbBookbusinessNumber) == 0
                 || strName.compare(kszGbBookhomeNumber) == 0
                 || strName.compare(kszGbBookmobile) == 0
                 || strName.compare(kszGbBookpager) == 0
                 || strName.compare(kszGbconferenceURL) == 0
                )
        {
            stNumber stNum;
            stNum.strName = strName;
            stNum.strValue = strValue;
            m_listNumber.push_back(stNum);
        }
        else if (strName.compare(kszGbBookprimaryContact) == 0)
        {
            stNumber stNum;
            stNum.strName = strName;
            int iPos = strValue.find('@');
            if (iPos != yl::string::npos)
            {
                stNum.strValue = strValue.substr(0, iPos);
            }
            else
            {
                stNum.strValue = strValue;
            }
            m_listNumber.push_back(stNum);
        }
    }

//  if (m_strDisplayName.empty())
//  {
//      m_strDisplayName = m_strFirstName + " " + m_strLastName;
//  }

    m_strNameIndex = commonAPI_transT9String(m_strDisplayName);

    return true;
}

SearchHitType ContactGBBookUnit::SearchHit(const yl::string & strSearchKey,
        SearchMethod searchMethod /*= SEARCH_BY_AUTO*/, StringMatchType eMatchtype /*= CONTAIN_MATCH*/)
{
    SearchHitType eType = BaseContactRecord::SearchHit(strSearchKey, searchMethod, eMatchtype);
    if (HIT_TYPE_NONE != eType)
    {
        return eType;
    }

    if (commonAPI_StringMatch(m_strFirstName, strSearchKey, false, eMatchtype))
    {
        // 名字的索引匹配.
        return HIT_TYPE_NAME;
    }

    if (commonAPI_StringMatch(m_strLastName, strSearchKey, false, eMatchtype))
    {
        // 名字的索引匹配.
        return HIT_TYPE_NAME;
    }

    return HIT_TYPE_NONE;
}
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
//////////////////////////////////////////////////////////////////////////
//Metaswitch
MTSWLogGroupRecord::MTSWLogGroupRecord()
{
    m_eType = DIR_TYPE_MTSW_LOG_GROUP;
    m_strDisplayName = "";
    m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
}


//////////////////////////////////////////////////////////////////////////
//MTSWCallLogRecord


CALL_TYPE MTSWCallLogRecord::GetLogType()
{
    return m_eLogType;
}

const::yl::string & MTSWCallLogRecord::GetNumber()
{
    return GetNumberByIndex(0);
}

bool MTSWCallLogRecord::PraseData(cJSON * pObject)
{
#define kszName "Name"
#define kszDirectoryNumber "DirectoryNumber"
#define kszDateTime "DateTime"
#define kszDuration "Duration"

    char * pValue = NULL;

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszName);

    if (pValue != NULL)
    {
        m_strDisplayName = pValue;
    }

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszDirectoryNumber);

    if (pValue != NULL)
    {
        stNumber stNum;
        stNum.strName = kszDirectoryNumber;
        stNum.strValue = pValue;

        m_listNumber.push_back(stNum);
    }

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszDateTime);

    if (pValue != NULL)
    {
        m_strBornTick = TransformTime(pValue);
    }

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszDuration);

    if (pValue != NULL)
    {
        m_strDuration = pValue;
    }

    return !m_strBornTick.empty();
}

#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
int ContactMetaSwitchMenu::IsContactNameExist(const yl::string & strFirstName,
        const yl::string & strLastName)
{
    yl::string strName = strFirstName + " " + strLastName;
    IdChangeMap::iterator iter = m_mapNameToId.find(strName);

    if (iter != m_mapNameToId.end())
    {
        return iter->second;
    }
    return -1;
}

int ContactMetaSwitchMenu::IsContactNameExist(const yl::string & strName)
{
    IdChangeMap::iterator iter = m_mapNameToId.find(strName);

    if (iter != m_mapNameToId.end())
    {
        return iter->second;
    }
    return -1;
}


void ContactMetaSwitchContactList::SetNumber(const yl::string & strName,
        const yl::string & strNumber)
{
    //m_listNumber.clear();
    stNumber stNum;
    stNum.strName = strName;
    stNum.strValue = strNumber;
    m_listNumber.push_back(stNum);
}

SearchHitType ContactMetaSwitchContactList::SearchHit(const yl::string & strSearchKey,
        SearchMethod searchMethod /*= SEARCH_BY_AUTO*/, StringMatchType eMatchtype /* = CONTAIN_MATCH */)
{
    SearchHitType eType = BaseContactRecord::SearchHit(strSearchKey, searchMethod, eMatchtype);
    if (HIT_TYPE_NONE != eType)
    {
        return eType;
    }

    if (commonAPI_StringMatch(m_strNickName, strSearchKey, false, eMatchtype))
    {
        // 名字的索引匹配.
        return HIT_TYPE_NAME;
    }

    if (commonAPI_StringMatch(m_strOrganisation, strSearchKey, false, eMatchtype))
    {
        // 名字的索引匹配.
        return HIT_TYPE_NAME;
    }

    return HIT_TYPE_NONE;
}

#define kszMtswName "N"
#define kszMtswGroup "GROUP"

#define kszMtswFirstName "GIVEN"
#define kszMtswLastName "FAMILY"

#define kszMtswNickName "NICKNAME"
#define kszMtswTel  "TEL"
#define kszMtswOrg  "ORG"
#define kszMtswTitle    "TITLE"
#define kszMtswUID  "UID"

#define kszMtswAttr "_"
#define kszMtswOrgName "ORGNAME"

#define kszMetaHomeNumber "_HOME"
#define kszMetaWorkNumber "_WORK"
#define kszMetaMoble    "_CELL"
#define kszMetaFax      "_FAX"
#define kszMetaNumber   "NUMBER"

bool ContactMetaSwitchContactList::PraseData(cJSON * pObject)
{
    cJSON * pContactName = cJSON_GetObjectItem(pObject, kszMtswName);

    char * pValue = NULL;

    if (pContactName != NULL)
    {
        pValue = cJSON_GetFistChildValueByObjectName(pContactName, kszMtswFirstName);

        if (pValue != NULL)
        {
            m_strFirstName = pValue;
        }

        pValue = cJSON_GetFistChildValueByObjectName(pContactName, kszMtswLastName);

        if (pValue != NULL)
        {
            m_strLastName = pValue;
        }

        m_strDisplayName = m_strFirstName;
        if (!m_strDisplayName.empty() && !m_strLastName.empty())
        {
            m_strDisplayName.append(",");
        }
        m_strDisplayName.append(m_strLastName);
    }

    cJSON * pGroup = cJSON_GetObjectItem(pObject, kszMtswGroup);

    if (pGroup)
    {
        for (cJSON * pGroupChild = pGroup->child->child; pGroupChild; pGroupChild = pGroupChild->next)
        {
            if (pGroupChild->string == NULL)
            {
                continue;
            }

            if (strcmp(pGroupChild->string, kszMtswNickName) == 0)
            {
                cJSON * pNickName = pGroupChild;

                if (pNickName && pNickName->child)
                {
                    cJSON * pObjectValue = cJSON_GetObjectItem(pNickName->child, kszMtswAttr);

                    if (pObjectValue && pObjectValue->valuestring)
                    {
                        m_strNickName = pObjectValue->valuestring;
                    }
                }
            }

            else if (strcmp(pGroupChild->string, kszMtswTel) == 0)
            {
                //number
                cJSON * pTel = pGroupChild;
                if (pTel)
                {
                    for (cJSON * pData = pTel->child; pData; pData = pData->next)
                    {
                        stNumber stNum;

                        pValue = cJSON_GetFistChildValueByObjectName(pData, kszMetaNumber);

                        if (pValue == NULL)
                        {
                            continue;
                        }

                        stNum.strValue = pValue;

                        stNum.strName = pData->child->string;

//                      if (strcmp(pData->child->string, kszMetaHomeNumber) == 0)
//                      {
//                          stNum.strName = kszMtswHomeNumber;
//                      }
//                      else if (strcmp(pData->child->string, kszMetaWorkNumber) == 0)
//                      {
//                          stNum.strName = kszMtswWorkNumber;
//                      }
//                      else if (strcmp(pData->child->string, kszMetaMoble) == 0)
//                      {
//                          stNum.strName = kszMtswMobile;
//                      }
//                      else if (strcmp(pData->child->string, kszMetaFax) == 0)
//                      {
//                          stNum.strName = kszMtswFax;
//                      }
//                      else
//                      {
//                          stNum.strName = kszMtswOtherNumber;
//                      }
                        m_listNumber.push_back(stNum);
                    }
                }
            }
            else if (strcmp(pGroupChild->string, kszMtswOrg) == 0)
            {
                cJSON * pOrg = pGroupChild;

                if (pOrg && pOrg->child)
                {
                    pValue = cJSON_GetFistChildValueByObjectName(pOrg->child, kszMtswOrgName);

                    if (pValue != NULL)
                    {
                        m_strOrganisation = pValue;
                    }
                }
            }
            else if (strcmp(pGroupChild->string, kszMtswTitle) == 0)
            {
                cJSON * pTile = pGroupChild;

                if (pTile && pTile->child)
                {
                    cJSON * pObjectValue = cJSON_GetObjectItem(pTile->child, kszMtswAttr);

                    if (pObjectValue && pObjectValue->valuestring)
                    {
                        m_strJobTitle = pObjectValue->valuestring;
                    }
                }
            }
            else if (strcmp(pGroupChild->string, kszMtswUID) == 0)
            {
                cJSON * pUID = pGroupChild;

                if (pUID && pUID->child)
                {
                    cJSON * pObjectValue = cJSON_GetObjectItem(pUID->child, kszMtswAttr);

                    if (pObjectValue && pObjectValue->valuestring)
                    {
                        m_strUID = pObjectValue->valuestring;
                    }
                }
            }
        }
    }

    return !m_strUID.empty();
}

char * ContactMetaSwitchContactList::GetJsonString(bool bAdd/* = true*/)
{
    cJSON * root = cJSON_CreateObject();

    cJSON * vCardArray = cJSON_CreateArray();

    cJSON_AddItemToObject(root, "vCard", vCardArray);

    cJSON * contact = cJSON_CreateObject();

    cJSON_AddItemToArray(vCardArray, contact);

    cJSON * name = cJSON_CreateObject();

    cJSON_AddItemToObject(contact, "N", name);

    cJSON * GIVEN = cJSON_CreateObject();

    cJSON_AddItemToObject(name, "GIVEN", GIVEN);

    cJSON_AddItemToObject(GIVEN, "_", cJSON_CreateString(m_strFirstName.c_str()));

    cJSON * FAMILY = cJSON_CreateObject();

    cJSON_AddItemToObject(name, "FAMILY", FAMILY);

    cJSON_AddItemToObject(FAMILY, "_", cJSON_CreateString(m_strLastName.c_str()));


    cJSON * GroupArray = cJSON_CreateArray();


    cJSON_AddItemToObject(contact, "GROUP", GroupArray);

    cJSON * GroupContact = cJSON_CreateObject();

    cJSON_AddItemToArray(GroupArray, GroupContact);

    if (!bAdd)
    {
        cJSON * UIDArray = cJSON_CreateArray();

        cJSON * UID = cJSON_CreateObject();

        cJSON_AddItemToArray(UIDArray, UID);

        cJSON_AddItemToObject(UID, "_", cJSON_CreateString(m_strUID.c_str()));

        cJSON_AddItemToObject(GroupContact, "UID", UIDArray);
    }

    cJSON * TitleArray = cJSON_CreateArray();

    cJSON * Title = cJSON_CreateObject();

    cJSON_AddItemToArray(TitleArray, Title);

    cJSON_AddItemToObject(Title, "_", cJSON_CreateString(m_strJobTitle.c_str()));

    cJSON_AddItemToObject(GroupContact, "TITLE", TitleArray);



    cJSON * OrgArray = cJSON_CreateArray();

    cJSON * Org = cJSON_CreateObject();

    cJSON_AddItemToArray(OrgArray, Org);


    cJSON * OrgName = cJSON_CreateObject();


    cJSON_AddItemToObject(Org, "ORGNAME", OrgName);

    cJSON_AddItemToObject(OrgName, "_", cJSON_CreateString(m_strOrganisation.c_str()));

    cJSON_AddItemToObject(GroupContact, "ORG", OrgArray);


    cJSON * Tel = cJSON_CreateArray();

    cJSON_AddItemToObject(GroupContact, "TEL", Tel);

    for (YLList<stNumber>::ListIterator iter = m_listNumber.begin(); iter != m_listNumber.end(); iter++)
    {
        stNumber & stNum = *iter;

        cJSON * NumAll = cJSON_CreateObject();

        cJSON * Num = cJSON_CreateObject();

        cJSON_AddItemToObject(Num, "_", cJSON_CreateString(stNum.strValue.c_str()));

        cJSON_AddItemToObject(NumAll, "NUMBER", Num);

        if (stNum.strName.compare("NUMBER") != 0)
        {
            cJSON_AddItemToObject(NumAll, stNum.strName.c_str(), cJSON_CreateTrue());
        }

        cJSON_AddItemToArray(Tel, NumAll);
    }

    cJSON * NickArray = cJSON_CreateArray();

    cJSON * Nick = cJSON_CreateObject();

    cJSON_AddItemToArray(NickArray, Nick);

    cJSON_AddItemToObject(Nick, "_", cJSON_CreateString(m_strNickName.c_str()));

    cJSON_AddItemToObject(GroupContact, "NICKNAME", NickArray);

    char * out = cJSON_Print(root);
    cJSON_Delete(root);

    return out;
}

#define kszDirectoryNumber "DirectoryNumber"
#define kszExtentionName "Name"
#define kszExtDepartment "Department"
#define kszExtExtNumber "IntercomDialingCode"

bool ContactMetaSwitchExtention::PraseData(cJSON * pObject)
{
    char * pValue = NULL;

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszExtentionName);

    if (pValue != NULL)
    {
        m_strDisplayName = pValue;
    }

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszDirectoryNumber);

    if (pValue != NULL)
    {
        stNumber num;
        num.strName = kszMtswDirectoryNumber;
        num.strValue = pValue;
        m_listNumber.push_back(num);

        if (m_strDisplayName.empty())
        {
            m_strDisplayName = pValue;
        }
    }

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszExtExtNumber);

    if (pValue != NULL)
    {
        stNumber num;
        num.strName = kszMtswExtention;
        num.strValue = pValue;
        m_listNumber.push_back(num);

        if (m_strDisplayName.empty())
        {
            m_strDisplayName = pValue;
        }
    }


    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszExtDepartment);

    if (pValue != NULL)
    {
        m_strDepartment = pValue;
    }

    return !m_strDisplayName.empty();
}

bool ContactMetaSwitchMLHG::PraseData(cJSON * pObject)
{
    char * pValue = NULL;

    pValue = cJSON_GetFistChildValueByObjectName(pObject, "GroupType");

    if (pValue != NULL)
    {
        if (strcmp("MLHG", pValue) != 0)
        {
            return false;
        }
    }


    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszExtentionName);

    if (pValue != NULL)
    {
        m_strDisplayName = pValue;
    }

    pValue = cJSON_GetFistChildValueByObjectName(pObject, kszExtDepartment);

    if (pValue != NULL)
    {
        m_strDepartment = pValue;

    }

    return !m_strDisplayName.empty();
}
#endif
