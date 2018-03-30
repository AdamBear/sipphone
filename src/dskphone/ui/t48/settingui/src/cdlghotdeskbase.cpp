#include "cdlghotdeskbase.h"
#include <configiddefine.h>
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/qlineeditutility.h"
#include "configparser/modconfigparser.h"
#include "common/common_data_define.h"
#include "extended/hotdesking/include/modhotdesking.h"
#include "inputmethod/inputmethod.h"
#include "setting/src/advancedcallfeaturecontroller.h"
#include "account/include/modaccount.h"
#include "messagebox/modmessagebox.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#endif

#define PopupBoxID_REGISTERING          29281
#define PopupBoxID_REGISTERED           29282

//用于添加一个显示项控件
#define HotDesk_ADD_ITEM(szConfigName, szLabName, pItem)    if (configParser_GetConfigInt(szConfigName) == 1) \
    {\
        if (NULL == pItem)\
        {\
            pItem = new QLineEdit(this);\
        }\
        pItem->installEventFilter(this);\
        SetLineEditDefaultStyle(pItem);\
        qPair.first = szLabName;\
        qPair.second = pItem;\
        m_vecWidgetPair.push_back(qPair);\
    }

//用于删除一个显示项控件
#define HotDesk_DELETE_ITEM(p) if (NULL != p) \
    {\
        p->setParent(NULL);\
        p->deleteLater(); \
        p = NULL; \
    }

