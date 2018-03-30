#ifndef __MOD_NEGO_H__
#define __MOD_NEGO_H__
#ifdef IF_SUPPORT_VIDEO

#include "negohelper.h"

bool modNego_AdjustResolution(NegoInfo & stNegoInfo, VideoNegoInfo & pVideoInfo, NegoAction eAction,
                              int & iRes);

bool modNego_Renegotiation(int iAccountID, int iCallId, NegoAction eAction, int iRes, int iType,
                           bool bNegoNow);

void modNego_RegisterNegoCallBack(FunNegoHelperCallBack fun);

bool modNego_ExistAction(NegoAction eAction);

bool modNego_ExistNego(int iCallId);

bool modNego_StartFirstNego(int iCallId, int iAccountId);

bool modNego_ClearNegoInfo(int iCallId);
#endif //#ifdef IF_SUPPORT_VIDEO

#endif // __MOD_NEGO_H__
