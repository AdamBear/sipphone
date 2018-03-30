#include "voicemail.h"
#include "voicemailmanager.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "configparser/configiddefine.h"
#include "configparser/modconfigparser.h"
#include "account/include/accountmessagedefine.h"
#include "../include/modvoicemail.h"

CVoiceMail::CVoiceMail(int iAccount)
    : m_iAccountID(iAccount)
    , m_iReadedVoiceMailCount(0)
    , m_iUnReadVoiceMailCount(0)
    , m_iNotifyUnReadVoiceMailCount(0)
    , m_iNotifyReadedVoiceMailCount(0)
{
    m_bEnable = configParser_GetCfgItemIntValue(kszAccountVMIndication, m_iAccountID) == 1;
}

CVoiceMail::~CVoiceMail()
{

}

bool CVoiceMail::OnMessage(const VoiceMailUserInfo & mailUserInfo)
{
    if (m_iAccountID != mailUserInfo.iAccountID)
    {
        return false;
    }

    if (!acc_IsAccountAvailable(m_iAccountID)
            || acc_IsDirectIPAccount(m_iAccountID)
            || !IsVoiceMailEnable())
    {
        return false;
    }

    bool bNewVoiceMail = (mailUserInfo.iUnReadCount > m_iUnReadVoiceMailCount);

    VOICEMAIL_INFO("Voice Mail[%d] new[%d] old[%d], bNewVoiceMail[%d]", m_iAccountID,
                   mailUserInfo.iUnReadCount, mailUserInfo.iReadedCount, bNewVoiceMail);

    ////这里需要移植V80中性过来，添加配置项
    ////bool bAlertVoiceMail = configParser_GetConfigInt(kszAlertVoiceMail) != 0;
    //bool bAlertVoiceMail = true;

    if ((m_iReadedVoiceMailCount == mailUserInfo.iReadedCount
            && m_iUnReadVoiceMailCount == mailUserInfo.iUnReadCount))
    {
        VOICEMAIL_INFO("Voice mail count no change");
        return true;
    }

    m_iUnReadVoiceMailCount = mailUserInfo.iUnReadCount;
    m_iReadedVoiceMailCount = mailUserInfo.iReadedCount;
    m_iNotifyUnReadVoiceMailCount = m_iUnReadVoiceMailCount;
    m_iNotifyReadedVoiceMailCount = m_iReadedVoiceMailCount;

    _VoiceMailManager.SetLastVMailAccountID((m_iUnReadVoiceMailCount > 0) ? m_iAccountID : -1);

    //通知VoiceMail 数量改变
    etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, bNewVoiceMail, 1);

    return true;
}

//账号VoiceMail 是否启用
bool CVoiceMail::IsVoiceMailEnable()
{
    return m_bEnable;
}

//获取已读信息数
int CVoiceMail::GetReadedVoiceMailCount()
{
    return m_iReadedVoiceMailCount;
}

//获取未读信息数
int CVoiceMail::GetUnReadVoiceMailCount()
{
    return m_iUnReadVoiceMailCount;
}

//获取通知数量
int CVoiceMail::GetNotifyVoiceMailNumber(bool bReaded/* = false*/)
{
    return bReaded ? m_iNotifyReadedVoiceMailCount : m_iNotifyUnReadVoiceMailCount;
}

//设置通知消息数量
void CVoiceMail::SetNotifyVoiceMailNumber(int iCount /* = 0 */, bool bReaded/* = false*/)
{
    if (bReaded)
    {
        m_iNotifyReadedVoiceMailCount = iCount;
    }
    else
    {
        m_iNotifyUnReadVoiceMailCount = iCount;
    }
}

//获取ID
int CVoiceMail::GetAccountID() const
{
    return m_iAccountID;
}

//获取配置
bool CVoiceMail::GetVoiceMailConfig(VoiceMailConfigData & mailConfig)
{
    mailConfig.m_strMailNumber = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber,
                                 m_iAccountID);
    mailConfig.m_iAccountID = m_iAccountID;

    return true;
}

