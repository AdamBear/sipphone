#ifndef SETTING_STATUSCONTROLLER_H_
#define SETTING_STATUSCONTROLLER_H_

#include <list>
#include <ylstring.h>

#include "commondefine.h"

struct AccountStatusListData;
struct MenuItemListData;
struct PhoneStatusData;
struct ExternalDeviceData;
struct PhoneNetworkStatusData;

namespace NS_SETTING
{

class CStatusModel;

// Class CStatusController
class CStatusController
{
public:
    // Get the singleton instance.
    static CStatusController * GetInstance();
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

    // Get phone status.
    // | pData | [out] is the buffer.
    // Returns true if retrieved successfully, otherwise return false.
    bool GetPhoneStatusData(PhoneStatusData * pData) const;

    bool GetExternalDeviceData(ExternalDeviceData * pExternalDeviceData) const;

    // Get phone network status.
    // | pNetworkData | is the network status data retrieved.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetPhoneNetworkStatus(PhoneNetworkStatusData * pNetworkData) const;

    // Get phone network status.
    // | pNetworkData | is the network status data retrieved.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetPhoneIPv4NetworkStatus(PhoneNetworkStatusData * pNetworkData) const;

    // Get phone network status.
    // | pNetworkData | is the network status data retrieved.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetPhoneIPv6NetworkStatus(PhoneNetworkStatusData * pNetworkData) const;

    // Get phone account status list.
    // | pData | [out] is the buffer.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetPhoneAccountStatus(AccountStatusListData * pData);

private:
    CStatusController();
    ~CStatusController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();

    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CStatusModel * m_pStatusModel; // The status model instance.
    static CStatusController * m_pStatusController; // The status controller singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CStatusController);
};

}  // namespace NS_SETTING

#define g_pStatusController (NS_SETTING::CStatusController::GetInstance())

#endif  // SETTING_STATUSCONTROLLER_H_
