#ifndef _SETTINGUI_SOFTKEY_LABEL_LIST_DELEGATE_H_
#define _SETTINGUI_SOFTKEY_LABEL_LIST_DELEGATE_H_

#include "settinguidefine.h"
#if IF_SUPPORT_SOFTKEY_LABEL
#include "basesettinguidelegate.h"

class CSettinguiSoftkeyLabelListDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiSoftkeyLabelListDelegate();
    virtual ~CSettinguiSoftkeyLabelListDelegate();

    static CSettingUIDelegateBase * CreateSoftkeyLabelListDelegate();
public:
    virtual bool OnAction(int eActionType, int iDataIndex);
};

#endif

#endif //_SETTINGUI_SOFTKEY_LABEL_LIST_DELEGATE_H_
