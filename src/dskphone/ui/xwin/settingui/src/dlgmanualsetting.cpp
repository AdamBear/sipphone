#include "dlgmanualsetting.h"
#include "uicallbackfun.h"
#include "imeedit/ximeedit.h"
#include "keymap.h"

#include "baseui/include/t2xpicpath.h"
#include "baseui/include/t2xwidget.h"
#include "baseui/include/baseuicommon.h"
#include "commonapi/stringhelperapi.h"
#include "xwindow/xSoftkeyBar.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/dateandtimecontroller.h"
#include "settinguilogic/src/datetimemanualsettingcontroller.h"
#include "settingui/include/pageiddefine.h"
#include "settinguiwizardmanager.h"

#include "messagebox/modmessagebox.h"
#include "messagebox/messageboxbase.h"

#include "devicelib/networkserver.h"

#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"
#include "edk/include/modedk.h"
#include "edkui/include/modedkui.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_TEXT_DATE 1
#define INDEX_TEXT_TIME 2
#define ST_LEFT_INDEX 1
#define ST_RIGHT_INDEX 2
#define MSGBOX_ID_SAVE 107

#define TM_SINCE_YEAR        1900  // 时间纪元

const char * kszEditYear = "EditYear";
const char * kszEditMonth = "EditMonth";
const char * kszEditDay = "EditDay";
const char * kszEditHour = "EditHour";
const char * kszEditMinute = "EditMinute";
const char * kszEditSecond = "EditSecond";
const char * kszAuthorid = "date_and_time_manual";
}

CBaseDialog * CDlgManualSetting::CreateInstance()
{
    CDlgManualSetting * pDlg = new CDlgManualSetting;

    if (NULL != pDlg)
    {
        pDlg->loadContent("setting/dlgmanualsetting.xml");
    }

    return pDlg;
}

CDlgManualSetting::CDlgManualSetting()
    : m_pFocusView(NULL)
    , m_bEditAble(true)
{
}

CDlgManualSetting::~CDlgManualSetting()
{
    RemoveEditTextChangeSlotByID(kszEditYear);
    RemoveEditTextChangeSlotByID(kszEditMonth);
    RemoveEditTextChangeSlotByID(kszEditDay);
    RemoveEditTextChangeSlotByID(kszEditHour);
    RemoveEditTextChangeSlotByID(kszEditMinute);
    RemoveEditTextChangeSlotByID(kszEditSecond);
}

void CDlgManualSetting::Init()
{
    if (feature_UserModeIsOpen())
    {
        m_bEditAble = !feature_UserModeConfigItemReadOnly(kszAuthorid);
    }
}

void CDlgManualSetting::Uninit()
{
    removeViewSoftKey(this, ST_LEFT_INDEX);
    removeViewSoftKey(this, ST_RIGHT_INDEX);
}

void CDlgManualSetting::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (MSGBOX_ID_SAVE == pMessageBox->GetID() && MessageBox_TimeOut == pMessageBox->GetResult())
    {
        FallBackToThePreviousInterface();
        g_SetUICallBackFun.RefreshUI(true);
        return;
    }

    return CDlgSettingUI::MessageBoxCallBack(pMessageBox);
}

void CDlgManualSetting::loadChildrenElements(xml_node & node)
{
    CDlgSettingUI::loadChildrenElements(node);

    m_pFocusView = (xEditTextView *)getViewById(kszEditYear);
    SetupEditTextChangeSlotByID(kszEditYear);
    SetupEditTextChangeSlotByID(kszEditMonth);
    SetupEditTextChangeSlotByID(kszEditDay);
    SetupEditTextChangeSlotByID(kszEditHour);
    SetupEditTextChangeSlotByID(kszEditMinute);
    SetupEditTextChangeSlotByID(kszEditSecond);
}

