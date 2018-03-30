#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__
#include "directoryenumtypes.h"
#include "xmlparser.hpp"
#include <yllist.h>
#include "ylvector.h"
#include <ylstring.h>
#include "commonapi/stringhelperapi.h"
#include "ylhashmap.h"
#include "interfacedefine.h"
#include <ylsmartptr.h>
#include "json/cjson.h"
#define LA_SEARCH_STR_LEN 20

#define DEFULT_BT_IMAGE_PATH "Default:bluetooth_default.png"    //蓝牙默认头像路径 http://10.2.1.199/Bug.php?BugID=92573
//#define DEFULT_BT_IMAGE_PATH SYSTEM_IMAGE_PATH "contacts/bluetooth_default.png"

struct BaseDirDataRecord;

typedef YLSmartPtr<BaseDirDataRecord> SmartPtr;

typedef YLHashMap<yl::string, yl::string, STRING_HASH> MapNumberToName;

typedef YLHashMap<yl::string, int, STRING_HASH> IdChangeMap;

typedef YLVector<int> VectorId;

struct stNumber
{
    yl::string strName;

    yl::string strValue;
};

typedef YLList<stNumber> ListNumber;

struct BaseDirDataRecord //数据项基类
{
    DIR_DATA_ENTRY_TYPE GetType()
    {
        return m_eType;
    }

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & xmlNode);

    //转化为xml节点
    virtual bool ToXMLNode(xml_node & xmlNode);

    //是否被搜索命中
    virtual SearchHitType SearchHit(const yl::string & strSearchKey, SearchMethod searchMethod,
                                    StringMatchType eMatchtype = CONTAIN_MATCH);


    virtual bool Compare(BaseDirDataRecord * pRecordToComm);

    virtual const yl::string & GetDisplayName();

    virtual bool IsMutable()
    {
        return true;
    }

    static bool Comp(BaseDirDataRecord * lpData, BaseDirDataRecord * rpData)
    {
        if (lpData == NULL || rpData == NULL)
        {
            return false;
        }
        return strcasecmp(lpData->m_strDisplayName.c_str(), rpData->m_strDisplayName.c_str()) < 0;
    }


    BaseDirDataRecord();
    virtual ~BaseDirDataRecord();

    DIR_DATA_ENTRY_TYPE m_eType;

    DIR_INHERIT_TYPE m_eInheritType;

    yl::string m_strDisplayName;

    int m_nParentId;

    int m_nId;
};

struct BaseContactRecord: public BaseDirDataRecord
{
    ListNumber m_listNumber;

    yl::string m_strNameIndex;

#if IF_BUG_27560
    bool m_bHideSipName;    // 是否隐藏号码
#endif

    bool GetNumberByName(const yl::string & strName, yl::string & strNumber);

    bool GetNumberByIndex(int index, yl::string & strNumber);

    const yl::string & GetNumberByIndex(int index);
    const yl::string & GetNameByIndex(int index);

    virtual const yl::string GetNameIndex();

    BaseContactRecord()
    {
        m_eInheritType = DIR_INHERIT_CONTACT;
        m_listNumber.clear();
        m_strNameIndex.clear();
#if IF_BUG_27560
        m_bHideSipName = false;
#endif
    }

    //是否被搜索命中
    virtual SearchHitType SearchHit(const yl::string & strSearchKey,
                                    SearchMethod searchMethod = SEARCH_BY_AUTO, StringMatchType eMatchtype = CONTAIN_MATCH);

    yl::string GetSearchShowNumber(const yl::string & strSearchKey);

};

//一般组结构
struct DirGroupRecord: public BaseDirDataRecord
{
    YLVector<int> m_vChildrenId;

    yl::string m_strRing;  // 组铃声文件名(无路径).


    FileDownloadStatus m_eStatus;

    bool m_bReqNext;