//设置配置
bool CVoiceMail::SetVoiceMailConfig(VoiceMailConfigData & mailConfig)
{
    configParser_SetCfgItemStringValue(kszAccountVoiceMailNumber, m_iAccountID,
                                       mailConfig.m_strMailNumber.c_str(), CONFIG_LEVEL_PHONE);
    return true;
}

//获取信息
bool CVoiceMail::GetVoiceMailInfo(VoiceMailInfoData & mailInfo)
{
    if (!acc_IsAccountIDValid(m_iAccountID))
    {
        return false;
    }

    mailInfo.m_iAccountID = m_iAccountID;
    mailInfo.m_strMailNumber = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber,
                               m_iAccountID);
    mailInfo.m_nAccountStaus = (int) acc_GetState(m_iAccountID);
    yl::string strName = acc_GetDisplayName(m_iAccountID);
    if (strName.empty())
    {
        strName = acc_GetUsername(m_iAccountID);
    }

    mailInfo.m_strAccountDisplayName = strName;
    mailInfo.m_nReadCount = m_iReadedVoiceMailCount;
    mailInfo.m_nUnReadCount = m_iUnReadVoiceMailCount;
    mailInfo.m_nNotifyUnreadCount = m_iNotifyUnReadVoiceMailCount;
    mailInfo.m_nNotifyReadedCount = m_iNotifyReadedVoiceMailCount;
    mailInfo.m_bDisplayVoiceMail = IsVoiceMailEnable();
    return true;
}

//重置数量
void CVoiceMail::ResetVoiceMailCount()
{
    m_iNotifyReadedVoiceMailCount = 0;
    m_iNotifyUnReadVoiceMailCount = 0;
    m_iReadedVoiceMailCount = 0;
    m_iUnReadVoiceMailCount = 0;
}

//获取号码
bool CVoiceMail::GetVoiceMailNumber(yl::string & strNumber)
{
    strNumber = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber, m_iAccountID);
    return true;
}

//点击事件
void CVoiceMail::OnCallout()
{
    // 只处理账号注册成功则情况.
    LINE_STATE eStatus = acc_GetState(m_iAccountID);
    if (eStatus == LS_REGISTERED
            || eStatus == LS_SYSTEM_DEFAULT_ACCOUNT)
    {
        yl::string strNum;
        GetVoiceMailNumber(strNum);
        //号码为空要进入设置界面
        if (!strNum.empty())
        {
            talklogic_CallOut(strNum, "", m_iAccountID);
        }
    }
}

//状态改变
bool CVoiceMail::OnStateChange()
{
    //账号变成不可用的时候才通知, 变成可用的时候sip会发消息弹窗,中间状态不发消息
    LINE_STATE eState = acc_GetState(m_iAccountID);
    if (eState == LS_REGISTER_FAIL
            || eState == LS_DISABLE
            || eState == LS_EXIT
            || eState == LS_BE_UNREGISTERED)
    {
        //对于本来就没有未读消息的账号，不需要再通知。
        if (GetUnReadVoiceMailCount() == 0
                && GetNotifyVoiceMailNumber(false) == 0)
        {
            return false;
        }

        ResetVoiceMailCount();
        //不再提示弹窗,只更新显示
        etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, false, 0);
        return true;
    }

    return false;
}

//配置改变
bool CVoiceMail::OnConfigChange()
{
    bool bEnable = configParser_GetCfgItemIntValue(kszAccountVMIndication, m_iAccountID) == 1;
    if (bEnable == m_bEnable)
    {
        return false;
    }

    m_bEnable = bEnable;

    //显示状态改变后需要清除掉一些状态
    if (!IsVoiceMailEnable())
    {
        vmail_RemoveNotify(m_iAccountID);
    }
    else
    {
        SetNotifyVoiceMailNumber(m_iUnReadVoiceMailCount, false);
        SetNotifyVoiceMailNumber(m_iReadedVoiceMailCount, true);
    }

    return true;
}

