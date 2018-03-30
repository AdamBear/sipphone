#ifndef __I_MTSW_UI_HELPER_H__
#define __I_MTSW_UI_HELPER_H__

#if IF_FEATURE_METASWITCH_DIRECTORY

#include "contacts/directory/include/commondata.h"

class IMTSWUIHelper
{
public:
    IMTSWUIHelper() {}
    virtual ~IMTSWUIHelper() {}

public:
    virtual bool GetMTSWContactListData(ContactMetaSwitchContactList & itemData) = 0;
    virtual void SetInputFocus(const char * pszAttr) = 0;
    virtual bool IsAddition() = 0;
    virtual void EnterMTSWLoginPage() = 0;
};
#endif //IF_FEATURE_METASWITCH_DIRECTORY

#if IF_FEATURE_METASWITCH_CALLLOG
class IMTSWCallLogUIHelper
{
public:
    IMTSWCallLogUIHelper() {}
    virtual ~IMTSWCallLogUIHelper() {}

public:
    virtual void EnterMTSWLoginPage() = 0;
};
#endif //IF_FEATURE_METASWITCH_CALLLOG

#endif //__I_MTSW_UI_HELPER_H__
