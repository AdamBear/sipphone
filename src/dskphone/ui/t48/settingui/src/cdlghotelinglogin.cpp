/************************************************************************
 * FileName  : CDlgHotelingLogin.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/21 11:41:53
 ************************************************************************/

#include "cdlghotelinglogin.h"
#include "uikernel/qcomboboxutility.h"
#include "inputmethod/inputmethod.h"
#include "broadsoft/callcenter/include/ccmessagedefine.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "messagebox/modmessagebox.h"
#include "etlmsghandler/modetlmsghandler.h"
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "settinguicommon.h"
#include "baseui/t4xpicpath.h"
#include "uicommon.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qlineeditutility.h"

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#include "modconfigparser.h"
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#endif

namespace
{
#define HOTELING_PASSWORD_STRING    "******"
// #define ACTION_EXIT_LOGIN   "Action_exit_login"
}

static LRESULT OnHotelingLoginMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("DlgIdleScren OnHotelingLoginMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    switch (objMessage.message)
    {
    case ACD_MESSAGE_STATUS_CHANGE:
    case HOTELING_MESSAGE_STATUS_CHANGE:
        {
            if (CallCenter_IsACDOn())
            {
                int iACDStatus = ACD_GetAcdStatus();
                SETTINGUI_INFO("OnMSG A status [%d]", iACDStatus);

                if (iACDStatus != AS_IDLE)
                {
                    // http://10.2.1.199/Bug.php?BugID=60346
                    MessageBox_RemoveTopMessageBox(MessageBox_OK);

                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_CDlgHotelingLogin);
                    if (pDlg != NULL && pDlg->inherits("CDlgHotelingLogin"))
                    {
                        CDlgHotelingLogin * pDlgHotelingLogin = static_cast<CDlgHotelingLogin *>(pDlg);
                        pDlgHotelingLogin->OnMsgLoginSuccess();
                    }

                    //登录成功返回IDLE
                    UIManager_ReturnToIdle();
                }
            }
            else
            {
                int iHotelingStatus = CallCenter_GetHotelingStatus();
                SETTINGUI_INFO("OnMSG H status [%d]", iHotelingStatus);

                if (iHotelingStatus == HOTELING_STATUS_LOGIN)
                {
                    // http://10.2.1.199/Bug.php?BugID=60346
                    MessageBox_RemoveTopMessageBox(MessageBox_OK);

                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_CDlgHotelingLogin);
                    if (pDlg != NULL && pDlg->inherits("CDlgHotelingLogin"))
                    {
                        CDlgHotelingLogin * pDlgHotelingLogin = static_cast<CDlgHotelingLogin *>(pDlg);
                        pDlgHotelingLogin->OnMsgLoginSuccess();
                    }

                    //登录成功返回IDLE
                    UIManager_ReturnToIdle();
                }
            }
        }
        break;
    case HOTELING_MESSAGE_REQ_TIMEOUT:
    case HOTELING_MESSAGE_REQ_FAIL:
    case ACD_MESSAGE_LOGIN_TIMEOUT:
    case ACD_MESSAGE_LOGIN_FAIL:

        {
            MessageBox_RemoveTopMessageBox(MessageBox_OK);
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGIN_FAILED), MESSAGEBOX_NOTE, SHORT_TIME);
        }
        break;
    case ACD_MESSAGE_STATUS_UPDATE:
        {
            MessageBox_RemoveTopMessageBox(MessageBox_OK);

            CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_CDlgHotelingLogin);
            if (pDlg != NULL && pDlg->inherits("CDlgHotelingLogin"))
            {
                CDlgHotelingLogin * pDlgHetelingLogin = static_cast<CDlgHotelingLogin *>(pDlg);
                pDlgHetelingLogin->OnMsgLoginSuccess();
            }

            //登录成功返回IDLE
            UIManager_ReturnToIdle();
        }
        break;
    case ACD_MESSAGE_ACTION_RESULT:
        {
            // 登录成功
            if (0 == objMessage.lParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
            }
            else
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGIN_FAILED), MESSAGEBOX_NOTE, SHORT_TIME);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

