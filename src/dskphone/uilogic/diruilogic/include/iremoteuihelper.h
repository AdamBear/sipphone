#ifndef __I_REMOTE_UI_HELPER_H__
#define __I_REMOTE_UI_HELPER_H__


class IRemoteUIHelper
{
public:
    IRemoteUIHelper() {}
    virtual ~IRemoteUIHelper() {}

public:
    virtual void OnFinishDownLoad(int nGroupID, bool bSuccess)      = 0;
    virtual void OnFinishAllDownLoad(int nGroupID)                  = 0;
};

#endif
