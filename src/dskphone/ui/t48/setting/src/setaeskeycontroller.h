#ifndef SETTING_SETAESKEYCONTROLLER_H_
#define SETTING_SETAESKEYCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

struct AesKeyData;
struct MenuItemListData;

namespace NS_SETTING
{
class CSetAesKeyModel;

// Class CSetAesKeyController.
class CSetAesKeyController
{
public:
    // Get the singleton instance.
    static CSetAesKeyController * GetInstance();
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

    // Get AES key data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetAesKeyData(AesKeyData * pData) const;
    // Set AES key data.
    // | refData | is the data to set.
    void SetAesKeyData(const AesKeyData & refData);

private:
    // Ctor and Dtor.
    CSetAesKeyController();
    ~CSetAesKeyController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    CSetAesKeyModel * m_pModel; // The model instance.

    static CSetAesKeyController * m_pController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CSetAesKeyController);
};

}  // namespace NS_SETTING

#define g_pSetAesKeyController (NS_SETTING::CSetAesKeyController::GetInstance())

#endif  // SETTING_SETAESKEYCONTROLLER_H_
