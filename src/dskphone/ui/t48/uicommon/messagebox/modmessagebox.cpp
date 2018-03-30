#include "modmessagebox.h"


#include "commonmessagebox.h"
#include "passwordmessagebox.h"
#include "xsipasswordbox.h"
#include "selectmessagebox.h"
#include "imageselectbox.h"
#include "bluetoothbox.h"
#include "wifibox.h"
#include "titlemenubox.h"
#include "ldappasswordbox.h"

#include "setting/include/modsetting.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "mainwnd/mainwnd.h"
#include "dsklog/log.h"
#include "keyguard/include/modkeyguard.h"
#include "commonapi/systemhelperapi.h"
#include "inputmethod/inputmethod.h"

#include "directoryui/include/moddirectoryui.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "feature/include/modfeature.h"
#include "account/include/modaccount.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#endif

#if IF_FEATURE_METASWITCH_ACD
#include "mtswacdlistbox.h"
#endif
#if IF_FEATURE_METASWITCH
#include "mtswcommportalbox.h"
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "mtswvoicemailbox.h"
#endif

void MessageBox_ShowCommonMsgBox(CMsgBoxCallBackAction * pAction,
                                 const QString & strNote,
                                 MESSAGEBOX_TYPE eType,
                                 int nMsTime/* = -1*/,
                                 const QString & strCancelBtnTitle/* = ""*/,
                                 const QString & strOkBtnTitle/* = ""*/,
                                 int iID/* = -1*/,
                                 const QVariant & extraData/* = QVariant()*/,
                                 bool bShowCountDown/* = false*/,
                                 MESSAGEBOX_RESULT eTimeResult/* = MessageBox_Cancel*/)
{
    if (eType != MESSAGEBOX_NOTE
            && eType != MESSAGEBOX_Cancel
            && eType != MESSAGEBOX_Ok
#if IF_BUG_30016
            && eType != MESSAGEBOX_GPICKUP
#endif
            && eType != MESSAGEBOX_OkCancel
            && eType != MESSAGEBOX_OkBlock)
//      && eType != MESSAGEBOX_VoiceMail
//      && eType != MESSAGEBOX_DiscardSave)
    {
        POPUPBOX_WARN("MessageBox_ShowCommonMsgBox eType error eType[%d]", eType);
        return;
    }

    POPUPBOX_WARN("MessageBox_ShowCommonMsgBox eType[%d]", eType);

    CCommonMessageBox * pCommonMsgBox = new CCommonMessageBox(_MainWND.GetMaskFrame(), bShowCountDown);

    if (NULL != pCommonMsgBox)
    {
        pCommonMsgBox->SetType(eType);
        pCommonMsgBox->SetTimeoutResult(eTimeResult);
        pCommonMsgBox->SetMessageBoxCommonInfo(strNote, nMsTime, iID);

        VecMsgBoxSoftkey vecSoftkey;
        switch (eType)
        {
        case MESSAGEBOX_NOTE:
        {
            //note类型不需要按钮
        }
        break;
        case MESSAGEBOX_Cancel:
        {
            MsgBoxSoftkey softkey;

            //cancel按钮
            softkey.m_result = MessageBox_Cancel;
            if (!strCancelBtnTitle.isEmpty())
            {
                softkey.m_title = strCancelBtnTitle;
            }
            else
            {
                softkey.m_title = LANG_TRANSLATE(TRID_CANCEL);
            }
            vecSoftkey.push_back(softkey);
        }
        break;
        case MESSAGEBOX_Ok:
        {
            MsgBoxSoftkey softkey;

            //cancel按钮
            softkey.m_result = MessageBox_OK;
            if (!strCancelBtnTitle.isEmpty())
            {
                softkey.m_title = strOkBtnTitle;
            }
            else
            {
                softkey.m_title = LANG_TRANSLATE(TRID_OK);
            }
            vecSoftkey.push_back(softkey);
        }
        break;
        case MESSAGEBOX_OkCancel:
        {
            MsgBoxSoftkey softkey;

            //cancel按钮
            softkey.m_result = MessageBox_Cancel;
            if (!strCancelBtnTitle.isEmpty())
            {
                softkey.m_title = strCancelBtnTitle;
            }
            else
            {
                softkey.m_title = LANG_TRANSLATE(TRID_CANCEL);
            }
            vecSoftkey.push_back(softkey);

            //ok按钮
            softkey.m_result = MessageBox_OK;
            if (!strOkBtnTitle.isEmpty())
            {
                softkey.m_title = strOkBtnTitle;
            }
            else
            {
                softkey.m_title = LANG_TRANSLATE(TRID_OK);
            }
            vecSoftkey.push_back(softkey);
        }
        break;
        case MESSAGEBOX_OkBlock:
        {
            MsgBoxSoftkey softkey;

            //cancel按钮
            softkey.m_result = MessageBox_Cancel;
            if (!strCancelBtnTitle.isEmpty())
            {
                softkey.m_title = strCancelBtnTitle;
            }
            else
            {
                softkey.m_title = LANG_TRANSLATE(TRID_CANCEL);
            }
            vecSoftkey.push_back(softkey);

            //ok按钮
            softkey.m_result = MessageBox_OK;
            if (!strOkBtnTitle.isEmpty())
            {
                softkey.m_title = strOkBtnTitle;
            }
            else
            {
                softkey.m_title = LANG_TRANSLATE(TRID_BLOCK);
            }
            vecSoftkey.push_back(softkey);
        }
        break;
        }

        pCommonMsgBox->SetMessageBoxSpecialInfo(vecSoftkey);
        pCommonMsgBox->SetExtraData(extraData);

        MessageBox_ShowMessageBox(pAction, pCommonMsgBox);
    }
}

