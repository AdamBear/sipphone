#ifndef _DIRECTORY_EXTERNAL_DATA_STRUCTURE_HEADER_
#define _DIRECTORY_EXTERNAL_DATA_STRUCTURE_HEADER_
#include <ylstring.h>
#include <yllist.h>
#include "ylvector.h"

//本地联系人对外的数据结构
struct ContactGeneralItemData
{
    yl::string m_strDisplayName;  // Contact display name.
    yl::string m_strGroupName;  // Group name.
    yl::string m_strOfficePhoneNumber;  // Office phone number.
    yl::string m_strMobilePhoneNumber;  // Mobile phone number.
    yl::string m_strOtherPhoneNumber;  // Other phone number.
    yl::string m_strRing;  // Ring name(no path).
    yl::string m_strAutoDivert;
    int m_nLine;  // Line.

    yl::string m_strDefaultPhoto;
    int m_nSelectedPhoto;  // 选中的图片.

#if IF_FEATURE_FAVORITE_DIR
    int m_nFavoriteIndex;
#endif

#ifdef IF_FEATURE_PSTN
    bool m_bIsFavorite;
#endif

    ContactGeneralItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_strDisplayName.clear();
        m_strGroupName.clear();
        m_strOfficePhoneNumber.clear();
        m_strMobilePhoneNumber.clear();
        m_strOtherPhoneNumber.clear();
        m_strRing.clear();
        m_strAutoDivert.clear();
        m_nLine = 0;

        m_strDefaultPhoto.clear();
        m_nSelectedPhoto = 0;

#if IF_FEATURE_FAVORITE_DIR
        m_nFavoriteIndex = -1;
#endif

#ifdef IF_FEATURE_PSTN
        m_bIsFavorite = false;
#endif
    }
};

//本地联系人组数据
struct ContactGroupData
{
    yl::string m_strDisplayName;  // Group display name.
    yl::string m_strRing;  // Group ring name.
};


// BroadSoft contact item.
struct ContactBroadSoftItemData
{
    int m_eType;  // type.
    yl::string m_strDisplayName;  // Display name.
    yl::string m_strNumber;  // 电话号码.
    yl::string m_strUserId;  // 用户id.
    yl::string m_strGroupId;  // 组id.
    yl::string m_strExtension;  // 扩展.

    yl::string m_strMobile;  // 移动号码.
    yl::string m_strEmail;  // E-mail.
    yl::string m_strDepartment;  // 部门.

    bool m_bCriteriaMode;

    ContactBroadSoftItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = 0;
        m_strDisplayName = "";
        m_strUserId = "";
        m_strGroupId = "";
        m_strExtension = "";
        m_strNumber = "";
        m_strMobile = "";
        m_strEmail = "";
        m_strDepartment = "";
        m_bCriteriaMode = false;
    }
};
// Metaswitch contact item.
struct ContactMetaSwitchItemData
{
    int m_eType;  // type.
    yl::string m_strDisplayName;  // Display name.
    yl::string m_strGroupId;  // 组id.
    yl::string m_strExtension;  // 扩展.

    yl::string m_strLastName;  //
    yl::string m_strFirstName;  // 名.
    yl::string m_strNickName;
    yl::string m_strJobTitle;
    yl::string m_strOrganisation;
    yl::string m_strHomePhone;
    yl::string m_strWorkPhone;
    yl::string m_strMobile;
    yl::string m_strFax;
    yl::string m_strOtherPhone;
    yl::string m_strSMS;
    yl::string m_strEmail1;  // E-mail.
    yl::string m_strEmail2;  // E-mail.
    yl::string m_strDepartment;  // 部门.

    ContactMetaSwitchItemData()
    {
        Reset();
    }

    void Reset()
    {
        m_eType = 0;
        m_strDisplayName = "";
        m_strGroupId = "";
        m_strExtension = "";
        m_strLastName = "";  //
        m_strFirstName = "";  // 名.
        m_strNickName = "";
        m_strJobTitle = "";
        m_strOrganisation = "";
        m_strHomePhone = "";
        m_strWorkPhone = "";
        m_strMobile = "";
        m_strFax = "";
        m_strOtherPhone = "";
        m_strSMS = "";
        m_strEmail1 = "";  // E-mail.
        m_strEmail2 = "";  // E-mail.
        m_strDepartment = "";  // 部门.
    }
};

// BroadSoft contact list. if m_nIndexFrom == 0 && m_nCountToGet == 0,
// Retrieve all contact.
struct ContactListData
{
    YLVector<int> m_listId;  // BroadSoft contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count.

    ContactListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listId.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

// 联系人快捷菜单的数据.
struct DirShortcutData
{
    yl::string m_strIdName;  // ID名.

    yl::string m_strDisplayName;

    DirShortcutData()
    {
        Reset();
    }

    void Reset()
    {
        m_strIdName.clear();
        m_strDisplayName.clear();
    }
};

// 联系人快捷菜单列表.
struct DirShortcutListData
{
    YLList<DirShortcutData> m_listData;  // [out] 获取到的列表.
    int m_nIndexFrom;  // 开始下标.
    int m_nCountToGet;  // 获取数量.
    int m_nTotalCount;  // 总数.

    DirShortcutListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listData.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

struct FastIndex
{
    char m_ckey;
    int m_nIdex;
};

typedef YLList<FastIndex> FastIndexList;

//to do   -syl
//其他数据结构暂置于此
//Dir UI的结构，暂时放置于此
//Contact Menu类型
enum CONTACT_MENU_TYPE
{
    CMT_LOCAL = 0,
    CMT_BLACK,
    CMT_REMOTE,
    CMT_BROADSOFT_,
    CMT_LDAP,
    CMT_CALLLOG_,
    CMT_LAST, // 此值也被作为联系人选项隐藏列表数字大小,不应被设置为0或者负值
};

#define MAX_PHONEBOOK_URL_LENGTH 548        //远程地址簿URL长度

#endif
