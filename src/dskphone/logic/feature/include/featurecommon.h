#ifndef __FEATURE_COMMON_H__
#define __FEATURE_COMMON_H__

#include <ETLLib.hpp>
#include "innermsgdefine.h"

#define DND_NODE_NAME           "DoNotDisturbEvent"
#define FWD_NODE_NAME           "ForwardingEvent"
#define ACD_LOGON_NAME          "AgentLoggedOnEvent"
#define ACD_READY_NAME          "AgentReadyEvent"
#define ACD_NOTREADY_NAME       "AgentNotReadyEvent"
#define ACD_LOGOFF_NAME         "AgentLoggedOffEvent"
#define CALL_RECORD_NAME        "xBw:CallRecordingModeEvent"

#define EXECUTIVE_NODE_NAME     "xBw:ExecutiveEvent"
#define ASSISTANT_NODE_NAME     "xBw:ExecutiveAssistantEvent"

#if NABLECOMM_BUGFREE_16773
#define ACD_WRAPUP_NAME         "AgentWorkingAfterCallEvent"
#endif

//////////////////////////////////////////////////////////////////////////
// The code info
class CCodeInfo
{
public:
    CCodeInfo()
    {
        m_bSendOn = false;
        m_bSendOff = false;
        m_strOnCode.clear();
        m_strOffCode.clear();
        m_strLastOffCode.clear();
    }
    // Write
    bool SetOnCode(const char * pOnCode);
    bool SetOffCode(const char * pOffCode);
    // Read
    const char * GetOnCode() const;
    const char * GetOffCode() const;
    const char * GetSendOffCode() const;

    // Status
    // Get
    bool NeedSendOn();
    bool NeedSendOff();
    // Set Send Flag
    // | bOn | [in] true: Set SendOn. false: Set SendOff
    void SetSendFlag(BOOL bOn);
    // Clean Send Flag
    void CleanSendFlag();

    // BackupCode
    inline void OnSendOnCode()
    {
        m_strLastOffCode = m_strOffCode;
        m_bSendOn = false;
    }
    inline void OnSendOffCode()
    {
        m_strLastOffCode.clear();
        m_bSendOff = false;
    }

public:
    yl::string m_strOnCode;
    yl::string m_strOffCode;
    // Fault-tolerant
    bool        m_bSendOn;
    bool        m_bSendOff;
    yl::string
    m_strLastOffCode; // 备份offcode. 当配置被清空时, 没有offcode无法关闭打开的Code的情况
};

//////////////////////////////////////////////////////////////////////////
// Code的上电发送标志
struct CodeSendFlag
{
    enum
    {
        Unknown,
        FWD,
        DND,
        Anonymous,
    };
    int iCodeType;    // FWD,DND,Anonymous
    YLVector<bool> arrSendFlag; // 是否有发送的标志

    CodeSendFlag();

    ~CodeSendFlag() {}
};

#if IF_FEATURE_PAGING
//////////////////////////////////////////////////////////////////////////
// Paging Group信息
struct PagingGroupInfo
{
    int iIndex;            // 序号
    yl::string strLabel;   // 显示名
    yl::string strAddress; // ip地址
    int iChannel;

    PagingGroupInfo()
    {
        iIndex = -1;
        strLabel = "";
        strAddress = "";
        iChannel = 0;
    }

    virtual ~PagingGroupInfo() {};
};

// 定义结构体数组
typedef YLVector<PagingGroupInfo> PagingListVec;

// 回调函数
typedef void (*PagingListCallBack)(void);
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// 用户权限
enum USER_TYPE
{
    UT_USER = 0,
    UT_VAR,
    UT_ADMIN,
    UT_NONE,
};

enum USER_MODE_MSG
{
    // wparam: old user, lparam: new user
    USER_MODE_MSG_USER_TYPE_CHANGE = INTER_MSG_USER_MODE_MESSAGE_BEGIN,
    USER_MODE_MSG_MAX = INTER_MSG_USER_MODE_MESSAGE_END
};

/////////////////////////////////////////////////////////////////////////////////////////
// rtp.xml中的配置项.
struct RTPConfigureItem
{
    yl::string m_strIdName;     // id名.
    yl::string m_strDisplayName;// 显示名.
    int m_nPriority;            // 优先级
    bool m_isEnable;            // 是否开启.

    RTPConfigureItem()
    {
        m_strIdName = "";
        m_strDisplayName = "";
        m_nPriority = 0xFF;
        m_isEnable = false;
    }
};

// RTP配置项列表.
typedef YLList<RTPConfigureItem> RTPConfigureList;

struct UserModePermission
{
    yl::string m_strName;
    bool m_bShow;
    bool m_bReadOnly;

    UserModePermission()
    {
        m_strName = "";
        m_bShow = false;
        m_bReadOnly = false;
    }

    void SetData(yl::string strName, bool bShow, bool bReadOnly)
    {
        m_strName = strName;
        m_bShow = bShow;
        m_bReadOnly = bReadOnly;
    }
};

struct UserModeListData
{
    YLVector<UserModePermission> m_list;  // Permission info list.
    int m_nIndexFrom;           // Retrieve from which index.
    int m_nCountToGet;          // Count to retrieve.
    int m_nTotalCount;          // Total item count.

    UserModeListData()
    {
        Reset();
    }

    void Reset()
    {
        m_list.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
    }
};

#endif // __FEATURE_COMMON_H__
