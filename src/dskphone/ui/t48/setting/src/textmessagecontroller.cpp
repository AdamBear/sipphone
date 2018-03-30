#include "textmessagecontroller.h"
#include "account/include/modaccount.h"
// #include "commonunits/statusmanager.h"
#include "setting/include/common.h"
#include "devicelib/networkserver.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "lamp/backlight/include/modbacklight.h"
#include "textmessagemodel.h"
#include "xmlparser/xmlparser.hpp"
#include "voip/modvoip.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CTextMessageController.
CTextMessageController * CTextMessageController::m_pInst = NULL;

// Click action definition.
// In box click action.
static const char kszCAInBox[] = "CDlgMessageFrame_inbox";
// Send box click action.
static const char kszCASentBox[] = "CDlgMessageFrame_sentbox";
// Out box click action.
static const char kszCAOutBox[] = "CDlgMessageFrame_outbox";
// Draft box click action.
static const char kszCADraftBox[] = "CDlgMessageFrame_draftbox";

// Static.
CTextMessageController * CTextMessageController::GetInstance()
{
    // Create the singleton if not existed.
    if (m_pInst == NULL)
    {
        m_pInst = new CTextMessageController();
        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }

    return m_pInst;
}

// Static.
void CTextMessageController::ReleaseInstance()
{
    // Release the singleton if existed.
    if (m_pInst != NULL)
    {
        m_pInst->MyFinal();

        delete m_pInst;
        m_pInst = NULL;
    }
}

// Static.
LRESULT CTextMessageController::OnMessage(msgObject & refObj)
{
    if (TM_TIMER != refObj.message)
    {
        SETTING_INFO("CTextMessageController: Receive message[%d] wParam[%d] lParam[%d]"
                     , refObj.message, refObj.wParam, refObj.lParam);
        SETTING_INFO("SIP_MSG_UPDATE_SMS : %d\nSIP_MSG_SEND_SMS_RESULT : %d\nTM_TIMER : %d\nCONFIG_MSG_BCAST_CHANGED : %d"
                     , SIP_MSG_UPDATE_SMS, SIP_MSG_SEND_SMS_RESULT, TM_TIMER, CONFIG_MSG_BCAST_CHANGED);
    }

    LRESULT retVal = FALSE;
    switch (refObj.message)
    {
    case SIP_MSG_UPDATE_SMS:
        // Add a recieved message.
        if (m_pInst != NULL && m_pInst->m_pModel->IsSMSEnable())
        {
            backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            //retVal = m_pInst->OnReceivedMessage(refObj);
            retVal = m_pInst->m_pModel->ProcessIntanceMessage(refObj);
        }
        break;

    case SIP_MSG_SEND_SMS_RESULT:
        // 处理消息发送后返回的结果.
        if (m_pInst != NULL)
        {
            retVal = m_pInst->OnMessageSendResult(refObj);
        }
        break;


    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (refObj.wParam != ST_SMS
                    || m_pInst == NULL
                    || m_pInst->m_pModel == NULL)
            {
                return 0;
            }

            SETTING_INFO("TextMessage: Receive [CONFIG_MSG_BCAST_CHANGED] wParam[ST_SMS] lParam[%d]",
                         refObj.lParam);
            if (0 == refObj.lParam)
            {
                // 立即保存更改到文件中
                retVal = m_pInst->m_pModel->SaveConfigFile();
            }
            else if (1 == refObj.lParam)
            {
                // 重新下载短信文件
                retVal = m_pInst->m_pModel->ReloadConfigFile(false);
            }
            else if (2 == refObj.lParam)
            {
                retVal = TRUE;
                if (!m_pInst->m_pModel->IsSMSEnable())
                {
                    m_pInst->m_pModel->RemoveAllMessage();
                    m_pInst->m_pModel->SaveConfigFile();
                }
            }
        }
        break;

    default:
        break;
    }

    return retVal;
}

// Static.
LRESULT CTextMessageController::OnTimeMessage(msgObject & refObj)
{
    LRESULT retVal = FALSE;

    if (TM_TIMER == refObj.message && m_pInst != NULL)
    {
        retVal = m_pInst->OnMessageTimer(refObj);
    }
    return retVal;
}

bool CTextMessageController::GetAccountMsgList(
    AccountTextMsgListData * pData) const
{
    // Get account message info from model.
    return m_pModel != NULL ? m_pModel->GetAccountMsgList(pData) : false;
}

int CTextMessageController::GetFirstUnReadInMessageIndex() const
{
    // Get first un-read in message.
    return m_pModel != NULL ? m_pModel->GetFirstUnReadInMessageIndex() : -1;
}

bool CTextMessageController::GetMessageList(TextMsgSimpListData * pData,
        const yl::string & strClickAction) const
{
    // Get message list from model.
    int nGType = MyClickAction2GroupType(strClickAction);
    return m_pModel != NULL ? m_pModel->GetMessageList(pData, nGType) : false;
}

