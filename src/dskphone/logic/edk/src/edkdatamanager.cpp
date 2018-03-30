///////////////////////////////////////////////////////////
//  CEdkDataManager.cpp
//  Implementation of the Class CEdkDataManager
//  Created on:      21-一月-2015 15:32:25
//  Original author: dlw
///////////////////////////////////////////////////////////

#include "edkdatamanager.h"
#include "edkmanager.h"
#include <xmlparser/xmlparser.hpp>
#include <commonapi/stringhelperapi.h>
#include <etlmsghandler/modetlmsghandler.h>
#include "service_config.h"
#include "../include/edkmsgdefine.h"
#include "adapterbox/include/modadapterbox.h"

#define _XMLFILEPATH               CONFIG_DATA_PATH "edkconfig.xml"


// NODE
#define NODE_ROOT                   "edk"
#define NODE_MACO                   "macro"
#define NODE_PROMPT             "prompt"
#define NODE_SOFTKEY                "softkey"
#define NODE_SOFTKEY_KEY            "key"
#define NODE_UI                 "ui"

// ATTRIBUTE
#define ATTRI_INDEX             "index"
#define ATTRI_ENABLE                "enable"
#define ATTRI_NAME                  "name"
#define ATTRI_ACTION                "action"
#define ATTRI_MASKED                "masked"
#define ATTRI_LABEL             "label"
#define ATTRI_INPUT_TYPE            "type"
#define ATTRI_TITLE                 "title"
#define ATTRI_INSERT                "position"
#define ATTRI_SOFTKEYID             "softkey_id"

// 可配置UI界面
#define UI_IDLE                 "idle"
#define UI_INCOMING_CALL            "incomingcall"
#define UI_CONNECTING               "connecting"
#define UI_TRANSFER_CONNECTING      "transfer_connecting"
#define UI_ON_TALK                  "ontalk"
#define UI_CALL_FAILED              "callfailed"
#define UI_RING_BACK                "ringback"
#define UI_TRANFER_RING_BACK        "transfer_ringback"
#define UI_TALK_HOLD                "hold"
#define UI_TALK_HELD                "held"
#define UI_CONFERENCED              "conferenced"
#define UI_DIALTONE             "dialtone"
#define UI_DIALING                  "dialing"

// 弹出框默认输入法类型
#define INPUT_TYPE_NUM              "numeric"
#define INPUT_TYPE_TEXT         "text"

#define SOFTKEY_SIZE              10

#if IF_FEATURE_EDK
IMPLEMENT_GETINSTANCE(CEdkDataManager)

bool Compare(edk_softkey_data_t & DateL, edk_softkey_data_t & DateR)
{
    return DateL.m_iKeyPos < DateR.m_iKeyPos;
}

CEdkDataManager::CEdkDataManager()
{
    // 注册消息处理
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CEdkDataManager::OnMessage);

}

CEdkDataManager::~CEdkDataManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CEdkDataManager::OnMessage);

}

LRESULT CEdkDataManager::OnMessage(msgObject & objMessage)
{
    BOOL bHandled = TRUE;
    EDK_INFO("CEdkDataManager::OnMessage [%d, %d]", objMessage.message, objMessage.wParam);
    switch (objMessage.message)
    {
    case  CONFIG_MSG_BCAST_CHANGED:
        {
            if (objMessage.wParam == ST_EDK_SWITCH || objMessage.wParam == ST_EDK_CONFIG)
            {
                // 提示模块配置关闭,强制关闭内部控制开关,刷新UI(softkey和title)
                if (objMessage.wParam == ST_EDK_SWITCH
                        && objMessage.lParam == 1
                        && configParser_GetConfigInt(kszEdkTipMode) == 0
                        && _EDKManager.IsTipModEnable() == true)
                {
                    _EDKManager.SetTipMode(false);
                    AdapterBox_UpdateTopWnd();
                    break;
                }

                // 刷新 softkey 和 custom key
                if (_EDKDataMgr.LoadEDKConfigXml())
                {
                    AdapterBox_UpdateSoftKey();
                    dsskey_RefressDsskeyByType(DT_CUSTOM_KEY);
                }
            }
        }
        break;
    default:
        {
            bHandled = FALSE;
        }
        break;
    }

    return bHandled ? 1 : 0;
}

