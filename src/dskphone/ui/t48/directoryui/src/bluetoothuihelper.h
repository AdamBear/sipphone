#ifndef __BLUETOOTH_UI_HELPER_H__
#define __BLUETOOTH_UI_HELPER_H__

#include "commonuihelper.h"

class CBluetoothUIHelper : public CCommonUIHelper
{
public:
    CBluetoothUIHelper();
    virtual ~CBluetoothUIHelper();

    static CBluetoothUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual bool FillDetail(int nId, SmartPtr & pDetail, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnGroupChange();
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);

    virtual bool FillListItem(int nId, int nItemIndex, int nDataIndex, int nDataCount,
                              SmartPtr & pDetail);

    void CheckSettingContactTitle();

    void InitSettingTitle();
};

#endif
