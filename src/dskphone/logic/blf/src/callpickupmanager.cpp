#include "callpickupmanager.h"
#include "blf_inc.h"
#include "modblf.h"
#include "lamp/backlight/include/modbacklight.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "configparser/modconfigparser.h"
#include "talk/talklogic/include/modtalklogic.h"

#define BLF_PLAY_TONE_TIME 10*1000

CPickUpManager & CPickUpManager::GetInstance()
{
    static CPickUpManager Instace;
    return Instace;
}

CPickUpManager::CPickUpManager()
{
    m_bIsIdleRing = false;
}

void CPickUpManager::UpdateCallPark(yl::string strKey, int iDsskeyID, ePickUpOpt eOptType)
{
    APP_INFO("CPickUpManager::UpdateCallPark strKey[%s], iDsskeyID[%d], eOptType[%d]", strKey.c_str(), iDsskeyID, eOptType);

    bool bChange = false;

    for (YLList<RingMonitor>::iterator it = m_listCallParkData.begin(); it != m_listCallParkData.end(); ++it)
    {
        RingMonitor & refIter = *it;

        if (refIter.m_strKey.compare(strKey) != 0)
        {
            continue;
        }

        ////监控号码一致，但是dsskey id不一样 也保存
        if (refIter.m_vecDsskeyID[0] != iDsskeyID)
        {
            continue;
        }

        if (eOptType == Opt_Remove)
        {
            m_listCallParkData.erase(it);
        }

        bChange = true;
        break;
    }

    if (!bChange && eOptType == Opt_Add)
    {
        RingMonitor AddData;
        AddData.m_strKey = strKey;
        AddData.m_vecDsskeyID.push_back(iDsskeyID);
        m_listCallParkData.push_back(AddData);

        bChange = true;
    }

    if (bChange && !m_FlushCallparkTimer.IsTimerRuning())
    {
        APP_INFO("Set CallPickFlush Timer");
        m_FlushCallparkTimer.SetTimer(50, NULL, MK_MSG_ENTRY(this, CPickUpManager::OnCallParktimer));
    }
}

void CPickUpManager::UpdateCallPickup(yl::string strKey, eBLFMangaerType eManagerType,
                                      ePickUpOpt eOptType)
{
    APP_INFO("CPickUpManager::UpdateCallPickup strKey[%s],eManagerType[%d],eOptType[%d]",
             strKey.c_str(), eManagerType, eOptType);

    bool bChange = false;

    YLList<yl::string> lstMenu;
    YLVector<int> vecDsskeyID;
    blf_GetPickUpShowContentByKey(lstMenu, vecDsskeyID, strKey, eManagerType);

    for (YLList<RingMonitor>::ListIterator iter = m_listData.begin(); iter != m_listData.end(); ++iter)
    {
        RingMonitor & refIter = *iter;

        if (refIter.m_eManagerType != eManagerType)
        {
            continue;
        }

        if (refIter.m_strKey.compare(strKey) != 0)
        {
            continue;
        }

        if (eOptType == Opt_Remove)
        {
            // 全部减少为空时才清除
            if (vecDsskeyID.size() <= 0)
            {
                m_listData.erase(iter);
            }
            else
            {
                // 更新显示数据
                refIter.m_lstMenu = lstMenu;
                refIter.m_vecDsskeyID = vecDsskeyID;
            }
        }
        else if (eOptType == Opt_Add)
        {
            // 更新显示数据
            refIter.m_lstMenu = lstMenu;
            refIter.m_vecDsskeyID = vecDsskeyID;
        }

        // 过滤重复
        bChange = true;
        break;
    }

    // 新增的, 且被分配到Dsskey, 才记录
    if (!bChange && eOptType == Opt_Add && vecDsskeyID.size() > 0)
    {
        RingMonitor AddData;
        AddData.m_eManagerType = eManagerType;
        AddData.m_strKey = strKey;
        AddData.m_lstMenu = lstMenu;
        AddData.m_vecDsskeyID = vecDsskeyID;
        m_listData.push_back(AddData);

        bChange = true;
    }

    if (bChange && !m_FlushTimer.IsTimerRuning())
    {
        APP_INFO("Set PickupFlush Timer");
        m_FlushTimer.SetTimer(50, NULL, MK_MSG_ENTRY(this, CPickUpManager::OnTimer));
    }
}

void CPickUpManager::PlayTone()
{
    voice_PlayTone(TONE_TEXT_MSG, 300);
}

