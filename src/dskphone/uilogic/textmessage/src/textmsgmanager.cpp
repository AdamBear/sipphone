//#include "setting_inc.h"

#include "setting/include/modsetting.h"
#include <ETLLib.hpp>
#include <interfacedefine.h>
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "setting/include/modsetting.h"
#include "voice/include/voicemanager.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "lamp/powerled/include/powerledmessagedefine.h"
#include "voip/modvoip.h"
#include "textmsgmanager.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "xmlparser/xmlparser.hpp"
#include "idlescreen/include/modidlescreen.h"

// 计时器超时时间.
static const int knTextMsgIntervalTimeout = 5000;
// 账号最大id.
static const int knTextMsgMaxAccountId = 4;
// 未读接收信息的图片名.
static const char kszTextMsgInMsgUnreadIcon[] = "message_in_unread.png";
// 已读接收信息的图片名.
static const char kszTextMsgInMsgReadIcon[] = "message_in_read.png";
// 草稿信息的图片名.
static const char kszTextMsgOutMsgDraftIcon[] = "message_draft.png";
// 发送中信息的图片名.
static const char kszTextMsgOutMsgSendingIcon[] = "message_out_sending.png";
// 发送完毕信息的图片名.
static const char kszTextMsgOutMsgSentIcon[] = "message_out_sent.png";
// 每组信息最大数.
static const int knTextMsgMaxMsgCountPerGroup =  50;
// The configure file name.
//static const char kszTextMsgConfigureFileName[] = "Message.xml";
// The xml file node definition.
// Root node name.
static const char kszTextMsgNodeRoot[] = "root_message";
// Text message node name.
static const char kszTextMsgNodeItem[] = "item";
// Text message from display name.
static const char kszTextMsgAttrFromName[] = "from_name";
// Text message from user name.
static const char kszTextMsgAttrFromUserName[] = "from_user_name";
// Text message from server name.
static const char kszTextMsgAttrFromServer[] = "from_server";
// Text message to display name.
static const char kszTextMsgAttrToName[] = "to_name";
// Text message to user name.
static const char kszTextMsgAttrToUserName[] = "to_user_name";
// Text message to server name.
static const char kszTextMsgAttrToServer[] = "to_server";
// Text message content.
static const char kszTextMsgAttrText[] = "text";
// Text message time.
static const char kszTextMsgAttrTime[] = "time";
// Text message group type.
static const char kszTextMsgAttrGroupType[] = "group_type";
// Text message status type.
static const char kszTextMsgAttrStatusType[] = "status_type";
// In box click action.
static const char kszTextMsgCAInBox[] = "CDlgMessageFrame_inbox";
// Send box click action.
static const char kszTextMsgCASentBox[] = "CDlgMessageFrame_sentbox";
// Out box click action.
static const char kszTextMsgCAOutBox[] = "CDlgMessageFrame_outbox";
// Draft box click action.
static const char kszTextMsgCADraftBox[] = "CDlgMessageFrame_draftbox";


// Message handle method.
LRESULT OnMessageMsg(msgObject & objMessage)
{
    return _TextMsgManager.ProcessMsg(objMessage);
}

CTextMsgManager::CTextMsgManager()
    : m_iTimerID((UINT) & m_iTimerID)
{
}

CTextMsgManager::~CTextMsgManager()
{
    DoRelease();

    // 释放链表
    ItemList::iterator itB = m_listItem.begin();
    for (; itB != m_listItem.end(); ++itB)
    {
        DEL_OBJ((*itB));
    }
    m_listItem.clear();
}


void CTextMsgManager::Init()
{
    DoInit();
}

bool CTextMsgManager::DoInit()
{
    etl_RegisterMsgHandle(SIP_MSG_UPDATE_SMS, SIP_MSG_UPDATE_SMS, OnMessageMsg);
    etl_RegisterMsgHandle(SIP_MSG_SEND_SMS_RESULT, SIP_MSG_SEND_SMS_RESULT, OnMessageMsg);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnMessageMsg);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnMessageMsg);

    m_bCheckFirst = true;

    //// Load text messages from configure file.
    LoadConfigFile();

    //// Check un-read message.
    CheckUnRead();

    ProcessTextMessageChange();

    return true;
}

void CTextMsgManager::Uninit()
{
    DoRelease();
}

bool CTextMsgManager::DoRelease()
{
    etl_UnregisterMsgHandle(SIP_MSG_UPDATE_SMS, SIP_MSG_UPDATE_SMS, OnMessageMsg);

    etl_UnregisterMsgHandle(SIP_MSG_SEND_SMS_RESULT, SIP_MSG_SEND_SMS_RESULT, OnMessageMsg);

    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnMessageMsg);
    // Save text messages to configure file.
    return SaveConfigFile();
}