CDlgHotelingLogin::CDlgHotelingLogin(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_pUserID(NULL)
    , m_pPwd(NULL)
    , m_pAutoLoginCmb(NULL)
    , m_pHostCmb(NULL)
{
    SetWindowType(eWT_SubPage);

    //SetTopWnd();

    // 设置窗体位置
    //SetDlgGeometry(RECT_SETTING);
#if !IF_FEATURE_XMLBROWSER_HOTELING
    InitData();
#endif
}

CDlgHotelingLogin::~CDlgHotelingLogin()
{
#if IF_FEATURE_XMLBROWSER_HOTELING
    if (m_eType == HOTELING_XMLBROWSER)
    {
        return;
    }
#endif
}

void CDlgHotelingLogin::Init()
{
#if !IF_FEATURE_XMLBROWSER_HOTELING
    //Call Center
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnHotelingLoginMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                          OnHotelingLoginMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT,
                          OnHotelingLoginMsg);
    etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_FAIL, HOTELING_MESSAGE_REQ_FAIL, OnHotelingLoginMsg);
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_LOGIN_FAIL, OnHotelingLoginMsg);
#endif
}

void CDlgHotelingLogin::Uninit()
{
    //Call Center
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnHotelingLoginMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                            OnHotelingLoginMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT,
                            OnHotelingLoginMsg);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_REQ_FAIL, HOTELING_MESSAGE_REQ_FAIL, OnHotelingLoginMsg);
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_LOGIN_FAIL, OnHotelingLoginMsg);
}

bool CDlgHotelingLogin::LoadPageData()  // 读取页面数据
{
    //指针判空
    if (m_pUserID == NULL
            || m_pPwd == NULL
            || m_pAutoLoginCmb == NULL
            || m_pHostCmb == NULL)
    {
        return false;
    }

    if (CallCenter_IsACDOn())
    {
        m_strTitle = LANG_TRANSLATE(TRID_ACD_LOGIN).toUtf8().data();
    }
    else
    {
        m_strTitle = LANG_TRANSLATE(CallCenter_IsFlexibleSeatingOn() ? TRID_FLEXSEAT_LOGIN :
                                    TRID_GUEST_LOGIN).toUtf8().data();
    }

    yl::string strUser;
    Hoteling_GetUserId(strUser);
    m_pUserID->setText(QString::fromUtf8(strUser.c_str()));

    if (Hoteling_IsAutoLogin())
    {
        m_pPwd->setText(HOTELING_PASSWORD_STRING);
    }

    m_pAutoLoginCmb->setCurrentIndex(m_pAutoLoginCmb
                                     ->findData(Hoteling_IsAutoLogin(), Qt::UserRole));

    return true;
}

bool CDlgHotelingLogin::IsStatusChanged() // 查看页面状态是否改变
{
    return false;
}

bool CDlgHotelingLogin::SavePageData() // 保存页面数据
{
    //指针判空
    if (m_pUserID == NULL
            || m_pPwd == NULL
            || m_pAutoLoginCmb == NULL
            || m_pHostCmb == NULL)
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGIN_FAILED), MESSAGEBOX_NOTE, 2000);

        return false;
    }

#if IF_FEATURE_XMLBROWSER_HOTELING
    if (m_eType == HOTELING_XMLBROWSER)
    {
        yl::string strUser = m_pUserID->text().toUtf8().data();
        //判断UserID是否为空
        if (strUser.empty())
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USER_ID_CANNOT_BE_EMPTY), MESSAGEBOX_NOTE,
                                        SHORT_TIME);
            SetErrorFocus(m_pUserID);
            return false;
        }

        yl::string strPassword = m_pPwd->text().toUtf8().data();

        configParser_SetConfigString(kszHotelingUserName, strUser.c_str(), CONFIG_LEVEL_PHONE);
        configParser_SetConfigString(kszHotelingPassword, strPassword.c_str(), CONFIG_LEVEL_PHONE);

        if (xmlbrowser_HotelingRequest(true))
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGINING)
                                        , MESSAGEBOX_Cancel);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOGIN_FAILED)
                                        , MESSAGEBOX_NOTE, SHORT_TIME);
        }
        return false;
    }
