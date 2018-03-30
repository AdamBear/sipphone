#if FEATURE_BROADTOUCH_UC
#include "uccontactdata.h"
#include "xmlparser.hpp"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "requestrosteraction.h"
#include "devicelib/networkserver.h"

#include <sys/time.h>
//#include <fstream>
#include <signal.h>

#include "modcommon.h"
#include "contacts/directory/include/directorymessagedefine.h"

#include "threadtask/include/modthreadtask.h"
#include "commonunits/modcommonunits.h"

#define  BUF_SIZE   32768
//test to do -syl
#define  TA_PRESENCE_REQUSET 10
//iq节点
const char * const kszNodeNameIq = "iq";

const char * const kszNodeNameQuery = "query"; //query节点
const char * const kszNodeContactStorage = "contact_storage"; //contact_storage节点
const char * const kszNodeNameItem = "item";
const char * const kszAttributeNameID = "jid"; //Presence中的关键字Jid属性
const char * const kszAttributeCommunication = "communication";
const char * const kszAttributeNameSubscription = "subscription";
const char * const kszAttributeNameName = "name";
const char * const kszAttributeNameAsk = "ask";
const char * const kszNodeNamePresence = "presence";
const char * const kszAttributeNameFrom = "from";
const char * const kszNodeNameShow = "show";
const char * const kszNodeAddressBook = "addressbook"; //addressbook节点
const char * const kszNodeContacts = "contacts";
const char * const kszNodeContact = "contact";
const char * const kszNodeFavorites = "favourites"; //Favorites节点
const char * const kszNodeFavorite = "favourite"; //单个Favorite联系人节点
const char * const kszAttributeId = "id";       //匹配Favorite联系人的id属性
const char * const kszNodeNameStatus = "status";
const char * const kszNodeNameFreeText = "freeText";

const char * const kszNodeGroups = "groups"; //Groups节点
const char * const kszNodeGroup = "group"; //Group节点
const char * const kszNodeMember = "member"; //member节点

const char * const kszNodeDisplayName = "display_name";

const char * const kszNodeNameMessage = "message";
const char * const kszNodeNameEvent = "event";
const char * const kszNodeNameItems = "items";
const char * const kszNodeNameLastUpdate = "last-update";

#define TIME_REQ_FAVORITE_INTERVAL 2*60*1000

#define ksznDefaultIdBegin 20

#define UC_TRACE_DYNAMIC_LOG    0

#if UC_TRACE_DYNAMIC_LOG
#define uc_trace_path_pre  CONFIG_PATH "uc_trace/"
static int s_index = 0;
#endif

// int  push_handler(char *xml_stream, size_t stream_len);
// int  push_handler_presence(char *xml_stream, size_t stream_len);

int push_handler(char * xml_stream, size_t stream_len)
{
    if (g_pUCPresence->IsIniting())
    {
        APP_INFO("is in Inin!");
        return 0;
    }

    APP_INFO("xml push handler = %s", xml_stream);
    etl_NotifyAppEx(
        false,
        PRESENCE_CONTACT_CHANGE,
        0,
        0,
        stream_len,
        xml_stream);

    return 0;
}

int push_handler_presence(char * xml_stream, size_t stream_len)
{
    if (g_pUCPresence->IsIniting())
    {
        APP_INFO("is in Inin!");
        return 0;
    }
    APP_INFO("xml push handler presence = %s", xml_stream);
    etl_NotifyAppEx(
        false,
        PRESENCE_PRESENCE_CHANGE,
        0,
        0,
        stream_len,
        xml_stream);

    return 0;
}

void disconnect_handler(XMPP_CONN * conn, int errnono, void * disconn_private_data)
{
    if (g_pUCPresence->IsIniting())
    {
        APP_INFO("is in Inin!");
        return;
    }

    APP_INFO("disconnect_handler errnono=%d", errnono);

    if (conn != NULL)
    {
        etl_NotifyAppEx(
            false,
            PRESENCE_PRESENCE_DISCONNECT,
            errnono, 0, sizeof(XMPP_CONN *), (void *)conn);
    }
}

CUCContactData * CUCContactData::m_Instace = NULL;

CUCContactData * CUCContactData::GetInstance()
{
    if (m_Instace == NULL)
    {
        m_Instace = new CUCContactData;
    }

    return m_Instace;
}

CUCContactData::CUCContactData()
{
    m_eType = PRESENCE_UC;

    m_nPreGroupSize = -1;

    m_bIsIniting = false;

    m_bLock = false;

    m_bAllowChangePresence = false;

    m_strLastUpdateTime = "";

    m_connInfo = new XMPP_CONN_INFO();
    m_conn = new XMPP_CONN();

    if (m_connInfo != NULL)
    {
        m_connInfo->push_handler = push_handler;
        m_connInfo->push_handler_presence = push_handler_presence;
        m_connInfo->disconnect_handler = disconnect_handler;
        m_connInfo->jid = NULL;
        m_connInfo->pass = NULL;

        //m_connInfo->log_level = XMPP_LOG_LEVEL_DEBUG;
        //m_connInfo->log_method = XMPP_LOG_PRINT;
        m_connInfo->request_timeout = 3;
        // 超时时间和ping包检测时间都设置在60s
        m_connInfo->idle_timeout = 60;
        m_connInfo->idle_ping_interval = 60;
    }

    //设置日志等级
    SetXmppLog(-1, commonUnits_GetLogLevel());

#if UC_TRACE_DYNAMIC_LOG
    yl::string strCmd = fmtString("rm -rf %s", uc_trace_path_pre);
    system(strCmd.c_str());
    strCmd = fmtString("mkdir -p %s", uc_trace_path_pre);
    system(strCmd.c_str());
#endif
    commonUnits_AddSysLogFunc(CUCContactData::SetXmppLog);
}

CUCContactData::~CUCContactData()
{
    commonUnits_RemoveSysLogFunc(CUCContactData::SetXmppLog);
    // 释放资源
    delete m_connInfo;
    m_connInfo = NULL;

    delete m_conn;
    m_conn = NULL;
}

void CUCContactData::SetXmppLog(int iModuleID, int iLogLevel)
{
    APP_INFO("reset xmpp log info [%d]", iLogLevel);
    xmpp_log_set_level(iLogLevel);
}

bool CUCContactData::RefreshAllInfo()
{
    APP_INFO("CPresenceModel::RefreshAllInfo()");

    //正在更新返回失败
    if (m_bIsIniting)
    {
        return true;
    }

    if (m_conn == NULL || m_connInfo == NULL)
    {
        return false;
    }

    xmpp_close(m_conn);

    m_bLock = true;

    if (m_favoriteTimer.IsTimerRuning())
    {
        m_favoriteTimer.KillTimer();
    }

    m_objLocalPresenceData.Reset();
    etl_NotifyApp(false, PRESENCE_MESSAGE_USER_INFO_CHANGE, 0, 0);

    //先清除原先数据
    etl_NotifyApp(false, PRESENCE_MESSAGE_FAVORITE_CHANGE, 0, 1);

    MyResetContactInfo();

    ReSetId(ksznDefaultIdBegin);
    m_mapGroupKey.put(kszAllBuddiesName, ALL_BUDDIES_ID);

    m_mapGroupKey.put(kszFavourite, FAVOURITE_ID);

    m_mapContacts.clear();
    // for test
    //configParser_SetConfigInt(kszEnableUC,1);
//  configParser_SetConfigString(kszPresenceJID,"225255622@gtsce.eu.bc.im");
//  configParser_SetConfigString(kszPresencePassword,"almnabq1kit4t85gqope1qzw5zb6aw");

    // 开启uc才会继续操作
    if (configParser_GetConfigInt(kszEnableUC) == 0 || !netIsNetworkReady()
            || configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return false;
    }

    m_strUserID = configParser_GetConfigString(kszPresenceJID);

    m_strPassword = configParser_GetConfigString(kszPresencePassword);

    if (m_strUserID.empty() || m_strPassword.empty())
    {
        return false;
    }

    m_strLogId = m_strUserID;
    m_strLogId += GetConnectURI();

    m_connInfo->jid = const_cast<char *>(m_strLogId.c_str());
    m_connInfo->pass = const_cast<char *>(m_strPassword.c_str());

//  APP_INFO("--------------------------------------------------------------------------------------------------- JID = %s----------------", m_connInfo->jid);
//  APP_INFO("--------------------------------------------------------------------------------------------------- password = %s---------", m_connInfo->pass);


    CPresenceRequestAction * pAction = TaskAction_CreatePresenceRequestAction();
    if (pAction != NULL)
    {
        pAction->SetExtraInfo(m_conn, m_connInfo);
        pAction->SetSync(false);
        pAction->SetReqType(CPresenceRequestAction::REQ_INIT);
        pAction->SetRead(true);
        pAction->SetCallback(this);
        TaskAction_ExecAction(pAction);
        m_bIsIniting = true;
        return true;
    }

    return false;
}


