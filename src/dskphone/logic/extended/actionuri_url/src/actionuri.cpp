#include "actionuri_url_inc.h"
#include "setting_page_define.h"
#include "adapterbox/include/modadapterbox.h"
#include "keyguard/include/modkeyguard.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "exp/include/modexp.h"

namespace NS_AU
{
//////////////////////////////////////////////////////////////////////////
// Class CActionUri.
CActionUri * CActionUri::m_pInst = NULL;

// 命令之间的分割符号.
static const char kszCmdSeparation[] = " ";
// OK键的替换符，目的是将OK作为数字键，以达到连续数字命令的需求：1234OK=>1234~.
static const char kszCmdOKReplacer[] = "~";
// 命令标识符.
static const char kszCmdIdentifier[] = "key=";

static const char kszRemoteIP[] = "&RIP=";
// 缓存最大长度.
static const int knMaxBufferLen = 260;
// 输入号码最大长度.
static const int knMaxDialLen = 32;
#if IF_BUG_27560
// 是否显示呼出呼入信息
static const char kszCmdDisplay[] = "Display=";
#endif

bool CActionUri::SendKeyMsg(int nKeyCode)
{
    if (nKeyCode >= PHONE_KEY_0 && nKeyCode <= PHONE_KEY_9
            || nKeyCode == PHONE_KEY_STAR
            || nKeyCode == PHONE_KEY_POUND
#if IF_FEATURE_DTMF_FLASH
            || nKeyCode >= PHONE_KEY_A && nKeyCode <= PHONE_KEY_E)
#else
            || nKeyCode >= PHONE_KEY_A && nKeyCode <= PHONE_KEY_D)
#endif
    {
        commonUnits_SendSimulateKey(DEV_MSG_DIGITKEY_EVENT, nKeyCode);
    }
    else
    {
        commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, nKeyCode);
    }

    return false;
}

// Static.
CActionUri * CActionUri::GetInstance()
{
    // 创建单键.
    if (m_pInst == NULL)
    {
        m_pInst = new CActionUri();
        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }

    return m_pInst;
}

// Static.
void CActionUri::ReleaseInstance()
{
    // 销毁单键.
    if (m_pInst != NULL)
    {
        m_pInst->MyFinal();

        delete m_pInst;
        m_pInst = NULL;
    }
}

LRESULT CActionUri::OnRequestCTR(msgObject & msg)
{
    bool bAllowThisMsg = accessCtr_Process(msg);
    if (!bAllowThisMsg)
    {
        msg.ReplyMessage(FALSE);
        return TRUE;
    }
    else
    {
        msg.ReplyMessage(TRUE);
    }

    return TRUE;
}

LRESULT CActionUri::OnMessage(msgObject & refObj)
{
    // 如果未获得用户许可,则不处理
    bool bAllowThisMsg = accessCtr_Process(refObj);

    if (!bAllowThisMsg)
    {
        return TRUE;
    }
    LRESULT retVal = FALSE;

    switch (refObj.message)
    {
    case DSK_MSG_ACTION_URI:
    case DSK_MSG_SETPHONE_CFG:
    case SIP_CMD_PASS_ACTION_URI:
        {
            // 解析传过来的字符串.
            if (m_pInst != NULL)
            {
                if (m_pInst->MyParseCode(refObj))
                {
                    retVal = m_pInst->ProcessCmdList() ? TRUE : FALSE;
                }
            }
        }
        break;

    case TM_TIMER:
        {
            g_pActionUri->OnTimer(refObj.wParam);
        }
        break;
    }

    return retVal;
}

bool CActionUri::OnTimer(UINT uTimerId)
{
    if (uTimerId == (UINT)&m_mapCmd)
    {
        if (!talklogic_SessionExist(false))
        {
            timerKillThreadTimer((UINT)&m_mapCmd);
            // 重启.
            backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);

            commonUnits_Reboot(0);
        }
    }

    else if (uTimerId == (UINT)&m_ListCmd)
    {
        ProcessCmdList();
    }

    return true;
}

bool CActionUri::ProcessCmdList()
{
    if (m_ListCmd.size() <= 0)

    {
        return false;
    }

    yl::string strCmd = m_ListCmd.front();

    DoCmd(strCmd);

    ListIter iter = m_ListCmd.begin();
    m_ListCmd.erase(iter);
    if (m_ListCmd.size() <= 0 && m_bIsCmdTimeOn)
    {
        timerKillThreadTimer((UINT)&m_ListCmd);
        m_bIsCmdTimeOn = false;
    }
    else if (m_ListCmd.size() > 0 && !m_bIsCmdTimeOn)
    {
        // 启动命令计时器.
        timerSetThreadTimer((UINT)&m_ListCmd, 50);
        m_bIsCmdTimeOn = true;
    }
    return true;
}

