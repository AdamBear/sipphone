#include "itemdatavaluetrans.h"

//item
#include "baseui/include/detaillistitem.h"
#include "baseui/include/detaillistitembase.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/radiolistitem.h"
#include "baseui/include/inputmethod.h"

#include "bluetoothdevitem.h"
#include "wifinetitem.h"
#include "textmessageitem.h"
#include "btchannelcontrollitem.h"
#include "mainmenuitem.h"

//itemdata
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguibluetoothdevitemdata.h"
#include "settinguiwifinetitemdata.h"
#include "settinguicustomcomboboxitemdata.h"
#include "settinguimenudetailitemdata.h"
#include "settinguiradioitemdata.h"
#include "settinguitextmessageitemdata.h"
#include "settinguibtchannelcontrollitemdata.h"
#include "settinguimainmenuitemdata.h"
#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
#include "voicemailtimeritemdata.h"
#include "voicemailtimeritem.h"
#endif


///////////////////////////////////////////////////////////////////////////////////
CItemDataValueTransBase::CItemDataValueTransBase()
{

}

CItemDataValueTransBase::~CItemDataValueTransBase()
{

}

CItemDataValueTransBase * CItemDataValueTransBase::CreatBaseItemDataValueTrans()
{
    CItemDataValueTransBase * pTrans = new CItemDataValueTransBase();
    return pTrans;
}

void CItemDataValueTransBase::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pItem);
    if (NULL != pItem && NULL != pItemData && SETTING_ITEM_TYPE_BASE == pItemData->GetItemType())
    {
        pDetailItem->SetShowColon(pItemData->IsShowColon());
        pDetailItem->SetShowIndex(pItemData->IsShowIndex());
        pDetailItem->SetTitle(pItemData->GetDisplayName().c_str());
        pDetailItem->SetValue(pItemData->GetValue());
    }
}

///////////////////////////////////////////////////////////////////////////////////
CMenuItemDataValueTrans::CMenuItemDataValueTrans()
{

}

CMenuItemDataValueTrans::~CMenuItemDataValueTrans()
{

}

CItemDataValueTransBase * CMenuItemDataValueTrans::CreatMenuItemDataValueTrans()
{
    CMenuItemDataValueTrans * pMenuTrans = new CMenuItemDataValueTrans();
    return pMenuTrans;
}

void CMenuItemDataValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    CSettingUIMenuItemDataPtr pMenuItemData = CSettingUIMenuItemData::GetMenuListDataItem(pItemData);
    if (NULL != pItem && NULL != pMenuItemData)
    {
        pMenuItem->SetShowIndex(pMenuItemData->IsShowIndex());
        pMenuItem->SetMenu(pMenuItemData->GetDisplayName().c_str());
    }
}

//////////////////////////////////////////////////////////////////////////////////
CComboxBoxItemDataTrans::CComboxBoxItemDataTrans()
{

}

CComboxBoxItemDataTrans::~CComboxBoxItemDataTrans()
{

}

CItemDataValueTransBase * CComboxBoxItemDataTrans::CreatComboxboxItemDataValueTrans()
{
    CComboxBoxItemDataTrans * pComboxBox = new CComboxBoxItemDataTrans();
    return pComboxBox;
}

void CComboxBoxItemDataTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CComboBoxListItemPtr pComboBoxItem = CComboBoxListItem::GetComboBoxListItem(pItem);
    CSettingUIComboBoxItemDataPtr pCmbItemData = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                pItemData);
    if (NULL == pCmbItemData)
    {
#if IF_SUPPORT_CUSTOM_COMBO_BOX
        pCmbItemData = (CSettingUIComboBoxItemData *)
                       CSettingUICustomComboBoxItemData::GetCustomComboBoxDataItem(pItemData);
#endif
    }

    if (NULL != pComboBoxItem && NULL != pCmbItemData)
    {
        // 清空之前数据
        pComboBoxItem->ClearValue();

        pComboBoxItem->SetShowIndex(pCmbItemData->IsShowIndex());
        pComboBoxItem->SetShowColon(pCmbItemData->IsShowColon());
        pComboBoxItem->SetTitle(pCmbItemData->GetDisplayName().c_str());

        pComboBoxItem->SetSwitchSoftKey(pCmbItemData->IsSwitchSoftKey());
        pComboBoxItem->SetComboBoxDisable(!pCmbItemData->IsEnable());
        pComboBoxItem->SetIsSetupSoftkeyByItem(pCmbItemData->IsSetupSoftkeyByItem());

        ListComboBoxOptionData pCmbOptionData = pCmbItemData->GetComboBoxOptionData();
        ListComboBoxOptionData::iterator iter = pCmbOptionData.begin();
        for (; iter != pCmbOptionData.end(); ++iter)
        {
            CSettingUIComboBoxOption * pOptionData = *iter;
            if (NULL != pOptionData)
            {
                pComboBoxItem->AddValue(pOptionData->m_strDisplayName.c_str(), pOptionData->m_strItemVale.c_str());
            }
        }

        if (!pCmbItemData->GetValue().empty())
        {
            pComboBoxItem->SetItemData(pCmbItemData->GetValue().c_str());
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
CEditItemDataTrans::CEditItemDataTrans()
{

}

CEditItemDataTrans::~CEditItemDataTrans()
{

}

CItemDataValueTransBase * CEditItemDataTrans::CreatEditItemDataValueTrans()
{
    CEditItemDataTrans * pEditTrans = new CEditItemDataTrans;
    return pEditTrans;
}

void CEditItemDataTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CEditListItemPtr pEditItem = CEditListItem::GetEditListItem(pItem);
    CSettingUILineEditItemDataPtr pInputData = dynamic_cast<CSettingUILineEditItemDataPtr>(pItemData);
    if (NULL != pEditItem && NULL != pInputData)
    {
        pEditItem->SetShowIndex(pInputData->IsShowIndex());
        pEditItem->SetShowColon(pInputData->IsShowColon());
        pEditItem->InstallIME(pInputData->GetIME().c_str(), pInputData->GetDefaultIME().c_str());
        pEditItem->SetSuddenPassword(true);
        pEditItem->SetPassword(pInputData->IsPassword());
        pEditItem->SetTitle(pInputData->GetDisplayName().c_str());
        pEditItem->SetValue(pInputData->GetValue());
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
#ifndef _WIN32
#warning hint empty
#endif
        pEditItem->SetHintText(pInputData->GetHintText());
#endif
        pEditItem->SetEditEnable(pInputData->IsEnable());
        pEditItem->SetMaxLength(pInputData->GetMaxLength());
    }
}

///////////////////////////////////////////////////////////////////////////////////
CDetailMenuItemDataValueTrans::CDetailMenuItemDataValueTrans()
{

}

CDetailMenuItemDataValueTrans::~CDetailMenuItemDataValueTrans()
{

}

CItemDataValueTransBase * CDetailMenuItemDataValueTrans::CreatDetailMenuItemDataValueTrans()
{
    CDetailMenuItemDataValueTrans * pTrans = new CDetailMenuItemDataValueTrans();
    return pTrans;
}

void CDetailMenuItemDataValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CDetailListItemPtr pDetailItem = CDetailListItem::GetDetailListItem(pItem);
    CSettingUIDetailMenuItemDataPtr pDetailMenuItemData =
        CSettingUIDetailMenuItemData::GetDetailMenuListDataItem(pItemData);
    if (NULL != pItem && NULL != pDetailMenuItemData)
    {
        pDetailItem->SetShowIndex(pDetailMenuItemData->IsShowIndex());
        pDetailItem->SetShowColon(pDetailMenuItemData->IsShowColon());
        pDetailItem->SetTitle(pDetailMenuItemData->GetDisplayName().c_str());
        pDetailItem->SetValue(pDetailMenuItemData->GetValue());
    }
}

///////////////////////////////////////////////////////////////////////////////////
CRadioItemDataValueTrans::CRadioItemDataValueTrans()
{

}

CRadioItemDataValueTrans::~CRadioItemDataValueTrans()
{

}

CItemDataValueTransBase * CRadioItemDataValueTrans::CreatRadioItemDataValueTrans()
{
    CRadioItemDataValueTrans * pRadioTrans = new CRadioItemDataValueTrans();
    return pRadioTrans;
}

void CRadioItemDataValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CRadioListItem * pRadioItem = CRadioListItem::GetRadioListItem(pItem);
    CSettingUIRadioItemDataPtr pRadioItemData = CSettingUIRadioItemData::GetRadioListDataItem(
                pItemData);
    if (NULL != pRadioItem && NULL != pRadioItemData)
    {
        pRadioItem->SetShowIndex(pRadioItemData->IsShowIndex());
        pRadioItem->SetMenu(pRadioItemData->GetDisplayName().c_str());
        pRadioItem->SetCheck(pRadioItemData->IsCheck());
    }
}

///////////////////////////////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
CBluetoothDevItemValueTrans::CBluetoothDevItemValueTrans()
{

}

CBluetoothDevItemValueTrans::~CBluetoothDevItemValueTrans()
{

}

CItemDataValueTransBase * CBluetoothDevItemValueTrans::CreatBluetoothDevItemDataValueTrans()
{
    CBluetoothDevItemValueTrans * pBluetoothDevTrans = new CBluetoothDevItemValueTrans();
    return pBluetoothDevTrans;
}

void CBluetoothDevItemValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CBluetoothDevItem * pBluetoothDevItem = CBluetoothDevItem::GetBluetoothDevItem(pItem);
    CSettingUIBluetoothDevItemDataPtr pBluetoothDevItemData =
        CSettingUIBluetoothDevItemData::GetBluetoothDevDataItem(pItemData);
    if (NULL != pBluetoothDevItem && NULL != pBluetoothDevItemData)
    {
        pBluetoothDevItem->SetDevIcon(pBluetoothDevItemData->GetDevIcon());
        pBluetoothDevItem->SetName(pBluetoothDevItemData->GetDisplayName());
        pBluetoothDevItem->SetConnectStatusIcon(pBluetoothDevItemData->GetConnectStatusIcon());
        pBluetoothDevItem->SetMac(pBluetoothDevItemData->GetMac());
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_WIFI
CWifiNetItemValueTrans::CWifiNetItemValueTrans()
{

}

CWifiNetItemValueTrans::~CWifiNetItemValueTrans()
{

}

CItemDataValueTransBase * CWifiNetItemValueTrans::CreatWifiNetItemDataValueTrans()
{
    CWifiNetItemValueTrans * pWifiNetTrans = new CWifiNetItemValueTrans();
    return pWifiNetTrans;
}

void CWifiNetItemValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CWifiNetItem * pWifiNetItem = CWifiNetItem::GetWifiNetItem(pItem);
    CSettingUIWifiNetItemDataPtr pWifiNetItemData = CSettingUIWifiNetItemData::GetWifiNetDataItem(
                pItemData);
    if (NULL != pWifiNetItem && NULL != pWifiNetItemData)
    {
        pWifiNetItem->SetSignalStrengthIcon(pWifiNetItemData->GetSignalStrengthIcon());
        pWifiNetItem->SetName(pWifiNetItemData->GetDisplayName());
        pWifiNetItem->SetConnectStatusIcon(pWifiNetItemData->GetConnectStatusIcon());
        pWifiNetItem->SetEncryptIcon(pWifiNetItemData->GetEncryptIcon());
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////////
CTextMessageItemValueTrans::CTextMessageItemValueTrans()
{

}

CTextMessageItemValueTrans::~CTextMessageItemValueTrans()
{

}

CItemDataValueTransBase * CTextMessageItemValueTrans::CreatTextMessageItemDataValueTrans()
{
    CTextMessageItemValueTrans * pTextMessageTrans = new CTextMessageItemValueTrans();
    return pTextMessageTrans;
}

void CTextMessageItemValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CTextMessageItem * pTextMessageItem = CTextMessageItem::GetTextMessageItem(pItem);
    CSettingUITextMessageItemDataPtr pTextMessageItemData =
        CSettingUITextMessageItemData::GetTextMessageDataItem(pItemData);
    if (NULL != pTextMessageItem && NULL != pTextMessageItemData)
    {
        pTextMessageItem->SetStatusIcon(pTextMessageItemData->GetStatusIcon());
        pTextMessageItem->SetName(pTextMessageItemData->GetDisplayName());
        pTextMessageItem->SetDateString(pTextMessageItemData->GetDateString());
        pTextMessageItem->SetUrgent(pTextMessageItemData->GetUrgent());
    }
}

///////////////////////////////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
CBTChannelControllItemValueTrans::CBTChannelControllItemValueTrans()
{

}

CBTChannelControllItemValueTrans::~CBTChannelControllItemValueTrans()
{

}

CItemDataValueTransBase *
CBTChannelControllItemValueTrans::CreatBTChannelControllItemDataValueTrans()
{
    CBTChannelControllItemValueTrans * pTrans = new CBTChannelControllItemValueTrans();
    return pTrans;
}

void CBTChannelControllItemValueTrans::TransItemDataValue(CSettingUIItemDataBase * pItemData,
        xListViewItem * pItem)
{
    CBTChannelControllItem * pBTItem = CBTChannelControllItem::GetBTChannelControllItem(pItem);
    CSettingUIBTChannelControllItemData * pBTItemData =
        CSettingUIBTChannelControllItemData::GetBTChannelControllDataItem(pItemData);
    if (NULL != pBTItem && NULL != pBTItemData)
    {
        pBTItem->ClearValue();

        pBTItem->SetShowIndex(pBTItemData->IsShowIndex());
        pBTItem->SetShowColon(pBTItemData->IsShowColon());
        pBTItem->SetTitle(pBTItemData->GetDisplayName().c_str());

        pBTItem->SetSwitchSoftKey(pBTItemData->IsSwitchSoftKey());
        pBTItem->SetComboBoxDisable(!pBTItemData->IsEnable());
        pBTItem->SetIsSetupSoftkeyByItem(pBTItemData->IsSetupSoftkeyByItem());

        ListComboBoxOptionData pCmbOptionData = pBTItemData->GetComboBoxOptionData();
        ListComboBoxOptionData::iterator iter = pCmbOptionData.begin();
        for (; iter != pCmbOptionData.end(); ++iter)
        {
            CSettingUIComboBoxOption * pOptionData = *iter;
            if (NULL != pOptionData)
            {
                pBTItem->AddValue(pOptionData->m_strDisplayName.c_str(), pOptionData->m_strItemVale.c_str());
            }
        }

        if (!pBTItemData->GetValue().empty())
        {
            pBTItem->SetItemData(pBTItemData->GetValue().c_str());
        }

        pBTItem->SetExtraName(pBTItemData->GetExtraDispname());
        pBTItem->SetExtraValue(pBTItemData->GetExtraValue());
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////////
#if IF_SETTING_MAINMENU_IS_TABLEVIEW
CMainMenuItemValueTrans::CMainMenuItemValueTrans()
{

}

CMainMenuItemValueTrans::~CMainMenuItemValueTrans()
{

}
CItemDataValueTransBase* CMainMenuItemValueTrans::CreatMainMenuItemDataValueTrans()
{
    CMainMenuItemValueTrans* pTextMessageTrans = new CMainMenuItemValueTrans();
    return pTextMessageTrans;
}

void CMainMenuItemValueTrans::TransItemDataValue(CSettingUIItemDataBase* pItemData, xListViewItem* pItem)
{
    CMainMenuItem* pMainMenuItem = CMainMenuItem::GetMainMenuItem(pItem);
    CSettingUIMainMenuItemDataPtr pMainMenuItemData =
        CSettingUIMainMenuItemData::GetMainMenuDataItem(pItemData);
    if (NULL != pMainMenuItem && NULL != pMainMenuItemData)
    {
        pMainMenuItem->SetName(pMainMenuItemData->GetDisplayName());
        pMainMenuItem->SetStatusIcon(pMainMenuItemData->GetPicPath());
    }
}
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
CVoiceMailTimerItemValueTrans::CVoiceMailTimerItemValueTrans()
{

}

CVoiceMailTimerItemValueTrans::~CVoiceMailTimerItemValueTrans()
{

}

CItemDataValueTransBase* CVoiceMailTimerItemValueTrans::CreatVoiceMailTimerItemDataValueTrans()
{
    CVoiceMailTimerItemValueTrans* pTextMessageTrans = new CVoiceMailTimerItemValueTrans();
    return pTextMessageTrans;
}

void CVoiceMailTimerItemValueTrans::TransItemDataValue(CSettingUIItemDataBase* pItemData, xListViewItem* pItem)
{
    CVoiceMailTimerItem* pVMTimerItem = CVoiceMailTimerItem::GetVoiceMailTimerItem(pItem);
    CVoiceMailTimerItemDataPtr pVMTimerItemData =
        CVoiceMailTimerItemData::GetVoiceMailTimerDataItem(pItemData);
    if (NULL != pVMTimerItem && NULL != pVMTimerItemData)
    {
        if (pVMTimerItemData->IsIconShow())
        {
            pVMTimerItem->SetIconShow(true);
            pVMTimerItem->SetIcon(pVMTimerItemData->GetStatusIcon());
        }
        else
        {
            pVMTimerItem->SetIconShow(false);
        }

        if (pVMTimerItemData->IsStatusTextShow())
        {
            pVMTimerItem->SetTextStatusShow(true);
            pVMTimerItem->SetPlayStatus(pVMTimerItemData->GetStatusText());
        }
        else
        {
            pVMTimerItem->SetTextStatusShow(false);
        }

        pVMTimerItem->SetTimerMaxTick(pVMTimerItemData->GetTimerMaxTicks());
        pVMTimerItem->SetTotalTime(pVMTimerItemData->GetTotalTimeString());
        int nPlayStatus = pVMTimerItemData->GetPlayStatus();
        switch (nPlayStatus)
        {
        case VMPS_PLAY:
            {
                pVMTimerItem->StartTimer();
            }
            break;
        case VMPS_STOP:
            {
                pVMTimerItem->StopTimer();
            }
            break;
        case VMPS_PAUSE:
            {
                pVMTimerItem->PauseTimer();
            }
            break;
        case VMPS_RESUME:
            {
                pVMTimerItem->ResumeTimer();
            }
            break;

        default:
            break;
        }
    }
}
#endif
