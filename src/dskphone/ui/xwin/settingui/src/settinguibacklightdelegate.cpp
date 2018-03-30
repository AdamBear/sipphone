#include "settinguibacklightdelegate.h"

#if IF_SUPPORT_BACK_LIGHT_LEVEL
#include "settinguilogic/src/settinguibasepagedata.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "lamp/backlight/include/modbacklight.h"
#include "settinguilogic/src/settinguibaseitemdata.h"

namespace
{
#define BACK_LIGHT_TIME_AWAYLS_ON         0
#define BACK_LIGHT_TIME_FIFTEEN_SECONDS   15
#define BACK_LIGHT_TIME_THIRTY_SECONDS    30
#define BACK_LIGHT_TIME_ONE_MINUTE        60
#define BACK_LIGHT_TIME_TWO_MINUTES       120
#define BACK_LIGHT_TIME_FIVE_MINUTES      300
#define BACK_LIGHT_TIME_TEN_MINUTES       600
#define BACK_LIGHT_TIME_HALF_HOUR         1800

#define BACK_LIGHT_TIME_LEVEL_0             0
#define BACK_LIGHT_TIME_LEVEL_1             1
#define BACK_LIGHT_TIME_LEVEL_2             22
#define BACK_LIGHT_TIME_LEVEL_3             44
#define BACK_LIGHT_TIME_LEVEL_4             89
#define BACK_LIGHT_TIME_LEVEL_5             209
#define BACK_LIGHT_TIME_LEVEL_6             449
#define BACK_LIGHT_TIME_LEVEL_7             1199

#define BACK_LIGHT_ACTIVE_LEVEL_MIN         1
#define BACK_LIGHT_ACTIVE_LEVEL_MAX         10
#define BACK_LIGHT_ACTIVE_LEVEL_DEFAULT     8

#define BACK_LIGHT_INACTIVE_LEVEL_OFF         0
#define BACK_LIGHT_INACTIVE_LEVEL_LOW         1

#define BACK_LIGHT_INVALID_LEVEL            -1

}

CSettingUIBackLightDelegate::CSettingUIBackLightDelegate()
{
    backlight_ShowBacklightUI(true);
    backlight_OpenBacklightEvent(false);
}

CSettingUIBackLightDelegate::~CSettingUIBackLightDelegate()
{
    backlight_ShowBacklightUI(false);
    backlight_OpenBacklightEvent(true);
}

CSettingUIDelegateBase* CSettingUIBackLightDelegate::CreateSettingUIBackLightDelegate()
{
    CSettingUIBackLightDelegate* pDelegate = new CSettingUIBackLightDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CSettingUIBackLightDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return NULL;
    }

    int nActiveLevel = configParser_GetConfigInt(kszActiveLevel);
    if (nActiveLevel < BACK_LIGHT_ACTIVE_LEVEL_MIN || nActiveLevel > BACK_LIGHT_ACTIVE_LEVEL_MAX)
    {
        // 超出范围,使用默认值
        nActiveLevel = BACK_LIGHT_ACTIVE_LEVEL_DEFAULT;
    }
    SetComboboxValue(BACKLIGHTACTIVELEVEL, nActiveLevel);

    int nInActiveLevel = configParser_GetConfigInt(kszActiveLevel);
    if (nInActiveLevel < BACK_LIGHT_INACTIVE_LEVEL_OFF || nInActiveLevel > BACK_LIGHT_INACTIVE_LEVEL_LOW)
    {
        // 超出范围,使用默认值
        nInActiveLevel = BACK_LIGHT_INACTIVE_LEVEL_LOW;
    }
    SetComboboxValue(BACKLIGHTINACTIVELEVEL, nInActiveLevel);

    int nBackLightTime = configParser_GetConfigInt(kszBackLightTime);
    AdjustBackLightTime(nBackLightTime);
    SetComboboxValue(BACKLIGHTTIME, nBackLightTime);

    ResetBacklight();

    return m_pPageData;
}

bool CSettingUIBackLightDelegate::SavePageData(CSettingUIPageDataBase* pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    bool bRet = CSettinguiDataDelegateBase::SavePageData(pPageData);
    // 刷新Event
    backlight_RefreshEvent();
    return bRet;
}

void CSettingUIBackLightDelegate::OnComboBoxValueChange(int nIndex)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(nIndex);
    if (NULL == pItemData || pItemData->GetItemID().empty())
    {
        return;
    }

    yl::string strItemID = pItemData->GetItemID();
    if (BACKLIGHTACTIVELEVEL == strItemID)
    {
        ResetBacklight();
    }
}

void CSettingUIBackLightDelegate::ResetBacklight(int nLightLevel /*= -1*/)
{
    if (BACK_LIGHT_INVALID_LEVEL == nLightLevel)
    {
        int nActiveLevel = GetComboboxValue(BACKLIGHTACTIVELEVEL);
        backlight_SetBacklightLevel(nActiveLevel);
    }
    else
    {
        backlight_SetBacklightLevel(nLightLevel);
    }
}

void CSettingUIBackLightDelegate::AdjustBackLightTime(int& nTimes)
{
    if (nTimes <= BACK_LIGHT_TIME_LEVEL_0)
    {
        nTimes = BACK_LIGHT_TIME_AWAYLS_ON;
    }
    else if (nTimes >= BACK_LIGHT_TIME_LEVEL_1 && nTimes < BACK_LIGHT_TIME_LEVEL_2)
    {
        nTimes = BACK_LIGHT_TIME_FIFTEEN_SECONDS;
    }
    else if (nTimes >= BACK_LIGHT_TIME_LEVEL_2 && nTimes < BACK_LIGHT_TIME_LEVEL_3)
    {
        nTimes = BACK_LIGHT_TIME_THIRTY_SECONDS;
    }
    else if (nTimes >= BACK_LIGHT_TIME_LEVEL_3 && nTimes < BACK_LIGHT_TIME_LEVEL_4)
    {
        nTimes = BACK_LIGHT_TIME_ONE_MINUTE;
    }
    else if (nTimes >= BACK_LIGHT_TIME_LEVEL_4 && nTimes < BACK_LIGHT_TIME_LEVEL_5)
    {
        nTimes = BACK_LIGHT_TIME_TWO_MINUTES;
    }
    else if (nTimes >= BACK_LIGHT_TIME_LEVEL_5 && nTimes < BACK_LIGHT_TIME_LEVEL_6)
    {
        nTimes = BACK_LIGHT_TIME_FIVE_MINUTES;
    }
    else if (nTimes >= BACK_LIGHT_TIME_LEVEL_6 && nTimes < BACK_LIGHT_TIME_LEVEL_7)
    {
        nTimes = BACK_LIGHT_TIME_TEN_MINUTES;
    }
    else
    {
        nTimes = BACK_LIGHT_TIME_HALF_HOUR;
    }
}

#endif
