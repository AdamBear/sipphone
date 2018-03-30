#ifndef SETTING_RESETANDREBOOTCONTROLLER_H_
#define SETTING_RESETANDREBOOTCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"
#include "ETLLib/ETLLib.hpp"
#include "setting/include/common.h"

struct MenuItemListData;
struct ResetToFactoryData;

namespace NS_SETTING
{
class CResetAndRebootModel;

// Class CResetAndRebootController.
class CResetAndRebootController
{
public:
    // Get the singleton instance.
    static CResetAndRebootController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // 处理消息.
    // | refObj | 是要处理的消息.
    static LRESULT OnMessage(msgObject & refObj);

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

    // Get reset to factory data.
    // | pData | [out] is the buffer.
    // Return true if retrieved successfully, otherwise return false.
    bool GetResetData(ResetToFactoryData * pData);
    // Set reset to factory data.
    // | refData | is the data to set.
    void SetResetData(const ResetToFactoryData & refData);

    // Reset now!
    // | nOption | the reset option, is ResetOptionType enum.
    // Return true if reset successfully, otherwise return false.
    bool ResetNow(int nOption);

    // Reboot Now!
    // Return true if reboot successfully, otherwise return false.
    bool RebootNow(int iTimeDelay = 1);

    void OnRebootTimer();

    UINT GetRebootMsgID()
    {
        return (UINT)(&m_iReboot);
    }

    void RegisterRebootTipCallBack(FunRebootTipCallBack fun);

private:
    // Ctor and Dtor.
    CResetAndRebootController();
    ~CResetAndRebootController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

    // 显示重启的提示信息
    void ShowRebootTip(const yl::string & strTip);

private:
    CResetAndRebootModel       *      m_pModel;  // The model instance.
    static CResetAndRebootController * m_pController; // The singleton instance.
    int                               m_iReboot;
    FunRebootTipCallBack              m_funRebootTip;
    DISALLOW_COPY_AND_ASSIGN(CResetAndRebootController);
};
}  // namespace NS_SETTING

#define g_pResetAndRebootController (NS_SETTING::CResetAndRebootController::GetInstance())

#endif  // SETTING_RESETANDREBOOTCONTROLLER_H_
