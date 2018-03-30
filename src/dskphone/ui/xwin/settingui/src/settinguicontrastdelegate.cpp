#include "settinguicontrastdelegate.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/modsetuilogic.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "dsklog/log.h"

namespace
{
#define MAX_LEVEL 10
#define MIN_LEVEL 1
}

CSettinguiContrastDelegate::CSettinguiContrastDelegate()
{

}

CSettinguiContrastDelegate::~CSettinguiContrastDelegate()
{
    OnLeavePage();
}

CSettingUIDelegateBase * CSettinguiContrastDelegate::CreateContrastDelegate()
{
    CSettinguiContrastDelegate * pDelegate = new CSettinguiContrastDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiContrastDelegate::OnComboBoxValueChange(int nIndex)
{
    int nLevel = GetComboboxValue(CONTRAST_LEVEL);
    if (MIN_LEVEL <= nLevel && MAX_LEVEL >= nLevel)
    {
        SettingUILogic_Contrast(nLevel);
    }
}

void CSettinguiContrastDelegate::OnLeavePage()
{
    int nLevel = configParser_GetConfigInt(kszContrastLevel);
    if (GetComboboxValue(CONTRAST_LEVEL) != nLevel)
    {
        SettingUILogic_Contrast(nLevel);
    }
}
