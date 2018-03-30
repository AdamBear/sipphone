#ifndef __SETTINGUI_BASE_ACCOUNT_ITEM_DATA_H__
#define __SETTINGUI_BASE_ACCOUNT_ITEM_DATA_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"

#include "settinguicommondata.h"
#include "settinguibaseitemdata.h"

enum
{
    // 列表项类型(每个继承类使用一个字节来区分)
    SETTING_ITEM_TYPE_ACCOUNT_BASE      = 0x00070000,
    SETTING_ITEM_TYPE_ACCOUNT_MARK      = 0x0000FF00,
};

class CSettingUIAccountItemDataBase : public CSettingUIItemDataBase
{
public:
    CSettingUIAccountItemDataBase(int eItemType);
    virtual ~CSettingUIAccountItemDataBase();

    virtual void Reset();

    static int GetBaseAccountDataItemType();
    static CSettingUIAccountItemDataBase * GetBaseAccountDataItem(CSettingUIItemDataBase * pDataItem);

    static CSettingUIItemDataBase * CreateBaseAccountDataItem();

    // 解析Item数据
    static bool ParserAccountItemData(const xml_node & node, CSettingUIAccountItemDataBase * pItemInfo);

public:
    // 获取、设置LineID
    int GetLineID();
    void SetLineID(int iLineID);

protected:
    int                     m_iLineID;              // 账号ID
};

typedef CSettingUIAccountItemDataBase   *   CSettingUIAccountItemDataBasePtr;

#endif // __SETTINGUI_BASE_ACCOUNT_ITEM_DATA_H__

