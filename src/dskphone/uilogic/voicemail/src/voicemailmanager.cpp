#include "voicemailmanager.h"
#include "account/include/modaccount.h"
#include "adapterbox/include/modadapterbox.h"
#include "voicemail.h"

#if IF_TEST_VOICEMAIL
#define VOICEMAIL_MSG_TEST ACCOUNT_MESSAGE_END - 1
#endif // IF_TEST_VOICEMAIL

LRESULT OnVoiceMailMessage(msgObject & msg)
{
    return _VoiceMailManager.OnMessage(msg);
}

CVoiceMailManager::CVoiceMailManager()
{
    m_iLastVoiceMailID = -1;
}

CVoiceMailManager::~CVoiceMailManager()
{
    Release();
}

//初始化
bool CVoiceMailManager::Init()
{
    VOICEMAIL_INFO("Voice Mail Init");

    etl_RegisterMsgHandle(SIP_MSG_UPDATE_MWI_INFO, SIP_MSG_UPDATE_MWI_INFO, OnVoiceMailMessage);
    etl_RegisterMsgHandle(ACCOUNT_VOICE_MAIL_STATE_CHANGED, ACCOUNT_VOICE_MAIL_STATE_CHANGED,
                          OnVoiceMailMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnVoiceMailMessage);

#if IF_TEST_VOICEMAIL
    etl_RegisterMsgHandle(VOICEMAIL_MSG_TEST, VOICEMAIL_MSG_TEST, OnVoiceMailMessage);
    VOICEMAIL_INFO("***********TEST VOICEMAIL[%d]********************", VOICEMAIL_MSG_TEST);
#endif // 1

    //初始化所有账号的VoiceMail
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        CVoiceMail * pVmail = new CVoiceMail(i);
        m_listVoiceMail.push_back(pVmail);
    }

    return true;
}

//释放
bool CVoiceMailManager::Release()
{
    etl_UnregisterMsgHandle(SIP_MSG_UPDATE_MWI_INFO, SIP_MSG_UPDATE_MWI_INFO, OnVoiceMailMessage);
    etl_UnregisterMsgHandle(ACCOUNT_VOICE_MAIL_STATE_CHANGED, ACCOUNT_VOICE_MAIL_STATE_CHANGED,
                            OnVoiceMailMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnVoiceMailMessage);

#if IF_TEST_VOICEMAIL
    etl_UnregisterMsgHandle(VOICEMAIL_MSG_TEST, VOICEMAIL_MSG_TEST, OnVoiceMailMessage);
#endif // 1

    for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
            itBeg != m_listVoiceMail.end();
            ++itBeg)
    {
        CVoiceMail * pVmail = (*itBeg);
        if (NULL != pVmail)
        {
            delete pVmail;
        }
    }

    m_listVoiceMail.clear();

    return true;
}

//消息处理
bool CVoiceMailManager::OnMessage(msgObject & msg)
{
    if (msg.message == SIP_MSG_UPDATE_MWI_INFO)
    {
        CVoiceMail * pVmail = GetVoiceMailAccountByID(msg.wParam);
        if (NULL == pVmail)
        {
            return false;
        }

        VoiceMailUserInfo mailUserInfo;

        mailUserInfo.iAccountID = msg.wParam;

        MapVoiceMailUserInfo(mailUserInfo, msg.GetExtraData());

        pVmail->OnMessage(mailUserInfo);
    }
    else if (msg.message == ACCOUNT_VOICE_MAIL_STATE_CHANGED)
    {
        VOICEMAIL_INFO("ACCOUNT_VOICE_MAIL_STATE_CHANGED[%d][%d]", msg.wParam, msg.lParam);
        //更新弹窗
        if (msg.lParam == 1)
        {
#ifdef _T42
            //账户不可用时,检测是否需要删除voiceMail图标
            bool bNoNewVoiceMail = true;
            for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
                    itBeg != m_listVoiceMail.end();
                    ++itBeg)
            {
                //帐号可用，且存在新语音留言
                CVoiceMail * pVmail = (*itBeg);
                if (NULL != pVmail
                        && acc_IsAccountAvailable(pVmail->GetAccountID()
                                                  && pVmail->GetUnReadVoiceMailCount() > 0))
                {
                    bNoNewVoiceMail = false;
                    break;
                }
            }
            if (bNoNewVoiceMail)
            {
                //通知VoiceMail数量改变
                etl_NotifyApp(false, ACCOUNT_VOICE_MAIL_COUNT_CHANGED, false, 0);
            }
#else
            CVoiceMail * pVmail = GetVoiceMailAccountByID(msg.wParam);
            if (NULL == pVmail)
            {
                return false;
            }

            pVmail->OnStateChange();
#endif
        }

        // 账号状态改变成不可用时需要重新设置 m_iLastVoiceMailID
        if (m_iLastVoiceMailID != msg.wParam)
        {
            return true;
        }

        LINE_STATE eState = acc_GetState(m_iLastVoiceMailID);
        if (eState == LS_REGISTER_FAIL
                || eState == LS_DISABLE
                || eState == LS_EXIT
                || eState == LS_BE_UNREGISTERED)
        {
            SetLastVMailAccountID(-1);
        }
    }
    else if (msg.message == CONFIG_MSG_BCAST_CHANGED)
    {
        if (ST_SIPLINE != msg.wParam)
        {
            return false;
        }

        CVoiceMail * pVmail = GetVoiceMailAccountByID(msg.lParam);
        if (NULL == pVmail)
        {
            return false;
        }

        return pVmail->OnConfigChange();
    }