bool CTextMessageController::GetMessageDetail(TextMsgItemDetailData * pData,
        const yl::string & strClickAction, int nIndex) const
{
    // Get the message detail from model.
    int nGType = MyClickAction2GroupType(strClickAction);
    return m_pModel != NULL
           ? m_pModel->GetMessageDetail(pData, nGType, nIndex) : false;
}

int CTextMessageController::AddDraftMessage(const TextMsgSendItemData & refData)
{
    // Add a draft message to the model.
    return m_pModel != NULL
           ? m_pModel->AddMessage(refData, TEXT_MSG_GROUP_DRAFT,
                                  CTextMessageModel::STATUS_UNREAD) : false;
}

bool CTextMessageController::SendTheMessage(const TextMsgSendItemData & refData)
{
    if (m_pModel == NULL)
    {
        return false;
    }

    // First add a message to the sending group.
    bool isSucceed = false;
    int nIndex = m_pModel->AddMessage(refData, TEXT_MSG_GROUP_SENDING,
                                      CTextMessageModel::STATUS_UNREAD);
    if (nIndex >= 0)
    {
        // Use selected account to send text message.
        if (acc_IsAccountIDValid(refData.m_nFromAccountId))
        {
            // 规范化接收方.
            yl::string strTo;
            if (refData.m_strTo.substr(0, 4) != "sip:")
            {
                strTo.append("sip:");
            }

            size_t uFoundPos = refData.m_strTo.find("@");
            if (uFoundPos != yl::string::npos)
            {
                strTo.append(refData.m_strTo);

                if (refData.m_strTo.find(":") == yl::string::npos)
                {
                    strTo = commonAPI_FormatString("%s:%d", strTo.c_str(), acc_GetServerPort(refData.m_nFromAccountId));
                }
            }
            else
            {
                strTo.append(refData.m_strTo);
                strTo.append("@");
                strTo.append(acc_GetServerName(refData.m_nFromAccountId));
                strTo = commonAPI_FormatString("%s:%d", strTo.c_str(), acc_GetServerPort(refData.m_nFromAccountId));
            }

            modVoip_SendTextMessage(refData.m_nFromAccountId, refData.m_strText.c_str(), strTo.c_str());

        }
    }

    return isSucceed;
}

bool CTextMessageController::RemoveMessage(const yl::string & strClickAction,
        int nIndex)
{
    // Remove the specified index message from model.
    int nGType = MyClickAction2GroupType(strClickAction);
    return m_pModel != NULL ? m_pModel->RemoveMessage(nGType, nIndex) : false;
}

bool CTextMessageController::RemoveMessageList(const yl::string & strClickAction,
        const std::list<int> & listIndex)
{
    // 删除一组信息.
    int nGType = MyClickAction2GroupType(strClickAction);
    return m_pModel != NULL ? m_pModel->RemoveMessageList(nGType, listIndex) : false;
}

bool CTextMessageController::RemoveAllMessage(const yl::string & strClickAction)
{
    // Remove all messages own to some group.
    int nGType = MyClickAction2GroupType(strClickAction);
    return m_pModel != NULL ? m_pModel->RemoveAllMessage(nGType) : false;
}

int CTextMessageController::GetTotalUnRead() const
{
    return m_pModel != NULL ? m_pModel->GetTotalUnRead() : 0;
}

int CTextMessageController::GetAccountUnRead(int nAccountId) const
{
    // 获取某个账号对应的未读信息.
    return m_pModel != NULL ? m_pModel->GetAccountUnRead(nAccountId) : 0;
}

CTextMessageController::CTextMessageController()
    : m_pModel(NULL)
{

}

CTextMessageController::~CTextMessageController()
{

}

bool CTextMessageController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CTextMessageModel();

        // Register message.
        etl_RegisterMsgHandle(SIP_MSG_UPDATE_SMS,
                              SIP_MSG_UPDATE_SMS,
                              OnMessage);

        etl_RegisterMsgHandle(SIP_MSG_SEND_SMS_RESULT,
                              SIP_MSG_SEND_SMS_RESULT,
                              OnMessage);

        etl_RegisterMsgHandle(TM_TIMER,
                              TM_TIMER,
                              OnTimeMessage);

        etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                              CONFIG_MSG_BCAST_CHANGED,
                              OnMessage);
    }
    return m_pModel != NULL;
}

