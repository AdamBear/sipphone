#include "cdlgresettofactory.h"
#include "baseui/fontcolor.h"
#include "setting/include/common.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "setting/src/resetandrebootcontroller.h"
#include "setting/include/authorityiddefine.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/qcomboboxutility.h"
#include "adapterbox/include/modadapterbox.h"
#include "configparser/modconfigparser.h"
#include "baselibs/include/configserver/configserverinc.h"
#include "service_autop.h"
#include "mainwnd/mainwnd.h"


namespace
{
#define     RESET_LOCAL_SETTINGS_ACTION             "reset_local_config"
#define     RESET_WEB_SETTINGS_ACTION               "reset_web_config"
#define     RESET_NON_STATIC_SETTINGS_ACTION        "reset_non_static_config"
#define     RESET_STATIC_SETTINGS_ACTION            "reset_static_config"
#define     RESET_TO_FACTORY_ACTION                 "reset_to_factory"
#define     RESET_USERDATA_AND_CONFIGDATA           "reset_userdata_and_configdata"

#define     kszAutopSeparateWebConfig               "AutopSeparateWebConfig"
#define     kszAutoProvisionCustomProtect           "AutoProvisionCustomProtect"

// http://10.2.1.199/Bug.php?BugID=108653
// 延迟60s
#define     RESET_DELAY_TIME        60000
}

Q_DECLARE_METATYPE(ResetOptionType)

CDlgResetToFactory::CDlgResetToFactory(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }
}

CDlgResetToFactory::~CDlgResetToFactory()
{
}

bool CDlgResetToFactory::LoadPageData()
{
    this->setFocus();

    MakeReadyForRelayout();

    QWidgetPair qPair;
    qPair.bAddMark = false;
    qPair.bChangeStyleOnPress = true;

    if (m_eType == RPT_BASIC
            && configParser_GetConfigInt(kszAutoProvisionCustomProtect) == 1)
    {
        // 加载local settings项
        qPair.first = LANG_TRANSLATE(TRID_RESET_LOCAL_SETTINGS);
        qPair.third = RESET_LOCAL_SETTINGS_ACTION;
        qPair.strAuthorId = kszResetLocalConfig;
        m_vecWidgetPair.push_back(qPair);

        qPair.first = LANG_TRANSLATE(TRID_RESET_USERDATA_AND_LOCAL_SETTINGS);
        qPair.third = RESET_USERDATA_AND_CONFIGDATA;
        qPair.strAuthorId = kszResetUserDataUserCfg;
        m_vecWidgetPair.push_back(qPair);

        AddWidget(m_vecWidgetPair);
        return true;
    }

    if (configParser_GetConfigInt(kszAutoProvisionCustomProtect) == 1)
    {
        // 加载local settings项
        qPair.first = LANG_TRANSLATE(TRID_RESET_LOCAL_SETTINGS);
        qPair.third = RESET_LOCAL_SETTINGS_ACTION;
        qPair.strAuthorId = kszResetLocalConfig;
        m_vecWidgetPair.push_back(qPair);

        // 加载web settings项
        if (configParser_GetConfigInt(kszAutopSeparateWebConfig) == 1)
        {
            qPair.first = LANG_TRANSLATE(TRID_RESET_WEB_SETTINGS);
            qPair.third = RESET_WEB_SETTINGS_ACTION;
            qPair.strAuthorId = kszResetWebConfig;
            m_vecWidgetPair.push_back(qPair);
        }

        // 加载non-static settings项
        qPair.first = LANG_TRANSLATE(TRID_RESET_NON_DEVICE_SETTINGS);
        qPair.third = RESET_NON_STATIC_SETTINGS_ACTION;
        qPair.strAuthorId = kszResetNonStaticConfig;
        m_vecWidgetPair.push_back(qPair);

        // 加载static settings项
        qPair.first = LANG_TRANSLATE(TRID_RESET_DEVICE_SETTINGS);
        qPair.third = RESET_STATIC_SETTINGS_ACTION;
        qPair.strAuthorId = kszResetStaticConfig;
        m_vecWidgetPair.push_back(qPair);
    }

    // 加载 reset userdata&config项
    if (configParser_GetConfigInt(kszAutoProvisionCustomProtect) == 1)
    {
        qPair.first = LANG_TRANSLATE(TRID_RESET_USERDATA_AND_LOCAL_SETTINGS);
        qPair.third = RESET_USERDATA_AND_CONFIGDATA;
        qPair.strAuthorId = kszResetUserDataUserCfg;
        m_vecWidgetPair.push_back(qPair);
    }

    // 加载 reset to factory项
    qPair.first = LANG_TRANSLATE(TRID_RESET_TO_FACTORY);
    qPair.third = RESET_TO_FACTORY_ACTION;
    qPair.strAuthorId = kszResetToFactory;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
    return true;
}

