#include "model.h"
#include "dsklog/log.h"
#include "path_define.h"
#include "dlgstatusphone.h"
#include "setting/src/statuscontroller.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "commonapi/stringhelperapi.h"


#include "dl/dl.h"
#include "devicelib/networkserver.h"


//构造函数
CDlgStatusPhone::CDlgStatusPhone(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabFirmware(NULL)
    , m_pLabHardware(NULL)
    , m_pLabMAC(NULL)
    , m_pLabWifiMAC(NULL)
    , m_pLabBluetoothMAC(NULL)
    , m_pLabModel(NULL)
    , m_pLabProductID(NULL)
    , m_pLabCertificate(NULL)
    , m_pLabAMRLicense(NULL)
#if IF_BUG_34374
    , m_pLabCfgVerValue(NULL)
#endif
{
    InitPageData();
}

CDlgStatusPhone::~CDlgStatusPhone()
{

}

//界面初始化
void CDlgStatusPhone::InitPageData()
{
    m_pLabFirmware = new QLabel(this);
    m_pLabHardware = new QLabel(this);
    m_pLabMAC = new QLabel(this);
    m_pLabWifiMAC = new QLabel(this);
    m_pLabBluetoothMAC = new QLabel(this);
    m_pLabModel = new QLabel(this);
    m_pLabProductID = new QLabel(this);
    m_pLabCertificate = new QLabel(this);
    m_pLabAMRLicense = new QLabel(this);
#if IF_BUG_34374
    m_pLabCfgVerValue = new qScrollTextLabel(this);
#endif

    //指针判空
    if (m_pLabFirmware == NULL || m_pLabHardware == NULL || m_pLabMAC == NULL
            || m_pLabModel == NULL || m_pLabProductID == NULL || m_pLabCertificate == NULL
            || NULL == m_pLabWifiMAC || NULL == m_pLabBluetoothMAC || NULL == m_pLabAMRLicense
#if IF_BUG_34374
            || NULL == m_pLabCfgVerValue
#endif
       )
    {
        return;
    }
    //设置文字颜色
    SetWgtFontColor(m_pLabModel, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabHardware, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabFirmware, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabProductID, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabMAC, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabWifiMAC, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabBluetoothMAC, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabCertificate, FONT_TYPE_TEXT);
    SetWgtFontColor(m_pLabAMRLicense, FONT_TYPE_TEXT);
#if IF_BUG_34374
    SetWgtFontColor(m_pLabCfgVerValue, FONT_TYPE_TEXT);
#endif

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_PRODUCT_NAME);
    qPair.first = strLab;
    qPair.second = m_pLabModel;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_HARDWARE_VERSION);
    qPair.first = strLab;
    qPair.second = m_pLabHardware;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_FIRMWARE);
    qPair.first = strLab;
    qPair.second = m_pLabFirmware;
    m_vecWidgetPair.push_back(qPair);

#if IF_BUG_34374
    yl::string strValue = configParser_GetConfigString(kszCfgVersion);
    if (!strValue.empty())
    {
        strLab = LANG_TRANSLATE(TRID_CFG_VER);
        qPair.first = strLab;
        qPair.second = m_pLabCfgVerValue;
        m_vecWidgetPair.push_back(qPair);
    }
#endif

    strLab = LANG_TRANSLATE(TRID_RRODUCT_ID);
    qPair.first = strLab;
    qPair.second = m_pLabProductID;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MAC);
    qPair.first = strLab;
    qPair.second = m_pLabMAC;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_WIFI_MAC);
    qPair.second = m_pLabWifiMAC;
    m_vecWidgetPair.push_back(qPair);

    qPair.first = LANG_TRANSLATE(TRID_BLUETOOTH_MAC);
    qPair.second = m_pLabBluetoothMAC;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_DEVICE_CERT);
    qPair.first = strLab;
    qPair.second = m_pLabCertificate;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_AMR_LICENSE);
    qPair.first = strLab;
    qPair.second = m_pLabAMRLicense;

    AMR_LICENSE_RESULT eResult = comminUnits_GetAmrLicenseResult();

    if (eResult == AMR_LICENSE_INSTALL_SUCCESS)
    {
        m_pLabAMRLicense->setText(LANG_TRANSLATE(TRID_INSTALLED));
        m_vecWidgetPair.push_back(qPair);
    }
    else if (eResult == AMR_LICENSE_INSTALL_FAIL)
    {
        m_pLabAMRLicense->setText(LANG_TRANSLATE(TRID_INSTALLED_FAILED));
        m_vecWidgetPair.push_back(qPair);
    }
    else
    {
        m_pLabAMRLicense->hide();
    }

    AddWidget(m_vecWidgetPair);

    //主动取消默认的Enter保存效果
    //disconnect(m_pGeneralFrame, SIGNAL(ProcessCurrentItem(const QString&)), this, SIGNAL(ProcessPageAction(const QString&)));

}