void MessageBox_ShowCommonMsgBox(CMsgBoxCallBackAction * pAction,
                                 const QString & strNote,
                                 VecMsgBoxSoftkey & vecSoftkey,
                                 int nMsTime/* = -1*/,
                                 int iID/* = -1*/,
                                 const QVariant & extraData/* = QVariant()*/,
                                 bool bShowCountDown/* = false*/,
                                 MESSAGEBOX_RESULT eTimeResult/* = MessageBox_Cancel*/)
{
    POPUPBOX_WARN("MessageBox_ShowCommonMsgBox Type Special");

    CCommonMessageBox * pCommonMsgBox = new CCommonMessageBox(_MainWND.GetMaskFrame(), bShowCountDown);

    if (NULL != pCommonMsgBox)
    {
        pCommonMsgBox->SetType(MESSAGEBOX_SPECIAL);
        pCommonMsgBox->SetTimeoutResult(eTimeResult);
        pCommonMsgBox->SetMessageBoxCommonInfo(strNote, nMsTime, iID);

        pCommonMsgBox->SetMessageBoxSpecialInfo(vecSoftkey);
        pCommonMsgBox->SetExtraData(extraData);

        MessageBox_ShowMessageBox(pAction, pCommonMsgBox);
    }
}

void MessageBox_ShowMessageBox(CMsgBoxCallBackAction * pAction,
                               CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    _MessageBoxManager.AddMessageBox(pMessageBox, pAction);
}

void MessageBox_ShowPasswordBox(CMsgBoxCallBackAction * pAction,
                                const QString & strUserName,
                                const QVariant & extraData/* = QVariant()*/,
                                int iID/* = -1*/)
{
    //显示“admin password”
    QString strNode = strUserName;
    //LANG_TRANSLATE查找不到对应项就会返回原串
    strNode = LANG_TRANSLATE(strNode);
    strNode += " ";
    strNode += LANG_TRANSLATE(TRID_PASSWORD);
    CPasswordMessageBox * pPasswordMsgBox = new CPasswordMessageBox(_MainWND.GetMaskFrame(),
            LANG_TRANSLATE(strNode),
            LANG_TRANSLATE(TRID_PASSWORD_IS_WRONG),
            "",
            strUserName);
    if (NULL != pPasswordMsgBox)
    {
        pPasswordMsgBox->SetID(iID);
        pPasswordMsgBox->SetType(MESSAGEBOX_PWD);          // 设置密码框属性
#if IF_FEATURE_AUTOP_IME
        yl::string strPasswordIME = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
        if (!strPasswordIME.empty())
        {
            pPasswordMsgBox->SetDefaultIME(strPasswordIME.c_str(), kszAdvancedSettingPasswordIME);
        }
        else
        {
            pPasswordMsgBox->SetDefaultIME(IME_abc_Pwd, kszAdvancedSettingPasswordIME);// 设置默认的输入法
        }
#else
        pPasswordMsgBox->SetDefaultIME(IME_abc_Pwd, kszAdvancedSettingPasswordIME);// 设置默认的输入法
#endif
        pPasswordMsgBox->SetExtraData(extraData);
        _MessageBoxManager.AddMessageBox(pPasswordMsgBox, pAction);
    }
}

