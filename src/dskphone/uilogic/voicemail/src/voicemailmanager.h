#ifndef __VOICEMAIL_MANAGER_H__
#define __VOICEMAIL_MANAGER_H__
#include "voicemail/include/voicemaildefines.h"
#include <ETLLib.hpp>
#include "interfacedefine.h"

class CVoiceMail;

class CVoiceMailManager
{
public:
    static CVoiceMailManager & GetInstance()
    {
        static CVoiceMailManager sInc;
        return sInc;
    }

    typedef YLList<CVoiceMail *> LIST_VOICEMAILACCOUNT;

    //消息处理
    bool OnMessage(msgObject & msg);

    //初始化
    bool Init();

    //获取最近更新账号ID
    int GetLastVMailAccountID();

    //设置最近更新账号
    void SetLastVMailAccountID(int iAccountID);

    // 获取Voice Mail配置项列表.
    bool GetVoiceMailConfigList(VoiceMailConfigListData & refConfigData);

    // 设置Voice Mail配置项列表.
    bool SetVoiceMailConfig(VoiceMailConfigListData & refConfigData);

    //获取所有账号当前有的Voice Mail列表.
    bool GetVoiceMailInfoList(VoiceMailInfoListData & refInfoData);

    // 点击某个下标对应的Voice Mail.
    void OnConnect(int iAccountID);

    // Message按键处理
    bool OnMsgKeyPress();

    //获取数量
    /** 参数：
        [in] iAccountID: 账号ID
        [in] bReaded: 是否为已读的数量
        [in] bNotify：是否为通知的消息数量
    */
    int GetVoiceMailNumber(int iAccountID = -1, bool bReaded = false, bool bNotify = false);

    //设置数量
    /** 参数：
        [in] iAccountID: 账号ID
        [in] bReaded: 是否为已读的数量
        [in] bNotify：是否为通知的消息数量
        [in] iCount: 数量
    */
    void SetVoiveMailNumber(int iAccountID = -1, bool bReaded = false, bool bNotify = true,
                            int iCount = 0);

    //获取指定VoiceMail 对象
    CVoiceMail * GetVoiceMailAccountByID(int iAccountID);

protected:
    //数据转换
    void MapVoiceMailUserInfo(VoiceMailUserInfo & mailUserInfo, void * pData);

    //释放
    bool Release();

#if IF_TEST_VOICEMAIL
    bool ProcessTestMsg(msgObject & objMsg);
#endif

private:
    CVoiceMailManager();
    ~CVoiceMailManager();

    //账号列表
    LIST_VOICEMAILACCOUNT m_listVoiceMail;

    //最近更新账号ID
    int m_iLastVoiceMailID;
};

#define _VoiceMailManager CVoiceMailManager::GetInstance()
#endif //__VOICEMAIL_MANAGER_H__
