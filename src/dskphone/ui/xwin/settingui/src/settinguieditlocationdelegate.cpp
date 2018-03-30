#include "settinguieditlocationdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_ANYWHERE

#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"

#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_OUTBOUND_PHONE_NUMBER 3
}

CSettinguiEditLocationDeletae::CSettinguiEditLocationDeletae()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiEditLocationDeletae::OnMessage);
}

CSettinguiEditLocationDeletae::~CSettinguiEditLocationDeletae()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiEditLocationDeletae::OnMessage);
}

CSettingUIDelegateBase * CSettinguiEditLocationDeletae::CreateEditLocationDelegate()
{
    CSettinguiEditLocationDeletae * pDelegate = new CSettinguiEditLocationDeletae;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiEditLocationDeletae::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiEditLocationDeletae * pDelegate = (CSettinguiEditLocationDeletae *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDAnyWhereEditLocation);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_ANYWHERE_GET_LOCATION_DETAIL == msg.wParam)
        {
            pDelegate->OnGetDetailLocation();
        }
        else if (XSI_ANYWHERE_MODIFY_LOCATION == msg.wParam || XSI_ANYWHERE_MODIFY_LOCATIONS == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_SAVE);
        }
        else
        {
            return FALSE;
        }
    }
    else if (XSI_MESSAGE_REQ_FAIL == msg.message)
    {
        pDelegate->OnMessageReqFail(msg);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

void CSettinguiEditLocationDeletae::InitExtraData(void * pData)
{
    CSettinguiXSIDelegatebase::InitExtraData(pData);

    SAYWLocationArray arrLocation;
    AnyWhere_GetAllLocationsQueryResultList(arrLocation);

    if (m_iLineID >= 0 && m_iLineID < (int)arrLocation.size())
    {
        m_LocationDetail = arrLocation.m_locatArr[m_iLineID];
    }
}

CSettingUIPageDataBase * CSettinguiEditLocationDeletae::LoadPageData(bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        AnyWhere_RequestLocationDetail(m_LocationDetail.m_strPhoneNum);
        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), m_eMsgboxID, -1,
                       MESSAGEBOX_Cancel);
    }

    return m_pPageData;
}

bool CSettinguiEditLocationDeletae::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    yl::string strOutBoundPhoneNum = GetValue(ANYWHERE_EDITLOCATION_ALTNUM);
    //检测Outbound Alternate Phone Number/SIP URI是否合法
    if (!IsOutBountPhoneNumberValid(strOutBoundPhoneNum))
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(INDEX_OUTBOUND_PHONE_NUMBER);
        validInfo.SetValidNote(_LANG(TRID_OUTBOUND_ALTERNATE_NUMBER_INVALID));
        validInfo.SetMsgBoxType(SETTING_MESSAGEBOX_Cancel);
        validInfo.SetShowtime(2000);
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    SAYWLocation stLocationDetail = m_LocationDetail;
    stLocationDetail.m_bActive = GetComboboxValue(ANYWHERE_EDITLOCATION_ACTIVE);
    stLocationDetail.m_strDescription = GetValue(ANYWHERE_EDITLOCATION_NAME);
    stLocationDetail.m_strPhoneNum = GetValue(ANYWHERE_EDITLOCATION_NUMBER);
    stLocationDetail.m_strOutBoundPhoneNum = strOutBoundPhoneNum;
    stLocationDetail.m_bEnableDiversionInhibitor = GetComboboxValue(ANYWHERE_EDITLOCATION_DI);
    stLocationDetail.m_bRequireAnswerConfirmation = GetComboboxValue(ANYWHERE_EDITLOCATION_AC);
    stLocationDetail.m_bUseBbasedCallControlServices = GetComboboxValue(ANYWHERE_EDITLOCATION_SCC);

    //执行添加操作
    AnyWhere_RequestModifyLocation(m_LocationDetail.m_strPhoneNum, stLocationDetail);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiEditLocationDeletae::OnCancleClick()
{
    AnyWhere_CancelAllRequest();
}

void CSettinguiEditLocationDeletae::OnGetDetailLocation()
{
    //取消加载提示框
    MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

    if (NULL == m_pPageData)
    {
        return;
    }

    //获取数据
    SAYWLocation stLocationDetail;
    if (AnyWhere_GetLocationDetailQueryResult(stLocationDetail))
    {
        SetComboboxValue(ANYWHERE_EDITLOCATION_ACTIVE, stLocationDetail.m_bActive);
        SetValue(ANYWHERE_EDITLOCATION_NAME, stLocationDetail.m_strDescription);
        SetValue(ANYWHERE_EDITLOCATION_NUMBER, stLocationDetail.m_strPhoneNum);
        SetValue(ANYWHERE_EDITLOCATION_ALTNUM, stLocationDetail.m_strOutBoundPhoneNum);
        SetComboboxValue(ANYWHERE_EDITLOCATION_DI, stLocationDetail.m_bEnableDiversionInhibitor);
        SetComboboxValue(ANYWHERE_EDITLOCATION_AC, stLocationDetail.m_bRequireAnswerConfirmation);
        SetComboboxValue(ANYWHERE_EDITLOCATION_SCC, stLocationDetail.m_bUseBbasedCallControlServices);
    }

    g_SetUICallBackFun.RefreshUI(false, GetPageID());
}

bool CSettinguiEditLocationDeletae::IsOutBountPhoneNumberValid(const yl::string & strOutboundNumber)
{
    if (!strOutboundNumber.empty()
            && !(BWCallControl_CheckPhoneNumberValid(strOutboundNumber.c_str())
                 || CheckIsSipURI(strOutboundNumber))
            || 1 == strOutboundNumber.size())   //这个字段要大于1
    {
        return false;
    }

    return true;
}

//判断是否Sip URI格式的号码
// The SIP URI must be in user@domain format.
bool CSettinguiEditLocationDeletae::CheckIsSipURI(const yl::string & strNumber)
{
    if (strNumber.empty())
    {
        return FALSE;
    }

    yl::string::size_type nPos = strNumber.find('@');

    if (nPos != yl::string::npos
            && nPos != 0
            && nPos != strNumber.size() - 1)
    {
        return TRUE;
    }

    return FALSE;
}

#endif //IF_FEATURE_BROADSOFT_ANYWHERE
