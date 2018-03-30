#ifndef __DIRDEFINE_H__
#define __DIRDEFINE_H__

// 组Xml结点定义.
#define kszGXmlRoot  "root_group"
#define kszGXmlNodeGroup  "group"
#define kszGXmlAttrIdName  "id_name"
#define kszGXmlAttrDisplayName  "display_name"
#define kszGXmlAttrRing  "ring"
#define kszGXmlAttrReadOnly  "read_only"

//////////////////////////////////////////////////////////////////////////
// 联系人Xml结点定义.
#define kszXMLRootNode "root"
#define kszCXmlRoot  "root_contact"
#define kszCXmlNodeContact  "contact"
#define kszCXmlAttrDisplayName  "display_name"
// #define kszCXmlAttrOfficeNumber  "office_number"
// #define kszCXmlAttrMobileNumber  "mobile_number"
// #define kszCXmlAttrOtherNumber "other_number"
#define kszCXmlAttrLine  "line"
#define kszCXmlAttrRing  "ring"
#define kszCXmlAttrGroupIdName  "group_id_name"
#define kszCXmlAttrDefaultPhoto  "default_photo"
#define kszCXmlAttrSelectPhoto  "selected_photo"
#define kszCXmlAttrFavorate  "is_favorite"
#define kszCxmlAttrNameIndex  "name_index"

// 所有组的组名.
#define kszGroupNameAll  TRID_ALL_CONTACTS

#define kszGroupNameAllCalls TRID_ALL_CALLS
// 黑名单组的组名.
#define kszGroupNameBlackList  TRID_BLACKLIST
// Company组的组名.
#define kszGroupNameCompany  "Company"
// Family组的组名.
#define kszGroupNameFamily  "Family"
// Friend组的组名.
#define kszGroupNameFriend  "Friend"
// VIP组的组名.
//#define kszGroupNameVIP  "VIP"

// 联系人导出的Xml文件名.
#define kszXportXmlFile  "contact_xported.xml"

// 客户联系人Xml文件名.
#define kszMacXmlFile  "mac-contact.xml"

#define kszMacCallLogFile "mac-call_data.xml"

#define kszLocalMaxContactNum 1000

#define kszBlackListMaxNum 30

// 配置文件路径, 由外部指定
extern yl::string g_strConfigPath;

// 临时文件路径
extern yl::string g_strTempPath;

// 组Xml结点定义.
#define kszGXmlRoot  "root_group"
#define kszGXmlNodeGroup  "group"
#define kszGXmlAttrIdName  "id_name"
#define kszGXmlAttrDisplayName  "display_name"
#define kszGXmlAttrRing  "ring"
#define kszGXmlAttrReadOnly  "read_only"

//////////////////////////////////////////////////////////////////////////
// 联系人Xml结点定义.
#define kszCXmlRoot  "root_contact"
#define kszCXmlNodeContact  "contact"
#define kszCXmlAttrDisplayName  "display_name"
#define kszOfficeNumber  "office_number"
#define kszMobileNumber  "mobile_number"
#define kszOtherNumber "other_number"
#define kszCXmlAttrLine  "line"
#define kszCXmlAttrRing  "ring"
#define kszCXmlAttrGroupIdName  "group_id_name"
#define kszCXmlAttrDefaultPhoto  "default_photo"
#define kszCXmlAttrSelectPhoto  "selected_photo"
#define kszCXmlAttrFavorate  "is_favorite"
#define kszCxmlAttrNameIndex  "name_index"
#define kszCxmlAttrFavorite "priority"
#define kszCxmlAttrDivert   "auto_divert"

