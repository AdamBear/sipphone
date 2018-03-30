#ifndef __I_DIR_UI_HELPER__
#define __I_DIR_UI_HELPER__

#include <ylsmartptr.h>
#include "directorycommon.h"


class IDirUIHelper
{
public:
    IDirUIHelper() {}
    virtual ~IDirUIHelper() {}

public:
    virtual const yl::string & GetSearchKey()                                            = 0;

    virtual void RefreshUI(int nType, int nParam)                                       = 0;
    virtual void RefreshUIAfterDeleted(bool bBackUI = false)                            = 0;
#if DIR_UI_FEATURE_LOAD_DATA_LIST
    virtual bool AdjustListData(int nTotalCount, int & nDataIndex, int & nDataCount)      = 0;
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
    virtual bool GetSelectContactId(VEC_CONTACT_ID & vecContactId)                       = 0;
    virtual bool IsSelectAllContact()                                                   = 0;
    virtual void OnBatchCopyContactSuccess()                                            = 0;
#endif
};

typedef YLSmartPtr<IDirUIHelper> IDirUIHelperPtr;

#endif
