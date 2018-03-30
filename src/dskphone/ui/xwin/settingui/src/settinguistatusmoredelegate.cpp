#include "settinguistatusmoredelegate.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

CSettinguiStatusMoregDelegate::CSettinguiStatusMoregDelegate()
{

}

CSettinguiStatusMoregDelegate::~CSettinguiStatusMoregDelegate()
{

}

CSettingUIDelegateBase * CSettinguiStatusMoregDelegate::CreateStatusMoreDelegate()
{
    CSettinguiStatusMoregDelegate * pDelegate = new CSettinguiStatusMoregDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiStatusMoregDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettinguiDataDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL != pPagedata)
    {
        bool bShow = (1 == configParser_GetConfigInt(kszDisplayVQReportOnUI));
        SetItemShow(pPagedata, STATUS_MORE_RTP, bShow);
    }

    return bResult;
}