void CUCContactData::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    APP_INFO("CPresenceModel::OnTaskActionCallback");
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != TA_PRESENCE_REQUSET)
    {
        return;
    }

    CPresenceRequestAction * pPresenceAction =
        static_cast<CPresenceRequestAction *>(pTaskAction);

    if (pPresenceAction == NULL)
    {
        return;
    }

    if (pPresenceAction->GetReqType() == CPresenceRequestAction::REQ_INIT)
    {
        //关闭初始化状态标志位
        APP_INFO("Init over [%d]", m_conn->conn_errno);
        m_bIsIniting = false;

        CheckReConnect();

        if (!pTaskAction->IsOperationSucceed())
        {
            APP_INFO("CPresenceModel::OnTaskActionCallback Error = %d", pTaskAction->GetTaskActionErrorType());

            //30秒后重连
            //g_pPresenceController->ReqAllBuddiesOnTimer();

            //释放action
            TaskAction_DestroyAction(pPresenceAction);

            return;
        }

        m_RefreshTimer.SetTimer(5 * 1000, NULL, MK_MSG_ENTRY(this, CUCContactData::OnTimeOut));

        m_favoriteTimer.SetTimer(TIME_REQ_FAVORITE_INTERVAL, NULL, MK_MSG_ENTRY(this,
                                 CUCContactData::OnFavoriteTimeOut));

        ReqAllPresence();
        //默认初始化在线状态
        if (m_bAllowChangePresence)
        {
            ReqChangPresence(_PRESENCE_TYPE_ONLINE);
        }
        else
        {
            ReqChangPresence(_PRESENCE_TYPE_OFFLINE);
        }
    }
    else if (pPresenceAction->GetReqType() == CPresenceRequestAction::REQ_CARD)
    {
        YLList<PresenceInfo> & refList = pPresenceAction->GetPresenceList();

        for (YLList<PresenceInfo>::iterator citer = refList.begin(); citer != refList.end(); citer++)
        {
            PresenceInfo & refData = *citer;

            IterMapPresenceInfo MapIter = MyFindInfoByKey(refData.strContactID);

            if (MapIter != m_mapPresenceInfo.end())
            {
                MapIter->second.strAvatarInfo = refData.strAvatarInfo;

                MapIter->second.m_bFirst = refData.m_bFirst;
            }

        }

        RefreshViewerPresence();

        //发消息通知Dsskey模块,加载头像
        etl_NotifyApp(false, PRESENCE_MESSAGE_FAVORITE_CHANGE, 0, 0);

        APP_INFO("Card Init over");
    }

    TaskAction_DestroyAction(pPresenceAction);
}

void CUCContactData::ReqAllPresence()
{
    size_t iSize = 0;
    char * pStream = NULL;
    m_cs.Lock();
    xmpp_request(m_conn, XMPP_REQUEST_PRESENCE_ALL,
                 &pStream, &iSize);

    m_cs.Unlock();
    free(pStream);
}

bool CUCContactData::RefreshRosterInfo(const yl::string & strRenameInfo, RefreshType & nRefreshType)
{
    APP_INFO("Begin RefreshRosterInfo");

    bool bRefresh = false;
    bool bFavoriteChanged = false;
    nRefreshType = REFRESH_NONE;

    xml_document objXmlDoc;
    objXmlDoc.load(strRenameInfo.c_str());

#if UC_TRACE_DYNAMIC_LOG
    yl::string strFile = fmtString("%s%05d_%s.xml", uc_trace_path_pre, ++s_index, __FUNCTION__);
    objXmlDoc.save_file(strFile.c_str());
#endif

    // check format
    xml_node nodeRoot = objXmlDoc.child(kszNodeNameIq);
    if (!nodeRoot)
    {
        APP_ERR("iq node empty!");
        //解析是否为更新联系人消息
        if (ParseUpdateMessage(objXmlDoc))
        {
            nRefreshType = REFRESH_ALL;
        }

        return false;
    }

    //Type属性不为set返回
    xml_attribute attrType = nodeRoot.attribute("type");

    if (!attrType || 0 != strcmp(attrType.as_string(), "set"))
    {
        APP_ERR("type attribute(%s) error!", (attrType ? attrType.as_string() : ""));
        return false;
    }

    xml_node nodeQuery = nodeRoot.child(kszNodeNameQuery);
    //无Query节点直接返回
    if (!nodeQuery)
    {
        APP_ERR("query node empty!");
        return false;
    }

    xml_attribute attrXmlns = nodeQuery.attribute("xmlns");
    //query节点xmlns属性不为jabber:iq:roster直接返回
    if (!attrXmlns || 0 != strcmp(attrXmlns.as_string(), "jabber:iq:roster"))
    {
        APP_ERR("xmls attribute(%s) empty!", (attrXmlns ? attrXmlns.as_string() : ""));
        return false;
    }

    xml_node nodeNameItem = nodeQuery.child(kszNodeNameItem);
    //query无item子节点直接返回
    if (!nodeNameItem)
    {
        APP_ERR("nameitem node empty!");
        return false;
    }

    PresenceInfo objItem;
    //parse xml & get data
    for (; nodeNameItem; nodeNameItem = nodeNameItem.next_sibling(kszNodeNameItem))
    {
        yl::string strAttributeJID;
        yl::string strSubScrip;

        xml_attribute attrNameID = nodeNameItem.attribute(kszAttributeNameID);
        xml_attribute attrSubScription = nodeNameItem.attribute("subscription");
        if (!attrNameID || !attrSubScription)
        {
            APP_ERR("nameid or subscription node empty!");
            continue;
        }
        // 获取JID
        strAttributeJID = attrNameID.as_string();

        int index = strAttributeJID.find("/");

        if (index != yl::string::npos)
        {
            strAttributeJID = strAttributeJID.substr(0, index);
        }

        APP_INFO("strAttributeJID = %s", strAttributeJID.c_str());

        if (strAttributeJID.empty())
        {
            APP_ERR("nameid node empty!");
            continue;
        }
        // 将JID转化为小写
        strAttributeJID = commonAPI_StringConvert(strAttributeJID, STRING_CONVERT_TO_LOWER);

        strSubScrip = attrSubScription.as_string();

        IterMapPresenceInfo iter = MyFindInfoByKey(strAttributeJID);

        objItem.Reset();
        //已存在
        if (iter != m_mapPresenceInfo.end())
        {
            if (iter->second.m_listAttribute.size() == 0
                    && (strSubScrip.compare("both") == 0
                        || strSubScrip.compare("to") == 0
                        || strSubScrip.compare("from") == 0))
            {
                nRefreshType = REFRESH_SINGLE;
            }

            if (strSubScrip.compare("both") == 0)
            {
                //修改联系人信息
                PresenceInfo & refData = iter->second;
                yl::string strDisplayName;
                xml_attribute attrName = nodeNameItem.attribute(kszAttributeNameName);

                if (attrName)
                {
                    strDisplayName = attrName.as_string();
                }

                // 容错，如果获取到联系人名称为空，则截取JID
                if (strDisplayName.empty())
                {
                    //取"@"之前的内容
                    strDisplayName = GetSubString(strAttributeJID, "", "@");
                }
                //过滤无修改退送过来的消息
                //if (strDisplayName.compare(refData.strContactName) == 0)
                if (!refData.strContactName.empty())
                {
                    continue;
                }
                refData.strContactName = strDisplayName;

                GroupKey key = 1;

                m_csSearch.Lock();
                GroupItem * pItem = GetGroupItemByID(key);

                MySort(*pItem);
                m_csSearch.Unlock();

                bRefresh = true;
                bFavoriteChanged |= IsFavoriteContact(strAttributeJID);
            }
            else if (strSubScrip.compare("remove") == 0)
            {
                bFavoriteChanged |= IsFavoriteContact(strAttributeJID);

                PresenceInfo & refData = iter->second;

                RemoveContactInGroup(refData.m_nLocalId);

                // 删除联系人
                RemoveFromMapByKey(iter->second.strContactID);



                bRefresh = true;
            }
            continue;
        }

        //添加联系人
        bool bNotAccepted = strSubScrip.compare("none") == 0;
        if (bNotAccepted
                || strSubScrip.compare("both") == 0
                || strSubScrip.compare("to") == 0)
        {
            objItem.strContactID = strAttributeJID;

            xml_attribute attrName = nodeNameItem.attribute(kszAttributeNameName);

            if (attrName)
            {
                objItem.strContactName = attrName.as_string();

                APP_INFO("name = %s", objItem.strContactName.c_str());
            }

            // 容错，如果获取到联系人名称为空，则截取JID
            if (objItem.strContactName.empty())
            {
                //取"@"之前的内容
                objItem.strContactName = GetSubString(strAttributeJID, "", "@");
            }

            if (bNotAccepted)
            {
                objItem.ePresenceType = _PRESENCE_TYPE_NONE;
            }
            else
            {
                objItem.ePresenceType = _PRESENCE_TYPE_ONLINE;      // default online
                //鉴于BroadSoft不会主动推送图片，无图片标记，需主动请求头像
                ReqContactAvatar(objItem);
            }

            ///////////////////////////////////////
            if (!IsUserJID(strAttributeJID))
            {
                InsertToMap(objItem);

                GroupKey key = ALL_BUDDIES_ID;

                m_csSearch.Lock();
                GroupItem * pItem = GetGroupItemByID(key);

                if (pItem != NULL)
                {
                    InsertIdToGroupList(objItem.strContactID, pItem->m_nIdList, true);
                    MySort(*pItem);
                }
                m_csSearch.Unlock();

                bRefresh = true;
            }
        }
    }

    // http://10.2.1.199/Bug.php?BugID=89063
    if (bFavoriteChanged)
    {
        etl_NotifyApp(false,
                      PRESENCE_MESSAGE_FAVORITE_CHANGE, 0, 0);
    }

    if (bRefresh)
    {
        MyResetSearchKey();
    }

    return bRefresh;
}

