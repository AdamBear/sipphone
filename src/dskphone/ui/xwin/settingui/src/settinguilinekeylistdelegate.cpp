#include "settinguilinekeylistdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "dsskeyuilogic/include/dsskeyuilogiccommon.h"
#include "dsskey/src/dsskeycommonfunc.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiLinekeyListDelegate::CSettinguiLinekeyListDelegate()
{
}

CSettinguiLinekeyListDelegate::~CSettinguiLinekeyListDelegate()
{
}

CSettingUIDelegateBase* CSettinguiLinekeyListDelegate::CreateLinekeyListDelegate()
{
    CSettinguiLinekeyListDelegate* pDelegate = new CSettinguiLinekeyListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiLinekeyListDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        PageOpenData opPageData(kszPageIDDsskeySetting);
        DsskeyPageInfo m_PageInfo;
        m_PageInfo.m_eUIType = DSSKEY_UI_LINEKEY;
        m_PageInfo.SetIndex(iDataIndex, 0);
        opPageData.pExtraData = (void*)(&m_PageInfo);
        g_SetUICallBackFun.EnterPage(opPageData);
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

CSettingUIPageDataBase* CSettinguiLinekeyListDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);
    if (NULL == m_pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        int nLinekeyNum = GetLinekeyNumber();
        if (nLinekeyNum <= 0)
        {
            return NULL;
        }

        yl::string strLinekeyName;
        for (int i = 0; i < nLinekeyNum; ++i)
        {
            strLinekeyName = commonAPI_FormatString("%s %d", _LANG(TRID_LINEKEY).c_str(), i + 1);
            AddMenuItem(m_pPageData, "", strLinekeyName, "");
        }
    }

    return m_pPageData;
}
