#ifndef __E911_ADDRESS_H__
#define __E911_ADDRESS_H__

#include <ETLLib.hpp>

struct E911AddressData
{
    int m_nID;
    yl::string m_strDisplayName;    //地址名字
    int m_nParentID;              // 存储地址的上一级ID
    YLVector<int> m_vChildrenID;   // 存储下一级地址列表
};

struct E911AddressListData
{
    YLVector<int> m_vlistID;
    int m_nIndexFrom;
    int m_nCountGet;
    int m_nTotalCount;

    E911AddressListData()
    {
        Clear();
    }

    void Clear()
    {
        m_vlistID.clear();
        m_nIndexFrom = 0;
        m_nCountGet = 0;
        m_nTotalCount = 0;
    }
};

#define kszE911RootID 0

typedef YLHashMap<int, E911AddressData*> MapE911Address;

#include "innermsgdefine.h"

enum E911MessageDefine
{
    E911_MESSAGE_BEGIN = INNER_MSG_E911_MESSAGE_BEGIN,

    E911_MESSAGE_DOWNLOAD_RESULT,

    E911_MESSAGE_DOWNLOAD_ADDRESS,

    E911_MESSAGE_END = INNER_MSG_E911_MESSAGE_END,
};

enum E911DownLoadStatus
{
    E911_DOWNLOAD_STATUS_NONE = 0,
    E911_DOWNLOAD_STATUS_DOWNLOADING,
    E911_DOWNLOAD_STATUS_CANCEL_DOWNLOAD,
    E911_DOWNLOAD_STATUS_DOWNLOAD_FAIL,
    E911_DOWNLOAD_STATUS_DOWNLOAD_SUCC,
};

#endif // __E911_ADDRESS_H__