bool CTextMessageController::MyFinal()
{
    // Release the model instance.
    if (m_pModel != NULL)
    {
        // Unregister message.
        etl_UnregisterMsgHandle(SIP_MSG_UPDATE_SMS,
                                SIP_MSG_UPDATE_SMS,
                                OnMessage);

        etl_UnregisterMsgHandle(SIP_MSG_SEND_SMS_RESULT,
                                SIP_MSG_SEND_SMS_RESULT,
                                OnMessage);

        etl_UnregisterMsgHandle(TM_TIMER,
                                TM_TIMER,
                                OnTimeMessage);

        delete m_pModel;
        m_pModel = NULL;
    }
    return m_pModel == NULL;
}

int CTextMessageController::MyClickAction2GroupType(
    const yl::string & strClickAction) const
{
    TextMsgGroupType eGType = TEXT_MSG_GROUP_TYPE_NONE;
    // Convert the click action to message group type.
    if (strClickAction == kszCAInBox)
    {
        eGType = TEXT_MSG_GROUP_IN;
    }
    else if (strClickAction == kszCAOutBox)
    {
        eGType = TEXT_MSG_GROUP_SENDING;
    }
    else if (strClickAction == kszCASentBox)
    {
        eGType = TEXT_MSG_GROUP_SENT;
    }
    else if (strClickAction == kszCADraftBox)
    {
        eGType = TEXT_MSG_GROUP_DRAFT;
    }
    else
    {
        eGType = TEXT_MSG_GROUP_TYPE_NONE;
    }

    return eGType;
}

LRESULT CTextMessageController::OnReceivedMessage(msgObject & objMessage)
{
    return FALSE;
    /*
    LRESULT retVal = FALSE;

    int nAccountId = static_cast<int>(objMessage.wParam);
    int nTextLen = static_cast<int>(objMessage.lParam);
    if (nTextLen > 0)
    {
        // Get text info.
        sip_sms_info_t* pSMSInfo = (sip_sms_info_t *)objMessage.GetExtraData();
        if (NULL == pSMSInfo)
        {
            return FALSE;
        }

        CTextMessageModel::ItemData stID;

        // Parse sendor info.
        stID.m_strFromName = pSMSInfo->user_info.display_name;
        stID.m_strFromUserName = pSMSInfo->user_info.user_name;
        stID.m_strFromServer = pSMSInfo->user_info.server_name;

        // Get reciever info from account.
        if (acc_IsAccountIDValid(nAccountId))
        {
            stID.m_strToName = acc_GetDisplayName(nAccountId);
            stID.m_strToUserName = acc_GetUsername(nAccountId);
            stID.m_strToServer = acc_GetServerName(nAccountId);
        }

        // Copy text.
        char* pszBuffer = new char[nTextLen + 1];
        if (pszBuffer != NULL)
        {
            memset(pszBuffer, 0, nTextLen + 1);
    //#warning "TODO:后续加上对SMS的解析"
            xml_document docText;
            if (!docText.load_buffer(pSMSInfo->xml_body, strlen(pSMSInfo->xml_body)))
            {
                return FALSE;
            }
    //          memcpy(pszBuffer, (BYTE*)pExtraData, nTextLen);
            stID.m_strText = pszBuffer;

            delete [] pszBuffer;
            pszBuffer = NULL;
        }

        SETTING_INFO("Text message, recieved message(%s)",
            stID.m_strText.c_str());

        // Cal time and other.
        stID.m_tTime = netGetLocalTime();
        stID.m_nGType = TEXT_MSG_GROUP_IN;
        stID.m_eSType = CTextMessageModel::STATUS_UNREAD;

        if (m_pModel != NULL)
        {
            retVal = m_pModel->AddDetailMessage(stID) > -1 ? TRUE : FALSE;
        }

        if (retVal)
        {
    //#warning "TODO:MessageUI_ReceivedNewMessage"
    //           MessageUI_ReceivedNewMessage();
        }
    }

    return retVal;
    */
}

LRESULT CTextMessageController::OnMessageSendResult(msgObject & objMessage)
{
    return m_pModel != NULL ? m_pModel->ProcessTextMessageSend(objMessage) : FALSE;
    /*
    if (objMessage.message != SIP_MSG_SEND_SMS_RESULT
        || NULL == objMessage.GetExtraData())
    {
        return FALSE;
    }

    sip_reason_info_t *pResultInfo = (sip_reason_info_t *)objMessage.GetExtraData();

    bool isSucceed = false;
    // 发送成功, 则将消息组更改为已发送.
    // 发送不成功, 不做更改(已在发送中了).
    if (pResultInfo->code == 0)
    {
        if (m_pModel != NULL)
        {
            isSucceed = m_pModel->UpdateMessageGType(TEXT_MSG_GROUP_SENDING,
                0, TEXT_MSG_GROUP_SENT);
        }
    }

    return isSucceed ? TRUE : FALSE;
    */
}

LRESULT CTextMessageController::OnMessageTimer(msgObject & objMessage)
{
    return m_pModel != NULL ? m_pModel->HandleTimeoutMessage(objMessage) : FALSE;
}

}  // namespace NS_SETTING
