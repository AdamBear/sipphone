#include "settinguisecurityclassficationdelegate.h"

#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"


CSettinguiSecurityClassficationDelegate::CSettinguiSecurityClassficationDelegate()
{

}

CSettinguiSecurityClassficationDelegate::~CSettinguiSecurityClassficationDelegate()
{

}

CSettingUIDelegateBase *
CSettinguiSecurityClassficationDelegate::CreateSecurityClassficationDelegate()
{
    CSettinguiSecurityClassficationDelegate * pDelegate = new CSettinguiSecurityClassficationDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiSecurityClassficationDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        for (int i = 0; i < acc_AccountNum(); ++i)
        {
            if (!acc_IsAccountAvailable(i))
            {
                continue;
            }

            if (configParser_GetCfgItemIntValue(kszAccountEnableSecurityClassification, i) != 1)
            {
                continue;
            }

            yl::string strItemID = commonAPI_FormatString("%d", i);
            yl::string strValue = talklogic_GetAccountSecurityLevel(i);
            if (strValue.empty())
            {
                strValue = TRID_UNCLASSIFIED;
            }
            yl::string strDisp = acc_GetAccountShowText(i);

            AddBaseItemData(m_pPageData, strDisp, _LANG(strValue), strItemID, true, false);
        }

    }

    return m_pPageData;
}
