#ifndef SETTING_AUTOPROVISIONMODEL_H_
#define SETTING_AUTOPROVISIONMODEL_H_

#include <ylstring.h>

#include "commondefine.h"

struct AutoProvisionData;
struct MenuItemListData;

namespace NS_SETTING
{
class CAutoProvisionModel;

// Class CAutoProvisionController.
class CAutoProvisionController
{
public:
    // Get the singleton instance.
    static CAutoProvisionController * GetInstance();
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

    // Get auto provision data.
    // | pDate | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetAutoProvisionData(AutoProvisionData * pData);
    // Set auto provision data.
    // | refData | is the data to set.
    void SetAutoProvisionData(const AutoProvisionData & refData, bool bSaveUrl = true);

    // Begin auto provision.
    // Return true begin successfully, otherwise return false.
    bool BeginAutoProvision();

    bool GetAuthenticationInfo(AutoProvisionData * pData);

    void StartCurrentConnect(AutoProvisionData * pData);

private:
    // Ctor and Dtor.
    CAutoProvisionController();
    ~CAutoProvisionController();

    // Initialize.
    // Return true if initialized successfully, otherwise return false.
    bool MyInit();
    // Finalize.
    // Return true if finalized successfully, otherwise return false.
    bool MyFinal();

private:
    CAutoProvisionModel * m_pModel; // The model instance.

    static CAutoProvisionController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CAutoProvisionController);
};

}  // namespace NS_SETTING

#define g_pAutoProvisionController (NS_SETTING::CAutoProvisionController::GetInstance())

#endif  // SETTING_AUTOPROVISIONMODEL_H_