    DirGroupRecord()
    {
        m_eType = DIR_TYPE_GROUP;
        m_eInheritType = DIR_INHERIT_GROUP;
        m_vChildrenId.clear();
        m_strRing.clear();

        m_eStatus = FILE_DOWNLOADED_OK;
        m_bReqNext = false;
    }
};
//本地通话记录数据记录
struct LocalLogItemRecord: public BaseContactRecord
{
    yl::string m_strLocalSipName;  // 本地账号对应的注册名.
    yl::string m_strLocalSipServer;  // 本地账号对应的主机名.
    //yl::string m_strRemoteSipName;  // 对方注册名.
    yl::string m_strRemoteSipServer;  // 对方主机名.

    yl::string m_strForwardSipName;  // 转移至什么号码.
    yl::string m_strForwardSipServer;  // 转移至什么服务器.
    yl::string m_StrForwardDisplayName;  // 转移至谁.

    yl::string m_strBornTick;
    yl::string m_strDuration;  // 通话时长.原来为int m_nDuration，modify by yuyt, 2014/3/18.

    yl::string m_strRelationNum;
    CALL_RELATION m_eRelation;

    int m_nLine;

    yl::string m_strPic;

    int m_nNumOfTimes;   // 历史记录条数

#ifdef IF_SUPPORT_VIDEO
    bool m_bVideoCall;
#endif //#ifdef IF_SUPPORT_VIDEO

    LocalLogItemRecord();

    const yl::string & GetNumber();

    time_t GetAdjustTime();

    virtual const yl::string & GetDisplayName();

    virtual const yl::string GetNameIndex();

    void SetNumber(const yl::string & strNumber);

    void SetLogType(CALL_TYPE eType);

    CALL_TYPE GetLogType();

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);
    //转化为xml节点
    virtual bool ToXMLNode(xml_node & xmlNode);

    virtual bool IsMutable();

    const yl::string & GetPhoto();

    bool IsCloudCallLog();

#if IF_BUG_27560
    yl::string GetNumberToUI();

    yl::string GetDisplayNameToUI();
#endif

    void SetRelationInfo(CALL_RELATION eRelation = CR_NONE, yl::string strRelation = "");
};

//本地联系人组记录
struct LocalDirGroupRecord: public DirGroupRecord
{
    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & xmlNode);

    //转化为xml节点
    virtual bool ToXMLNode(xml_node & xmlNode);

    virtual ~LocalDirGroupRecord();
};

//本地联系人记录
struct LocalDirContactRecord: public BaseContactRecord
{
    yl::string m_strRing;  // 铃声名(无路径).
    int m_nLine;  // Line.
    yl::string m_strDivert;

    yl::string m_strDefaultPhoto;
    int m_nSelectedPhoto;  // 选中的图片.

#if IF_FEATURE_FAVORITE_DIR
    int m_nFavoriteIndex;
#endif //IF_FEATURE_FAVORITE_DIR

#ifdef IF_FEATURE_PSTN
    bool m_bIsFavorite;
#endif

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);

    //转化为xml节点
    virtual bool ToXMLNode(xml_node & node);

    LocalDirContactRecord();

    ~LocalDirContactRecord();
};

// 蓝牙联系人
struct BlueToothDirContactRecord: public LocalDirContactRecord
{
    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);

    BlueToothDirContactRecord();

    ~BlueToothDirContactRecord();
};

//////////////////////////////////////////////////////////////////////////
// Broadsof历史记录

// Broadsoft联系人和历史记录组
struct BSFTLogGroupRecord: public DirGroupRecord
{
    yl::string m_strServerUrl; //服务器地址

    CALL_TYPE m_eLogType;

    int m_nAccountID;
    // XSI port
    yl::string m_strPort;

    BSFTLogGroupRecord();
};

// Broadsof历史记录
struct BSFTCallLogRecord: public BaseContactRecord
{
    yl::string m_strBornTick;  // 建立时间.
    yl::string m_strCountryCode; //
    yl::string m_strCallLogId;// BSFT Log CallLogId

    const yl::string & GetNumber();

