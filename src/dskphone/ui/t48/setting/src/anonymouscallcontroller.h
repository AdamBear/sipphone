#ifndef SETTING_ANONYMOUSCALLCONTROLLER_H_
#define SETTING_ANONYMOUSCALLCONTROLLER_H_

#include <ylstring.h>

#include "ETLLib.hpp"

#include "commondefine.h"

struct AnonymousCallData;
struct MenuItemListData;

namespace NS_SETTING
{

class CAnonymousCallModel;

// Class CAnonymousCallController.
class CAnonymousCallController
{
public:
    // Get the singleton instance.
    static CAnonymousCallController * GetInstance();
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

    // Get anonymous call and reject anonymous call data by line Id.
    // | pData | [out] is the buffer.
    // | nLineId | is the line Id.
    // Returns true if retrieved successfully, otherwise
    // returns false.
    bool GetAnonymousCallDataByLineId(AnonymousCallData * pData, int nLineId) const;
    // Set anonymous call and reject anonymous call data by line id.
    // | nLineId | is the line id.
    // | refData | is the data to set.
    void SetAnonymousCallDataByLineId(int nLineId, const AnonymousCallData & refData);

    // Get anonymous call and reject anonymous call data by click action.
    // | pData | [out] is the buffer.
    // | nLineId | is the click action.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetAnonymousCallDataByClickAction(AnonymousCallData * pData,
                                           const yl::string & strClickAction) const;
    // Set anonymous call and reject anonymous call data by click action.
    // | strClickAction | is the click action.
    // | refData | is the data to set.
    void SetAnonymousCallDataByClickAction(const yl::string & strClickAction,
                                           const AnonymousCallData & refData);

    bool IsAdministratorHasSet(const yl::string & strClickAction);

private:
    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

    // Ctor and Dtor.
    CAnonymousCallController();
    ~CAnonymousCallController();

private:
    CAnonymousCallModel * m_pModel; // The anonymous call model instance.
    static CAnonymousCallController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CAnonymousCallController);
};

}  // namespace NS_SETTING

#define g_pAnonymousCallController (NS_SETTING::CAnonymousCallController::GetInstance())

#endif  // SETTING_ANONYMOUSCALLCONTROLLER_H_