bool CActionUri::DoCmd(const yl::string & strCode)
{

    if (strCode.empty())
    {
        return false;
    }

    // 替换掉OK和POUND键.
    yl::string strCodeReplace = strCode;

    string_replace(strCodeReplace, "POUND", "#");

    if (strCodeReplace.find("HOOK") == yl::string::npos)
    {
        // 判断key as send的类型, 并替换掉的按键为OK键.
        string_replace(strCodeReplace, "OK", kszCmdOKReplacer);
    }

    // 先处理特殊命令,如盲转与咨询转
    if (ProcessTrans(strCodeReplace)
            || ProcessSetForward(strCodeReplace)
            || ProcessRemoteReq(strCodeReplace)
#if IF_BUG_27560
            || ProcessDisplay(strCodeReplace)
#endif
            || ProcessRegisterAcc(strCodeReplace)
            || ProcessLongPress(strCodeReplace))
    {
        return true;
    }

    // 对按键连写进行特殊处理, 如"12345", 应将之分割为"1 2 3 4 5".
    bool isDigitCmd = true;
    yl::string strCodeCpy;
    for (size_t i = 0; i < strCodeReplace.length() && i < knMaxDialLen; ++i)
    {
        yl::string strSingleCode = strCodeReplace.substr(i, 1);
        const CommandData * pCmdData = MyFindCmd(strSingleCode);
        if (pCmdData == NULL || pCmdData->m_eType != DIGITAL_KEY)
        {
            isDigitCmd = false;
            break;
        }
        // 加入命令列表
        strCodeCpy.push_back(strCodeReplace.at(i));
        strCodeCpy.append(kszCmdSeparation);

        if (strSingleCode == kszCmdOKReplacer)
        {
            // 忽略OK之后的命令
            break;
        }

    }  // for.

    // 如果是数字连写, 则进行分割.
    strCodeCpy = isDigitCmd ? strCodeCpy : strCodeReplace;

    // 取出命令列表.
    YLList<yl::string> listCmd;
    if (!commonAPI_splitStringBySeparator(strCodeCpy, ' ', listCmd))
    {
        return false;
    }

    // 执行列表中的每一个命令.
    for (YLList<yl::string>::iterator citer = listCmd.begin();
            citer != listCmd.end();
            citer++)
    {
        const yl::string & strSpecial = *citer;
        if (strSpecial.substr(0, 6) == "number")
        {
            MyDoOperationNumber(strSpecial);
        }
        else
        {
#if IF_BUG_32044
            if (!keyGuard_IsLock() && citer == listCmd.begin() && isDigitCmd) //指令全为数字
            {
                const CommandData * pfistCmdData = MyFindCmd("BackInFullDsskey");
                if (pfistCmdData != NULL)
                {
                    MyDoCmd(*pfistCmdData);
                }
            }
#endif

            const CommandData * pCmdData = MyFindCmd(*citer);
            if (pCmdData != NULL)
            {
                MyDoCmd(*pCmdData);
            }
        }
    }  // for.

    return true;
}