void CPickUpManager::FlushCallPickup()
{
    if (m_listData.size() == 0)
    {
        // S1: 如果当前列表为空，则退出提示的MENU UI
        DSSKEY_INFO("[PickupMenu] Exit Pickup Menu UI.");
        BlfStopVoice();
        idleScreen_DropStatus(PS_STATE_PICKUP_MENU);
    }
    else
    {
        // 点亮背光灯,重启节电
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), BLF_MESSAGE_FLUSH, 0, 0);

        // S2: 弹出新UI或者刷新UI
        BlfPlayVoice();

        if (configParser_GetConfigInt(kszIndicationBLFPickup) == 1)
        {
            idleScreen_SetStatus(PS_STATE_PICKUP_MENU);
        }
    }
}

BOOL CPickUpManager::OnTimer(msgObject & refObj)
{
    APP_INFO("---------CPickUpManager::OnTimer--------------");

    m_FlushTimer.KillTimer();

    FlushCallPickup();

    return TRUE;
}

BOOL CPickUpManager::OnCallParktimer(msgObject & refObj)
{
    m_FlushCallparkTimer.KillTimer();

    if (m_listCallParkData.size() == 0)
    {
        BlfStopVoice();
    }
    else
    {
        // 点亮背光灯,重启节电
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), BLF_MESSAGE_FLUSH, 0, 0);

        BlfPlayVoice();
    }

    return TRUE;
}

BOOL CPickUpManager::OnRingTimer(msgObject & refObj)
{
    m_RingTimer.KillTimer();
    if (!talklogic_SessionExist()
            || !talkLogic_IsSessionInTalking()
            || (0 == m_listData.size()
                && 0 == m_listCallParkData.size()))
    {
        return FALSE;
    }

    voice_PlayTone(TONE_TEXT_MSG, 300);
    m_RingTimer.SetTimer(BLF_PLAY_TONE_TIME, NULL, MK_MSG_ENTRY(this, CPickUpManager::OnRingTimer));

    return TRUE;
}

bool CPickUpManager::GetMenuList(YLList<yl::string> & listMenuData)
{
    if (m_listData.size() == 0)
    {
        return false;
    }

    m_listDsskeyID.clear();

    for (YLList<RingMonitor>::ListIterator iter = m_listData.begin(); iter != m_listData.end(); iter++)
    {
        RingMonitor & refData = *iter;

        for (YLList<yl::string>::ListIterator itr_Data = refData.m_lstMenu.begin();
                itr_Data != refData.m_lstMenu.end(); ++itr_Data)
        {
            listMenuData.push_back(*itr_Data);
        }

        for (int i = 0; i < refData.m_vecDsskeyID.size(); ++i)
        {
            m_listDsskeyID.push_back(refData.m_vecDsskeyID[i]);
        }
    }

    return listMenuData.size() != 0;
}

int CPickUpManager::GetPickupDsskeyIdByIndex(int index)
{
    if (index >= 0
            && index < m_listDsskeyID.size())
    {
        return m_listDsskeyID[index];
    }

    return -1;
}

int CPickUpManager::GetPickupDialogIndexByIndex(int index)
{
    if (index >= 0
            && index < m_listDsskeyID.size())
    {
        int nId =  m_listDsskeyID[index];

        int nDialogIndex = 0;

        for (int i = index - 1; i > -1; i--)
        {
            if (m_listDsskeyID[i] == nId)
            {
                ++nDialogIndex;
            }
        }

        return nDialogIndex;
    }

    return -1;
}

// 取消当前Pickup提示数据的显示，直到有新数据得到更新
void CPickUpManager::CancelPickupMenuShow()
{
    // 按Cancel则返回Idle界面，如果又有新来电则只显示新来电的信息 - 需求如此
    DSSKEY_INFO("[Pickup] Clear Status List On [Cancel] Press !");

    m_listData.clear();
    BlfStopVoice();

    idleScreen_DropStatus(PS_STATE_PICKUP_MENU);
}
bool CPickUpManager::IsAllExistInList(YLList<yl::string> & lstDecMenu,
                                      YLList<yl::string> & lstSrcMenu)
{
    for (YLList<yl::string>::ListIterator itr_Dec = lstDecMenu.begin(); itr_Dec != lstDecMenu.end();
            ++itr_Dec)
    {
        YLList<yl::string>::ListIterator itr_Src = lstSrcMenu.begin();
        for (; itr_Src != lstSrcMenu.end(); ++itr_Src)
        {
            if (*itr_Dec == *itr_Src)
            {
                break;
            }
        }

        // 只要有一个不存在, 就返回
        if (itr_Src == lstSrcMenu.end())
        {
            return false;
        }
    }

    return true;
}

