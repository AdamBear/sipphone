#include "datafactory.h"
#include "xmlparser/xmlparser.hpp"
#include "path_define.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "dsklog/log.h"

#include "settinguibaseitemdata.h"
#include "settinguicheckboxitemdata.h"
#include "settinguicomboboxitemdata.h"
#include "settinguilineedititemdata.h"
#include "settinguimenuitemdata.h"
#include "settinguisoftkeydata.h"
#include "settinguierrorinfo.h"

#include "settinguicommondata.h"
#include "configfiltermanager.h"
#include "pagedatafactory.h"

#include "itemdatafactory.h"
#include "itemdataparserfactory.h"
#include <settinguilogic/include/settingdefine.h>

namespace
{
#define MAX_SOFTKEY_NUM 20
static bool page_default_lock_in_value = true;
#if IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
const char* PAGEDATA_XML_ATTR_ITEM_HEIGHT = "itemheight";
const char* PAGEDATA_XML_ATTR_PAGE_ITEM_COUNT = "pageitemcount";
#endif
}

CDataFactory * CDataFactory::m_pInstance = NULL;

CDataFactory * CDataFactory::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CDataFactory();
        // 开机不预先加载到内存中
        m_pInstance->init();
    }
    return m_pInstance;
}

CDataFactory::CDataFactory()
{
}

bool CDataFactory::GetPageInfo(const yl::string & strPagename, CSettingUIPageDataBase * pPageData)
{
    return LoadPageDataByFileName(strPagename, pPageData);
}

bool CDataFactory::IsPageConfiged(const yl::string & strPagename)
{
    return m_mapInfo.find(strPagename) != m_mapInfo.end();
}

void CDataFactory::init()
{
}

bool CDataFactory::LoadPageDataByFileName(const yl::string & strFile,
        CSettingUIPageDataBase * pPageData)
{
    if (strFile.empty() || NULL == pPageData)
    {
        return NULL;
    }

    // 加载xml文件,初始化数据pagedata
    yl::string strFileName = strFile;
    if (-1 == strFileName.find(".xml"))
    {
        strFileName += ".xml";
    }

    if (!pathFileExist(strFileName.c_str()))
    {
        SETTINGUI_WARN("File [%s] not exsit", strFileName.c_str());
        return NULL;
    }

    xml_document doc;
    doc.load_file(strFileName.c_str());
    xml_node nodeRoot = doc.first_child();
    if (!nodeRoot)
    {
        return NULL;
    }

    if (NULL == pPageData)
    {
        return NULL;
    }

    ParsePageRoot(nodeRoot, pPageData);

    xml_node nodeChild = nodeRoot.child("softkey");
    if (nodeChild)
    {
        ParseSoftkeyInfo(nodeChild, pPageData);
    }

    nodeChild = nodeRoot.child("title");
    if (nodeChild)
    {
        pPageData->SetPageTitle(nodeChild.attribute("displayname").as_string());
    }

    nodeChild = nodeRoot.child("emptyhint");

    if (nodeChild)
    {
        pPageData->SetEmptyHint(nodeChild.attribute("text").as_string());
    }

    nodeChild = nodeRoot.child("item");
    if (nodeChild)
    {
        ParseItemNode(nodeChild, pPageData);
        ParseItemList(nodeChild, pPageData);
    }

    return pPageData;
}

void CDataFactory::PushPageInfo(const yl::string & strName, CSettingUIPageDataBase * pPageData)
{
    if (strName.empty() || NULL == pPageData
            || m_mapInfo.find(strName) != m_mapInfo.end())
    {
        return;
    }

    m_mapInfo.insert(strName, pPageData);
}

int GetUiTypeByString(const char * pType)
{
    if (NULL == pType)
    {
        return STUI_TYPE_ITEM;
    }

    if (0 == strcmp(pType, "menu"))
    {
        return STUI_TYPE_MENU;
    }
    else if (0 == strcmp(pType, "list"))
    {
        return STUI_TYPE_LIST;
    }
    else if (0 == strcmp(pType, "config"))
    {
        return STUI_TYPE_ITEM;
    }
    else if (0 == strcmp(pType, "radio"))
    {
        return STUI_TYPE_RADIO;
    }
    else
    {
        return STUI_TYPE_ITEM;
    }
}

