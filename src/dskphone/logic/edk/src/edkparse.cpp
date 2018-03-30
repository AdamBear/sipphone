///////////////////////////////////////////////////////////
//  CEdkParse.cpp
//  Implementation of the Class CEdkParse
//  Created on:      21-一月-2015 15:32:26
//  Original author: dlw
///////////////////////////////////////////////////////////

#include "edkparse.h"
#include "edkactionmanager.h"
#include <commonapi/stringhelperapi.h>
// 命令分隔符
#define COMMAND_SPLIT   '$'
#define DOLLAR_SPLIT    '$'

// 命令头
#define HEAD_MACRO      "$M"
#define HEAD_COMMAND    "C"
#define HEAD_TYPE       "T"
#define HEAD_PROMPT "P"
#define HEAD_SPEEDDIAL  "S"

#if IF_FEATURE_EDK
IMPLEMENT_GETINSTANCE(CEdkParse)

CEdkParse::CEdkParse()
{
}

CEdkParse::~CEdkParse()
{

}

/**
 * 打印信息
 */
yl::string MapType(EDKACTIONTYPE eActionType)
{
    yl::string strRet = "";
    switch (eActionType)
    {
    case EDK_TYPE_NON:
        strRet = "EDK_TYPE_NON";
        break;
    case EDK_INVITE:
        strRet = "EDK_INVITE";
        break;
    case EDK_REFER:
        strRet = "EDK_REFER";
        break;
    case EDK_DTMF:
        strRet = "EDK_DTMF";
        break;
    case EDK_POPUP:
        strRet = "EDK_POPUP";
        break;
    case EDK_WC:
        strRet = "EDK_WC";
        break;
    case EDK_PAUSE:
        strRet = "EDK_PAUSE";
        break;
    case EDK_HOLD:
        strRet = "EDK_HOLD";
        break;
    case EDK_HANG:
        strRet = "EDK_HANG";
        break;
    case EDK_URL:
        strRet = "EDK_URL";
        break;
    case EDK_DIGIT:
        strRet = "EDK_DIGIT";
        break;
    case EDK_ENTER_MENU:
        strRet = "EDK_ENTER_EMNU";
        break;
    case EDK_SOFTKEY:
        strRet = "EDK_SOFTKEY";
        break;
    case EDK_HARDKEY:
        strRet = "EDK_HARDKEY";
        break;
    case EDK_LABEL:
        strRet = "EDK_LABEL";
        break;
    case EDK_LED_CONTROL:
        strRet = "EDK_LED_CONTROL";
        break;
    case EDK_INTERCOM:
        strRet = "EDK_INTERCOM";
        break;
    default:
        break;
    }
    return strRet;
}

/**
 * 打印信息
 */
void PrintCommand(listEdkAction & listAction)
{
    for (LISTEDKACTIONITER iter = listAction.begin();
            iter != listAction.end(); ++iter)
    {
        EDK_INFO("EDKAction Type[%s] Cmd[%s]", MapType((*iter).m_eActionType).c_str(),
                 ((*iter).m_strAction).c_str());
    }
}

/**
 * 判断命令是否是合法的dtmf字符串
 */
bool IsValidDtmf(const yl::string & strKey)
{
    if (strKey.empty())
    {
        return false;
    }

    for (int i = 0; i < strKey.length(); i++)
    {
        char cFirst = strKey.at(i);
        if (cFirst == '*' || cFirst == '#'
                || cFirst == 'A' || cFirst == 'B'
                || cFirst == 'C' || cFirst == 'D'
                || cFirst == '+')
        {
            continue;
        }

        if (isdigit(cFirst) == 0)
        {
            return false;
        }
    }

    return true;
}

/**
 * 解析 XML 中的 action 字段
 * 返回值：true[正确解析] false[action内容有误]
 */
