#include "model.h"
#include "dlgstatusaccounts.h"
#include "qtcommon/qmisc.h"
#include "setting/src/statuscontroller.h"
#include "uikernel/languagehelper.h"
#include "setting/include/common.h"
#include "interfacedefine.h"
#include "baseui/fontcolor.h"
#include "actiondefinecommon.h"
#include "settingui/include/modsettingui.h"
#include "actiondefinecommon.h"
#include "account/include/modaccount.h"
#include "settingpage.h"

const yl::string strActionFromStatus             = "OpenSubPage(status_accounts)";

#if IF_SHOW_TEST_DEBUG_DIALOG
void show_debug(QString info = "no info", QString title = "debug");
void show_debug(QString info /* ="no info"  */, QString title /* = "debug"*/)
{
    QMessageBox::warning(NULL, title, info);
}
#endif

LRESULT CDlgStatusAccounts::StatusAccountMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("CDlgStatusAccounts Receive Msg[%d].", objMessage.message);

    CBaseDialog * pDlg = UIManager_GetTopWindow();
    CSettingWindow * pSettingWnd = dynamic_cast<CSettingWindow *>(pDlg);
    if (pSettingWnd == NULL)
    {
        return 0;
    }

    CDlgBaseSubPage * pSubPage = pSettingWnd->GetCurrentSubPage();
    if (pSubPage != NULL && pSubPage->inherits("CDlgStatusAccounts"))
    {
        if (ACCOUNT_STATUS_CHANGED == objMessage.message
                || ACCOUNT_DEFAULT_CHANGED == objMessage.message)
        {
            pSubPage->OnReShow();
            return TRUE;
        }
    }

    return FALSE;
}

CDlgStatusAccounts::CDlgStatusAccounts(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_bEnteredFromStatus(true)
    , m_bFirtEnter(true)
{
    InitPageData();
}

CDlgStatusAccounts::~CDlgStatusAccounts()
{
}

void CDlgStatusAccounts::Init()
{
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                          &CDlgStatusAccounts::StatusAccountMsg);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CDlgStatusAccounts::StatusAccountMsg);
}

void CDlgStatusAccounts::Uninit()
{
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                            &CDlgStatusAccounts::StatusAccountMsg);
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                            &CDlgStatusAccounts::StatusAccountMsg);
}

//界面初始化
void CDlgStatusAccounts::InitPageData()
{
    //
}

