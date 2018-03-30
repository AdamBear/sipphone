#ifndef _ITEM_DATA_VALUE_TRANS_H_
#define _ITEM_DATA_VALUE_TRANS_H_

#include "ylstring.h"
#include "settinguidefine.h"

class xListViewItem;
class CSettingUIItemDataBase;

////////////////////////////////////////////////////////////////
class CItemDataValueTransBase
{
public:
    CItemDataValueTransBase();
    virtual ~CItemDataValueTransBase();
    static CItemDataValueTransBase * CreatBaseItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
class CMenuItemDataValueTrans : public CItemDataValueTransBase
{
public:
    CMenuItemDataValueTrans();
    virtual ~CMenuItemDataValueTrans();
    static CItemDataValueTransBase * CreatMenuItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
class CComboxBoxItemDataTrans : public CItemDataValueTransBase
{
public:
    CComboxBoxItemDataTrans();
    virtual ~CComboxBoxItemDataTrans();
    static CItemDataValueTransBase * CreatComboxboxItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
class CEditItemDataTrans : public CItemDataValueTransBase
{
public:
    CEditItemDataTrans();
    virtual ~CEditItemDataTrans();
    static CItemDataValueTransBase * CreatEditItemDataValueTrans();

public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
class CDetailMenuItemDataValueTrans : public CItemDataValueTransBase
{
public:
    CDetailMenuItemDataValueTrans();
    virtual ~CDetailMenuItemDataValueTrans();
    static CItemDataValueTransBase * CreatDetailMenuItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
class CRadioItemDataValueTrans : public CItemDataValueTransBase
{
public:
    CRadioItemDataValueTrans();
    virtual ~CRadioItemDataValueTrans();
    static CItemDataValueTransBase * CreatRadioItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
class CBluetoothDevItemValueTrans : public CItemDataValueTransBase
{
public:
    CBluetoothDevItemValueTrans();
    virtual ~CBluetoothDevItemValueTrans();
    static CItemDataValueTransBase * CreatBluetoothDevItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};
#endif
////////////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_WIFI
class CWifiNetItemValueTrans : public CItemDataValueTransBase
{
public:
    CWifiNetItemValueTrans();
    virtual ~CWifiNetItemValueTrans();
    static CItemDataValueTransBase * CreatWifiNetItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};
#endif
////////////////////////////////////////////////////////////////
class CTextMessageItemValueTrans : public CItemDataValueTransBase
{
public:
    CTextMessageItemValueTrans();
    virtual ~CTextMessageItemValueTrans();
    static CItemDataValueTransBase * CreatTextMessageItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};

////////////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
class CBTChannelControllItemValueTrans : public CItemDataValueTransBase
{
public:
    CBTChannelControllItemValueTrans();
    virtual ~CBTChannelControllItemValueTrans();
    static CItemDataValueTransBase * CreatBTChannelControllItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase * pItemData, xListViewItem * pItem);
};
#endif

////////////////////////////////////////////////////////////////
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
class CMainMenuItemValueTrans : public CItemDataValueTransBase
{
public:
    CMainMenuItemValueTrans();
    virtual ~CMainMenuItemValueTrans();
    static CItemDataValueTransBase* CreatMainMenuItemDataValueTrans();
    virtual void TransItemDataValue(CSettingUIItemDataBase* pItemData, xListViewItem* pItem);
};
#endif


////////////////////////////////////////////////////////////////
#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
class CVoiceMailTimerItemValueTrans : public CItemDataValueTransBase
{
public:
    CVoiceMailTimerItemValueTrans();
    virtual ~CVoiceMailTimerItemValueTrans();
    static CItemDataValueTransBase* CreatVoiceMailTimerItemDataValueTrans();
public:
    virtual void TransItemDataValue(CSettingUIItemDataBase* pItemData, xListViewItem* pItem);
};
#endif

#endif //_ITEM_DATA_VALUE_TRANS_H_
