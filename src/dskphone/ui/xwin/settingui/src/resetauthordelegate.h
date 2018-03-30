#ifndef __SETTINGUI_RESET_AUTHOR_DELEGATE_H__
#define __SETTINGUI_RESET_AUTHOR_DELEGATE_H__

#include "settinguidatadelegatebase.h"

class CResetAuthorDelegate : public CSettinguiDataDelegateBase
{
public:
    CResetAuthorDelegate();
    virtual ~CResetAuthorDelegate();

    static CSettingUIDelegateBase * CreateResetAuthorDelegate();

    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnAction(int eActionType, int iDataIndex);
    // 打开界面附加操作
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
};

typedef CResetAuthorDelegate * CResetAuthorDelegatePtr;

#endif // __SETTINGUI_RESET_AUTHOR_DELEGATE_H__