void CEdkDataManager::Init()
{
    LoadEDKConfigXml();
}

void CEdkDataManager::GetSoftKeyDataByUIType(EDKUITYPE eUIType, listEdkSoftKey & listSoftKey)
{
    MAPSOFTKEYITER iter = m_mapSoftKey.find(eUIType);
    if (iter != m_mapSoftKey.end())
    {
        listSoftKey = iter->second;
    }
}

bool CEdkDataManager::LoadEDKConfigXml()
{
    if (!IsEnable())
    {
        Clear();
        return true;
    }

    // 清空数据
    Clear();

    // 解析 xml
    xml_document doc;
    if (!doc.load_file(_XMLFILEPATH))
    {
        return false;
    }
    xml_node nodeRoot = doc.child(NODE_ROOT);

    //maco
    xml_node nodeMaco = nodeRoot.child(NODE_MACO);
    edk_macro_data_t macroData;
    for (xml_node node = nodeMaco.first_child(); node; node = node.next_sibling())
    {
        // 宏名为空,则宏不生效
        yl::string strMacroName = node.attribute(ATTRI_NAME).value();
        if (strMacroName.length() <= 0)
        {
            continue;
        }

        macroData.m_iIndex = atoi(node.attribute(ATTRI_INDEX).value());
        macroData.m_bEnable = strcmp("1", node.attribute(ATTRI_ENABLE).value()) == 0;
        macroData.m_strAction = node.attribute(ATTRI_ACTION).value();
        // 宏名不能是 “空” 或者 以数字开头，否则用默认名称
        if (commonAPI_IsDigital(strMacroName.substr(0, 1)))
        {
            macroData.m_strName = "edk_macro";
        }
        else
        {
            macroData.m_strName = strMacroName;
        }

        m_listMacro.push_back(macroData);
    }

    //prompt
    xml_node nodePrompt = nodeRoot.child(NODE_PROMPT);
    edk_prompt_data_t promptData;
    for (xml_node node = nodePrompt.first_child(); node; node = node.next_sibling())
    {
        promptData.clear();

        promptData.m_iIndex = atoi(node.attribute(ATTRI_INDEX).value());
        promptData.m_bEnable = strcmp("1", node.attribute(ATTRI_ENABLE).value()) == 0;
        promptData.m_strLabel = node.attribute(ATTRI_LABEL).value();
        // 判断是否是需要隐藏
        promptData.m_bMasked = strcmp("masked", node.attribute(ATTRI_MASKED).value()) == 0;

        // 获取默认输入法,默认是text
        yl::string strValue = node.attribute(ATTRI_INPUT_TYPE).value();
        if (strcmp(strValue.c_str(), INPUT_TYPE_NUM) == 0)
        {
            promptData.m_eInputType = EDK_INPUT_NUMERIC;
        }
        else if (strcmp(strValue.c_str(), INPUT_TYPE_TEXT) == 0)
        {
            promptData.m_eInputType = EDK_INPUT_TEXT;
        }

        // 获取prompt标题
        strValue = node.attribute(ATTRI_TITLE).value();
        if (!strValue.empty())
        {
            promptData.m_strTitle = strValue;
        }

        AddPrompt(promptData);
    }

    //softkey-key
    xml_node nodeSoftkey = nodeRoot.child(NODE_SOFTKEY);
    edk_softkey_data_t dataSoftkey;
    int iIndex = 0;
    for (xml_node node = nodeSoftkey.first_child(); node; node = node.next_sibling(), ++iIndex)
    {
        if (iIndex >= SOFTKEY_SIZE)
        {
            break;
        }

        dataSoftkey.m_bEnable = strcmp("1", node.attribute(ATTRI_ENABLE).value()) == 0;
        dataSoftkey.m_eType = GetEdkSoftKeyType(atoi(node.attribute(ATTRI_INDEX).value()));
        dataSoftkey.m_strAction = node.attribute(ATTRI_ACTION).value();
        dataSoftkey.m_iKeyPos = atoi(node.attribute(ATTRI_INSERT).value()) - 1;
        dataSoftkey.m_strLabel = node.attribute(ATTRI_LABEL).value();
        dataSoftkey.m_strSoftkeyID = node.attribute(ATTRI_SOFTKEYID).value();
        dataSoftkey.m_strSoftkeyID = "#" + dataSoftkey.m_strSoftkeyID;
#if 0
        EDK_INFO("+++++++++++++++Index[%d], SoftkeyID[%s]", iIndex, dataSoftkey.m_strSoftkeyID.c_str());
#endif
        if (!dataSoftkey.m_bEnable)
        {
            continue;
        }

        m_listSoftkey.push_back(dataSoftkey);

        // 解析各个UI字段
        xml_node uiNnode = node.child(NODE_UI);

        // 判断各个UI界面的开关
        if (atoi(uiNnode.attribute(UI_IDLE).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_IDLE, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_INCOMING_CALL).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_INCOMING_CALL, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_CONNECTING).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_CONNECTING, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_TRANSFER_CONNECTING).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_TRANSFER_CONNECTING, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_ON_TALK).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_ON_TALK, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_CALL_FAILED).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_CALL_FAILED, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_RING_BACK).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_RING_BACK, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_TRANFER_RING_BACK).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_TRANFER_RING_BACK, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_TALK_HOLD).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_TALK_HOLD, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_TALK_HELD).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_TALK_HELD, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_CONFERENCED).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_CONFERENCED, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_DIALTONE).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_DIALTONE, dataSoftkey);
        }
        if (atoi(uiNnode.attribute(UI_DIALING).value()) == 1)
        {
            AddDataToSoftkeyMap(UI_DIALING, dataSoftkey);
        }
    }

    // 对position进行排序,避免插入softkey是位置错误
    Sort(m_listSoftkey, Compare);

    MAPSOFTKEYITER iter = m_mapSoftKey.begin();
    for (; iter != m_mapSoftKey.end(); iter++)
    {
        Sort(iter->second, Compare);
    }