// 解析XML流内容
bool CUCContactData::ProcessPresenceStream(const yl::string & strXmlStream)
{
    APP_INFO("Begin ProcessXmlStream");
    PresenceInfo refData;

    xml_document objXmlDoc;

    objXmlDoc.load(strXmlStream.c_str());

#if UC_TRACE_DYNAMIC_LOG
    yl::string strFile = fmtString("%s%05d_%s.xml", uc_trace_path_pre, ++s_index, __FUNCTION__);
    objXmlDoc.save_file(strFile.c_str());
#endif

    xml_node nodeRoot = objXmlDoc.child(kszNodeNamePresence);

    // "presence" node
    if (!nodeRoot)
    {
        APP_ERR("presence node empty!");
        return false;
    }

    xml_attribute attrType = nodeRoot.attribute("type");

    if (attrType && 0 == strcmp(attrType.as_string(), "error"))
    {
        APP_ERR("type attribute(%s) error", attrType.as_string());
        return false;
    }

    // get "from" attribute
    xml_attribute attrNameFrom = nodeRoot.attribute(kszAttributeNameFrom);
    yl::string strFrom;

    if (attrNameFrom)
    {
        strFrom = attrNameFrom.as_string();
    }

    if (strFrom.empty())
    {
        APP_ERR("from attribute empty!");
        return false;
    }

    size_t pos = strFrom.find('/', 0);
    if (yl::string::npos != pos)
    {
        strFrom = strFrom.substr(0, pos);
    }
    refData.strContactID = commonAPI_StringConvert(strFrom, STRING_CONVERT_TO_LOWER);

    xml_node nodeChildPresence = nodeRoot.child(kszNodeNamePresence);
    //是否为新的Aggregated Presence
    bool bPersistentPresence = false;
    if (!nodeChildPresence.empty())
    {
        xml_attribute attXmlns = nodeChildPresence.attribute("xmlns");
        yl::string strXmlns = "";
        if (attXmlns)
        {
            strXmlns = attXmlns.as_string();
            if (strXmlns == "urn:xmpp:broadsoft:presence")
            {
                bPersistentPresence = true;
            }
        }
    }
    APP_INFO("CUCContactData::ProcessPresenceStream bPersistentPresence[%d]", bPersistentPresence);
    if (bPersistentPresence)
    {
        //默认ONLINE
        refData.ePresenceType = _PRESENCE_TYPE_ONLINE;
        xml_node nodeChildShow = nodeChildPresence.child(kszNodeNameShow);

        if (!nodeChildShow.empty())
        {
            yl::string strValue = nodeChildShow.child_value();

            if (!strValue.empty())
            {
                refData.ePresenceType = GetPrensenceTypeByString(strValue);
            }
        }
        else if (!attrType.empty()
                 && (0 == strcmp(attrType.as_string(), "unavailable")
                     || 0 == strcmp(attrType.as_string(), "unsubscribe")
                     || 0 == strcmp(attrType.as_string(), "unsubscribed")))
        {
            refData.ePresenceType = _PRESENCE_TYPE_OFFLINE;
        }

        xml_node nodeChildFreeText = nodeChildPresence.child(kszNodeNameFreeText);

        if (!nodeChildFreeText.empty())
        {
            refData.m_strTitle = nodeChildFreeText.child_value();
            APP_INFO("FreeText: %s", refData.m_strTitle.c_str());
        }
    }
    else
    {
        //判断type值
        if (!attrType.empty()
                && (0 == strcmp(attrType.as_string(), "unavailable")
                    || 0 == strcmp(attrType.as_string(), "unsubscribe")
                    || 0 == strcmp(attrType.as_string(), "unsubscribed")))
        {
            refData.ePresenceType = _PRESENCE_TYPE_OFFLINE;
        }
        else
        {
            //默认ONLINE
            refData.ePresenceType = _PRESENCE_TYPE_ONLINE;
            // 判断是否有子节点
            // 没有说明是无用的信息,则不进行处理
            xml_node nodeFirstChild = nodeRoot.first_child();

            if (!nodeFirstChild.empty())
            {
                // get "show" attribute
                xml_node nodeNameShow = nodeRoot.child(kszNodeNameShow);

                if (!nodeNameShow)
                {
                    // 没有show结点的情况下为在线状态
                    refData.ePresenceType = _PRESENCE_TYPE_ONLINE;
                }
                else
                {
                    yl::string strValue = nodeNameShow.child_value();

                    if (!strValue.empty())
                    {
                        refData.ePresenceType = GetPrensenceTypeByString(strValue);
                    }
                    else
                    {
                        refData.ePresenceType = _PRESENCE_TYPE_ONLINE;
                    }
                }
            }
        }
    }



    APP_INFO("Change Contact Id(%s)", refData.strContactID.c_str());
    APP_INFO("Change Status(%d)", refData.ePresenceType);

    xml_node nodeStatus = nodeRoot.child("status");

    if (nodeStatus
            && !bPersistentPresence)
    {
        refData.m_strTitle = nodeStatus.child_value();

        APP_INFO("Status: %s", refData.m_strTitle.c_str());
    }

    // http://10.2.1.199/Bug.php?BugID=88578
    if ((attrType || nodeRoot.attribute("id")) && !nodeStatus)
    {
        IterMapPresenceInfo IterInfoMap = MyFindInfoByKey(refData.strContactID);
        if (IterInfoMap != m_mapPresenceInfo.end())
        {
            refData.m_strTitle = IterInfoMap->second.m_strTitle;
        }
    }

    // x
    xml_node nodeX = nodeRoot.child("x");

    if (nodeX)
    {
        for (; nodeX; nodeX = nodeX.next_sibling("x"))
        {
            xml_attribute attrXmlns = nodeX.attribute("xmlns");

            if (attrXmlns && strcmp(attrXmlns.as_string(), "vcard-temp:x:update") == 0)
            {
                xml_node nodePhoto = nodeX.child("photo");

                if (nodePhoto)
                {
                    // 新的特征码
                    yl::string strAvatarSignature = nodePhoto.child_value();

                    APP_INFO("strAvatarSignature = %s", strAvatarSignature.c_str());

                    IterMapPresenceInfo iter = MyFindInfoByKey(strFrom);
                    if (iter != m_mapPresenceInfo.end())
                    {
                        APP_INFO("old strAvatarSignature = %s", iter->second.strAvatarSignature.c_str());
                        APP_INFO("new strAvatarSignature = %s", strAvatarSignature.c_str());
                        refData.strAvatarSignature = strAvatarSignature;
                        // 非lock状态且头像变化，要重新获取头像
                        // lock状态下已经提前获取过头像，不需要再获取
                        if (!iter->second.m_bFirst && iter->second.strAvatarSignature != strAvatarSignature)
                        {
                            // 如果头像改变，则需要重新获取头像信息
                            //向外部模块发送状态改变消息
                            etl_NotifyAppEx(
                                false,
                                PRESENCE_MESSAGE_AVATAR_CHANGE,
                                0,
                                0,
                                refData.strContactID.size() + 1,
                                refData.strContactID.c_str());
                            break;
                        }
                    }
                }
                else
                {
                    APP_INFO("Signature is lack strXmlStream(%s)", strXmlStream.c_str());
                    refData.strAvatarSignature = "";
                }
                break;
            }
        }
    }

    //如果是Favorite联系人，发消息通知Dsskey模块
    if (IsFavoriteContact(refData.strContactID))
    {
        etl_NotifyApp(
            false,
            PRESENCE_MESSAGE_FAVORITE_CHANGE,
            0,
            0);
    }

    AddCommonData(refData);

    return true;
}