//////////////////////////////////////////////////////////////////////////
// 本地历史记录结点定义
// 根结点.
#define kszCallLogNodeRoot                  "root_call_log"
// 各个记录项结点.
#define kszNodeItem                 "item"
// 通话ID.
#define kszAttrCallId               "call_id"
// 通话类型.
#define kszAttrCallType             "type"
// 通话建立日期和时间.
#define kszAttrBornTick             "born_tick"
// 本地账号对应的注册名.
#define kszAttrLocalSipName         "local_sip_name"
// 本地账号对应的主机名.
#define kszAttrLocalSipServer       "local_sip_server"
// 对方注册名.
#define kszAttrRemoteSipName        "remote_sip_name"
// 对方显示名.
#define kszAttrRemoteDisplayName    "remote_display_name"
// 对方主机名.
#define kszAttrRemoteSipServer      "remote_sip_server"
// 通话时长.
#define kszAttrDuration             "duration"
// 同一天的相同记录数
#define kszAttrCount                "count"
//是否蓝牙手机通话记录
#define kszMobileCallLog            "btcalllog"
// 隐藏号码
#define kszAttrHideSipName         "hide_sipName"

#define kszAttrRelation         "relation"
#define kszAttrRelationNum         "relation_num"
#define kszAttrNumOfTimes       "number_of_times"

// 通话建立日期和时间.
#define kszAttrVideoCall            "video"

//定义最大缓冲区
#define  knMaxBufferLen 260

#define kszRemoteNodeName "Name"  // Name结点.
#define kszRemoteNodeUrl "URL"  // Url结点.


// Broadsoft历史节点
// 联系人入口节点.
#define kszBSFTLogNodeEntry "callLogsEntry"
#define kszBSFTLogCountryCode "countryCode"
// 联系人名字结点.
#define kszBSFTLogNodeName "name"
// 联系人号码结点.
#define kszBSFTLogNodeNumber "phoneNumber"
// 联系人号码结点.
#define kszBSFTLogNodeTime "time"
// 联系人callLogId结点.
#define kszBSFTLogNodeCallLogId "callLogId"


//BroadSoft 联系人节点
// 联系人入口节点.
#define kszNodeEntry  "entry"
// 联系人名字结点.
#define kszNodeName  "name"
// 联系人号码结点.
#define kszNodeNumber  "number"

// 联系人姓结点.
#define kszNodeLastName  "lastName"
// 联系人名结点.
#define kszNodeFirstName "firstName"
// 联系人user id.
#define kszNodeUserId  "userId"
// 联系人group id.
#define kszNodeGroupId  "groupId"
// 联系人扩展.
#define kszNodeExtension  "extension"
// 联系人手机号.
#define kszNodeMobile  "mobile"
// 联系人yahoo id.
#define kszNodeYahooId  "yahooId"
// 联系人paper.
#define kszNodePaper  "paper"
// 联系人E-mail.
#define kszNodeEmail  "emailAddress"
// 联系人头衔.
#define kszNodeTitle  "title"
// 联系人居住地.
#define kszNodeLocation  "location"
// 联系人addressLine1.
#define kszNodeAddressLine1  "addressLine1"
// 联系人addressLine2.
#define kszNodeAddressLine2  "addressLine2"
// 联系人所在的城市.
#define kszNodeCity  "city"
// 联系人所在的镇.
#define kszNodeState  "state"
// 联系人的ZIP号.
#define kszNodeZip  "zip"
// 联系人的国家.
#define kszNodeCountry  "country"
// 联系人的部门.
#define kszNodeDepartment  "department"

#define kszNodeIpmId "impId"

#define kszNodeAddDetails  "additionalDetails"

//http://xsp1.iop1.broadworks.net:80/com.broadsoft.xsi-actions/v2.0/user/Yealink2User1@as.iop1.broadworks.net/directories/Personal
#define BRROADWORKS_CONTACT_CMD_PREFEX "%s://%s:%s/com.broadsoft.xsi-actions/%s/user/%s/directories/"

#define BRROADWORKS_CONTACT_NEW_CMD_PREFEX "%s://%s:%s/%s/%s/user/%s/directories/"

#define BRROADWORKS_DEFAULT_URL "com.broadsoft.xsi-actions"

//#define szCalllistFile            (g_strConfigPath + "call_data.xml").c_str()
//#define szContactsFile            (g_strConfigPath + "contact_list.xml").c_str()
//#define szContactGroupFile        (g_strConfigPath + "contact_group_list.xml").c_str()

#define kszPlcmPhoneBook TRID_PLCMBOOK

#endif





