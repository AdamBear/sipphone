#if (defined(IF_SUPPORT_BLUETOOTH ) || defined(IF_SUPPORT_WIFI))
#ifndef _VIEW_MODE_CTRLER_HELPER_H_
#define _VIEW_MODE_CTRLER_HELPER_H_

#include "viewmodectrler.h"
#include "singletonclassdefine.h"

enum CtrlerType
{
    CtrlerNone = -1,
    CtrlerBt = 0,
    CtrlerWifi = 1,
};

struct CtrlerInfo
{
    CtrlerType      eType;
    int             iCount;
    CViewModeControler   *  pCtrler;

    CtrlerInfo()
    {
        eType = CtrlerNone;
        iCount = 0;
        pCtrler = NULL;
    }

    CtrlerInfo & operator = (const CtrlerInfo & ctrlInfo)
    {
        if (this != &ctrlInfo)
        {
            eType = ctrlInfo.eType;
            iCount = ctrlInfo.iCount;
            pCtrler = ctrlInfo.pCtrler;
        }
        return *this;
    }
};

class CViewModeCtrlerHelper
{
    DECLARE_SINGLETON_CLASS(CViewModeCtrlerHelper)

public:
    CViewModeControler * GetCtrler(int iUIType);
    bool ReleaseCtrler(CViewModeControler ** pCtrler);

private:
    bool FindCtrlerInfoByType(CtrlerType eType, CtrlerInfo & ctrlerInfo);
    bool FindCtrlerInfoByCtrler(CViewModeControler * pCtrler, CtrlerInfo & ctrlerInfo);
    void SetCtrlerInfo(CtrlerType eType, CViewModeControler * pCtrler);
    void SetRefrenceCount(CtrlerType eType, bool bIncrease);
private:
    YLVector<CtrlerInfo>        m_vecCtrler;
};

#define _ViewModeCtrlerHelper GET_SINGLETON_INSTANCE(CViewModeCtrlerHelper)

#endif // _VIEW_MODE_CTRLER_HELPER_H_
#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
