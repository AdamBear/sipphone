#include "textmessagecontroller.h"
#include "account/include/modaccount.h"
#include "devicelib/networkserver.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/powerled/include/powerledmessagedefine.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "xmlparser/xmlparser.hpp"

#include "voip/sipsender.h"
#include "idlescreen/include/modidlescreen.h"
#include "voice/include/modvoice.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "baseui/include/t2xpicpath.h"
#include "baseui/include/baseuicommon.h"
#include "contacts/directory/include/moddirinterface.h"

// 账号最大id.
static const int knMaxAccountId = 4;

// 每组信息最大数.
static const int knMaxMsgCountPerGroup = 50;

// The configure file name.
static const char kszConfigureFileName[] = "text_message_list.xml";

// The xml file node definition.
// Root node name.
static const char kszNodeRoot[] = "root_message";
// Text message node name.
static const char kszNodeItem[] = "item";
// Text message from display name.
static const char kszAttrFromName[] = "from_name";
// Text message from user name.
static const char kszAttrFromUserName[] = "from_user_name";
// Text message from server name.
static const char kszAttrFromServer[] = "from_server";
// Text message to display name.
static const char kszAttrToName[] = "to_name";
// Text message to user name.
static const char kszAttrToUserName[] = "to_user_name";
// Text message to server name.
static const char kszAttrToServer[] = "to_server";
// Text message content.
static const char kszAttrText[] = "text";
// Text message time.
static const char kszAttrTime[] = "time";
// Text message group type.
static const char kszAttrGroupType[] = "group_type";
// Text message status type.
static const char kszAttrStatusType[] = "status_type";

// 计时器id.
static const UINT kuTimerId = 70007;
// 计时器超时时间.
static const int knIntervalTimeout = 5000;

typedef YLHashMap<yl::string, bool> MAP_ACCOUNT_NAME;

//////////////////////////////////////////////////////////////////////////
// Class CTextMessageController.
CTextMessageController * CTextMessageController::m_pInst = NULL;

// Static.
CTextMessageController * CTextMessageController::GetInstance()
{
    // Create the singleton if not existed.
    if (NULL == m_pInst)
    {
        m_pInst = new CTextMessageController();
        if (NULL != m_pInst)
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
    if (NULL != m_pInst)
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

    if (NULL != m_pInst)
    {
        return FALSE;
    }

    LRESULT retVal = FALSE;
    switch (refObj.message)
    {
    case SIP_MSG_UPDATE_SMS:
        // Add a recieved message.
        if (m_pInst->IsSMSEnable())
        {
            backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            retVal = m_pInst->ProcessIntanceMessage(refObj);
        }
        break;

    case SIP_MSG_SEND_SMS_RESULT:
        // 处理消息发送后返回的结果.
        retVal = m_pInst->ProcessTextMessageSend(refObj);
        break;

    case TM_TIMER:
        retVal = m_pInst->HandleTimeoutMessage(refObj);
        break;

    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (refObj.wParam != ST_SMS)
            {
                return 0;
            }

            SETTING_INFO("TextMessage: Receive [CONFIG_MSG_BCAST_CHANGED] wParam[ST_SMS] lParam[%d]",
                         refObj.lParam);
            if (0 == refObj.lParam)
            {
                // 立即保存更改到文件中
                retVal = m_pInst->SaveConfigFile();
            }
            else if (1 == refObj.lParam)
            {
                // 重新下载短信文件
                retVal = m_pInst->ReloadConfigFile(false);
            }
            else if (2 == refObj.lParam)
            {
                retVal = TRUE;
                if (!m_pInst->IsSMSEnable())
                {
                    m_pInst->RemoveAllMessage();
                    m_pInst->SaveConfigFile();
                }
            }
        }
        break;

    default:
        break;
    }

    return retVal;
}

int CTextMessageController::AddDraftMessage(const TextMsgSendItemData & refData)
{
    // Add a draft message
    return AddMessage(refData, TEXT_MSG_GROUP_DRAFT, CTextMessageController::STATUS_UNREAD);
}

bool CTextMessageController::SendTheMessage(const TextMsgSendItemData & refData)
{

    // First add a message to the sending group.
    bool isSucceed = false;
    int nIndex = AddMessage(refData, TEXT_MSG_GROUP_SENDING,
                            CTextMessageController::STATUS_UNREAD);
    if (nIndex >= 0)
    {
        // Use selected account to send text message.
        if (acc_IsAccountIDValid(refData.m_nFromAccountId))
        {
            // 规范化接收方.
            yl::string strTo;
            if ("sip:" != refData.m_strTo.substr(0, 4))
            {
                strTo.append("sip:");
            }

            size_t uFoundPos = refData.m_strTo.find("@");
            if (yl::string::npos != uFoundPos)
            {
                strTo.append(refData.m_strTo);

                if (yl::string::npos == refData.m_strTo.find(":"))
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

#ifndef _WIN32
            _SIPSender.SendTextMessage(refData.m_nFromAccountId, refData.m_strText.c_str(), strTo.c_str());
#endif

        }
    }

    return isSucceed;
}

CTextMessageController::CTextMessageController()
    : m_nMsgDetailIndex(0)
    , m_isDirty(false)
{

}

CTextMessageController::~CTextMessageController()
{

}

void CTextMessageController::MyInit()
{
    m_bCheckFirst = true;

    // Instantiate the model instance.

    // Register message.
    etl_RegisterMsgHandle(SIP_MSG_UPDATE_SMS,  SIP_MSG_UPDATE_SMS, OnMessage);
    etl_RegisterMsgHandle(SIP_MSG_SEND_SMS_RESULT, SIP_MSG_SEND_SMS_RESULT, OnMessage);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnMessage);

    m_mapUnRead.clear();

    // Load text messages from configure file.
    MyLoadConfigFile();

    // Check un-read message.
    MyCheckUnRead();

    ProcessTextMessageChange();
}

