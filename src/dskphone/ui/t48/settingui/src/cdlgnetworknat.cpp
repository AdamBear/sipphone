#include "cdlgnetworknat.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "setting/include/authorityiddefine.h"
#include "setting/include/common.h"
#include "setting/src/networkcontroller.h"
#include "networksettingmanager.h"
#include "dsklog/log.h"
#include "uikernel/qcheckbox.h"
#include "uikernel/qlineeditutility.h"

CDlgNetworkNAT::CDlgNetworkNAT(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckStunStatus(NULL)
    , m_pEditStunServerAddr(NULL)
    , m_pEditStunServerPort(NULL)
{
    InitData();
}

CDlgNetworkNAT::~CDlgNetworkNAT()
{
    //
}

void CDlgNetworkNAT::InitData()
{
    m_pCheckStunStatus = new qCheckBox(this);
    m_pEditStunServerAddr = new QLineEdit(this);
    m_pEditStunServerPort = new QLineEdit(this);

    if (NULL == m_pCheckStunStatus
            || NULL == m_pEditStunServerAddr
            || NULL == m_pEditStunServerPort)
    {
        return;
    }

//  m_pCheckStunStatus->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
//  m_pCheckStunStatus->addItem(LANG_TRANSLATE(TRID_ENABLED), true);

    SetLineEditDefaultStyle(m_pEditStunServerAddr);
    qLineEditUtility::SetMaxLength(m_pEditStunServerAddr, EDIT_TYPE_SERVER);
    qInstallIME(m_pEditStunServerAddr, IME_ALL);

    SetLineEditDefaultStyle(m_pEditStunServerPort);
    qLineEditUtility::SetMaxLength(m_pEditStunServerPort, EDIT_TYPE_IP_PORT);
    qInstallIME(m_pEditStunServerPort, IME_123);

    //添加控件
    QWidgetPair qPair;
    QString strLab;
    strLab = LANG_TRANSLATE(TRID_NAT_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckStunStatus;
    qPair.strAuthorId = kszRegisterNATTraversal;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_STUN_SERVER);
    qPair.first = strLab;
    qPair.second = m_pEditStunServerAddr;
    qPair.strAuthorId = kszRegisterSTUNServer;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_STUN_PORT);
    qPair.first = strLab;
    qPair.second = m_pEditStunServerPort;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    m_pCheckStunStatus->hide();
    m_pEditStunServerAddr->hide();
    m_pEditStunServerPort->hide();

    m_pCheckStunStatus->installEventFilter(this);
    m_pEditStunServerAddr->installEventFilter(this);
    m_pEditStunServerPort->installEventFilter(this);
}


bool CDlgNetworkNAT::LoadPageData()
{
    if (NULL == m_pCheckStunStatus
            || NULL == m_pEditStunServerAddr
            || NULL == m_pEditStunServerPort)
    {
        return false;
    }

    qLineEditUtility::SetCurrentInputMethod(m_pEditStunServerAddr, EDIT_TYPE_SERVER);
    qLineEditUtility::SetCurrentInputMethod(m_pEditStunServerPort, EDIT_TYPE_IP_PORT);


    NetworkStunData stunData;
    if (NULL != g_pNetworkController && g_pNetworkController->GetStunData(&stunData))
    {
        m_pCheckStunStatus->SetData(stunData.m_bEnable);
        m_pEditStunServerAddr->setText(stunData.m_strStunSeverAddr.c_str());
        m_pEditStunServerPort->setText(QString::number(stunData.m_iPort));
    }
    return true;
}

bool CDlgNetworkNAT::IsStatusChanged()
{
    NetworkStunData pageData;
    NetworkStunData configData;

    if (GetPageData(pageData) && NULL != g_pNetworkController
            && g_pNetworkController->GetStunData(&configData))
    {
        if (pageData.m_bEnable != configData.m_bEnable
                || pageData.m_strStunSeverAddr != configData.m_strStunSeverAddr
                || pageData.m_iPort != configData.m_iPort)
        {
            return true;
        }
    }

    return false;
}

bool CDlgNetworkNAT::SavePageData()
{
    if (!IsStatusChanged())
    {
        return true;
    }

    NetworkStunData stunData;
    if (GetPageData(stunData))
    {
        g_pNetworkController->SetStunData(stunData);
        // 设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return true;
}

bool CDlgNetworkNAT::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (NULL != m_pCheckStunStatus && NULL != m_pEditStunServerAddr)
    {
        if (m_pCheckStunStatus->GetData())
        {
            if (m_pEditStunServerAddr->text().isEmpty()
                    || !qLineEditUtility::IsFormatValid(m_pEditStunServerAddr->text(), EDIT_TYPE_SIP_SERVER))
            {
                objErrorContent.strErrorContent = TRID_STUN_SERVER_IS_INVALID;
                objErrorContent.pLineEdit = m_pEditStunServerAddr;
                return false;
            }
        }
    }

    if (NULL != m_pEditStunServerPort)
    {
        bool bPortValid = true;
        if (m_pEditStunServerPort->text().isEmpty())
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

bool CDlgNetworkNAT::GetPageData(NetworkStunData & refData)
{
    if (NULL ==  m_pCheckStunStatus
            || NULL == m_pEditStunServerAddr
            || NULL == m_pEditStunServerPort)
    {
        return false;
    }

    refData.m_bEnable = m_pCheckStunStatus->GetData();
    refData.m_strStunSeverAddr = m_pEditStunServerAddr->text().toUtf8().data();
    if (!m_pEditStunServerPort->text().isEmpty())
    {
        refData.m_iPort = m_pEditStunServerPort->text().toInt();
    }

    return true;
}
