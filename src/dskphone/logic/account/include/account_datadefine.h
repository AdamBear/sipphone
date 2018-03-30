#ifndef __ACCOUNT_DATADEFINE_H__
#define __ACCOUNT_DATADEFINE_H__

#include "account_inc.h"

#include "accountmessagedefine.h"
#include <pstnadapter/include/pstnadapterdatadefine.h>
#include "xmlparser.hpp"


#define DEBUG_ACCOUNT 0

// 目前备份服务器是两个
#define SIPSERVER_NUM       2


#define INVALID_ACCOUNTID       -1  // 无效账号ID

#define SIP_LINE_SIZE               100                                         // SIP帐号段最大个数
#define PSTN_LINE_SIZE              50                                          // PSTN帐号段最大个数
#define PSTN_LINE_CURRENT_MAX_SIZE  MAX_PSTN_LINE_COUNT                         // PSTN账号当前最大个数

enum ACCOUNT_ID_TYPE
{
    SIP_LINE_BEGIN_ID = 0,                                          // SIP帐号段本地起始ID
    SIP_LINE_END_ID = (SIP_LINE_BEGIN_ID + SIP_LINE_SIZE - 1),      // SIP账号段本地结束ID
    PSTN_LINE_BEGIN_ID = (SIP_LINE_END_ID + 1),                     // PSTN帐号段起始ID
    PSTN_LINE_END_ID = (PSTN_LINE_BEGIN_ID + PSTN_LINE_SIZE - 1),   // PSTN账号段结束ID
    H323ACCOUNTID = 150,
    H323ACCOUNTID_IP = 151,
};

// 账号类型
enum ACCOUNT_TYPE
{
    ACCOUNT_TYPE_NONE = 0,              // 空类型
    ACCOUNT_TYPE_SIP = 1 << 0,          // SIP账号
    ACCOUNT_TYPE_IP_DIRECT = 1 << 1,    // IP直拨账号
    ACCOUNT_TYPE_PSTN = 1 << 2,         // PSTN账号
    ACCOUNT_TYPE_H323 = 1 << 3,         // h323账号
};
#define ACCOUNT_TYPE_ALL 0xFFFF // 所有账号

#define ACCOUNTID_OF_IPDIRECT_CALL 16

enum DTMF_TYPE
{
    DTMF_INBAND = 0,
    DTMF_RFC2833,
    DTMF_SIPINFO,
    DTMF_AUTO_SIPINFO,
};

// PICK类型
enum PICKUP_TYPE
{
    PT_GROUP,
    PT_DIRECTED,
};

enum SIP_SERVER_TYPE
{
    SST_NONE = -1,
    SST_DEFAULT = 0,
    SST_BROADSOFT = 2,
    SST_COSCOCOM = 4,
    SST_UCAP = 6,
    SST_GENESYS = 8,
    SST_GENESYS_ADV = 10,
#if IF_FEATURE_START2START_ACD
    SST_START2START = 12,
#endif
#if IF_FEATURE_GCS_21300
    SST_COMVERSE = 14,
#endif
    SST_GENBAND = 15,
};

//通话类型定义
enum CALL_AV_TYPE
{
    CAVT_UNKNOWN = -1,
    CAVT_VOICE   = 0,
    CAVT_VIDEO,
    CAVT_MIX,
    CAVT_PSTN
};

enum DTM_DIAL_TONE
{
    CN_STANDARD_DIAL_TONE = 0,
    CN_SPECIAL_CONDITION_TONE = 1,
    CN_MESSAGE_WAITING_TONE = 2,
};

enum LINE_STATE
{
    LS_UNKNOW = -1,
    LS_DISABLE,                      // 0
    LS_REGISTERING,                  // 1
    LS_REGISTERED,                   // 2
    LS_REGISTER_FAIL,                // 3
    LS_BE_UNREGISTERED,              // 4
    LS_SYSTEM_DEFAULT_ACCOUNT,       // 5
    LS_INEXISTENCE,                  // 6
    LS_RE_REGISTERING,               // 7 正在进行重注册, 重注册之前先进行了注销
    LS_UNREGISTERING,                // 8 正在注销该账户
    LS_EXIT,                         // 9 EXIT
    LS_USING,
    LS_DISABLE_REG,                  // 11 账号不需要注册，有单独开关控制
};

struct account_register_reason_t
{
    account_register_reason_t()
    {
        clear();
    }

    int m_iCode;
    yl::string m_strReason;

    void clear()
    {
        m_iCode = 0;
        m_strReason.clear();
    }
};

struct AccountInfo
{
    int iAccountID;
    yl::string strUserName;
    yl::string strServerName;
};

enum CloudAccountRegFailCode
{
    REG_SUCC = 0,
    REG_NETWORK_UNAVAILABLE,
    REG_PHONE_NOT_FOUND, //用户名不存在
    REG_PIN_NOT_FOUND,//pin不存在
    REG_UNAUTHORIZED, //SIP 401 密码错误
    REG_FORBIDDEN,    //SIP 403
    REG_NOT_FOUND,      // SIP 404
    REG_SERVER_ERROR,   // SIP 500
    REG_TIMEOUT,
    REG_OTHER_ERROR,
};