    CALL_TYPE m_eLogType;

#if IF_FEATURE_MUTI_CALLLOG
    int m_nLineID;
#endif

    BSFTCallLogRecord()
    {
        m_eType = DIR_TYPE_BSFT_CALLLOG;
        m_strBornTick.clear();
        m_strCountryCode.clear();
        m_strCallLogId.clear();
        m_eLogType = CT_UNKNOW;
#if IF_FEATURE_MUTI_CALLLOG
        m_nLineID = -1;
#endif
    }

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);

    CALL_TYPE GetLogType();

    virtual bool IsMutable();
};
// Metaswitch联系人和历史记录组
struct MTSWLogGroupRecord: public DirGroupRecord
{
    MTSWLogGroupRecord();
};

// Metaswitch历史记录
struct MTSWCallLogRecord: public BaseContactRecord
{
    yl::string m_strBornTick;  // 建立时间.
    yl::string m_strDuration;  // 通话时间

    const yl::string & GetNumber();

    CALL_TYPE m_eLogType;

    MTSWCallLogRecord()
    {
        m_eType = DIR_TYPE_MTSW_CALLLOG;
        m_strBornTick.clear();
        m_eLogType = CT_UNKNOW;
        m_strDuration = "00:00:00";
    }

    CALL_TYPE GetLogType();

    bool PraseData(cJSON * pObject);
};

struct ContactBroadSoftMenu : public DirGroupRecord
{
    yl::string m_strFullUrl;

    yl::string m_strSearchUrl;

    yl::string m_strCurSearchKey;

    int m_nServerTotalCount;
    int m_nCountPerPage;

    int m_nNextStartIndex;

    int m_nSearchStartIndex;

    bool m_bCustomSearch;

    //组内联系人名字到ID的转换表
    IdChangeMap m_mapNameToId;

    //存在返回id，否则返回-1
    int IsContactNameExist(const yl::string & stdName);

    ContactBroadSoftMenu()
    {
        m_eType = DIR_TYPE_BROADSOFT_MENU;
        Reset();
    }

    void Reset()
    {
        //m_mapNameToId.clear();
        m_nServerTotalCount = 0;
        m_nCountPerPage = 0;
        m_nNextStartIndex = 0;
        m_nSearchStartIndex = 0;
        m_strCurSearchKey.clear();

        m_bCustomSearch = false;

        m_strDisplayName = "";
        m_strFullUrl = "";
        m_strSearchUrl = "";
        m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
    }

    void CopyShowUIData(ContactBroadSoftMenu * pOriData);
};

struct ContactBroadSoftCommon : public BaseContactRecord
{
    ContactBroadSoftCommon()
    {
        m_eType = DIR_TYPE_BROADSOFT_ITEM_COMMON;
    }

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);
};

// Struct ContactBroadSoftPersonal, 记录某个broad soft联系人 .
struct ContactBroadSoftPersonal : public BaseContactRecord
{
    ContactBroadSoftPersonal()
    {
        m_eType = DIR_TYPE_BROADSOFT_ITEM_PERSONAL;
        m_nParentId = kszPersonalId;
    }

    void SetNumber(const yl::string & strNumber);

    virtual bool ParseFromXMLNode(const xml_node & node);
};
// Struct ContactBroadSoftEnterprise, ContactBroadSoftItemBase子类.
// 记录BroadSoft某个企业的信息.
struct ContactBroadSoftEnterprise : public BaseContactRecord
{
    yl::string m_strLastName;  // 姓.
    yl::string m_strFirstName;  // 名.
//  yl::string m_strUserId;  // 用户id.
//  yl::string m_strGroupId;  // 组id.
//  yl::string m_strYahooId;  // Yahoo id.
//  yl::string m_strPaper;  // Paper.
    yl::string m_strEmail;  // E-mail.
//  yl::string m_strTitle;  // Title.
//  yl::string m_strLocation;  // 位置.
//  yl::string m_strAddressLine1;  // Address line 1.
//  yl::string m_strAddressLine2;  // Address line 2.
//  yl::string m_strCity;  // 城市.
//  yl::string m_strState;  // 镇.
//  yl::string m_StrZip;  // Zip号.
//  yl::string m_strCountry;  // 国家.
    yl::string m_strDepartment;  // 部门.

