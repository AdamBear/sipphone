#include "explogiclcdmanager.h"

#ifdef _EXP_BLACK_SUPPORT
#include "dsklog/log.h"
#include <ylstl/ylstringalgorithm.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "dsskey/include/dsskey_enumtypes.h"

#include "dsskey/include/moddsskey.h"

#include <account/include/modaccount.h>
#include "talk/fwd_dnd/include/modfwddnd.h"
#include <configiddefine.h>

#include "edk/include/edkmsgdefine.h"
#include "edk/include/modedk.h"

#include "adapterbox/include/modadapterbox.h"

#include "lamp/backlight/include/modbacklight.h"

#include "lamp/powersaving/include/powersavingmsgdefine.h"

#include "explogic.h"

#include <dskthread/mkthread.h>

CEXPLogicLCDManager::CEXPLogicLCDManager()
{

}

CEXPLogicLCDManager::~CEXPLogicLCDManager()
{

}

CEXPLogicLCDManager & CEXPLogicLCDManager::GetInstance()
{
    static CEXPLogicLCDManager intance;
    return intance;
}

bool CEXPLogicLCDManager::OnExpInsert(int nExpIndex, DSSKEY_MODULE_TYPE eExpType)
{


    etl_NotifyApp(false, DSK_MSG_EXT_CHANGE, eExpType, nExpIndex);

    msgBroadpostThreadMsg(DSK_MSG_EXT_CHANGE, eExpType,
                          nExpIndex);//向所有线程广播扩展台拔插的消息

    return true;

}

bool CEXPLogicLCDManager::OnExpRemove(int nExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
    if (nExpIndex < 0)
    {
        return false;
    }

    dsskey_RemoveModule(eExpType, nExpIndex);

    etl_NotifyApp(false, DSK_MSG_EXT_CHANGE, eExpType, nExpIndex);

    msgBroadpostThreadMsg(DSK_MSG_EXT_CHANGE, eExpType,
                          nExpIndex);//向所有线程广播扩展台拔插的消息

    return true;

}

LRESULT CEXPLogicLCDManager::OnEXPEvent(msgObject & msg)
{
    switch (msg.message)
    {
    case EXP_MESSAGE_SWITCH_PAGE:
        {

            ProcessEXPPageEvent(msg.wParam, msg.lParam);

        }
        break;
    default:
        break;
    }
    return TRUE;
}

void CEXPLogicLCDManager::ExpProcessReadyInit()
{
    return ;
}

void CEXPLogicLCDManager::ProcessEXPPageEvent(int nEXPIndex, int nPage)
{
    DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent nEXPIndex [%d], nPage [%d]", nEXPIndex, nPage);

    if (exp_IsTestMode())
    {
        return;
    }

    dsskey_RefressDsskeyUIDuetoSwitchExpPage(nEXPIndex, DSS_KEY_REFRESH_LED);

    DSSKEY_INFO("CEXPLogicManager::ProcessEXPPageEvent:post msg[EXP_MESSAGE_SWITCH_PAGE] to exp drawer,expIndex[%d] page[%d]",
                nEXPIndex, nPage);

    msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_SWITCH_PAGE, nEXPIndex,
                       nPage);
}

void CEXPLogicLCDManager::ProcessEXPKeyEvent(int nDsskeyID, int nStatus)
{
    return;
}

bool CEXPLogicLCDManager::RefreshExp(int nExpIndex, bool bRefreshStatusInfo/* = true*/)
{
    dsskey_RefressDsskeyUIDuetoExpUpdate(nExpIndex, false);

    if (bRefreshStatusInfo)
    {
        RefreshStatusInfo();
    }

    int page = exp_GetCurrentPageByIndex(nExpIndex);

    msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_SWITCH_PAGE, nExpIndex,
                       page);

    // 设置EXP对比度
    // 如果Backlight Time为Always Off，代表不亮灯，不亮扩展台的背光灯
    if (1 == configParser_GetConfigInt(kszBackLightTime))
    {
        return true;
    }

    DSSKEY_INFO("Post msg to exp draw thread:msg[EXP_MESSAGE_SWITCH_PAGE][DEC:%d],expindex[%d],pageindex[%d]",
                EXP_MESSAGE_SWITCH_PAGE, nExpIndex, page);
    int iLevel = configParser_GetConfigInt(kszContrastLevel);

    for (int i = 0; i < GetEXPNum(); ++i)
    {
        SetExpContrast(i, iLevel * 10);
    }

    return true;

}

void CEXPLogicLCDManager::Init()
{
    etl_RegisterMsgHandle(EXP_MESSAGE_SWITCH_PAGE, EXP_MESSAGE_SWITCH_PAGE, ::OnEXPEvent);
}

void CEXPLogicLCDManager::Uninit()
{
    etl_UnregisterMsgHandle(EXP_MESSAGE_SWITCH_PAGE, EXP_MESSAGE_SWITCH_PAGE, ::OnEXPEvent);
}

bool CEXPLogicLCDManager::InitExpData()
{
    DEVICELIB_INFO("exp_GetEXPNum[%d]", GetEXPNum());

    for (int i = 0; i < GetEXPNum(); i++)
    {
        RefreshExp(i);
    }

    return true;
}

bool CEXPLogicLCDManager::GetUseEXPColor() const
{
    return false;
}

yl::string CEXPLogicLCDManager::GetDrawThreadName()
{
    return TA_THREAD_EXP_DRAWER;
}



int CEXPLogicLCDManager::SetExpIcon(int nDsskeyID, int nIcon)
{
    //msgPostMsgToThreadEx(msgGetThreadByName(VP_EXP_THREAD_NAME), EXP_MSG_CTRL, EXT_STATE_ICON, nDsskeyID, sizeof(int) + 1, &nIcon);

    PackIconData(nDsskeyID, nIcon);

    return 0;
}