void CDataFactory::ParsePageRoot(const xml_node & node, CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

#ifndef _WIN32
#warning default data
#endif
    pPageData->SetPageID(node.attribute("id").as_string());
    pPageData->SetPageAuthorid(node.attribute("authorid").as_string());
    pPageData->SetReturnToIdle(strcmp(node.attribute("TimeoutReturn").as_string("yes"), "yes") == 0);
    yl::string strLockInValue = node.attribute("AllowEeventWhenLock").as_string();
    if (strLockInValue == "default")
    {
        pPageData->SetLock(page_default_lock_in_value);
    }
    else
    {
        pPageData->SetLock("yes" == strLockInValue);
    }
    pPageData->SetFocusIndex(node.attribute("defaultIndex").as_int());
#if IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    pPageData->SetItemHeight(node.attribute(PAGEDATA_XML_ATTR_ITEM_HEIGHT).as_int());
    pPageData->SetPageItemCount(node.attribute(PAGEDATA_XML_ATTR_PAGE_ITEM_COUNT).as_int());
#endif

    pPageData->SetNetworkChangedReboot(0 == strcmp(node.attribute("networkconfig").as_string(), "yes"));
    pPageData->SetRefreshPrevousView(0 == strcmp(node.attribute("RefreshPreviousView").as_string(),
                                     "yes"));
}

void CDataFactory::ParseSoftkeyInfo(const xml_node & node, CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    CSettingUISoftkeyData * pSoftkeyData = pPageData->GetListSoftkeyData();
    if (NULL == pSoftkeyData)
    {
        pSoftkeyData = new CSettingUISoftkeyData();
        if (NULL == pSoftkeyData)
        {
            return;
        }

        pSoftkeyData->Clear();
    }

    int iKeyIndex = 0;
    xml_attribute attrTmp = node.first_attribute();
    xml_node nodeSoftkeyitem = node.first_child();
    while (nodeSoftkeyitem)
    {
        CSettingUISoftkeyItemDataPtr pSoftkeyItem = new CSettingUISoftkeyItemData();
        if (NULL != pSoftkeyItem)
        {
            pSoftkeyItem->SetIndex(nodeSoftkeyitem.attribute("index").as_int());
            pSoftkeyItem->SetAction(nodeSoftkeyitem.attribute("action").as_string());
            pSoftkeyItem->SetDisplayName(nodeSoftkeyitem.attribute("displayname").as_string());
            pSoftkeyItem->SetIconPath(nodeSoftkeyitem.attribute("icon").as_string());

            if (!pSoftkeyData->AddSoftkeyItem(pSoftkeyItem))
            {
                delete pSoftkeyItem;
                pSoftkeyItem = NULL;
            }
        }

        nodeSoftkeyitem = nodeSoftkeyitem.next_sibling();
    }

    if (NULL != pPageData)
    {
        pPageData->SetListSoftkeyData(pSoftkeyData);
    }
}

void CDataFactory::ParseItemNode(const xml_node & node, CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }
    //目前item节点里面只配置消息以及界面点击响应
    ParseMessageInfo(node, pPageData->GetPointListPageMessageData());

    pPageData->SetAction(node.attribute("action").as_string());

    int iSize = pPageData->GetListPageMessageData().size();
}

void CDataFactory::ParseItemList(const xml_node & node, CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    xml_node nodeChildItem = node.first_child();
    while (nodeChildItem)
    {
        yl::string strType = nodeChildItem.name();
        CSettingUIItemDataBase * pItemInfo = NULL;

        pItemInfo = g_ItemDataFactory.GetItemData(strType);
        if (NULL == pItemInfo)
        {
            pItemInfo = g_ItemDataFactory.GetDefaultItemData();

            if (NULL == pItemInfo)
            {
                nodeChildItem = nodeChildItem.next_sibling();
                continue;
            }
        }

        if (g_ItemDataParserFactory.ParserItemData(strType, nodeChildItem, pItemInfo))
        {
            pPageData->AddItem(pItemInfo);
        }

        nodeChildItem = nodeChildItem.next_sibling();
    }
}

WIDGET_VALUE_TYPE GetValueTypeByString(const yl::string & strTypeString)
{
    if ("int" == strTypeString)
    {
        return WIDGET_VALUE_TYPE_INT;
    }
    else if ("string" == strTypeString)
    {
        return WIDGET_VALUE_TYPE_STRING;
    }
    else if ("bool" == strTypeString)
    {
        return WIDGET_VALUE_TYPE_BOOL;
    }
    else
    {
        return WIDGET_VALUE_TYPE_UNKNOW;
    }
}