    ContactBroadSoftEnterprise()
    {
        m_eType = DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE;
        m_nParentId = kszEnterPriseId;
    }
    virtual bool ParseFromXMLNode(const xml_node & node);
};

struct ContactBroadSoftCustom : public BaseContactRecord
{
    yl::string m_strLastName;  // 姓.
    yl::string m_strFirstName;  // 名.
    yl::string m_strUserId;  // 用户id.
    yl::string m_strName;   //Name
    yl::string m_strEmail;  // E-mail.
    yl::string m_strDepartment;
    //yl::string m_strImpId; //Presence ID

    ContactBroadSoftCustom()
    {
        m_eType = DIR_TYPE_BROADSOFT_ITEM_CUSTOM;
    }

    virtual bool ParseFromXMLNode(const xml_node & node);
};


struct ContactBroadSoftGroup : public BaseContactRecord
{
    yl::string m_strLastName;  // 姓.
    yl::string m_strFirstName;  // 名.
//  yl::string m_strUserId;  // 用户id.
//  yl::string m_strGroupId;  // 组id.
//  yl::string m_strYahooId;  // Yahoo id.
//  yl::string m_strPaper;  // Paper.
    yl::string m_strEmail;  // E-mail.
//  yl::string m_strTitle;  // Title.
//  yl::string m_strLocation;  // 位置.
//  yl::string m_strAddressLine1;  // Address line 1.
//  yl::string m_strAddressLine2;  // Address line 2.
//  yl::string m_strCity;  // 城市.
//  yl::string m_strState;  // 镇.
//  yl::string m_StrZip;  // Zip号.
//  yl::string m_strCountry;  // 国家.
    yl::string m_strDepartment;  // 部门.

    ContactBroadSoftGroup()
    {
        m_eType = DIR_TYPE_BROADSOFT_ITEM_GROUP;
        m_nParentId = kszGroupId;
    }

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);
};


enum SORT_TYPE
{
    SORT_BY_FIRSTNAME,
    SORT_BY_LASTNAME,
};

struct ContactMetaSwitchMenu : public DirGroupRecord
{
    yl::string m_strFullUrl;

    yl::string m_strSearchUrl;

    int m_nServerTotalCount;
    int m_nCountPerPage;

    yl::string m_strUID;
    YLList<yl::string> m_listGroupMem;

    //组内联系人名字到ID的转换表
    IdChangeMap m_mapNameToId;

    //存在返回id，否则返回-1
    int IsContactNameExist(const yl::string & strFirstName, const yl::string & strLastName);
    int IsContactNameExist(const yl::string & strName);

    ContactMetaSwitchMenu()
    {
        m_eType = DIR_TYPE_METASWITCH_MENU;
        Reset();
    }

    void Reset()
    {
        //m_mapNameToId.clear();
        m_nServerTotalCount = 0;
        m_nCountPerPage = 0;

        m_strDisplayName = "";
        m_strFullUrl = "";
        m_strSearchUrl = "";
        m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
    }
};

struct ContactMetaSwitchContactList : public BaseContactRecord
{
    yl::string m_strLastName;  // 姓.
    yl::string m_strFirstName;  // 名.
    yl::string m_strNickName;
    yl::string m_strJobTitle;
    yl::string m_strOrganisation;

    //yl::string m_strDepartment;  // 部门.

    yl::string m_strUID;

    void SetNumber(const yl::string & strName, const yl::string & strNumber);

    ContactMetaSwitchContactList()
    {
        m_eType = DIR_TYPE_METASWITCH_ITEM_CONTACTLIST;
        m_nParentId = kszContactList;
    }
    virtual SearchHitType SearchHit(const yl::string & strSearchKey,
                                    SearchMethod searchMethod = SEARCH_BY_AUTO, StringMatchType eMatchtype = CONTAIN_MATCH);