#if 0

    // 打印解析结果
    EDK_INFO("*************** Macro ******************\n");
    LISTEDKMACROITER iterMacro = m_listMacro.begin();
    for (iterMacro; iterMacro != m_listMacro.end(); iterMacro++)
    {
        EDK_INFO("m_iIndex[%d] m_bEnable[%d] m_strName[%s] m_strAction[%s]\n",
                 (*iterMacro).m_iIndex, (*iterMacro).m_bEnable,
                 (*iterMacro).m_strName.c_str(), (*iterMacro).m_strAction.c_str());
    }

    EDK_INFO("*************** prompt ******************\n");
    LISTEDKPROMPTITER iterPrompt = m_listPrompt.begin();
    for (iterPrompt; iterPrompt != m_listPrompt.end(); iterPrompt++)
    {
        EDK_INFO("m_iIndex[%d] m_bEnable[%d] m_strLabel[%s] m_iMaxInput[%d] m_bMasked[%d]\n",
                 (*iterPrompt).m_iIndex, (*iterPrompt).m_bEnable,
                 (*iterPrompt).m_strLabel.c_str(), (*iterPrompt).m_iMaxInput,
                 (*iterPrompt).m_bMasked);
    }

    EDK_INFO("*************** softkey list******************\n");
    LISTEDKSOFTKEYITER iterSoftList = m_listSoftkey.begin();
    for (; iterSoftList != m_listSoftkey.end(); iterSoftList++)
    {
        EDK_INFO("m_iIndex[%d] m_bEnable[%d] m_iKeyPos[%d] m_strLabel[%s] m_strAction[%s]\n",
                 (*iterSoftList).m_iIndex, (*iterSoftList).m_bEnable, (*iterSoftList).m_iKeyPos,
                 (*iterSoftList).m_strLabel.c_str(), (*iterSoftList).m_strAction.c_str());
    }

    EDK_INFO("*************** softkey map******************\n");
    MAPSOFTKEYITER iterSoftKey = m_mapSoftKey.begin();

    for (iterSoftKey; iterSoftKey != m_mapSoftKey.end(); iterSoftKey++)
    {
        EDK_INFO("#######\n");
        listEdkSoftKey tmpList = iterSoftKey->second;
        LISTEDKSOFTKEYITER tmpIter = tmpList.begin();
        for (tmpIter; tmpIter != tmpList.end(); tmpIter++)
        {
            EDK_INFO("UI[%d]: m_iIndex[%d] m_bEnable[%d] m_iKeyPos[%d] m_strLabel[%s] m_strAction[%s]\n",
                     iterSoftKey->first, (*tmpIter).m_iIndex, (*tmpIter).m_bEnable,
                     (*tmpIter).m_iKeyPos, (*tmpIter).m_strLabel.c_str(),
                     (*tmpIter).m_strAction.c_str());
        }
    }