// 消息处理
bool CTextMsgManager::ProcessMsg(msgObject & objMessage)
{
    //PrintSIPMessage(refObj);
    LRESULT bHandled = true;

    switch (objMessage.message)
    {
    case TM_TIMER:
        {
            OnTimer(objMessage.wParam);
        }
        break;
    case SIP_MSG_UPDATE_SMS:
        {
            SETTING_INFO("receive msg [ SIP_MSG_UPDATE_SMS ]");
            backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            ProcessIntanceMessage(objMessage);
        }
        break;
    case SIP_MSG_SEND_SMS_RESULT:
        {
            OnMessageSendResult(objMessage);
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_SMS != objMessage.wParam)
            {
                break;
            }

            if (0 == objMessage.lParam)
            {
                SetDirty(true);

                // 立即保存更改到文件中
                SaveConfigFile();
            }
            else if (1 == objMessage.lParam)
            {
                // 重新下载短信文件
                LoadConfigFile(false);
            }
            else if (2 == objMessage.lParam)
            {
                if (!IsSMSEnable())
                {
                    RemoveAllMessage();
                    SetDirty(true);
                    SaveConfigFile();
                }
            }
        }
        break;
    default:
        {
            bHandled = false;
        }
        break;
    }

    return bHandled;
}

bool CTextMsgManager::GetAccountMsgList(AccountTextMsgListData & refData)
{
    YLHashMap<yl::string, bool> mapAccountName;
    AccountTextMsgData stATMD;
    for (int i = 0; i < knTextMsgMaxAccountId; ++i)
    {
        if (acc_IsAccountIDValid(i))
        {
            stATMD.Reset();

            // 获取账号显示名, 如果显示名为空, 则获取用户名.
            stATMD.m_strAccountDisplayName = acc_GetDisplayName(i);
            if (stATMD.m_strAccountDisplayName.empty())
            {
                stATMD.m_strAccountDisplayName = acc_GetUsername(i);
            }

            // 获取账号当前有的信息数.
            stATMD.m_nUnReadCount = GetAccountUnRead(acc_GetUsername(i));

            mapAccountName.insert(acc_GetUsername(i), true);
        }

        refData.m_listMsg.push_back(stATMD);
    }

    // 遍历map, 查找所有的未读信息.
    for (UnReadMap::iterator citer = m_mapUnRead.begin();
            citer != m_mapUnRead.end();
            ++citer)
    {
        if (mapAccountName.find(citer->first) == mapAccountName.end())
        {
            stATMD.m_strAccountDisplayName = citer->first;
            stATMD.m_nUnReadCount = citer->second;
            refData.m_listMsg.push_back(stATMD);
        }
    }

    return true;
}

int CTextMsgManager::GetFirstUnReadInMessageIndex()
{
    int i = -1;

    // 获取第一个接收到的第一个未读信息.
    for (ItemList::const_iterator citer = m_listItem.begin();
            citer != m_listItem.end();
            ++citer)
    {
        const TextMsgItemData * pItemData = *citer;
        if (pItemData != NULL
                && pItemData->m_nGType == TEXT_MSG_GROUP_IN)
        {
            ++i;

            if (pItemData->m_eSType == TMSG_STATUS_UNREAD)
            {
                break;
            }
        }
    }

    return i;
}

bool CTextMsgManager::GetMessageList(TextMsgSimpListData & refData, TextMsgGroupType eGtype)
{
    // 参数检查.
    if (eGtype <= TEXT_MSG_GROUP_TYPE_NONE
            || eGtype >= TEXT_MSG_GROUP_TYPE_MAX)
    {
        return false;
    }

    // 获取下标起始值.
    int nIndexFrom = refData.m_nIndexFrom;
    int nIndexTo = refData.m_nCountToGet + nIndexFrom;

    // 遍历并返回指定下标的信息列表.
    TextMsgItemSimpData stTMISD;
    int i = -1;
    ItemList::iterator citer = m_listItem.begin();
    for (; citer != m_listItem.end(); ++citer)
    {
        TextMsgItemData * pItemData = (*citer);
        if (pItemData != NULL && pItemData->m_nGType == eGtype)
        {
            ++i;
            if (i >= nIndexFrom
                    && i < nIndexTo
                    && ID2TMSD(stTMISD, *pItemData))
            {
                refData.m_listMessage.push_back(stTMISD);
            }
        }
    }  // for.

    // 获取结果总数.
    refData.m_nTotalCount = i + 1;
    return true;
}

