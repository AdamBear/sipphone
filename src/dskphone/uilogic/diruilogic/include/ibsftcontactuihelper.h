#ifndef __I_BSFT_CONTACT_UI_HELPER_H__
#define __I_BSFT_CONTACT_UI_HELPER_H__

#include "directorycommon.h"


class IBSFTContactUIHelper
{
public:
    IBSFTContactUIHelper() {}
    virtual ~IBSFTContactUIHelper() {}

public:
    virtual bool GetBSFTPersonalData(ContactBroadSoftPersonal & itemData)                        = 0;
    virtual bool GetBSFTPreciseSearchData(int & nGroupId, ContactBroadSoftItemData & itemData)    = 0;
    virtual void SetInputFocus(const char * pszAttr)                                             = 0;
#if DIR_UI_FEATURE_LOAD_DATA_LIST
    virtual bool LoadBSFTPreciseSearchData(int nDataIndex, int nDataCount)                      = 0;
#endif
    virtual void EnterPreciseSearch()                                                           = 0;
    virtual void OnPreciseSearchResult()                                                        = 0;
    virtual void OnEnterPreciseSearch()                                                         = 0;
};

#endif