void CDlgManualSetting::RefreshUI(bool bReLoadData /* = false */)
{
    if (!bReLoadData)
    {
        return;
    }

    LoadPageData(bReLoadData);

    yl::string strTitle = _LANG(TRID_MANUAL_SETTING);
    if (Edk_IsEnable() && EDK_IsTipModeEnable())
    {
        strTitle = EdkUI_GetMenuIDByPageData(kszPageIDManualSetting, "", -1);
    }

    RefreshTitle(strTitle);

    SetCommonSoftkey();
#if IF_SUPPORT_LEFT_RIGHT_SOFTKEY
    SetLeftRightSoftKey();
#endif
}

bool CDlgManualSetting::LoadPageData(bool bReLoadData /* = false */)
{
    if (!bReLoadData)
    {
        return false;
    }

    time_t tmLocal = netGetLocalTime();
    tm * ptmTransit = localtime(&tmLocal);

    SetEditValueById(kszEditYear, ptmTransit->tm_year + TM_SINCE_YEAR);

    SetEditValueById(kszEditMonth, ptmTransit->tm_mon + 1);
    SetEditValueById(kszEditDay, ptmTransit->tm_mday);
    SetEditValueById(kszEditHour, ptmTransit->tm_hour);
    SetEditValueById(kszEditMinute, ptmTransit->tm_min);
    SetEditValueById(kszEditSecond, ptmTransit->tm_sec);

    return true;
}

bool CDlgManualSetting::onKeyPressedEvent(int key)
{
    if (PHONE_KEY_0 <= key && PHONE_KEY_9 >= key && !m_bEditAble)
    {
        return true;
    }

    if (PHONE_KEY_UP == key || PHONE_KEY_DOWN == key || PHONE_KEY_LEFT == key || PHONE_KEY_RIGHT == key)
    {
        OnDirectionKeyPress(key);
        return true;
    }

    if (PHONE_KEY_OK == key)
    {
        OnSavePress();
        return true;
    }

    return CDlgSettingUI::onKeyPressedEvent(key);
}

bool CDlgManualSetting::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    if (ST_LEFT == iType || ST_RIGHT == iType)
    {
        int nKey = (ST_LEFT == iType) ? PHONE_KEY_LEFT : PHONE_KEY_RIGHT;
        OnDirectionKeyPress(nKey);
    }
    else if (ST_SAVE == iType)
    {
        OnSavePress();
    }
    else if (ST_BACK == iType)
    {
        FallBackToThePreviousInterface();
    }

    return true;
}

yl::string CDlgManualSetting::IntTimeToStr(int nTime)
{
    return g_pDateTimeManualSettingController.GetStringByIntTime(nTime);
}

void CDlgManualSetting::OnDirectionKeyPress(int nKey)
{
    if (!m_bEditAble || NULL == m_pFocusView)
    {
        return;
    }

    RectifyFocusEditValue(true);

    yl::string strDirection = "";
    if (PHONE_KEY_UP == nKey)
    {
        strDirection = XWINDOW_NAME_NEXT_FOCUS_UP;
    }
    else if (PHONE_KEY_DOWN == nKey)
    {
        strDirection = XWINDOW_NAME_NEXT_FOCUS_DOWN;
    }
    else if (PHONE_KEY_LEFT == nKey)
    {
        strDirection = XWINDOW_NAME_NEXT_FOCUS_LEFT;
    }
    else if (PHONE_KEY_RIGHT == nKey)
    {
        strDirection = XWINDOW_NAME_NEXT_FOCUS_RIGHT;
    }
    else
    {
        return;
    }

    yl::string strNextView = m_pFocusView->getStringProperty(strDirection, "");
    xEditTextView * pEdit = (xEditTextView *)getViewById(strNextView);
    if (NULL != pEdit && pEdit != m_pFocusView)
    {
        m_pFocusView = pEdit;
        m_pFocusView->setFocus();
    }
}