void MessageBox_ShowUnlockBox(CMsgBoxCallBackAction * pAction,
                              int iID/* = -1*/)
{
    yl::string strUser = feature_UserModeGetUserName(feature_UserModeGetCurrType());
    yl::string strUserDisplayName = Setting_GetUserDisplayName(strUser);
    // 弹出解锁框
    CPasswordMessageBox * pUnlockdBox = new CPasswordMessageBox(_MainWND.GetMaskFrame(),
            /////////////////////////////////翻译
            LANG_TRANSLATE(TRID_PLEASE_ENTER_UNLOCK_PIN),
            LANG_TRANSLATE(TRID_CURRENT_PIN_IS_INCORRECT),
            keyGuard_GetUnlockPIN().c_str(),
            toQString(strUser.c_str()));

    if (NULL != pUnlockdBox)
    {
        pUnlockdBox->SetID(iID);
        pUnlockdBox->SetType(MESSAGEBOX_UNLOCK);           // 设置解锁框属性
        pUnlockdBox->MySetExitTime(AUTO_EXIT_TIME);
#if IF_FEATURE_AUTOP_IME
        yl::string strIME = commonUnits_GetIMEByType(kszUnlockPinIME);
        if (!strIME.empty())
        {
            pUnlockdBox->SetDefaultIME(strIME.c_str(), kszUnlockPinIME);// 设置默认的输入法
        }
#else
        pUnlockdBox->SetDefaultIME(IME_123_Pwd, kszUnlockPinIME);// 设置默认的输入法
#endif
        _MessageBoxManager.AddMessageBox(pUnlockdBox, pAction);
    }
}

#if IF_FEATURE_METASWITCH
void MessageBox_ShowMTSWCommportalBox(CMsgBoxCallBackAction * pAction,
                                      int iID /*= -1*/)
{
    // 创建弹出框
    CMTSWCommportalBox * pDlgPwdBox = new CMTSWCommportalBox(_MainWND.GetMaskFrame());

    if (NULL != pDlgPwdBox)
    {
        pDlgPwdBox->SetID(iID);
        pDlgPwdBox->SetType(MESSAGEBOX_MTSW_COMMPORTAL);

        //提交验证密码由回调函数完成

        _MessageBoxManager.AddMessageBox(pDlgPwdBox, pAction);
    }
}
#endif


void MessageBox_ShowXSIPasswordBox(CMsgBoxCallBackAction * pAction,
                                   const QString & strUserTitle,
                                   const QString & strUserName,
                                   const QString & strPasswordTitle,
                                   int iID/* = -1*/)
{
    // 创建弹出框
    CXSIPasswordBox * pDlgXSIPwdBox = new CXSIPasswordBox(_MainWND.GetMaskFrame());

    if (NULL != pDlgXSIPwdBox)
    {
        pDlgXSIPwdBox->SetID(iID);
        pDlgXSIPwdBox->SetType(MESSAGEBOX_BSFT_PWD);

        pDlgXSIPwdBox->SetUserStr(strUserTitle, strUserName, strPasswordTitle);

        //提交验证密码由回调函数完成

        _MessageBoxManager.AddMessageBox(pDlgXSIPwdBox, pAction);
    }
}

#if IF_FEATURE_34162
void MessageBox_ShowLdapPasswordBox(CMsgBoxCallBackAction * pAction,
                                    const QString & strUserName,
                                    const QString & strPassword,
                                    int iID/* = -1*/)
{
    // 创建弹出框
    CLdapPasswordBox * pDlgLdapPwdBox = new CLdapPasswordBox(_MainWND.GetMaskFrame());

    if (NULL != pDlgLdapPwdBox)
    {
        pDlgLdapPwdBox->SetID(iID);
        pDlgLdapPwdBox->SetType(MESSAGEBOX_LDAP_PWD);

        //提交验证密码由回调函数完成

        _MessageBoxManager.AddMessageBox(pDlgLdapPwdBox, pAction);
    }
}
#endif

CMessageBoxBase * MessageBox_ShowSelectBox(CMsgBoxCallBackAction * pAction,
        VecMsgBoxSoftkey & vecSoftkey,
        int nSelectBoxType,
        IFrameListDelegate * pDelegate,
        const QString & strTitle,
        int iID /* = -1 */)
{
    CSelectMessageBox * pSelectBox = new CSelectMessageBox(nSelectBoxType, pDelegate,
            _MainWND.GetMaskFrame());

    if (NULL != pSelectBox)
    {
        pSelectBox->SetID(iID);
        pSelectBox->SetType(MESSAGEBOX_SELECT);

        pSelectBox->Init(strTitle);
        pSelectBox->SetMessageBoxSpecialInfo(vecSoftkey);

        _MessageBoxManager.AddMessageBox(pSelectBox, pAction);
    }

    return pSelectBox;
}

