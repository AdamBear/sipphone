#ifdef IF_SUPPORT_VIDEO

#include "modnego.h"

bool modNego_AdjustResolution(NegoInfo & stNegoInfo, VideoNegoInfo & pVideoInfo, NegoAction eAction,
                              int & iRes)
{
    if (_Negohelper.CaculateCurrentAbility(stNegoInfo.iCallId, pVideoInfo, eAction, iRes) == NEGO_WAIT)
    {
        if (stNegoInfo.bSupportRenego)
        {
            // 根据计算的目标分辨率发起重协商
            _Negohelper.Renegotiation(stNegoInfo.iAccountId, stNegoInfo.iCallId, eAction, iRes, 3,
                                      stNegoInfo.bNegoNow);
            return true;
        }

        TALK_INFO("Not Support Negotiation");
        return true;
    }
    else
    {
        // 如果不需要协商，则要之前等待resume生效的任务失效，要删除
        _Negohelper.DeleteCallNegoInfo(stNegoInfo.iCallId, DLT_WAIT);
    }

    // 大于这个值说明要协商对方发送的分辨率（分辨率提升）
    if (iRes > CAL_RES_SEND_1080P && stNegoInfo.bSupportRenego)
    {
        // 根据计算的目标分辨率发起重协商,这个重协商外部不需要等待
        _Negohelper.Renegotiation(stNegoInfo.iAccountId, stNegoInfo.iCallId, eAction, iRes, 3,
                                  stNegoInfo.bNegoNow);
    }

    return false;
}

bool modNego_Renegotiation(int iAccountID, int iCallId, NegoAction eAction, int iRes, int iType,
                           bool bNegoNow)
{
    return _Negohelper.Renegotiation(iAccountID, iCallId, eAction, iRes, iType, bNegoNow);
}

void modNego_RegisterNegoCallBack(FunNegoHelperCallBack fun)
{
    _Negohelper.RegisterCallBack(fun);
}

bool modNego_ExistAction(NegoAction eAction)
{
    return _Negohelper.ExistAction(eAction);
}

bool modNego_ExistNego(int iCallId)
{
    return _Negohelper.ExistNego(iCallId);
}

bool modNego_StartFirstNego(int iCallId, int iAccountId)
{
    return _Negohelper.StartFirstNego(iCallId, iAccountId);
}

bool modNego_ClearNegoInfo(int iCallId)
{
    return _Negohelper.DeleteCallNegoInfo(iCallId, DLT_ALL);
}
#endif //#ifdef IF_SUPPORT_VIDEO