void CDlgManualSetting::OnSavePress()
{
    if (!m_bEditAble)
    {
        return;
    }

    RectifyFocusEditValue(true);

    DateTime stDateTime;
    stDateTime.m_strYear = GetEditValueById(kszEditYear);
    stDateTime.m_strMonth = GetEditValueById(kszEditMonth);
    stDateTime.m_strDay = GetEditValueById(kszEditDay);
    stDateTime.m_strHour = GetEditValueById(kszEditHour);
    stDateTime.m_strMinute = GetEditValueById(kszEditMinute);
    stDateTime.m_strSecond = GetEditValueById(kszEditSecond);

    //设置前需设置时间模式
    g_pDateAndTimeController.SetTimeType(DATE_AND_TIME_MANUAL);
    g_pDateAndTimeController.SetDataTimeData(stDateTime);

    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_NOTE;
    stMsgInfo.strTitle = _LANG(TRID_NOTE);
    stMsgInfo.strNote =  _LANG(TRID_SAVING_CONFIG_FILE);
    stMsgInfo.nMsTime = SHORT_TIME;
    stMsgInfo.pAction = this;
    stMsgInfo.iID = MSGBOX_ID_SAVE;

    MessageBox_ShowCommonMsgBox(stMsgInfo);


}

void CDlgManualSetting::SetEditValueById(const yl::string & strID, int nValue)
{
    xEditTextView * pEdit = (xEditTextView *)getViewById(strID);
    if (NULL != pEdit)
    {
#if IF_MANUAL_SET_SUPPORT_FULL_YEAR
        if (strID == kszEditYear)
        {
            yl::string strYear = g_pDateTimeManualSettingController.GetStringByIntTime(nValue, true);
            pEdit->SetInitText(strYear);
        }
        else
#endif // endif
        {
            pEdit->SetInitText(IntTimeToStr(nValue));
        }

        pEdit->SetEnabled(m_bEditAble);
    }
}

yl::string CDlgManualSetting::GetEditValueById(const yl::string & strID)
{
    yl::string strResult = "";

    xEditTextView * pEdit = (xEditTextView *)getViewById(strID);
    if (NULL != pEdit)
    {
        strResult = pEdit->GetText();
    }

    return strResult;
}

void CDlgManualSetting::SetCommonSoftkey()
{
    CArraySoftKeyBarData vecSoftKey;

    CSoftKeyBarData objSoftkeyData;

    objSoftkeyData.m_iType = ST_BACK;
    objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_BACK);
    vecSoftKey.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_EMPTY;
    objSoftkeyData.m_strSoftkeyTitle = "";
    vecSoftKey.push_back(objSoftkeyData);

    objSoftkeyData.m_iType = ST_EMPTY;
    objSoftkeyData.m_strSoftkeyTitle = "";
    vecSoftKey.push_back(objSoftkeyData);

    if (m_bEditAble)
    {
        objSoftkeyData.m_iType = ST_SAVE;
        objSoftkeyData.m_strSoftkeyTitle = _LANG(TRID_SAVE);
        vecSoftKey.push_back(objSoftkeyData);
    }
    else
    {
        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";
        vecSoftKey.push_back(objSoftkeyData);
    }

    SetSoftKey(vecSoftKey);
}

#if IF_SUPPORT_LEFT_RIGHT_SOFTKEY
void CDlgManualSetting::SetLeftRightSoftKey()
{
    if (!m_bEditAble)
    {
        return;
    }

    CIconHotKey* pLeftKey = new CIconHotKey;
    CIconHotKey* pRightKey = new CIconHotKey;

    if (NULL == pLeftKey || NULL == pRightKey)
    {
        if (NULL != pLeftKey)
        {
            delete pLeftKey;
            pLeftKey = NULL;
        }

        if (NULL != pRightKey)
        {
            delete pRightKey;
            pRightKey = NULL;
        }

        return;
    }

    pLeftKey->SetType(ST_LEFT);
    pLeftKey->SetIndex(ST_LEFT_INDEX);
    pLeftKey->SetupPressSlot(this, static_cast<slotMethod>(&CDlgManualSetting::OnSoftKeyPress));
    pLeftKey->SetIcon(PIC_LEFT_KEY);

    pRightKey->SetType(ST_RIGHT);
    pRightKey->SetIndex(ST_RIGHT_INDEX);
    pRightKey->SetupPressSlot(this, static_cast<slotMethod>(&CDlgManualSetting::OnSoftKeyPress));
    pRightKey->SetIcon(PIC_RIGHT_KEY);

    if (!setupViewSoftKey(this, pLeftKey))
    {
        delete pLeftKey;
        pLeftKey = NULL;
    }

    if (!setupViewSoftKey(this, pRightKey))
    {
        delete pRightKey;
        pRightKey = NULL;
    }
}

