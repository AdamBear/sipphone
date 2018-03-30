#ifndef _BACK_LIGHT_H_
#define _BACK_LIGHT_H_

#include <ETLLib.hpp>
#include <ylstring.h>
#include <yllist.h>
//#include "define/common.h"
#include "modbacklight.h"

struct BackLightEvent
{
    BACK_LIGHT_TYPE eType;
    int initLight;
    int priority;
    int brightTimer;

    BackLightEvent()
    {
        eType = BACK_LIGHT_NONE;
        initLight = 1;
        priority  = 0;
        brightTimer = -1;
    }
};

typedef YLList<BackLightEvent>  ListBackLight;
typedef ListBackLight::iterator     IterBackLight;
class BackLightEventParse
{
public:
    BackLightEventParse();
    ~BackLightEventParse();
    BackLightEvent GetEventDataByType(BACK_LIGHT_TYPE eType);

private:
    bool LoadEventData();

    bool CheckParseResult(const BackLightEvent & objEvent);

    BACK_LIGHT_TYPE StringToType(const yl::string & strTypeName);

private:
    ListBackLight m_listEventData;
};

class BackLightManager : public CMKProcessor
{
public:

    // Get the singleton instance.
    static BackLightManager * GetInstance() DECLARE_ATTR;

    // Release the singleton instance.
    static void ReleaseInstance();

    void AddEvent(BACK_LIGHT_TYPE eType);

    void EarseEvent(BACK_LIGHT_TYPE eType);

    void Reset();

    void RefreshEvent();

    void OpenBackLightEvent(bool bOpen);

    // 设置背光
    void SetBacklightLevel(int iLightLevel);

    // 设置EXP背光
    void SetEXPBacklightLevel(int iLightLevel);

    void UpdateExpLight();

    void ResetExpLightShake();

    bool IsOpenBackLight();

    //液晶扩展台背光灯等级的对应关系，10级对应到四级
    bool GetExpLightValueByLevel(int & iLightValue, int iLightLevel);
    bool GetColorExpLightValueByLevel(int & iLightValue, int iLightLevel);

    // http://10.2.1.199/Bug.php?BugID=49251
    // 设置是否在背光灯设置界面
    void ShowBacklightUI(bool bShowBKUI)
    {
        m_bShowSetBKUI = bShowBKUI;
    }

    // 启用/禁用背光灯
    void EnableBackLight(bool bDisable);

    // 设置预览背光灯等级模式
    void SetPreviewMode(bool bPreview)
    {
        m_bPreviewBackLightLevel = bPreview ;
    }

    // 获取是否为预览背光灯等级
    bool GetPreviewMode()
    {
        return m_bPreviewBackLightLevel;
    }

    // 重新获取设置数据
    void ReLoadConfig();

    // 获取上一个背光灯等级
    int GetLastLightLevel()
    {
        return m_iLastLigthLevel;
    }

    int GetLightLevel();

#if IF_FEATURE_PROVISIONING_MONTHLY
    void SetInactivityTimer();
    void ReLoadInactivityTimer();
    BOOL GetInactivityTimeExpire();
#endif

private:
    BackLightManager();
    ~BackLightManager();

    int GetTopEventPriority();// 获取优先级最高的Item

    BackLightEvent GetTopEvent();// 获取优先级最高的Item

    IterBackLight FindItemByType(BACK_LIGHT_TYPE eType);

    void ClearData();

    void DoEvent(const BackLightEvent & objEvent);

    void MySetBacklightLevel(int iLightLevel);

    BOOL OnTimerOut(msgObject & objMessage);

    void PrintfBackLightInfo();

    int GetBackLightTimer();

    // http://10.2.1.199/Bug.php?BugID=49251
    BOOL OnBkLevelTimerOut(msgObject & objMessage);

#if IF_FEATURE_PROVISIONING_MONTHLY
    BOOL OnInactiveTimerOut(msgObject & objMessage);
#endif

private:
    BackLightEventParse m_EventParse;
    static BackLightManager  *  m_pOneSelf;
    ListBackLight       m_listSaverEvent;
    mkThreadTimer       m_objTimer;
    bool                m_bOpenBackLight;
    int                 m_iLastLigthLevel; //上一次的亮度
    int                 m_iLastExpLigthLevel; //EXP上一次的亮度
    // http://10.2.1.199/Bug.php?BugID=49251
    bool                m_bShowSetBKUI;
    bool                m_bDisableBackLight; //是否启用背光灯
    mkThreadTimer       m_objBkLevelTimer;
    bool                m_bPreviewBackLightLevel; // 是否为预览背光灯等级

#if IF_FEATURE_PROVISIONING_MONTHLY
    // 是否放置一段时间没人使用
    bool                m_bInactivityTimeExpire;
    // 放置不动的时间
    mkThreadTimer       m_objInactiveTimer;
    int                 m_iInactivityInterval;
#endif
};

#define g_pBackLightManager (BackLightManager::GetInstance())
#endif