void CTextMessageController::MyFinal()
{
    // Release the model instance.
    // Unregister message.
    etl_UnregisterMsgHandle(SIP_MSG_UPDATE_SMS, SIP_MSG_UPDATE_SMS, OnMessage);
    etl_UnregisterMsgHandle(SIP_MSG_SEND_SMS_RESULT, SIP_MSG_SEND_SMS_RESULT, OnMessage);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnMessage);
    MySaveConfigFile();
}

LRESULT CTextMessageController::OnReceivedMessage(msgObject & objMessage)
{
    return FALSE;
}

void CTextMessageController::SetTempTextMsgDetailData(const TextMsgItemDetailData & stMsgDetail)
{
    m_stMsgDetail = stMsgDetail;
}

TextMsgItemDetailData CTextMessageController::GetTempTextMsgDetailData()
{
    return m_stMsgDetail;
}

void CTextMessageController::ResetTempTextMsgDetailData()
{
    m_stMsgDetail.Reset();
}

void CTextMessageController::SetTempDataIndex(int nIndex)
{
    m_nMsgDetailIndex = nIndex;
}

int CTextMessageController::GetTempDataIndex()
{
    return m_nMsgDetailIndex;
}

bool CTextMessageController::GetMessageList(TextMsgSimpListData * pData,
        int nType) const
{
    // 参数检查.
    if (NULL == pData
            || nType <= TEXT_MSG_GROUP_TYPE_NONE
            || nType >= TEXT_MSG_GROUP_TYPE_MAX)
    {
        return false;
    }

    // 获取下标起始值.
    int nIndexFrom = pData->m_nIndexFrom;
    int nIndexTo = 0;
    if (pData->m_nCountToGet <= 0)
    {
        nIndexTo = m_listItem.size() + nIndexFrom;
    }

    // 遍历并返回指定下标的信息列表.
    TextMsgItemSimpData stTMISD;
    int i = -1;
    ItemList::const_iterator citer = m_listItem.begin();
    for (; citer != m_listItem.end(); ++citer)
    {
        const ItemData * pItemData = *citer;
        if (NULL != pItemData && pItemData->m_nGType == nType)
        {
            ++i;
            if (i >= nIndexFrom
                    && i < nIndexTo
                    && MyID2TMSD(&stTMISD, *pItemData))
            {
                pData->m_listMessage.push_back(stTMISD);
            }
        }
    }  // for.

    // 获取结果总数.
    pData->m_nTotalCount = i + 1;
    return true;
}

int CTextMessageController::GetFirstUnReadInMessageIndex() const
{
    int i = -1;

    // 获取第一个接收到的第一个未读信息.
    for (ItemList::const_iterator citer = m_listItem.begin();
            citer != m_listItem.end();
            ++citer)
    {
        const ItemData * pItemData = *citer;
        if (NULL != pItemData
                && TEXT_MSG_GROUP_IN == pItemData->m_nGType)
        {
            ++i;

            if (STATUS_UNREAD == pItemData->m_eSType)
            {
                break;
            }
        }
    }  // for.

    return i;
}

bool CTextMessageController::GetAccountMsgList(AccountTextMsgListData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    MAP_ACCOUNT_NAME mapAccountName;
    AccountTextMsgData stATMD;
    for (int i = 0; i < knMaxAccountId; ++i)
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
            stATMD.m_nUnReadCount = MyGetAccountUnRead(acc_GetUsername(i));

            mapAccountName.insert(acc_GetUsername(i), true);
        }

        pData->m_vecMsg.push_back(stATMD);
    }  // for.

    // 遍历map, 查找所有的未读信息.
    for (UnReadMap::iterator citer = m_mapUnRead.begin();
            citer != m_mapUnRead.end();
            ++citer)
    {
        if (mapAccountName.find(citer->first) == mapAccountName.end())
        {
            stATMD.m_strAccountDisplayName = citer->first;
            stATMD.m_nUnReadCount = citer->second;
            pData->m_vecMsg.push_back(stATMD);
        }
    }

    return true;
}

