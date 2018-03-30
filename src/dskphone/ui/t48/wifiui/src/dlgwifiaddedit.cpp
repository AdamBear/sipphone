#include "wifi_inc.h"

namespace
{
enum
{
    WIFI_UI_ADD_PAGE_TYPE_ADD,
    WIFI_UI_ADD_PAGE_TYPE_DETAIL_TO_ADD, // 由详情进入添加，SSID、Profile Name不允许编辑
    WIFI_UI_ADD_PAGE_TYPE_EDIT,
};

enum
{
    COMBOX_ID_SERCURE_NONE = 0, // SERCURE_NONE
    COMBOX_ID_SERCURE_WEP,      // SERCURE_WEP
    COMBOX_ID_SERCURE_WPA_PAK,  // SERCURE_WPA_PSK
    COMBOX_ID_SERCURE_WPA2_PAK, // SERCURE_WPA2_PSK
    COMBOX_ID_SERCURE_WPA_EAP,  // SERCURE_WPA_EAP
    COMBOX_ID_SERCURE_WPA2_EAP, // SERCURE_WPA2_EAP
};
}


CDlgWifiAddEdit::CDlgWifiAddEdit(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_pCmbSecurityMode(NULL)
    , m_pEditProfileName(NULL)
    , m_pEditSSID(NULL)
    , m_pEditUserName(false)
    , m_pEditWPASharedKey(NULL)
    , m_pCmbCipherType(NULL)
    , m_iPageType(WIFI_UI_ADD_PAGE_TYPE_ADD)
    , m_iNetID(-1)
{
    SetWindowType(eWT_SubPage);

    InitData();

    InitListItem();

    SetWindowTitle(LANG_TRANSLATE(TRID_ADD_NETWORK));

    this->installEventFilter(this);
}

CDlgWifiAddEdit::~CDlgWifiAddEdit(void)
{
}

/********************************* 继承自CListSubPage ***********************************/
bool CDlgWifiAddEdit::LoadPageData()
{
    m_iNetID = _UIWifiManager.GetCurID();
    SETTINGUI_INFO("[CDlgWifiAddEdit::LoadPageData()] ID[%d]", m_iNetID);

    if (m_iNetID < 0)
    {
        return true;
    }

#if IF_FEATURE_AUTOP_IME
    yl::string strPasswordIME = commonUnits_GetIMEByType(kszWifiPasswordIME);
    if (!strPasswordIME.empty())
    {
        qSetCurrentIME(m_pEditWPASharedKey, strPasswordIME.c_str(), 0, kszWifiPasswordIME);
    }
#else
    qLineEditUtility::SetCurrentInputMethod(kszWifiPasswordIME, EDIT_TYPE_PASSWORD);
#endif

    ENUM_WIFI_PAGE_TYPE eType = (ENUM_WIFI_PAGE_TYPE)_UIWifiManager.GetPageType();

    if (ENUM_WIFI_EDIT_PAGE_TYPE == eType)
    {
        m_iPageType = WIFI_UI_ADD_PAGE_TYPE_EDIT;
        SetWindowTitle(LANG_TRANSLATE(TRID_EDIT_NETWORK));
    }
    else if (ENUM_WIFI_ADD_PAGE_BY_WIFI_DETAIL_TYPE == eType)
    {
        m_iPageType = WIFI_UI_ADD_PAGE_TYPE_DETAIL_TO_ADD;
    }

    if (GetNetWorkInfoByID(m_iNetID, m_wifiInfo))
    {
        SetPageDataByType();
    }

    // http://10.2.1.199/Bug.php?BugID=105679
    if (WIFI_UI_ADD_PAGE_TYPE_DETAIL_TO_ADD == m_iPageType)
    {
        if (m_pEditProfileName != NULL)
        {
            m_pEditProfileName->setEnabled(false);
        }

        if (m_pEditSSID != NULL)
        {
            m_pEditSSID->hide();
            m_pEditSSID->setEnabled(false);
        }
    }

    return true;
}

void CDlgWifiAddEdit::SetPageDataByType()
{
    if (NULL == m_pEditSSID || NULL == m_pEditProfileName || NULL == m_pCmbSecurityMode
            || NULL == m_pEditWPASharedKey || NULL == m_pEditUserName)
    {
        return;
    }

    SETTINGUI_INFO("[WifiAddEdit::SetPageDataByType] Security[%d], SSID[%s], profile[%s], encryption[%d]",
                   m_wifiInfo.iSercureMode, m_wifiInfo.strSSID.c_str(), m_wifiInfo.strProfileName.c_str(),
                   m_wifiInfo.iEncrypt);

    // SSID
    m_pEditSSID->setText(toQString(m_wifiInfo.strSSID));

    // Profie Name
    if (m_wifiInfo.strProfileName.empty())
    {
        m_pEditProfileName->setText(toQString(m_wifiInfo.strSSID));
    }
    else
    {
        m_pEditProfileName->setText(toQString(m_wifiInfo.strProfileName));
    }

    // Sercure Mode
    m_pCmbSecurityMode->setCurrentIndex(GetSecurityModeIndexBySecurityType(m_wifiInfo.iSercureMode));

    // User Name & Password
    if (SERCURE_WPA_EAP == m_wifiInfo.iSercureMode || SERCURE_WPA2_EAP == m_wifiInfo.iSercureMode)
    {
        m_pEditUserName->setText(toQString((m_wifiInfo.strEapName)));
        m_pEditWPASharedKey->setText(toQString(m_wifiInfo.strEapPwd));
    }
    else
    {
        m_pEditWPASharedKey->setText(toQString(m_wifiInfo.strPsk));
    }

    // Encrypt
    switch (m_wifiInfo.iSercureMode)
    {
    case SERCURE_WPA_PSK:
    case SERCURE_WPA2_PSK:
    case SERCURE_WPA_EAP:
    case SERCURE_WPA2_EAP:
        {
            m_pCmbCipherType->setCurrentIndex(m_pCmbCipherType->findData(m_wifiInfo.iEncrypt));
        }
        break;
    case SERCURE_NONE:
    default:
        break;
    }
}

bool CDlgWifiAddEdit::IsStatusChanged()
{
    if (m_iNetID != WIFI_SECTION_ID_INVAILED)
    {
        WiFiNetInfo preWifiInfo;
        if (GetNetWorkInfoByID(m_iNetID, preWifiInfo))
        {
            GetCurPageData();

            if ((m_wifiInfo.iSercureMode != preWifiInfo.iSercureMode)
                    || (m_wifiInfo.iEncrypt != preWifiInfo.iEncrypt)
                    || (0 != strcmp(m_wifiInfo.strSSID.c_str(), preWifiInfo.strSSID.c_str()))
                    || (0 != strcmp(m_wifiInfo.strProfileName.c_str(), preWifiInfo.strProfileName.c_str()))
               )
            {
                return true;
            }

            // Eap 安全模式判断用户名和密码，其他方式只判断密码
            if (SERCURE_WPA_EAP == m_wifiInfo.iSercureMode || SERCURE_WPA2_EAP == m_wifiInfo.iSercureMode)
            {
                return (0 != strcmp(m_wifiInfo.strEapName.c_str(), preWifiInfo.strEapName.c_str()))
                       || (0 != strcmp(m_wifiInfo.strEapPwd.c_str(), preWifiInfo.strEapPwd.c_str()));
            }
            else
            {
                return (0 != strcmp(m_wifiInfo.strPsk.c_str(), preWifiInfo.strPsk.c_str()));
            }
        }
    }

    return false;
}

bool CDlgWifiAddEdit::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL != m_pEditSSID && m_pEditSSID->text().trimmed().isEmpty())
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_SSID_CANNOT_BE_EMPTY);
        objErrorContent.pLineEdit = m_pEditSSID;
        return false;
    }

    if (m_pCmbSecurityMode != NULL &&
            (m_pCmbSecurityMode->currentIndex() == COMBOX_ID_SERCURE_WPA_EAP
             || m_pCmbSecurityMode->currentIndex() == COMBOX_ID_SERCURE_WPA2_EAP)
            && m_pEditUserName != NULL && m_pEditUserName->text().trimmed().isEmpty())
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_USER_NAME_COULD_NOT_BE_EMPTY);
        objErrorContent.pLineEdit = m_pEditUserName;
        return false;
    }

    if (IsNeedPassword()
            && NULL != m_pEditWPASharedKey && m_pEditWPASharedKey->text().trimmed().isEmpty())
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_WPA_SHARED_KEY_CANNOT_BE_EMPTY);
        objErrorContent.pLineEdit = m_pEditWPASharedKey;
        return false;
    }

    return true;
}

