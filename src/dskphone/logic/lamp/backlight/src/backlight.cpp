#include "backlight.h"

#include "dsklog/log.h"

#include <ylalgorithm.h>
#include "configiddefine.h"
#include "path_define.h"

#include "devicelib/phonedevice.h"
#include "xmlparser/xmlparser.hpp"
#include "configparser/modconfigparser.h"
#include <interfacedefine.h>
#include "talk/talklogic/include/modtalklogic.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "innermsgdefine.h"
#include "powersaving/include/powersavingmsgdefine.h"
#include "powersaving/include/modpowersaving.h"
#include "commonunits/t2xpath.h"

#include "exp/include/modexp.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

//#include "dsskey/src/EXPHelper.h"

#define OPEN_BACK_LIGHT_DEBUG       0
#define DEFAULT_BACKLIGHT_TIME      15
#define REWRITECONFIG 1
#define BACKLIGHT_DISABLE_VALUE     1000

const yl::string strXmlFullPath(RESOURCE_PATH"device-res/backLight/");
const yl::string blackLightNodeRoot = "backlight";
const yl::string blackLightNodeItem = "event";
const yl::string AttrNameEventname = "eventname";

const yl::string AttrNamePriority = "priority";
const int         iErrorPriority = -1;

const yl::string AttrNameInitlight = "initlight";
const int         iErrorInitlight = -1;

const yl::string AttrNameTime = "time";
const int         iErrorTime = -2;



/************************************************************************/
/*                       about EventParse                               */
/************************************************************************/
BackLightEventParse::BackLightEventParse()
{
    LoadEventData();
}

BackLightEventParse::~BackLightEventParse()
{

}

BackLightEvent BackLightEventParse::GetEventDataByType(BACK_LIGHT_TYPE eType)
{
    IterBackLight iter = m_listEventData.begin();

    for (/* */; iter != m_listEventData.end(); ++iter)
    {
        if ((*iter).eType == eType)
        {
            break;
        }
    }

    if (iter == m_listEventData.end())
    {
        BACKLIGHT_WARN("BackLightEventParse::GetEventDataByType input type error");
        return BackLightEvent();
    }

    return *iter;
}

bool BackLightEventParse::LoadEventData()
{
    xml_document doc;
    yl::string strPath = res_getPathByName(RESOURCE_PATH_BACKLIGHT);

    if (!doc.load_file(strPath.c_str()))
    {
        BACKLIGHT_WARN("BackLightEventParse::LoadEventData load file error!");
        return false;
    }
    // Find root node.
    xml_node nodeRoot = doc.child(blackLightNodeRoot.c_str());
    if (!nodeRoot)
    {
        BACKLIGHT_WARN("BackLightEventParse::LoadEventData find root node error!");
        return false;
    }

    xml_node nodeChild = nodeRoot.child(blackLightNodeItem.c_str());
    if (!nodeChild)
    {
        return false;
    }

    // Iterate the child nodes to get data.
    while (nodeChild)
    {
        BackLightEvent objEvent;
        objEvent.eType = StringToType(nodeChild.attribute(AttrNameEventname.c_str()).value());
        objEvent.brightTimer = nodeChild.attribute(AttrNameTime.c_str()).as_int();
        objEvent.initLight = nodeChild.attribute(AttrNameInitlight.c_str()).as_int();
        objEvent.priority = nodeChild.attribute(AttrNamePriority.c_str()).as_int();
        if (CheckParseResult(objEvent))// 验证数据的正确性
        {
            m_listEventData.push_back(objEvent);// 添加数据
        }
        else
        {
            BACKLIGHT_WARN("BackLightEventParse::LoadEventData file data error");
        }
        nodeChild = nodeChild.next_sibling();
    }

    return true;
}

bool BackLightEventParse::CheckParseResult(const BackLightEvent & objEvent)
{
    return (objEvent.eType != BACK_LIGHT_NONE
            && objEvent.brightTimer != iErrorTime
            && objEvent.initLight != iErrorInitlight
            && objEvent.priority != iErrorPriority);
}