// 处理TextMessage消息
bool CTextMsgManager::ProcessIntanceMessage(msgObject & objMessage)
{
    // http://192.168.1.99/Bug.php?BugID=22025
    if (!IsSMSEnable())
    {
        return true;
    }

    // 取信息附加块
    sip_sms_info_t * pSMSInfo = (sip_sms_info_t *) objMessage.GetExtraData();
    if (NULL == pSMSInfo)
    {
        return false;
    }

    BasicUserInfo toUserInfo, fromUserInfo;
    MapSipUserInfo_BasicInfo(&pSMSInfo->user_info, fromUserInfo);

    int iAccount = objMessage.wParam;
    if (acc_IsAccountIDValid(iAccount))
    {
        toUserInfo.strDisplay = acc_GetDisplayName(iAccount);
        toUserInfo.strNumber = acc_GetUsername(iAccount);
        toUserInfo.strServer = acc_GetServerName(iAccount);
    }

    xml_document doc;
    doc.load_file(szXmlMessageFile);

    xml_node nodeRoot = doc.child(kszTextMsgNodeRoot);
    xml_node nodeChild = nodeRoot.append_child(kszTextMsgNodeItem);
    nodeChild.append_attribute(kszTextMsgAttrFromName, fromUserInfo.strDisplay.c_str());
    nodeChild.append_attribute(kszTextMsgAttrFromUserName, fromUserInfo.strNumber.c_str());
    nodeChild.append_attribute(kszTextMsgAttrFromServer, fromUserInfo.strServer.c_str());
    nodeChild.append_attribute(kszTextMsgAttrToName, toUserInfo.strDisplay.c_str());
    nodeChild.append_attribute(kszTextMsgAttrToUserName, toUserInfo.strNumber.c_str());
    nodeChild.append_attribute(kszTextMsgAttrToServer, toUserInfo.strServer.c_str());

    // yzh 这里解析文字信息内容
    xml_document docText;
    yl::string strText(pSMSInfo->xml_body);
    if (!docText.load_buffer(pSMSInfo->xml_body, strlen(pSMSInfo->xml_body)))
    {
        return false;
    }

    nodeChild.append_attribute(kszTextMsgAttrText, strText.c_str());
    nodeChild.append_attribute(kszTextMsgAttrStatusType, TMSG_STATUS_UNREAD);
    nodeChild.append_attribute(kszTextMsgAttrGroupType, TEXT_MSG_GROUP_IN);
    time_t timeMsg = netGetLocalTime();
    nodeChild.append_attribute(kszTextMsgAttrTime, timeMsg);
    doc.save_file(szXmlMessageFile);

    TextMsgItemData itemData;
    itemData.m_strFromName = fromUserInfo.strDisplay;
    itemData.m_strFromServer = fromUserInfo.strServer;
    itemData.m_strFromUserName = fromUserInfo.strNumber;
    itemData.m_strText = strText;
    itemData.m_strToName = toUserInfo.strDisplay;
    itemData.m_strToServer = toUserInfo.strServer;
    itemData.m_strToUserName = toUserInfo.strNumber;
    itemData.m_tTime = timeMsg;
    itemData.m_eSType = TMSG_STATUS_UNREAD;
    itemData.m_nGType = TEXT_MSG_GROUP_IN;
    InsertItem(itemData);

    SETTING_INFO("the number of message is = %d", GetTotalUnRead());
    ProcessTextMessageChange();

    return true;
}

// 处理TextMessage未读消息
bool CTextMsgManager::ProcessTextMessageChange()
{
    int iUnReadNum = GetTotalUnRead();
    SETTING_INFO("ProcessTextMessageChange iUnReadNum[%d]", iUnReadNum);
    int iRet = 1;
    // wParam: unread num, lParam: 1=show, 0=hide
    //  if (1 == objMsg.lParam)
    {
        if (iUnReadNum > 0)
        {
            if (!talklogic_SessionExist())
            {
                voice_PlayTone(TONE_TEXT_MSG, 300);
                if (m_bCheckFirst)
                {
                    etlSleep(300);
                    // 上电检测会占用较多CPU，未读提示音的定时器会无法被处理，导致声音响太久，
                    // 次处延迟后再停止响提示音
                    voice_StopTone(TONE_TEXT_MSG);
                    m_bCheckFirst = false;
                }
            }
            idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, commonAPI_FormatString("%d", iUnReadNum));
        }
        else
        {
            idleScreen_DropStatus(PS_STATE_TEXTMESSAGE);
        }
    }

    etl_NotifyApp(false, POWER_LED_TEXT_MESSAGE, false, 0);

    return iRet;
}

bool CTextMsgManager::OnTimer(int iTimerID)
{
    if (iTimerID != m_iTimerID)
    {
        return false;
    }

    // 保存.
    SaveConfigFile();
    return true;
}

bool CTextMsgManager::LoadConfigFile(bool bCheck /*= true*/)
{
    // http://192.168.1.99/Bug.php?BugID=22025
    if (!IsSMSEnable())
    {
        return false;
    }

    // Get the xml file's full path.
    // Get full xml path.
    if (!pathFileExist(szXmlMessageFile))
    {
        SETTING_WARN("file is not found [%s]", szXmlMessageFile);
        return false;
    }

    RemoveAllMessage(bCheck);

    xml_document doc;
    doc.load_file(szXmlMessageFile);
    xml_node nodeRoot = doc.child(kszTextMsgNodeRoot);
    if (!nodeRoot)
    {
        return false;
    }

    TextMsgItemData itemData;
    xml_node nodeChild = nodeRoot.child(kszTextMsgNodeItem);
    if (!nodeChild)
    {
        return false;
    }

    while (nodeChild)
    {
        itemData.Reset();
        // Parse the attribute values.
        itemData.m_strFromName = nodeChild.attribute(kszTextMsgAttrFromName).value();
        itemData.m_strFromUserName = nodeChild.attribute(kszTextMsgAttrFromUserName).value();
        itemData.m_strFromServer = nodeChild.attribute(kszTextMsgAttrFromServer).value();
        itemData.m_strToName = nodeChild.attribute(kszTextMsgAttrToName).value();
        itemData.m_strToUserName = nodeChild.attribute(kszTextMsgAttrToUserName).value();
        itemData.m_strToServer = nodeChild.attribute(kszTextMsgAttrToServer).value();
        itemData.m_strText = nodeChild.attribute(kszTextMsgAttrText).value();
        itemData.m_tTime = nodeChild.attribute(kszTextMsgAttrTime).as_int();
        itemData.m_nGType = nodeChild.attribute(kszTextMsgAttrGroupType).as_int();
        itemData.m_eSType = static_cast<TEXTMSG_STATUS_TYPE>(nodeChild.attribute(
                                kszTextMsgAttrStatusType).as_int());

        // PUSH到成员链表中去
        if (InsertItem(itemData) < 0)
        {
            return false;
        }
        nodeChild = nodeChild.next_sibling();
    }

    return true;
}

