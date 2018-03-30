#ifndef __I_PLCM_UI_HELPER_H__
#define __I_PLCM_UI_HELPER_H__


class IPLCMUIHelper
{
public:
    IPLCMUIHelper() {}
    virtual ~IPLCMUIHelper() {}

public:
    virtual void OnDownLoadGroup(int nGroupID, bool bSuccess)       = 0;
    virtual void OnFinishAllDownLoad(int nGroupID)                  = 0;
};

#endif