#if IF_TEST_VOICEMAIL
    else if (msg.message == VOICEMAIL_MSG_TEST)
    {
        ProcessTestMsg(msg);
    }
#endif // IF_TEST_VOICEMAIL

    else
    {
        return false;
    }

    return true;
}

#if IF_TEST_VOICEMAIL
#include "../include/modvoicemail.h"
int _VoiceMailGetRandNum(bool bUserInput = false)
{
    if (bUserInput)
    {
        int iInput = 0;
        scanf("%d", &iInput);
        return iInput;
    }
    //播种
    srand((unsigned) time(NULL));
    return (rand() % 500);
}

bool CVoiceMailManager::ProcessTestMsg(msgObject & objMsg)
{
    if (objMsg.message != VOICEMAIL_MSG_TEST)
    {
        return false;
    }

    VOICEMAIL_INFO("Revieve Voice Mail TestMsg[%d] Wparam[%d] lParam[%d]", objMsg.message,
                   objMsg.wParam, objMsg.lParam);

    if (objMsg.wParam == 1) //测试消息数量
    {
        sip_mwi_info_t mwiMsg;
        VOICEMAIL_INFO("input new mail count");
        mwiMsg.new_msg = _VoiceMailGetRandNum(true);
        VOICEMAIL_INFO("input old mail count");
        mwiMsg.old_msg = _VoiceMailGetRandNum(true);

        etl_NotifyAppEx(FALSE, SIP_MSG_UPDATE_MWI_INFO, objMsg.lParam, 0, sizeof(sip_mwi_info_t), &mwiMsg);
    }
    else if (objMsg.wParam == 2) //测试账号变更
    {
        etl_NotifyApp(FALSE, ACCOUNT_VOICE_MAIL_STATE_CHANGED, objMsg.lParam, _VoiceMailGetRandNum() % 2);
    }
    else if (objMsg.wParam == 3) //清空通知
    {
        vmail_RemoveNotify();
    }
}
#endif

// 获取Voice Mail配置项列表.
bool CVoiceMailManager::GetVoiceMailConfigList(VoiceMailConfigListData & refConfigData)
{
    VoiceMailConfigData mailConfig;
    for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
            itBeg != m_listVoiceMail.end();
            ++ itBeg)
    {
        CVoiceMail * pVmail = (*itBeg);
        if (NULL != pVmail)
        {
            mailConfig.Reset();
            if (pVmail->GetVoiceMailConfig(mailConfig))
            {
                refConfigData.push_back(mailConfig);
            }
        }
    }

    return refConfigData.size() > 0;
}

// 设置Voice Mail配置项列表.
bool CVoiceMailManager::SetVoiceMailConfig(VoiceMailConfigListData & refConfigData)
{
    int i = 0;
    for (VoiceMailConfigListData::iterator itBeg = refConfigData.begin();
            itBeg != refConfigData.end();
            ++ itBeg, ++i)
    {
#if 1
        CVoiceMail * pVmail = GetVoiceMailAccountByID(i);
#else
        VoiceMailConfigData & mailConfigData = (*itBeg);
        CVoiceMail * pVmail = GetVoiceMailAccountByID(mailConfigData.m_iAccountID);
#endif // 1
        if (NULL != pVmail)
        {
            pVmail->SetVoiceMailConfig((*itBeg));
        }
    }

    return true;
}

//获取所有账号当前有的Voice Mail列表.
bool CVoiceMailManager::GetVoiceMailInfoList(VoiceMailInfoListData & refInfoData)
{
    VoiceMailInfoData mailInfoData;
    for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
            itBeg != m_listVoiceMail.end();
            ++itBeg)
    {
        CVoiceMail * pVmail = (*itBeg);
        mailInfoData.Reset();
        if (NULL != pVmail
                && pVmail->GetVoiceMailInfo(mailInfoData))
        {
            refInfoData.push_back(mailInfoData);
        }
    }

    return refInfoData.size() > 0;
}

