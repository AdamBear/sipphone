#include "explogicmanager.h"

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
#include "dsklog/log.h"
#include <ylstl/ylstringalgorithm.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "dsskey/include/dsskey_enumtypes.h"

#include "dsskey/include/moddsskey.h"

#include <account/include/modaccount.h>
#include "dsskey/src/dsskeycommonfunc.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include <configiddefine.h>

#include "edk/include/edkmsgdefine.h"
#include "edk/include/modedk.h"

#include "adapterbox/include/modadapterbox.h"

#include "lamp/backlight/include/modbacklight.h"

#include "lamp/powersaving/include/powersavingmsgdefine.h"
namespace
{
static const char szTransTypeLowTrans[] = "0%";
static const char szTransType20Percent[] = "20%";
static const char szTransType40Percent[] = "40%";
static const char szTransType60Percent[] = "60%";
static const char szTransType80Percent[] = "80%";
static const char szTransTypeHeightTrans[] = "100%";

#define EXP_MSG_SEND_COUNT  3
}

CEXPLogicManager::CEXPLogicManager()
    : m_nSendDelayCount(0)
{
    m_listExpInfo.m_nCount = 0;
}

CEXPLogicManager::~CEXPLogicManager()
{

}

int CEXPLogicManager::GetEXPNum() const
{
    return exp_GetExpCounts(DT_EXP_20)
           + exp_GetExpCounts(DT_EXP_38)
           + exp_GetExpCounts(DT_EXP_39)
           + exp_GetExpCounts(DT_EXP_40)
           + exp_GetExpCounts(DT_EXP_50)
           + exp_GetExpCounts(DT_EXP_CM69);
}


DEVICE_TYPE_DEF CEXPLogicManager::GetEXPType() const
{
    return (DEVICE_TYPE_DEF)exp_GetExpTypeByIndex(0);
}

DSSKEY_MODULE_TYPE CEXPLogicManager::EXPManager_GetEXPModuleType()
{
    DSSKEY_MODULE_TYPE moduleType = DMT_NON;
    DEVICE_TYPE_DEF eType = (DEVICE_TYPE_DEF)exp_GetExpTypeByIndex(0);
    switch (eType)
    {
    case DT_EXP_38:
        moduleType = DMT_EXP38_MODULE;
        break;
    case DT_EXP_39:
        moduleType = DMT_EXP39_MODULE;
        break;
    case DT_EXP_40:
        moduleType = DMT_EXP40_MODULE;
        break;
    case DT_EXP_50:
        moduleType = DMT_EXP50_MODULE;
        break;
    case DT_EXP_CM69:
        moduleType = DMT_EXPCM69_MODULE;
        break;
    case DT_EXP_20:
        moduleType = DMT_EXP20_MODULE;
        break;
    default:
        break;
    }
    return moduleType;
}

bool CEXPLogicManager::IsEXPKey(int nDssKeyID)
{
    DSSKEY_INFO("exptype [%d] module type [%d]", GetEXPType(), GetModuleType(nDssKeyID));
    return ((DSSKEY_MODULE_TYPE)GetEXPType()) == (GetModuleType(nDssKeyID));

}

int CEXPLogicManager::GetMaxExpNum()
{
    return MAX_EXP_NUM;
}

int CEXPLogicManager::SetExpContrast(int nExpIndex, int nContrast)
{
    return 1;
}

int CEXPLogicManager::SetEXPBackGround(const char * pStrBackGround, int nTransparentLevel/* = 0*/,
                                       bool bRefresh/* = true*/)
{
    return 1;
}

int CEXPLogicManager::EnterExpScreenSaver(bool bEnter)
{
    return 1;
}