    bool PraseData(cJSON * pObject);

    char * GetJsonString(bool bAdd = true);
};

struct ContactMetaSwitchExtention : public BaseContactRecord
{
    yl::string m_strDepartment;  // 部门.

    ContactMetaSwitchExtention()
    {
        m_eType = DIR_TYPE_METASWITCH_ITEM_EXTENTION;
        m_nParentId = kszExtention;

        m_strDepartment.clear();
    }

    bool PraseData(cJSON * pObject);
};

struct ContactMetaSwitchMLHG : public BaseContactRecord
{
    yl::string m_strDepartment;  // 部门.

    ContactMetaSwitchMLHG()
    {
        m_eType = DIR_TYPE_METASWITCH_ITEM_MLHG;
        m_nParentId = kszMLHG;
        m_strDepartment.clear();
    }

    bool PraseData(cJSON * pObject);
};
// 继承于CContactRemoteItemBase类, 是远程联系人菜单.
struct ContactRemoteMenu : public DirGroupRecord
{
    yl::string m_strURL;  // 远程联系人地址.

    yl::string m_strTitle;  // Url标题.

    yl::string m_strUserName;

    yl::string m_strPassWord;

    YLList<int> m_listHotKey;

    int m_nNextPageId;

    int m_nprevPageId;

    bool m_bSearchRootGroup;

    yl::string m_strSearchKey;

    ContactRemoteMenu()
    {
        m_eType = DIR_TYPE_REMOTE_MENU;
        Reset();
    }

    virtual ~ContactRemoteMenu() {}

    void Reset()
    {
        m_strDisplayName = "";
        m_strURL = "";
        m_strTitle = "";
        m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
        m_listHotKey.clear();
        m_nNextPageId = knInvalidRId;
        m_nprevPageId = knInvalidRId;
        m_bSearchRootGroup = false;

        m_strSearchKey = "";
    }
    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);

    yl::string GetUrl();
};

// Struct ContactRemoteHotKey.
// 继承自ContactRemoteItemBase, 表示远程联系人快捷键.
struct ContactRemoteHotKey : public ContactRemoteMenu
{
    yl::string m_strKey;  // 快捷键的值.

    ContactRemoteHotKey()
    {
        m_eType = DIR_TYPE_REMOTE_HOTKEY;
    }

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);
};

// 继承于CContactRemoteItemBase类, 记录具体联系人的数据.
struct ContactRemoteUnit : public BaseContactRecord
{
    ListNumber m_listExtra;

    ContactRemoteUnit()
    {
        m_eType = DIR_TYPE_REMOTE_UNIT;
        m_listExtra.clear();
    }

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);
};

// 联系人快捷方式配置项的数据结构, 对应于配置文件中的一个配置项.
struct DirectoryShortcutData
{
    static const int knInvalidPriority = 0xFFFF;

    yl::string m_strIdName;  // Id名.
    int m_nPriority;  // 优先级.
    bool m_isEnable;  // 是否启用.
    yl::string m_strDisplayName;

    DirectoryShortcutData()
    {
        m_nPriority = knInvalidPriority;
        m_isEnable = false;
        m_strIdName.clear();
        m_strDisplayName.clear();
    }

    static bool Compare(DirectoryShortcutData & reflIter, DirectoryShortcutData & refrIter);

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & xmlNode) ;
};


struct ContactCloudEnterprise : public ContactBroadSoftEnterprise
{
    ContactCloudEnterprise()
    {
        m_eType = DIR_TYPE_CLOUD_ITEM_ENTERPRISE;
        m_nParentId = kszEnterPriseId;
    }
};

