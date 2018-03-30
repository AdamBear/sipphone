#include "model.h"
#include "cdlgauthority.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "baseui/fontcolor.h"
#include "configparser/modconfigparser.h"
#include "commonapi/systemhelperapi.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include <configiddefine.h>
#include "setting/include/authoritymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qlineeditutility.h"
#include "inputmethod/inputmethod.h"
#include "feature/include/modfeature.h"
#include <ylstring.h>

#if IF_FEATURE_AUTOP_IME
#include "commonunits/modcommonunits.h"
#endif

Q_DECLARE_METATYPE(USER_TYPE);
CDlgAuthority::CDlgAuthority(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbAuthority(NULL)
    , m_pEditPassword(NULL)
{
    InitData();
}

CDlgAuthority::~CDlgAuthority()
{
}

void CDlgAuthority::InitData()
{
    m_pCmbAuthority = new QComboBox(this);
    if (m_pCmbAuthority == NULL)
    {
        //内存已满立即返回
        return;
    }

    qComboBoxUtility::SetComboBoxStyle(m_pCmbAuthority, Qt::black);
    //从配置中读取三级用户的名称，加入到下拉框当中
    yl::string strUserName = feature_UserModeGetUserName(UT_USER);
    if (!strUserName.empty())
    {
        m_pCmbAuthority->addItem(LANG_TRANSLATE(strUserName.c_str()), QVariant::fromValue(UT_USER));
    }

    if (configParser_GetConfigInt(kszVarEnable) != 0)
    {
        yl::string strVarName = feature_UserModeGetUserName(UT_VAR);
        if (!strVarName.empty())
        {
            m_pCmbAuthority->addItem(LANG_TRANSLATE(strVarName.c_str()), QVariant::fromValue(UT_VAR));
        }
    }

    yl::string strAdminName = feature_UserModeGetUserName(UT_ADMIN);
    if (!strAdminName.empty())
    {
        m_pCmbAuthority->addItem(LANG_TRANSLATE(strAdminName.c_str()), QVariant::fromValue(UT_ADMIN));
    }

    m_pEditPassword = new QLineEdit(this);
    if (NULL == m_pEditPassword)
    {
        return;
    }
    m_pEditPassword->setEchoMode(QLineEdit::Password);
    qLineEditUtility::SetLineEditStyle(m_pEditPassword, Qt::black);
    //最大输入长度限制
    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);
    //安装输入法
    qInstallIME(m_pEditPassword, IME_PWD);

#if IF_FEATURE_AUTOP_IME
    yl::string strPasswordIME = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(m_pEditPassword, strPasswordIME.c_str(), 0, kszAdvancedSettingPasswordIME);
    }
#endif

    m_pEditPassword->installEventFilter(this);
    m_pCmbAuthority->installEventFilter(this);
    this->installEventFilter(this);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_USER_TYPE);
    qPair.first = strLab;
    qPair.second = m_pCmbAuthority;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.first = strLab;
    qPair.second = m_pEditPassword;
    m_vecWidgetPair.push_back(qPair);


    AddWidget(m_vecWidgetPair);
}


bool CDlgAuthority::IsStatusChanged()
{
    USER_TYPE  ePageData = m_pCmbAuthority->itemData(
                               m_pCmbAuthority->currentIndex()).value<USER_TYPE>();
    if (ePageData != m_eAuthority || !(m_pEditPassword->text().isEmpty()))
    {
        return true;
    }
    return false;
}

bool CDlgAuthority::LoadPageData()
{
    m_eAuthority = feature_UserModeGetCurrType();
    int nCount = 0;
    nCount = m_pCmbAuthority->count();
    for (int i = 0; i < nCount; i++)
    {
        if (m_pCmbAuthority->itemData(i).value<USER_TYPE>() == m_eAuthority)
        {
            m_pCmbAuthority->setCurrentIndex(i);
            return true;
        }
    }
    return false;
}

bool CDlgAuthority::SavePageData()
{
    if (!IsStatusChanged())
    {
        return true;
    }

    USER_TYPE eRequestUser = m_pCmbAuthority->itemData(
                                 m_pCmbAuthority->currentIndex()).value<USER_TYPE>();
    yl::string strRequestUser = feature_UserModeGetUserName(eRequestUser);
    yl::string strPassword = qReturnEdit(m_pEditPassword).toUtf8().data();
    if (FullKeyboardEnabled())
    {
        strPassword = fromQString(m_pEditPassword->text());
    }
    bool bPasswdCorrent = commonAPI_CheckPassword(strRequestUser.c_str(), strPassword.c_str());

    if (bPasswdCorrent && configParser_GetConfigInt(kszUserPwdLimit) != 0
            && eRequestUser != UT_ADMIN)
    {
        if (strPassword == strRequestUser)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NEED_TO_CHANG_PASSWD), MESSAGEBOX_NOTE,
                                        SHORT_TIME);
            // 重新设置蕉点
            if (NULL != m_pEditPassword)
            {
                m_pEditPassword->setText(toQString(""));
                m_pEditPassword->setFocus();
            }
            return false;
        }
    }
    //验证密码
    if (!bPasswdCorrent)
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_PASSWORD_IS_WRONG), MESSAGEBOX_NOTE,
                                    SHORT_TIME);
        // 重新设置蕉点
        if (NULL != m_pEditPassword)
        {
            m_pEditPassword->setFocus();
        }
        return false;
    }

    feature_UserModeSetCurrType(eRequestUser);

    //发送消息通知缓存的界面刷新
    etl_NotifyApp(true, AUTHORITY_MESSAGE_AUTHORITY_CHANGED, 0, 0);

    //弹出保存成功的提示框
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_USER_MODE_CHANGED),
                                MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                MSG_ID_SHOW_NOTE_BEFORE_BACK);
    return false;
}

