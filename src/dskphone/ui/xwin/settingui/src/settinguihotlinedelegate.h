#ifndef _SETTINGUI_HOT_LINE_DELEGATE_H_
#define _SETTINGUI_HOT_LINE_DELEGATE_H_

#include <settingui/include/settinguidefine.h>

#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguidirectoryassociatebasedelegate.h"

class CSettingUIHotLineDelegate : public CDirectoryAssociateBaseDelegate
{
public:
    CSettingUIHotLineDelegate();
    ~CSettingUIHotLineDelegate();

    static CSettingUIDelegateBase* CreateSettingUIHotLineDelegate();
public:
    static void UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber);
protected:
    void UpdateHotlineData(const yl::string& strContactNumber);
    virtual bool IsShowDirectorySoftkey();

    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
};
#endif

#endif //_SETTINGUI_HOT_LINE_DELEGATE_H_