bool CDlgStatusAccounts::LoadPageData()
{
    if (m_bFirtEnter)
    {
        m_bFirtEnter = false;
        return false;
    }
    ClearAllContent();
    m_bEnteredFromStatus = (m_strSubPageAction == strActionFromStatus);

    if (m_bEnteredFromStatus)
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_ACCOUNT_STATUS));
        SetNoClickBG();
    }
    else
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_ACCOUNTS));
    }

    this->setFocus();
    AccountStatusListData currentPageData;
    bool bIsSuccess =
        g_pStatusController->GetPhoneAccountStatus(&currentPageData);

    int iDefaultAccountID = acc_GetDefaultAccount();

    QWidgetPair qPair;
    QString strAccountNumber;
    QString strAccountStatus = "";

    if (bIsSuccess && currentPageData.m_listStatus.size() > 0)
    {
        YLList<AccountStatusData>::iterator it = currentPageData.m_listStatus.begin();

        if (m_bEnteredFromStatus)
        {
            int iAcountNum = acc_AccountNum();

            for (int i = 0; i < iAcountNum; ++i)
            {
                if (acc_GetState(i) != LS_REGISTERING &&
                        acc_GetState(i) != LS_REGISTERED  &&
                        acc_GetState(i) != LS_REGISTER_FAIL)
                {
                    continue;
                }

                QLabel * pLab = new QLabel(this);
                if (pLab == NULL)
                {
                    return false;
                }
                pLab->setObjectName("Lab");
                pLab->setText("");
                SetWgtFontColor(pLab, FONT_TYPE_TEXT);

                //初始化pair
                qPair.first = "(" + LANG_TRANSLATE(TRID_EMPTY) + ")";
                qPair.second = pLab;
                qPair.bAddIndex = true;
                qPair.eType = SETTING_ITEM_NORMAL;
                qPair.third = "";
                if (!m_bEnteredFromStatus)
                {
                    qPair.third = "enter";
                    qPair.eType = SETTING_ITEM_MENU;
                }

                //设置对应的状态和显示名
                for (it = currentPageData.m_listStatus.begin(); it != currentPageData.m_listStatus.end(); ++it)
                {
                    if ((*it).m_nID == i)
                    {
                        //提起状态信息
                        switch ((*it).m_nStatus)
                        {
                        case LS_UNKNOW:
                            strAccountStatus = LANG_TRANSLATE(TRID_UNKNOWN);
                            break;
                        case LS_DISABLE:
                            strAccountStatus = LANG_TRANSLATE(TRID_DISABLED);
                            break;
                        case LS_REGISTERING:
                            strAccountStatus = LANG_TRANSLATE(TRID_REGISTERING);
                            break;
                        case LS_REGISTERED:
                        case LS_DISABLE_REG:
                            {
                                if (iDefaultAccountID == i)
                                {
                                    strAccountStatus = LANG_TRANSLATE(TRID_DEFAULT_ACCOUNT);
                                }
                                else
                                {
                                    strAccountStatus = LANG_TRANSLATE(TRID_REGISTERED);
                                }
                            }
                            break;
                        case LS_REGISTER_FAIL:
                            strAccountStatus = LANG_TRANSLATE(TRID_REGISTER_FAIL);
                            break;
                        case LS_BE_UNREGISTERED:
                            strAccountStatus = LANG_TRANSLATE(TRID_UNREGISTERED);
                            break;
                        case LS_SYSTEM_DEFAULT_ACCOUNT:
                            strAccountStatus = LANG_TRANSLATE(TRID_DEFAULT_ACCOUNT);
                            break;
                        case LS_INEXISTENCE:
                            strAccountStatus = LANG_TRANSLATE(TRID_INEXISTENCE);
                            break;
                        case LS_UNREGISTERING:
                            strAccountStatus = LANG_TRANSLATE(TRID_UNREGISTERING);
                            break;
                        case LS_EXIT:
                            strAccountStatus = LANG_TRANSLATE(TRID_EXIT);
                            break;
                        default:
                            break;
                        }

                        strAccountNumber = "(";
                        strAccountNumber += toQString(acc_GetAccountShowText(i));
                        strAccountNumber += ")";

                        pLab->setText(strAccountStatus);
                        qPair.first = strAccountNumber;

                        break;
                    }
                }

                m_vecWidgetPair.push_back(qPair);
            }
        }
        else
        {
            int iAccNum = acc_AccountNum();
            for (int nIndex = 0; nIndex < iAccNum; nIndex++)
            {
                QLabel * pLab = new QLabel(this);
                if (pLab == NULL)
                {
                    return false;
                }
                pLab->setText("");
                SetWgtFontColor(pLab, FONT_TYPE_TEXT);

                //初始化pair
                qPair.first = "(" + LANG_TRANSLATE(TRID_EMPTY) + ")";
                qPair.second = pLab;
                qPair.bAddIndex = true;
                qPair.eType = SETTING_ITEM_NORMAL;
                qPair.third = "";
                if (!m_bEnteredFromStatus)
                {
                    qPair.third = "enter";
                    qPair.eType = SETTING_ITEM_MENU;
                }

                //设置对应的状态和显示名
                for (it = currentPageData.m_listStatus.begin(); it != currentPageData.m_listStatus.end(); ++it)
                {
                    if ((*it).m_nID == nIndex)
                    {
                        //提起状态信息
                        switch ((*it).m_nStatus)
                        {
                        case LS_UNKNOW:
                            strAccountStatus = LANG_TRANSLATE(TRID_UNKNOWN);
                            break;
                        case LS_DISABLE:
                            strAccountStatus = LANG_TRANSLATE(TRID_DISABLED);
                            break;
                        case LS_REGISTERING:
                            strAccountStatus = LANG_TRANSLATE(TRID_REGISTERING);
                            break;
                        case LS_REGISTERED:
                        case LS_DISABLE_REG:
                            {
                                if (iDefaultAccountID == nIndex)
                                {
                                    strAccountStatus = LANG_TRANSLATE(TRID_DEFAULT_ACCOUNT);
                                }
                                else
                                {
                                    strAccountStatus = LANG_TRANSLATE(TRID_REGISTERED);
                                }
                            }
                            break;
                        case LS_REGISTER_FAIL:
                            strAccountStatus = LANG_TRANSLATE(TRID_REGISTER_FAIL);
                            break;
                        case LS_BE_UNREGISTERED:
                            strAccountStatus = LANG_TRANSLATE(TRID_UNREGISTERED);
                            break;
                        case LS_SYSTEM_DEFAULT_ACCOUNT:
                            strAccountStatus = LANG_TRANSLATE(TRID_DEFAULT_ACCOUNT);
                            break;
                        case LS_INEXISTENCE:
                            strAccountStatus = LANG_TRANSLATE(TRID_INEXISTENCE);
                            break;
                        case LS_UNREGISTERING:
                            strAccountStatus = LANG_TRANSLATE(TRID_UNREGISTERING);
                            break;
                        case LS_EXIT:
                            strAccountStatus = LANG_TRANSLATE(TRID_EXIT);
                            break;
                        default:
                            break;
                        }

                        strAccountNumber = "(";
                        //获取帐号号码
                        if ((*it).m_strAccountName.empty())
                        {
                            strAccountNumber += LANG_TRANSLATE(TRID_EMPTY);
                            strAccountNumber += ")";
                        }
                        else
                        {
                            strAccountNumber += toQString((*it).m_strAccountName.c_str());
                            strAccountNumber += ")";
                        }

                        pLab->setText(strAccountStatus);
                        qPair.first = strAccountNumber;

                        break;
                    }
                }

                m_vecWidgetPair.push_back(qPair);
            }
        }

        if (m_bEnteredFromStatus)
        {
            AddWidget(m_vecWidgetPair, true, false);
        }
        else
        {
            AddWidget(m_vecWidgetPair, true, false, true);
        }

        return true;
    }
    else
    {
        if (m_bEnteredFromStatus)
        {
            ClearAllContent();
            AddWidget(m_vecWidgetPair, true, false);
        }
        else
        {
            QWidgetPair qPair(SETTING_ITEM_MENU);

            //设置页面数据
            strAccountNumber = "(" + LANG_TRANSLATE(TRID_EMPTY) + ")";
            int iAccNum = acc_AccountNum();
            for (int nIndex = 0; nIndex < iAccNum; ++nIndex)
            {
                qPair.first = strAccountNumber;
                if (!m_bEnteredFromStatus)
                {
                    qPair.third = ACTION_ENTER;
                }
                m_vecWidgetPair.push_back(qPair);
            }

            AddWidget(m_vecWidgetPair, true, true, true);
        }

        return false;
    }
}