void CDlgWifiAddEdit::GetCurPageData()
{
    if (NULL == m_pEditProfileName || NULL == m_pEditSSID || NULL == m_pCmbSecurityMode
            || NULL == m_pCmbCipherType || NULL == m_pEditUserName || NULL == m_pEditWPASharedKey)
    {
        return;
    }

    // Net ID
    m_wifiInfo.iNetID = m_iNetID;

    // Profile Name
    if (m_pEditProfileName->text().trimmed().isEmpty())
    {
        m_wifiInfo.strProfileName = m_pEditSSID->text().toUtf8().data();
    }
    else
    {
        m_wifiInfo.strProfileName = m_pEditProfileName->text().toUtf8().data();
    }

    m_wifiInfo.strSSID = m_pEditSSID->text().toUtf8().data();

    // security
    m_wifiInfo.iSercureMode = (WIFI_SERCURE_MODE)m_pCmbSecurityMode->itemData(
                                  m_pCmbSecurityMode ->currentIndex(), Qt::UserRole).value<int>();

    // encryption
    m_wifiInfo.iEncrypt = (WIFI_ENCRYPT_TYPE)m_pCmbCipherType->itemData(
                              m_pCmbCipherType->currentIndex(), Qt::UserRole).value<int>();

    // User Name & Password
    if (SERCURE_WPA_EAP == m_wifiInfo.iSercureMode || SERCURE_WPA2_EAP == m_wifiInfo.iSercureMode)
    {
        m_wifiInfo.strEapName = m_pEditUserName->text().toUtf8().data();
        m_wifiInfo.strEapPwd = m_pEditWPASharedKey->text().toUtf8().data();
        m_wifiInfo.strPsk = "";
    }
    else
    {
        m_wifiInfo.strEapName = "";
        m_wifiInfo.strEapPwd = "";
        m_wifiInfo.strPsk = m_pEditWPASharedKey->text().toUtf8().data();
    }
}