#endif

    return true;
}

EDKSOFTKEYTYPE CEdkDataManager::GetEdkSoftKeyType(int iIndex)
{
    if (iIndex == 1)
    {
        return EDK_SOFTKEY_1;
    }
    else if (iIndex == 2)
    {
        return EDK_SOFTKEY_2;
    }
    else if (iIndex == 3)
    {
        return EDK_SOFTKEY_3;
    }
    else if (iIndex == 4)
    {
        return EDK_SOFTKEY_4;
    }
    else if (iIndex == 5)
    {
        return EDK_SOFTKEY_5;
    }
    else if (iIndex == 6)
    {
        return EDK_SOFTKEY_6;
    }
    else if (iIndex == 7)
    {
        return EDK_SOFTKEY_7;
    }
    else if (iIndex == 8)
    {
        return EDK_SOFTKEY_8;
    }
    else if (iIndex == 9)
    {
        return EDK_SOFTKEY_9;
    }
    else if (iIndex == 10)
    {
        return EDK_SOFTKEY_10;
    }
    else
    {
        return EDK_SOFTKEY_NON;
    }
}

bool CEdkDataManager::AddDataToSoftkeyMap(const yl::string & strUIType,
        const edk_softkey_data_t & dataSoftKey)
{
    EDKUITYPE eUIType = MapEdkUIType(strUIType);
    if (eUIType == EDK_UI_NON)
    {
        return false;
    }

    MAPSOFTKEYITER iter = m_mapSoftKey.find(eUIType);
    if (iter != m_mapSoftKey.end())
    {
        // 如果在当前的map 中找到了UI类型，则在列表中添加softkey数据
        iter->second.push_back(dataSoftKey);
    }
    else
    {
        // 如果没有找，则添加UI类型，和softkey列表
        listEdkSoftKey listSoftKey;
        listSoftKey.push_back(dataSoftKey);
        m_mapSoftKey.put(eUIType, listSoftKey);
    }

    return true;
}

