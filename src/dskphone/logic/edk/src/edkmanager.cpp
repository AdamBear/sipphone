///////////////////////////////////////////////////////////
//  CEdkManager.cpp
//  Implementation of the Class CEdkManager
//  Created on:      21-一月-2015 15:32:26
//  Original author: dlw
///////////////////////////////////////////////////////////

#include "edkmanager.h"
#include "edkparse.h"
#include "edkactionmanager.h"
#include "etlmsghandler/modetlmsghandler.h"

#if IF_FEATURE_EDK

// EDK执行异常检测时间,单位:ms
#define COMMAND_CHECK_TIME      (1000 * 3)
// 一段时间内EDK执行的最大条数
#define MAX_ACTION_COUNT        (20 * (COMMAND_CHECK_TIME / 1000))
// 消息转换为string
#define CASE_MSG_TO_STRING(msg, str)    case msg: str = #msg; break;

IMPLEMENT_GETINSTANCE(CEdkManager)
CEdkManager::CEdkManager()
    : m_iActionSize(0),
      m_bCheckTimerStatus(false)
{
    m_bTipModeEnable = false;
}

CEdkManager::~CEdkManager()
{
}

/**
 * 初始化
 */
bool CEdkManager::Init()
{
    EDK_INFO("[Init][EDK switch:%s]", _EDKDataMgr.IsEnable() ? "On" : "off");

    if (!_EDKDataMgr.IsEnable())
    {
        return false;
    }

    // 数据管理初始化
    _EDKDataMgr.Init();
    // 消息注册
    etl_RegisterMsgHandle(DSK_MSG_DO_EDK, DSK_MSG_DO_EDK, &CEdkManager::OnMessage);

    return true;
}

/**
 * 执行 EDK 命令
 */
bool CEdkManager::DoAction(const yl::string & strAction, int iDsskeyId/* = -1*/,
                           EDKTRIGGERTYPE eTriggerType/* = EDK_TRIGGER*/)
{
    // 判断开关是否开启
    if (eTriggerType == EDK_TRIGGER && !_EDKDataMgr.IsEnable())
    {
        return false;
    }

    // 该处理是为了解决命令调用死循环问题
    // http://10.2.1.199/Bug.php?BugID=85707
    if (eTriggerType != BLF_TRIGGER)
    {
        IncreaseActionSize();
    }

    if (!IsCheckTimerRuning())
    {
        SetCheckTimer(true);
    }
    else if (GetActionSize() > MAX_ACTION_COUNT)
    {
        return false;
    }

    // v81支持多命令流
    // 开始解析
    return _EDKParser.ParseAction(strAction, iDsskeyId);
}

/**
* 执行命令流
*/
yl::string CEdkManager::GetLabelFromAction(const yl::string & strAction, EDKACTIONTYPE eType)
{
    // 判断开关是否开启
    if (!_EDKDataMgr.IsEnable())
    {
        return "";
    }

    return _EDKParser.ParseActionForType(strAction, eType);
}

bool CEdkManager::OnEdkSoftKeyProess(EDKSOFTKEYTYPE eType)
{
    edk_softkey_data_t edkSoftkeyData;
    _EDKDataMgr.GetSoftkeyData(eType, edkSoftkeyData);

    if (edkSoftkeyData.m_strAction.empty())
    {
        return false;
    }

    return DoAction(edkSoftkeyData.m_strAction);
}

void CEdkManager::GetEdkSoftKeyList(YLList<SoftKey_Data> & listSoftKey)
{
    return m_edkSoftKeyMgr.GetEdkSoftKeyList(listSoftKey);
}

bool CEdkManager::GetSoftkeyLabelByType(EDKSOFTKEYTYPE eType, yl::string & strLabel)
{
    edk_softkey_data_t edkSoftkeyData;
    if (!_EDKDataMgr.GetSoftkeyData(eType, edkSoftkeyData)
            || !edkSoftkeyData.m_bEnable)
    {
        return false;
    }

    if (edkSoftkeyData.m_strLabel.empty())
    {
        strLabel = GetLabelFromAction(edkSoftkeyData.m_strAction, EDK_LABEL);
        EDK_INFO("+++++++++++++++++++softkey[%d], label[%s]", eType, strLabel.c_str());
    }
    else
    {
        strLabel = edkSoftkeyData.m_strLabel;
    }

    return true;
}