bool CDlgWifiAddEdit::GetNetWorkInfoByID(int iNetID, WiFiNetInfo & netInfo)
{
    if (WIFI_UI_ADD_PAGE_TYPE_DETAIL_TO_ADD == m_iPageType)
    {
        return WIFI_GetNetworkInfoByID(iNetID, netInfo);
    }

    return WIFI_GetSavedNetInfoByID(iNetID, netInfo);
}

/*********************************************** others ************************************************/
void CDlgWifiAddEdit::InitData()
{
    m_pCmbCipherType = new QComboBox(this);
    m_pCmbSecurityMode = new QComboBox(this);
    m_pEditProfileName = new QLineEdit(this);
    m_pEditSSID = new QLineEdit(this);
    m_pEditUserName = new QLineEdit(this);
    m_pEditWPASharedKey = new QLineEdit(this);

    if (NULL == m_pEditWPASharedKey || NULL == m_pEditSSID
            || NULL == m_pEditProfileName || NULL == m_pCmbCipherType
            || NULL == m_pEditUserName || NULL == m_pCmbSecurityMode)
    {
        return;
    }

    // ComboBox
    m_pCmbSecurityMode->addItem(LANG_TRANSLATE(TRID_NONE), SERCURE_NONE);
    m_pCmbSecurityMode->addItem(LANG_TRANSLATE(TRID_WEP), SERCURE_WEP);
    m_pCmbSecurityMode->addItem(LANG_TRANSLATE(TRID_WPA_PSK), SERCURE_WPA_PSK);
    m_pCmbSecurityMode->addItem(LANG_TRANSLATE(TRID_WPA2_PSK), SERCURE_WPA2_PSK);
    m_pCmbSecurityMode->addItem(LANG_TRANSLATE(TRID_WPA_EAP), SERCURE_WPA_EAP);
    m_pCmbSecurityMode->addItem(LANG_TRANSLATE(TRID_WPA2_EAP), SERCURE_WPA2_EAP);

    connect(m_pCmbSecurityMode, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCmbModeChanged(int)));

    m_pCmbCipherType->addItem(LANG_TRANSLATE(TRID_NONE), CIPHER_NONE);

    // ComboBox
    qComboBoxUtility::SetComboBoxStyle(m_pCmbSecurityMode, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbCipherType, Qt::black);

    // Edit
    SetLineEditDefaultStyle(m_pEditProfileName);
    SetLineEditDefaultStyle(m_pEditSSID);
    SetLineEditDefaultStyle(m_pEditUserName);
    SetLineEditDefaultStyle(m_pEditWPASharedKey);

    //设置输入框输入的最大长度
    qLineEditUtility::SetMaxLength(m_pEditProfileName, EDIT_TYPE_WIFI_SSID_PROFILE);
    qLineEditUtility::SetMaxLength(m_pEditSSID, EDIT_TYPE_WIFI_SSID_PROFILE);
    qLineEditUtility::SetMaxLength(m_pEditUserName, EDIT_TYPE_NAME_SHORT);
    qLineEditUtility::SetMaxLength(m_pEditWPASharedKey, EDIT_TYPE_WIFI_PWD);

    //为输入框安装输入法
    qInstallIME(m_pEditProfileName, IME_DIAL);
    qInstallIME(m_pEditSSID, IME_DIAL);
    qInstallIME(m_pEditUserName, IME_Name);
    qInstallIME(m_pEditWPASharedKey, IME_LOWER);

    qLineEditUtility::SetCurrentInputMethod(m_pEditUserName, EDIT_TYPE_NAME);

    // 设置回显
    m_pEditWPASharedKey->setEchoMode(QLineEdit::Password);

    m_pCmbCipherType->installEventFilter(this);
    m_pCmbSecurityMode->installEventFilter(this);
    m_pEditProfileName->installEventFilter(this);
    m_pEditSSID->installEventFilter(this);
    m_pEditUserName->installEventFilter(this);
    m_pEditWPASharedKey->installEventFilter(this);
}