//Account Type
enum SipAccountType
{
    ACCOUNT_NONE = -1,
    ACCOUNT_COMMON = 0,         // 普通sip账号
    ACCOUNT_YEALINK = 1,        // yealink云账号
    ACCOUNT_CUSTOM = 2,         // 自定义
    ACCOUNT_ZOOM = 3,           // zoom云账号
    ACCOUNT_PEXIP = 4,          // pexip
    ACCOUNT_MIND = 5,           // mind
    ACCOUNT_BLUEJEANS = 6,      // bj

    ACCOUNT_TYPE_END = 7,       // 限定账号定义范围
};

//TransPort
enum TransPort
{
    TRANSPORT_UDP,
    TRANSPORT_TCP,
    TRANSPORT_TLS,
};

//Account Type
enum AccountProtocolType
{
    PROTOCOL_NONE = -1,         // 无协议
    PROTOCOL_SIP = 0,           // sip协议
    PROTOCOL_H323 = 1,          // h323协议
    PROTOCOL_PSTN = 2,          // pstn协议
};

enum CloudAccountLoginStatus
{
    STATUS_LOGIN_REQUEST = -1,
    STATUS_LOGIN_NONE = 0,
    STATUS_LOGINING,
    STATUS_CANCEL_LOGINING,
    STATUS_LOGIN_FAIL,
    STATUS_LOGIN_SUCC,
};

enum OrderType
{
    LEXICOGRAPHIC_ORDER,
    POSITIVE_ORDER,
    REVERSE_ORDER,
};

#define kszAttrUserName     "username"
#define kszAttrPassWord     "password"
#define kszAttrSavePassWord "savepassword"
#define kszAttrType         "type"
#define kszAttrState        "state"
#define kszAttrAccount      "account"
#define kszAttrRegistername "registername"
#define kszAttrServer       "server"
#define kszAttrLabel        "label"
#define kszAttrTransport    "transport"
#define kszAttrDtmfType     "dtmf_type"
#define kszAttrDtmfInfoType "dtmf_info_type"
#define kszAttrDtmfPayload  "dtmfpayload"
#define kszAttrKeeplive     "keepalive"
#define kszAttrSrtp         "srtp_encryption"
#define kszAttrPort         "port"
#define kszAttrBfcp         "bfcp"
#define kszAttrFecc         "fecc"
#define kszAttrExtvaddonce      "extvaddonce"
#define kszAttrExpires          "expires"

#define kszAttrBorntick     "born_tick"

struct CloudAccountInfo
{
    int m_nAccountId;
    SipAccountType m_eType;
    CloudAccountLoginStatus m_eStatus;
    yl::string m_strPinCode;
    yl::string m_strLabel;
    yl::string m_strUserName;
    yl::string m_strRegName;
    yl::string m_strServer;
    yl::string m_strPassWord;
    bool m_bSavePass;
    bool m_bSaveMsgToFile;

    int m_nTransport;       // 传输方式
    int m_nSrtp;
    int m_nDtmfType;
    int m_nDtmfInfoType;
    int m_nDtmfPayload;
    int m_nKeepAlive;
    int m_nPort;            // 端口
    int m_nBfcp;            // 辅流
    int m_nFecc;            // 远端摄像头控制
    int m_nExtvaddonce;     // pexip平台下，特定处理
    int m_nExpires;         // 超时时间

    CloudAccountInfo()
    {
        Clear();
    }

    void Clear()
    {
        m_nAccountId = -1;
        m_eType = ACCOUNT_COMMON;
        m_eStatus = STATUS_LOGIN_NONE;
        m_strPinCode.clear();
        m_strLabel.clear();
        m_strUserName.clear(); // Pexip对应alias别名
        m_strRegName.clear();   // Pexip对应UserName
        m_strServer.clear();
        m_strPassWord.clear();
        m_bSavePass = false;
        m_bSaveMsgToFile = true;

        m_nTransport = -1;      // 注意：-1在请求文件中表示不设置，使用默认值代替
        m_nSrtp = -1;
        m_nDtmfType = -1;
        m_nDtmfInfoType = -1;
        m_nDtmfPayload = -1;
        m_nKeepAlive = -1;

        m_nPort = -1;
        m_nBfcp = -1;
        m_nFecc = -1;
        m_nExtvaddonce = -1;
        m_nExpires = -1;
    }

    const char * GetNameByType(SipAccountType eType, bool bLabel);

    bool ParseFromXMLNode(const xml_node & node);

    bool ToXMLNode(xml_node & xmlNode);

    void Print();
};

typedef YLVector<CloudAccountInfo> VectorInfo;

enum CLOUD_OPERATION_TYPE
{
    CLOUD_NONE,
    CLOUD_GET_CONTACTLIST,
    CLOUD_GET_MD5,
    CLOUD_GET_EMAIL,
    CLOUD_GET_PIN_INFO,
    CLOUD_REMOVE_PIN_CODE,
};

#endif // __ACCOUNT_DATADEFINE_H__