bool CEdkParse::ParseAction(const yl::string & strContent, int iDsskeyId/* = -1*/)
{
    EDK_INFO("ParseAction start[%s] DsskeyId[%d]", strContent.c_str(), iDsskeyId);

    // 解析命令
    YLList<int> listMacro;
    YLList<yl::string> listSoftkey;
    InitObjAction();

    if (!ParseCommand(strContent, listMacro, listSoftkey, true, iDsskeyId))
    {
        EDK_INFO("Parse action invalid action");
        return false;
    }

    if (!MergeCommand())
    {
        EDK_INFO("Merge action fail");
        return false;
    }

    // 移除不需要执行的action类型
    RemoveActionByType(EDK_LABEL);

    // 直接把解析完的命令结构体给actionmanager
    bool bRet = DoAction();

    EDK_INFO("ParseAction end[%d]", bRet);
    return bRet;
}

/*
** 解析已生成的命令列表，合并相同命令
*/
bool CEdkParse::MergeCommand()
{
    listEdkAction::iterator it = m_objActionInfo.m_listAction.begin();
    edk_action_data_t * pPreAction = NULL;

    for (int iIndex = 0; it != m_objActionInfo.m_listAction.end();)
    {
        edk_action_data_t & actionData = (*it);

        if (EDK_POPUP == actionData.m_eActionType)
        {
            // 弹出去，与上一个命令合并，生成预处理命令列表
            int iPromptIndex = 0, iMaxInput = 0;
            if (sscanf(actionData.m_strAction.c_str(), "P%dN%d", &iPromptIndex, &iMaxInput) != 2)
            {
                EDK_INFO("Parse prompt errro");
                return false;
            }

            // 取出弹出框的信息
            edk_prompt_data_t edkPrompt;
            if (!_EDKDataMgr.GetPromptByIndex(iPromptIndex, edkPrompt)
                    || !edkPrompt.m_bEnable)
            {
                EDK_INFO("Get prompt data[%d] error", iPromptIndex);
                return false;
            }

            edkPrompt.m_iMaxInput = iMaxInput;
            if (edkPrompt.m_iMaxInput > 99
                    || edkPrompt.m_iMaxInput <= 0)
            {
                edkPrompt.m_iMaxInput = 99;
            }

            _EDKDataMgr.SetPrompt(edkPrompt);

            // 拼凑弹出框命令
            edk_preaction_data_t sPreActionData;
            char chBuffer[48] = {0};
            sprintf(chBuffer, "%d", iPromptIndex);
            sPreActionData.m_sActionData.m_eActionType = EDK_POPUP;
            sPreActionData.m_sActionData.m_strAction = chBuffer;

            // 如果上一个是数字，则与数字命令合并
            if (NULL != pPreAction
                    && EDK_DIGIT == pPreAction->m_eActionType)
            {
                sPreActionData.m_iIndex = iIndex - 1;
                m_objActionInfo.m_listPreAction.push_back(sPreActionData);
                it = m_objActionInfo.m_listAction.erase(it);
                continue;
            }

            actionData.m_eActionType = EDK_DIGIT;
            actionData.m_strAction = "";

            sPreActionData.m_iIndex = iIndex;
            m_objActionInfo.m_listPreAction.push_back(sPreActionData);
        }
        else if (EDK_INVITE == actionData.m_eActionType
                 || EDK_REFER == actionData.m_eActionType
                 || EDK_DTMF == actionData.m_eActionType
                 || EDK_INTERCOM == actionData.m_eActionType)
        {
            // 与上一次的数字命令合并
            if (NULL == pPreAction
                    || pPreAction->m_eActionType != EDK_DIGIT)
            {
                return false;
            }
            // 将自己删除
            pPreAction->m_eActionType = actionData.m_eActionType;
            it = m_objActionInfo.m_listAction.erase(it);
            continue;
        }

        // 记录前一个命令
        pPreAction = &(*it);
        ++it;
        iIndex++;
    }

    return true;
}

