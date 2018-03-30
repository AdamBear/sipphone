#ifndef _EXP_REFRESHER_H
#define _EXP_REFRESHER_H

#include "expdefine.h"
#include "mypainter.h"
#include "expusb_api.h"

class EXPRefresher
{

public:
    EXPRefresher();
    virtual ~EXPRefresher();

public:
    virtual void Init();
    virtual void Uninit();

public:
    virtual void SetMmapInfo(int nFileHandle, char * pFBMmap, unsigned int nFBMmapLength);

    virtual void SyncImageToMem(CMyImage & imgCanvas);

    virtual void RefreshButton(int nIndex, int nPage, int nRow, int nCol,
                               bool bRefreshIcon, bool bRefreshText, bool bRefreshFull = false);

    virtual void RefreshScreen(int nIndex, int iCanvasIndex = 1, int iFlushRightNow = 1);

    virtual void RefreshLight(int nIndex, int nRow, int nCol, int nLightColor);

    virtual void RefreshBackLight(int nIndex, int nBackLightLevel);

    virtual void RefreshLogo(int nIndex);

    virtual void RefreshMemPage(int nIndex, int nMemPage);

protected:
    //灯映射转换
    virtual void LightAdapt(expusb_led & objLedInfo, int nLightColor);

protected:
    int m_nFileHandle;
    char * m_pFBMmap;
    unsigned int m_nFBMmapLength;
};

#endif //_EXP_REFRESHER_H