_PRESENCE_TYPE CUCContactData::GetPrensenceTypeByString(const yl::string & strPresence)
{
    if (strPresence == "away")
    {
        return _PRESENCE_TYPE_AWAY;
    }
    else if (strPresence == "chat")
    {
        return _PRESENCE_TYPE_CHATTY;
    }
    else if (strPresence == "xa")
    {
        return _PRESENCE_TYPE_EXTENDED_AWAY;
    }
    else if (strPresence == "dnd")
    {
        return _PRESENCE_TYPE_BUSY;
    }
    else if (strPresence == "" || strPresence == "available")
    {
        return _PRESENCE_TYPE_ONLINE;
    }

    return _PRESENCE_TYPE_NONE;
}

bool CUCContactData::ParseCardData(PresenceInfo & objData, const yl::string & strCardXmlInfo)
{
    APP_INFO("Begin ParseCardData,strCardXmlInfo[%s]", strCardXmlInfo.c_str());

    xml_document objXmlDoc;

    objXmlDoc.load(strCardXmlInfo.c_str());

#if UC_TRACE_DYNAMIC_LOG
    yl::string strFile = fmtString("%s%05d_%s.xml", uc_trace_path_pre, ++s_index, __FUNCTION__);
    objXmlDoc.save_file(strFile.c_str());
#endif

    // check format
    xml_node nodeNameIq = objXmlDoc.child(kszNodeNameIq);

    if (!nodeNameIq)
    {
        APP_ERR("nameiq node empty!");
        return false;
    }

    xml_node nodeCard = nodeNameIq.child("vCard");

    if (!nodeCard)
    {
        APP_ERR("vCard node empty!");
        return false;
    }

    xml_node nodePhoto = nodeCard.child("PHOTO");
    xml_node nodeType;
    yl::string strPicType;

    if (!nodePhoto)
    {
        return false;
    }

    nodeType = nodePhoto.child("TYPE");

    if (!nodeType)
    {
        return false;
    }

    strPicType = nodeType.child_value();

    if (strPicType.length() >= 7 && 0 == strPicType.find_first_of("image/"))
    {
        xml_node nodeBinVal = nodeType.next_sibling("BINVAL");

        if (nodeBinVal)
        {
            yl::string strBasr64Data;
            strBasr64Data = nodeBinVal.child_value();

            if (!strBasr64Data.empty())
            {
                objData.strAvatarInfo = GetAvatar(strBasr64Data, objData.strContactID, GetSubString(strPicType,
                                                  "image/", ""));
                APP_INFO("phote:%s", objData.strAvatarInfo.c_str());
                return true;
            }
            else
            {
                APP_INFO("BINVAL empty!");
            }
        }
    }
    return false;
}

yl::string CUCContactData::GetAvatar(const yl::string & strBase64Data,
                                     const yl:: string & strContactID, const yl::string & strPicFormat)
{
    if (strBase64Data.empty() || strContactID.empty() || strPicFormat.empty())
    {
        return "";
    }

    // transform base64 to binary
    char        b64[BUF_SIZE];
    unsigned    b64_len = 0;
    unsigned    bin_len;

    if (strBase64Data.length() > BUF_SIZE)
    {
        APP_WARN("Avatar data is too large!");
        return "";
    }

    strncpy(b64, strBase64Data.c_str(), strBase64Data.length());
    b64[BUF_SIZE - 1] = '\0';

    // the length of b64 characters
    b64_len = strBase64Data.length();
    b64_len = remove_newline(b64, b64_len);

    int nLength = base64_decoded_len(b64, b64_len);

    if (nLength > BUF_SIZE)
    {
        APP_WARN("out buffer too small, need expaneded");
        return "";
    }
    unsigned char   bin[BUF_SIZE];
    memset(bin, 0, BUF_SIZE);

    bin_len = base64_decode(b64, b64_len, bin);

    yl::string strPicName = TEMP_PATH + strContactID + "." + strPicFormat;


    file_write(strPicName.c_str(), (char *)bin, bin_len);

    return strContactID + "." + strPicFormat;
}

