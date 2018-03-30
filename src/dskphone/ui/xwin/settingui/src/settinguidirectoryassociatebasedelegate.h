#ifndef _DIRECTORY_ASSOCIATE_DELEGATE_H_
#define _DIRECTORY_ASSOCIATE_DELEGATE_H_

#include <settingui/include/settinguidefine.h>

#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguidatadelegatebase.h"

class CDirectoryAssociateBaseDelegate : public CSettinguiDataDelegateBase
{
public:
    CDirectoryAssociateBaseDelegate();
    ~CDirectoryAssociateBaseDelegate();
    static CSettingUIDelegateBase* CreateDirectoryAssociateBaseDelegate();

public:
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey);
    virtual bool IsShowDirectorySoftkey();
    virtual void OnEditValueChange(int nIndex);

protected:
    void UpdateDeleteSoftkey(bool bShowDelete);

};
#endif

#endif //_DIRECTORY_ASSOCIATE_DELEGATE_H_