// 保存到文件
bool CTextMsgManager::SaveConfigFile()
{
    if (!GetDirty())
    {
        return false;
    }

    timerKillThreadTimer(m_iTimerID);

    xml_document doc;
    xml_node nodeRoot = doc.append_child(kszTextMsgNodeRoot);

    YLList<TextMsgItemData *>::iterator itrBegin = m_listItem.begin();
    // Add root node and iterate to add child nodes.
    for (; itrBegin != m_listItem.end(); ++itrBegin)
    {
        if (const TextMsgItemData * pItemData = (*itrBegin))
        {
            xml_node nodeChild = nodeRoot.append_child(kszTextMsgNodeItem);
            if (!nodeChild)
            {
                return false;
            }

            // Add child node.
            nodeChild.append_attribute(kszTextMsgAttrFromName, pItemData->m_strFromName.c_str());
            nodeChild.append_attribute(kszTextMsgAttrFromUserName, pItemData->m_strFromUserName.c_str());
            nodeChild.append_attribute(kszTextMsgAttrFromServer, pItemData->m_strFromServer.c_str());
            nodeChild.append_attribute(kszTextMsgAttrToName, pItemData->m_strToName.c_str());
            nodeChild.append_attribute(kszTextMsgAttrToUserName, pItemData->m_strToUserName.c_str());
            nodeChild.append_attribute(kszTextMsgAttrToServer, pItemData->m_strToServer.c_str());
            nodeChild.append_attribute(kszTextMsgAttrText, pItemData->m_strText.c_str());
            nodeChild.append_attribute(kszTextMsgAttrTime, (int) pItemData->m_tTime);
            nodeChild.append_attribute(kszTextMsgAttrGroupType, pItemData->m_nGType);
            nodeChild.append_attribute(kszTextMsgAttrStatusType, (int) pItemData->m_eSType);
        }
        // 再创建一个结点
    }

    // Get full xml path.
    // Save to file.
    doc.save_file(szXmlMessageFile);
    SetDirty(false);
    return true;
}

LRESULT CTextMsgManager::OnMessageSendResult(msgObject & objMessage)
{
    // yzh 实现有问题
    if (objMessage.message != SIP_MSG_SEND_SMS_RESULT
            || NULL == objMessage.GetExtraData())
    {
        return false;
    }

    sip_reason_info_t * pResultInfo = (sip_reason_info_t *) objMessage.GetExtraData();

    bool isSucceed = false;
    // 发送成功, 则将消息组更改为已发送.
    // 发送不成功, 不做更改(已在发送中了).
    if (pResultInfo->code == 0)
    {
        isSucceed = UpdateMessageGType(TEXT_MSG_GROUP_SENDING, 0, TEXT_MSG_GROUP_SENT);
    }

    return isSucceed ? true : false;
}

int CTextMsgManager::AddDetailMessage(const TextMsgItemData & refItem)
{
    int nIndex = InsertItem(refItem);
    if (nIndex > -1)
    {
        // 如果插入成功, 且未接收的消息, 则提示idle.
        if (refItem.m_nGType == TEXT_MSG_GROUP_IN
                && refItem.m_eSType == TMSG_STATUS_UNREAD)
        {
            yl::string strCount = commonAPI_FormatString("%d", GetTotalUnRead());
            idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, strCount);
            //有通话时不播放提示音
            if (!talklogic_SessionExist(true))
            {
                // 响铃.
                voice_PlayTone(TONE_TEXT_MSG, 500);
            }
        }
        // 亮灯.
        etl_NotifyApp(true, POWER_LED_TEXT_MESSAGE, true, 0);

        SetDirty(true);
    }

    return nIndex;
}

int CTextMsgManager::InsertItem(const TextMsgItemData & refItem)
{
    if (refItem.m_nGType <= TEXT_MSG_GROUP_TYPE_NONE
            || refItem.m_nGType >= TEXT_MSG_GROUP_TYPE_MAX
            || refItem.m_eSType <= TMSG_STATUS_TYPE_NONE
            || refItem.m_eSType >= TMSG_STATUS_TYPE_MAX)
    {
        return -1;
    }

    // 遍历信息列表, 找到插入位置(所谓插入位置, 即待插入项的时间大于链表中
    // 后续项的位置.
    bool isFound = false;  // 是否找到.
    ItemList::iterator iterLoc = m_listItem.begin();  // 插入位置.
    int nIndex = -1;  // 记录插入点的下标.
    ItemList::iterator iter = m_listItem.begin();
    while (iter != m_listItem.end())
    {
        const TextMsgItemData * pItemData = (*iter);
        if (pItemData != NULL && pItemData->m_nGType == refItem.m_nGType)
        {
            ++nIndex;

            // 如果超过上限, 则删除多余的.
            if (nIndex >= knTextMsgMaxMsgCountPerGroup - 1)
            {
                // 如果是未读接收消息, 则在删除之前更新数量.
                if (pItemData->m_nGType == TEXT_MSG_GROUP_IN
                        && pItemData->m_eSType == TMSG_STATUS_UNREAD)
                {
                    AddUnReadToAccount(pItemData->m_strToUserName, -1);
                }

                delete (*iter);
                (*iter) = NULL;
                iter = m_listItem.erase(iter);
            }
            else
            {
                if (pItemData->m_tTime < refItem.m_tTime && !isFound)
                {
                    iterLoc = iter;
                    isFound = true;
                }
                ++iter;
            }
        }
        else
        {
            ++iter;
        }
    }  // while.

    // 如果未找到插入点, 则插入到链表末尾.
    if (!isFound)
    {
        iterLoc = m_listItem.end();
        nIndex = (int) m_listItem.size();
    }

    // 插入信息.
    TextMsgItemData * pNew = new TextMsgItemData();
    if (pNew != NULL)
    {
        *pNew = refItem;
        m_listItem.insert(iterLoc, pNew);

        if (pNew->m_nGType == TEXT_MSG_GROUP_IN
                && pNew->m_eSType == TMSG_STATUS_UNREAD)
        {
            AddUnReadToAccount(pNew->m_strToUserName, 1);
        }
    }

    return nIndex;
}

