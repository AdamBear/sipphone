#include "dlgautopcodeauthorized.h"

#if IF_BUG_23826

#include "qtcommon/qmisc.h"
#include "inputmethod/inputmethod.h"
#include "keymap.h"
#include "uikernel/languagehelper.h"
#include "commonunits/modcommonunits.h"
#include "uicommon.h"
#include "uimanager/moduimanager.h"
#include "baseui/fontcolor.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/t4xpicpath.h"

CDlgAutoPCodeAuthorized::CDlgAutoPCodeAuthorized(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_pEditName(NULL)
    , m_pEditPassword(NULL)
{
    SetRejectReturnToIdle();
    InitData();
}

CDlgAutoPCodeAuthorized::~CDlgAutoPCodeAuthorized()
{

}

void CDlgAutoPCodeAuthorized::InitData()
{
    m_pEditName = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);
    if (m_pEditName == NULL || m_pEditPassword == NULL)
    {
        return ;
    }

    //安装输入法
    qInstallIME(m_pEditName, IME_DIAL);
    qInstallIME(m_pEditPassword, IME_PWD);

    qLineEditUtility::SetMaxLength(m_pEditName, EDIT_TYPE_NAME);
    qLineEditUtility::SetMaxLength(m_pEditPassword, EDIT_TYPE_PASSWORD);

    //设置输入框风格
    SetLineEditDefaultStyle(m_pEditName);
    SetLineEditDefaultStyle(m_pEditPassword);

    //设置密码回显示方式
    m_pEditPassword->setEchoMode(QLineEdit::Password);

    m_pEditName->installEventFilter(this);
    m_pEditPassword->installEventFilter(this);
    this->installEventFilter(this);

    //注册信号量
    //connect(this, SIGNAL(ProcessPageAction(const QString&)), this, SLOT(OnProcessPageAction(const QString&)));

    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_AUTOP_AHTHORITY));

    //控件显示
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_USER_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PASSWORD);
    qPair.first = strLab;
    qPair.second = m_pEditPassword;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
}

bool CDlgAutoPCodeAuthorized::LoadPageData()
{
    this->setFocus();

    //设置首选输入法
    qLineEditUtility::SetCurrentInputMethod(m_pEditName, EDIT_TYPE_NAME);
    qLineEditUtility::SetCurrentInputMethod(m_pEditPassword, EDIT_TYPE_NAME_PWD);

    return true;
}

bool CDlgAutoPCodeAuthorized::IsStatusChanged()
{
    return true;
}

bool CDlgAutoPCodeAuthorized::SavePageData()
{
    return true;
}

bool CDlgAutoPCodeAuthorized::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{

    objSoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_OK);

    return true;
}

bool CDlgAutoPCodeAuthorized::eventFilter(QObject * pObject, QEvent * pEvent)
{

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}


bool CDlgAutoPCodeAuthorized::OnAction(const QString & strAction)
{
    if (strAction == ACTION_SAVE_DATA)
    {
        yl::string strName = m_pEditName->text().toUtf8().data();
        yl::string strPassword = m_pEditPassword->text().toUtf8().data();

        AutopStartCodeConnect(strName.c_str(), strPassword.c_str());

        emit BtnBackClick(); //设置拒绝回退到idle之后调用returntoidle无法退出界面，需主动退出
        UIManager_ReturnToIdle();
        return true;
    }
    else if (strAction == ACTION_QUIT)
    {
        emit BtnBackClick(); //设置拒绝回退到idle之后调用returntoidle无法退出界面，需主动退出
        UIManager_ReturnToIdle();
        return true;
    }

    return false;
}


#endif // end IF_BUG_23826