void CUCContactData::AddCommonData(PresenceInfo & refData)
{
    APP_INFO("CPresenceModel::AddCommonData");
    // refresh all map
    IterMapPresenceInfo IterInfoMap = MyFindInfoByKey(refData.strContactID);
    if (IterInfoMap != m_mapPresenceInfo.end())
    {
        APP_INFO("Exist Contact");

        // 没有刷新Card，保持之前的值
        refData.strAvatarInfo = IterInfoMap->second.strAvatarInfo;
        refData.m_listAttribute = IterInfoMap->second.m_listAttribute;

        // ContactName始终保持之前值
        refData.strContactName = IterInfoMap->second.strContactName;
        refData.m_bFirst = IterInfoMap->second.m_bFirst;

        refData.m_nLocalId = IterInfoMap->second.m_nLocalId;

        // 状态切换需要重新排序
        bool bStatusChanged = refData.ePresenceType != IterInfoMap->second.ePresenceType;
        IterInfoMap->second = refData;

        if (bStatusChanged)
        {
            m_csSearch.Lock();
            for (GroupList::iterator it = m_GroupList.begin(); it != m_GroupList.end(); it++)
            {
                GroupItem & refGroup = *it;

                // All组排序
                if (refGroup.m_keyId == ALL_BUDDIES_ID)
                {
                    MySort(*it);
                    continue;
                }

                // 对应其它组排序
                CIterContactID iter = refGroup.m_nIdList.begin();
                for (; iter != refGroup.m_nIdList.end(); iter++)
                {
                    if (refData.m_nLocalId == *iter)
                    {
                        APP_INFO("AddCommonData, Contact(%s) (%d)(%d) Group=(%s)", refData.strContactID.c_str(),
                                 refGroup.m_keyId, refData.m_nLocalId, refGroup.m_strDisplayName.c_str());
                        MySort(*it);
                        break;
                    }
                }
            }
            m_csSearch.Unlock();
        }
    }
    else
    {
        APP_INFO("Nonexistence, Contact(%s)", refData.strContactID.c_str());
        if (IsUserJID(refData.strContactID))
        {
            // 本地账号，不加入map
            SetUserPresenceData(refData);
        }
    }
}

//返回本地自己的Presence信息
const PresenceInfo & CUCContactData::GetLocalPresence()
{
    return m_objLocalPresenceData;
}

bool CUCContactData::RefreshAvatar(const yl::string & strContactId)
{
    APP_INFO("CPresenceModel::RefreshAvatar,m_objData.strContactID = %s", strContactId.c_str());
    if (strContactId.empty())
    {
        return false;
    }

    PresenceInfo * pData = &m_objLocalPresenceData;
    bool isUser = false;

    if (IsUserJID(strContactId))
    {
        isUser = true;
    }
    else
    {
        IterMapPresenceInfo iter = MyFindInfoByKey(strContactId);
        if (iter != m_mapPresenceInfo.end())
        {
            pData = &(iter->second);
        }
        else
        {
            return false;
        }
    }

    bool bRefreshCard = ReqContactAvatar(*pData);

    //如果是Favorite联系人，发消息通知Dsskey模块
    //必须是异步的，必须在AddCommonData调用之前，在AddCommonData中会对m_objData清空
    if (IsFavoriteContact(strContactId))
    {
        etl_NotifyApp(
            false,
            PRESENCE_MESSAGE_FAVORITE_CHANGE,
            0,
            0);
    }

    if (!bRefreshCard)
    {
        return false;
    }

    if (isUser)
    {
        //向外部模块发送状态改变消息
        etl_NotifyAppEx(
            false,
            PRESENCE_MESSAGE_USER_STATUS_CHANGE,
            0,
            0,
            (int)m_objLocalPresenceData.strContactID.length() + 1,
            m_objLocalPresenceData.strContactID.c_str());
    }

    return true;
}

bool CUCContactData::ReqContactAvatar(PresenceInfo & refData)
{
    if (refData.strContactID.empty())
    {
        return false;
    }

    refData.m_bFirst = false;

    bool bRefreshCard = false;

    char * xml_stream = NULL;
    size_t stream_len;
    m_cs.Lock();
    if (xmpp_request(m_conn, XMPP_REQUEST_CARD_SINGLE,
                     &xml_stream, &stream_len, refData.strContactID.c_str()) == 0)
    {
        m_cs.Unlock();
        bRefreshCard = ParseCardData(refData, xml_stream);
    }
    else
    {
        APP_INFO("[%s] req card first Fail!", refData.strContactID.c_str());
        if (xml_stream != NULL)
        {
            free(xml_stream);
            xml_stream = NULL;
        }

        if (xmpp_request(m_conn, XMPP_REQUEST_CARD_SINGLE,
                         &xml_stream, &stream_len, refData.strContactID.c_str()) != 0)
        {
            APP_INFO("refresh card error!");
            refData.strAvatarSignature = "";
        }
        else
        {

            bRefreshCard = ParseCardData(refData, xml_stream);
        }

        m_cs.Unlock();
    }

    free(xml_stream);

    return bRefreshCard;
}

yl::string CUCContactData::GetConnectURI()
{
    yl::string strURI("/Yealink");

    strURI.append("-").append(configParser_GetConfigString(kszProductName)).append(":").append(
        netGetMACText('-'));

    // 获取当前系统时间
//  struct timeval tData;
//  gettimeofday(&tData, NULL);
//  char szBuffer[128] = "";
//  sprintf(szBuffer, "%d", tData.tv_sec);

    // 生成随机的描述符
//  strURI += szBuffer;

    return strURI;
}


// 设置用户的Presence数据
void CUCContactData::SetUserPresenceData(const PresenceInfo & presenceData)
{
    APP_INFO("CPresenceModel::SetUserPresenceData");

    m_objLocalPresenceData.strContactName = presenceData.strContactName;
    m_objLocalPresenceData.m_listAttribute = presenceData.m_listAttribute;
    if (m_bAllowChangePresence)
    {
        m_objLocalPresenceData.ePresenceType = presenceData.ePresenceType;
    }
    else if (presenceData.ePresenceType != _PRESENCE_TYPE_OFFLINE)
    {
        m_objLocalPresenceData.ePresenceType = presenceData.ePresenceType;
    }
    m_objLocalPresenceData.strContactID = presenceData.strContactID;
    m_objLocalPresenceData.strAvatarSignature = presenceData.strAvatarSignature;
    m_objLocalPresenceData.m_strTitle = presenceData.m_strTitle;

    //向外部模块发送状态改变消息
    etl_NotifyAppEx(
        false,
        PRESENCE_MESSAGE_USER_STATUS_CHANGE,
        0,
        0,
        (int)m_objLocalPresenceData.strContactID.length() + 1,
        m_objLocalPresenceData.strContactID.c_str());
}

// 判断ID是否为用户ID
bool CUCContactData::IsUserJID(const yl::string & strID)
{
    return strcasecmp(m_strUserID.c_str(), strID.c_str()) == 0;
}

void CUCContactData::ReqMyAvatar()
{
    APP_INFO("CPresenceModel::GetMyAvatar");


    //主动获取自己的头像
    char * xml_stream1 = NULL;
    size_t stream_len1;

    if (!m_bAllowChangePresence)
    {
        m_objLocalPresenceData.ePresenceType = _PRESENCE_TYPE_ONLINE;
    }

    m_cs.Lock();
    if (xmpp_request(m_conn, XMPP_REQUEST_CARD_SINGLE,
                     &xml_stream1, &stream_len1, m_strUserID.c_str()) != 0)
    {
        m_cs.Unlock();
        APP_INFO("Get MyAvatar error!");
        m_objLocalPresenceData.strAvatarInfo = "";
    }
    else
    {
        m_cs.Unlock();
        APP_INFO("Get MyAvatar success!");

        // 获取自己的图片的时候,可能服务器还未推送自己的JID下来(比如账号没有登录)
        // 设置自己的jid
        m_objLocalPresenceData.strContactID = m_strUserID;
        if (ParseCardData(m_objLocalPresenceData, xml_stream1))
        {
            etl_NotifyApp(false, PRESENCE_MESSAGE_USER_INFO_CHANGE, 0, 0);
        }
    }

    free(xml_stream1);
}