void MessageBox_ShowImageSelectBox(CMsgBoxCallBackAction * pAction,
                                   int nRowCount,
                                   IFrameListDelegate * pDelegate,
                                   int iID /* = -1 */)
{
    CImageSelectBox * pImageSelectBox = new CImageSelectBox(nRowCount, pDelegate,
            _MainWND.GetMaskFrame());

    if (NULL != pImageSelectBox)
    {
        pImageSelectBox->SetID(iID);
        pImageSelectBox->SetType(MESSAGEBOX_IMAGE_SELECT);

        _MessageBoxManager.AddMessageBox(pImageSelectBox, pAction);
    }
}

void MessageBox_ShowOptionBox(CMsgBoxCallBackAction * pAction
                              , const vecOptionMsgBoxItem & vecOptionItem
                              , int iID /*= -1 */
                              , const QVariant & extraData /*= QVariant()*/
                              , bool bDefaultFocus/* = false*/
                              , int nDefaultIndex/* = -1*/
                              , int nTimeTodie/* = 0*/)
{
    COptionMessageBox * pOptionBox = new COptionMessageBox(_MainWND.GetMaskFrame());
    if (NULL != pOptionBox)
    {
        pOptionBox->SetID(iID);
        pOptionBox->SetType(MESSAGEBOX_OPTION);
        pOptionBox->SetExtraData(extraData);

        if (nTimeTodie > 0)
        {
            pOptionBox->SetTimeToIdle(nTimeTodie);
        }

        pOptionBox->Init(vecOptionItem);

        pOptionBox->SetDefaultFocus(bDefaultFocus, nDefaultIndex);

        _MessageBoxManager.AddMessageBox(pOptionBox, pAction);
    }
}
void MessageBox_ShowBluetoothBox(CMsgBoxCallBackAction * pAction,
                                 const QString & lpszText,
                                 bool bAutoConnect,
                                 int nMsTime /* = -1 */,
                                 int iID /* = -1 */)
{
    CBluetoothBox * pBluetoothBox = new CBluetoothBox(_MainWND.GetMaskFrame());

    if (NULL != pBluetoothBox)
    {
        pBluetoothBox->SetID(iID);
        pBluetoothBox->SetType(MESSAGEBOX_BLUETOOTH);
        pBluetoothBox->SetAutoConnect(bAutoConnect);
        pBluetoothBox->SetMessageBoxInfo(lpszText, nMsTime);

        _MessageBoxManager.AddMessageBox(pBluetoothBox, pAction);
    }
}

// Wi-Fi password box
void MessageBox_ShowWiFiPasswordBox(CMsgBoxCallBackAction * pAction,
                                    const QString & strName,
                                    const QVariant & extraData /*= QVariant()*/,
                                    int iID /*= -1*/)
{
    CPasswordMessageBox * pPasswordMsgBox = new CPasswordMessageBox(_MainWND.GetMaskFrame(),
            LANG_TRANSLATE(strName),
            LANG_TRANSLATE(TRID_PASSWORD_IS_WRONG),
            "",
            strName);
    if (NULL != pPasswordMsgBox)
    {
        pPasswordMsgBox->SetID(iID);
        pPasswordMsgBox->SetType(MESSAGEBOX_WIFIDETAIL);           // 设置密码框属性
        pPasswordMsgBox->SetDefaultIME(IME_123_Pwd);// 设置默认的输入法
        pPasswordMsgBox->SetExtraData(extraData);

        qLineEditUtility::SetMaxLength(pPasswordMsgBox->GetLineEdit(), EDIT_TYPE_WIFI_PWD);

        _MessageBoxManager.AddMessageBox(pPasswordMsgBox, pAction);
    }
}

// Wi-Fi detail box
void MessageBox_ShowWifiDetailBox(CMsgBoxCallBackAction * pAction,
                                  const QString & lpszText,
                                  int nMsTime /* = -1 */,
                                  int iID /* = -1 */)
{
    CWifiDetailBox * pWifiDetailBox = new CWifiDetailBox(_MainWND.GetMaskFrame());

    if (NULL != pWifiDetailBox)
    {
        pWifiDetailBox->SetID(iID);
        pWifiDetailBox->SetType(MESSAGEBOX_WIFIDETAIL);
        pWifiDetailBox->SetMessageBoxInfo(lpszText, nMsTime);

        _MessageBoxManager.AddMessageBox(pWifiDetailBox, pAction);
    }
}