// 处理通话中盲转及咨询转等特殊的命令,如key=ATrans=10000,则该code为ATrans=10000,需要再进行处理
bool CActionUri::ProcessTrans(const yl::string & strCode)
{
    // 查找等号,如果没有的话,则不处理
    size_t uPosFound = strCode.find("=");
    if (uPosFound == strCode.npos)
    {
        return false;
    }

    yl::string strCmd = strCode.substr(0, uPosFound);
    yl::string strVal = strCode.substr(uPosFound + 1);

    yl::string strCallId = "";

    uPosFound = strVal.find(":");

    if (uPosFound != strVal.npos)
    {
        strCallId = strVal.substr(uPosFound + 1);
        strVal = strVal.substr(0, uPosFound);
    }

    // 查找等号前的关键字
    const CommandData * pCmdData = MyFindCmd(strCmd);
    if (NULL == pCmdData)
    {
        ACTION_WARN("code not found");
        return false;
    }

    bool bHandled = true;
    switch (pCmdData->m_nVal)
    {
    case OPEARTION_A_TRANS:
        {
            // 咨询转
            backlight_RegisterEvent(BACK_LIGHT_TALK, true);

            if (strCallId.empty())
            {
                talklogic_AttendedTransfer(strVal);
            }
            else
            {
                int CallId = atoi(strCallId.c_str());
                talklogic_AttendedTransfer(CallId, strVal);
            }
        }
        break;
    case OPERATION_B_TRANS:
        {
            // 盲转
            backlight_RegisterEvent(BACK_LIGHT_TALK, true);

            if (strCallId.empty())
            {
                talklogic_BlindTransfer(strVal);
            }
            else
            {
                int CallId = atoi(strCallId.c_str());
                talklogic_BlindTransfer(CallId, strVal.c_str());
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

bool CActionUri::ProcessSetForward(yl::string & strCode)
{
    ACTION_INFO("CActionUri::ProcessSetForward[%s]", strCode.c_str());

    //http://192.168.1.99/Bug.php?BugID=24680
    //http://192.168.1.99/Bug.php?BugID=17616
    //将上述用于设置非同步的FWD语法转换为已经实现的另一套语法
    //该套语法是不支持BSFT同步的服务器使用
    FormatSetFwdCfg(strCode);

    ACTION_INFO("SetFwdCfgCode After Formatted [%s]", strCode.c_str());

    size_t uPosFound = strCode.find("forward=set&");
    if (uPosFound == strCode.npos)
    {
        return false;
    }

    strCode = strCode.substr(uPosFound + 12);

    commonUnits_SetForwardCfg(strCode.c_str());

    return true;
}


//处理Action Url，设置其ip地址，如key=#IP：10.2.5.203
bool CActionUri::ProcessRemoteReq(const yl::string & strCode)
{
    // 查找冒号,如果没有的话,则不处理
    size_t uPosFound = strCode.find(":");
    if (uPosFound == strCode.npos)
    {
        return false;
    }

    yl::string strCmd = strCode.substr(0, uPosFound);
    yl::string strVal = strCode.substr(uPosFound + 1);

    // 查找冒号前的关键字
    const CommandData * pCmdData = MyFindCmd(strCmd);
    if (NULL == pCmdData)
    {
        return false;
    }

    //去除左右空格
    strVal = commonAPI_TrimString(strVal, " ");

    bool bHandled = true;
    switch (pCmdData->m_nVal)
    {
#ifdef IF_SUPPORT_UME
    case OPERATION_SET_REMOTE_IP:
        {
            //容错，补头
            if (strVal.find("://") == yl::string::npos)
            {
                strVal = "http://" + strVal;
            }
            //容错，补尾
            if (strVal.substr(strVal.length() - 1) != "/")
            {
                strVal.append("/");
            }
            backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            configParser_SetConfigString(kszAURemoteIP, strVal.c_str());
            g_pActionUrl->SendIpOk();
        }
        break;
#endif

    case OPERATION_ANSWER_CALL:
        {
            int CallId = atoi(strVal.c_str());
            bHandled = talklogic_AnswerCall(CallId);
        }
        break;

    case PHONE_KEY_CANCEL:
        {
            int CallId = atoi(strVal.c_str());
            bHandled = talklogic_ExitSession(CallId);
        }
        break;
    case PHONE_KEY_HOLD:
        {
            int CallId = atoi(strVal.c_str());
            bHandled = talklogic_HoldSession(CallId);
        }
        break;

    case OPERATION_UNHOLD_CALL:
        {
            int CallId = atoi(strVal.c_str());
            bHandled = talklogic_UnHoldSession(CallId);
        }
        break;

    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

bool CActionUri::ProcessRegisterAcc(yl::string & strCode)
{
    ACTION_INFO("CActionUri::ProcessRegisterAcc[%s]", strCode.c_str());

    yl::string strPrefix = "Register&";
    size_t uPosFound = strCode.find(strPrefix);
    if (uPosFound == strCode.npos)
    {
        return false;
    }

    strCode = strCode.substr(uPosFound + strPrefix.length());
    return acc_SetAccountInfo(strCode);
}

bool CActionUri::ProcessLongPress(yl::string & strCode)
{
    size_t uPosFound = strCode.find("_LONGPRESS");
    if (uPosFound == strCode.npos)
    {
        return false;
    }

    yl::string strKey = strCode.substr(0, uPosFound);
    int iDsskeyIndex = -1;
    int iExpIndex = -1;
    int iDsskeyID = -1;
    if (sscanf(strKey.c_str(), "L%d", &iDsskeyIndex) == 1)
    {
        iDsskeyID = iDsskeyIndex - 1;
    }
    else if (sscanf(strKey.c_str(), "EXP-%d-%d", &iExpIndex, &iDsskeyIndex) == 2)
    {
        DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(iExpIndex - 1);
        if (iExpIndex < 1 || iExpIndex > MAX_EXP_NUM
                || iDsskeyIndex < 1 || iDsskeyIndex > exp_GetExpKeyNumByType(eType))
        {
            return true;
        }
        DSSKEY_MODULE_TYPE moduleType = exp_GetEXPModuleType();
        iDsskeyID = dsskey_GetDsskeyID(moduleType, iExpIndex - 1, iDsskeyIndex - 1);
    }

    ACTION_INFO("Action ProcessLongPress[%s] iDsskeyID[%d]", strKey.c_str(), iDsskeyID);
    if (iDsskeyID > -1)
    {
        ACTION_INFO("AdapterBox_DsskeyLongPress[%d]", iDsskeyID);
        return AdapterBox_DsskeyLongPress(iDsskeyID);
    }

    return false;
}

CActionUri::CActionUri()
    : m_bIsCmdTimeOn(false)
{
#if IF_BUG_27560
    m_bHideInfo = false;
#endif
}

CActionUri::~CActionUri()
{

}

bool CActionUri::MyInit()
{
    struct stTemp
    {
        yl::string m_strCode;  // 操作代码.
        CodeType m_eType;  // 操作类型.
        int m_nVal;  // 对应的话机按键或者消息id.
    };

    static const stTemp arrCfgList[] =
    {
        // 功能键.
        {"OK", FUNCTION_KEY, PHONE_KEY_OK},
        {"ENTER", FUNCTION_KEY, PHONE_KEY_OK},
        {"ANSWER", OPERATION, OPERATION_ANSWER_CALL},
        {"ASW", OPERATION, OPERATION_ANSWER_CALL},
        {"Asw", OPERATION, OPERATION_ANSWER_CALL},

        {"SPEAKER", FUNCTION_KEY, PHONE_KEY_HANDFREE},
        {"F_TRANSFER", FUNCTION_KEY, PHONE_KEY_TRANSFER},
        {"MUTE", FUNCTION_KEY, PHONE_KEY_MUTE},
        {"HOLD", FUNCTION_KEY, PHONE_KEY_HOLD},
        {"F_HOLD", FUNCTION_KEY, PHONE_KEY_HOLD},
        {"CANCEL", FUNCTION_KEY, PHONE_KEY_CANCEL},
        {"Cancel", FUNCTION_KEY, PHONE_KEY_CANCEL},
        {"x", FUNCTION_KEY, PHONE_KEY_CANCEL},
        {"X", FUNCTION_KEY, PHONE_KEY_CANCEL},
        {"F_CONFERENCE", FUNCTION_KEY, PHONE_KEY_CONFERENCE},
        {"F1", FUNCTION_KEY, PHONE_KEY_FN1},
        {"F2", FUNCTION_KEY, PHONE_KEY_FN2},
        {"F3", FUNCTION_KEY, PHONE_KEY_FN3},
        {"F4", FUNCTION_KEY, PHONE_KEY_FN4},
        {"MSG", FUNCTION_KEY, PHONE_KEY_MESSAGE},
        {"UP", FUNCTION_KEY, PHONE_KEY_UP},
        {"LEFT", FUNCTION_KEY, PHONE_KEY_LEFT},
        {"DOWN", FUNCTION_KEY, PHONE_KEY_DOWN},
        {"RIGHT", FUNCTION_KEY, PHONE_KEY_RIGHT},
        {"VOLUME_UP", FUNCTION_KEY, PHONE_KEY_VOLUME_INCREASE},
        {"VOLUME_DOWN", FUNCTION_KEY, PHONE_KEY_VOLUME_DECREASE},
        {"HEADSET", FUNCTION_KEY, PHONE_KEY_HEADSET_CTRL},
        {"OFFHOOK", FUNCTION_KEY, PHONE_KEY_HANDSET_OFF_HOOK},
        {"ONHOOK", FUNCTION_KEY, PHONE_KEY_HANDSET_ON_HOOK},
#if IF_BUG_32044
        {"BackInFullDsskey", FUNCTION_KEY, PHONE_KEY_FULLDSSKEY_BACK}, //full dsskey界面虚拟back键
#endif

        // 数字键.
        {"~", DIGITAL_KEY, PHONE_KEY_OK},
        {"1", DIGITAL_KEY, PHONE_KEY_1},
        {"2", DIGITAL_KEY, PHONE_KEY_2},
        {"3", DIGITAL_KEY, PHONE_KEY_3},
        {"4", DIGITAL_KEY, PHONE_KEY_4},
        {"5", DIGITAL_KEY, PHONE_KEY_5},
        {"6", DIGITAL_KEY, PHONE_KEY_6},
        {"7", DIGITAL_KEY, PHONE_KEY_7},
        {"8", DIGITAL_KEY, PHONE_KEY_8},
        {"9", DIGITAL_KEY, PHONE_KEY_9},
        {"0", DIGITAL_KEY, PHONE_KEY_0},
        {"*", DIGITAL_KEY, PHONE_KEY_STAR},
        {"#", DIGITAL_KEY, PHONE_KEY_POUND},
        {"POUND", DIGITAL_KEY, PHONE_KEY_POUND},

        // 操作键.
        {"Reboot", OPERATION, OPERATION_REBOOT},
        {"Reset", OPERATION, OPERATION_RESET},
        {"AutoP", OPERATION, OPERATION_AUTOP},
        {"DNDOn", OPERATION, OPERATION_DND_ON},
        {"DNDOff", OPERATION, OPERATION_DND_OFF},
        {"CallWaitingOn", OPERATION, OPERATION_CALLWAIT_ON},
        {"CallWaitingOff", OPERATION, OPERATION_CALLWAIT_OFF},
        {"ATrans", OPERATION, OPEARTION_A_TRANS},
        {"BTrans", OPERATION, OPERATION_B_TRANS},
        {"CALLEND", OPERATION, OPERATION_CALL_END},
        {"CallEnd", OPERATION, OPERATION_CALL_END},

        {"RD", OPERATION, OPERATION_REDIAL},
        {"MENU", OPERATION, OPERATION_ENTER_MENU},
        {"CONTACT", OPERATION, OPERATION_ENTER_CONTACT},
        {"REC", OPERATION, OPERATION_RECORD},
        {"ViewForwardedCall", OPERATION, OPERATION_VIEW_FORWARD},
        {"ViewMissedCall", OPERATION, OPERATION_VIEW_MISSCALL},

#ifdef IF_SUPPORT_UME
        {"IP", OPERATION, OPERATION_SET_REMOTE_IP},
        {"DeleteReportIP", OPERATION, OPERATION_DEL_REMOTE_IP},
#endif
        {"UNHOLD", OPERATION, OPERATION_UNHOLD_CALL},
        {"F_UNHOLD", OPERATION, OPERATION_UNHOLD_CALL},

        {"L1", OPERATION, OPERATION_LINEKEY_0},
        {"L2", OPERATION, OPERATION_LINEKEY_1},
        {"L3", OPERATION, OPERATION_LINEKEY_2},
        {"L4", OPERATION, OPERATION_LINEKEY_3},
        {"L5", OPERATION, OPERATION_LINEKEY_4},
        {"L6", OPERATION, OPERATION_LINEKEY_5},
        {"L7", OPERATION, OPERATION_LINEKEY_6},
        {"L8", OPERATION, OPERATION_LINEKEY_7},
        {"L9", OPERATION, OPERATION_LINEKEY_8},
        {"L10", OPERATION, OPERATION_LINEKEY_9},
        {"L11", OPERATION, OPERATION_LINEKEY_10},
        {"L12", OPERATION, OPERATION_LINEKEY_11},
        {"L13", OPERATION, OPERATION_LINEKEY_12},
        {"L14", OPERATION, OPERATION_LINEKEY_13},
        {"L15", OPERATION, OPERATION_LINEKEY_14},
        {"L16", OPERATION, OPERATION_LINEKEY_15},
        {"L17", OPERATION, OPERATION_LINEKEY_16},
        {"L18", OPERATION, OPERATION_LINEKEY_17},
        {"L19", OPERATION, OPERATION_LINEKEY_18},
        {"L20", OPERATION, OPERATION_LINEKEY_19},
        {"L21", OPERATION, OPERATION_LINEKEY_20},
        {"L22", OPERATION, OPERATION_LINEKEY_21},
        {"L23", OPERATION, OPERATION_LINEKEY_22},
        {"L24", OPERATION, OPERATION_LINEKEY_23},
        {"L25", OPERATION, OPERATION_LINEKEY_24},
        {"L26", OPERATION, OPERATION_LINEKEY_25},
        {"L27", OPERATION, OPERATION_LINEKEY_26},
        {"L28", OPERATION, OPERATION_LINEKEY_27},
        {"L29", OPERATION, OPERATION_LINEKEY_28},
        {"BACK_IDLE", OPERATION, OPERATION_BACK_IDLE},

    };

    for (int i = 0; i < sizeof(arrCfgList) / sizeof(arrCfgList[0]); ++i)
    {
        if (!IsURISupprot(arrCfgList[i].m_eType, arrCfgList[i].m_nVal))
        {
            continue ;
        }

        CommandData * pNewCmd = new CommandData();
        if (pNewCmd != NULL)
        {
            pNewCmd->m_strCode = arrCfgList[i].m_strCode;
            pNewCmd->m_eType = arrCfgList[i].m_eType;
            pNewCmd->m_nVal = arrCfgList[i].m_nVal;

            m_mapCmd[arrCfgList[i].m_strCode] = pNewCmd;
        }
    }  // for.

    // 注册消息.
    etl_RegisterMsgHandle(DSK_MSG_ACTION_URI,
                          DSK_MSG_ACTION_URI,
                          OnMessage);

    etl_RegisterMsgHandle(SIP_CMD_PASS_ACTION_URI,
                          SIP_CMD_PASS_ACTION_URI,
                          OnMessage);

    etl_RegisterMsgHandle(DSK_MSG_SETPHONE_CFG,
                          DSK_MSG_SETPHONE_CFG,
                          OnMessage);

    // 注册消息.
    etl_RegisterMsgHandle(
        TM_TIMER,
        TM_TIMER,
        OnMessage);

    etl_RegisterMsgHandle(DSK_MSG_ACTION_URI_CONFIRM,
                          DSK_MSG_ACTION_URI_CONFIRM,
                          OnRequestCTR);

    return true;
}

bool CActionUri::IsURISupprot(CodeType eType, int iKeyCode)
{
    // CP不支持远程RECORD
    if (OPERATION == eType
            && OPERATION_RECORD == iKeyCode
            && (DEVICELIB_IS_CP(devicelib_GetPhoneType())))
    {
        return false;
    }

    return true;
}

bool CActionUri::MyFinal()
{
    // 清空配置map表.
    CmdMap::iterator iter = m_mapCmd.begin();

    for (; iter != m_mapCmd.end(); ++iter)
    {
        if (iter->second != NULL)
        {
            delete iter->second;
            iter->second = NULL;
        }
    }  // iter;
    m_mapCmd.clear();

    // 取消消息注册.
    etl_UnregisterMsgHandle(DSK_MSG_ACTION_URI,
                            DSK_MSG_ACTION_URI,
                            OnMessage);

    etl_UnregisterMsgHandle(SIP_CMD_PASS_ACTION_URI,
                            SIP_CMD_PASS_ACTION_URI,
                            OnMessage);

    etl_UnregisterMsgHandle(DSK_MSG_SETPHONE_CFG,
                            DSK_MSG_SETPHONE_CFG,
                            OnMessage);

    // 取消消息注册.
    etl_UnregisterMsgHandle(
        TM_TIMER,
        TM_TIMER,
        OnMessage);

    etl_UnregisterMsgHandle(DSK_MSG_ACTION_URI_CONFIRM,
                            DSK_MSG_ACTION_URI_CONFIRM,
                            OnRequestCTR);

    return true;
}

CActionUri::CommandData * CActionUri::MyFindCmd(const yl::string & strCode)
{

    // 查找配置.
    CmdMap::iterator iter = m_mapCmd.find(strCode);
    return iter != m_mapCmd.end() ? iter->second : NULL;
}

bool CActionUri::MyParseCode(msgObject & refObj)
{
    yl::string strUriCpy;

    char szBuffer[knMaxBufferLen + 1] = {0};
    memcpy(szBuffer, refObj.GetExtraData(), refObj.GetExtraSize());
    szBuffer[knMaxBufferLen] = '\0';
    if (szBuffer[0] != '\0')
    {
        // 拷贝Uri.
        strUriCpy = szBuffer;

        // 兼容sip-notify方式
        strUriCpy = commonAPI_TrimString(strUriCpy, "\r \n\t");

        ACTION_INFO("Action URI: IP(%s)", strUriCpy.c_str());

        yl::string strRemoteIP;

        size_t uEndFound = strUriCpy.find(kszRemoteIP);

        if (uEndFound != yl::string::npos)
        {
            strRemoteIP = strUriCpy.substr(uEndFound + strlen(kszRemoteIP));

            ACTION_INFO("Action URI: IP(%s)", strRemoteIP.c_str());

            strUriCpy = strUriCpy.substr(0, uEndFound);
        }
        // 提取code.
        size_t uPosFound = strUriCpy.find(kszCmdIdentifier);

        if (uPosFound != yl::string::npos)
        {
            strUriCpy = strUriCpy.substr(uPosFound + strlen(kszCmdIdentifier));

            // 去掉code左右的空格、回车（\n）、换行（\r）
            strUriCpy = commonAPI_TrimString(strUriCpy, "\r \n\t");
        }
    }

    if (strUriCpy.empty())
    {
        return false;
    }

    ACTION_INFO("Action(%s)", strUriCpy.c_str());

    return commonAPI_splitStringBySeparator(strUriCpy, ';', m_ListCmd);
}

bool CActionUri::MyDoCmd(const CommandData & stCD)
{
    bool isSucceed = false;

    ACTION_INFO("Do command %d %d", stCD.m_eType, stCD.m_nVal);

#if IF_BUG_32044
    switch (stCD.m_nVal)
    {
    case PHONE_KEY_CONFERENCE:
    case PHONE_KEY_CANCEL:
    case PHONE_KEY_MUTE:
    case PHONE_KEY_TRANSFER:
    case PHONE_KEY_HOLD:
    case PHONE_KEY_FN1:
    case PHONE_KEY_FN2:
    case PHONE_KEY_FN3:
    case PHONE_KEY_FN4:
        //发送full dsskey界面模拟按back键消息
        SendKeyMsg(PHONE_KEY_FULLDSSKEY_BACK);
        break;
    default:
        break;
    }
#endif

    // 根据类型进行不同的处理.
    switch (stCD.m_eType)
    {
    case FUNCTION_KEY:
    case DIGITAL_KEY:
        if (stCD.m_nVal == PHONE_KEY_MESSAGE)
        {
            SendKeyMsg(stCD.m_nVal);
            return true;
        }
        // 发送按键消息.
        backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);

        SendKeyMsg(stCD.m_nVal);
        break;

    case OPERATION:
        isSucceed = MyDoOperation(stCD);
        break;

    default:
        break;
    }

    return isSucceed;
}

bool CActionUri::MyDoOperation(const CommandData & stCD)
{
    bool isSucceed = false;

    // 处理操作.
    if (stCD.m_eType == OPERATION)
    {
        // 有ActionUri操作，退出屏保
        AdapterBox_ExitScreenSaver();
        switch (stCD.m_nVal)
        {
        case OPERATION_REBOOT:
            //外部模块已有通话判断处理操作
            //启动重起计时器
//          if (!talklogic_SessionExist(false))
//          {
            backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);

            AdapterBox_Reboot(0);
//          }
//          else
//          {
//              // 启动重起的计时器.
//              timerSetThreadTimer((UINT)&m_mapCmd, 3000);
//          }

            break;

        case OPERATION_AUTOP:
            {
                // 执行自动更新.
                backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);

                // 配置开启, 退出通话, 允许更新
                if (configParser_GetConfigInt(kszAutopInTalking) != 0)
                {
                    ACTION_INFO("Force exit talking, autoping!");
                    talklogic_ExitAllSession();
                }

                Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0, 0);
            }
            break;

        case OPERATION_DND_ON:
            // 开启DND.
            isSucceed = dnd_SetEnable(acc_GetDefaultAccount(), true);
            if (isSucceed)
            {
                backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            }
            break;

        case OPERATION_DND_OFF:
            // 关闭DND.
            isSucceed = dnd_SetEnable(acc_GetDefaultAccount(), false);
            if (isSucceed)
            {
                backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            }
            break;
        case OPERATION_REDIAL:
            // 重播上一个拨出的号码.
            MyRedial();
            break;
        case OPERATION_CALL_END:
            {
                talklogic_ExitFocusSession();
            }
            break;
        case OPERATION_RESET:
            {
                //本线程自己处理
                etl_NotifyApp(false, SYS_MSG_RESET_FACTORY, 0, 0);
            }
            break;
#if IF_BUG_36591
        case OPERATION_VIEW_FORWARD:
            {
                LocalLog_ClearLogs(kszForwardNotifId);
            }
            break;
        case OPERATION_VIEW_MISSCALL:
            {
                LocalLog_ClearLogs(kszMissNotifId);
            }
            break;
#endif
        case OPERATION_ENTER_MENU:
            {
                AdapterBox_SettingUIEnterPage(MENU_PAGE_MAIN);
            }
            break;
        case OPERATION_ENTER_CONTACT:
            {
                AdapterBox_DirUIEnterLocalMenuPage();
            }
            break;
        case OPERATION_RECORD:
            {
                return talklogic_CallRecord();
            }
            break;
#ifdef IF_SUPPORT_UME
        case OPERATION_DEL_REMOTE_IP:
            {
                isSucceed = configParser_SetConfigString(kszAURemoteIP, "");
            }
            break;
#endif
        case OPERATION_ANSWER_CALL:
            {
                return talklogic_AcceptIncomingCall();
            }
            break;
        case OPERATION_CALLWAIT_ON:
            {
                isSucceed = configParser_SetConfigInt(kszCallWaitingSwitch, 1);
                if (isSucceed)
                {
                    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_CALLWAIT, 1);
                }
            }
            break;
        case OPERATION_CALLWAIT_OFF:
            {
                isSucceed = configParser_SetConfigInt(kszCallWaitingSwitch, 0);
                if (isSucceed)
                {
                    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_CALLWAIT, 0);
                }
            }
            break;
        case OPERATION_BACK_IDLE:
            {
                talklogic_ExitAllSession();
                AdapterBox_ReturnToIdle(true);
            }
            break;
        default:
            {
                backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
                if (stCD.m_nVal >= OPERATION_LINEKEY_0
                        && stCD.m_nVal <= OPERATION_LINEKEY_28)
                {
                    dsskey_OnDsskeyClick(stCD.m_nVal - OPERATION_LINEKEY_0);
                }
            }
            break;
        }
    }

    return isSucceed;
}