bool CTextMsgManager::AddUnReadToAccount(const yl::string & strUserName, int nAdd)
{
    // 查找账号, 并向账号中添加新的未读信息.
    UnReadMap::iterator iter = m_mapUnRead.find(strUserName);
    if (iter != m_mapUnRead.end())
    {
        iter->second = iter->second + nAdd;
    }
    else
    {
        // 如果没有则不需要减少未接数.
        int nAddCpy = nAdd > 0 ? nAdd : 0;
        m_mapUnRead.insert(strUserName, nAddCpy);
    }
    return true;
}

bool CTextMsgManager::GetDirty()
{
    return m_isDirty;
}

void CTextMsgManager::SetDirty(bool isDirty)
{
    if (m_isDirty != isDirty)
    {
        m_isDirty = isDirty;

        if (m_isDirty)
        {
            timerSetThreadTimer(m_iTimerID, knTextMsgIntervalTimeout);
        }
    }
}

bool CTextMsgManager::UpdateMessageGType(TextMsgGroupType eGType, int nIndex,
        TextMsgGroupType  eGNewType)
{
    // 查找租中指定序号的信息数据.
    TextMsgItemData * pItemData = FindItem(eGType, nIndex);
    if (pItemData != NULL)
    {
        TextMsgItemData pNewItemData;
        pNewItemData.m_nGType = eGNewType;
        pNewItemData.m_strFromName = pItemData->m_strFromName;
        pNewItemData.m_strFromUserName = pItemData->m_strFromUserName;
        pNewItemData.m_strFromServer = pItemData->m_strFromServer;
        pNewItemData.m_strToName = pItemData->m_strToName;
        pNewItemData.m_strToUserName = pItemData->m_strToUserName;
        pNewItemData.m_strToServer = pItemData->m_strToServer;
        pNewItemData.m_strText = pItemData->m_strText;
        pNewItemData.m_tTime = pItemData->m_tTime;
        pNewItemData.m_eSType = pItemData->m_eSType;

        RemoveMessage(eGType, nIndex);

        CheckUnRead();
        // insert box
        InsertItem(pNewItemData);

        SetDirty(true);
        return true;
    }

    return false;
}

void CTextMsgManager::CheckUnRead()
{
    int nTotal = GetTotalUnRead();
    if (nTotal > 0)
    {
#if SUPPORT_SMS
        // 找到最新的短消息
        TextMsgItemData * pItemData = FindItem(TEXT_MSG_GROUP_IN, nTotal - 1);
        if (NULL != pItemData)
        {
            // 如果SMS有处理,则将此处的信息设为已读,SMS的数据将由SMS Model来处理
            // 将所有的信息都设为已读
            for (ItemList::iterator iter = m_listItem.begin(); iter != m_listItem.end(); ++iter)
            {
                TextMsgItemData * pItemDataTemp = (*iter);
                if (pItemDataTemp != NULL && pItemDataTemp->m_nGType == TEXT_MSG_GROUP_IN)
                {
                    pItemDataTemp->m_eSType = TMSG_STATUS_READ;
                }
            }
            m_mapUnRead.clear();
        }
#endif
    }
}

bool CTextMsgManager::ID2TMSD(TextMsgItemSimpData & refData, const TextMsgItemData & refItem)
{
    // 拷贝名字.
    yl::string strDisplay;  // 显示名.
    switch (refItem.m_nGType)
    {
    case TEXT_MSG_GROUP_IN:
        {
            strDisplay = GetDisplayName(refItem.m_strFromUserName, refItem.m_strFromServer, false);
        }
        break;
    case TEXT_MSG_GROUP_DRAFT:
    case TEXT_MSG_GROUP_SENDING:
    case TEXT_MSG_GROUP_SENT:
        {
            strDisplay = GetDisplayName(refItem.m_strToUserName, refItem.m_strToServer, false);
        }
        break;
    default:
        break;
    }

    // 从账号中查找账号名.
    refData.m_strName = strDisplay;

    // TODO 拷贝时间.
    // for debug
    refData.m_tTime = refItem.m_tTime;//commonUnits_Time2NewFormatString(refItem.m_tTime, false);

    // 获取图片(全路径).
    refData.m_strIconPath = GetIconPath(refItem.m_nGType, refItem.m_eSType);

    // 拷贝类型.
    refData.m_eGType = static_cast<TextMsgGroupType>(refItem.m_nGType);

    return true;
}

