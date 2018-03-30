#ifndef _MOD_IMPLEMENT_DSSKEY_PERFORMANCE_H_
#define _MOD_IMPLEMENT_DSSKEY_PERFORMANCE_H_

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "dsskey/include/dsskeystructure.h"

void DP_Init();

void DP_Uninit();

bool DP_ParseDsskeyPerformance(DsskeyDisplayData & displayData, const DssKeyLogicData & logicData);

#endif //_MOD_IMPLEMENT_DSSKEY_PERFORMANCE_H_