bool CDlgResetToFactory::IsStatusChanged()
{
    return false;
}

bool CDlgResetToFactory::SavePageData()
{
    return false;
}

bool CDlgResetToFactory::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

bool CDlgResetToFactory::OnAction(const QString & strAction)
{
    if (OnShowPromptAction(strAction))
    {
        return true;
    }

    return false;
}

void CDlgResetToFactory::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nMsgBoxId = pMessageBox->GetID();
    int nMsgBoxResult = pMessageBox->GetResult();

    switch (nMsgBoxId)
    {
    case RESET_CONFIG_ACTION_RESET_LOCAL_SETTINGS:
    case RESET_CONFIG_ACTION_RESET_WEB_SETTINGS:
    case RESET_CONFIG_ACTION_RESET_NON_STATIC_SETTINGS:
    case RESET_CONFIG_ACTION_RESET_STATIC_SETTINGS:
        {
            if (MessageBox_OK == nMsgBoxResult)
            {
                int nRestoreType = GetRestoreType(nMsgBoxId);

                if (-1 == nRestoreType)
                {
                    SETTING_ERR("GetRestoreType(int) fail in CDlgResetToFactory::ProcessMsgBoxCallBack when Restore");

                    return;
                }



                //处理所有其他事件，避免调用Send阻塞后弹框无法弹出
                QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                            QEventLoop::ExcludeSocketNotifiers);

                PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

                switch (ePhoneType)
                {
                case PT_T46S:
                case PT_T48S:
                case PT_T49:
                    {
                        if (Autop_GetAutopStatus() == AUTOP_POWER_ON_END)
                        {
                            ShowWaitMessageBox(nMsgBoxId);

                            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                                        QEventLoop::ExcludeSocketNotifiers);

                            msgSendMsgToThreadTimeoutEx(msgGetThreadByName(ATP_NAMAGE_NAME), ATP_MSG_REQ_CLEAR_CFG,
                                                        nRestoreType, 0, 0, NULL, RESET_DELAY_TIME);
                        }
                        else
                        {
                            MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_RESET_FAIL_PLEASE_RETRY_LATER),
                                                        MESSAGEBOX_NOTE, 2000);
                        }
                    }
                    break;
                default:
                    {
                        ShowWaitMessageBox(nMsgBoxId);

                        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents |
                                                    QEventLoop::ExcludeSocketNotifiers);

                        msgSendMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERCFG,
                                           nRestoreType, 0);
                    }
                    break;
                }


                return;
            }
        }
        break;
    case RESET_CONFIG_ACTION_RESET_FACTORY:
        {
            if (MessageBox_OK == nMsgBoxResult)
            {
                AdapterBox_ResetFactory(RESET_OPTION_ALL);
            }
        }
        break;
    case RESET_CONFIG_ACITON_RESET_USERDATA_AND_CONFIG:
        {
            if (MessageBox_OK == nMsgBoxResult)
            {
                AdapterBox_ResetFactory(RESET_OPTION_USERDATA_USERCFG);
            }
        }
        break;
    default:
        break;
    }
}