BACK_LIGHT_TYPE BackLightEventParse::StringToType(const yl::string & strTypeName)
{
    BACK_LIGHT_TYPE eType = BACK_LIGHT_NONE;

    if (strTypeName == "reset")
    {
        eType = BACK_LIGHT_DEFAULT;
    }
    else if (strTypeName == "network")
    {
        eType = BACK_LIGHT_NETWORK;
    }
    else if (strTypeName == "account")
    {
        eType = BACK_LIGHT_ACCOUNT;
    }
    else if (strTypeName == "talk")
    {
        eType = BACK_LIGHT_TALK;
    }
    else if (strTypeName == "lighten")
    {
        eType = BACK_LIGHT_LIGHTEN;
    }
    else if (strTypeName == "misscall")
    {
        eType = BACK_LIGHT_MISSCALL;
    }
    else if (strTypeName == "forwardcall")
    {
        eType = BACK_LIGHT_FORWARDCALL;
    }
    else if (strTypeName == "message")
    {
        eType = BACK_LIGHT_MESSAGE;
    }
    else if (strTypeName == "usbplay")
    {
        eType = BACK_LIGHT_USBPLAY;
    }
    else
    {
        BACKLIGHT_WARN("BackLightEventParse::StringToType string error!");
    }

    return eType;
}

/************************************************************************/
/*                       about BackLightManager                         */
/************************************************************************/
BackLightManager * BackLightManager::m_pOneSelf = NULL;

LRESULT OnBackLightConfigChange(msgObject & msg)
{
    if (msg.message == DSK_MSG_LIGHT_STATUS_CHANGE)
    {
        BACKLIGHT_INFO("DSK_MSG_LIGHT_STATUS_CHANGE wparam[%d] lparam[%d]", msg.wParam, msg.lParam);
        // wParam[0/1]=常亮[否/是]
        if (msg.wParam == 1)
        {
            if (msg.lParam != 0)
            {
                g_pBackLightManager->AddEvent(BACK_LIGHT_LIGHTEN);
            }
            else
            {
                g_pBackLightManager->EarseEvent(BACK_LIGHT_LIGHTEN);
            }
        }
        else
        {
            // 如果是预览背光灯等级，不处理按键亮默认等级灯，
            // 防止预览背光灯等级时，闪一下配置文件中的等级灯
            if (g_pBackLightManager->GetPreviewMode())
            {
                return TRUE;
            }

            g_pBackLightManager->ReLoadConfig();
            g_pBackLightManager->Reset();
        }

        return TRUE;
    }
    else if (msg.message == CONFIG_MSG_BCAST_CHANGED)
    {
#if OPEN_BACK_LIGHT_DEBUG
        BACKLIGHT_INFO("CONFIG_MSG_BCAST_CHANGED");
#endif // OPEN_BACK_LIGHT_DEBUG

        g_pBackLightManager->ReLoadConfig();
        g_pBackLightManager->Reset();
#if IF_FEATURE_PROVISIONING_MONTHLY
        if (msg.wParam == ST_INACTIVE_TIME)
        {
            g_pBackLightManager->ReLoadInactivityTimer();
        }
#endif
        return TRUE;
    }
    else if (BACKLIGHT_SET_ENABLE_MSG == msg.message)
    {
        BACKLIGHT_INFO("BACKLIGHT_SET_ENABLE_MSG(%d)", msg.wParam);
        // 临时禁用/启用背光灯功能；wParam = 0/1表示禁用/启用
        g_pBackLightManager->EnableBackLight(1 == msg.wParam);
    }
    else if (DSK_MSG_LIGHT_LEVEL_CHANGE == msg.message)
    {
        BACKLIGHT_INFO("DSK_MSG_LIGHT_LEVEL_CHANGE [%d]", DSK_MSG_LIGHT_LEVEL_CHANGE);
        g_pBackLightManager->ReLoadConfig();
        g_pBackLightManager->SetBacklightLevel(msg.wParam);
        g_pBackLightManager->SetEXPBacklightLevel(msg.wParam);
    }
    else if (DSK_MSG_GET_ACTIVITE_STATUS == msg.message)
    {
#if IF_FEATURE_PROVISIONING_MONTHLY
        int iRet = g_pBackLightManager->GetInactivityTimeExpire() && !talklogic_SessionExist() ? 0 : 1;
        msg.ReplyMessage(iRet);
        BACKLIGHT_INFO("BackLight PHONE_MSG_GETACTIVETATUS: ReplyMessage(%d)", iRet);
#else
        int iRet = !talklogic_SessionExist() ? 0 : 1;
        msg.ReplyMessage(iRet);
#endif
    }


    return FALSE;
}