void CUCContactData::ReqRosterAll()
{
    APP_INFO("CPresenceModel::ReqRosterAll");

    char * xml_stream1 = NULL;
    size_t stream_len1;

    m_cs.Lock();
    if (xmpp_request(m_conn, XMPP_REQUEST_ROSTER_ALL,
                     &xml_stream1, &stream_len1, m_strUserID.c_str()) != 0)
    {
        m_cs.Unlock();
        APP_INFO("ReqRosterAll error!");
    }
    else
    {
        m_cs.Unlock();
        APP_INFO("ReqRosterAll success!");
    }

    free(xml_stream1);
}

//请求获取Favorite联系人
bool CUCContactData::ReqAddressbookList(bool bRefreshAll /*= false*/)
{
    APP_INFO("CPresenceModel::GetFavoriteList()");

    bool bReturn = false;
    char * xml_stream = NULL;
    size_t stream_len;
    //容错
    if (m_conn->conn_status != XMPP_CONN_CONNECTED)
    {
        return false;
    }
    m_cs.Lock();
    if (xmpp_request(m_conn, XMPP_REQUEST_ADDRESSBOOK_ALL,
                     &xml_stream, &stream_len) != 0)
    {
        m_cs.Unlock();
        APP_INFO("Get Favorite error!");
    }
    else
    {
        m_cs.Unlock();
        APP_INFO("Get Favorite success!");

        m_nPreGroupSize = m_GroupList.size();

        m_csSearch.Lock();
        //解析前先清空原数据
        for (GroupList::iterator iter = m_GroupList.begin(); iter != m_GroupList.end();)
        {
            GroupItem & refItem = *iter;
            if (refItem.m_strDisplayName.compare(kszAllBuddiesName) == 0)
            {
                iter++;
                continue;
            }
            else
            {
                iter = m_GroupList.erase(iter);
            }
        }
        m_csSearch.Unlock();

        //获取成功解析联系人
        ParseAddressbookXml(xml_stream, bRefreshAll);
        bReturn = true;
    }

    //发消息通知Dsskey模块
    etl_NotifyApp(false, PRESENCE_MESSAGE_FAVORITE_CHANGE, 0, 0);

    free(xml_stream);

    return bReturn;
}

bool CUCContactData::ReqChangPresence(_PRESENCE_TYPE eType)
{
    APP_INFO("CPresenceModel::ReqChangPresence eType = [%d]", eType);

    // 不允许设置None状态
    if (_PRESENCE_TYPE_NONE == eType)
    {
        return false;
    }

    if (g_pUCPresence->GetLocalPresence().ePresenceType == eType)
    {
        return true;
    }

    char * xml_stream = NULL;
    size_t stream_len;

    if (m_conn->conn_status != XMPP_CONN_CONNECTED)
    {
        xmpp_close(m_conn);

        if (xmpp_open(m_conn, m_connInfo) != 0)
        {
            APP_INFO("connect error");
            //连接失败，直接返回
            return false;
        }
        else
        {
            APP_INFO("connect succ");
            //连接成功重新获取联系人状态
            ReqAllPresence();
        }
    }

    bool suc = true;

    m_cs.Lock();
    if (xmpp_request(m_conn, XMPP_REQUEST_STATUS_SET,
                     &xml_stream, &stream_len, eType) != 0)
    {
        m_cs.Unlock();
        APP_INFO("ReqChangeStatus error!");
        suc = false;
    }
    else
    {
        m_cs.Unlock();
        APP_INFO("ReqChangeStatus suc!");
        if (m_bAllowChangePresence
                && eType == _PRESENCE_TYPE_OFFLINE)
        {
            m_objLocalPresenceData.ePresenceType = _PRESENCE_TYPE_OFFLINE;

            //向外部模块发送状态改变消息
            etl_NotifyAppEx(
                false,
                PRESENCE_MESSAGE_USER_STATUS_CHANGE,
                0,
                0,
                (int)m_objLocalPresenceData.strContactID.length() + 1,
                m_objLocalPresenceData.strContactID.c_str());
            //离线断开连接
            xmpp_close(m_conn);

            //所有联系人设置为OffLine状态
            for (IterMapPresenceInfo iter = m_mapPresenceInfo.begin(); iter != m_mapPresenceInfo.end(); iter++)
            {
                PresenceInfo & refData = iter->second;
                if (refData.ePresenceType != _PRESENCE_TYPE_OFFLINE)
                {
                    refData.ePresenceType = _PRESENCE_TYPE_OFFLINE;
                }
            }
            //通知dsskye改变状态
            etl_NotifyApp(
                false,
                PRESENCE_MESSAGE_FAVORITE_CHANGE,
                0,
                0);
        }
    }
    free(xml_stream);

    if (suc && eType == _PRESENCE_TYPE_OFFLINE)
    {
        RefreshViewerPresence();
    }

    return suc;
}

