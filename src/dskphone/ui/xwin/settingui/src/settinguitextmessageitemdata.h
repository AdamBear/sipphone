#ifndef __SETTINGUI_TEXTMESSAGE_ITEM_DATA_H__
#define __SETTINGUI_TEXTMESSAGE_ITEM_DATA_H__

#include "settinguilogic/src/settinguibaseitemdata.h"

enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_TYPE_MESSAGE       = 0x00080000,
};

class CSettingUITextMessageItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUITextMessageItemData();
    virtual ~CSettingUITextMessageItemData();

    static int GetTextMessageDataItemType();
    static CSettingUITextMessageItemData * GetTextMessageDataItem(CSettingUIItemDataBase * pDataItem);
    static CSettingUIItemDataBase * CreateTextMessageDataItem();

public:
    // 消息图标
    const yl::string & GetStatusIcon();
    void SetStatusIcon(const yl::string & strIcon);
    // 时间字符串
    const yl::string & GetDateString();
    void SetDateString(const yl::string & strDate);
    // 紧急标志
    bool GetUrgent();
    void SetUrgent(bool bUrgent);

protected:
    yl::string      m_strDate;  // 日期.
    yl::string      m_strIcon; // 图标
    bool            m_bUrgent;
};

typedef     CSettingUITextMessageItemData   *   CSettingUITextMessageItemDataPtr;

#endif //__SETTINGUI_TEXTMESSAGE_ITEM_DATA_H__