//状态显示softkey没有保存项
bool CDlgStatusAccounts::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

void CDlgStatusAccounts::OnEnterAction()
{
    if (m_pGeneralFrame == NULL)
    {
        return ;
    }

    int iMenuIndex = chMIN(chMAX(m_pGeneralFrame->GetFocusItemDataIndex() + 1, 0), acc_AccountNum());
    char strAction[30];
    sprintf(strAction, "OpenSubPage(account_%d)", iMenuIndex);

    SubMenuData subMenuData;
    subMenuData.iFirstSelectItem = 0;
    subMenuData.strMenuAction = strAction;
    emit openpage(subMenuData);
}

bool CDlgStatusAccounts::IsStatusChanged()
{
    return false;
}

bool CDlgStatusAccounts::SavePageData()
{
    return true;
}

bool CDlgStatusAccounts::OnAction(const QString & strAction)
{
    if (strAction == ACTION_ENTER)
    {
        OnEnterAction();
        return true;
    }
    return false;
}

void CDlgStatusAccounts::OnReShow()
{
    if (m_pGeneralFrame != NULL)
    {
        int iCurrentIndex = m_pGeneralFrame->GetFocusItemDataIndex();
        // setsubpage时会loaddata，调用OnReShow时也会loaddata，影响速度
        LoadPageData();
        SetCurrentIndex(iCurrentIndex);
        UpdateWnd();
    }
}

