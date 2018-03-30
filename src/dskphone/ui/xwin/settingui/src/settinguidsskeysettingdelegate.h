#ifndef _SETTINGUI_DSSKEY_SETTING_DELEGATE_H_
#define _SETTINGUI_DSSKEY_SETTING_DELEGATE_H_

#include <settingui/include/settinguidefine.h>
#if IF_SUPPORT_SELECT_CONTACT_INFO
#include "settinguidirectoryassociatebasedelegate.h"
#else
#include "settinguidatadelegatebase.h"
#endif
#include "dsskey/include/dsskey_enumtypes.h"
#include "dsskeyuilogic/include/dsskeyuilogiccommon.h"

typedef YLList<int> ListAccounts;

class CSettingUIComboBoxItemData;

#if IF_SUPPORT_SELECT_CONTACT_INFO
class CSettinguiDsskeySettingDelegate : public CDirectoryAssociateBaseDelegate
#else
class CSettinguiDsskeySettingDelegate : public CSettinguiDataDelegateBase
#endif
{
public:
    CSettinguiDsskeySettingDelegate();
    virtual ~CSettinguiDsskeySettingDelegate();

    static CSettingUIDelegateBase* CreateDsskeySettingDelegate();
public:
    static bool CompareID(int& nIDL, int& nIDR);
    virtual void InitExtraData(void* pData);
    virtual CSettingUIPageDataBase* LoadPageData(bool bReLoad = false);
    virtual bool SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious = true,
                              bool bPopHint = true);
    // ComboBox值改变响应函数
    virtual void OnComboBoxValueChange(int nIndex);
    // Edit值改变响应函数
#if IF_SUPPORT_SELECT_CONTACT_INFO
public:
    static void UpdateContactInfo(const yl::string& strContactName, const yl::string& strContactNumber);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex);
    virtual bool IsShowDirectorySoftkey();
protected:
    void UpdateDsskeyData(const yl::string& strContactName, const yl::string& strContactNumber);
#endif
protected:
    void SetPageValue();
    void UpdateTitle();
    void LoadListItemData(bool bReLoad);
    void LoadDsskeyTypeItem(bool bReadOnly);
    void LoadKeyEventTypeItem(bool bReadOnly);
    void LoadLineItemData(bool bReadOnly);
    void LoadIndexItemData(bool bReadOnly, const yl::string& strID);
    void LoadLineEditItemData(bool bReadOnly, const yl::string& strID);
#if IF_DSSKEY_SUPPORT_SHORT_LABEL
    void LoadSimpleLineEditItem(bool bReadOnly, const yl::string& strID);
#endif
    void LoadCandidateComboboxItem(const yl::string& strItemID, const yl::string& strDisplayName,
                                   bool bKeyEvent, bool bReadOnly);
    void GetLogicData();

    void AddComboboxoption(CSettingUIComboBoxItemData* pItemData, const yl::string& strDisplayName,
                           const yl::string& strValue);
    void GetPageData();
    // 获取页面实际Dsskey类型
    int GetRealType();
    int GetIndexByType(int nType, bool bKeyEvent);
    int GetTypeByIndex(int iIndex, bool bKeyEvent);
    int GetIndex();

    void LoadBaseItemData(const yl::string& strItemID, const yl::string& strDisplayName,
                          const yl::string& strValue = "");

#ifdef IF_FEATURE_PSTN
    bool GetAccountList(ListAccounts& listAccountId);
#endif // IF_FEATURE_PSTN

private:
    // UI到逻辑的转换
    static int GetAttrTypeByName(const yl::string& strID);

    // 逻辑层控件到UI映射
    static LPCSTR GetItemByAttrType(int iIndex);

private:
    // dsskey 页面信息
    DsskeyPageInfo          m_PageInfo;

    // 当前的dsskey配置值
    DssKeyUIData            m_DssKeyData;
    DssKeyUIData            m_OldDssKeyData;

    VecCandidateValue       m_vecKeyList;
    VecCandidateValue       m_vecKeyEvent;

};

#endif //_SETTINGUI_DSSKEY_SETTING_DELEGATE_H_