bool CActionUri::MyDoOperationNumber(const yl::string & strCode)
{
    if (strCode.empty())
    {
        return false;
    }

    // 从字符串中提取要拨出的号码.
    yl::string strNumber;
    size_t uPosNumberBegin = strCode.find("number=");
    size_t uPosNumberEnd = strCode.find("&");
    if (uPosNumberBegin != yl::string::npos)
    {
        if (uPosNumberEnd == yl::string::npos)
        {
            uPosNumberEnd = strCode.length() + 1;
        }
    }
    strNumber = strCode.substr(uPosNumberBegin + 7,
                               uPosNumberEnd - uPosNumberBegin - 7);

    // 从字符串中提取拨出时使用的账号.
    yl::string strAccount;
    yl::string strServer;
    size_t uPosAccountBegin = strCode.find("outgoing_uri=");
    size_t uPosAccountEnd = strCode.length() + 1;
    if (uPosAccountBegin != yl::string::npos)
    {
        strAccount = strCode.substr(uPosAccountBegin + 13,
                                    uPosAccountEnd - uPosAccountBegin - 13);
    }

    uPosAccountEnd = strAccount.find("@");

    if (uPosAccountEnd != strAccount.npos)
    {
        size_t uPosServerEnd = strAccount.find(":");
        if (uPosServerEnd != strAccount.npos)
        {
            strServer = strAccount.substr(uPosAccountEnd + 1, uPosServerEnd - uPosAccountEnd - 1);
        }
        else
        {
            strServer = strAccount.substr(uPosAccountEnd + 1);
        }
        strAccount = strAccount.substr(0, uPosAccountEnd);
    }

    // 根据账号名和服务器获取账号id.
    int iAccount = strServer.empty() ? acc_GetAccountIDByName(strAccount.c_str()) : acc_GetAccountIDByName(strAccount.c_str(), strServer.c_str());

    // 根据号码获取联系人名.
    yl::string strName;
    Dir_GetCotactNameByNumber(strNumber, strName, LOCAL_DIR);
    backlight_RegisterEvent(BACK_LIGHT_TALK, true);

    if (acc_IsAccountAvailable(iAccount))
    {
        talklogic_CallOut(strNumber.c_str(), strName.c_str(), iAccount);
    }
    else
    {
        talklogic_CallOut(strNumber.c_str(), strName.c_str());
    }

    return true;
}

