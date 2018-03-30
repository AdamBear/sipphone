#pragma once

#include "ylhashmap.h"
#include "talkadapter_data.h"
#include "sessionstateproxy.h"
#include "talk/talkproxy/include/helpclass.h"
#include "broadsoft/callpark/include/callparkcommon.h"

class CBeTransferredSession;
class CLocalConfSession;
class CSingleSession;

class CTalkUIDataManager
    : public app::SingletonObject<CTalkUIDataManager>
{
public:
    typedef YLHashMap<int, DataLogic2UIBase*>       MAP_SESSION_DATA;

public:

    /* 获取ui数据入口 */
    bool GetDataBySessionID(const CSessionStateProxy& proxy, DataLogic2UI& logicData, bool bBaseData = false);

    /* 获取来电显示名 */
    bool GetIncomingCallName(yl::string& strNum);

public:

    /* 获取ui数据入口 */
    void OnSessionDestory(int iSessionID);

private:

    /* 依状态获取数据 */
    DataLogic2UIBase* GetBetransSessionData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetLocalConfSessionData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetDialRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetConnectRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetRingRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetTalkRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetFinishRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);
    DataLogic2UIBase* GetParkNotifyRoutineData(const CSessionStateProxy& proxy, CListenInfo* priv, bool bBaseData);

public:
    DataTalking2UI* GetTalkData(const CSessionStateProxy& proxy);
    DataLocalConf* GetConfData(const CSessionStateProxy& proxy);
    DataDial* GetDialData(const CSessionStateProxy& proxy);
    DataRing* GetRingData(const CSessionStateProxy& proxy);
    DataConnect* GetConnectData(const CSessionStateProxy& proxy);
    DataCallFinished* GetFinishData(const CSessionStateProxy& proxy);
    DataParkNotify* GetParkNotifyData(const CSessionStateProxy& proxy);

private:

    template<typename T>
    T& GetDataImpl(int iSessionID);

private:
    MAP_SESSION_DATA    m_mapData2UI;
};

template<typename T>
T& CTalkUIDataManager::GetDataImpl(int iSessionID)
{
    MAP_SESSION_DATA::iterator it = m_mapData2UI.find(iSessionID);
    if (it != m_mapData2UI.end())
    {
        DataLogic2UIBase* pCache = it->second;

        /* 类型转换成功，不需要重新分配 */
        T* pdata2UI = dynamic_cast<T*>(pCache);
        if (pdata2UI != NULL)
        {
            return *pdata2UI;
        }

        delete pCache;
        m_mapData2UI.erase(it);
    }

    T* pdata2UI = new T;
    m_mapData2UI[iSessionID] = pdata2UI;
    return *pdata2UI;
}

#define _TalkUIData    GET_SINGLETON_INSTANCE(CTalkUIDataManager)

