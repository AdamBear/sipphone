#ifndef __I_GENBAND_CONTACT_UI_HELPER_H__
#define __I_GENBAND_CONTACT_UI_HELPER_H__

#if IF_FEATURE_GBBOOK

#include "directorycommon.h"

class IGenbandContactUIHelper
{
public:
    IGenbandContactUIHelper() {}
    virtual ~IGenbandContactUIHelper() {}

public:
    virtual void EnterPreciseSearch()                                                           = 0;
    virtual void OnPreciseSearchResult()                                                        = 0;
    virtual void OnEnterPreciseSearch()                                                         = 0;
    virtual void ShowSearchModeOption(int nUIState)                                             = 0;
    virtual bool GetGABPreciseSearchData(yl::string& strKey)                                    = 0;
    virtual void OnEnterManageGroup(int nDirType, int nID)                                      = 0;
    virtual bool GetGenbandContactData(ContactGBBookUnit& itemData)                             = 0;
    virtual bool GetGenbandGroupData(yl::string& strGroupName)                                  = 0;
    virtual void SetInputFocus(const char* pszAttr)                                             = 0;
    virtual void GetGenbandNumber(LIST_ATTRIBUTE& lsAttr, ContactGBBookUnit* pGroup)            = 0;
    virtual void OnChooseAccountResult(int nAccountID)                                          = 0;
};

#endif
#endif //__I_GENBAND_CONTACT_UI_HELPER_H__