void CDlgWifiAddEdit::InitListItem()
{
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_SECURITY_MODE);
    qPair.first = strLab;
    qPair.second = m_pCmbSecurityMode;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PROFILE_NAME);
    qPair.first = strLab;
    qPair.second = m_pEditProfileName;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_SSID);
    qPair.first = strLab;
    qPair.second = m_pEditSSID;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CIPHER_TYPE);
    qPair.first = strLab;
    qPair.second = m_pCmbCipherType;
    m_vecWidgetPair.push_back(qPair);

    if (m_pCmbSecurityMode != NULL &&
            (m_pCmbSecurityMode->currentIndex() == COMBOX_ID_SERCURE_WPA_EAP
             || m_pCmbSecurityMode->currentIndex() == COMBOX_ID_SERCURE_WPA2_EAP))
    {
        qPair.first = LANG_TRANSLATE(TRID_USER_NAME);
        qPair.second = m_pEditUserName;
        m_vecWidgetPair.push_back(qPair);
    }
    else if (m_pEditUserName != NULL)
    {
        m_pEditUserName->hide();
    }

    strLab = LANG_TRANSLATE(TRID_WPA_SHARED_KEY);
    qPair.first = strLab;
    qPair.second = m_pEditWPASharedKey;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
}

int CDlgWifiAddEdit::GetSecurityModeIndexBySecurityType(int nSecurityType)
{
    switch (nSecurityType)
    {
    case SERCURE_WEP:
        return COMBOX_ID_SERCURE_WEP;
    case SERCURE_WPA:
    case SERCURE_WPA_PSK:
        return COMBOX_ID_SERCURE_WPA_PAK;
    case SERCURE_WPA2:
    case SERCURE_WPA2_PSK:
        return COMBOX_ID_SERCURE_WPA2_PAK;
    case SERCURE_WPA_EAP:
        return COMBOX_ID_SERCURE_WPA_EAP;
    case SERCURE_WPA2_EAP:
        return COMBOX_ID_SERCURE_WPA2_EAP;
    case SERCURE_NONE:
    default:
        return COMBOX_ID_SERCURE_NONE;
    }
}

void CDlgWifiAddEdit::OnCmbModeChanged(int iIdx)
{
    if (NULL == m_pCmbSecurityMode)
    {
        return;
    }

    if (m_pCmbCipherType != NULL)
    {
        m_pCmbCipherType->clear();

        switch (m_pCmbSecurityMode->itemData(iIdx, Qt::UserRole).value<int>())
        {
        case SERCURE_NONE:
            m_pCmbCipherType->addItem(LANG_TRANSLATE(TRID_NONE), CIPHER_NONE);
            break;
        case SERCURE_WEP:
            m_pCmbCipherType->addItem(LANG_TRANSLATE(TRID_WEP), CIPHER_W40);
            break;
        case SERCURE_WPA_PSK:
        case SERCURE_WPA2_PSK:
        case SERCURE_WPA_EAP:
        case SERCURE_WPA2_EAP:
            m_pCmbCipherType->addItem(LANG_TRANSLATE(TRID_TKIP), CIPHER_TKIP);
            m_pCmbCipherType->addItem(LANG_TRANSLATE(TRID_AES), CIPHER_CCMP);
            m_pCmbCipherType->addItem(LANG_TRANSLATE(TRID_TKIP_AES), CIPHER_AUTO);
            break;
        default:
            break;
        }

        m_pCmbCipherType->setCurrentIndex(0);
    }

    if (m_pEditWPASharedKey != NULL)
    {
        int nSecurityMode = m_pCmbSecurityMode->itemData(iIdx, Qt::UserRole).value<int>();

        if (SERCURE_WEP == nSecurityMode)
        {
            qLineEditUtility::SetMaxLength(m_pEditWPASharedKey, EDIT_TYPE_WIFI_WEP_PWD);
        }
        else
        {
            qLineEditUtility::SetMaxLength(m_pEditWPASharedKey, EDIT_TYPE_WIFI_PWD);
        }
    }

    InitListItem();
}

