#ifndef _SETTINGUI_ADV_ACCOUNT_LIST_DELEGATE_H_
#define _SETTINGUI_ADV_ACCOUNT_LIST_DELEGATE_H_

#include "basesettinguidelegate.h"

class CSettinguiAdvAccountListDelegate : public CSettingUIDelegateBase
{
public:
    CSettinguiAdvAccountListDelegate();
    ~CSettinguiAdvAccountListDelegate();

    static CSettingUIDelegateBase * CreateAdvAccountListDelegate();
public:
    static LRESULT OnMessage(msgObject & msg);
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual void OnEnterPage(const yl::string & strPage, int iDataIndex);
    virtual void GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey);
	//回到Idle前的处理函数
	virtual void OnReturnToIdle();
	// 点击Back事件
	virtual bool OnBackAction();
protected:
    int GetAccountIDByIndex(int nIndex);
    void LoadAccountItem(CSettingUIPageDataBase * pPageData);
    yl::string GetStatusStringByAccountID(int nAccountID);
    void AddDetailMenuItem(const yl::string & strDisplayName, const yl::string & strValue,
                           int nAccountID, const yl::string & strNextPageID);
#ifdef IF_FEATURE_PSTN
    void AddPstnItem(int nAccountID);
	void ClearPstnInfo();
#endif
};

#endif //_SETTINGUI_ADV_ACCOUNT_LIST_DELEGATE_H_
