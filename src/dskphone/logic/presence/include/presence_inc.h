#ifndef _PRESENCE_INC_H_
#define _PRESENCE_INC_H_
#if FEATURE_BROADTOUCH_UC

#include <yllist.h>
#include <ylstring.h>
#include "ylhashmap.h"

#include <ETLLib.hpp>
#include "innermsgdefine.h"
#include "contacts/directory/include/commondata.h"

// Presence消息定义.
enum PresenceMessage
{
    PRESENCE_MESSAGE_BEGIN = INNER_MSG_PRESENCE_MESSAGE_BEGIN,

    //注册者状态改变
    PRESENCE_MESSAGE_USER_STATUS_CHANGE = PRESENCE_MESSAGE_BEGIN + 1,

    PRESENCE_MESSAGE_USER_INFO_CHANGE  =  PRESENCE_MESSAGE_USER_STATUS_CHANGE + 1,

    PRESENCE_MESSAGE_AVATAR_CHANGE = PRESENCE_MESSAGE_USER_INFO_CHANGE + 1,

    //Favorite联系人状态或头像改变,内容改变lparm = 1表示清空
    PRESENCE_MESSAGE_FAVORITE_CHANGE = PRESENCE_MESSAGE_AVATAR_CHANGE + 1,

    //dsskey配置为UC FAvorite类型，或有其他类型配置为N/A类型
    PRESENCE_MESSAGE_DSSKEY_CHANGE = PRESENCE_MESSAGE_FAVORITE_CHANGE + 1,

    //联系人增删，修改备注消息
    PRESENCE_CONTACT_CHANGE = PRESENCE_MESSAGE_DSSKEY_CHANGE + 1, //模块内部使用消息

    //联系人状态改变消息
    PRESENCE_PRESENCE_CHANGE = PRESENCE_CONTACT_CHANGE + 1, //模块内部使用消息

    // UC联系人断开
    PRESENCE_PRESENCE_DISCONNECT = PRESENCE_PRESENCE_CHANGE + 1,

    PRESENCE_MESSAGE_END = INNER_MSG_PRESENCE_MESSAGE_END
};

// 目前只需要XML流状态,以后还会扩展
struct XMPPResult
{
    yl::string strXMLStream;
};

enum RefreshType
{
    REFRESH_NONE,
    REFRESH_SINGLE,
    REFRESH_ALL,
};

enum AttrType
{
    ATTR_TYPE_NONE,
    ATTR_TYPE_ALL,
    ATTR_TYPE_NUMBER,
    ATTR_TYPE_NAME_AND_NUM,
};

struct Attribute
{
    yl::string m_strName; //属性名
    yl::string m_strValue; //属性值
    AttrType m_eType;  //属性类型，如号码

    Attribute()
    {
        m_strName.clear();
        m_strValue.clear();
        m_eType = ATTR_TYPE_NONE;
    }
};

enum eSortType
{
    SORT_BY_DEFOULT,
    SORT_BY_NAME
};

struct PresenceInfo
{
    yl::string   strContactID; //jid
    yl::string   strContactName;//名字
    yl::string  strAvatarInfo;  //头像文件名
    yl::string  strAvatarSignature;     //头像图片的特征码
    _PRESENCE_TYPE ePresenceType; //状态

    yl::string m_strTitle;//签名

    YLList<Attribute> m_listAttribute;//属性列表

    bool m_bFirst; //是否第一次

    yl::string m_strSeq;//排序字符串

    int m_nLocalId;

    yl::string & GetSortKey()
    {
        if (!m_strSeq.empty())
        {
            return m_strSeq;
        }
        else
        {
            return strContactName;

        }
    }

    bool operator < (PresenceInfo & item)
    {
        if ((int)ePresenceType < (int)item.ePresenceType)
        {
            return true;
        }
        else if (ePresenceType == item.ePresenceType)
        {
            return strcasecmp(GetSortKey().c_str(), item.GetSortKey().c_str()) < 0;
        }

        return false;
    }

    yl::string GetAttributeValue(AttrType eType)
    {
        for (YLList<Attribute>::iterator iter = m_listAttribute.begin(); iter != m_listAttribute.end();
                iter++)
        {
            Attribute & refData = *iter;
            if (refData.m_eType == eType)
            {
                return refData.m_strValue;
            }
        }

        return "";
    }

    bool GetAttributeValue(int iIndex, AttrType eType, Attribute & stAtt)
    {
        int i = 0;
        for (YLList<Attribute>::iterator iter = m_listAttribute.begin(); iter != m_listAttribute.end();
                iter++)
        {
            Attribute & refData = *iter;
            if (refData.m_eType == eType)
            {
                if (i == iIndex)
                {
                    stAtt = refData;
                    return true;
                }

                i++;
            }
        }

        return false;
    }

    yl::string GetAttributeValueByName(const yl::string & strAttName)
    {
        for (YLList<Attribute>::iterator iter = m_listAttribute.begin(); iter != m_listAttribute.end();
                iter++)
        {
            Attribute & refData = *iter;
            if (refData.m_strName == strAttName)
            {
                return refData.m_strValue;
            }
        }

        return "";
    }

    PresenceInfo()
    {
        Reset();
    }

    void Reset()
    {
        m_bFirst = true;
        ePresenceType = _PRESENCE_TYPE_NONE;
        strAvatarInfo   = "";
        strContactID  = "";
        strContactName = "";
        m_listAttribute.clear();
        strAvatarSignature = "";
        m_strTitle.clear();
        m_nLocalId = -1;
        m_strSeq.clear();
    }
};

enum PresenceType
{
    PRESENCE_BASE,
    PRESENCE_LYNC,
    PRESENCE_UC,
};

// 自定义类型
typedef YLHashMap<int, PresenceInfo> MapPresenceInfo;
typedef YLHashMap<yl::string, int> MapKeyToId;
typedef int GroupKey;

typedef YLList<GroupKey>            ListContactID;


typedef ListContactID::const_iterator   CIterContactID;
typedef MapPresenceInfo::iterator        IterMapPresenceInfo;

typedef YLList<int> ListPresenceInfo;

typedef ListPresenceInfo::iterator       IterPresenceInfo;
typedef ListPresenceInfo::const_iterator CIterPresenceInfo;


struct GroupItem
{
    yl::string m_strDisplayName;//组名
    ListContactID m_nIdList;//组内联系人jid链表
    GroupKey m_keyId;

    int iSeq;

    void ReSet()
    {
        m_strDisplayName.clear();
        m_nIdList.clear();

        m_keyId = -1;
        iSeq = -1;

    }

    static bool Comp(GroupItem & lItem, GroupItem & rItem)
    {
        return lItem.iSeq < rItem.iSeq;
    }

    GroupItem()
    {
        ReSet();
    }
};

typedef YLList<GroupItem> GroupList;

struct ContactBuddyListData
{
    ListPresenceInfo m_list;
    int m_nIndexFrom; // Retrieve from which index.
    int m_nCountToGet; // Count to retrieve.
    int m_nTotalCount; // Total item count.
    PresenceType m_eType;

    ContactBuddyListData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = PRESENCE_UC;
        m_list.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

#define ALL_BUDDIES_ID 1
#define FAVOURITE_ID 2

#define kszAllBuddiesName "All Buddies"
#define kszFavourite "UC Favorites"

#endif //FEATURE_BROADTOUCH_UC

#endif