bool CDlgResetToFactory::OnShowPromptAction(const QString & strAction)
{
    bool bResult = true;
    int nMsgBoxId = RESET_CONFIG_ACTION_BASE;
    yl::string strMsgNote;

    if (RESET_LOCAL_SETTINGS_ACTION == strAction)
    {
        nMsgBoxId = RESET_CONFIG_ACTION_RESET_LOCAL_SETTINGS;
        strMsgNote = TRID_IF_RESET_LOCAL_SETTING;
    }
    else if (RESET_WEB_SETTINGS_ACTION == strAction)
    {
        nMsgBoxId = RESET_CONFIG_ACTION_RESET_WEB_SETTINGS;
        strMsgNote = TRID_IF_RESET_WEB_SETTING;
    }
    else if (RESET_NON_STATIC_SETTINGS_ACTION == strAction)
    {
        nMsgBoxId = RESET_CONFIG_ACTION_RESET_NON_STATIC_SETTINGS;
        strMsgNote = TRID_IF_RESET_NDEVICE_SETTING;
    }
    else if (RESET_STATIC_SETTINGS_ACTION == strAction)
    {
        nMsgBoxId = RESET_CONFIG_ACTION_RESET_STATIC_SETTINGS;
        strMsgNote = TRID_IF_RESET_DEVICE_SETTING;
    }
    else if (RESET_TO_FACTORY_ACTION == strAction)
    {
        nMsgBoxId = RESET_CONFIG_ACTION_RESET_FACTORY;
        strMsgNote = TRID_IF_RESET_TO_FACTORY_SETTING;
    }
    else if (RESET_USERDATA_AND_CONFIGDATA == strAction)
    {
        nMsgBoxId = RESET_CONFIG_ACITON_RESET_USERDATA_AND_CONFIG;
        strMsgNote = TRID_IF_RESET_USERDATA_LOCALCONFIG;
    }
    else
    {
        bResult = false;
    }

    if (bResult)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(strMsgNote.c_str()),
                                    MESSAGEBOX_OkCancel, -1, "", "", nMsgBoxId);
    }

    return bResult;
}

void CDlgResetToFactory::ShowWaitMessageBox(int nAction)
{
    QString strWaitNote;

    if (RESET_CONFIG_ACTION_RESET_LOCAL_SETTINGS == nAction)
    {
        strWaitNote = LANG_TRANSLATE(TRID_RESETLOCAL_PLEASE_WAIT);
    }
    else if (RESET_CONFIG_ACTION_RESET_WEB_SETTINGS == nAction)
    {
        strWaitNote = LANG_TRANSLATE(TRID_RESETWEB_PLEASE_WAIT);
    }
    else if (RESET_CONFIG_ACTION_RESET_NON_STATIC_SETTINGS == nAction)
    {
        strWaitNote = LANG_TRANSLATE(TRID_RESETNDEVICE_PLEASE_WAIT);
    }
    else if (RESET_CONFIG_ACTION_RESET_STATIC_SETTINGS == nAction)
    {
        strWaitNote = LANG_TRANSLATE(TRID_RESETDEVICE_PLEASE_WAIT);
    }

    if (!strWaitNote.isEmpty())
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(strWaitNote), MESSAGEBOX_NOTE, 2000,
                                    "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
}

int CDlgResetToFactory::GetRestoreType(int nAction)
{
    int nResult = -1;

    // 对应 ATP_MSG_REQ_CLEAR_CFG/CSM_CLEAR_USERCFG 消息的wParam值
    // wParam：0:phone, 1:web, 2:autop, 3:device, 4:non device,

    if (RESET_CONFIG_ACTION_RESET_LOCAL_SETTINGS == nAction)
    {
        nResult = 0;
    }
    else if (RESET_CONFIG_ACTION_RESET_WEB_SETTINGS == nAction)
    {
        nResult = 1;
    }
    else if (RESET_CONFIG_ACTION_RESET_STATIC_SETTINGS == nAction)
    {
        nResult = 3;
    }
    else if (RESET_CONFIG_ACTION_RESET_NON_STATIC_SETTINGS == nAction)
    {
        nResult = 4;
    }

    return nResult;
}

void CDlgResetToFactory::SetData(void* pData /* = NULL */)
{
    if (strcmp(m_strSubPageAction.c_str(), "OpenSubPage(basic_reset_config)") == 0)
    {
        m_eType = RPT_BASIC;
    }
    else
    {
        m_eType = RPT_ADV;
    }
}