BackLightManager * BackLightManager::GetInstance()
{
    // Instantiate one if not exists.
    if (m_pOneSelf == NULL)
    {
        m_pOneSelf = new BackLightManager();
    }
    return m_pOneSelf;
}

void BackLightManager::ReleaseInstance()
{
    // Release the instance if exists.
    if (m_pOneSelf != NULL)
    {
        delete m_pOneSelf;
        m_pOneSelf = NULL;
    }
}

BackLightManager::BackLightManager()
    : m_bOpenBackLight(true),
      m_iLastLigthLevel(-1),
      m_iLastExpLigthLevel(-1),
      m_bPreviewBackLightLevel(false)
{
    etl_RegisterMsgHandle(DSK_MSG_LIGHT_STATUS_CHANGE, DSK_MSG_LIGHT_STATUS_CHANGE,
                          OnBackLightConfigChange);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnBackLightConfigChange);
    etl_RegisterMsgHandle(BACKLIGHT_SET_ENABLE_MSG, BACKLIGHT_SET_ENABLE_MSG, OnBackLightConfigChange);
    etl_RegisterMsgHandle(DSK_MSG_LIGHT_LEVEL_CHANGE, DSK_MSG_LIGHT_LEVEL_CHANGE,
                          OnBackLightConfigChange);
#if IF_FEATURE_PROVISIONING_MONTHLY
    etl_RegisterMsgHandle(DSK_MSG_GET_ACTIVITE_STATUS, DSK_MSG_GET_ACTIVITE_STATUS,
                          OnBackLightConfigChange);

    m_bInactivityTimeExpire = false;
    m_iInactivityInterval = 30;
    ReLoadInactivityTimer();
#endif
    m_bShowSetBKUI = false;

    ReLoadConfig();

    // 添加进默认的Item
    BackLightEvent objEvent = m_EventParse.GetEventDataByType(BACK_LIGHT_DEFAULT);
    m_listSaverEvent.push_back(objEvent);
    this->DoEvent(objEvent);
}

BackLightManager::~BackLightManager()
{

}

void BackLightManager::AddEvent(BACK_LIGHT_TYPE eType)
{
    if (eType <= BACK_LIGHT_NONE || eType >= BACK_LIGHT_MAX)
    {
        BACKLIGHT_WARN("BackLightManager::AddEvent type error!");
        return;
    }

    BACKLIGHT_INFO("BackLightManager::AddEvent [%d]", eType);
    IterBackLight it = FindItemByType(eType);

    // 若没有相应的Event插入该类型的Event
    if (it == m_listSaverEvent.end())
    {
        BackLightEvent objEvent = m_EventParse.GetEventDataByType(eType);
        // 容错：如果配置文件里没有的话，就添加指定类型的灯状态
        if (objEvent.eType == BACK_LIGHT_NONE)
        {
            objEvent.eType = eType;
        }
        // 若Event优先级最高则执行该Event
        if (objEvent.priority <= GetTopEventPriority())
        {
            DoEvent(objEvent);
        }
        m_listSaverEvent.push_back(objEvent);
    }
    else
    {
        // 若插入的是以存在的Event
        // 并且优先级最高
        BackLightEvent objEvent = GetTopEvent();
        if (objEvent.priority <= (*it).priority)
        {
            DoEvent(objEvent);
        }
    }

#if OPEN_BACK_LIGHT_DEBUG
    PrintfBackLightInfo();
#endif
}

