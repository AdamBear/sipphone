#ifndef __VOICEMAIL_ACCOUNT_H__
#define __VOICEMAIL_ACCOUNT_H__
#include "../include/voicemaildefines.h"

class CVoiceMail
{
public:
    explicit CVoiceMail(int iAccountID);
    ~CVoiceMail();

    bool OnMessage(const VoiceMailUserInfo & mailUserInfo);

    //账号VoiceMail 是否启用
    bool IsVoiceMailEnable();

    //获取ID
    int GetAccountID() const;

    //获取未读信息数
    int  GetUnReadVoiceMailCount();

    //获取已读信息数
    int  GetReadedVoiceMailCount();

    //获取配置
    bool GetVoiceMailConfig(VoiceMailConfigData & mailConfig);

    //设置配置
    bool SetVoiceMailConfig(VoiceMailConfigData & mailConfig);

    //获取信息
    bool GetVoiceMailInfo(VoiceMailInfoData & mailInfo);

    //获取号码
    bool GetVoiceMailNumber(yl::string & strNumber);

    //点击事件
    void OnCallout();

    //状态改变
    bool OnStateChange();

    //配置改变
    bool OnConfigChange();

    //获取通知的消息数量
    int GetNotifyVoiceMailNumber(bool bReaded = false);

    //设置通知的消息数量
    void SetNotifyVoiceMailNumber(int iCount = 0, bool bReaded = false);

protected:

    void ResetVoiceMailCount();

private:
    const int     m_iAccountID;                    //账号ID
    int           m_iReadedVoiceMailCount;         //已读语音信息数
    int           m_iUnReadVoiceMailCount;         //未读语音信息数
    int           m_iNotifyUnReadVoiceMailCount;   //通知的未读消息数量
    int           m_iNotifyReadedVoiceMailCount;   //通知的已读消息数量
    bool          m_bEnable;                       //VoiceMail 状态开关
};

#endif //__VOICEMAIL_ACCOUNT_H__