void CDlgWifiAddEdit::Print()
{
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()] begin ");
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()]SecurityMode[%d]", m_wifiInfo.iSercureMode);
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()]Profile[%s]",
                   m_wifiInfo.strProfileName.c_str());
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()]SSID[%s]", m_wifiInfo.strSSID.c_str());
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()]CiperType[%d]", m_wifiInfo.iEncrypt);
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()]Passwork[%s]", m_wifiInfo.strPsk.c_str());
    SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::Print()] end ");
}

bool CDlgWifiAddEdit::SavePageData()
{
    if (NULL == m_pEditWPASharedKey || NULL == m_pEditSSID
            || NULL == m_pEditProfileName || NULL == m_pCmbCipherType
            || NULL == m_pEditUserName || NULL == m_pCmbSecurityMode)
    {
        return false;
    }

    ReturnAllEdit();
    GetCurPageData();

    if (m_iPageType != WIFI_UI_ADD_PAGE_TYPE_EDIT)
    {
        if (WIFI_GetSavedNetworkNum() >= MAX_SAVED_NUM)
        {
            _UIWifiManager.Wifi_ShowCommonMsgBox(NULL,
                                                 LANG_TRANSLATE(TRID_LIST_IS_FULL_PLEASE_DELETE_BEFORE_ADD)
                                                 , MESSAGEBOX_Cancel, SHORT_TIME);
        }
        else
        {
            InputDataErrorInfo objErrorInfo;
            if (IsInputDataValid(objErrorInfo))
            {
                Print();
                bool bAdd = WIFI_AddNetwork(&m_wifiInfo);

                SETTINGUI_INFO("[SettingUI][CDlgWifiAddEdit::SavePageData()][add[%d]]", bAdd);
                if (!bAdd)
                {
                    _UIWifiManager.Wifi_ShowCommonMsgBox(NULL,
                                                         LANG_TRANSLATE(TRID_SAVE_FAILED_THE_SAME_PROFILE_NAME_OR_SSID_EXIST),
                                                         MESSAGEBOX_Cancel, SHORT_TIME);
                    return false;
                }
            }
        }
    }
    else if (m_iNetID >= 0)
    {
        InputDataErrorInfo objErrorInfo;
        bool bSt = IsStatusChanged();
        bool bIn = IsInputDataValid(objErrorInfo);

        if (bSt && bIn) // 判断数据合法性
        {
            Print();

            if (WIFI_ModifyNetwork(m_iNetID, &m_wifiInfo))
            {
                _UIWifiManager.SetNeedUpdate(true);
                Print();
            }
            else
            {
                _UIWifiManager.Wifi_ShowCommonMsgBox(NULL,
                                                     LANG_TRANSLATE(TRID_SAVE_FAILED_THE_SAME_PROFILE_NAME_OR_SSID_EXIST), MESSAGEBOX_Cancel,
                                                     SHORT_TIME);
                return false;
            }
        }
    }

    return true;
}

bool CDlgWifiAddEdit::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    for (int i = 0; i < SOFTKEY_NUM; ++i)
    {
        objWndData[i].m_strSoftkeyTitle.clear();
        objWndData[i].m_strSoftkeyAction.clear();
        objWndData[i].m_strIconPath.clear();
    }

    if (NULL != this)
    {
        if (NULL != this->focusWidget()
                && (this->focusWidget()->inherits("QLineEdit")
                    || this->focusWidget()->inherits("QTextEdit")))
        {
            objWndData[0].m_strSoftkeyAction = IME_ACTION;

            objWndData[1].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DELETE);
            objWndData[1].m_strSoftkeyAction = IME_DEL_ACTION;
            objWndData[1].m_strIconPath = PIC_SOFTKEY_DELETE;
        }

        objWndData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_SAVE);
        objWndData[2].m_strSoftkeyAction = ACTION_SAVE_DATA;
        objWndData[2].m_strIconPath = PIC_SOFTKEY_SAVE;
    }
    return true;
}

bool CDlgWifiAddEdit::IsNeedPassword() const
{
    if (NULL == m_pCmbSecurityMode)
    {
        return false;
    }

    int nCurrentType = m_pCmbSecurityMode->currentIndex();

    return (COMBOX_ID_SERCURE_WEP == nCurrentType
            || COMBOX_ID_SERCURE_WPA_PAK == nCurrentType
            || COMBOX_ID_SERCURE_WPA2_PAK == nCurrentType);
}