void BackLightManager::EarseEvent(BACK_LIGHT_TYPE eType)
{
    BACKLIGHT_INFO("BackLight Earse Event(%d)", eType);
    if (eType <= BACK_LIGHT_NONE || eType >= BACK_LIGHT_MAX)
    {
        return;
    }

    if (eType == BACK_LIGHT_DEFAULT)
    {
        return;
    }

    IterBackLight it = FindItemByType(eType);
    // 若没有相应的Event不进行后续的处理
    if (it == m_listSaverEvent.end())
    {
        return;
    }

    if (-1 == (*it).brightTimer)
    {
        PowerSaving_SetAvoidPowerSaving(false);
    }
    // 若删除的是优先级最高的Event
    BackLightEvent objItem = GetTopEvent();
    if (objItem.eType == (*it).eType)
    {
        m_listSaverEvent.erase(it);
        DoEvent(GetTopEvent());
    }
    else
    {
        m_listSaverEvent.erase(it);
    }

#if OPEN_BACK_LIGHT_DEBUG
    PrintfBackLightInfo();
#endif
}

void BackLightManager::Reset()
{
    AddEvent(BACK_LIGHT_DEFAULT);
}

// 重新执行优先级最高的屏保策略
void BackLightManager::RefreshEvent()
{
    this->DoEvent(GetTopEvent());
}

// 获取优先级最高的Item
int BackLightManager::GetTopEventPriority()
{
    return GetTopEvent().priority;
}

// 获取优先级最高的Item
BackLightEvent BackLightManager::GetTopEvent()
{
    // 防止链表为空照成死机的容错
    if (m_listSaverEvent.size() == 0)
    {
        BACKLIGHT_WARN("m_listSaverEvent is Null!");
        return m_EventParse.GetEventDataByType(BACK_LIGHT_DEFAULT);
    }

    IterBackLight iter = m_listSaverEvent.begin();
    IterBackLight itTopEvent = iter;

    for (/* */; iter != m_listSaverEvent.end(); ++iter)
    {
        if ((*iter).priority < (*itTopEvent).priority)
        {
            itTopEvent = iter;
        }
    }

    return *itTopEvent;
}

IterBackLight BackLightManager::FindItemByType(BACK_LIGHT_TYPE eType)
{
    IterBackLight iter = m_listSaverEvent.begin();
    for (/* */; iter != m_listSaverEvent.end(); ++iter)
    {
        if ((*iter).eType == eType)
        {
            break;
        }
    }

    return iter;
}

int BackLightManager::GetBackLightTimer()
{
    int iTimer = configParser_GetConfigInt(kszBackLightTime);

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        if (iTimer == 1)
        {
            BACKLIGHT_WARN("color phone can not be %d", iTimer);
            iTimer = 0;
        }
    }

    // 配置表中存放的单位是秒
    // http://192.168.1.99/Bug.php?BugID=22028
    // http://10.2.1.199/Bug.php?BugID=39401
    return iTimer * 1000;
}

void BackLightManager::DoEvent(const BackLightEvent & objEvent)
{
    // stop timer
    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }

#if IF_FEATURE_PROVISIONING_MONTHLY
    SetInactivityTimer();
