#ifndef HOTELING_GUEST_UNLOCK_CHECK_DELEGATE_H_
#define HOTELING_GUEST_UNLOCK_CHECK_DELEGATE_H_

#include "settinguidatadelegatebase.h"

class CHotelingGuestUnlockCheckDelegate : public CSettinguiDataDelegateBase
{
public:
    CHotelingGuestUnlockCheckDelegate();
    ~CHotelingGuestUnlockCheckDelegate();

    static CSettingUIDelegateBase * CreateHotelingGuestUnlockCheckDelegate();

public:
    virtual bool ProcessMsgCallBack(int nResult, int nID, void * pExtraData = NULL);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
};

#endif //HOTELING_GUEST_UNLOCK_CHECK_DELEGATE_H_