void CPickUpManager::ClearCallParkList(int iAccountId /* = -1 */, int iDsskeyID/* = -1 */, const yl::string strKey /* = "" */)
{
    //清除单个账号所有的监控号码
    if (iAccountId != -1)
    {
        for (YLList<RingMonitor>::iterator it = m_listCallParkData.begin(); it != m_listCallParkData.end();)
        {
            RingMonitor & refIter = *it;

            if (iAccountId == dsskey_GetDsskeyLine(refIter.m_vecDsskeyID[0]))
            {
                m_listCallParkData.erase(it++);
            }
            else
            {
                it++;
            }
        }

        BlfStopVoice();
        return;
    }

    //dsskey 监控方移除
    if (dsskey_GetDsskeyType(iDsskeyID) == DT_CALLPARK)
    {
        UpdateCallPark(strKey, iDsskeyID, Opt_Remove);
    }
}

void CPickUpManager::BlfPlayVoice()
{
    DSSKEY_INFO("CPickUpManager::BlfPlayVoice");
    if ((m_listData.size() == 0 || configParser_GetConfigInt(kszPickupAllowBlfRing) != 1)
            && (m_listCallParkData.size() == 0 || configParser_GetConfigInt(kszCallParkBlfAudioSwitch) != 1))
    {
        return;
    }

    if (talklogic_SessionExist())
    {
        PlayBlfToneInTalk();
    }
    else
    {
        BlfIdleRing();
    }
}

bool CPickUpManager::BlfIdleRing()
{
    //开关同时开启才响铃,没有要更新的数据也不响音
    // 通话状态下，不响铃声
    if ((0 == m_listData.size()
            && 0 == m_listCallParkData.size())
            || talklogic_SessionExist())
    {
        return false;
    }

    m_bIsIdleRing = true;
    m_RingTimer.KillTimer();
    yl::string strBuffer = configParser_GetConfigString(kszBlfRingType);
    voice_PlayRing(strBuffer.c_str(), "Splash.wav");

    return true;
}

void CPickUpManager::BlfStopVoice()
{
    m_RingTimer.KillTimer();
    m_bIsIdleRing = false;
    //没有可更新的dsskey数据时，如果当前存在session，则停止tone音
    if ((0 == m_listData.size()
            && 0 == m_listCallParkData.size())
            && talklogic_SessionExist())
    {
        voice_StopTone(TONE_TEXT_MSG);
        return;
    }

    //IDLE 下，直接停止铃声
    if (0 == m_listData.size()
            && 0 == m_listCallParkData.size())
    {
        voice_StopRing();
        return;
    }

    if (talklogic_SessionExist())
    {
        //通话存在时，停止Tone音
        voice_StopTone(TONE_TEXT_MSG);
    }

    ////Dial和Ring界面不响任何铃声
    ////没有可更新的dsskey数据时,且不存在话路时，停止播放任何铃声
    //if ( (true == talklogic_IsFoucsInRingSession() || true == talklogic_IsDialSessionExist())
    //  || (0 == m_listData.size() && !talklogic_SessionExist()) )
    //{
    //  m_RingTimer.KillTimer();
    //  voice_StopTone(TONE_TEXT_MSG);
    //  voice_StopRing();
    //  return;
    //}
}

BOOL CPickUpManager::PlayBlfToneInTalk()
{
    //有数据需要停止铃声
    if ((m_listData.size() > 0
            || m_listCallParkData.size() > 0)
            && m_bIsIdleRing)
    {
        m_bIsIdleRing = false;
        voice_StopRing();
    }
    if (!m_RingTimer.IsTimerRuning())
    {
        m_RingTimer.SetTimer(BLF_PLAY_TONE_TIME, NULL, MK_MSG_ENTRY(this, CPickUpManager::OnRingTimer));
    }
    return TRUE;
}

bool CPickUpManager::IsBlfIdleRing()
{
    return m_bIsIdleRing;
}

bool CPickUpManager::StopBlfIdleRing()
{
    //有数据需要停止铃声
    if (m_bIsIdleRing)
    {
        m_bIsIdleRing = false;
        voice_StopRing();
    }
    return true;
}