/*
** 解析命令流
*/
bool CEdkParse::ParseActionString(yl::string & strAction, YLList<int> & listMacro,
                                  YLList<yl::string> & listSoftkey, int iDsskeyId/* = -1*/)
{
    EDK_INFO("ParseActionString = %s, DsskeyId = %d", strAction.c_str(), iDsskeyId);

    edk_action_data_t edkAction;
    bool bHandleSucc = true;

    if (IsValidDtmf(strAction))
    {
        edkAction.m_eActionType = EDK_DIGIT;
        edkAction.m_strAction = strAction;
    }
    else if (strncmp(strAction.c_str(), "T", 1) == 0)
    {
        bHandleSucc = ParseTypeAction(strAction, edkAction);
    }
    else if (strncmp(strAction.c_str(), "C", 1) == 0)
    {
        bHandleSucc = ParseCommandAction(strAction, edkAction);
    }
    else if (strncmp(strAction.c_str(), "S", 1) == 0)
    {
        yl::string strCmd = strAction.substr(1);

        edkAction.m_eActionType = EDK_SOFTKEY;
        edkAction.m_strAction = strCmd.to_lower();

        // 自定义softkey 则展开
        if (_EDKDataMgr.IsCustomSoftkey(edkAction.m_strAction))
        {
            if (CheckSoftkeyReCall(strCmd, listSoftkey))
            {
                return false;
            }

            m_objActionInfo.m_listAction.push_back(edkAction);
            listSoftkey.push_back(strCmd);

            edk_softkey_data_t edkSoftkey;
            _EDKDataMgr.GetSoftkeyDataByID(edkAction.m_strAction, edkSoftkey);
            EDK_INFO(" ------ softkey Action[%s]", edkSoftkey.m_strAction.c_str());
            return ParseCommand(edkSoftkey.m_strAction, listMacro, listSoftkey, true, iDsskeyId);
        }
    }
    else if (strncmp(strAction.c_str(), "I", 1) == 0)
    {
        yl::string strCmd = strAction.substr(1);

        edkAction.m_eActionType = EDK_ENTER_MENU;
        edkAction.m_strAction = strCmd.to_lower();
    }
    else if (strncmp(strAction.c_str(), "K", 1) == 0)
    {
        yl::string strCmd = strAction.substr(1);

        edkAction.m_eActionType = EDK_HARDKEY;
        edkAction.m_strAction = strCmd;
    }
    // 弹出框
    else if (strncmp(strAction.c_str(), "P", 1) == 0)
    {
        bHandleSucc = ParsePromptAction(strAction, edkAction);
    }
    else if (strncmp(strAction.c_str(), "LED", 3) == 0)
    {
        yl::string strCmd = strAction.substr(3);

        edkAction.m_eActionType = EDK_LED_CONTROL;
        edkAction.m_strAction = strCmd;
        edkAction.m_iDsskeyId = iDsskeyId;
        EDK_INFO("EDK LED:%s", strCmd.c_str());

        if (!_EdkActionManager.ParseActionByType(edkAction))
        {
            return false;
        }
    }
    else if (strncmp(strAction.c_str(), "LB", 2) == 0)
    {
        yl::string strCmd = strAction.substr(2);

        edkAction.m_eActionType = EDK_LABEL;
        edkAction.m_strAction = strCmd;
    }
    else if (strncmp(strAction.c_str(), "M", 1) == 0
             || strncmp(strAction.c_str(), "!", 1) == 0)
    {
        edk_macro_data_t edkMacro;
        strAction = strAction.substr(1);
        if (!_EDKDataMgr.GetMacroByName(strAction, edkMacro))
        {
            return false;
        }

        // 如果宏嵌套的话，直接返回，避免死循环
        if (CheckMacroReCall(edkMacro.m_iIndex, listMacro))
        {
            return false;
        }
        if (!edkMacro.m_bEnable)
        {
            return false;
        }

        listMacro.push_back(edkMacro.m_iIndex);
        return ParseCommand(edkMacro.m_strAction, listMacro, listSoftkey, true, iDsskeyId);
    }
    else
    {
        bHandleSucc = false;
    }

    if (!bHandleSucc)
    {
        return false;
    }

    EDK_INFO("############## Action: [EdkActionType:%d][ActionString:%s]", edkAction.m_eActionType,
             edkAction.m_strAction.c_str());
    m_objActionInfo.m_listAction.push_back(edkAction);
    //m_listAction.push_back(edkAction);
    return true;
}