void MessageBox_ShowTitleMenuBox(CMsgBoxCallBackAction * pAction,
                                 IPopupMenuDelegate * pDelegate,
                                 const QPoint & ptMenu,
                                 int iID /* = -1 */)
{
    CTitleMenuBox * pTitleMenu = new CTitleMenuBox(_MainWND.GetMaskFrame(), pDelegate, ptMenu);

    if (NULL != pTitleMenu)
    {
        pTitleMenu->SetID(iID);
        pTitleMenu->SetType(MESSAGEBOX_LINE_SELECT);
        _MessageBoxManager.AddMessageBox(pTitleMenu, pAction);
    }
}

CMessageBoxBase * MessageBox_GetTopMessageBox()
{
    return _MessageBoxManager.GetTopMessageBox();
}

void MessageBox_ShowEDKInputBox(CMsgBoxCallBackAction * pAction
                                , int nMaxInput
                                , const QString & strHint
                                , MESSAGEBOX_DEFAULT_INPUT_TYPE nDefalutInputType
                                , bool bIsMaskFeedback
                                , int iID
                                , const QVariant & extraData)
{
    QString strTitle = LANG_TRANSLATE(TRID_EDK_PROMPT);
    if (!strHint.isEmpty())
    {
        strTitle = strHint;
    }

    CPasswordMessageBox * pEDKBox = new CPasswordMessageBox(_MainWND.GetMaskFrame()
            , strTitle
            , nMaxInput);

    if (NULL != pEDKBox)
    {
        pEDKBox->SetID(iID);
        // 设置解锁框类型
        pEDKBox->SetType(MESSAGEBOX_EDK);
        pEDKBox->MySetExitTime(AUTO_EXIT_TIME);

        pEDKBox->SetExtraData(extraData);

        //设置默认的输入法
        switch (nDefalutInputType)
        {
        case MessageBox_Input_Type_abc:
        {
            pEDKBox->SetDefaultIME(IME_abc_Pwd);
        }
        break;
        case MessageBox_Input_Type_123:
        {
            pEDKBox->SetDefaultIME(IME_123_Pwd);
        }
        break;
        default:
        {
            pEDKBox->SetDefaultIME(IME_123_Pwd);
        }
        break;
        }

        //设置是否用密码方式显示输入的内容，true则显示为*，false则不mask，即显示原输入内容
        pEDKBox->SetFeedBackShowType(bIsMaskFeedback);

        _MessageBoxManager.AddMessageBox(pEDKBox, pAction);
    }
}

bool MessageBox_RemoveMessageBox(CMsgBoxCallBackAction * pAction,
                                 MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/,
                                 int iID/* = -1*/)
{
    POPUPBOX_INFO("MessageBox RemoveMessageBox");
    if (NULL == pAction)
    {
        POPUPBOX_WARN("CMsgBoxCallBackAction is null");
        return false;
    }

    return _MessageBoxManager.RemoveMessageBox(pAction, eResult, true, iID);
}

bool MessageBox_RemoveTopMessageBox(MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/)
{
    return _MessageBoxManager.RemoveTopMessageBox(eResult);
}

bool MessageBox_RemoveAllMessageBox(MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/)
{
    return _MessageBoxManager.RemoveAllMessageBox(eResult);
}

int MessageBox_GetMessageBoxCount()
{
    return _MessageBoxManager.GetMessageBoxCount();
}