bool CTextMessageController::GetMessageDetail(TextMsgItemDetailData * pData,
        int nType,
        int nIndex)
{
    // 查找租中指定序号的信息数据.
    ItemData * pItemData = MyFindItem(nType, nIndex);
    if (NULL != pItemData)
    {
        // 转换数据.
        MyID2TMIDD(pData, *pItemData);

        // 如果为接收信息, 则更改接收信息的数量, 并将
        // 未读信息更改为已读信息.
        if (TEXT_MSG_GROUP_IN == pItemData->m_nGType
                && STATUS_UNREAD == pItemData->m_eSType)
        {
            int nCountBefore = MyGetTotalUnRead();

            MyAddUnReadToAccount(pItemData->m_strToUserName, -1);
            pItemData->m_eSType = STATUS_READ;

            // 如果读完之后, 未读信息变为0, 则取消未读信息状态.
            int nCountCur = MyGetTotalUnRead();
            if (nCountBefore != nCountCur
                    && 0 == nCountCur)
            {
                idleScreen_DropStatus(PS_STATE_TEXTMESSAGE);
                ProcessTextMessageChange();

                // 关灯.
#if defined(_T4X)
                etl_NotifyApp(true, POWER_LED_TEXT_MESSAGE, false, 0);
#else
                ledlamp_PostEvent(LAMP_EVENTTYPE_MESSAGE, false);
#endif
            }
            else if (nCountCur > 0)
            {
                idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, commonAPI_FormatString("%d", nCountCur));
#ifndef _WIN32
#warning
#endif
                //              idleScreen_ClearNotifyTypeFlag(PS_STATE_TEXTMESSAGE, SNT_POPUP);
            }

            // 保存.
            MySetDirty(true);
        }

        return true;
    }

    return false;
}

int CTextMessageController::AddMessage(const TextMsgSendItemData & refItem,
                                       int nGType,
                                       StatusType eSType)
{
    if (nGType <= TEXT_MSG_GROUP_TYPE_NONE || nGType >= TEXT_MSG_GROUP_TYPE_MAX
            || eSType <= STATUS_TYPE_NONE || eSType >= STATUS_TYPE_MAX)
    {
        return -1;
    }

    // 插入信息.
    int nIndex = -1;
    ItemData stID;
    if (MyTMDID2ID(&stID, nGType, eSType, refItem))
    {
        nIndex = AddDetailMessage(stID);
    }

    return nIndex;
}

bool CTextMessageController::UpdateMessageGType(int nGType,
        int nIndex,
        int nGNewType)
{
    // 查找租中指定序号的信息数据.
    ItemData * pItemData = MyFindItem(nGType, nIndex);
    if (NULL != pItemData)
    {
        // 更改组类型.
        //pItemData->m_nGType = nGNewType;
        ItemData pNewItemData;
        pNewItemData.m_nGType = nGNewType;
        pNewItemData.m_strFromName = pItemData->m_strFromName;
        pNewItemData.m_strFromUserName = pItemData->m_strFromUserName;
        pNewItemData.m_strFromServer = pItemData->m_strFromServer;
        pNewItemData.m_strToName = pItemData->m_strToName;
        pNewItemData.m_strToUserName = pItemData->m_strToUserName;
        pNewItemData.m_strToServer = pItemData->m_strToServer;
        pNewItemData.m_strText = pItemData->m_strText;
        pNewItemData.m_tTime = pItemData->m_tTime;
        pNewItemData.m_eSType = pItemData->m_eSType;

        RemoveMessage(nGType, nIndex);

        MyCheckUnRead();
        // insert box
        MyInsertItem(pNewItemData);

        MySetDirty(true);
        return true;
    }

    return false;
}

bool CTextMessageController::RemoveMessage(int nType, int nIndex)
{
    // 遍历信息列表, 从中删除指定组中下标对应的项.
    int i = -1;  // 记录成员在组中的下标.
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        ItemData * pItemData = *iter;
        if (NULL != pItemData && pItemData->m_nGType == nType)
        {
            ++i;
            if (i == nIndex)
            {
                // 如果为未读接收信息, 需要更改数量.
                if (TEXT_MSG_GROUP_IN == pItemData->m_nGType
                        && STATUS_UNREAD == pItemData->m_eSType)
                {
                    MyAddUnReadToAccount(pItemData->m_strToUserName, -1);
                    // 处理未读短信提示
                    ProcessTextMessageChange();
                }

                delete *iter;
                *iter = NULL;
                m_listItem.erase(iter);

                MyCheckUnRead();
                MySetDirty(true);
                return true;
            }
        }
    }  // for.

    return false;
}

