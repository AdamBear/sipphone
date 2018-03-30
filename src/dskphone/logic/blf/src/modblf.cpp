#include "blf_inc.h"
#include "modblf.h"
#include "blflistmsgdefine.h"
#include "blflistmanager.h"
#include "callpickupmanager.h"
#include "blfmanager.h"
#include "customdatamanager.h"

void blf_Init()
{
    g_objBLFListManager.Init();

    g_objBLFManager.Init();

    g_objBLFCustomDataManager.LoadFromConfigFiles();
}

bool blf_GetPickupMenuList(YLList<yl::string> & listMenu)
{
    return g_objPickupManager.GetMenuList(listMenu);
}

#if IF_FEATURE_SHOW_BLF_CALLID
bool blf_GetBLFMenuList(int nDsskeyId, YLList<yl::string>& listMenu, YLList<int>& listStatus)
{
    if (dsskey_GetDsskeyType(nDsskeyId) == DT_BLF)
    {
        return g_objBLFManager.GetMenuList(nDsskeyId, listMenu, listStatus);
    }
    else if (dsskey_GetDsskeyType(nDsskeyId) == DT_BLFLIST)
    {
        return g_objBLFListManager.GetMenuList(nDsskeyId, listMenu, listStatus);
    }
    return false;
}

bool blf_ShowBLFMenu(int nDsskeyId)
{
    if (dsskey_GetDsskeyType(nDsskeyId) == DT_BLF)
    {
        return g_objBLFManager.ShowBLFMenu(nDsskeyId);
    }
    else if (dsskey_GetDsskeyType(nDsskeyId) == DT_BLFLIST)
    {
        return g_objBLFListManager.ShowBLFMenu(nDsskeyId);
    }
    return false;
}
#endif

// 获取指定序号对应的Pickup信息
int blf_GetPickupDsskeyIdByIndex(int iIndex)
{
    return g_objPickupManager.GetPickupDsskeyIdByIndex(iIndex);
}

int blf_GetDialogIndexIdByIndex(int iIndex)
{
    return g_objPickupManager.GetPickupDialogIndexByIndex(iIndex);
}


void blf_CancelPickupMenuShow()
{
    g_objPickupManager.CancelPickupMenuShow();
}

/*****************************************************************************/

CBLFBaseManager * GetInstaceByType(eBLFMangaerType eType)
{
    if (eType == BLFLIST_Manager)
    {
        return &g_objBLFListManager;
    }
    else
    {
        return &g_objBLFManager;
    }
}

void blf_GetPickUpShowContentByKey(YLList<yl::string> & listMenuData, YLVector<int> & listDsskeyID,
                                   const yl::string & strKey, eBLFMangaerType eType)
{
    CBLFBaseManager * pManager = GetInstaceByType(eType);

    if (pManager != NULL)
    {
        pManager->GetPickUpShowContentByKey(listMenuData, listDsskeyID, strKey);
    }
}

bool blf_GetDialogInfoByDsskeyId(DialgInfo & refData, int nDsskeyId, eBLFMangaerType eType,
                                 int index/* = 0*/)
{
    CBLFBaseManager * pManager = GetInstaceByType(eType);

    if (pManager != NULL)
    {
        return pManager->GetDialogInfoByDsskeyId(refData, nDsskeyId, index);
    }

    return false;
}

bool blf_IsSupportAutopPickUpByDsskey(int nDsskeyId, int index /* = 0 */)
{
    DialgInfo sDialogInfo;
    g_objBLFManager.GetDialogInfoByDsskeyId(sDialogInfo, nDsskeyId, index);

    return sDialogInfo.m_bSipPickUp;
}

bool blf_GetBlfListData(YLVector<AccountMoitorData> & refVector)
{
    return g_objBLFListManager.GetBlfListData(refVector);
}

void blf_SetDsskeyId(int iDsskeyId, int iAccountId, const yl::string & strKey)
{
    return g_objBLFListManager.SetDsskeyId(iDsskeyId, iAccountId, strKey);
}

eBLFOperateType blf_GetCustomOperationType(Dsskey_Status eDssStatus, SESSION_STATE eSessState)
{
    return g_objBLFCustomDataManager.GetOperationType(eDssStatus, eSessState);
}

yl::string blf_GetCustomOperationValue(Dsskey_Status eDssStatus, SESSION_STATE eSessState)
{
    return g_objBLFCustomDataManager.GetOperationValue(eDssStatus, eSessState);
}

void blf_RefreshDsskey(YLList<int> & lstFlushDsskey)
{
    CBLFBaseManager::FlushDsskeyUI(lstFlushDsskey);
}

bool blf_RefreshDsskey(const int iDsskeyId, DsskeyExpRefresh dsskeyRefresh/* = DSS_KEY_REFRESH_ALL*/)
{
    return CBLFBaseManager::FlushDsskeyUI(iDsskeyId, dsskeyRefresh);
}

void blf_UnBindDsskeyId(int iDsskeyId, int iAccountId)
{
    return g_objBLFListManager.UnBindDsskeyId(iDsskeyId, iAccountId);
}

void blf_PlayVoice()
{
    g_objPickupManager.BlfPlayVoice();
}


void blf_StopVoice()
{
    g_objPickupManager.BlfStopVoice();
}

#if IF_BUG_32812
bool blf_GetBLFListMonitorList(const int iLineID, YLList<yl::string> & listMonitor)
{
    return g_objBLFListManager.GetBLFListMonitorList(iLineID, listMonitor);
}

bool blf_IsAccountHaveBLFListData(const int iLineID)
{
    return g_objBLFListManager.IsAccountHaveBLFListData(iLineID);
}

bool blf_GetBLFListAutoSetDataBykey(const int iDsskeyId, autoset_dsskeydata_t & tAutoSetDssKeyData)
{
    return g_objBLFListManager.GetBLFAutoSetDataBykey(iDsskeyId, tAutoSetDssKeyData);
}
#endif

void blf_ProcessTalkFoucsChange()
{
    g_objBLFManager.ProcessTalkFoucsChange();
}

bool blf_IsBlfIdleRing()
{
    return g_objPickupManager.IsBlfIdleRing();
}


bool blf_StopBlfIdleRing()
{
    return g_objPickupManager.StopBlfIdleRing();
}
