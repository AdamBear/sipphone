#ifndef __SETTINGUI_ZERO_NETWORK_DELEGATE_H__
#define __SETTINGUI_ZERO_NETWORK_DELEGATE_H__

#include "settinguidatadelegatebase.h"

class CZeroNetworkDelegate : public CSettinguiDataDelegateBase
{
public:
    CZeroNetworkDelegate();
    virtual ~CZeroNetworkDelegate();

    static CSettingUIDelegateBase * CreateZeroNetworkDelegate();

    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
    virtual bool PageDataFilter(CSettingUIPageDataBase * pPagedata, bool bReload = false);
    virtual bool SavePageData(CSettingUIPageDataBase * pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    // 处理按键事件
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool PageDataValid(CSettingUIPageDataBase* pPagedata, CSettingUIDataValidInfo& validInfo);

private:
    void SetValue(const yl::string & strItemId, const yl::string & strValue);

    void FilterIpModeData(CSettingUIPageDataBase * pPagedata);
    void FilterVLanData(CSettingUIPageDataBase * pPagedata);
    void Filter8021Data(CSettingUIPageDataBase * pPagedata);
    void FilterVLanDhcp(CSettingUIPageDataBase * pPagedata);
    void FilterLLDPData(CSettingUIPageDataBase * pPagedata);
    void FilterCdpData(CSettingUIPageDataBase * pPagedata);
    void FilterStunData(CSettingUIPageDataBase * pPagedata);

    void FilterIpV4Data(CSettingUIPageDataBase * pPagedata);
    void FilterIpV6Data(CSettingUIPageDataBase * pPagedata);
    void HideIpV4StaticIPItem(CSettingUIPageDataBase * pPagedata);
    void HideIpV6StaticIPItem(CSettingUIPageDataBase * pPagedata);
    void HideIpV4StaticDnsItem(CSettingUIPageDataBase * pPagedata);
    void HideIpV6StaticDnsItem(CSettingUIPageDataBase * pPagedata);
};

typedef CZeroNetworkDelegate * CZeroNetworkDelegatePtr;

#endif // __SETTINGUI_ZERO_NETWORK_DELEGATE_H__