void CDataFactory::ParseErrorInfo(const yl::string & strErrorString,
                                  CSettingUIItemDataBase * pItemInfo)
{
    // 格式：
    // error="Redial Time is invalid!"
    // error="format:Hotline delay is invalid!;range:Hotline delay should be 0 ~ 10"
    if (strErrorString.empty() || NULL == pItemInfo)
    {
        return;
    }

    CSettingUIErrorInfo * pErrorInfo = new CSettingUIErrorInfo();
    if (NULL == pErrorInfo)
    {
        return;
    }

    if (yl::string::npos == strErrorString.find(":"))
    {
        pErrorInfo->SetEmptyError(strErrorString);
        pErrorInfo->SetFormateError(strErrorString);
        pErrorInfo->SetRangeError(strErrorString);
    }
    else
    {
        LIST_STRING OutPutList;
        commonAPI_splitStringBySeparator(strErrorString, ';', OutPutList);
        LIST_STRING::ListIterator iter = OutPutList.begin();
        for (; iter != OutPutList.end(); ++iter)
        {
            yl::string strError = *iter;

            int iPriority = 0;
            yl::string::size_type iPos = strError.find(':');
            if (iPos != yl::string::npos)
            {
                yl::string strType = strError.substr(0, iPos);

                if (strError.size() > iPos + 1)
                {
                    ++iPos;
                    strError = strError.substr(iPos, strError.size() - iPos);
                }
                else
                {
                    continue;
                }

                if ("empty" == strType)
                {
                    pErrorInfo->SetEmptyError(strError);
                }
                else if ("format" == strType)
                {
                    pErrorInfo->SetFormateError(strError);
                }
                else if ("range" == strType)
                {
                    pErrorInfo->SetRangeError(strError);
                }
            }
        }
    }

    pItemInfo->SetItemErrorInfo(pErrorInfo);
}

int MsgStringAction2MsgType(const yl::string & strMessageAction)
{
    int eMsgType = MESSAGE_METHOD_SEND;
    if (strMessageAction == "Send")
    {
        eMsgType = MESSAGE_METHOD_SEND;
    }
    else if (strMessageAction == "PostMsgToThread")
    {
        eMsgType = MESSAGE_METHOD_POST_THREAD;
    }
    else if (strMessageAction == "PostMsg")
    {
        eMsgType = MESSAGE_METHOD_BROADCAST;
    }
    else if (strMessageAction == "NotifyApp")
    {
        eMsgType = MESSAGE_METHOD_NOTIFYAPP;
    }
    else if (strMessageAction == "NotifyAppEx")
    {
        eMsgType = MESSAGE_METHOD_NOTIFYAPPEX;
    }

    return eMsgType;
}

void CDataFactory::ParseMessageInfo(const xml_node& node,
                                    LIST_SETTING_MSG_DATA* pMsgInfo)
{
    // eg: message="PostMsg(0x10000, 11, 0);PostMsg(0x10000, 11, 0);"
    yl::string strMsg = node.attribute("message").as_string();
    if (strMsg.empty() || NULL == pMsgInfo)
    {
        return;
    }

    LIST_STRING OutPutList;
    commonAPI_splitStringBySeparator(strMsg, ';', OutPutList);
    LIST_STRING::ListIterator iter = OutPutList.begin();
    for (; iter != OutPutList.end(); ++iter)
    {
        CSettingUIMessageDataPtr pMsg = new CSettingUIMessageData();
        if (NULL == pMsg)
        {
            continue;
        }
        yl::string strMessage = *iter;
        yl::string strMessageAction;
        yl::string strMessageContent;

        int iPriority = 0;
        yl::string::size_type iPos = strMessage.find(':');
        if (iPos != yl::string::npos)
        {
            yl::string strPri = strMessage.substr(0, iPos);
            iPriority = atoi(strPri.c_str());

            if (strMessage.size() > iPos + 1)
            {
                ++iPos;
                strMessage = strMessage.substr(iPos, strMessage.size() - iPos);
            }
        }

        if (commonAPI_ParseActionInfo(&strMessageAction, &strMessageContent, strMessage))
        {
            pMsg->SetSendMethod(MsgStringAction2MsgType(strMessageAction));

            LIST_STRING parmList;
            commonAPI_splitStringBySeparator(strMessageContent, ',', parmList);
            LIST_STRING::ListIterator iterParm = parmList.begin();
            for (int i = 1; iterParm != parmList.end(); ++iterParm, ++i)
            {
                yl::string strParm = *iterParm;
                strParm.trim_both();
                int iMsgIDOrParam = atoi(strParm.c_str());
                if (1 == i)
                {
                    pMsg->SetMsgID(iMsgIDOrParam);
                    if (0 == iMsgIDOrParam)
                    {
                        int iHex;
                        if (1 != sscanf(strParm.c_str(), "%x", &iHex))
                        {
                            return;
                        }

                        pMsg->SetMsgID(iHex);
                    }
                }
                else if (2 == i)
                {
                    pMsg->SetwParam(iMsgIDOrParam);
                    if (0 == iMsgIDOrParam && "0" != strParm)
                    {
                        pMsg->SetwParam(-1);
                        pMsg->SetWparamID(strParm);
                    }
                }
                else if (3 == i)
                {
                    pMsg->SetlParam(iMsgIDOrParam);
                    if (0 == iMsgIDOrParam && "0" != strParm)
                    {
                        pMsg->SetlParam(-1);
                        pMsg->SetLparamID(strParm);
                    }
                }
                else if (4 == i)
                {
                    pMsg->SetDstThread(strParm);
                }
            }

            pMsg->SetPriority(iPriority);
            pMsgInfo->push_back(pMsg);
        }
    }
}