yl::string CTextMsgManager::GetDisplayName(const yl::string & strUser, const yl::string & strServer,
        bool isSelf) const
{
    yl::string strRet;

    // 计算自己.
    if (isSelf)
    {
        yl::string strFullName(strUser);
        if (!strServer.empty())
        {
            strFullName.append("@");
            strFullName.append(strServer);
        }

        // 获取账号显示名.
        int iAccount = INVALID_ACCOUNTID; //acc_GetAccountIDByFullName(strFullName.c_str());
        if (acc_IsAccountIDValid(iAccount))
        {
            strRet = configParser_GetCfgItemStringValue(kszAccountDisplayName, iAccount);
        }
    }
    else
    {
        // 如果不是自己, 则从联系人中查找名字.
        strRet = "";//Directory_GetContactNameByNumber(strUser.c_str());
    }

    return strRet.empty() ? strUser : strRet;
}


bool CTextMsgManager::SendTheMessage(const TextMsgSendItemData & refData)
{
    // First add a message to the sending group.
    bool isSucceed = false;
    int nIndex = AddMessage(refData,
                            TEXT_MSG_GROUP_SENDING,
                            TMSG_STATUS_READ);
    if (nIndex >= 0)
    {
        // Use selected account to send text message.
        int iSelAccount = refData.m_nFromAccountId;
        if (acc_IsAccountIDValid(iSelAccount))
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
            }
            else
            {
                strTo.append(refData.m_strTo);
                strTo.append("@");
                strTo.append(acc_GetServerName(iSelAccount));
            }

            return modVoip_SendTextMessage(iSelAccount, refData.m_strText.c_str(), strTo.c_str());
        }
    }

    return isSucceed;
}
int CTextMsgManager::AddMessage(const TextMsgSendItemData & refItem, TextMsgGroupType eGType,
                                TEXTMSG_STATUS_TYPE eSType)
{
    if (eGType <= TEXT_MSG_GROUP_TYPE_NONE || eGType >= TEXT_MSG_GROUP_TYPE_MAX
            || eSType <= TMSG_STATUS_TYPE_NONE || eSType >= TMSG_STATUS_TYPE_MAX)
    {
        return -1;
    }

    // 插入信息.
    int nIndex = -1;
    TextMsgItemData stID;
    if (TMDID2ID(stID, eGType, eSType, refItem))
    {
        nIndex = AddDetailMessage(stID);
    }

    return nIndex;
}

bool CTextMsgManager::TMDID2ID(TextMsgItemData & refData, int nGType, TEXTMSG_STATUS_TYPE eSType,
                               const TextMsgSendItemData & refItem)
{
    if (nGType > TEXT_MSG_GROUP_TYPE_NONE
            && nGType < TEXT_MSG_GROUP_TYPE_MAX
            && eSType > TMSG_STATUS_TYPE_NONE &&
            eSType < TMSG_STATUS_TYPE_MAX)
    {
        // 获取发送者信息.
        int iAccount = refItem.m_nFromAccountId;
        refData.m_strFromName = acc_GetDisplayName(iAccount);
        refData.m_strFromUserName = acc_GetUsername(iAccount);
        refData.m_strFromServer = acc_GetServerName(iAccount);

        // 拷贝数据.
        refData.m_strToName = refItem.m_strTo;

        // 是否有发server.
        size_t uPosFound = refItem.m_strTo.find("@");
        if (uPosFound != yl::string::npos)
        {
            refData.m_strToUserName = refItem.m_strTo.substr(0, uPosFound);
            refData.m_strToServer = refItem.m_strTo.substr(uPosFound + 1);
        }
        else
        {
            refData.m_strToUserName = refItem.m_strTo;
            // 如果没指定服务器, 则和发送方在同一服务器中.
            refData.m_strToServer = refData.m_strFromServer;
        }

        refData.m_strText = refItem.m_strText;
        refData.m_nGType = nGType;
        refData.m_eSType = eSType;
        refData.m_tTime = refItem.m_tTime;
        return true;
    }

    return false;
}

bool CTextMsgManager::RemoveAllMessage(TextMsgGroupType eGtype)
{
    bool bInbox = false;
    // 遍历, 并删除指定组中的所有信息.
    ItemList::iterator iter = m_listItem.begin();
    while (iter != m_listItem.end())
    {
        TextMsgItemData * pItemData = (*iter);
        if (pItemData != NULL)
        {
            if (pItemData->m_nGType == eGtype)
            {
                // 如果为未读接收信息, 需要更改数量.
                if (pItemData->m_nGType == TEXT_MSG_GROUP_IN
                        && pItemData->m_eSType == TMSG_STATUS_UNREAD)
                {
                    bInbox = true;
                    AddUnReadToAccount(pItemData->m_strToUserName, -1);
                }

                delete (*iter);
                (*iter) = NULL;
                iter = m_listItem.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        else
        {
            iter = m_listItem.erase(iter);  // 删除空指针项.
        }
    }  // while.

    CheckUnRead();
    SetDirty(true);

    // 处理未读短信提示
    if (bInbox)
    {
        ProcessTextMessageChange();
    }
    return true;
}

bool CTextMsgManager::RemoveAllMessage(bool bCheck /*= true*/)
{
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();)
    {
        TextMsgItemData * pItemData = (*iter);
        if (pItemData == NULL)
        {
            continue;
        }
        // 如果为未读接收信息, 需要更改数量.
        if (pItemData->m_nGType == TEXT_MSG_GROUP_IN
                && pItemData->m_eSType == TMSG_STATUS_UNREAD)
        {
            AddUnReadToAccount(pItemData->m_strToUserName, -1);
        }

        delete (*iter);
        (*iter) = NULL;
        iter = m_listItem.erase(iter);
    }

    if (bCheck)
    {
        // 处理未读短信提示
        ProcessTextMessageChange();
    }

    return true;
}

bool CTextMsgManager::RemoveMessage(TextMsgGroupType eGtype, int nIndex)
{
    // 遍历信息列表, 从中删除指定组中下标对应的项.
    int i = -1;  // 记录成员在组中的下标.
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        TextMsgItemData * pItemData = (*iter);
        if (pItemData != NULL && pItemData->m_nGType == eGtype)
        {
            ++i;
            if (i == nIndex)
            {
                // 如果为未读接收信息, 需要更改数量.
                if (pItemData->m_nGType == TEXT_MSG_GROUP_IN
                        && pItemData->m_eSType == TMSG_STATUS_UNREAD)
                {
                    AddUnReadToAccount(pItemData->m_strToUserName, -1);
                    // 处理未读短信提示
                    ProcessTextMessageChange();
                }

                delete (*iter);
                (*iter) = NULL;
                m_listItem.erase(iter);

                CheckUnRead();
                SetDirty(true);
                return true;
            }
        }
    }  // for.

    return false;
}

