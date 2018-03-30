#ifndef __CCCOMMON_H__
#define __CCCOMMON_H__

#include <yllist.h>
#include <ylstring.h>
#include <ylhashmap.h>
#include "common/common_data_define.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "callcenter_data_define.h"
#include "innermsgdefine.h"
#include "talk/talklogic/include/uiandlogic_base.h"

// ACD type.
enum ACD_TYPE
{
    ACD_BASE = 0,       // no type.
    ACD_COSMOCOM = 1,   // idle.
    ACD_BROADSOFT = 2,  // available.
    ACD_GENESYS = 3,
#if IF_FEATURE_START2START_ACD
    ACD_START2START = 4,
#endif
    ACD_METASWITCH = 5,

    ACD_TYPE_MAX
};

//Hoteling状态
enum HOTELING_STATUS
{
    HOTELING_STATUS_NONE = -1,// no type.
    HOTELING_STATUS_LOGOUT, //Offline
    HOTELING_STATUS_LOGIN, //OnLine
};

//Call Center Queue Status
enum QUEUE_STATUS
{
    QUEUE_NONE = -1,
    QUEUE_EMPTY = 0,  //empty
    QUEUE_NORMAL,    //normal
    QUEUE_THRESHOLD_EXCEEDED //Threshold Exceeded
};

//Hoteling向服务器请求数据结构
struct hoteling_Request
{
    yl::string m_strUser;
    yl::string m_strPassword;
    HOTELING_STATUS m_eStatus;
    bool m_bIsAuto;

    hoteling_Request()
    {
        Reset();
    }

    void Reset()
    {
        m_strUser.clear();
        m_strPassword.clear();
        m_bIsAuto = false;
        m_eStatus = HOTELING_STATUS_NONE;

    }
};
//Call Information
struct CallCenterInfo : public DataLogic2UIBase
{
    yl::string m_strUserId; //CC userID
    yl::string m_strCallCenterName; //CC name
    int m_nCountInQueue; // number In queue
    int m_nWaitTime;  //Wait Time
    int m_nLongestTime;

    CallCenterInfo()
    {
        Reset();
    }

    void Reset()
    {
        m_nCountInQueue = -1;
        m_nLongestTime = -1;
        m_nWaitTime = -1;
        m_strCallCenterName = "";
        m_strUserId = "";
    }
};

//Queue Status Info
struct CallCenterQueueStatus
{
    yl::string m_strUserId; //CC userID
    yl::string m_strName;  //CC name
    QUEUE_STATUS m_eStatus; //queue status

    CallCenterQueueStatus()
    {
        Reset();
    }

    void Reset()
    {
        m_eStatus = QUEUE_NONE;
        m_strName = "";
        m_strUserId = "";
    }
};

//ACD及Hoteling登陆数据
struct ACDLogin : public LogicDataBase
{
    yl::string m_strUser; //hoteling 登陆用户名
    yl::string m_strPassword; //hoteling 登陆密码
    bool m_bIsAuto; //是否自动登陆
    bool m_bIsUseHost;//use host是否开启

    ACDLogin()
    {
        Reset();
    }

    void Reset()
    {
        m_strUser.clear();
        m_strPassword.clear();
        m_bIsAuto = false;
        m_bIsUseHost = false;
    }
};

//ACD Action
enum ACD_ACTION
{
    AA_LOGIN,
    AA_AVAIL,
    AA_UNAVAIL,
    AA_LOGOUT,
    AA_WRAPUP,
#if IF_FEATURE_START2START_ACD
    AA_REFRESH,
#endif
};

struct DispCode
{
    yl::string m_strCode;
    yl::string m_strCodeName;

    DispCode(const yl::string strCode, const yl::string strName) : m_strCode(strCode),
        m_strCodeName(strName)
    {}

    DispCode() : m_strCode(), m_strCodeName()
    {}
};

struct Supervisor
{
    yl::string m_strName;
    yl::string m_strNumber;
};

struct acd_accout
{
    char user[100];
    char password[100];
};

//ACD UnAvailable Reason List
typedef YLList<yl::string> ReasonList;
//typedef YLHashMap<yl::string,yl::string,STRING_HASH> ReasonMap;
typedef YLList<DispCode> DispCodeList;
typedef YLList<Supervisor> SupervisorList;

//typedef ReasonMap::iterator ReasonMapIter;

//////////////////////////////////////////////////////////////////////////
// metaswitch acd数据结构


// metaswitch acd类型
enum EMetaswitchACDServiceLevel
{
    EMTSWACDSL_None,
    EMTSWACDSL_MLHG,
    EMTSWACDSL_BasicACD,
    EMTSWACDSL_PremiumACD
};

// MLHG组数据
struct SMLHGData
{
    yl::string strName; // 名字
    bool bLoginLogoutSupport; //是否允许登录登出
    ACD_STATUS eStatus; // 登录状态还是登出状态
    EMetaswitchACDServiceLevel eACDType; // 当前acd类型

    SMLHGData()
    {
        strName.clear();
        bLoginLogoutSupport = true;
        eStatus = AS_IDLE;
        eACDType = EMTSWACDSL_None;
    }

    // 根据字符串转为具体类型
    bool SetACDType(yl::string strType)
    {
        if (strType == "None")
        {
            eACDType = EMTSWACDSL_None;
        }
        else if (strType == "MLHG")
        {
            eACDType = EMTSWACDSL_MLHG;
        }
        else if (strType == "Basic ACD")
        {
            eACDType = EMTSWACDSL_BasicACD;
        }
        else if (strType == "Premium ACD")
        {
            eACDType = EMTSWACDSL_PremiumACD;
        }
        else
        {
            eACDType = EMTSWACDSL_None;
        }

        return true;
    }

    bool IsLogin()
    {
        return eStatus != AS_IDLE;
    }
};

// MLHG组成员数据
struct SMLHGMemberData
{
    yl::string  strDirectoryNumber; //成员号码
    bool        bLoggedIn; // 成员在这个组中是否登录

    yl::string  strDepartment; // Department
    bool        bLoginLogoutSupported;
    yl::string  strName;
    yl::string  strSubscriberType;
};

// MLHG队列数据
struct SMLGHQueuesData
{
    yl::string  strAVGWaitingTime; // 队列平均等待时间
    int         nCountInQueue; // 队列中等待的人数
    int         nLogged;        // 已登录的成员数
    bool        bLoggedIn;      // 是否登录
};


//////////////////////////////////////////////////////////////////////////

#endif