int CEXPLogicManager::RefreshStatusInfo()
{
    // 刷新DND状态
    dsskey_UpdateDNDStatus(dnd_IsEnable(acc_GetDefaultAccount()));
    // 刷新FWD状态
    YLList<int> dsskeys;
    dsskey_GetDsskeysByType(dsskeys, DT_FWD);
    for (YLList<int>::iterator it = dsskeys.begin(); it != dsskeys.end(); ++it)
    {
        // 获取DSSKey Value
        yl::string strValue = dsskey_GetDsskeyValue(*it);
        DSSKEY_INFO("Update FWD dsskey: DsskeyID=%d value=%s", *it, strValue.c_str());
        // 获取当前FWD状态信息
        yl::string strTarget;
        int iAccountID = FWDDND_ACCOUNT_GLOBAL;
        if (fwd_GetMode() == FWDDND_MODE_ACCOUNT)
        {
            iAccountID = acc_GetDefaultAccount();
        }
        bool bEnable = fwd_GetInfo(iAccountID, FWD_TYPE_ALWAYS, &strTarget);

        DSSKEY_INFO("FWD info for dsskey: account=%d enable=%d target=%s",
                    iAccountID, bEnable, strTarget.c_str());
        // 判断是否需要亮灯
        if ((!strValue.empty()) && strValue != strTarget)
        {
            // DSSKey Value非空且与Target不等：不亮灯
            dsskey_UpdateFWDStatus(*it, false);
        }
        else
        {
            // 其他情况：根据全局/默认账号Always FWD是否启用决定是否亮灯
            dsskey_UpdateFWDStatus(*it, bEnable || fwd_GetInfo(iAccountID, FWD_TYPE_BUSY)
                                   || fwd_GetInfo(iAccountID, FWD_TYPE_NO_ANSWER));
        }
    }
    return 0;
}

void CEXPLogicManager::Exp39Switch(int iExpIndex)
{
    int iPage = exp_GetCurrentPageByIndex(iExpIndex);

    int iMaxPageNum = configParser_GetConfigInt(kszExp39PageNum);

    if (iMaxPageNum <= 0)
    {
        iMaxPageNum = 2;
    }

    int iNewPage = ((iPage + 1) % iMaxPageNum);

    exp_SetCurrentPageByIndex(iExpIndex, iNewPage);

    DSSKEY_INFO("EXP 39 index[%d] Switch to [%d]", iExpIndex, iNewPage);

    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), EXP_MESSAGE_SWITCH_PAGE, iExpIndex,
                       iNewPage);
}