/**
 * 执行命令流
 */
bool CEdkParse::DoAction()
{
    PrintCommand(m_objActionInfo.m_listAction);
    // 让命令执行类开始执行命令流
    return _EdkActionManager.DoAction(m_objActionInfo);
}

bool CEdkParse::CheckMacroReCall(int iIndex, YLList<int> & listMacro)
{
    for (YLList<int>::ListIterator iter = listMacro.begin();
            iter != listMacro.end(); iter++)
    {
        if (iIndex == *iter)
        {
            EDK_INFO("EDK MacroReCall");
            return true;
        }
    }

    return false;
}

bool CEdkParse::CheckSoftkeyReCall(const yl::string & strSoftkeyID,
                                   YLList<yl::string> & listSoftkey)
{
    for (YLList<yl::string>::ListIterator iter = listSoftkey.begin();
            iter != listSoftkey.end(); iter++)
    {
        if (strSoftkeyID == *iter)
        {
            EDK_INFO("EDK SoftkeyReCall");
            return true;
        }
    }

    return false;
}

/*
** 解析命令
** 参数:bNeedInterruptAfterFault  解析出错后是否中断
*/
bool CEdkParse::ParseCommand(const yl::string & strAction, YLList<int> & listMacro,
                             YLList<yl::string> & listSoftkey,
                             bool bNeedInterruptAfterFault/* = true*/, int iDsskeyId/* = -1*/)
{
    if (strAction.empty())
    {
        return true;
    }

    yl::string strContent = strAction;
    /*  char *pToken = strtok(strContent.c_str(), "$");
        if (NULL == pToken)
        {
            return ParseActionString(action)
        }

        edk_action_data_t edkAction;
        while (NULL != pToken)
        {
            ParseActionString(pToken);
            pToken = strtok(NULL, "$");
        }
    */
    // 不带$的处理流程
    if (NULL == strchr(strContent.c_str(), '$'))
    {
        return ParseActionString(strContent, listMacro, listSoftkey, iDsskeyId);
    }

    // 带$的处理流程
    int iLength = strContent.length();
    char * pPosLast = (char *)strContent.c_str();
    char * pPosNew = pPosLast;

    while (1)
    {
        // 如果是最后一个字符
        if (pPosLast - strContent.c_str() + 1 == iLength)
        {
            pPosNew = pPosLast;
        }
        else
        {
            // 在上一个的基础上查找以$包含的字符串
            pPosNew = strchr(pPosLast + 1, '$');
            if (NULL == pPosNew)
            {
                pPosNew = &strContent[strContent.length() - 1];
            }
            else
            {
                if ((pPosLast[0] != '$'))
                {
                    pPosNew = &strContent[pPosNew - strContent.c_str() - 1];
                }
            }
        }

        // 取出当前的命令内容
        yl::string strCommand = strContent.substr(pPosLast - strContent.c_str(), pPosNew - pPosLast + 1);

        // 判断命令是否有效，是否需要$包含
        if (!IsActionValid(strCommand.c_str()))
        {
            return false;
        }

        strCommand.trim_both("$");

        EDK_INFO("Get command = %s", strCommand.c_str());


        // 如果非空，则创建命令结构
        if (strCommand.length() != 0)
        {
            if (!ParseActionString(strCommand, listMacro, listSoftkey, iDsskeyId)
                    && bNeedInterruptAfterFault)
            {
                EDK_INFO("Parse action error");
                return false;
            }
        }

        // 遍历完成
        if (pPosNew - strContent.c_str() + 1 >= iLength)
        {
            break;
        }

        pPosLast = pPosNew + 1;
    }

    return true;
}

/*
** 判断当前命令是否完整
*/
bool CEdkParse::IsActionComplete()
{
    if (m_objActionInfo.m_listAction.size() == 0)
    {
        return true;
    }

    edk_action_data_t & lastEDK = m_objActionInfo.m_listAction.back();
    if (EDK_TYPE_NON == lastEDK.m_eActionType
            || EDK_DIGIT == lastEDK.m_eActionType)
    {
        return false;
    }

    return true;
}

