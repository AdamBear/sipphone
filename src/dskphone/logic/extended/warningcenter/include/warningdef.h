#ifndef __WARNING_DEF_H__
#define __WARNING_DEF_H__

#define DEFAULT_USER_ADMIN      "admin"
#define DEFAULT_USER_USER       "user"
#define DEFAULT_USER_VAR        "var"

enum WarningType
{
    WT_NONE = 0,
    WT_NETWORK_FAILED,           // 网络不可用
    WT_DEFAULT_USERPSWD_USING,   // 话机用户默认密码使用中
    WT_AUTOP_AUTH_FAILED,        // autop鉴权失败
    WT_ACCOUNT_REG_FAILED        // 账号注册失败
};

#endif // __WARNING_DEF_H__
