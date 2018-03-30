#ifndef __BSSCA_BLFPARSE_H__
#define __BSSCA_BLFPARSE_H__

#ifdef IF_FEATURE_BROADSOFT_SCA

#include <ETLLib.hpp>

BLF_STATUS_CODE MapBLFStatus(LPCSTR lpStatus, LPCSTR lpDirect, bool bRender = true);
void TraceBlockData(LPCSTR lpBlock, int iLenth);

class CSCAStatusManager;

class CBLFParse
{
public:
    CBLFParse();

    bool Parse(LPCSTR lpNotify, int iLenth, CSCAStatusManager * pCallBack);

private:
    int                     m_iVersion;
};
#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
#endif // __BSSCA_BLFPARSE_H__