// 命令是否需要用$包含
bool CEdkParse::IsActionWithDollar(const yl::string & strAction)
{
    if (IsValidDtmf(strAction))
    {
        return false;
    }

    return true;
}

// 命令是否合法
bool CEdkParse::IsActionValid(const yl::string & strAction)
{
    if (strAction.empty())
    {
        return false;
    }

    if (IsActionWithDollar(strAction))
    {
        if (strAction.length() <= 1)
        {
            return false;
        }

        if (strAction.at(0) != '$'
                || strAction.at(strAction.length() - 1) != '$')
        {
            return false;
        }
    }
    else
    {
        if (strAction.at(0) == '$'
                || strAction.at(strAction.length() - 1) == '$')
        {
            return false;
        }
    }

    return true;
}
#endif

/*
** 解析命令：Type
*/
bool CEdkParse::ParseTypeAction(yl::string & strAction, edk_action_data_t & edkAction)
{
    strAction = strAction.substr(1);

    if (strcmp(strAction.c_str(), "invite") == 0)
    {
        edkAction.m_eActionType = EDK_INVITE;
    }
    else if (strcmp(strAction.c_str(), "refer") == 0)
    {
        edkAction.m_eActionType = EDK_REFER;
    }
    else if (strcmp(strAction.c_str(), "dtmf") == 0)
    {
        edkAction.m_eActionType = EDK_DTMF;
    }
    else if (strcmp(strAction.c_str(), "intercom") == 0)
    {
        edkAction.m_eActionType = EDK_INTERCOM;
    }
    else
    {
        return false;
    }

    return true;
}

/*
** 解析命令：Command
*/
bool CEdkParse::ParseCommandAction(yl::string & strAction, edk_action_data_t & edkAction)
{
    yl::string strCmd = strAction.substr(1);

    if (strCmd == "h" || strCmd == "hold")
    {
        edkAction.m_eActionType = EDK_HOLD;
    }
    else if (strCmd == "hu" || strCmd == "hang")
    {
        edkAction.m_eActionType = EDK_HANG;
    }
    else if (strCmd == "wc" || strCmd == "waitconnect")
    {
        edkAction.m_eActionType = EDK_WC;
    }
    else if (strncmp(strCmd.c_str(), "pause", 5) == 0)
    {
        strCmd = strCmd.substr(5);
        if (atoi(strCmd.c_str()) <= 0 || atoi(strCmd.c_str()) > 10)
        {
            return false;
        }

        edkAction.m_eActionType = EDK_PAUSE;
        edkAction.m_strAction = strCmd;
    }
    else if (strncmp(strCmd.c_str(), "p", 1) == 0)
    {
        strCmd = strCmd.substr(1);
        if (atoi(strCmd.c_str()) <= 0 || atoi(strCmd.c_str()) > 10)
        {
            return false;
        }

        edkAction.m_eActionType = EDK_PAUSE;
        edkAction.m_strAction = strCmd;
    }
    else
    {
        return false;
    }

    return true;
}

