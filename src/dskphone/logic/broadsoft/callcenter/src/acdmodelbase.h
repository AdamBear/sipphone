#ifndef __ACD_MODEL_BASE_H__
#define __ACD_MODEL_BASE_H__

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD) || IF_FEATURE_METASWITCH_ACD

#include <ylstring.h>
#include "cccommon.h"
#include <ETLLib.hpp>
#include "interfacedefine.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"
#include "talk/talklogic/include/uiandlogic_common.h"

class CACDModelBase : public CMKProcessor
{
public:
    CACDModelBase(void);
    ~CACDModelBase(void);

    // 消息处理
    virtual LRESULT OnMessage(msgObject & msg);

    // 登录ACD
    virtual bool ReqLogin(acd_accout * pAcd = NULL)
    {
        return false;
    }
    //请求状态改变，包括登陆，登陆时需考虑考虑初始化状态
    virtual bool ReqChangeACDStatus(ACD_ACTION eACDAction)
    {
        return false;
    }

    virtual bool ProcessAfterLogin()
    {
        return false;
    }

    // 获取ACD类型
    ACD_TYPE Get_ACDType()
    {
        return m_eACDType;
    }
    // 设置ACD类型
    void Set_ACDType(ACD_TYPE eType)
    {
        m_eACDType = eType;
    }

    // 获取ACD绑定账号ID
    int GetAccountId()
    {
        return m_nAccountId;
    }
    // 设置绑定账号ID
    virtual void SetAccountId(int iAccountId)
    {
        m_nAccountId = iAccountId;
    }

    // 获取ACD当前状态
    virtual ACD_STATUS GetAcdStatus()
    {
        return m_eACDLineState;
    }
    // 设置ACD当前状态
    virtual void SetStatus(ACD_STATUS eStatus);

    virtual bool IsUnavailReasonCodeOn()
    {
        return false;
    }

    yl::string  GetLoginName()
    {
        return m_strUserName;
    }

    bool GetReasonCode(DispCodeList & listData);

#if IF_FEATURE_START2START_ACD
    void SetSessionID(const yl::string & strSessionid)
    {
        m_strSessionid = strSessionid;
    }
    const yl::string & GetSessionID()
    {
        return m_strSessionid;
    }
#endif

#if IF_FEATURE_METASWITCH_ACD
    virtual void GetCurReason(yl::string & strReasonName);
    //设置Unavailable Reason
    virtual void SetCurReason(const yl::string & strReason);
#endif

    bool CallOut(const yl::string & strNumber, const yl::string & strName,
                 int iAccountId = AUTO_SELECT_LINE_ID, CallOut_Param * pCallParam = NULL);

protected:
    bool ProcessAutoAvailble(ACD_STATUS eStatus);

    // 对定时器的处理
    BOOL OnAutoAvailTimer(msgObject & refObj);

protected:
    yl::string m_strUserName;       // 登陆使用的用户名
    yl::string m_strPassword;       // 登陆使用的密码

    ACD_TYPE    m_eACDType;         //该ACDModel的类型,根据服务器类型划分
    int         m_nAccountId;       //该ACD功能绑定账号ID
    ACD_STATUS  m_eACDLineState;    //ACD功能当前状态

    mkThreadTimer m_objTimer;  // 计时器.

#if IF_FEATURE_START2START_ACD
    yl::string      m_strSessionid;     //提示对应session id
#endif
};

#endif

#endif  // __ACD_MODEL_BASE_H__
