#ifndef __SETTINGUI_BASE_ACCOUNT_PAGE_DATA_H__
#define __SETTINGUI_BASE_ACCOUNT_PAGE_DATA_H__

#include "settinguibasepagedata.h"

enum
{
    // 第1层子类，使用3、4位
    SETTING_PAGE_DATA_TYPE_ACCOUNT      = 0x00010000,
};

class CSettingUIAccountPageDataBase : public CSettingUIPageDataBase
{
public:
    CSettingUIAccountPageDataBase();
    virtual ~CSettingUIAccountPageDataBase();

    virtual void Reset();

public:
    static int GetAccountPageDataType();
    static CSettingUIAccountPageDataBase * GetAccountPageData(CSettingUIPageDataBase * pPageData);

    static CSettingUIPageDataBase * CreateAccountPageData();

public:
    int GetItemLineIDByDataIndex(int nDataIndex);

public:
    int GetLineID();
    void SetLineID(int iLineID);

protected:
    int                                 m_iLineID;
};

typedef  CSettingUIAccountPageDataBase   *  CSettingUIAccountPageDataBasePtr;

#endif // __SETTINGUI_BASE_ACCOUNT_PAGE_DATA_H__

