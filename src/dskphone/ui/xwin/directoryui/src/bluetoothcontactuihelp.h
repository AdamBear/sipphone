#ifndef __BLUETOOTH_CONTACT_UI_HELPER_H__
#define __BLUETOOTH_CONTACT_UI_HELPER_H__

#ifdef IF_SUPPORT_BLUETOOTH_CONTACT

#include "commonuihelper.h"

class CBluetoothUIHelper : public CCommonUIHelper
{
public:
    CBluetoothUIHelper();
    virtual ~CBluetoothUIHelper();

    static CBluetoothUIHelper* GetUIHelper(CDirUIHelperPtr& pUIHelper);

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption);
    virtual bool GetNormalTitle(yl::string& strTitle, int nGroupId = knInvalidRId);
    virtual bool GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                            CArraySoftKeyBarData& vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

protected:
    virtual bool UpdateListItem(xListViewItem* pListItem, int nId, SmartPtr& pDetail);
};

#endif // #ifdef IF_SUPPORT_BLUETOOTH_CONTACT
#endif // #ifndef __BLUETOOTH_CONTACT_UI_HELPER_H__