static LRESULT OnHotdeskbaseMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("CDlgHotDeskBase OnHotdeskbaseMsg message[%d] wParam[%d] lParam[%d]",
                   objMessage.message, objMessage.wParam, objMessage.lParam);

    CDlgHotDeskBase * pHotDeskBase = qobject_cast<CDlgHotDeskBase *>(UIManager_GetPrivateDlg(
                                         DLG_CDlgHotDeskBase));
    if (NULL == pHotDeskBase
            || !pHotDeskBase->IsTopPage())
    {
        return 0;
    }

    bool bGuiQuery = *((bool *)objMessage.GetExtraData());
    if (bGuiQuery)
    {
        return 0;
    }

    int iAccountID = objMessage.wParam;
    switch (objMessage.message)
    {
    case ACCOUNT_STATUS_CHANGED:
        {
            if (acc_IsAccountAvailable(iAccountID))
            {
                pHotDeskBase->ProcessRegisterResult(true);
            }
            else if (acc_GetState(iAccountID) == LS_REGISTER_FAIL
                     || acc_GetState(iAccountID) == LS_UNKNOW)
            {
                pHotDeskBase->ProcessRegisterResult(false);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

CDlgHotDeskBase::CDlgHotDeskBase(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_pEditRegisterName(NULL)
    , m_pEditUserName(NULL)
    , m_pEditPassword(NULL)
    , m_pEditSIP_Server1(NULL)
    , m_pEditSIP_Server2(NULL)
    , m_pEditOutboundProxy1(NULL)
    , m_pEditOutboundProxy2(NULL)
{
    SetWindowType(eWT_SubPage);

    if (1 == configParser_GetConfigInt(kszLogonWizardNeverTimeout))
    {
        SetRejectReturnToIdle();
    }

    m_bWaitForRegister = configParser_GetConfigInt(kszExitWizardAfterLogin) == 0;

    //标题
    m_strTitle = LANG_TRANSLATE(TRID_HOT_DESKING);

    m_pGeneralFrame->SetNote(LANG_TRANSLATE(TRID_NONE));

    this->installEventFilter(this);
}

CDlgHotDeskBase::~CDlgHotDeskBase()
{
}

void CDlgHotDeskBase::Init()
{
    if (m_bWaitForRegister)
    {
        etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnHotdeskbaseMsg);
    }

    MakeReadyForRelayout();

    QWidgetPair qPair;
    if (hotDesking_IsLogonWizard())
    {
        //有配置logon wizard的情况
        HotDesk_ADD_ITEM(kszStart_HotDeskingRegisterName, LANG_TRANSLATE(TRID_REGISTER_NAME),
                         m_pEditRegisterName);
        HotDesk_ADD_ITEM(kszStart_HotDeskingUserName, LANG_TRANSLATE(TRID_USER_NAME), m_pEditUserName);
        HotDesk_ADD_ITEM(kszStart_HotDeskingPassword, LANG_TRANSLATE(TRID_PASSWORD), m_pEditPassword);
        HotDesk_ADD_ITEM(kszStart_HotDeskingSipServer, LANG_TRANSLATE(TRID_SIP_SERVER1),
                         m_pEditSIP_Server1);
        HotDesk_ADD_ITEM(kszStart_HotDeskingSipServer, LANG_TRANSLATE(TRID_SIP_SERVER2),
                         m_pEditSIP_Server2);
        HotDesk_ADD_ITEM(kszStart_HotDeskingOutbound, LANG_TRANSLATE(TRID_OUTBOUND_PROXY1),
                         m_pEditOutboundProxy1);
        HotDesk_ADD_ITEM(kszStart_HotDeskingOutbound, LANG_TRANSLATE(TRID_OUTBOUND_PROXY2),
                         m_pEditOutboundProxy2);
    }
    else
    {
        //dsskey启动hotdesking的情况
        HotDesk_ADD_ITEM(kszHotDeskingRegisterName, LANG_TRANSLATE(TRID_REGISTER_NAME),
                         m_pEditRegisterName);
        HotDesk_ADD_ITEM(kszHotDeskingUserName, LANG_TRANSLATE(TRID_USER_NAME), m_pEditUserName);
        HotDesk_ADD_ITEM(kszHotDeskingPassword, LANG_TRANSLATE(TRID_PASSWORD), m_pEditPassword);
        HotDesk_ADD_ITEM(kszHotDeskingSipServer, LANG_TRANSLATE(TRID_SIP_SERVER1), m_pEditSIP_Server1);
        HotDesk_ADD_ITEM(kszHotDeskingSipServer, LANG_TRANSLATE(TRID_SIP_SERVER2), m_pEditSIP_Server2);
        HotDesk_ADD_ITEM(kszHotDeskingOutbound, LANG_TRANSLATE(TRID_OUTBOUND_PROXY1),
                         m_pEditOutboundProxy1);
        HotDesk_ADD_ITEM(kszHotDeskingOutbound, LANG_TRANSLATE(TRID_OUTBOUND_PROXY2),
                         m_pEditOutboundProxy2);
    }

    //register name
    if (NULL != m_pEditRegisterName)
    {
        qLineEditUtility::SetMaxLength(m_pEditRegisterName, EDIT_TYPE_ACCOUNT_NAME);
#if IF_FEATURE_AUTOP_IME
        qInstallIME(m_pEditRegisterName, IME_Name);
        yl::string strRegisterName = commonUnits_GetIMEByType(kszHotDeskingRegisterIME);
        if (!strRegisterName.empty())
        {
            qSetCurrentIME(m_pEditRegisterName, strRegisterName.c_str(), 0, kszHotDeskingRegisterIME);
        }
#else
        qInstallIME(m_pEditRegisterName, IME_Name);
#endif
    }
    //user name
    if (NULL != m_pEditUserName)
    {
        qLineEditUtility::SetMaxLength(m_pEditUserName, EDIT_TYPE_PHONE_NUMBER);
#if IF_FEATURE_AUTOP_IME
        qInstallIME(m_pEditUserName, IME_Name);
        yl::string strUserName = commonUnits_GetIMEByType(kszHotDeskingUserNameIME);
        if (!strUserName.empty())
        {
            qSetCurrentIME(m_pEditUserName, strUserName.c_str(), 0, kszHotDeskingUserNameIME);
        }
#else
        qInstallIME(m_pEditUserName, IME_Name);
#endif
    }
    //password
    if (NULL != m_pEditPassword)
    {
        m_pEditPassword->setEchoMode(QLineEdit::Password);
        qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);

        qInstallIME(m_pEditPassword, IME_PWD);

#if IF_FEATURE_AUTOP_IME
        yl::string strPassword = commonUnits_GetIMEByType(kszHotDeskingPasswordIME);
        if (!strPassword.empty())
        {
            qSetCurrentIME(m_pEditPassword, strPassword.c_str(), 0, kszHotDeskingPasswordIME);
        }
#endif
    }
    //sip server1
    if (NULL != m_pEditSIP_Server1)
    {
        qLineEditUtility::SetMaxLength(m_pEditSIP_Server1, EDIT_TYPE_SIP_SERVER);
        qInstallIME(m_pEditSIP_Server1, IME_ALL);
        qLineEditUtility::SetCurrentInputMethod(m_pEditSIP_Server1, EDIT_TYPE_SIP_SERVER);
    }
    //sip server2
    if (NULL != m_pEditSIP_Server2)
    {
        qLineEditUtility::SetMaxLength(m_pEditSIP_Server2, EDIT_TYPE_SIP_SERVER);
        qInstallIME(m_pEditSIP_Server2, IME_ALL);
        qLineEditUtility::SetCurrentInputMethod(m_pEditSIP_Server2, EDIT_TYPE_SIP_SERVER);
    }
    //outbound1
    if (NULL != m_pEditOutboundProxy1)
    {
        qLineEditUtility::SetMaxLength(m_pEditOutboundProxy1, EDIT_TYPE_PROXY);
        qInstallIME(m_pEditOutboundProxy1, IME_ALL);
        qLineEditUtility::SetCurrentInputMethod(m_pEditOutboundProxy1, EDIT_TYPE_SIP_SERVER);
    }
    //outbound2
    if (NULL != m_pEditOutboundProxy2)
    {
        qLineEditUtility::SetMaxLength(m_pEditOutboundProxy2, EDIT_TYPE_PROXY);
        qInstallIME(m_pEditOutboundProxy2, IME_ALL);
        qLineEditUtility::SetCurrentInputMethod(m_pEditOutboundProxy2, EDIT_TYPE_SIP_SERVER);
    }

    AddWidget(m_vecWidgetPair);
}

void CDlgHotDeskBase::Uninit()
{
    if (m_bWaitForRegister)
    {
        etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnHotdeskbaseMsg);
    }

    UninstallAllIME();
    if (NULL != m_pGeneralFrame)
    {
        MakeReadyForRelayout();
    }
    HotDesk_DELETE_ITEM(m_pEditRegisterName);
    HotDesk_DELETE_ITEM(m_pEditUserName);
    HotDesk_DELETE_ITEM(m_pEditPassword);
    HotDesk_DELETE_ITEM(m_pEditSIP_Server1);
    HotDesk_DELETE_ITEM(m_pEditSIP_Server2);
    HotDesk_DELETE_ITEM(m_pEditOutboundProxy1);
    HotDesk_DELETE_ITEM(m_pEditOutboundProxy2);
}

//通过控制获取已有配置数据，通过V层在界面上显示出来
bool CDlgHotDeskBase::LoadPageData()
{

    if (NULL != m_pListDelegate && NULL != m_pGeneralFrame)
    {
        m_pListDelegate->RefreshFocus();
    }
    else
    {
        this->setFocus();
    }

    return true;
}

//通过控制获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgHotDeskBase::IsStatusChanged()
{
    return true;
}

//读取当前界面的配置，通过C层保存
bool CDlgHotDeskBase::SavePageData()
{
    HotDeskBaseData currentPageData;
    GetCurrentPageData(currentPageData);

    //调用控制层：保存数据
    if (NULL != g_pAdvancedCallFeatureController)
    {
        g_pAdvancedCallFeatureController->SetHotDeskBaseData(currentPageData);
    }

    if (m_bWaitForRegister)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_REGISTERING_PLEASE_WAIT)
                                    , MESSAGEBOX_NOTE, 10 * 1000,
                                    "", "", PopupBoxID_REGISTERING);
        return false;
    }

    return true;
}