EDKUITYPE CEdkDataManager::MapEdkUIType(const yl::string & strUIType)
{
    if (strUIType == UI_IDLE)
    {
        return EDK_IDLE;
    }
    else if (strUIType == UI_INCOMING_CALL)
    {
        return EDK_INCOMING_CALL;
    }
    else if (strUIType == UI_CONNECTING)
    {
        return EDK_CONNECTING;
    }
    else if (strUIType == UI_TRANSFER_CONNECTING)
    {
        return EDK_TRANSFER_CONNECTING;
    }
    else if (strUIType == UI_ON_TALK)
    {
        return EDK_ON_TALK;
    }
    else if (strUIType == UI_CALL_FAILED)
    {
        return EDK_CALL_FAILED;
    }
    else if (strUIType == UI_RING_BACK)
    {
        return EDK_RING_BACK;
    }
    else if (strUIType == UI_TRANFER_RING_BACK)
    {
        return EDK_TRANFER_RING_BACK;
    }
    else if (strUIType == UI_TALK_HOLD)
    {
        return EDK_TALK_HOLD;
    }
    else if (strUIType == UI_TALK_HELD)
    {
        return EDK_TALK_HELD;
    }
    else if (strUIType == UI_CONFERENCED)
    {
        return EDK_CONFERENCED;
    }
    else if (strUIType == UI_DIALTONE)
    {
        return EDK_DIALTONE;
    }
    else if (strUIType == UI_DIALING)
    {
        return EDK_DIALING;
    }
    else
    {
        return EDK_UI_NON;
    }
}

bool CEdkDataManager::GetMacroByName(const yl::string & strName, edk_macro_data_t & sMacroData)
{
    LISTEDKMACROITER iter = m_listMacro.begin();
    for (iter; iter != m_listMacro.end(); iter++)
    {
        edk_macro_data_t & tmpData = *iter;
        if (tmpData.m_strName == strName)
        {
            sMacroData = tmpData;
            return true;
        }
    }
    return false;
}

bool CEdkDataManager::GetPromptByIndex(int iIndex, edk_prompt_data_t & sPromptData)
{
    LISTEDKPROMPTITER iter = m_listPrompt.begin();
    for (iter; iter != m_listPrompt.end(); iter++)
    {
        edk_prompt_data_t & tmpData = *iter;
        if (tmpData.m_iIndex == iIndex)
        {
            sPromptData = tmpData;
            return true;
        }
    }
    return false;
}

bool CEdkDataManager::SetPrompt(const edk_prompt_data_t & sPromptData)
{
    LISTEDKPROMPTITER iter = m_listPrompt.begin();
    for (iter; iter != m_listPrompt.end(); iter++)
    {
        edk_prompt_data_t & sTmpPromptData = *iter;
        if (sPromptData.m_iIndex == sTmpPromptData.m_iIndex)
        {
            sTmpPromptData.m_bEnable = sPromptData.m_bEnable;
            sTmpPromptData.m_bMasked = sPromptData.m_bMasked;
            sTmpPromptData.m_iMaxInput = sPromptData.m_iMaxInput;
            sTmpPromptData.m_strLabel = sPromptData.m_strLabel;
            return true;
        }
    }
    return false;
}

/*
** 没有保存在配置文件的prompt生成临时下标,从50开始
*/
int CEdkDataManager::GetFreePromptIndex()
{
    // 配置文件不存在等异常,list会为空
    if (m_listPrompt.size() == 0)
    {
        return 50;
    }

    edk_prompt_data_t & sTmpPromptData = m_listPrompt.back();

    if (sTmpPromptData.m_iIndex >= 50)
    {
        int iIndex = sTmpPromptData.m_iIndex;

        return ++iIndex;
    }

    return 50;
}

/*
** 添加弹出框到链表
*/
bool CEdkDataManager::AddPrompt(edk_prompt_data_t & edkPromptData)
{
    for (LISTEDKPROMPTITER iter = m_listPrompt.begin();
            iter != m_listPrompt.end();
            iter ++)
    {
        edk_prompt_data_t & sTempPromptData = *iter;

        if (sTempPromptData.m_iIndex == edkPromptData.m_iIndex)
        {
            return false;
        }
    }

    m_listPrompt.push_back(edkPromptData);

    if (edkPromptData.m_bEnable)
    {
        EDK_INFO("[Add prompt][Label:%s][Title:%s][Max char:%d][Numberic:%d][Mask:%d]",
                 edkPromptData.m_strLabel.c_str(),
                 edkPromptData.m_strTitle.c_str(),
                 edkPromptData.m_iMaxInput,
                 edkPromptData.m_eInputType,
                 edkPromptData.m_bMasked);
    }

    return true;
}