void MessageBox_ShowGuestUnlockBox(CMsgBoxCallBackAction * pAction,
                                   int iID/* = -1*/)
{
    yl::string strUser = feature_UserModeGetUserName(feature_UserModeGetCurrType());
    QString strPwd = toQString(configParser_GetCfgItemStringValue(kszAccountHotelingUnlockPin,
                               acc_GetHetlingAccountID()));
    // 弹出解锁框
    CPasswordMessageBox * pUnlockdBox = new CPasswordMessageBox(_MainWND.GetMaskFrame(),
            LANG_TRANSLATE(TRID_PLEASE_ENTER_UNLOCK_PIN),
            LANG_TRANSLATE(TRID_UNLOCK_PIN_INCORRECT),
            strPwd,
            toQString(strUser.c_str()),
            MAX_LEN_HOTELING_PASSWORD);

    if (NULL != pUnlockdBox)
    {
        //http://10.2.1.199/Bug.php?BugID=92370
        pUnlockdBox->SetIme(IME_123);
        pUnlockdBox->SetID(iID);
        pUnlockdBox->SetType(MESSAGEBOX_GUEST_UNLOCK);         // 设置解锁框属性
        pUnlockdBox->MySetExitTime(AUTO_EXIT_TIME);
#if IF_FEATURE_AUTOP_IME
        yl::string strIME = commonUnits_GetIMEByType(kszUnlockPinIME);
        if (!strIME.empty())
        {
            pUnlockdBox->SetDefaultIME(strIME.c_str(), kszUnlockPinIME);// 设置默认的输入法
        }
#else
        pUnlockdBox->SetDefaultIME(EDIT_TYPE_NUMBER_PWD, kszUnlockPinIME);// 设置默认的输入法
#endif
        _MessageBoxManager.AddMessageBox(pUnlockdBox, pAction);
    }
}

void MessageBox_ShowDsskeyLockPasswordBox(CMsgBoxCallBackAction * pAction,
        const QVariant & extraData/* = QVariant()*/,
        int iID/* = -1*/)
{
    QString strTitle = toQString(feature_UserModeGetUserName(UT_ADMIN));
    strTitle = LANG_TRANSLATE(strTitle);
    strTitle += " ";
    strTitle += LANG_TRANSLATE(TRID_PASSWORD);
    QString strPwd = toQString(configParser_GetConfigString(kszDsskeyLockPassword));
    CPasswordMessageBox * pPasswordMsgBox = new CPasswordMessageBox(_MainWND.GetMaskFrame(),
            LANG_TRANSLATE(strTitle),
            LANG_TRANSLATE(TRID_PASSWORD_IS_WRONG),
            strPwd,
            strTitle);

    if (NULL != pPasswordMsgBox)
    {
        pPasswordMsgBox->SetID(iID);
        pPasswordMsgBox->SetType(MESSAGEBOX_GUEST_UNLOCK);
        pPasswordMsgBox->MySetExitTime(AUTO_EXIT_TIME);
#if IF_FEATURE_AUTOP_IME
        yl::string strIME = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
        if (!strIME.empty())
        {
            pPasswordMsgBox->SetDefaultIME(strIME.c_str(), kszAdvancedSettingPasswordIME);
        }
#else
        pPasswordMsgBox->SetDefaultIME(IME_abc_Pwd, kszAdvancedSettingPasswordIME);
#endif
        pPasswordMsgBox->SetExtraData(extraData);
        _MessageBoxManager.AddMessageBox(pPasswordMsgBox, pAction);
    }
}

#if IF_FEATURE_METASWITCH_ACD
void MessageBox_ShowMTSWAcdListBox(CMsgBoxCallBackAction * pAction,
                                   const QString & strMlghName,
                                   EMTSWAcdListShowLoggedMode eShowLoggedMode,
                                   int iID /* = -1 */)
{
    CMTSWAcdListBox * pAcdListBox = new CMTSWAcdListBox(_MainWND.GetMaskFrame());

    if (NULL != pAcdListBox)
    {
        pAcdListBox->SetID(iID);
        pAcdListBox->SetLoggedBtnMode(eShowLoggedMode);
        pAcdListBox->SetType(MESSAGEBOX_MTSWACDLIST);
        pAcdListBox->SetMessageBoxInfo(strMlghName, -1);

        _MessageBoxManager.AddMessageBox(pAcdListBox, pAction);
    }
}
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
void MessageBox_ShowMTSWVoiceMailOptionBox(CMsgBoxCallBackAction * pAction,
        const QString & lpszText,
        bool bRead,
        bool bAddToNetwork,
        int nMsTime/* = -1*/,
        int iID/* = -1*/)
{
    CMTSWVoiceMailBox * pMTSWVoiceMailBox = new CMTSWVoiceMailBox(_MainWND.GetMaskFrame());

    if (NULL != pMTSWVoiceMailBox)
    {
        pMTSWVoiceMailBox->SetID(iID);
        pMTSWVoiceMailBox->SetType(MESSAGEBOX_MTSW_VOICEMAIL);
        pMTSWVoiceMailBox->SetRead(bRead);
        pMTSWVoiceMailBox->SetAddToNetwork(bAddToNetwork);
        pMTSWVoiceMailBox->SetMessageBoxInfo(lpszText, nMsTime);

        _MessageBoxManager.AddMessageBox(pMTSWVoiceMailBox, pAction);
    }
}
#endif