#endif

    // 背光灯AlwaysOff
    if (m_bDisableBackLight)
    {
        return;
    }

    if (m_bShowSetBKUI
            && m_objBkLevelTimer.IsTimerRuning())
    {
        // http://10.2.1.199/Bug.php?BugID=49251
        // start timer
        int iTimer = GetBackLightTimer();
        if (iTimer != 0)
        {
            m_objBkLevelTimer.SetTimer(iTimer, NULL, MK_MSG_ENTRY(this, BackLightManager::OnBkLevelTimerOut));
        }
    }

    int iLightLevel = 0;
    if (objEvent.initLight == 1)
    {
        iLightLevel = configParser_GetConfigInt(kszActiveLevel);// read active level
    }
    else
    {
        iLightLevel = configParser_GetConfigInt(kszInactiveLevel);// read inactive level
    }

    // count time
    int iTimer = objEvent.brightTimer;
    // brightTimer = 0 读取配置中的亮灯时间
    if (objEvent.eType == BACK_LIGHT_DEFAULT || objEvent.brightTimer == 0)
    {
#if OPEN_BACK_LIGHT_DEBUG
        BACKLIGHT_INFO("objEvent.eType == BACK_LIGHT_DEFAULT || objEvent.brightTimer == 0");
#endif
        // 读取时间
        iTimer = GetBackLightTimer();

        //亮灯
        MySetBacklightLevel(iLightLevel);

        if (iTimer <= 0) //配置异常，按Always On处理
        {
            return;
        }
    }
    else
    {
#if OPEN_BACK_LIGHT_DEBUG
        BACKLIGHT_INFO("BackLightManager::DoEvent [%d]", objEvent.eType);
#endif

        //其他情况先亮灯
        MySetBacklightLevel(iLightLevel);
        // 时间为-1,表示这个过程无限长，不设置灭灯定时器
        if (objEvent.brightTimer == -1)
        {
            PowerSaving_SetAvoidPowerSaving(true);
            return;
        }
    }

    // start timer
    m_objTimer.SetTimer(iTimer, NULL, MK_MSG_ENTRY(this, BackLightManager::OnTimerOut));
}

void BackLightManager::PrintfBackLightInfo()
{
#if OPEN_BACK_LIGHT_DEBUG
    BACKLIGHT_INFO("******PrintfBackLightInfo******");
    IterBackLight it = m_listSaverEvent.begin();

    for (; it != m_listSaverEvent.end(); ++it)
    {
        BACKLIGHT_INFO("eType[%d], initLight[%d], priority[%d], brightTimer[%d]"
                       , (*it).eType, (*it).initLight, (*it).priority, (*it).brightTimer);
    }

    BACKLIGHT_INFO("******PrintfBackLightInfo******");
#endif
}

BOOL BackLightManager::OnTimerOut(msgObject & objMessage)
{
    // stop timer
    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }

    int iLightLevel = 0;
    if (GetTopEvent().initLight != 1)
    {
        iLightLevel = configParser_GetConfigInt(kszActiveLevel);// read active level
    }
    else
    {
#ifdef _T42
        iLightLevel = 0;
#else
        iLightLevel = configParser_GetConfigInt(kszInactiveLevel);// read inactive level
#endif
    }
    BACKLIGHT_INFO("OnTimerOut InactiveLevel[%d]", iLightLevel);

    if (!PowerSaving_IsInPowerSavingMode())
    {
        if (m_bOpenBackLight)
        {
            BACKLIGHT_INFO("PowerSaving_IsInPowerSavingMode [%d]", iLightLevel);
            SetBacklightLevel(iLightLevel);
        }
    }

    return TRUE;
}

BOOL BackLightManager::OnBkLevelTimerOut(msgObject & objMessage)
{
    // stop timer
    if (m_objBkLevelTimer.IsTimerRuning())
    {
        m_objBkLevelTimer.KillTimer();
    }

    if (m_bShowSetBKUI)
    {
        int iLightLevel = configParser_GetConfigInt(kszInactiveLevel);

        BACKLIGHT_INFO("OnBkLevelTimerOut InactiveLevel[%d]", iLightLevel);
        SetBacklightLevel(iLightLevel);
    }

    return TRUE;
}

void BackLightManager::OpenBackLightEvent(bool bOpen)
{
    if (m_bOpenBackLight != bOpen)
    {
        m_bOpenBackLight = bOpen;

        // 重新开启背光灯事件
        // 需要重新执行最高优先级的事件
        if (m_bOpenBackLight)
        {
            backlight_RefreshEvent();
        }
    }
}

