#ifndef _SETTINGUI_DATA_DELEGATE_BASE_
#define _SETTINGUI_DATA_DELEGATE_BASE_

#include "basesettinguidelegate.h"
#include "settinguilogic/include/settinguicommondata.h"

class CSettinguiDataDelegateBase : public CSettingUIDelegateBase
{
public:
    CSettinguiDataDelegateBase();
    virtual ~CSettinguiDataDelegateBase();

    static CSettingUIDelegateBase * CreateDataDelegate();

public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);

protected:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);
    void SetComboboxValue(const yl::string & strItemId, int nValue);
    int GetComboboxValue(const yl::string & strItemId);
    yl::string GetValue(const yl::string & strItemId);
    void ShowNoteMessgaeBox(const yl::string & strNote, int nId,
                            int nTime = NS_SET::g_SetPopNormalTime);
    bool IsPageOnOffCodeValid(const yl::string & strOn, const yl::string & strOff);
};
#endif
