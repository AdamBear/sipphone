#include "modbroadsoft_sca.h"
#include "scamanager.h"
#include "dsskey/include/moddsskey.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "commonunits/modcommonunits.h"

#ifdef IF_FEATURE_BROADSOFT_SCA

// 初始化
void bsSCA_Init()
{
    _BSSCAManager.Init();
}

// 尝试进行SCA状态的显示
BOOL bsSCA_ShowSCAStatus(int iAccoundID)
{
    return _BSSCAManager.ShowSCAStatus(iAccoundID);
}

// 函数描述:   获取指定账号的SCA 呼入数据信息
SCA_Info * bsSCA_GetCallInData(int iAccoundID, int iScaIndex)
{
    return _BSSCAManager.GetCallInData(iAccoundID, iScaIndex);
}

// 是否需要占线
BOOL bsSCA_BeNeedToSeizeLine(int iAccoundID, int iBLAIndex)
{
    BLF_STATUS_CODE eStatus = _BSSCAManager.GetStatusByIndex(iBLAIndex, iAccoundID);

    BOOL bNeed = TRUE;
    switch (eStatus)
    {
    case CN_BLA_STATUS_HELD:
    case CN_BLA_STATUS_BRIDGE_HELD:
    case CN_BLA_STATUS_ACTIVE:
    case CN_BLA_STATUS_BRIDGE_ACITVE:
    case CN_BLA_STATUS_HELD_PRIVATE:
        {
            bNeed = FALSE;
        }
        break;
    default:
        break;
    }

    SCA_INFO("eStatus [%d] Need Seize Line [%d]", eStatus, bNeed);
    return bNeed;
}

// 获取指账号状态的线路序号
int bsSCA_GetFirstIndexByStatus(BLF_STATUS_CODE eState, int iAccount, int iDsskeyIndex)
{
    return _BSSCAManager.GetFirstIndexByStatus(eState, iAccount, iDsskeyIndex);
}

// 更新灯状态
VOID bsSCA_UpdateLightInfo(int iAccoundID/* = -1*/)
{
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        if (iAccoundID != -1
                && iAccoundID != i)
        {
            continue;
        }
        _BSSCAManager.UpdateLightInfo(i);
    }
}

// 获取指定账号首个空闲线路
int bsSCA_GetFirstFreeIndex(int iAccount)
{
    return _BSSCAManager.GetFirstFreeIndex(iAccount);
}

// 获取指定账号响铃线路
int bsSCA_GetAlertingIndex(int iAccount)
{
    return _BSSCAManager.GetAlertingIndex(iAccount);
}

// 获取账号线路的呼出信息
BOOL bsSCA_GetLineIndexCallInfo(int iAccoundID, int iScaIndex, yl::string & strUserName,
                                yl::string & strUri)
{
    return _BSSCAManager.GetLineIndexCallInfo(iAccoundID, iScaIndex, strUserName, strUri);
}

bool bsSCA_GetDsskeySessionByStatus(int iAccount, BLF_STATUS_CODE eState)
{
    return _BSSCAManager.GetDsskeySessionByStatus(iAccount, eState);
}

BLF_STATUS_CODE bsSCA_GetStatusByIndex(int iBlaIndex, int iAccountID)
{
    return _BSSCAManager.GetStatusByIndex(iBlaIndex, iAccountID);
}

int bsSCA_GetBLAIndex(int iDsskeyID)
{
    int iBlaIndex = 0;
    dsskey_GetBLAIndexByDsskey(iBlaIndex, iDsskeyID, dsskey_GetDsskeyLine(iDsskeyID));
    int iCounts = dsskey_CountsDsskeyByAccID(dsskey_GetDsskeyLine(iDsskeyID));
    return _BSSCAManager.GetBLAIndex(dsskey_GetDsskeyLine(iDsskeyID), iBlaIndex, iCounts);
}

// 获取dsskey对应的线路状态
BLF_STATUS_CODE bsSCA_GetStatusByDsskeyID(int iDsskeyID)
{
    return _BSSCAManager.GetStatusByDsskeyID(iDsskeyID);
}

void bsSCA_FreeIndex(int iIndex, int iLine)
{
    _BSSCAManager.SetFreeIndex(iIndex, iLine);
}

ListSCAInfo * bsSCA_GetListByAccount(int iAccountID)
{
    return _BSSCAManager.GetListByAccount(iAccountID);
}

int bsSCA_GetCurrntCallNumByAccount(int iAccountID, int nDsskeyIndex/* = 0*/)
{
    return _BSSCAManager.GetCurrentCallNumByAccount(iAccountID, nDsskeyIndex);
}

BLF_STATUS_CODE bsSCA_GetStatus2UIShow(int iAccountId, int iIndex)
{
    return _BSSCAManager.GetStatus2UIShow(iAccountId, iIndex);
}

yl::string bsSCA_GetBLANum(int iAccountId)
{
    yl::string strNumber = acc_GetBLANum(iAccountId);

    if (strNumber.empty())
    {
        CSCAStatusManager* pScaManager = _BSSCAManager.GetManagerByAccount(iAccountId);
        if (pScaManager != NULL)
        {
            strNumber = pScaManager->GetBlaNumer();
        }
    }

    return strNumber;
}

#else
// 初始化
void bsSCA_Init() {}
// 尝试进行SCA状态的显示
BOOL bsSCA_ShowSCAStatus(int iAccoundID)
{
    return FALSE;
}
// 函数描述:   获取指定账号的SCA 呼入数据信息
SCA_Info * bsSCA_GetCallInData(int iAccoundID, int iScaIndex)
{
    return NULL;
}
// 是否需要占线
BOOL bsSCA_BeNeedToSeizeLine(int iAccoundID, int iBLAIndex)
{
    return FALSE;
}

// 获取指账号状态的线路序号
int bsSCA_GetFirstIndexByStatus(BLF_STATUS_CODE eState, int iAccount)
{
    return -1;
}
// 更新灯状态
VOID bsSCA_UpdateLightInfo(int iAccoundID) {}
// 获取指定账号首个空闲线路
int bsSCA_GetFirstFreeIndex(int iAccount)
{
    return -1;
}

// 获取指定账号响铃线路
int bsSCA_GetAlertingIndex(int iAccount)
{
    return -1;
}
// 获取账号线路的呼出信息
BOOL bsSCA_GetLineIndexCallInfo(int iAccoundID, int iScaIndex, yl::string & strUserName,
                                yl::string & strUri)
{
    return FALSE;
}

BLF_STATUS_CODE bsSCA_GetStatusByDsskeyID(int iDsskeyID)
{
    return CN_BLA_STATUS_UNKOWN
}

ListSCAInfo * bsSCA_GetListByAccount(int iAccountID)
{
    return NULL;
}

BLF_STATUS_CODE bsSCA_GetStatus2UIShow(int iAccountId, int iIndex)
{
    return CN_BLA_STATUS_UNKOWN;
}

#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