void BackLightManager::ClearData()
{
    m_listSaverEvent.clear();
    m_listSaverEvent.push_back(m_EventParse.GetEventDataByType(BACK_LIGHT_DEFAULT));
}

void BackLightManager::MySetBacklightLevel(int iLightLevel)
{
    if (m_bOpenBackLight)
    {
        SetBacklightLevel(iLightLevel);
        SetEXPBacklightLevel(iLightLevel);
    }
}

bool BackLightManager::IsOpenBackLight()
{
    return m_bOpenBackLight;
}

//液晶也扩展台背光灯等级的对应关系，10级对应到四级
bool BackLightManager::GetExpLightValueByLevel(int & iLightValue, int iLightLevel)
{
    if (iLightLevel == 0)
    {
        iLightValue = 0;
        return true;
    }
    if (1 <= iLightLevel && iLightLevel <= 10)
    {
        iLightValue = (iLightLevel / 3 + 1) * 25;
        if (iLightValue >= 100)
        {
            iLightValue = 75;
        }
        return true;
    }
    return false;
}

bool BackLightManager::GetColorExpLightValueByLevel(int & iLightValue, int iLightLevel)
{
    if (iLightLevel == 0)
    {
        iLightValue = 0;
        return true;
    }
    if (1 <= iLightLevel && iLightLevel <= 10)
    {
        iLightValue = iLightLevel;
        return true;
    }
    else
    {
        iLightValue = 10;
        return true;
    }

    return false;
}

void BackLightManager::SetBacklightLevel(int iLightLevel)
{
    if (m_bDisableBackLight)
    {
        return;
    }

    // 背光灯防抖
    if (iLightLevel == m_iLastLigthLevel)
    {
        return;
    }

    if (0 == m_iLastLigthLevel
            && 0 < iLightLevel)
    {
        BACKLIGHT_INFO("Notify [POWERSAVINGMEG_SCREEN_AWAKE][%d]", POWERSAVINGMEG_SCREEN_AWAKE);
        etl_NotifyApp(false, POWERSAVIGNMEG_NOTIFY_OPERATE, 0,
                      0); // 视为被动操作，powersaving模块会通知UI唤醒屏幕
    }

    if (m_bShowSetBKUI)
    {
        // http://10.2.1.199/Bug.php?BugID=49251
        // start timer
        int iTimer = GetBackLightTimer();
        if (iTimer != 0)
        {
            m_objBkLevelTimer.SetTimer(iTimer, NULL, MK_MSG_ENTRY(this, BackLightManager::OnBkLevelTimerOut));
        }
    }

    using namespace yl;
    int ilight = min(max(iLightLevel * 10, 0), 100);
    BACKLIGHT_INFO("BackLight Set light(%d)", ilight);
    etl_NotifyApp(false, BACKLIGHT_LEVEL_CHANGED_MSG, iLightLevel, m_iLastLigthLevel);
    devicelib_SetBackLightLevel(ilight);// init light
#ifndef _VPX
    if (exp_GetExpTypeByIndex(0) == DT_EXP_39
            || exp_GetExpTypeByIndex(0) == DT_EXP_40
            || exp_GetExpTypeByIndex(0) == DT_EXP_CM69
            || exp_GetExpTypeByIndex(0) == DT_EXP_20)
    {
        int iEXPNum = exp_GetEXPNum();
        for (int i = 0; i < iEXPNum; i++)
        {
            //根据背光等级获取扩展台背光值
            int iValue;
            if (!GetExpLightValueByLevel(iValue, iLightLevel))
            {
                continue;
            }

            // 发消息给EXP线程，让EXP线程处理显示消息
            msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_BACKLIGHT, i, iValue);
        }
    }
#endif

    m_iLastLigthLevel = iLightLevel;
}

void BackLightManager::SetEXPBacklightLevel(int iLightLevel)
{
    if (m_bDisableBackLight)
    {
        return;
    }

    // 背光灯防抖
    if (iLightLevel == m_iLastExpLigthLevel)
    {
        return;
    }

    m_iLastExpLigthLevel = iLightLevel;

    if (exp_GetExpTypeByIndex(0) == DT_EXP_50)
    {
        int iValue;
        if (GetColorExpLightValueByLevel(iValue, iLightLevel))
        {
            exp_EXPBackLightUpdate(iValue);
        }
    }
}