/*
** 通过type获取softkey data
*/
bool CEdkDataManager::GetSoftkeyData(EDKSOFTKEYTYPE eType, edk_softkey_data_t & strSoftkey)
{
    for (LISTEDKSOFTKEYITER iter = m_listSoftkey.begin();
            iter != m_listSoftkey.end();
            iter ++)
    {
        edk_softkey_data_t & edkSoftkey = *iter;
        if (edkSoftkey.m_eType == eType)
        {
            strSoftkey = edkSoftkey;

            return true;
        }
    }

    return false;
}

/*
** 通过type获取softkey id
*/
bool CEdkDataManager::GetSoftkeyID(EDKSOFTKEYTYPE eType, yl::string & strID)
{
    edk_softkey_data_t edkSoftkey;

    if (!GetSoftkeyData(eType, edkSoftkey))
    {
        return false;
    }

    strID = edkSoftkey.m_strSoftkeyID;

    return true;
}

/*
** 通过type获取softkey label
*/
bool CEdkDataManager::GetSoftKeyLabel(EDKSOFTKEYTYPE eType, yl::string & strLabel)
{
    edk_softkey_data_t edkSoftkey;

    if (!GetSoftkeyData(eType, edkSoftkey))
    {
        return false;
    }
    strLabel = edkSoftkey.m_strLabel;

    return true;
}

/*
** 通过type获取softkey action
*/
bool CEdkDataManager::GetSoftKeyAction(EDKSOFTKEYTYPE eType, yl::string & strAction)
{
    edk_softkey_data_t edkSoftkey;

    if (!GetSoftkeyData(eType, edkSoftkey))
    {
        return false;
    }
    strAction = edkSoftkey.m_strAction;

    return true;
}

/*
** 判断是否为custom key
*/
bool CEdkDataManager::IsCustomSoftkey(const yl::string & strSoftkeyID)
{
    edk_softkey_data_t edkSoftkey;

    if (GetSoftkeyDataByID(strSoftkeyID, edkSoftkey))
    {
        return true;
    }

    return false;
}

bool CEdkDataManager::IsEnable()
{
    return (1 == configParser_GetConfigInt(kszEdkSwitch));
}

void CEdkDataManager::Clear()
{
    m_listMacro.clear();
    m_listPrompt.clear();
    m_listSoftkey.clear();
    m_mapSoftKey.clear();
}

/*
** 通过softkey id获取type
*/
EDKSOFTKEYTYPE CEdkDataManager::GetSoftkeyTypeByID(const yl::string & strSoftkeyID)
{
    edk_softkey_data_t edkSoftkey;

    if (!GetSoftkeyDataByID(strSoftkeyID, edkSoftkey))
    {
        return EDK_SOFTKEY_NON;
    }

    return edkSoftkey.m_eType;
}

/*
** 通过softkey id获取softkey data
*/
bool CEdkDataManager::GetSoftkeyDataByID(const yl::string & strSoftkeyID,
        edk_softkey_data_t & edkSoftkey)
{
    for (LISTEDKSOFTKEYITER iter = m_listSoftkey.begin();
            iter != m_listSoftkey.end();
            iter ++)
    {
        edk_softkey_data_t & edkSoftkeyData = *iter;

        yl::string strSrc = edkSoftkeyData.m_strSoftkeyID;
        yl::string strDest = strSoftkeyID;
        if (strSrc.to_lower() == strDest.to_lower())
        {
            edkSoftkey = edkSoftkeyData;

            return true;
        }
    }

    return false;
}

// 清空临时prompt
void CEdkDataManager::ClearTempPromt()
{
    while (m_listPrompt.size() != 0)
    {
        const edk_prompt_data_t & edkPrompt = m_listPrompt.back();

        if (edkPrompt.m_iIndex >= 50)
        {
            m_listPrompt.pop_back();
            continue;
        }

        break;
    }
}

#endif //IF_FEATURE_EDK
