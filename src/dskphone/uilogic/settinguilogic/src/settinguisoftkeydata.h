#ifndef __SETTINGUI_SOFTKEY_DATA_H__
#define __SETTINGUI_SOFTKEY_DATA_H__

#include "ylstl/ylstring.h"
#include "ylstl/yllist.h"


///////////////////////////////// CSettingUISoftkeyItemData //////////////////////////////////////
class CSettingUISoftkeyItemData
{
public:
    CSettingUISoftkeyItemData();
    virtual ~CSettingUISoftkeyItemData();

public:
    void Reset();

    CSettingUISoftkeyItemData & operator = (const CSettingUISoftkeyItemData & objSoftkeyItemData);

    bool GetSoftkeyData(CSettingUISoftkeyItemData & objSoftkeyItemData);
    bool SetSoftkeyData(const CSettingUISoftkeyItemData & objSoftkeyItemData);

public:
    int GetIndex() const;
    void SetIndex(int nIndex);

    const yl::string & GetDisplayName() const;
    void SetDisplayName(const yl::string & strDisplayName);
    const yl::string & GetAction() const;
    void SetAction(const yl::string & strAction);
    const yl::string & GetIconPath() const;
    void SetIconPath(const yl::string & strIconPath);

private:
    int                     m_nIndex;
    yl::string              m_strDisplayName;
    yl::string              m_strAction;
    yl::string              m_strIconPath;
};

typedef     CSettingUISoftkeyItemData   *   CSettingUISoftkeyItemDataPtr;
typedef     YLList<CSettingUISoftkeyItemData*>   LIST_SOFTKEY_ITEM_DATA_PTR;

///////////////////////////////// CSettingUISoftkeyData //////////////////////////////////////
class CSettingUISoftkeyData
{
public:
    CSettingUISoftkeyData();
    virtual ~CSettingUISoftkeyData();

public:
    void Reset();
    void Clear();

    CSettingUISoftkeyData & operator = (const CSettingUISoftkeyData & objSoftkeyData);

public:
    bool AddSoftkeyItem(CSettingUISoftkeyItemData * objSoftkeyItemData);
    bool DelSoftkeyItem(int nIndex);
    CSettingUISoftkeyItemData * GetSoftkeyItem(int nIndex);
    LIST_SOFTKEY_ITEM_DATA_PTR GetSoftkeyData();

private:
    LIST_SOFTKEY_ITEM_DATA_PTR      m_listSoftkeyData;
};

#endif