/*
** 解析命令：Prompt
*/
bool CEdkParse::ParsePromptAction(yl::string & strAction, edk_action_data_t & edkAction)
{
    bool bHandleSucc = false;
    int iPromptIndex, iMaxInput;
    edk_prompt_data_t edkPrompt;

    if (sscanf(strAction.c_str(), "P%dN%d", &iPromptIndex, &iMaxInput) == 2)
    {
        bHandleSucc = true;
    }
    else
    {
        // 命令格式:P<label>&T<title>&C<characters number allowed>&N&M
        // 解析并构造一个prompt
        yl::string::size_type nPos = strAction.find_first_of('&');

        if (nPos == yl::string::npos)
        {
            edkPrompt.m_strLabel = strAction.substr(1);
        }
        else
        {
            edkPrompt.m_strLabel = strAction.substr(1, nPos - 1);

            yl::string strParam = strAction.substr(nPos + 1);
            yl::string::size_type nLastPos;
            bool bIsEnd = false;

            while (!bIsEnd)
            {
                nPos = strParam.find_first_of('&');
                nLastPos = nPos;

                if (nPos == yl::string::npos)
                {
                    bIsEnd = true;
                    nLastPos = strParam.find_last_of('\0');
                }

                if (strParam.at(0) == 'C')
                {
                    // substr(startPos, Count);
                    yl::string strMaxInput = strParam.substr(0, nLastPos);
                    int iMaxInput = -1;

                    if (sscanf(strMaxInput.c_str(), "C%d", &iMaxInput) != 1)
                    {
                        return false;
                    }

                    edkPrompt.m_iMaxInput = iMaxInput;
                    EDK_DEBUG("[Parse Action][Prompt][MaxInput:%d]", iMaxInput);
                }
                else if (strParam.at(0) == 'T')
                {
                    yl::string strTitle = "";
                    if (strParam.length() <= 1
                            || (strParam.length() > 1 && strParam.at(1) == '&'))
                    {
                        // do nothing
                    }
                    else
                    {
                        edkPrompt.m_strTitle = strParam.substr(1, nLastPos - 1);
                        EDK_DEBUG("[Parse Action][Prompt][Title:%s]",
                                  edkPrompt.m_strTitle.c_str());
                    }
                }
                else if (strParam.at(0) == 'N')
                {
                    edkPrompt.m_eInputType = EDK_INPUT_NUMERIC;
                }
                else if (strParam.at(0) == 'M')
                {
                    edkPrompt.m_bMasked = true;
                }
                else
                {
                    EDK_WARN("[Parse Action][Prompt][Format error]");
                    return false;
                }

                strParam = strParam.substr(nPos + 1);
            }
        }

        edkPrompt.m_bEnable = true;
        edkPrompt.m_iIndex = _EDKDataMgr.GetFreePromptIndex();
        _EDKDataMgr.AddPrompt(edkPrompt);

        char chBuffer[48] = { 0 };
        sprintf(chBuffer, "P%dN%d", edkPrompt.m_iIndex, edkPrompt.m_iMaxInput);
        strAction = chBuffer;
        bHandleSucc = true;
    }

    if (bHandleSucc)
    {
        edkAction.m_eActionType = EDK_POPUP;
        edkAction.m_strAction = strAction;
    }

    return bHandleSucc;
}

/*
** 解析并获取某种type的命令
*/
yl::string CEdkParse::ParseActionForType(const yl::string & strAction, EDKACTIONTYPE eType)
{
    EDK_INFO("Parse Action for type[%d] strat[%s]", eType, strAction.c_str());

    YLList<int> listMacro;
    YLList<yl::string> listSoftkey;
    InitObjAction();

    if (!ParseCommand(strAction, listMacro, listSoftkey, false))
    {
        return "";
    }

    // 获取链表第一个数据
    edk_action_data_t edkActionData;
    GetActionByType(eType, edkActionData);

    return edkActionData.m_strAction;
}

/*
** 通过type从链表中删除相应的action
*/
bool CEdkParse::RemoveActionByType(EDKACTIONTYPE eActionType)
{
    for (LISTEDKACTIONITER iter = m_objActionInfo.m_listAction.begin();
            iter != m_objActionInfo.m_listAction.end();)
    {
        edk_action_data_t & edkActionData = *iter;
        if (edkActionData.m_eActionType == eActionType)
        {
            iter = m_objActionInfo.m_listAction.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    return true;
}

/*
** 通过type从链表中获取相应的action
*/
bool CEdkParse::GetActionByType(EDKACTIONTYPE eActionType, edk_action_data_t & edkActionData)
{
    for (LISTEDKACTIONITER iter = m_objActionInfo.m_listAction.begin();
            iter != m_objActionInfo.m_listAction.end();
            iter++)
    {
        edk_action_data_t & edkAction = *iter;

        if (edkAction.m_eActionType == eActionType)
        {
            edkActionData = *iter;
            return true;
        }
    }

    return false;
}

// 初始化结构体
void CEdkParse::InitObjAction()
{
    m_objActionInfo.clear();
    m_objActionInfo.m_iActionId = ++m_iId;
}