enum _PRESENCE_TYPE
{
    _PRESENCE_TYPE_ONLINE,  // 用户在线而且准备好沟通
    _PRESENCE_TYPE_CHATTY,  // 用户在线而且希望和人聊天
    _PRESENCE_TYPE_AWAY,       // 用户在线,但是已经离开电脑10分钟
    _PRESENCE_TYPE_EXTENDED_AWAY,  // Extended Away
    _PRESENCE_TYPE_BUSY,       // 用户处于忙碌状态
    _PRESENCE_TYPE_OFFLINE, // 用户处于脱机状态
    _PRESENCE_TYPE_UNKNOWN, // 未知状态
    _PRESENCE_TYPE_NONE,
    _PRESENCE_TYPE_MAX,
};

// 搜索结果项.
struct DirectorySearchData
{
    yl::string m_strName;  // 名字.
    yl::string m_strNumber;  // 号码.

    yl::string m_strPic;
    yl::string m_strNameIndex;
    _PRESENCE_TYPE m_eStatus;

    DIR_TYPE m_eDirType;


    bool IsSearchHit(const yl::string & strSearchKey, SearchMethod searchMethod)
    {

        if (searchMethod == SEARCH_BY_T9)
        {
            if (commonAPI_StringMatch(m_strNameIndex, strSearchKey, true)
                    || commonAPI_StringMatch(m_strNumber, strSearchKey, true))

            {
                // 名字的索引匹配, 插入到搜索结果链表末尾.
                return true;
            }
        }
        else if (searchMethod == SEARCH_BY_NAME)
        {
            // 默认搜索只匹配名字.
            if (commonAPI_StringMatch(m_strName, strSearchKey, false))
            {
                // 名字的索引匹配.
                return true;
            }
        }
        return false;
    }


    const yl::string & GetNumber() const
    {
        return m_strNumber;
    }

    DirectorySearchData()
    {
        Reset();
    }


    static bool Comp(const DirectorySearchData & leftDsd, const DirectorySearchData & rightDsd)
    {
        return strcasecmp(leftDsd.m_strName.c_str(), rightDsd.m_strName.c_str()) < 0;
    }

    void Reset()
    {
        m_strName.clear();
        m_strNumber.clear();
        m_strPic.clear();
        m_strNameIndex.clear();
        m_eStatus = _PRESENCE_TYPE_NONE;
        m_eDirType = BASE_DIR;
    }
};

struct SuperDirData
{
    int m_nID;
    int m_nNumberIdex;

    SuperDirData()
    {
        m_nID = -1;
        m_nNumberIdex = 0;
    }
};

struct SuperSearchItemListData
{
    YLList<DirectorySearchData> m_listContact;  // Searched result list.
    yl::string m_strSearch;  // Search string.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count;


    SuperSearchItemListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listContact.clear();
        m_strSearch = "";
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

bool RemoteIdCmp(const SuperDirData & reflData, const SuperDirData & refrData);

bool RemoteContactIdCmp(int nLeftId, int nRightId);

#if IF_FEATURE_GBBOOK

struct ContactGBMenu : public DirGroupRecord
{
    yl::string m_strUrl;

    yl::string m_strPabGroupUrl;

    ContactGBMenu()
    {
        m_eType = DIR_TYPE_GB_MENU;
        Reset();
    }

    void Reset()
    {
        m_strUrl = "";
        m_strPabGroupUrl = "";
        m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
    }
};


struct ContactGBBookUnit : public BaseContactRecord
{
    yl::string m_strEmail;

    yl::string m_strFirstName;

    yl::string m_strLastName;

    yl::string m_strGroupFlag;

    //从xml解析
    virtual bool ParseFromXMLNode(const xml_node & node);

    ContactGBBookUnit()
    {
        m_eType = DIR_TYPE_GB_UNIT;

        m_strEmail = "";

        m_strFirstName = "";

        m_strLastName = "";

        m_strGroupFlag = "";

        m_nParentId = kszAllContactId;
    }
    //是否被搜索命中
    virtual SearchHitType SearchHit(const yl::string & strSearchKey,
                                    SearchMethod searchMethod = SEARCH_BY_AUTO, StringMatchType eMatchtype = CONTAIN_MATCH);
};


#endif

#endif //_DIRECTORY_DATA_H_