#endif

    ACDLogin lgData;
    //user id
    lgData.m_strUser = m_pUserID->text().toUtf8().data();

    //password
    lgData.m_strPassword = m_pPwd->text().toUtf8().data();

    if (lgData.m_strPassword == HOTELING_PASSWORD_STRING)
    {
        lgData.m_strPassword = "";
    }

    if (!CallCenter_IsACDOn())
    {
        //auto
        lgData.m_bIsAuto = m_pAutoLoginCmb
                           ->itemData(m_pAutoLoginCmb->currentIndex(), Qt::UserRole).toBool();

        lgData.m_bIsUseHost = false;
    }
    else if (Get_ACDType() == ACD_BROADSOFT)
    {
        //auto
        lgData.m_bIsAuto = m_pAutoLoginCmb
                           ->itemData(m_pAutoLoginCmb->currentIndex(), Qt::UserRole).toBool();

        //host
        lgData.m_bIsUseHost = m_pHostCmb
                              ->itemData(m_pHostCmb->currentIndex(), Qt::UserRole).toBool();

    }
    else if (Get_ACDType() == ACD_COSMOCOM
             || Get_ACDType() == ACD_GENESYS)
    {
        //需要实现ACD登录
        lgData.m_bIsUseHost = true;
    }

    //判断UserID是否为空
    if (lgData.m_strUser.empty()
            && !(lgData.m_bIsUseHost && Get_ACDType() == ACD_BROADSOFT))
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_USER_ID_CANNOT_BE_EMPTY), MESSAGEBOX_NOTE,
                                    SHORT_TIME);
        SetErrorFocus(m_pUserID);
        return false;
    }

    QString strLoginType = CallCenter_IsACDOn() ? LANG_TRANSLATE(TRID_ACD) :
                           (CallCenter_IsFlexibleSeatingOn() ? LANG_TRANSLATE(TRID_FLEXSEAT) : LANG_TRANSLATE(TRID_HOTELING));

    if (!CallCenter_Login(lgData))
    {
        MessageBox_ShowCommonMsgBox(NULL, strLoginType + LANG_TRANSLATE(TRID_LOGIN_FAILED)
                                    , MESSAGEBOX_NOTE, SHORT_TIME);
        return false;
    }
    else
    {
        MessageBox_ShowCommonMsgBox(NULL, strLoginType + " " + LANG_TRANSLATE(TRID_LOGINING)
                                    , MESSAGEBOX_Cancel);
    }

    return false;
}

bool CDlgHotelingLogin::SaveCompleted()
{
    return false;
}

bool CDlgHotelingLogin::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    objSoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_LOGIN);
    objSoftkeyData[3].m_strIconPath = PIC_SOFTKEY_ACD_LOGIN;

    return true;
}

void CDlgHotelingLogin::SetData(void * pData /* = NULL */)
{
#if IF_FEATURE_XMLBROWSER_HOTELING
    if (m_strSubPageAction.find("xmlbrowser_hoteling_login") != yl::string::npos)
    {
        m_eType = HOTELING_XMLBROWSER;
    }
    else
    {
        m_eType = HOTELING_NOMAL;
        etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE, ACD_MESSAGE_STATUS_CHANGE, OnHotelingLoginMsg);
        etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_UPDATE, ACD_MESSAGE_LOGIN_FAIL, OnHotelingLoginMsg);
        etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_TIMEOUT, HOTELING_MESSAGE_REQ_TIMEOUT,
                              OnHotelingLoginMsg);
        etl_RegisterMsgHandle(HOTELING_MESSAGE_REQ_FAIL, HOTELING_MESSAGE_REQ_FAIL, OnHotelingLoginMsg);
        etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                              OnHotelingLoginMsg);
    }

    InitData();
#endif
}

