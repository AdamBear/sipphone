#ifndef __CALLPICKUP_MANAGER_H__
#define __CALLPICKUP_MANAGER_H__

#include "blfcommon.h"
#include "ETLLib.hpp"

class CPickUpManager: public CMKProcessor
{

public:
    static CPickUpManager & GetInstance();

    // 更新Pickup的数据
    void UpdateCallPickup(yl::string strKey, eBLFMangaerType eManagerType, ePickUpOpt eOptType);

    void UpdateCallPark(yl::string strKey, int iDsskeyID, ePickUpOpt eOptType);

    // 获取Pickup的菜单数据
    bool GetMenuList(YLList<yl::string> & listMenuData);

    int GetPickupDsskeyIdByIndex(int index);

    int GetPickupDialogIndexByIndex(int index);

    // 取消当前Pickup提示数据的显示，直到有新数据得到更新
    void CancelPickupMenuShow();

    void ClearCallParkList(int iAccountId = -1, int iDsskeyID = -1, const yl::string strKey = "");

    bool BlfIdleRing();

    void BlfStopVoice();

    BOOL PlayBlfToneInTalk();

    void BlfPlayVoice();

    bool IsBlfIdleRing();

    bool StopBlfIdleRing();

protected:

    //定时器消息
    BOOL OnTimer(msgObject & refObj);

    BOOL OnCallParktimer(msgObject& refObj);

    BOOL OnRingTimer(msgObject & refObj);

    //// 拨放铃音
    void PlayTone();

    void FlushCallPickup();

    bool IsAllExistInList(YLList<yl::string> & lstDecMenu, YLList<yl::string> & lstSrcMenu);

private:

    CPickUpManager();

private:

    //存在来电的监控
    YLList<RingMonitor> m_listData;

    YLList<RingMonitor> m_listCallParkData;

    YLVector<int> m_listDsskeyID;

    mkThreadTimer m_FlushCallparkTimer;

    mkThreadTimer m_FlushTimer;

    mkThreadTimer m_RingTimer;

    bool m_bIsIdleRing;
};

#define g_objPickupManager (CPickUpManager::GetInstance())


#endif