void CDlgManualSetting::OnSoftKeyPress(xSignal* sender, WPARAM wParam, LPARAM lParam,
                                       int nDataBytes, LPVOID pData)
{
    if (ST_RIGHT == wParam)
    {
        OnDirectionKeyPress(PHONE_KEY_RIGHT);
    }
    else if (ST_LEFT == wParam)
    {
        OnDirectionKeyPress(PHONE_KEY_LEFT);
    }
}
#endif

bool CDlgManualSetting::IsRejectReturnToIdle()
{
#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard())
    {
        return true;
    }
#endif
    return false;
}

void CDlgManualSetting::RectifyFocusEditValue(bool bEditFinish /*= false*/)
{
    if (NULL == m_pFocusView)
    {
        return;
    }

    yl::string strId = m_pFocusView->m_strId;
    yl::string strOldValue = m_pFocusView->GetText();
    yl::string strResult = strOldValue;

    if (kszEditYear == strId)
    {
        strResult = g_pDateTimeManualSettingController.GetDateTimeShowText(DTT_YEAR,
                    strOldValue, bEditFinish);
        RectifyDayEditValue();
    }
    else if (kszEditMonth == strId)
    {
        strResult = g_pDateTimeManualSettingController.GetDateTimeShowText(DTT_MONTH,
                    strOldValue, bEditFinish);
        RectifyDayEditValue();
    }
    else if (kszEditDay == strId)
    {
        yl::string strMonth = GetEditValueById(kszEditMonth);
        yl::string strYear = GetEditValueById(kszEditYear);

        strResult = g_pDateTimeManualSettingController.GetDayShowText(strOldValue, strMonth,
                    strYear, bEditFinish);
    }
    else if (kszEditHour == strId)
    {
        strResult = g_pDateTimeManualSettingController.GetDateTimeShowText(DTT_HOUR,
                    strOldValue, bEditFinish);
    }
    else if (kszEditMinute == strId)
    {
        strResult = g_pDateTimeManualSettingController.GetDateTimeShowText(DTT_MINUTE,
                    strOldValue, bEditFinish);
    }
    else if (kszEditSecond == strId)
    {
        strResult = g_pDateTimeManualSettingController.GetDateTimeShowText(DTT_SECOND,
                    strOldValue, bEditFinish);
    }

    m_pFocusView->SetInitText(strResult);
}

void CDlgManualSetting::RectifyDayEditValue()
{
    yl::string strMonth = GetEditValueById(kszEditMonth);
    yl::string strYear = GetEditValueById(kszEditYear);
    yl::string strDay = GetEditValueById(kszEditDay);

    strDay = g_pDateTimeManualSettingController.GetDayShowText(strDay, strMonth,
             strYear, true);
    xEditTextView* pEditView = (xEditTextView*)getViewById(kszEditDay);
    if (NULL == pEditView)
    {
        return;
    }
    pEditView->SetInitText(strDay);
}

void CDlgManualSetting::slotOnTextChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    RectifyFocusEditValue(false);
}

void CDlgManualSetting::SetupEditTextChangeSlotByID(const yl::string strName)
{
    xEditTextView* pEditView = (xEditTextView*)getViewById(strName);

    if (NULL == pEditView)
    {
        return;
    }

    SLOT_CONNECT(pEditView, signalTextChanged, this, CDlgManualSetting::slotOnTextChange);
}

void CDlgManualSetting::RemoveEditTextChangeSlotByID(const yl::string strName)
{
    xEditTextView* pEditView = (xEditTextView*)getViewById(strName);

    if (NULL == pEditView)
    {
        return;
    }

    SLOT_DISCONNECT(pEditView, signalTextChanged, this, CDlgManualSetting::slotOnTextChange);
}