bool CTextMsgManager::RemoveMessageList(TextMsgGroupType eGtype, YLList<int> & listIndex)
{
    int i = -1;
    bool isDirty = false;
    YLList<int> listCpy = listIndex;// (listIndex.begin(), listIndex.end());
    ItemList::iterator iter = m_listItem.begin();
    // 遍历信息列表, 从中删除指定组中下标对应的项.
    while (iter != m_listItem.end())
    {
        if (listCpy.size() == 0)
        {
            break;
        }

        bool isRemoved = false;
        TextMsgItemData * pItemData = *iter;
        if (pItemData != NULL && pItemData->m_nGType == eGtype)
        {
            ++i;
            for (YLList<int>::iterator iterIndex = listCpy.begin();
                    iterIndex != listCpy.end();
                    ++iterIndex)
            {
                if (i == *iterIndex)
                {
                    // 如果为未读接收信息, 需要更改数量.
                    if (pItemData->m_nGType == TEXT_MSG_GROUP_IN
                            && pItemData->m_eSType == TMSG_STATUS_UNREAD)
                    {
                        AddUnReadToAccount(pItemData->m_strToUserName, -1);
                    }

                    delete *iter;
                    *iter = NULL;
                    iter = m_listItem.erase(iter);

                    isDirty = true;
                    isRemoved = true;
                    listCpy.erase(iterIndex);
                    break;
                }
            }  // for.
        }

        if (!isRemoved)
        {
            ++iter;
        }
    }  // for.

    if (isDirty)
    {
        CheckUnRead();
        SetDirty(true);
    }
    return true;
}

TextMsgItemData * CTextMsgManager::FindItem(int nGType, int nIndex)
{
    // 参数检查.
    if (nGType <= TEXT_MSG_GROUP_TYPE_NONE
            || nGType >= TEXT_MSG_GROUP_TYPE_MAX || nIndex < 0)
    {
        return NULL;
    }

    // 遍历查找到信息.
    int i = -1;
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        TextMsgItemData * pItemData = (*iter);
        if (pItemData != NULL && pItemData->m_nGType == nGType)
        {
            ++i;
            if (i == nIndex)
            {
                return pItemData;
            }
        }
    }  // for.

    return NULL;
}

int CTextMsgManager::AddDraftMessage(const TextMsgSendItemData & refData)
{
    // Add a draft message to the model.
    return AddMessage(refData, TEXT_MSG_GROUP_DRAFT, TMSG_STATUS_READ);
}

TEXTMSG_STATUS_TYPE CTextMsgManager::GetMessageStatus(TextMsgGroupType eGtype, int nIndex)
{
    // 查找租中指定序号的信息数据.
    TextMsgItemData * pItemData = FindItem(eGtype, nIndex);
    if (NULL == pItemData)
    {
        return TMSG_STATUS_TYPE_NONE;
    }

    return pItemData->m_eSType;
}

bool CTextMsgManager::GetMessageDetail(TextMsgItemDetailData & refData, TextMsgGroupType eGtype,
                                       int nIndex)
{
    // 查找租中指定序号的信息数据.
    TextMsgItemData * pItemData = FindItem(eGtype, nIndex);
    if (pItemData != NULL)
    {
        // 转换数据.
        ID2TMIDD(refData, *pItemData);

        // 如果为接收信息, 则更改接收信息的数量, 并将
        // 未读信息更改为已读信息.
        if (pItemData->m_nGType == TEXT_MSG_GROUP_IN
                && pItemData->m_eSType == TMSG_STATUS_UNREAD)
        {
            int nCountBefore = GetTotalUnRead();

            AddUnReadToAccount(pItemData->m_strToUserName, -1);
            pItemData->m_eSType = TMSG_STATUS_READ;

            // 如果读完之后, 未读信息变为0, 则取消未读信息状态.
            int nCountCur = GetTotalUnRead();
            if (nCountBefore != nCountCur
                    && nCountCur == 0)
            {
                ProcessTextMessageChange();
            }
#ifdef _T4X
            else if (nCountCur > 0)
            {
                idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, commonAPI_FormatString("%d", nCountCur));
                // for debug
                //idleScreen_SetPopupStatus(PS_STATE_TEXTMESSAGE, POPUP_WINDOW_NO_POPUP);
                idleScreen_ClearNotifyTypeFlag(PS_STATE_TEXTMESSAGE, SNT_POPUP);
            }
#endif // _T4X

            // 保存.
            SetDirty(true);
        }

        return true;
    }

    return false;
}

