#ifndef _SETTINGUI_BT_WIFI_DELEGATE_H_
#define _SETTINGUI_BT_WIFI_DELEGATE_H_

#if defined (IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)

#include "basesettinguidelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "viewmodeuimanager/include/viewmodeuidefine.h"

#define STR_VALUE_SPLIT_TOKEN ","

enum ItemDataID
{
    ITEM_DATA_ID_BEGIN = VIEW_MODE_UI_TYPE_END + 1,
    BT_ITEM_DATA_ID_MENU_SWITCH,
    BT_ITEM_DATA_ID_DETAIL_SYNC,
    BT_ITEM_DATA_ID_DONGLE_INFO_NAME,
    BT_ITEM_DATA_ID_CHANNEL_CONTROLL,
    BT_ITEM_DATA_ID_DEVICE_MOBILE_ENABLE,
    BT_ITEM_DATA_ID_DEVICE_MUSIC_ENABLE,
    ITEM_DATA_ID_END,
};

class xFrameLayout;
class CSettingUIBtWifiDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIBtWifiDelegate();
    virtual ~CSettingUIBtWifiDelegate();

    static CSettingUIDelegateBase * CreateBtWifiDelegate();

public:
    int GetUIType();
    int GetFocusDataID();
    bool GetEventParam(int nDataIndex, int & iDataID, yl::string & strValue);
    // 设置是否显示正在连接中的动画
    void ShowConnectAni(bool bEnable);

public:
    virtual void InitExtraData(void * pData);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);

public:
    virtual bool OnBackAction();
    virtual void OnReturnToIdle();
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
    int GetItemIndexByDataID(int nDataID);
    virtual bool IsResetFocusWhileReload()
    {
        return true;
    }

protected:
    xFrameLayout * GetTitle();
    void SaveFocusIndexItemData();
    virtual void InitPageListData();
    virtual CSettingUIItemDataBase * GetItem(int nIndex, int nDataID);
    virtual CSettingUIItemDataBase * CreateDefaultItemData(const ViewInfo & viewInfo);
    virtual bool InitListItemData(CSettingUIItemDataBase * pItemData, const ViewInfo & viewInfo);
    bool GetVecItemDataID2Info(VecItemDataID2Info & vecItemDataID2Info);

#if IF_SETTING_BT_WIFI_DEBUG
    void printfVecViewInfo(const VecViewInfo & vecInfo);
#endif

protected:
    int m_iUIType;
    int m_iVecInfoNum;
    int m_iNewFocusIndex;
    bool m_bDataNone;
};

#endif // (IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)
#endif //_SETTINGUI_BT_WIFI_DELEGATE_H_