void CUCContactData::ParseAddressbookXml(const yl::string & strContactsXml,
        bool bRefreshAll /*= false*/)
{
    //m_favoriteList.push_back("xiaojiangmai@gmail.com");
    APP_INFO("Begin ParseFavoriteXml,strContactsXml[%s]", strContactsXml.c_str());
    xml_document objXmlDoc;

    objXmlDoc.load(strContactsXml.c_str());

#if UC_TRACE_DYNAMIC_LOG
    yl::string strFile = fmtString("%s%05d_%s.xml", uc_trace_path_pre, ++s_index, __FUNCTION__);
    objXmlDoc.save_file(strFile.c_str());
#endif

    // check format
    xml_node nodeRoot = objXmlDoc.child(kszNodeNameIq);

    if (!nodeRoot)
    {
        APP_ERR("iq node empty!");
        return;
    }

    xml_node nodeQuery = nodeRoot.child(kszNodeNameQuery);

    if (!nodeQuery)
    {
        APP_ERR("query node empty!");
        return;
    }

    xml_node nodeStorage = nodeQuery.child(kszNodeContactStorage);

    if (!nodeStorage)
    {
        APP_ERR("storage node empty!");
        return;
    }

    xml_node nodeAddressBook = nodeStorage.child(kszNodeAddressBook);

    if (!nodeAddressBook)
    {
        APP_ERR("addressbook node empty!");
        return;
    }

    CPresenceRequestAction * pAction = NULL;
    GroupItem GBuddiesItem;

    if (m_bIsIniting)
    {
        GBuddiesItem.m_strDisplayName = kszAllBuddiesName;
        //All 排前面
        GBuddiesItem.iSeq = -2;
        SetGroupKey(GBuddiesItem);

        pAction = TaskAction_CreatePresenceRequestAction();
        if (pAction == NULL)
        {
            return;
        }

        pAction->SetReqType(CPresenceRequestAction::REQ_CARD);
        pAction->SetExtraInfo(m_conn, m_connInfo);
        pAction->SetSync(false);
        pAction->SetRead(true);
        pAction->SetCallback(this);
    }

    xml_node nodeContacts = nodeAddressBook.child(kszNodeContacts);

    if (!nodeContacts)
    {
        APP_ERR("contacts node empty!");
        return;
    }

    for (xml_node nodeContact = nodeContacts.first_child(); nodeContact;
            nodeContact = nodeContact.next_sibling(kszNodeContact))
    {
        PresenceInfo Item;

        YLList<Attribute> listNumAttribute;
        //获取联系人id
        xml_attribute attrId = nodeContact.attribute(kszAttributeId);
        yl::string strContactId ;

        if (attrId)
        {
            strContactId = attrId.as_string();
        }

        if (m_mapContacts.find(strContactId) != m_mapContacts.end()
                && !bRefreshAll)
        {
            continue;
        }

        xml_node nodeChild = nodeContact.first_child();

        for (; nodeChild; nodeChild = nodeChild.next_sibling())
        {
            yl::string strContent = nodeChild.child_value();

            yl::string strNodeName = nodeChild.name();

            if (strNodeName.compare("display_name") == 0)
            {
                Item.strContactName = strContent;
            }
            else if (strNodeName.compare(kszAttributeCommunication) == 0)
            {
                yl::string strType;

                xml_attribute attrType = nodeChild.attribute("type");

                if (attrType)
                {
                    strType = attrType.as_string();
                }

                if (strType.compare(kszAttributeNameID) == 0)
                {
                    Item.strContactID = commonAPI_StringConvert(strContent, STRING_CONVERT_TO_LOWER);
                }
                else if (strType.compare("sip") == 0)
                {
                    Attribute attNumber;
                    attNumber.m_strName = "Number";
                    attNumber.m_strValue = GetSubString(strContent, ":", "@");
                    attNumber.m_eType = ATTR_TYPE_NUMBER;

                    listNumAttribute.push_back(attNumber);
                }
                else if (!strType.empty())
                {
                    Attribute attrItem;
                    if (strType.find("phone") != strType.npos || strType.find("number") != strType.npos)
                    {
                        attrItem.m_eType = ATTR_TYPE_NUMBER;
                    }
                    attrItem.m_strName = strType;
                    attrItem.m_strValue = strContent;

                    listNumAttribute.push_back(attrItem);
                }
            }
            else
            {
                Attribute attrItem;
                attrItem.m_strName = strNodeName;
                attrItem.m_strValue = strContent;

                Item.m_listAttribute.push_back(attrItem);
            }
        }

        //号码属性至于前
        if (listNumAttribute.size() != 0)
        {
            YLList<Attribute>::iterator ItemIter = Item.m_listAttribute.begin();

            for (YLList<Attribute>::iterator iter = listNumAttribute.begin(); iter != listNumAttribute.end();
                    iter++)
            {
                Item.m_listAttribute.insert(ItemIter, *iter);

                //ItemIter = ItemIter->next;
            }
            //Item.m_listAttribute.splice(Item.m_listAttribute.begin(),listNumAttribute);
        }

        //名字为空去firstname和lastname属性显示
        if (Item.strContactName.empty())
        {
            yl::string strFirstName = Item.GetAttributeValueByName("first_name");
            yl::string strLastName = Item.GetAttributeValueByName("last_name");

            if (!strFirstName.empty() || !strLastName.empty())
            {
                Item.strContactName = strFirstName + " " + strLastName;
            }
            else
            {
                //名字为空去JID @前面部分
                Item.strContactName = GetSubString(Item.strContactID, "", "@");

                if (Item.strContactName.empty())
                {
                    Item.strContactName = Item.strContactID;
                }
            }
        }

        if (!strContactId.empty() && !Item.strContactID.empty())
        {
            m_mapContacts.put(strContactId, Item.strContactID);
        }

        Item.ePresenceType = _PRESENCE_TYPE_OFFLINE;        // default offline

        if (IsUserJID(Item.strContactID))
        {
            SetUserPresenceData(Item);
        }
        else if (!Item.strContactID.empty())
        {
            if (m_bIsIniting)
            {
                InsertToMap(Item);

                InsertIdToGroupList(Item.strContactID, GBuddiesItem.m_nIdList);

                if (pAction != NULL)
                {
                    pAction->PushPresenceInfo(Item);
                }
            }
            else
            {
                // 更新信息
                IterMapPresenceInfo iter = MyFindInfoByKey(Item.strContactID);
                if (iter != m_mapPresenceInfo.end())
                {
                    APP_INFO("update [%s]", Item.strContactID.c_str());
                    PresenceInfo & sPresenceInfo = iter->second;

                    // 以下信息可能发生改变
                    sPresenceInfo.strContactName = Item.strContactName;
                    sPresenceInfo.m_listAttribute = Item.m_listAttribute;
                }
            }
        }
    }

    if (m_bIsIniting)
    {
        MySort(GBuddiesItem);

        m_csSearch.Lock();
        m_GroupList.push_back(GBuddiesItem);
        m_csSearch.Unlock();

        TaskAction_ExecAction(pAction);
    }

    xml_node nodeFavorites = nodeAddressBook.child(kszNodeFavorites);

    if (nodeFavorites)
    {
        GroupItem GItem;
        GItem.m_strDisplayName = kszFavourite;
        SetGroupKey(GItem);

        for (xml_node nodeFavorite = nodeFavorites.child(kszNodeFavorite); nodeFavorite;
                nodeFavorite = nodeFavorite.next_sibling(kszNodeFavorite))
        {
            //获取Favorite Id
            xml_attribute attrId = nodeFavorite.attribute(kszAttributeId);
            yl::string strContactId;

            if (attrId)
            {
                strContactId = attrId.as_string();
            }

            YLHashMap<yl::string, yl::string>::iterator iter;
            //通过id在map中找到Jid
            iter = m_mapContacts.find(strContactId);
            if (iter != m_mapContacts.end())
            {
                yl::string strJid = iter->second;
                //该Jid是联系人才加入到链表中
                InsertIdToGroupList(strJid, GItem.m_nIdList);
            }
        }

        MySort(GItem);

        m_csSearch.Lock();
        m_GroupList.push_back(GItem);
        m_csSearch.Unlock();
    }

    xml_node nodeGroups = nodeAddressBook.child(kszNodeGroups);

    if (nodeGroups)
    {
        for (xml_node nodeGroup = nodeGroups.child(kszNodeGroup); nodeGroup;
                nodeGroup = nodeGroup.next_sibling(kszNodeGroup))
        {
            GroupItem GItem;

            xml_attribute attr = nodeGroup.attribute(kszNodeDisplayName);

            if (attr)
            {
                GItem.m_strDisplayName = attr.as_string();
            }

            attr = nodeGroup.attribute("q");

            if (attr)
            {
                GItem.iSeq = atoi(attr.as_string());

            }

            if (GItem.m_strDisplayName.empty())
            {
                continue;
            }

            SetGroupKey(GItem);

            for (xml_node nodeMember = nodeGroup.child(kszNodeMember); nodeMember;
                    nodeMember = nodeMember.next_sibling(kszNodeMember))
            {
                attr = nodeMember.attribute(kszAttributeId);
                yl::string strContactId;

                if (attr)
                {
                    strContactId = attr.as_string();
                }

                YLHashMap<yl::string, yl::string>::iterator iter;
                //通过id在map中找到Jid
                iter = m_mapContacts.find(strContactId);
                if (iter != m_mapContacts.end())
                {
                    yl::string strJid = iter->second;
                    //该Jid是联系人才加入到链表中
                    InsertIdToGroupList(strJid, GItem.m_nIdList);
                }
            }

            m_csSearch.Lock();
            m_GroupList.push_back(GItem);
            m_csSearch.Unlock();
        }

        m_csSearch.Lock();
        Sort(m_GroupList, GroupItem::Comp);
        //m_GroupList.sort(GroupItem::Comp);
        m_csSearch.Unlock();
    }

    if (m_GroupList.size() != m_nPreGroupSize && m_nPreGroupSize != -1)
    {
        // 发送消息通知UI
        etl_NotifyApp(false, BROADSOFT_BUDDYLIST_GROUP_CHANGE, 0, 0);

    }
}

bool CUCContactData::InsertIdToGroupList(const yl::string & strContactId, ListContactID & refList,
        bool bSort/* = false*/)
{
    //该Jid是联系人才加入到链表中
    IterMapPresenceInfo presenceIter = MyFindInfoByKey(strContactId);
    if (presenceIter != m_mapPresenceInfo.end())
    {
        //Jid加入到Favorite联系人链表
        GroupKey & stKey = presenceIter->second.m_nLocalId;

        if (bSort)
        {
            MyInsert(refList, stKey);
        }
        else
        {
            refList.push_back(stKey);
        }
        return true;
    }
    else
    {
        APP_INFO("not found!");
        return false;
    }
}

