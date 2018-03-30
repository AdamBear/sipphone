#ifndef __MESSAGELIST_DELEGATE_H__
#define __MESSAGELIST_DELEGATE_H__

#include "basesettinguidelegate.h"

class CMessageListDelegate : public CSettingUIDelegateBase
{
public:
    CMessageListDelegate();
    virtual ~CMessageListDelegate();

    static CSettingUIDelegateBase* CreateStatusDelegate();
    virtual bool PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload = false);

private:
    void SetItemHide(CSettingUIPageDataBase* pPagedata, const yl::string& strItemId);
};

typedef CMessageListDelegate* CMessageListDelegatePtr;

#endif // __MESSAGELIST_DELEGATE_H__