void CDlgHotelingLogin::InitData()
{
    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetItemSelectBG(PIC_WIDGET_SELECT_BG);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }

    m_pUserID           = new QLineEdit(this);
    m_pPwd              = new QLineEdit(this);
    m_pAutoLoginCmb     = new QComboBox(this);
    m_pHostCmb          = new QComboBox(this);

    //指针判空
    if (m_pUserID == NULL
            || m_pPwd == NULL
            || m_pAutoLoginCmb == NULL
            || m_pHostCmb == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pUserID, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pPwd, Qt::black);
    m_pPwd->setEchoMode(QLineEdit::Password);

    qComboBoxUtility::SetComboBoxStyle(m_pAutoLoginCmb, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pHostCmb, Qt::black);

    m_pAutoLoginCmb->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pAutoLoginCmb->addItem(LANG_TRANSLATE(TRID_ON), true);

    m_pHostCmb->addItem(LANG_TRANSLATE(TRID_OFF), false);
    m_pHostCmb->addItem(LANG_TRANSLATE(TRID_ON), true);

    //设置输入框输入的最大长度
    qLineEditUtility::SetMaxLength(m_pUserID, EDIT_TYPE_NAME);
    qLineEditUtility::SetMaxLength(m_pPwd, EDIT_TYPE_PASSWORD);

#if IF_FEATURE_AUTOP_IME
    qInstallIME(m_pUserID, IME_Name);
    qInstallIME(m_pPwd, IME_PWD);
    yl::string strUerIME = commonUnits_GetIMEByType(kszHotelingUserIME);
    if (!strUerIME.empty())
    {
        qSetCurrentIME(m_pUserID, strUerIME.c_str(), 0, kszHotelingUserIME);
    }

    yl::string strPasswordIME = commonUnits_GetIMEByType(kszHotelingPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(m_pPwd, strPasswordIME.c_str(), 0, kszHotelingPasswordIME);
    }

#else
    //为输入框安装输入法
    qInstallIME(m_pUserID, IME_DIGIT);
    qInstallIME(m_pPwd, IME_PWD);
    qLineEditUtility::SetCurrentInputMethod(m_pPwd, EDIT_TYPE_NUMBER_PWD);
#endif
    //设置Activation标签
    QWidgetPair qPair;

    qPair.first = LANG_TRANSLATE(TRID_USER_ID);
    qPair.second = m_pUserID;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.second = m_pPwd;
    m_vecWidgetPair.push_back(qPair);

#if IF_FEATURE_XMLBROWSER_HOTELING
    if (m_eType == HOTELING_XMLBROWSER)
    {
        m_pAutoLoginCmb->setHidden(true);
        m_pHostCmb->setHidden(true);
    }
    else
#endif
        if (!CallCenter_IsACDOn())
        {
            // Broadsoft ACD登录
            qPair.first = LANG_TRANSLATE(TRID_AUTO_LOGIN);
            qPair.second = m_pAutoLoginCmb;
            m_vecWidgetPair.push_back(qPair);

            m_pHostCmb->setHidden(true);
        }
        else if (Get_ACDType() == ACD_BROADSOFT)
        {
            // Broadsoft ACD登录
            qPair.first = LANG_TRANSLATE(TRID_AUTO_LOGIN);
            qPair.second = m_pAutoLoginCmb;
            m_vecWidgetPair.push_back(qPair);

            if (CallCenter_IsFlexibleSeatingOn())
            {
                m_pHostCmb->setHidden(true);
            }
            else
            {
                qPair.first = LANG_TRANSLATE(TRID_USE_HOST);
                qPair.second = m_pHostCmb;
                m_vecWidgetPair.push_back(qPair);
            }
        }
        else if (Get_ACDType() == ACD_COSMOCOM
                 || Get_ACDType() == ACD_GENESYS)
        {
            m_pAutoLoginCmb->setHidden(true);
            m_pHostCmb->setHidden(true);
        }

    AddWidget(m_vecWidgetPair, true, true, true, true);

    //安装事件过滤器
    m_pUserID->installEventFilter(this);
    m_pPwd->installEventFilter(this);
    m_pAutoLoginCmb->installEventFilter(this);
    m_pHostCmb->installEventFilter(this);
    this->installEventFilter(this);
}

void CDlgHotelingLogin::OnMsgLoginSuccess()
{
    //确保以后如果设置了拒绝回到idle也能正常退出界面
    emit BtnBackClick();
}