// 对输入框的数据进行验证
bool CDlgHotDeskBase::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //register name
    if (NULL != m_pEditRegisterName)
    {
        if (m_pEditRegisterName->text().isEmpty())
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_REGISTER_NAME_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditRegisterName;
            return false;
        }
    }

    //user name
    if (NULL != m_pEditUserName)
    {
        if (m_pEditUserName->text().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditUserName->text(), EDIT_TYPE_ACCOUNT_NAME))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_USER_NAME_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditUserName;
            return false;
        }
    }
    //password
    if (NULL != m_pEditPassword)
    {
        if ((m_pEditPassword->text().isEmpty() && configParser_GetConfigInt(kszEnablePwdEmpty) == 0)
                || (!m_pEditPassword->text().isEmpty()
                    && !qLineEditUtility::IsFormatValid(m_pEditPassword->text(), EDIT_TYPE_PASSWORD)))
        {
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PASSWORD_IS_INVALID);
            objErrorContent.pLineEdit = m_pEditPassword;
            return false;
        }
    }
    //sip server
    if (NULL != m_pEditSIP_Server1
            && NULL != m_pEditSIP_Server2)
    {
        if (m_pEditSIP_Server1->text().isEmpty() && m_pEditSIP_Server2->text().isEmpty())
        {
            objErrorContent.strErrorContent = TRID_SIP_PROXY_CANNOT_BE_EMPTY;
            objErrorContent.pLineEdit = m_pEditSIP_Server1;
            return false;
        }
        else if (!m_pEditSIP_Server1->text().isEmpty()
                 && !qLineEditUtility::IsFormatValid(m_pEditSIP_Server1->text(), EDIT_TYPE_SIP_SERVER))
        {
            objErrorContent.strErrorContent = TRID_SIP_SERVER1_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditSIP_Server1;
            return false;
        }
        else if (!m_pEditSIP_Server2->text().isEmpty()
                 && !qLineEditUtility::IsFormatValid(m_pEditSIP_Server2->text(), EDIT_TYPE_SIP_SERVER))
        {
            objErrorContent.strErrorContent = TRID_SIP_SERVER2_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditSIP_Server2;
            return false;
        }
    }

    return true;
}