bool CDlgStatusPhone::LoadPageData()
{
    this->setFocus();
    PhoneStatusData phoneStatusData;

    //指针判空
    if (m_pLabFirmware == NULL || m_pLabHardware == NULL || m_pLabMAC == NULL
            || m_pLabModel == NULL || m_pLabProductID == NULL || m_pLabCertificate == NULL
            || NULL == m_pLabWifiMAC || NULL == m_pLabBluetoothMAC)
    {
        return false;
    }

    if (g_pStatusController->GetPhoneStatusData(&phoneStatusData))
    {
        m_pLabModel->setText(toQString(phoneStatusData.m_strModel.c_str()));
        m_pLabHardware->setText(toQString(phoneStatusData.m_strHardWare.c_str()));
        m_pLabFirmware->setText(toQString(phoneStatusData.m_strFirmWare.c_str()));
        m_pLabProductID->setText(toQString(phoneStatusData.m_strProductId.c_str()));
        m_pLabMAC->setText(toQString(phoneStatusData.m_strMac.c_str()));
        if (GetDeviceCertStatus())
        {
            m_pLabCertificate->setText(LANG_TRANSLATE(TRID_FACTORY_INSTALLED));
        }
        else
        {
            m_pLabCertificate->setText(LANG_TRANSLATE(TRID_NOT_INSTALLED));
        }
    }

    ExternalDeviceData stExtrernalDevice;
    if (g_pStatusController->GetExternalDeviceData(&stExtrernalDevice))
    {
        if (stExtrernalDevice.m_strWifiMac.empty())
        {
            m_pLabWifiMAC->setText(LANG_TRANSLATE(TRID_NA));
        }
        else
        {
            m_pLabWifiMAC->setText(toQString(stExtrernalDevice.m_strWifiMac));
        }

        if (stExtrernalDevice.m_strBluetoothMac.empty())
        {
            m_pLabBluetoothMAC->setText(LANG_TRANSLATE(TRID_NA));
        }
        else
        {
            m_pLabBluetoothMAC->setText(toQString(stExtrernalDevice.m_strBluetoothMac));
        }
    }

#if IF_BUG_34374
    yl::string strValue = configParser_GetConfigString(kszCfgVersion);
    if (!strValue.empty())
    {
        m_pLabCfgVerValue->setText(toQString(strValue.c_str()));
    }
#endif

    return true;
}

//状态显示softkey没有保存项
bool CDlgStatusPhone::GetSoftkeyData(CArraySoftKeyBarData & objSoftData)
{
    objSoftData[3].m_strSoftkeyAction = "";
    objSoftData[3].m_strSoftkeyTitle  = "";

    return true;
}

bool CDlgStatusPhone::IsStatusChanged()
{
    return false;
}

bool CDlgStatusPhone::SavePageData()
{
    return true;
}

bool CDlgStatusPhone::GetDeviceCertStatus()
{
    dl_ca_info_ex info_ex;
    dl_response_t ret_response;


    memset(&info_ex, 0, sizeof(dl_ca_info_ex));
    /* cert_flags证书类型，普通CA为0；客户端证书DL_CLIENT_CERT */
    if (dl_get_ca_info_ex(szDeviceCertDefaultFile, &info_ex, DL_CLIENT_CERT, &ret_response))
    {
        SETTINGUI_INFO("dl_cert_verify  Error %d: %s", ret_response.code, ret_response.code_msg);
        return false;
    }
    else
    {
        SETTINGUI_INFO("          version = %d", info_ex.version);
        SETTINGUI_INFO("          serial_num = %s", info_ex.serial_num);
        SETTINGUI_INFO("          signature_algorithm = %s", info_ex.signature_algorithm);
        SETTINGUI_INFO("          pubkey_algorithm = %s", info_ex.pubkey_algorithm);
        SETTINGUI_INFO("          pubkey_bits = %d", info_ex.pubkey_bits);
        SETTINGUI_INFO("          priv_key_algor = %s", info_ex.priv_key_algor);
        SETTINGUI_INFO("          priv_key_bits = %d", info_ex.priv_key_bits);
        SETTINGUI_INFO("          issued_to = %s", info_ex.issued_to);
        SETTINGUI_INFO("          issued_by = %s", info_ex.issued_by);
        SETTINGUI_INFO("          validity_begin = %s", info_ex.validity_begin);
        SETTINGUI_INFO("          expire_date = %s", info_ex.expire_date);
        SETTINGUI_INFO("          hash = 0x%x", info_ex.hash);
        SETTINGUI_INFO("          extensions_num = %d", info_ex.extensions_num);
    }
    yl::string strMac = commonAPI_StringConvert(netGetMACText(NULL), STRING_CONVERT_TO_LOWER);
    yl::string strInfo = commonAPI_StringConvert(info_ex.issued_to, STRING_CONVERT_TO_LOWER);
    SETTINGUI_INFO("          strInfo = %s", strInfo.c_str());
    SETTINGUI_INFO("          strMac = %s", strMac.c_str());
    if (strInfo.compare(strMac) == 0)
    {
        /**
        * ca_path，指定信任证书CA路径，CA证书以hash值命名存放，可通过导入到话机中查看证书hash值，然后预制到phoneca/目录下
        * ca_file，指定信任证书CA文件
        * cert_flags证书类型，普通CA为0，客户端证书DL_CLIENT_CERT
        */
        if (dl_cert_verify((LPSTR)szDeviceCertDefaultFile, (LPSTR)szTrustCertPath, NULL, DL_CLIENT_CERT, &ret_response))
        {
            SETTINGUI_INFO("dl_cert_verify  Error %d: %s", ret_response.code, ret_response.code_msg);
            return false;
        }
    }
    else
    {
        return false;
    }

    SETTINGUI_INFO("GetDeviceCertStatus end");
    return true;
}