//获取数量
int CVoiceMailManager::GetVoiceMailNumber(int iAccountID /* = -1 */, bool bReaded/* = false*/,
        bool bNotify/* = false*/)
{
    int iCount = 0;

    for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
            itBeg != m_listVoiceMail.end();
            ++itBeg)
    {
        CVoiceMail * pVmail = (*itBeg);
        if (NULL == pVmail)
        {
            continue;
        }

        if (iAccountID != -1
                && iAccountID == pVmail->GetAccountID())
        {
            if (!acc_IsAccountAvailable(iAccountID))
            {
                return 0;
            }

            if (bNotify)
            {
                return pVmail->GetNotifyVoiceMailNumber(bReaded);
            }
            else
            {
                return bReaded ? pVmail->GetReadedVoiceMailCount() : pVmail->GetUnReadVoiceMailCount();
            }
        }

        if (acc_IsAccountAvailable(pVmail->GetAccountID()))
        {
            if (bNotify)
            {
                iCount += pVmail->GetNotifyVoiceMailNumber(bReaded);
            }
            else
            {
                iCount += bReaded ? pVmail->GetReadedVoiceMailCount() : pVmail->GetUnReadVoiceMailCount();
            }
        }
    }

    return iCount;
}

//设置VoiceMail 数量
void CVoiceMailManager::SetVoiveMailNumber(int iAccountID /* = -1 */, bool bReaded /* = false */,
        bool bNotify /* = true */, int iCount/* = 0*/)
{
    for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
            itBeg != m_listVoiceMail.end();
            ++itBeg)
    {
        CVoiceMail * pVmail = (*itBeg);
        if (NULL == pVmail)
        {
            continue;
        }

        if (iAccountID == -1)
        {
            pVmail->SetNotifyVoiceMailNumber(iCount, bReaded);
            continue;
        }

        if (iAccountID == pVmail->GetAccountID())
        {
            if (bNotify)
            {
                pVmail->SetNotifyVoiceMailNumber(iCount, bReaded);
            }
            else
            {
                //TODO
            }
            break;
        }
    }
}

//获取最近更新账号ID
int CVoiceMailManager::GetLastVMailAccountID()
{
    return m_iLastVoiceMailID;
}

//设置最近更新账号
void CVoiceMailManager::SetLastVMailAccountID(int iAccountID)
{
    if (iAccountID < 0)
    {
        // 当m_iLastVoiceMailID对应的账号状态发生改变时，如果默认账号有语音留言，则设置为默认账号
        // 否则获取第一个有语音留言的账号

        m_iLastVoiceMailID = -1;

        for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
                itBeg != m_listVoiceMail.end();
                ++itBeg)
        {
            //帐号可用，且存在新语音留言
            CVoiceMail * pVmail = (*itBeg);
            if (NULL != pVmail
                    && acc_IsAccountAvailable(pVmail->GetAccountID())
                    && pVmail->GetUnReadVoiceMailCount() > 0)
            {
                m_iLastVoiceMailID = pVmail->GetAccountID();
                break;
            }
        }
    }
    else
    {
        m_iLastVoiceMailID = iAccountID;
    }

    VOICEMAIL_INFO("CVoiceMailManager::SetLastVMailAccountID(iAccountID = %d) m_iLastVoiceMailID = %d",
                   iAccountID, m_iLastVoiceMailID);
}

// 点击某个下标对应的Voice Mail.
void CVoiceMailManager::OnConnect(int iAccountID)
{
    CVoiceMail * pVmail = GetVoiceMailAccountByID(iAccountID);
    if (NULL != pVmail)
    {
        pVmail->OnCallout();
    }
}

// Message按键处理
bool CVoiceMailManager::OnMsgKeyPress()
{
    //建议由UI自己实现逻辑，涉及到界面跳转
#if 1
    VoiceMailInfoListData tInfoList;
    if (!GetVoiceMailInfoList(tInfoList))
    {
        return false;
    }

    YLList<VoiceMailInfoData>::const_iterator citer = tInfoList.begin();
    for (int nIndex = 0; citer != tInfoList.end(); ++citer, nIndex++)
    {
        const VoiceMailInfoData & refVMID = *citer;
        if (refVMID.m_nAccountStaus != LS_SYSTEM_DEFAULT_ACCOUNT)
        {
            continue;
        }

        //语音信箱号码呼出。
        AdapterBox_VoiceMailCallOut(nIndex);

        break;
    }  // for.
#endif // 0

    return true;
}

//获取指定VoiceMail 对象
CVoiceMail * CVoiceMailManager::GetVoiceMailAccountByID(int iAccountID)
{
    for (LIST_VOICEMAILACCOUNT::iterator itBeg = m_listVoiceMail.begin();
            itBeg != m_listVoiceMail.end();
            ++itBeg)
    {
        CVoiceMail * pVmail = (*itBeg);

        if (NULL != pVmail
                && iAccountID == pVmail->GetAccountID())
        {
            return pVmail;
        }
    }

    return NULL;
}

//数据转换
void CVoiceMailManager::MapVoiceMailUserInfo(VoiceMailUserInfo & mailUserInfo, void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    sip_mwi_info_t * pMWIInfo = (sip_mwi_info_t *) pData;

    mailUserInfo.iReadedCount = pMWIInfo->old_msg;
    mailUserInfo.iUnReadCount = pMWIInfo->new_msg;
}
