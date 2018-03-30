#include "moddsskeyperformance.h"
#include "implperformanceurl.h"
#include "implperformancesimpletype.h"
#include "implperformancesession.h"
#include "implperformancepaging.h"
#include "implperformanceline.h"
#include "implperformanceintercom.h"
#include "implperformancegroup.h"
#include "implperformancecallpark.h"
#include "implperformanceblf.h"
#include "dsskeyperformancemanager.h"
#include "implperformanceucfavorite.h"

typedef YLVector<ImplDsskeyPerformanceBase *> vec_ImplPerf;

static vec_ImplPerf s_vecImplPerf;

/////////////for Dsskey Log Type String////////////////
void AddPerformance(vec_ImplPerf & vecPerf, ImplDsskeyPerformanceBase * pPerf)
{
    if (NULL != pPerf)
    {
        vecPerf.push_back(pPerf);
    }
}

void DP_Init()
{
    _DsskeyPerfManager.Init();

    s_vecImplPerf.clear();

    // 较复杂的类型 13
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBase(DT_NA));
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceURL);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBLFBase(DT_BLF));
#ifdef IF_FEATURE_GENBAND
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBLFBase(DT_BLFGROUP));
#endif
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBLFBase(DT_BLFLIST));
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceCallPark);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceLocalGroup);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceXMLGroup);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBrsfGroup);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceIntercom);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceLine);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePaging);

    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceSpeedDial);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePickup);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceGroupPickup);

    // 简单类型 33
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceVoiceMail);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceDTMF);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceXmlBrowser);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceLDAP);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceFavorite);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceConference);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceTransfer);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceHold);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceRedial);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceCallReturn);

    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceSMS);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceGroupListen);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePublicHold);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePrivateHold);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceZeroTouch);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMeetMe);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceACDTrace);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceDispcode);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceEscalate);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceKeypadLock);

    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceRetrievePark);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceHotDesk);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceNetworkFavorite);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBuddies);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMyStatus);

    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePrefix);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceForward);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceDND);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceRecord(DT_RECORD));
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceRecord(DT_URLRECORD));
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceRecord(DT_VOICERECORD));
#endif
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceACD);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceHoteling);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceUCFavorite);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePagingList);
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceEDK);
#ifdef IF_BT_SUPPORT_PHONE
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceBTLINE);
#endif
#if IF_BUG_PLCMDIR
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformancePlcm);
#endif

#if IF_BUG_31595
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceXmlBrowserInfo);
#endif
#if IF_BUG_14856
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMissedCall);
#endif
#ifdef IF_FEATURE_GETBALANCE
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceGETBALANCE);
#endif

#if IF_FEATURE_METASWITCH
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMTSWLogout);
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMessageList);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMTSWDir);
#endif
#if IF_FEATURE_METASWITCH_CALLPARK
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceMTSWCallPark);
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    AddPerformance(s_vecImplPerf, new ImplDsskeyPerformanceHistory);
#endif
}

void DP_Uninit()
{
    vec_ImplPerf::iterator it = s_vecImplPerf.begin();
    vec_ImplPerf::iterator itEnd = s_vecImplPerf.end();
    for (; it != itEnd; ++it)
    {
        if (*it != NULL)
        {
            delete (*it);
            *it = NULL;
        }
    }
    s_vecImplPerf.clear();
}

bool DP_ParseDsskeyPerformance(DsskeyDisplayData & displayData, const DssKeyLogicData & logicData)
{
    bool bRet = false;
    // 数据有效性验证
    if (logicData.eDKType < DT_NONE
            || logicData.eDKType > DT_LAST)
    {
        DSSKEYUI_WARN("ParseLogicData Dsskey Type error");
    }
    else
    {
        bRet = _DsskeyPerfManager.ParseDisplayData(displayData, logicData);
    }

    return bRet;
}

// void DP_AddImplPerformance( ImplDsskeyPerformanceBase * pPerf )
// {
//  if (NULL == pPerf)
//  {
//      DSSKEYUI_WARN("You cann't Add a Null ImplDsskeyPerformanceBase!");
//  }
// }