/**
* 从action里面获取eType类型的数据
*/
yl::string CEdkManager::GetDsskeyLabelFromAction(int iDsskeyID, EDKACTIONTYPE eType)
{
    EDK_INFO("GetDsskeyLabelFromAction iDsskeyID:%d, enable:%d", iDsskeyID, _EDKDataMgr.IsEnable());
    if (iDsskeyID == -1 || !_EDKDataMgr.IsEnable())
    {
        return "";
    }

    // label优先级:用户配置>命令$LB<label>$>系统默认label
    DssKeyLogicData DsskeyData;
    if (dsskey_GetData(iDsskeyID, DsskeyData)
            && !DsskeyData.strLabel.empty())
    {
        return DsskeyData.strLabel;
    }

    yl::string strLabel;
    if (!DsskeyData.strValue.empty())
    {
        strLabel = GetLabelFromAction(DsskeyData.strValue, eType);
        EDK_INFO("GetDsskeyLabelFromAction action[%s], dsskey id[%d], strLabel[%s]",
                 DsskeyData.strValue.c_str(), iDsskeyID, strLabel.c_str());
    }

    // http://10.2.1.199/Bug.php?BugID=111461
    /*if (strLabel.empty())
    {
        strLabel = dsskey_GetDefaultLabel(DT_CUSTOM_KEY);
    }*/

    return strLabel;
}

/**
* 设置提示模式开关
*/
bool CEdkManager::SetTipMode(bool bEnable)
{
    bool bSwitchEnable = _EDKDataMgr.IsEnable() && configParser_GetConfigInt(kszEdkTipMode);

    m_bTipModeEnable = bSwitchEnable && bEnable;

    // 返回设置结果,开关为开,默认设置成功
    return bSwitchEnable;
}

bool CEdkManager::SetCheckTimer(bool bEnable)
{
    m_bCheckTimerStatus = bEnable;
    if (bEnable)
    {
        timerSetThreadTimer((UINT)&m_iActionSize, COMMAND_CHECK_TIME);
    }
    else
    {
        timerKillThreadTimer((UINT)&m_iActionSize);
    }

    return true;
}


bool CEdkManager::OnTimer(UINT uTimer)
{
    // 统计一段时间内EDK执行的命令数量, 通过该方式暂时解决命令循环调用导致死循环问题
    static UINT uCount = 0;
    if (uTimer == (UINT)&m_iActionSize)
    {
        if (GetActionSize() > MAX_ACTION_COUNT)
        {
            EDK_WARN("There may be something wrong, example: action recall.");
            uCount = 0;
            SetCheckTimer(false);
            _EdkActionManager.CancelAction(-1, true);
        }
        else
        {
            // EDK执行结束1min后关闭该定时器
            uCount ++;
            if (uCount >= 60 / (COMMAND_CHECK_TIME / 1000))
            {
                uCount = 0;
                SetCheckTimer(false);
            }
        }
        ResetActionSize();

        return true;
    }

    return false;
}

// 消息值打印
void PrintMsgName(msgObject& objMsg)
{
    yl::string strMsg("");

    switch (objMsg.message)
    {
        CASE_MSG_TO_STRING(DSK_MSG_DO_EDK, strMsg);
    default:
        break;
    }

    if (!strMsg.empty())
    {
        WIRE_INFO("[Recv msg][msg:%s][wParam:%d][lParam:%d]",
                  strMsg.c_str(), objMsg.wParam, objMsg.lParam);
    }
}

LRESULT CEdkManager::OnMessage(msgObject& objMsg)
{
    PrintMsgName(objMsg);

    LPCSTR pszExtraData = (LPCSTR)objMsg.GetExtraData();
    if (textEMPTY(pszExtraData))
    {
        EDK_WARN("[Recv msg][Extra data empty]");
        return false;
    }

    yl::string str;
    bool bHandle = false;
    switch (objMsg.message)
    {
    case DSK_MSG_DO_EDK:
        bHandle = _EDKManager.DoAction(pszExtraData);
        break;
    default:
        break;
    }

    return bHandle;
}
#endif