bool CTextMsgManager::ID2TMIDD(TextMsgItemDetailData & refData, const TextMsgItemData & refItem)
{
    yl::string strFromName;  // 发送者名.
    yl::string strToName;  // 接收者名.

    // 根据类型获取发送方和接收方的显示名.
    switch (refItem.m_nGType)
    {
    case TEXT_MSG_GROUP_IN:
        {
            strFromName = GetDisplayName(refItem.m_strFromUserName, refItem.m_strFromServer, false);
            strToName = GetDisplayName(refItem.m_strToUserName, refItem.m_strToServer, true);
        }
        break;
    case TEXT_MSG_GROUP_DRAFT:
    case TEXT_MSG_GROUP_SENDING:
    case TEXT_MSG_GROUP_SENT:
        {
            strFromName = GetDisplayName(refItem.m_strFromUserName, refItem.m_strFromServer, true);
            strToName = GetDisplayName(refItem.m_strToUserName, refItem.m_strToServer, false);
        }
        break;
    default:
        break;
    }

    // 拷贝From.
    refData.m_strFromName = strFromName;
    refData.m_strFromUserName = refItem.m_strFromUserName;
    refData.m_strFromServer = refItem.m_strFromServer;

    // 拷贝to.
    refData.m_strToName = strToName;
    refData.m_strToUserName = refItem.m_strToUserName;
    refData.m_strToServer = refItem.m_strToServer;

    // 拷贝信息内容.
    refData.m_strText = refItem.m_strText;

    // 获取图片(全路径).
    refData.m_strIconPath = GetIconPath(refItem.m_nGType, refItem.m_eSType);

    // TODO 拷贝时间.
    // for debug
    refData.m_tTime = refItem.m_tTime;//commonUnits_Time2NewFormatString(refItem.m_tTime);

    // 拷贝类型.
    refData.m_eGType = static_cast<TextMsgGroupType>(refItem.m_nGType);

    return true;
}

yl::string CTextMsgManager::GetIconPath(int nGType, TEXTMSG_STATUS_TYPE eSType)
{
    // 获取目录路径.
    yl::string strRet;
    strRet.append(THEME_IMAGE_PATH);
    if (strRet.substr(strRet.length() - 1) != "/")
    {
        strRet.append("/");
    }
    strRet.append("textmessage/");

    // 获取各个图片.
    switch (nGType)
    {
    case TEXT_MSG_GROUP_IN:
        switch (eSType)
        {
        case TMSG_STATUS_UNREAD:
            {
                strRet.append(kszTextMsgInMsgUnreadIcon);
            }
            break;
        case TMSG_STATUS_READ:
            {
                strRet.append(kszTextMsgInMsgReadIcon);
            }
            break;
        default:
            break;
        }
        break;
    case TEXT_MSG_GROUP_DRAFT:
        {
            strRet.append(kszTextMsgOutMsgDraftIcon);
        }
        break;
    case TEXT_MSG_GROUP_SENDING:
        {
            strRet.append(kszTextMsgOutMsgSendingIcon);
        }
        break;
    case TEXT_MSG_GROUP_SENT:
        {
            strRet.append(kszTextMsgOutMsgSentIcon);
        }
        break;
    default:
        {
            strRet.clear();
        }
        break;
    }

    return strRet;
}

int CTextMsgManager::GetAccountUnRead(const yl::string & strUserName)
{
    // 获取账号的未读信息数.
    UnReadMap::iterator citer = m_mapUnRead.find(strUserName);
    return citer != m_mapUnRead.end() ? citer->second : 0;
}

int CTextMsgManager::GetTotalUnRead()
{
    // 遍历map统计所有未读信息数.
    int nTotal = 0;
    for (UnReadMap::iterator citer = m_mapUnRead.begin(); citer != m_mapUnRead.end(); ++citer)
    {
        nTotal += citer->second;
    }

    return nTotal;
}

// SMS是否可用
bool CTextMsgManager::IsSMSEnable()
{
    return (1 == configParser_GetConfigInt(kszSMSEnable));
}

// Click action definition.
TextMsgGroupType CTextMsgManager::ClickAction2GroupType(const yl::string & strClickAction) const
{
    TextMsgGroupType eGType = TEXT_MSG_GROUP_TYPE_NONE;
    // Convert the click action to message group type.
    if (strClickAction == kszTextMsgCAInBox)
    {
        eGType = TEXT_MSG_GROUP_IN;
    }
    else if (strClickAction == kszTextMsgCAOutBox)
    {
        eGType = TEXT_MSG_GROUP_SENDING;
    }
    else if (strClickAction == kszTextMsgCASentBox)
    {
        eGType = TEXT_MSG_GROUP_SENT;
    }
    else if (strClickAction == kszTextMsgCADraftBox)
    {
        eGType = TEXT_MSG_GROUP_DRAFT;
    }
    else
    {
        eGType = TEXT_MSG_GROUP_TYPE_NONE;
    }

    return eGType;
}