//通过遍历判断是否是Favorite联系人
bool CUCContactData::IsFavoriteContact(const yl::string & strJid)
{
    IterMapPresenceInfo iterMap = MyFindInfoByKey(strJid);

    if (iterMap == m_mapPresenceInfo.end())
    {
        return false;
    }
    int nId = iterMap->second.m_nLocalId;

    for (GroupList::iterator it = m_GroupList.begin(); it != m_GroupList.end(); it++)
    {
        GroupItem & refGroup = *it;
        if (refGroup.m_strDisplayName.compare(kszFavourite) == 0)
        {
            CIterContactID iter = refGroup.m_nIdList.begin();
            for (; iter != refGroup.m_nIdList.end(); iter++)
            {
                //比较Jid
                if (nId == *iter)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

BOOL CUCContactData::OnTimeOut(msgObject & objMessage)
{
    APP_INFO("CPresenceModel::OnTimeOut");
    if (m_RefreshTimer.IsTimerRuning())
    {
        m_RefreshTimer.KillTimer();

        m_bLock = false;
        //TO DO

        RefreshViewerPresence();
    }
    return TRUE;
}

BOOL CUCContactData::OnFavoriteTimeOut(msgObject & objMessage)
{
    APP_INFO("Auto Request Favorite List:");

    g_pUCPresence->ReqAddressbookList();

    return TRUE;
}

// 刷新观察者对象的Presence状态
void CUCContactData::RefreshViewerPresence()
{
    APP_INFO("CPresenceController::RefreshViewerPresence");

    if (IsLocked())
    {
        APP_INFO("Locked!");
        return;
    }

    // 发送消息通知UI
    etl_NotifyAppEx(false, BROADSOFT_BUDDYLIST_REFRESH_FINISHED, 0, 0, 0, 0);
}

void CUCContactData::SetGroupKey(GroupItem & refData)
{
    MapKeyToId::iterator iter = m_mapGroupKey.find(refData.m_strDisplayName);
    if (iter != m_mapGroupKey.end())
    {
        refData.m_keyId = iter->second;
    }
    else
    {
        refData.m_keyId = GetId();
        m_mapGroupKey.put(refData.m_strDisplayName, refData.m_keyId);
    }
}

void CUCContactData::RemoveContactInGroup(GroupKey & key)
{
    for (GroupList::iterator iter = m_GroupList.begin(); iter != m_GroupList.end(); iter++)
    {
        GroupItem & refGItem = *iter;

        for (ListContactID::iterator it = refGItem.m_nIdList.begin(); it != refGItem.m_nIdList.end();)
        {
            if (key == *it)
            {
                it = refGItem.m_nIdList.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
}

bool CUCContactData::GetImageByNumber(yl::string & strParh, const yl::string & strNumber)
{
    StringMatchType eMachType = TOTAL_TAIL_MATCH;
    int nUcTailMatchNum = configParser_GetConfigInt(kszUcTailMatchNumber);
    //使用完全匹配
    if (nUcTailMatchNum == 0)
    {
        eMachType = TOTAL_MATH;
    }
    else if (strNumber.size() < nUcTailMatchNum)
    {
        return false;
    }


    for (IterMapPresenceInfo mapIter = m_mapPresenceInfo.begin(); mapIter != m_mapPresenceInfo.end();
            mapIter++)
    {
        PresenceInfo & refData = mapIter->second;

        if (IsSearchHit(refData, strNumber, ATTR_TYPE_NUMBER, eMachType))
        {
            if (refData.strAvatarInfo.empty())
            {
                continue;;
            }

            strParh = TEMP_PATH + refData.strAvatarInfo;

            APP_INFO("CUCContactData::GetImageByNumber strPath[%s],strNumber[%s]", strParh.c_str(),
                     strNumber.c_str());
            return true;
        }
    }

    return false;
}

bool CUCContactData::CheckReConnect()
{
    if (m_conn == NULL)
    {
        return false;
    }

    int iErrorCode = m_conn->conn_errno;

    APP_INFO("CUCContactData::CheckReConnect [%d]", m_conn->conn_errno);
    // 检测3种错误，出错了需要重连
    if (XMPP_ERR_CONN == iErrorCode
            || XMPP_ERR_TIMEOUT == iErrorCode
            || XMPP_ERR_INTERNAL == iErrorCode
            || XMPP_ERR_NOT_CONNECTED == iErrorCode
            || XMPP_ERR_MEM == iErrorCode)
    {
        m_ReconnectTimer.SetTimer(60 * 1000, NULL,
                                  MK_MSG_ENTRY(this, CUCContactData::OnReconnectMessage));
        return false;
    }
    else
    {
        if (m_ReconnectTimer.IsTimerRuning())
        {
            m_ReconnectTimer.KillTimer();
        }
    }

    return true;
}

BOOL CUCContactData::OnReconnectMessage(msgObject & refObj)
{
    RefreshAllInfo();
    return 1;
}

void CUCContactData::SetAllowChangeStatus(bool bAllowChangePresence)
{
    m_bAllowChangePresence = bAllowChangePresence;
}

bool CUCContactData::IsAllowChangeStatus()
{
    m_bAllowChangePresence = configParser_GetConfigInt(kszEnableChangePresenceStatus);
    return m_bAllowChangePresence;
}

bool CUCContactData::SetXmppFeaturesAppend()
{
    if (m_conn == NULL)
    {
        APP_INFO("CUCContactData::SetXmppFeaturesAppend m_conn == NULL!");
        return false;
    }
    //配置要开启，因为XMPP_OPTIONS_SET_PROPERIES_MANUAL_FIELD要基于此配置才生效
    int nValue = 1;//configParser_GetConfigInt(kszPresenceForceSyncOpt);
    int nRet = xmpp_set_features(m_conn, XMPP_OPTIONS_APPEND_PROPERIES_FIELD, &nValue);
    APP_INFO("CUCContactData::SetXmppFeaturesAppend  XmppForceSyncOpt[%d] nRet[%d]\n", nValue, nRet);

    return 0 == nRet;
}

bool CUCContactData::SetXmppFeaturesManual()
{
    if (m_conn == NULL)
    {
        APP_INFO("CUCContactData::SetXmppFeaturesManual m_conn == NULL!");
        return false;
    }

    int nValue = configParser_GetConfigInt(kszPresenceForceSyncManualOpt);
    int nRet = xmpp_set_features(m_conn, XMPP_OPTIONS_SET_PROPERIES_MANUAL_FIELD, &nValue);
    APP_INFO("CUCContactData::SetXmppFeaturesManual  XmppForceSyncManulOpt[%d] nRet[%d]\n", nValue,
             nRet);

    return 0 == nRet;
}

bool CUCContactData::ParseUpdateMessage(xml_document & objXmlDoc)
{
    xml_node nodeRoot = objXmlDoc.child(kszNodeNameMessage);
    if (!nodeRoot)
    {
        return false;
    }

    xml_node nodeEvent = nodeRoot.child(kszNodeNameEvent);
    if (!nodeEvent)
    {
        return false;
    }

    xml_node nodeItems = nodeEvent.child(kszNodeNameItems);
    if (!nodeItems)
    {
        return false;
    }

    xml_node nodeItem = nodeItems.child(kszNodeNameItem);
    if (!nodeItem)
    {
        return false;
    }

    xml_node nodeLastUpdate = nodeItem.child(kszNodeNameLastUpdate);
    if (!nodeLastUpdate)
    {
        return false;
    }

    yl::string strLastUpdate = nodeLastUpdate.child_value();

    if (m_strLastUpdateTime == strLastUpdate)
    {
        return false;
    }
    else
    {
        m_strLastUpdateTime = strLastUpdate;
        return true;
    }
}

#endif //FEATURE_BROADTOUCH_UC