int CEXPLogicLCDManager::SetExpLabel(int nDsskeyID, const char * pStrLabel)
{
    return 0;
}

int CEXPLogicLCDManager::SetExpKeyDisp(int nDsskeyID, int nIcon, const char * pStrLabel,
                                       int nLedStatus, int nDsskeyBgType, int nDisplayFlag /*=1*/, const char * pStrIcon /*= NULL*/,
                                       int nStatus /*= 0*/, int nAuxiliaryStatus /*= 0*/)
{
    if (pStrLabel == NULL)
    {
        return 1;
    }

    int expIndex = -1;
    int rowIndex = 0;
    int colIndex = 0;
    int pageIndex = 0;

    GetExpInfo(nDsskeyID, expIndex, rowIndex, pageIndex, colIndex);
    DSSKEY_INFO("SetExpKeyDisp expIndex[%d] row[%d] col[%d] page[%d]", expIndex, rowIndex, colIndex,
                pageIndex);
    DSSKEY_INFO("SetExpKeyDisp label[%s] ledStatus[%d] displayflage[%x]", pStrLabel, nLedStatus,
                nDisplayFlag);
    if (expIndex < exp_GetExpCounts(DT_ALL))
    {
        bool bLight = IsExpKeyInCurrentPage(nDsskeyID);

        PackDisplayData(expIndex, pageIndex, rowIndex, colIndex, nIcon, nDisplayFlag, pStrLabel, bLight,
                        nLedStatus);
    }

    return 0;
}

int CEXPLogicLCDManager::SetExpLEDStatus(int nDsskeyID, int nLedStatus)
{

    int expIndex = -1;
    int rowIndex = 0;
    int colIndex = 0;
    int pageIndex = 0;

    GetExpInfo(nDsskeyID, expIndex, rowIndex, pageIndex, colIndex);
    DSSKEY_INFO("CEXPLogicLCDManager set expindex[%d] row[%d] col[%d] led status[%d]", expIndex,
                rowIndex, colIndex, nLedStatus);
    PackLEDData(expIndex, rowIndex, colIndex, nLedStatus);

    return 1;
}

int CEXPLogicLCDManager::SetExpBKLight(int nExpIndex, int nBKLight)
{

    DSSKEY_INFO("CEXPLogicLCDManager set expindex[%d] BKLight[%d]", nExpIndex, nBKLight);
    msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_BACKLIGHT, nExpIndex,
                       nBKLight);
    return 0;
}

int CEXPLogicLCDManager::SetExpContrast(int nExpIndex, int nContrast)
{
    DSSKEY_INFO("CEXPLogicLCDManager set expindex[%d] Contrast[%d]", nExpIndex, nContrast);
    msgPostMsgToThread(msgGetThreadByName(TA_THREAD_EXP_DRAWER), EXP_MESSAGE_SET_CONTRAST, nExpIndex,
                       nContrast);
    return 0;

}

int CEXPLogicLCDManager::SetEXPBackGround(const char * pStrBackGround,
        int nTransparentLevel /*= 0*/, bool bRefresh /*= true*/)
{
    return 0;
}

int CEXPLogicLCDManager::EnterExpScreenSaver(bool bEnter)
{
    return 0;
}

void CEXPLogicLCDManager::PackDisplayData(int nExpIndex, int nPageIndex, int nRowIndex,
        int nColIndex, int nIcon, int nDisplayFlag, const char * szLableBuff, bool bLight, int nLedStatus)
{
    ExpDisplayItemInfo ExpData;

    ExpData.m_nIndex = nExpIndex;
    ExpData.m_nPage = nPageIndex;
    ExpData.m_nRol = nRowIndex;
    ExpData.m_nCol = nColIndex;
    ExpData.icon = nIcon;
    ExpData.flag = nDisplayFlag;

    ExpData.bLight = bLight;
    ExpData.ledStatus = nLedStatus;
    ExpData.nCurrentPage = exp_GetCurrentPageByIndex(nExpIndex);

    if (NULL != szLableBuff)
    {

        strncpy(ExpData.szLabel, szLableBuff, EXP_LABEL_MAX_SIZE - 1);
        ExpData.szLabel[EXP_LABEL_MAX_SIZE - 1] = '\0';
    }

    AddEXPInfo(ExpData);
}

void CEXPLogicLCDManager::PackLEDData(int nExpIndex, int nRowIndex, int nColIndex, int nLedStatus)
{
    ExpDisplayItemInfo ExpData;
    ExpData.m_nIndex = nExpIndex;

    ExpData.m_nRol = nRowIndex;
    ExpData.m_nCol = nColIndex;
    ExpData.bLight = true;
    ExpData.bPaintIcon = false;
    ExpData.bPaintText = false;
    ExpData.ledStatus = nLedStatus;

    AddEXPInfo(ExpData);
}

void CEXPLogicLCDManager::PackIconData(int nDsskeyID, int nIcon)
{

    int nExpIndex = -1;
    int nRowIndex = 0;
    int nColIndex = 0;
    int nPageIndex = 0;

    GetExpInfo(nDsskeyID, nExpIndex, nRowIndex, nPageIndex, nColIndex);

    ExpDisplayItemInfo ExpData;

    ExpData.m_nIndex = nExpIndex;
    ExpData.m_nPage = nPageIndex;
    ExpData.m_nRol = nRowIndex;
    ExpData.m_nCol = nColIndex;
    ExpData.icon = nIcon;

    // 仅仅绘制图标
    ExpData.bPaintIcon = true;
    ExpData.bPaintText = false;

    AddEXPInfo(ExpData);
}
#endif//_EXP_BLACK_SUPPORT
