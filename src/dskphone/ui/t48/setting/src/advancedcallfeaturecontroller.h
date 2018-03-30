#ifndef SETTING_ADVANCEDCALLFEATURECONTROLLER_H_
#define SETTING_ADVANCEDCALLFEATURECONTROLLER_H_

#include <ylstring.h>
#include <list>

#include "commondefine.h"
#include "setting/include/common.h"
struct AccountData
{
    yl::string  m_strName;
    int         m_nID;
    AccountData()
    {
        Reset();
    }
    void Reset()
    {
        m_strName = "";
        m_nID = -1;
    }
};
struct AccountItemListData
{
    std::list<AccountData>  m_listAccount;
    AccountItemListData()
    {
        Reset();
    }
    void Reset()
    {
        m_listAccount.clear();
    }
};

struct AdvancedCallFeatureCallCompletionData;
struct AdvancedCallFeatureAutoRedialData;
struct AdvancedCallFeatureHotlineData;
struct AdvancedCallFeatureGeneralData;
struct MenuItemListData;

namespace NS_SETTING
{
class CAdvancedCallFeatureModel;

// Class CAdvancedCallFeatureController
class CAdvancedCallFeatureController
{
public:
    // Get the singleton instance, it is a lazy instance.
    static CAdvancedCallFeatureController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get page menu list by click action.
    // | pData | [out] is the data retrieved.
    // | strClickAction | is the click action, it must begin with OpenPage.
    // Such as "OpenPage("account_list.xml")".
    // Returns true if successfully, otherwise return false.
    bool GetPageMenuListByClickAction(MenuItemListData * pData,
                                      const yl::string & strClickAction) const;

    // Get title by click action.
    // | strClickAction | is the click action.
    // Return the title if Retrieved successfully, otherwise return empty.
    const yl::string GetTitleByClickAction(
        const yl::string & strClickAction) const;

    // Get general data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetGeneralData(AdvancedCallFeatureGeneralData * pData) const;
    // Set general data.
    // | refData | is the data to set.
    void SetGeneralData(const AdvancedCallFeatureGeneralData & refData);

    // Get hot line data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetHotlineData(AdvancedCallFeatureHotlineData * pData) const;
    // Set hot line data.
    // | refData | is the data to set.
    void SetHotlineData(const AdvancedCallFeatureHotlineData & refData);

    //获取可用账号
    bool GetValidAccountList(AccountItemListData * pData) const;

    //获取默认账号的ID
    int GetDefaultAccountID();

    //设置默认账号
    void SetDefaultAccount(const int & nAccountID);
    // Get auto redial data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetAutoRedialData(AdvancedCallFeatureAutoRedialData * pData) const;
    // Set auto redial data.
    // | refData | is the data to set.
    void SetAutoRedialData(const AdvancedCallFeatureAutoRedialData & refData);

    // Get call completion data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetCallCompletionData(AdvancedCallFeatureCallCompletionData * pData) const;
    // Set call completion data.
    // | refData | is the data to set.
    void SetCallCompletionData(const AdvancedCallFeatureCallCompletionData & refData);

    void SetHotDeskBaseData(const HotDeskBaseData & refData);

private:
    // Ctor and Dtor
    CAdvancedCallFeatureController();
    ~CAdvancedCallFeatureController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CAdvancedCallFeatureModel * m_pModel; // The model instance.
    static CAdvancedCallFeatureController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CAdvancedCallFeatureController);
};
}  // namespace NS_SETTING

#define g_pAdvancedCallFeatureController (NS_SETTING::CAdvancedCallFeatureController::GetInstance())

#endif  // SETTING_ADVANCEDCALLFEATURECONTROLLER_H_
