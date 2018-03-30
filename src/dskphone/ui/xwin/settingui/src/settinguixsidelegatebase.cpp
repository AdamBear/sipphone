#include "settinguixsidelegatebase.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiXSIDelegatebase::CSettinguiXSIDelegatebase()
    : m_eMsgboxID(XSI_MSGBOX_ID_NONE)
{

}

CSettinguiXSIDelegatebase::~CSettinguiXSIDelegatebase()
{

}

CSettingUIDelegateBase * CSettinguiXSIDelegatebase::CreateXSIDelegatebase()
{
    CSettinguiXSIDelegatebase * pDelegate = new CSettinguiXSIDelegatebase;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiXSIDelegatebase::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nID = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();
    if (XSI_MSGBOX_ID_SAVE == nID)
    {
        if (MessageBox_Cancel == nResult
                || MessageBox_Close == nResult)
        {
            OnCancleClick();
        }
        else if (MessageBox_OK == nResult)
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
        }
    }
    else if (XSI_MSGBOX_ID_NETWORK_UNAVAIL == nID
             || XSI_MSGBOX_ID_NO_ACCOUNT == nID
             || XSI_MSGBOX_ID_INCOMPLETE_INFO == nID
             || (XSI_MSGBOX_ID_FAIL == nID && MessageBox_TimeOut == nResult))
    {
        g_SetUICallBackFun.FallBackToPrevousPage();
    }
    else if ((XSI_MSGBOX_ID_LOADING == nID || XSI_MSGBOX_ID_FAIL == nID)
             && MessageBox_Cancel == nResult)
    {
        if (XSI_MSGBOX_ID_LOADING == nID)
        {
            OnCancleClick();
        }
        g_SetUICallBackFun.RefreshUI(false, GetPageID());
    }
    else if (XSI_MSGBOX_ID_PWD_ERROR == nID && MessageBox_TimeOut == nResult)
    {
        PageOpenData opPageData(kszPageIDXSIPassword);
        yl::string strExtraData = commonAPI_FormatString("%s+%d", GetPageID().c_str(), m_iLineID);
        opPageData.pExtraData = (void *)&strExtraData;

        g_SetUICallBackFun.FallBackToPrevousPage();
        g_SetUICallBackFun.EnterPage(opPageData);
    }
}

void CSettinguiXSIDelegatebase::OnMessageReqFinish(msgObject & msg)
{
}

void CSettinguiXSIDelegatebase::OnMessageReqFail(msgObject & msg)
{
    MessageBox_RemoveMessageBox(this, MessageBox_Destory, m_eMsgboxID);

    if (XSI_MSGBOX_ID_LOADING == m_eMsgboxID)
    {
        g_SetUICallBackFun.RefreshUI(false);
    }

    SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)msg.GetExtraData();
    if (pErrInfo != NULL)
    {
        switch (pErrInfo->m_nError)
        {
        case 41: // 网络错误
            {
                ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_NETWORK_UNAVAILABLE), XSI_MSGBOX_ID_NETWORK_UNAVAIL,
                               2000, MESSAGEBOX_Cancel);
                return ;
            }
            break;
        case 401: // 鉴权错误
            {
                XSI_STATUS eXsiStatus = CBaseHelper::GetXSIStatus(msg.lParam);
                if (XSI_SIPACCOUNT_NOT_AVAILABLE == eXsiStatus)
                {
                    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                   XSI_MSGBOX_ID_FAIL, 1500, MESSAGEBOX_NOTE);
                }
                else if (XSI_AUTH_ERROR == eXsiStatus)
                {
                    yl::string strNote = _LANG(TRID_WEB_ACCESS_PASSWORD_IS_WRONG);
                    if (1 == configParser_GetConfigInt(kszAllowSipAuth))
                    {
                        strNote = _LANG(TRID_SIP_CREDENTIAL_PASSWORD_IS_WRONG);
                    }
                    ShowMessageBox(_LANG(TRID_NOTE), strNote, XSI_MSGBOX_ID_PWD_ERROR, SHORT_TIME, MESSAGEBOX_NOTE);
                }
                return;
            }
            break;
        default:
            break;
        }
    }
    yl::string strContent = _LANG(TRID_FAILED_PLEASE_RETRY);// 默认错误
    switch (m_eMsgboxID)
    {
    case XSI_MSGBOX_ID_LOADING:// 加载失败
        {
            strContent = _LANG(TRID_LOAD_FAILED);
        }
        break;
    case XSI_MSGBOX_ID_SAVE:// 保存失败
        {
            strContent = _LANG(TRID_SAVE_FAILED);
        }
        break;
    case XSI_MSGBOX_ID_DELETE:// 删除失败
        {
            strContent = _LANG(TRID_DELETE_FIALED);
        }
        break;
    default:
        break;
    }

    ShowMessageBox(_LANG(TRID_WARNING), strContent, XSI_MSGBOX_ID_FAIL, 1500, MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_NONE;
}

void CSettinguiXSIDelegatebase::OnCancleClick()
{
}

bool CSettinguiXSIDelegatebase::IsHasAccountRegister()
{
    if (acc_GetRegisteredAccountNum() <= 0)
    {
        ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_NO_ACCOUNT), XSI_MSGBOX_ID_NO_ACCOUNT, 1500,
                       MESSAGEBOX_NOTE);
        return false;
    }

    return true;
}

bool CSettinguiXSIDelegatebase::IsEnableToEnterNextPage(const yl::string & strPageID,
        int nAccountID)
{
    if (kszPageIDAnyWhereLocationList == strPageID
            || kszPageIDCallControllRemoteOffice == strPageID
            || kszPageIDCallControllHideNumber == strPageID
            || kszPageIDCallControllAnonymoRrejection == strPageID
            || kszPageIDCallControllGNF == strPageID
            || kszPageIDCallControllCFNR == strPageID
            || kszPageIDCallControllMobility == strPageID
            || kszPageIDCallControllCallWaiting == strPageID
            || kszPageIDCallControllForwardSelective == strPageID
            || kszPageIDCallControllSimultaneousRingList == strPageID
            || kszPageIDCallControllAssistantDetails == strPageID
            || kszPageIDCallControllExecutiveDetails == strPageID)
    {
        // 判断xsi信息是否完整
        if (!BWCallControl_IsAccountXSIEnable(nAccountID))
        {
            ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_XSI_INFORMATION_IS_INCOMPLETE), XSI_MSGBOX_ID_INCOMPLETE_INFO, 1500,
                           MESSAGEBOX_NOTE);

            return false;
        }
    }

    return true;
}

void CSettinguiXSIDelegatebase::ShowMessageBox(const yl::string & strTitle,
        const yl::string & strNote, int nID, int nTime, int nType)
{
    MSGBOX_INFO msgBoxInfo;
    msgBoxInfo.pAction = this;
    msgBoxInfo.eType = (MESSAGEBOX_TYPE)nType;
    msgBoxInfo.strTitle = strTitle;
    msgBoxInfo.strNote = strNote;
    msgBoxInfo.nMsTime = nTime;
    msgBoxInfo.iID = nID;
    MessageBox_ShowCommonMsgBox(msgBoxInfo);
}
