#include "settinguisettingtypedelegate.h"
#include "uicallbackfun.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"

namespace
{
#define ID_ADVANCE_PASSWORD_MSGBOX 10
}

CSettingUISettingTypeDelegate::CSettingUISettingTypeDelegate()
{

}

CSettingUISettingTypeDelegate::~CSettingUISettingTypeDelegate()
{

}

CSettingUIDelegateBase * CSettingUISettingTypeDelegate::CreateSettingTypeDelegate()
{
    CSettingUISettingTypeDelegate * pDelegate = new CSettingUISettingTypeDelegate;
    return (CSettingUIDelegateBase *)pDelegate;
}

void CSettingUISettingTypeDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox
            && MESSAGEBOX_PWD == pMessageBox->GetType()
            && ID_ADVANCE_PASSWORD_MSGBOX == pMessageBox->GetID()
            && MessageBox_OK == pMessageBox->GetResult())
    {
        PageOpenData opPageData(kszPageIDAdvancedSetting);
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

void CSettingUISettingTypeDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    if (0 == strPage.compare(kszPageIDAdvancedSetting))
    {
        PopUpPasswordCheckMsgBox();
    }
    else
    {
        PageOpenData opPageData(strPage);
        opPageData.pExtraData = GetItemExtraDataByDataIndex(iDataIndex);
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

void CSettingUISettingTypeDelegate::PopUpPasswordCheckMsgBox()
{
    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_PWD;
    stMsgInfo.strTitle = _LANG(TRID_PLEASR_ENTER_PASSWORD);
    if (feature_UserModeIsOpen())
    {
        yl::string strUserName = feature_UserModeGetUserName(feature_UserModeGetCurrType());
        if (!strUserName.empty())
        {
            stMsgInfo.strTitle = _LANG(strUserName);
        }
    }

    yl::string strNode = _LANG(TRID_PASSWORD);
    strNode += ":";
    stMsgInfo.strNote = strNode;
    stMsgInfo.pAction = this;
    stMsgInfo.iID = ID_ADVANCE_PASSWORD_MSGBOX;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}