void BackLightManager::UpdateExpLight()
{
    BACKLIGHT_INFO("m_bDisableBackLight=%d,m_iLastLigthLevel=%d", m_bDisableBackLight,
                   m_iLastLigthLevel);

    int nLightLevel = m_bDisableBackLight ? 0 : m_iLastLigthLevel;

    if (exp_GetExpTypeByIndex(0) == DT_EXP_39
            || exp_GetExpTypeByIndex(0) == DT_EXP_40
            || exp_GetExpTypeByIndex(0) == DT_EXP_CM69
            || exp_GetExpTypeByIndex(0) == DT_EXP_20)
    {
        int iEXPNum = exp_GetEXPNum();
        for (int i = 0; i < iEXPNum; i++)
        {
            //根据背光等级获取扩展台背光值
            int iValue;
            if (!GetExpLightValueByLevel(iValue, nLightLevel))
            {
                continue;
            }
            // 发消息给EXP线程，让EXP线程处理显示消息
            msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_BACKLIGHT, i, iValue);
        }
    }
    else if (exp_GetExpTypeByIndex(0) == DT_EXP_50)
    {
        int iValue;
        if (GetColorExpLightValueByLevel(iValue, nLightLevel))
        {
            exp_EXPBackLightUpdate(iValue);
        }
    }
}

void BackLightManager::ResetExpLightShake()
{
    m_iLastExpLigthLevel = -1;
}

void BackLightManager::EnableBackLight(bool bEnable)
{
    BACKLIGHT_INFO("Enable BackLight [%d]", bEnable);

    // 禁用背光灯前先关闭
    if (!bEnable)
    {
        SetBacklightLevel(0);
        SetEXPBacklightLevel(0);
    }

    OpenBackLightEvent(bEnable);
}

void BackLightManager::ReLoadConfig()
{
    bool bDisabled = (GetBackLightTimer() == BACKLIGHT_DISABLE_VALUE);

    if (bDisabled)
    {
        SetBacklightLevel(0);
        SetEXPBacklightLevel(0);
    }
    m_bDisableBackLight = bDisabled;
}

int BackLightManager::GetLightLevel()
{
    return m_iLastLigthLevel;
}


#if IF_FEATURE_PROVISIONING_MONTHLY
BOOL BackLightManager::OnInactiveTimerOut(msgObject & objMessage)
{
    BACKLIGHT_INFO("BackLightManager::OnInactiveTimerOut", DSK_MSG_GET_ACTIVITE_STATUS);
    // stop timer
    m_bInactivityTimeExpire = true;
    if (m_objInactiveTimer.IsTimerRuning())
    {
        m_objInactiveTimer.KillTimer();
    }
    return TRUE;
}

BOOL BackLightManager::GetInactivityTimeExpire()
{
    return m_bInactivityTimeExpire;
}

void BackLightManager::ReLoadInactivityTimer()
{
    m_iInactivityInterval = configParser_GetConfigInt(kszInactivityTimeExpire);
    BACKLIGHT_INFO("BackLightManager::ReLoadInactivityTimer m_iInactivityInterval[%d]",
                   m_iInactivityInterval);
    SetInactivityTimer();
}

void BackLightManager::SetInactivityTimer()
{
    if (m_iInactivityInterval > 0)
    {
        m_bInactivityTimeExpire = false;
        if (m_objInactiveTimer.IsTimerRuning())
        {
            m_objInactiveTimer.KillTimer();
        }
        m_objInactiveTimer.SetTimer(m_iInactivityInterval * 60 * 1000, NULL, MK_MSG_ENTRY(this,
                                    BackLightManager::OnInactiveTimerOut));
    }
    else
    {
        m_bInactivityTimeExpire = true;
    }
}
#endif