bool CTextMessageController::RemoveMessageList(int nType, const IntList & listIndex)
{
    int i = -1;
    bool isDirty = false;
    IntList listCpy = listIndex;
    ItemList::iterator iter = m_listItem.begin();
    // 遍历信息列表, 从中删除指定组中下标对应的项.
    while (iter != m_listItem.end())
    {
        if (0 == listCpy.size())
        {
            break;
        }

        bool isRemoved = false;
        ItemData * pItemData = *iter;
        if (NULL != pItemData && pItemData->m_nGType == nType)
        {
            ++i;
            for (IntList::iterator iterIndex = listCpy.begin();
                    iterIndex != listCpy.end();
                    ++iterIndex)
            {
                if (i == *iterIndex)
                {
                    // 如果为未读接收信息, 需要更改数量.
                    if (TEXT_MSG_GROUP_IN == pItemData->m_nGType
                            && STATUS_UNREAD == pItemData->m_eSType)
                    {
                        MyAddUnReadToAccount(pItemData->m_strToUserName, -1);
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
        MyCheckUnRead();
        MySetDirty(true);
    }
    return true;
}

bool CTextMessageController::RemoveAllMessage(int nType)
{
    bool bInbox = false;
    // 遍历, 并删除指定组中的所有信息.
    ItemList::iterator iter = m_listItem.begin();
    while (iter != m_listItem.end())
    {
        ItemData * pItemData = *iter;
        if (NULL != pItemData)
        {
            if (pItemData->m_nGType == nType)
            {
                // 如果为未读接收信息, 需要更改数量.
                if (TEXT_MSG_GROUP_IN == pItemData->m_nGType
                        && STATUS_UNREAD == pItemData->m_eSType)
                {
                    bInbox = true;
                    MyAddUnReadToAccount(pItemData->m_strToUserName, -1);
                }

                delete *iter;
                *iter = NULL;
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

    MyCheckUnRead();
    MySetDirty(true);

    // 处理未读短信提示
    if (bInbox)
    {
        ProcessTextMessageChange();
    }
    return true;
}

bool CTextMessageController::RemoveAllMessage(bool bCheck /*= true*/)
{
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();)
    {
        ItemData * pItemData = *iter;
        if (pItemData == NULL)
        {
            continue;
        }
        // 如果为未读接收信息, 需要更改数量.
        if (TEXT_MSG_GROUP_IN == pItemData->m_nGType
                && STATUS_UNREAD == pItemData->m_eSType)
        {
            MyAddUnReadToAccount(pItemData->m_strToUserName, -1);
        }

        delete *iter;
        *iter = NULL;
        iter = m_listItem.erase(iter);
    }

    if (bCheck)
    {
        // 处理未读短信提示
        ProcessTextMessageChange();
    }
    return true;
}

int CTextMessageController::AddDetailMessage(const ItemData & refItem)
{
    int nIndex = MyInsertItem(refItem);
    if (nIndex > -1)
    {
        // 如果插入成功, 且未接收的消息, 则提示idle.
        if (TEXT_MSG_GROUP_IN == refItem.m_nGType
                && STATUS_UNREAD == refItem.m_eSType)
        {
            int nCount = GetTotalUnRead();
            yl::string strCount = commonAPI_FormatString("%d", nCount);
            idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, strCount);
            // 有通话时不播放提示音
            if (!talklogic_SessionExist(true))
            {
                // 响铃.
                voice_PlayTone(TONE_TEXT_MSG, 500);
            }
            // 亮灯.
#if defined(_T4X)
            etl_NotifyApp(true, POWER_LED_TEXT_MESSAGE, true, 0);
#else
            ledlamp_PostEvent(LAMP_EVENTTYPE_MESSAGE, true);
#endif
        }

        MySetDirty(true);
    }

    return nIndex;
}

int CTextMessageController::GetTotalUnRead()
{
    return MyGetTotalUnRead();
}

int CTextMessageController::GetAccountUnRead(int nAccountId)
{
    // 根据id获取账号.
    return MyGetAccountUnRead(acc_GetUsername(nAccountId));
}

bool CTextMessageController::HandleTimeoutMessage(msgObject & refMessage)
{
    if (TM_TIMER != refMessage.message
            || refMessage.wParam != kuTimerId)
    {
        return false;
    }

    // 保存.
    MySaveConfigFile();
    return true;
}

// 立刻保存文件
bool CTextMessageController::SaveConfigFile()
{
    MySetDirty(TRUE);

    // 立即保存更改到文件中
    return MySaveConfigFile();
}

// 重新加载配置
bool CTextMessageController::ReloadConfigFile(bool bCheck /*= true*/)
{
    // 删除旧的消息
    RemoveAllMessage(bCheck);

    // 重新下载短信文件
    return MyLoadConfigFile();
}

bool CTextMessageController::MyLoadConfigFile()
{
    // Get the xml file's full path.
    yl::string strFullPath(CONFIG_DATA_PATH);
    if (strFullPath.substr(strFullPath.length() - 1) != "/")
    {
        strFullPath.append("/");
    }
    strFullPath.append(kszConfigureFileName);

    // Parse the xml file.
    xml_document doc;
    if (!doc.load_file(strFullPath.c_str()))
    {
        return false;
    }

    // Find root node.
    xml_node nodeRoot = doc.child(kszNodeRoot);
    if (!nodeRoot)
    {
        return false;
    }

    xml_node nodeChild = nodeRoot.child(kszNodeItem);
    if (!nodeChild)
    {
        return false;
    }

    bool isSucceed = false;
    ItemData stID;
    while (nodeChild)
    {
        stID.Reset();

        // Parse the attribute values.
        stID.m_strFromName = nodeChild.attribute(kszAttrFromName).value();
        stID.m_strFromUserName = nodeChild.attribute(kszAttrFromUserName).value();
        stID.m_strFromServer = nodeChild.attribute(kszAttrFromServer).value();
        stID.m_strToName = nodeChild.attribute(kszAttrToName).value();
        stID.m_strToUserName = nodeChild.attribute(kszAttrToUserName).value();
        stID.m_strToServer = nodeChild.attribute(kszAttrToServer).value();
        stID.m_strText = nodeChild.attribute(kszAttrText).value();
        stID.m_tTime = nodeChild.attribute(kszAttrTime).as_int();
        stID.m_nGType = nodeChild.attribute(kszAttrGroupType).as_int();
        stID.m_eSType = static_cast<StatusType>(nodeChild.attribute(kszAttrStatusType).as_int());

        // Add message, but not notify, when all data loaded, we
        // notify.
        if (MyInsertItem(stID) >= 0)
        {
            isSucceed = true;
        }

        nodeChild = nodeChild.next_sibling();
    }  // while

    return isSucceed;
}

bool CTextMessageController::MySaveConfigFile()
{
    if (!MyGetDirty())
    {
        return false;
    }

    SETTING_INFO("Text message: save xml begin.");
    // Declare the xml instance
    xml_document doc;
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");
    xml_node nodeRoot = doc.append_child(kszNodeRoot);
    if (!nodeRoot)
    {
        return false;
    }

    // Add root node and iterate to add child nodes.
    ItemList::ListIterator citer = m_listItem.begin();
    for (; citer != m_listItem.end(); ++citer)
    {
        if (const ItemData * pItemData = *citer)
        {
            // Add child node.
            xml_node nodeChild = nodeRoot.append_child(kszNodeItem);
            if (!nodeChild)
            {
                return false;
            }

            // Add child node.
            nodeChild.append_attribute(kszAttrFromName, pItemData->m_strFromName.c_str());
            nodeChild.append_attribute(kszAttrFromUserName, pItemData->m_strFromUserName.c_str());
            nodeChild.append_attribute(kszAttrFromServer, pItemData->m_strFromServer.c_str());
            nodeChild.append_attribute(kszAttrToName, pItemData->m_strToName.c_str());
            nodeChild.append_attribute(kszAttrToUserName, pItemData->m_strToUserName.c_str());
            nodeChild.append_attribute(kszAttrToServer, pItemData->m_strToServer.c_str());
            nodeChild.append_attribute(kszAttrText, pItemData->m_strText.c_str());
            nodeChild.append_attribute(kszAttrTime, (int)pItemData->m_tTime);
            nodeChild.append_attribute(kszAttrGroupType, pItemData->m_nGType);
            nodeChild.append_attribute(kszAttrStatusType, (int)pItemData->m_eSType);
        }
    }  // for.

    // Get full xml path.
    yl::string strFullPath(CONFIG_DATA_PATH);
    if (strFullPath.substr(strFullPath.length() - 1) != "/")
    {
        strFullPath.append("/");
    }
    strFullPath.append(kszConfigureFileName);

    // Save to file.
    bool isSucceed = doc.save_file(strFullPath.c_str());

    if (isSucceed)
    {
        MySetDirty(false);
        timerKillThreadTimer(kuTimerId);
    }
    SETTING_INFO("Text message: save xml end.");

    return isSucceed;
}

int CTextMessageController::MyInsertItem(const ItemData & refItem)
{
    if (refItem.m_nGType <= TEXT_MSG_GROUP_TYPE_NONE
            || refItem.m_nGType >= TEXT_MSG_GROUP_TYPE_MAX
            || refItem.m_eSType <= STATUS_TYPE_NONE
            || refItem.m_eSType >= STATUS_TYPE_MAX)
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
        const ItemData * pItemData = *iter;
        if (NULL != pItemData && pItemData->m_nGType == refItem.m_nGType)
        {
            ++nIndex;

            // 如果超过上限, 则删除多余的.
            if (nIndex >= knMaxMsgCountPerGroup - 1)
            {
                // 如果是未读接收消息, 则在删除之前更新数量.
                if (TEXT_MSG_GROUP_IN == pItemData->m_nGType
                        && STATUS_UNREAD == pItemData->m_eSType)
                {
                    MyAddUnReadToAccount(pItemData->m_strToUserName, -1);
                }

                delete *iter;
                *iter = NULL;
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
        nIndex = (int)m_listItem.size();
    }

    // 插入信息.
    ItemData * pNew = new ItemData();
    if (pNew != NULL)
    {
        *pNew = refItem;
        m_listItem.insert(iterLoc, pNew);

        if (TEXT_MSG_GROUP_IN == pNew->m_nGType
                && STATUS_UNREAD == pNew->m_eSType)
        {
            MyAddUnReadToAccount(pNew->m_strToUserName, 1);
        }
    }

    return nIndex;
}

CTextMessageController::ItemData * CTextMessageController::MyFindItem(
    int nGType, int nIndex)
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
        ItemData * pItemData = *iter;
        if (NULL != pItemData && pItemData->m_nGType == nGType)
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

const CTextMessageController::ItemData * CTextMessageController::MyFindItem(
    int nGType, int nIndex) const
{
    // 参数检查.
    if (nGType <= TEXT_MSG_GROUP_TYPE_NONE
            || nGType >= TEXT_MSG_GROUP_TYPE_MAX || nIndex < 0)
    {
        return NULL;
    }

    // 遍历查找到信息.
    int i = -1;
    for (ItemList::const_iterator citer = m_listItem.begin();
            citer != m_listItem.end();
            ++citer)
    {
        const ItemData * pItemData = *citer;
        if (NULL != pItemData && pItemData->m_nGType == nGType)
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

yl::string CTextMessageController::MyGetIconPath(int nGType,
        StatusType eSType) const
{
    // 获取目录路径.
    yl::string strRet;

    // 获取各个图片.
    switch (nGType)
    {
    case TEXT_MSG_GROUP_IN:
        {
            switch (eSType)
            {
            case STATUS_UNREAD:
                {
                    return PIC_GENERAL_MESSAGE_UNREAD;
                }
                break;
            case STATUS_READ:
                {
                    return PIC_GENERAL_MESSAGE_READ;
                }
                break;
            default:
                break;
            }
        }
        break;
    case TEXT_MSG_GROUP_DRAFT:
        {
            return PIC_GENERAL_MESSAGE_DRAFT;
        }
        break;
    case TEXT_MSG_GROUP_SENDING:
        {
            return PIC_GENERAL_MESSAGE_SENDING;
        }
        break;
    case TEXT_MSG_GROUP_SENT:
        {
            return PIC_GENERAL_MESSAGE_SEND;
        }
        break;
    default:
        break;
    }
    return "";
}

yl::string CTextMessageController::MyGetAccountName(
    const yl::string & strUserName) const
{
    // 获取账号列表.
    return acc_GetDisplayName(acc_GetAccountIDByName(strUserName.c_str()));
}

bool CTextMessageController::MyID2TMSD(TextMsgItemSimpData * pData,
                                       const ItemData & refItem) const
{
    if (NULL == pData)
    {
        return false;
    }

    // 拷贝名字.
    yl::string strDisplay;  // 显示名.
    switch (refItem.m_nGType)
    {
    case TEXT_MSG_GROUP_IN:
        strDisplay = MyGetDisplayName(refItem.m_strFromUserName,
                                      refItem.m_strFromServer, false);
        break;

    case TEXT_MSG_GROUP_DRAFT:
    case TEXT_MSG_GROUP_SENDING:
    case TEXT_MSG_GROUP_SENT:
        strDisplay = MyGetDisplayName(refItem.m_strToUserName,
                                      refItem.m_strToServer, false);
        break;

    default:
        break;
    }

    // 从账号中查找账号名.
    pData->m_strName = strDisplay;

    // 拷贝时间.
    pData->m_strTime = Timet2String(refItem.m_tTime);

    // 获取图片(全路径).
    pData->m_strIconPath = MyGetIconPath(refItem.m_nGType, refItem.m_eSType);

    // 拷贝类型.
    pData->m_eGType = static_cast<TextMsgGroupType>(refItem.m_nGType);

    return true;
}

bool CTextMessageController::MyID2TMIDD(TextMsgItemDetailData * pData,
                                        const ItemData & refItem) const
{
    if (NULL != pData)
    {
        yl::string strFromName;  // 发送者名.
        yl::string strToName;  // 接收者名.

        // 根据类型获取发送方和接收方的显示名.
        switch (refItem.m_nGType)
        {
        case TEXT_MSG_GROUP_IN:
            strFromName = MyGetDisplayName(refItem.m_strFromUserName,
                                           refItem.m_strFromServer, false);
            strToName = MyGetDisplayName(refItem.m_strToUserName,
                                         refItem.m_strToServer, true);
            break;

        case TEXT_MSG_GROUP_DRAFT:
        case TEXT_MSG_GROUP_SENDING:
        case TEXT_MSG_GROUP_SENT:
            strFromName = MyGetDisplayName(refItem.m_strFromUserName,
                                           refItem.m_strFromServer, true);
            strToName = MyGetDisplayName(refItem.m_strToUserName,
                                         refItem.m_strToServer, false);
            break;

        default:
            break;
        }

        // 拷贝From.
        pData->m_strFromName = strFromName;
        pData->m_strFromUserName = refItem.m_strFromUserName;
        pData->m_strFromServer = refItem.m_strFromServer;

        // 拷贝to.
        pData->m_strToName = strToName;
        pData->m_strToUserName = refItem.m_strToUserName;
        pData->m_strToServer = refItem.m_strToServer;

        // 拷贝信息内容.
        pData->m_strText = refItem.m_strText;

        // 获取图片(全路径).
        pData->m_strIconPath = MyGetIconPath(refItem.m_nGType, refItem.m_eSType);

        // 拷贝时间.
        ////----luzh 翻译方案
        //pData->m_strTime = SettingUILogic_Time2String(refItem.m_tTime);
        pData->m_strTime = refItem.m_tTime;

        // 拷贝类型.
        pData->m_eGType = static_cast<TextMsgGroupType>(refItem.m_nGType);

        return true;
    }

    return false;
}

bool CTextMessageController::MyTMDID2ID(ItemData * pData,
                                        int nGType,
                                        StatusType eSType,
                                        const TextMsgSendItemData & refItem) const
{
    if (NULL != pData && nGType > TEXT_MSG_GROUP_TYPE_NONE
            && nGType < TEXT_MSG_GROUP_TYPE_MAX
            && eSType > STATUS_TYPE_NONE && eSType < STATUS_TYPE_MAX)
    {
        // 获取发送者信息.
        if (acc_IsAccountIDValid(refItem.m_nFromAccountId))
        {
            pData->m_strFromName = acc_GetDisplayName(refItem.m_nFromAccountId);
            pData->m_strFromUserName = acc_GetUsername(refItem.m_nFromAccountId);
            pData->m_strFromServer = acc_GetServerName(refItem.m_nFromAccountId);
        }

        // 拷贝数据.
        pData->m_strToName = refItem.m_strTo;

        // 是否有发server.
        size_t uPosFound = refItem.m_strTo.find("@");
        if (uPosFound != yl::string::npos)
        {
            pData->m_strToUserName = refItem.m_strTo.substr(0, uPosFound);
            pData->m_strToServer = refItem.m_strTo.substr(uPosFound + 1);
        }
        else
        {
            pData->m_strToUserName = refItem.m_strTo;
            // 如果没指定服务器, 则和发送方在同一服务器中.
            pData->m_strToServer = pData->m_strFromServer;
        }

        pData->m_strText = refItem.m_strText;
        pData->m_nGType = nGType;
        pData->m_eSType = eSType;
        pData->m_tTime = refItem.m_tTime;
        return true;
    }

    return false;
}

yl::string CTextMessageController::MyGetDisplayName(const yl::string & strUser,
        const yl::string & strServer,
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
        strRet = acc_GetDisplayName(acc_GetAccountIDByFullName(strFullName.c_str()));
    }
    else
    {
        // 如果不是自己, 则从联系人中查找名字.
        Dir_GetCotactNameByNumber(strUser, strRet, LOCAL_DIR);
    }

    return strRet.empty() ? strUser : strRet;
}

bool CTextMessageController::MyAddUnReadToAccount(
    const yl::string & strUserName, int nAdd)
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

int CTextMessageController::MyGetAccountUnRead(const yl::string & strUserName)
{
    // 获取账号的未读信息数.
    UnReadMap::iterator citer = m_mapUnRead.find(strUserName);
    return citer != m_mapUnRead.end() ? citer->second : 0;
}

int CTextMessageController::MyGetTotalUnRead()
{
    // 遍历map统计所有未读信息数.
    int nTotal = 0;
    for (UnReadMap::iterator citer = m_mapUnRead.begin();
            citer != m_mapUnRead.end();
            ++citer)
    {
        nTotal += citer->second;
    }
    return nTotal;
}

void CTextMessageController::MyCheckUnRead()
{
    int nTotal = MyGetTotalUnRead();
    if (0 == nTotal)
    {
        idleScreen_DropStatus(PS_STATE_TEXTMESSAGE);
        // 关灯.
#if defined(_T4X)
        etl_NotifyApp(true, POWER_LED_TEXT_MESSAGE, false, 0);
#else
        ledlamp_PostEvent(LAMP_EVENTTYPE_MESSAGE, false);
#endif
    }
    else if (nTotal > 0)
    {
        int nCount = GetTotalUnRead();
        MESSAGEUI_INFO("------------- unRead[%d] ---------", nCount);
        yl::string strCount = commonAPI_FormatString("%d", nCount);
        idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, strCount);
        // 开灯.
#if defined(_T4X)
        etl_NotifyApp(true, POWER_LED_TEXT_MESSAGE, true, 0);
#else
        ledlamp_PostEvent(LAMP_EVENTTYPE_MESSAGE, true);
#endif
    }
}

void CTextMessageController::MySetDirty(bool isDirty)
{
    if (m_isDirty != isDirty)
    {
        m_isDirty = isDirty;

        if (m_isDirty)
        {
            timerSetThreadTimer(kuTimerId, knIntervalTimeout);
        }
    }
}

bool CTextMessageController::MyGetDirty() const
{
    return m_isDirty;
}

// 处理TextMessage消息
bool CTextMessageController::ProcessIntanceMessage(msgObject & objMessage)
{
    if (!IsSMSEnable())
    {
        return TRUE;
    }

    // 取信息附加块
    sip_sms_info_t * pSMSInfo = (sip_sms_info_t *)objMessage.GetExtraData();
    if (NULL == pSMSInfo)
    {
        return FALSE;
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

    // Get the xml file's full path.
    yl::string strFullPath(CONFIG_DATA_PATH);
    if (strFullPath.substr(strFullPath.length() - 1) != "/")
    {
        strFullPath.append("/");
    }
    strFullPath.append(kszConfigureFileName);

    // Parse the xml file.
    xml_document doc;
    if (!doc.load_file(strFullPath.c_str()))
    {
        return false;
    }

    // Find root node.
    xml_node nodeRoot = doc.child(kszNodeRoot);
    if (!nodeRoot)
    {
        return false;
    }

    xml_node nodeChild = nodeRoot.append_child(kszNodeItem);
    nodeChild.append_attribute(kszAttrFromName, fromUserInfo.strDisplay.c_str());
    nodeChild.append_attribute(kszAttrFromUserName, fromUserInfo.strNumber.c_str());
    nodeChild.append_attribute(kszAttrFromServer, fromUserInfo.strServer.c_str());
    nodeChild.append_attribute(kszAttrToName, toUserInfo.strDisplay.c_str());
    nodeChild.append_attribute(kszAttrToUserName, toUserInfo.strNumber.c_str());
    nodeChild.append_attribute(kszAttrToServer, toUserInfo.strServer.c_str());

    // yzh 这里解析文字信息内容
    xml_document docText;
    yl::string strText(pSMSInfo->xml_body);
    if (!docText.load_buffer(pSMSInfo->xml_body, strlen(pSMSInfo->xml_body)))
    {
        return FALSE;
    }

    nodeChild.append_attribute(kszAttrText, strText.c_str());
    nodeChild.append_attribute(kszAttrStatusType, STATUS_UNREAD);
    nodeChild.append_attribute(kszAttrGroupType, TEXT_MSG_GROUP_IN);
    time_t timeMsg = netGetLocalTime();
    nodeChild.append_attribute(kszAttrTime, timeMsg);
    doc.save_file(strFullPath.c_str());

    ItemData itemData;
    itemData.m_strFromName = fromUserInfo.strDisplay;
    itemData.m_strFromServer = fromUserInfo.strServer;
    itemData.m_strFromUserName = fromUserInfo.strNumber;
    itemData.m_strText = strText;
    itemData.m_strToName = toUserInfo.strDisplay;
    itemData.m_strToServer = toUserInfo.strServer;
    itemData.m_strToUserName = toUserInfo.strNumber;
    itemData.m_tTime = timeMsg;
    itemData.m_eSType = STATUS_UNREAD;
    itemData.m_nGType = TEXT_MSG_GROUP_IN;
    MyInsertItem(itemData);

    SETTING_INFO("the number of message is = %d", GetTotalUnRead());
    ProcessTextMessageChange();

    return TRUE;
}

// 处理TextMessage未读消息
bool CTextMessageController::ProcessTextMessageChange()
{
    int iUnReadNum = GetTotalUnRead();
    SETTING_INFO("ProcessTextMessageChange iUnReadNum[%d]", iUnReadNum);
    int iRet = 1;
    // wParam: unread num, lParam: 1=show, 0=hide
    if (iUnReadNum > 0)
    {
        if (!talklogic_SessionExist())
        {
            voice_PlayTone(TONE_TEXT_MSG, 300);
            if (m_bCheckFirst)
            {
                etlSleep(300);
                // 上电检测会占用较多CPU，未读提示音的定时器会无法被处理，导致声音响太久，
                // 此处延迟后再停止响提示音
                voice_StopTone(TONE_TEXT_MSG);
                m_bCheckFirst = FALSE;
            }
        }
        idleScreen_SetStatus(PS_STATE_TEXTMESSAGE, commonAPI_FormatString("%d", iUnReadNum));
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_TEXTMESSAGE);
    }

    etl_NotifyApp(false, POWER_LED_TEXT_MESSAGE, false, 0);

    return iRet;
}

// 处理TextMessage发送消息
bool CTextMessageController::ProcessTextMessageSend(msgObject & objMessage)
{
    if (SIP_MSG_SEND_SMS_RESULT != objMessage.message
            || NULL == objMessage.GetExtraData())
    {
        return false;
    }

    sip_reason_info_t * pResultInfo = (sip_reason_info_t *)objMessage.GetExtraData();

    bool isSucceed = false;
    // 发送成功, 则将消息组更改为已发送.
    // 发送不成功, 不做更改(已在发送中了).
    if (0 == pResultInfo->code)
    {
        isSucceed = UpdateMessageGType(TEXT_MSG_GROUP_SENDING,
                                       0, TEXT_MSG_GROUP_SENT);
    }

    return isSucceed ? true : false;
}

// SMS是否可用
bool CTextMessageController::IsSMSEnable()
{
    return (1 == configParser_GetConfigInt(kszSMSEnable));
}