bool CEXPLogicManager::IsExpKeyInCurrentPage(int nDsskeyID)
{
    int iExpIndex = dsskey_GetModuleIndex(nDsskeyID);
    int iKeyIndex = dsskey_GetKeyIndex(nDsskeyID);
    int iPageIndex = 0;
    DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(iExpIndex);
    if (DT_EXP_50 == eType)
    {
        if (iKeyIndex < EXP_PAGE_SIZE)
        {
            iPageIndex = 0;
        }
        else if (iKeyIndex < EXP_PAGE_SIZE + EXP_PAGE_SIZE)
        {
            iPageIndex = 1;
        }
        else
        {
            iPageIndex = 2;
        }
    }
    else if (DT_EXP_38 != eType)
    {
        if (iKeyIndex < EXP_PAGE_SIZE)
        {
            iPageIndex = 0;
        }
        else
        {
            iPageIndex = 1;
        }
    }
    if (exp_GetCurrentPageByIndex(iExpIndex) == iPageIndex)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CEXPLogicManager::GetExpInfo(int dsskeyID, int & expIndex, int & rowIndex, int & pageIndex,
                                  int & colIndex)
{
    expIndex = dsskey_GetModuleIndex(dsskeyID);
    int keyIndex = dsskey_GetKeyIndex(dsskeyID);
    if (DT_EXP_50 == exp_GetExpTypeByIndex(expIndex))
    {
        if (keyIndex < EXP_PAGE_SIZE)
        {
            rowIndex = keyIndex / 2;
            pageIndex = 0;
        }
        else if (keyIndex < EXP_PAGE_SIZE + EXP_PAGE_SIZE)
        {
            rowIndex = (keyIndex - EXP_PAGE_SIZE) / 2;
            pageIndex = 1;
        }
        else
        {
            rowIndex = (keyIndex - EXP_PAGE_SIZE - EXP_PAGE_SIZE) / 2;
            pageIndex = 2;
        }
        colIndex = keyIndex % 2;
    }
    else if (DT_EXP_38 != exp_GetExpTypeByIndex(expIndex))
    {
        if (keyIndex < EXP_PAGE_SIZE)
        {
            rowIndex = keyIndex / 2;
            pageIndex = 0;
        }
        else
        {
            rowIndex = (keyIndex - EXP_PAGE_SIZE) / 2;
            pageIndex = 1;
        }
        colIndex = keyIndex % 2;
    }
    else
    {
        pageIndex = 0;
        if (keyIndex < EXP38_PAGE_SIZE)
        {
            rowIndex = keyIndex / 2;
            colIndex = keyIndex % 2;
        }
        else
        {
            rowIndex = (keyIndex - EXP38_PAGE_SIZE) / 2;
            colIndex = 2 + (keyIndex - EXP38_PAGE_SIZE) % 2;
        }
    }
}

void CEXPLogicManager::AddEXPInfo(const ExpDisplayItemInfo & ExpData)
{
    //先判断是否已经满了
    if (EXP_SEND_LIST_MAX_COUNT == m_listExpInfo.m_nCount)
    {
        SendEXPInfo();
    }

    m_listExpInfo.listExpInfo[m_listExpInfo.m_nCount] = ExpData;
    m_listExpInfo.m_nCount++;
    if (!m_objExpInfoSendTimer.IsTimerRuning())
    {
        SetExpInfoSendTimer();
        ++m_nSendDelayCount;
        if (m_nSendDelayCount > EXP_MSG_SEND_COUNT)
        {
            SendEXPInfo();
        }
    }
}

void CEXPLogicManager::SendEXPInfo()
{

    m_nSendDelayCount = 0;

    //每次发送后，可以重置一下计时器
    SetExpInfoSendTimer();

    int nCount = m_listExpInfo.m_nCount;
    if (nCount > 0)
    {
        ExpDisplayItemInfo * listInfo = new ExpDisplayItemInfo[nCount];
        for (int i = 0; i < nCount; ++i)
        {
            listInfo[i] = m_listExpInfo.listExpInfo[i];
        }

        DSSKEY_INFO("CEXPLogicManager::SendEXPInfo() pos msg EXP_MESSAGE_UPDATE_KEY to drawer[%s]  size of extra[%d]",
                    GetDrawThreadName().c_str(), sizeof(ExpDisplayItemInfo) * nCount + 1);
        BOOL bRet = msgPostMsgToThreadEx(
                        msgGetThreadByName(GetDrawThreadName().c_str())
                        , EXP_MESSAGE_UPDATE_KEY
                        , exp_GetExpCounts()
                        , nCount
                        , sizeof(ExpDisplayItemInfo) * nCount + 1
                        , listInfo);

        delete[] listInfo;

        m_listExpInfo.m_nCount = 0;
    }
}


void CEXPLogicManager::SetExpInfoSendTimer()
{
    DestroyExpInfoSendTimer();
    m_objExpInfoSendTimer.SetTimer(EXP_INFO_SEND_TIME, NULL, MK_MSG_ENTRY(this,
                                   CEXPLogicManager::OnExpInfoSendTimerOut));
}

void CEXPLogicManager::DestroyExpInfoSendTimer()
{
    if (m_objExpInfoSendTimer.IsTimerRuning())
    {
        m_objExpInfoSendTimer.KillTimer();
    }
}

BOOL CEXPLogicManager::OnExpInfoSendTimerOut(msgObject & objMessage)
{
    if (m_listExpInfo.m_nCount > 0)
    {
        SendEXPInfo();
    }
    else
    {
        DestroyExpInfoSendTimer();
    }

    return 0;
}
#endif