bool CDlgHotDeskBase::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_pGeneralFrame != NULL
            && m_pGeneralFrame->GetTotalDataCount() == 0
            && objWndData.size() > 3)
    {
        objWndData[3].m_strSoftkeyAction.clear();
        objWndData[3].m_strSoftkeyTitle.clear();
        return true;
    }

    return false;
}

//取得当前页面数据
void CDlgHotDeskBase::GetCurrentPageData(HotDeskBaseData & currentPageData)
{
    //register name
    if (NULL != m_pEditRegisterName)
    {
        currentPageData.m_strRegisterName = m_pEditRegisterName->text().trimmed().toUtf8().data();
    }

    //user name
    if (NULL != m_pEditUserName)
    {
        currentPageData.m_strUserName = m_pEditUserName->text().trimmed().toUtf8().data();
    }

    //sip server
    if (NULL != m_pEditSIP_Server1
            && NULL != m_pEditSIP_Server2)
    {
        currentPageData.m_strSipServer1 =   m_pEditSIP_Server1->text().trimmed().toUtf8().data();
        currentPageData.m_strSipServer2 =   m_pEditSIP_Server2->text().trimmed().toUtf8().data();
    }

    //password
    if (NULL != m_pEditPassword)
    {
        currentPageData.m_strPassword = m_pEditPassword->text().trimmed().toUtf8().data();
    }

    //outbound
    if (NULL != m_pEditOutboundProxy1
            && NULL != m_pEditOutboundProxy2)
    {
        currentPageData.m_strOutboundProxy1 =   m_pEditOutboundProxy1->text().trimmed().toUtf8().data();
        currentPageData.m_strOutboundProxy2 =   m_pEditOutboundProxy2->text().trimmed().toUtf8().data();
        currentPageData.m_bOutboundEnable = !currentPageData.m_strOutboundProxy1.empty()
                                            || !currentPageData.m_strOutboundProxy2.empty();
    }
}

// 卸载界面上所有的输入法
void CDlgHotDeskBase::UninstallAllIME()
{
    qUninstallIME(m_pEditRegisterName);
    qUninstallIME(m_pEditUserName);
    qUninstallIME(m_pEditPassword);
    qUninstallIME(m_pEditSIP_Server1);
    qUninstallIME(m_pEditSIP_Server2);
    qUninstallIME(m_pEditOutboundProxy1);
    qUninstallIME(m_pEditOutboundProxy2);
}
void CDlgHotDeskBase::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (pMessageBox == NULL)
    {
        return;
    }

    SETTINGUI_INFO("CDlgHotDeskBase::MessageBoxCallBack Popbox ID %d ,Result is %d.",
                   pMessageBox->GetID(), pMessageBox->GetResult());
    switch (pMessageBox->GetID())
    {
    case PopupBoxID_REGISTERING:
        {
            if (pMessageBox->GetResult() == MessageBox_Cancel)
            {
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_CONNECT_TIMEOUT_TRY_AGAIN),
                                            MESSAGEBOX_NOTE, 2 * 1000);
            }
        }
        break;
    case PopupBoxID_REGISTERED:
        {
            SetEnableReturnToIdle();
            //登录成功返回IDLE
            UIManager_ReturnToIdle();
        }
        break;
    default:
        break;
    }
}

void CDlgHotDeskBase::ProcessRegisterResult(bool bRegistered)
{
    MessageBox_RemoveMessageBox(this, MessageBox_Destory, PopupBoxID_REGISTERING);
    if (bRegistered)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_REGISTERING_SUCCED),
                                    MESSAGEBOX_NOTE, 2 * 1000,
                                    "", "", PopupBoxID_REGISTERED);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_REGISTERING_FAILED),
                                    MESSAGEBOX_NOTE, 2 * 1000);
    }
}
