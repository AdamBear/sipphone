#ifndef SETTING_STATUSCONTROLLER_H_
#define SETTING_STATUSCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct AccountStatusListData;
struct MenuItemListData;
struct PhoneStatusData;
struct PhoneNetworkStatusData;

class CStatusModel;

// Class CStatusController
class CStatusController
{
public:
    // Get the singleton instance.
    static CStatusController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get phone status.
    // | pData | [out] is the buffer.
    // Returns true if retrieved successfully, otherwise return false.
    bool GetPhoneStatusData(PhoneStatusData * pData) const;

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

	// Get phone model value.
	const yl::string GetModel() const;

	// Get phone hardware value.
	const yl::string GetHardWare() const;

	// Get phone firmware value.
	const yl::string GetFirmware() const;

	// Get phone product Id value.
	const yl::string GetProductId() const;

private:
    CStatusController();
    ~CStatusController();

private:
    static CStatusController * m_pStatusController; // The status controller singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CStatusController);
};

#define g_pStatusController (CStatusController::GetInstance())

#endif  // SETTING_STATUSCONTROLLER_H_