bool CActionUri::MyRedial()
{
    if (talklogic_Redial())
    {
        backlight_RegisterEvent(BACK_LIGHT_TALK, true);
        return true;
    }

    return false;
}

#if IF_BUG_23996
// 判断是否为Actionuri key
BOOL CActionUri::IsActionUriKey()
{
    return m_bActionUriKey;
}
// 设置Actionuri key
VOID CActionUri::SetActionUriKey(BOOL bIsActionUriKey)
{
    m_bActionUriKey = bIsActionUriKey;
}
#endif

// 格式化成功，返回true，失败false
bool CActionUri::FormatSetFwdCfg(yl::string & strCode)
{
    // 转换的语法格式见 http://192.168.1.99/Bug.php?BugID=24680
    // 需进行转换的语法一定包含"Fwd"
    if (strCode.find("Fwd") == yl::string::npos)
    {
        return false;
    }

    yl::string strFwdTypeEnable = "";
    yl::string strFwdTarget = "";
    yl::string strRingTimes = "";
    yl::string strAccount = "";

    char sTmpId[8] = { 0 };
    char strTmpCmd[128] = { 0 };
    sscanf(strCode.c_str(), "%[^&]&line=%s", strTmpCmd, sTmpId);

    // 将key分割保存到list中
    YLList<yl::string> listKey;
    if (commonAPI_splitStringBySeparator(strTmpCmd, '=', listKey)
            && listKey.size() > 0)
    {
        //第一个key为必有，且指示FWD类型与开关操作
        YLList<yl::string>::iterator iter = listKey.begin();
        yl::string strFwdAction = *iter;
        int nFwdType = TransFwdCfgTypeAction(strFwdAction, strFwdTypeEnable);
        if (nFwdType == FWD_TYPE_MANUAL)
        {
            // 第一个key错误，未读取到可用的key
            return false;
        }

        if ((++iter) != listKey.end())
        {
            //第二个key指示FWD target
            yl::string strTarget = *iter;
            switch (nFwdType)
            {
            case FWD_TYPE_ALWAYS:
                {
                    strFwdTarget = "&Forward_always_target=" + strTarget;
                }
                break;
            case FWD_TYPE_BUSY:
                {
                    strFwdTarget = "&Forward_busy_target=" + strTarget;
                }
                break;
            case FWD_TYPE_NO_ANSWER:
                {
                    strFwdTarget = "&Forward_noanswer_target=" + strTarget;
                    // no answer fwd可能具备第三个key，ringtimes
                    if ((iter++) != listKey.end())
                    {
                        strRingTimes = "&Forward_noanswer_ringtimes=" + *iter;
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    /* 获取账号ID信息
    line参数为空时，则取值当前默认账号ID（此处不需要减1）
    line参数非空时，line减1（该特殊语法认为line是从1开始）*/
    int nLineId = acc_GetDefaultAccount();
    if (strcmp(sTmpId, "") != 0)
    {
        nLineId = atoi(sTmpId) - 1;
    }
    sprintf(sTmpId, "%d", nLineId);
    strAccount = "&account=" + yl::string(sTmpId);

    strCode = "forward=set" + strFwdTypeEnable + strFwdTarget + strRingTimes + strAccount;
    return true;
}

int CActionUri::TransFwdCfgTypeAction(const yl::string & strFwdAction,
                                      yl::string & strFwdTypeEnable)
{
    if (strFwdAction == "AlwaysFwdOn")
    {
        strFwdTypeEnable = "&Forward_always_enable=1";
        return FWD_TYPE_ALWAYS;
    }
    else if (strFwdAction == "AlwaysFwdOff")
    {
        strFwdTypeEnable = "&Forward_always_enable=0";
        return FWD_TYPE_ALWAYS;
    }
    else if (strFwdAction == "BusyFwdOn")
    {
        strFwdTypeEnable = "&Forward_busy_enable=1";
        return FWD_TYPE_BUSY;
    }
    else if (strFwdAction == "BusyFwdOff")
    {
        strFwdTypeEnable = "&Forward_busy_enable=0";
        return FWD_TYPE_BUSY;
    }
    else if (strFwdAction == "NoAnswFwdOn")
    {
        strFwdTypeEnable = "&Forward_noanswer_enable=1";
        return FWD_TYPE_NO_ANSWER;
    }
    else if (strFwdAction == "NoAnswFwdOff")
    {
        strFwdTypeEnable = "&Forward_noanswer_enable=0";
        return FWD_TYPE_NO_ANSWER;
    }
    return FWD_TYPE_MANUAL;
}

#if IF_BUG_27560
// 处理设置隐藏通话信息的特殊接口
bool CActionUri::ProcessDisplay(const yl::string & strCode)
{
    if (strCode.find(kszCmdDisplay) == 0)
    {
        // 设置隐藏号码标志
        yl::string strValue = strCode.substr(sizeof(kszCmdDisplay) - 1, 1);

        ACTION_INFO("Display value=[%s], code=[%s]", strValue.c_str(), strCode.c_str());
        if (strValue == "0")
        {
            m_bHideInfo = true;
        }

        return true;
    }

    return false;
}
#endif


}  // namespace NS_AU
