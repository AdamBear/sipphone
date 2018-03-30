#include "dlgzeronetwork_nat.h"

#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcheckbox.h"
#include "inputmethod/inputmethod.h"
#include "setting/include/authorityiddefine.h"
#include "setting/include/common.h"
#include "setting/src/networkcontroller.h"
#include "settingui/src/networksettingmanager.h"

CDlgZeroNetwork_NAT::CDlgZeroNetwork_NAT(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckBoxNatStatus(NULL)
    , m_pEditStunServerAddr(NULL)
    , m_pEditStunServerPort(NULL)
{
    InitData();
}

CDlgZeroNetwork_NAT::~CDlgZeroNetwork_NAT()
{
    qUninstallIME(m_pEditStunServerPort);
    qUninstallIME(m_pEditStunServerAddr);
    m_vecWidgetPair.clear();
}

void CDlgZeroNetwork_NAT::InitData()
{
    m_pCheckBoxNatStatus = new qCheckBox(this);
    m_pEditStunServerAddr = new QLineEdit(this);
    m_pEditStunServerPort = new QLineEdit(this);

    if (NULL == m_pCheckBoxNatStatus
            || NULL == m_pEditStunServerAddr
            || NULL == m_pEditStunServerPort)
    {
        return;
    }

    SetLineEditDefaultStyle(m_pEditStunServerAddr);
    qLineEditUtility::SetMaxLength(m_pEditStunServerAddr, EDIT_TYPE_SERVER);
    qInstallIME(m_pEditStunServerAddr, IME_ALL);

    SetLineEditDefaultStyle(m_pEditStunServerPort);
    qLineEditUtility::SetMaxLength(m_pEditStunServerPort, EDIT_TYPE_IP_PORT);
    qInstallIME(m_pEditStunServerPort, IME_123);

    m_pEditStunServerAddr->hide();
    m_pEditStunServerPort->hide();

    m_pCheckBoxNatStatus->installEventFilter(this);
    m_pEditStunServerAddr->installEventFilter(this);
    m_pEditStunServerPort->installEventFilter(this);

    connect(m_pCheckBoxNatStatus, SIGNAL(checkChanged(bool)), this,
            SLOT(OnChkBoxEnableNATChanged(bool)));
}


bool CDlgZeroNetwork_NAT::LoadPageData()
{
    if (NULL == m_pCheckBoxNatStatus
            || NULL == m_pEditStunServerAddr
            || NULL == m_pEditStunServerPort)
    {
        return false;
    }

    this->blockSignals(true);
    qLineEditUtility::SetCurrentInputMethod(m_pEditStunServerAddr, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditStunServerPort, EDIT_TYPE_IP_PORT);


    NetworkStunData stunData;
    if (NULL != g_pNetworkController && g_pNetworkController->GetStunData(&stunData))
    {
        m_pCheckBoxNatStatus->SetData(stunData.m_bEnable);
        m_pEditStunServerAddr->setText(stunData.m_strStunSeverAddr.c_str());
        m_pEditStunServerPort->setText(QString::number(stunData.m_iPort));
    }
    this->blockSignals(false);
    return true;
}

bool CDlgZeroNetwork_NAT::IsStatusChanged()
{
    NetworkStunData pageData;
    NetworkStunData configData;

    if (GetPageData(pageData) && NULL != g_pNetworkController
            && g_pNetworkController->GetStunData(&configData))
    {
        if (pageData.m_bEnable != configData.m_bEnable
                || (pageData.m_bEnable && pageData.m_strStunSeverAddr != configData.m_strStunSeverAddr)
                || (pageData.m_bEnable && pageData.m_iPort != configData.m_iPort))
        {
            return true;
        }
    }

    return false;
}

bool CDlgZeroNetwork_NAT::SavePageData()
{
    if (!IsStatusChanged())
    {
        return true;
    }

    NetworkStunData stunData;
    if (GetPageData(stunData))
    {
        if (!stunData.m_bEnable)
        {
            NetworkStunData logicData;
            g_pNetworkController->GetStunData(&logicData);
            logicData.m_bEnable = stunData.m_bEnable;

            g_pNetworkController->SetStunData(logicData);
        }
        else
        {
            g_pNetworkController->SetStunData(stunData);
        }

        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

bool CDlgZeroNetwork_NAT::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL == m_pCheckBoxNatStatus || NULL == m_pEditStunServerPort
            || NULL == m_pEditStunServerAddr)
    {
        return true;
    }

    bool bEnable = m_pCheckBoxNatStatus->GetData();
    if (bEnable)
    {
        // Server
        if (m_pEditStunServerAddr->text().trimmed().isEmpty()
                || !qLineEditUtility::IsFormatValid(m_pEditStunServerAddr->text(), EDIT_TYPE_SIP_SERVER))
        {
            objErrorContent.strErrorContent = TRID_STUN_SERVER_IS_INVALID;
            objErrorContent.pLineEdit = m_pEditStunServerAddr;
            return false;
        }

        // Port
        bool bPortValid = true;
        if (m_pEditStunServerPort->text().trimmed().isEmpty())
        {
            bPortValid = false;
        }
        else
        {
            int iPort = m_pEditStunServerPort->text().toInt();
            if (iPort < 1024 || iPort > 65000)
            {
                bPortValid = false;
            }
        }

        if (!bPortValid)
        {
            objErrorContent.pLineEdit = m_pEditStunServerPort;
            objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_STUN_PORT_IS_INVALID);
            return false;
        }
    }

    return true;
}

bool CDlgZeroNetwork_NAT::GetPageData(NetworkStunData & refData)
{
    if (NULL ==  m_pCheckBoxNatStatus
            || NULL == m_pEditStunServerAddr
            || NULL == m_pEditStunServerPort)
    {
        return false;
    }

    refData.m_bEnable = m_pCheckBoxNatStatus->GetData();
    refData.m_strStunSeverAddr = m_pEditStunServerAddr->text().toUtf8().data();
    if (!m_pEditStunServerPort->text().isEmpty())
    {
        refData.m_iPort = m_pEditStunServerPort->text().toInt();
    }

    return true;
}

void CDlgZeroNetwork_NAT::OnChkBoxEnableNATChanged(bool bOn)
{
    m_pEditStunServerAddr->hide();
    m_pEditStunServerPort->hide();

    QString strLab = "";
    QWidgetPair qPair;
    m_vecWidgetPair.clear();

    strLab = LANG_TRANSLATE(TRID_NAT_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckBoxNatStatus;
    qPair.strAuthorId = kszRegisterNATTraversal;
    m_vecWidgetPair.push_back(qPair);

    if (bOn)
    {
        strLab = LANG_TRANSLATE(TRID_STUN_SERVER);
        qPair.first = strLab;
        qPair.second = m_pEditStunServerAddr;
        qPair.strAuthorId = kszRegisterNATTraversal;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_STUN_PORT);
        qPair.first = strLab;
        qPair.second = m_pEditStunServerPort;
        m_vecWidgetPair.push_back(qPair);
    }

    emit ReLayout(this);
}
