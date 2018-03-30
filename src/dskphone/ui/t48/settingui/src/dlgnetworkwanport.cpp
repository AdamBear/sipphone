#include "model.h"
#include "dlgnetworkwanport.h"
#include "setting/src/networkcontroller.h"

#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"

#include "devicelib/phonedevice.h"
#include "devicelib/networkserver.h"
#include "setting/src/networkcontroller.h"
#include "networksettingmanager.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qlineeditutility.h"
#include "inputmethod/inputmethod.h"
#include "edkui/edkmapdefine.h"

Q_DECLARE_METATYPE(WanType);
Q_DECLARE_METATYPE(IPv6WanType);

CDlgNetworkWANPort::CDlgNetworkWANPort(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxType(NULL)
#if IF_FEATURE_PPPOE
    , m_pEditPPPoEUser(NULL)
    , m_pEditPPPoEPWD(NULL)
#endif
    , m_pEditIP(NULL)
    , m_pEditSubnetMask(NULL)
    , m_pEditGateway(NULL)
    , m_pEditPriDNS(NULL)
    , m_pEditSecDNS(NULL)
    , m_pCmbBoxEnable(NULL)
    , m_pEditDHCPPriDNS(NULL)
    , m_pEditDHCPSecDNS(NULL)
    , m_nMode(0)
{
    InitData();
}

CDlgNetworkWANPort::~CDlgNetworkWANPort()
{

}

void CDlgNetworkWANPort::BeforeSetSubPage(void * pData)
{
    if (NULL == pData)
    {
        return;
    }

    m_nMode = *((int *)pData);
    if (m_nMode == IPAM_IPV4)
    {
        m_strTitle = LANG_TRANSLATE(TRID_IPV4_WAN_PORT).toUtf8().data();
        //设置提示模式action
        SetHintTitle(EDK_IPV4);
    }
    else
    {
        m_strTitle = LANG_TRANSLATE(TRID_IPV6_WAN_PORT).toUtf8().data();
        //设置提示模式action
        SetHintTitle(EDK_IPV6);
    }

    AddPageContent();
}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool CDlgNetworkWANPort::LoadPageData()
{
    //设置首选输入法
#if IF_FEATURE_PPPOE
    // IPV4 0 or IPV6 1 ipv6没有pppoe
    if (m_nMode == IPAM_IPV4)
    {
        qLineEditUtility::SetCurrentInputMethod(m_pEditPPPoEUser, EDIT_TYPE_LOWER);     //abc?
        qLineEditUtility::SetCurrentInputMethod(m_pEditPPPoEPWD, EDIT_TYPE_PASSWORD);
    }
#endif

    qLineEditUtility::SetCurrentInputMethod(m_pEditIP, EDIT_TYPE_IPV4);
    qLineEditUtility::SetCurrentInputMethod(m_pEditSubnetMask, EDIT_TYPE_IPV4_NETMASK);
    qLineEditUtility::SetCurrentInputMethod(m_pEditGateway, EDIT_TYPE_IPV4_NETMASK);
    qLineEditUtility::SetCurrentInputMethod(m_pEditPriDNS, EDIT_TYPE_IPV4);
    qLineEditUtility::SetCurrentInputMethod(m_pEditSecDNS, EDIT_TYPE_IPV4);
    qLineEditUtility::SetCurrentInputMethod(m_pEditDHCPPriDNS, EDIT_TYPE_IPV4);
    qLineEditUtility::SetCurrentInputMethod(m_pEditDHCPSecDNS, EDIT_TYPE_IPV4);

    // 当前进入的是IPV4还是IPV6界面
    m_eInitDnsServerData.m_nMode = m_nMode;
    //调用控制层：要读取全部数据，再根据类型进行显示
    bool bIsSuccess;
    if (m_nMode == IPAM_IPV4)
    {
        bIsSuccess = g_pNetworkController->GetWanTypeData(&m_eInitWanType)
#if IF_FEATURE_PPPOE
                     && g_pNetworkController->GetWanPppoeData(&m_eInitPPPoE)
#endif
                     && g_pNetworkController->GetWanStaticData(&m_eInitStaticIp)
                     && g_pNetworkController->GetWanStaticDnsServerData(&m_eInitDnsServerData);
    }
    else
    {
        bIsSuccess = g_pNetworkController->GetIPv6WanTypeData(&m_eInitWanType)
#if IF_FEATURE_PPPOE
                     && g_pNetworkController->GetWanPppoeData(&m_eInitPPPoE)
#endif
                     && g_pNetworkController->GetWanStaticIPv6Data(&m_eInitStaticIp)
                     && g_pNetworkController->GetWanStaticDnsServerData(&m_eInitDnsServerData);
    }

    if (bIsSuccess)
    {
        if (m_nMode == IPAM_IPV4)
        {
            //加载数据成功，设置当前页面数据
            SetCurrentPageData(m_eInitWanType);
#if IF_FEATURE_PPPOE
            SetCurrentPageData(m_eInitPPPoE);
#endif
            SetCurrentPageData(m_eInitStaticIp);
            SetCurrentPageData(m_eInitDnsServerData);
        }
        else
        {
            SetCurrentPageData(m_eInitWanType);
            SetCurrentPageData(m_eInitStaticIp);
            SetCurrentPageData(m_eInitDnsServerData);
        }
        return true;
    }
    else
    {
        //加载数据失败，返回false
        return false;
    }
}


//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgNetworkWANPort::IsStatusChanged()
{
    // 先判断WAN类型是否改变
    if (m_eInitWanType.m_eType != m_eCurrentPageWanType.m_eType)
    {
        return true;
    }

    //配置类型没有变化,则比较下面的配置是否有变化
    bool bResult = false;
    if (m_nMode == IPAM_IPV4)
    {
        switch (m_eCurrentPageWanType.m_eType)
        {
        case WAN_DHCP:
            {
                NetworkWanDnsServerData currentPageDataStaticDnsServer;

                GetCurrentPageData(currentPageDataStaticDnsServer);

                if (!ComparePageData(m_eInitDnsServerData, currentPageDataStaticDnsServer))
                {
                    bResult = true;
                }
            }
            break;
#if IF_FEATURE_PPPOE
        case WAN_PPPOE:
            {
                NetworkWanPppoeData currentPageDataPPPoE;

                GetCurrentPageData(currentPageDataPPPoE);

                if (!ComparePageData(m_eInitPPPoE, currentPageDataPPPoE))
                {
                    //前后页面数据没有改变，返回false
                    bResult = true;
                }
            }
            break;
#endif
        case WAN_STATIC:
            {
                NetworkWanStaticIpData currentPageDataStaticIp;

                GetCurrentPageData(currentPageDataStaticIp);

                if (!ComparePageData(m_eInitStaticIp, currentPageDataStaticIp))
                {
                    bResult = true;
                }
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (m_eCurrentPageWanType.m_eType)
        {
        case IPv6_WAN_DHCP:
            {
                NetworkWanDnsServerData currentPageDataStaticDnsServer;

                GetCurrentPageData(currentPageDataStaticDnsServer);

                if (!ComparePageData(m_eInitDnsServerData, currentPageDataStaticDnsServer))
                {
                    bResult = true;
                }
            }
            break;
        case IPv6_WAN_STATIC:
            {
                NetworkWanStaticIpData currentPageDataStaticIp;

                GetCurrentPageData(currentPageDataStaticIp);

                if (!ComparePageData(m_eInitStaticIp, currentPageDataStaticIp))
                {
                    bResult = true;
                }
            }
            break;
        default:
            break;
        }
    }

    return bResult;
}

void CDlgNetworkWANPort::ReturnAllEdit()
{
    qReturnEdit(m_pEditIP);
    qReturnEdit(m_pEditSubnetMask);
    qReturnEdit(m_pEditGateway);
    qReturnEdit(m_pEditPriDNS);
    qReturnEdit(m_pEditSecDNS);
    qReturnEdit(m_pEditDHCPPriDNS);
    qReturnEdit(m_pEditDHCPSecDNS);

#if IF_FEATURE_PPPOE
    if (m_nMode == IPAM_IPV4)
    {
        qReturnEdit(m_pEditPPPoEUser);
        qReturnEdit(m_pEditPPPoEPWD);
    }
#endif
}

// 卸载界面上所有的输入法
void CDlgNetworkWANPort::UninstallAllIME()
{
    qUninstallIME(m_pEditIP);
    qUninstallIME(m_pEditSubnetMask);
    qUninstallIME(m_pEditGateway);
    qUninstallIME(m_pEditPriDNS);
    qUninstallIME(m_pEditSecDNS);
#if IF_FEATURE_PPPOE
    qUninstallIME(m_pEditPPPoEUser);
    qUninstallIME(m_pEditPPPoEPWD);
#endif
    qUninstallIME(m_pEditDHCPPriDNS);
    qUninstallIME(m_pEditDHCPSecDNS);
}

//从V层读取当前界面的配置，通过C层保存
bool CDlgNetworkWANPort::SavePageData()
{
    //提起页面数据
    if (IsStatusChanged())
    {
        QString strContent;
        QString strTitle;

        m_eInitWanType.m_eType = m_eCurrentPageWanType.m_eType;

        if (m_nMode == IPAM_IPV4)
        {
            //保存网络类型
            g_pNetworkController->SetWanTypeData(m_eCurrentPageWanType);
            //调用控制层：保存数据
            switch (m_eCurrentPageWanType.m_eType)
            {
            case WAN_DHCP:
                {
                    NetworkWanDnsServerData currentPageData;
                    GetCurrentPageData(currentPageData);

                    //保存网络类型
                    g_pNetworkController->SetWanStaticDnsServerData(currentPageData);
                }
                break;
#if IF_FEATURE_PPPOE
            case WAN_PPPOE:
                {
                    //先判断VLAN状态，如果是开启的，则要将其关闭
                    NetworkVlanData configVlanData;
                    bool bIsSuccess = g_pNetworkController->GetVlanData(&configVlanData);
                    if (bIsSuccess)
                    {
                        if (configVlanData.m_isWanEnable)
                        {
                            configVlanData.m_isWanEnable = false;
                        }

                        if (configVlanData.m_isPcEnable)
                        {
                            configVlanData.m_isPcEnable = false;
                        }

                        g_pNetworkController->SetVlanData(configVlanData);
                    }

                    NetworkWanPppoeData currentPageDataPPPoE;
                    GetCurrentPageData(currentPageDataPPPoE);
                    GetCurrentPageData(m_eInitPPPoE);

                    g_pNetworkController->SetWanPppoeData(currentPageDataPPPoE);
                }
                break;
#endif
            case WAN_STATIC:
                {
                    NetworkWanStaticIpData currentPageDataStaticIp;
                    GetCurrentPageData(currentPageDataStaticIp);
                    GetCurrentPageData(m_eInitStaticIp);

                    g_pNetworkController->SetWanStaticData(currentPageDataStaticIp);
                }
                break;
            default:
                break;
            }
        }
        else
        {
            //保存网络类型
            g_pNetworkController->SetIPv6WanTypeData(m_eCurrentPageWanType);

            //调用控制层：保存数据
            switch (m_eCurrentPageWanType.m_eType)
            {
            case IPv6_WAN_DHCP:
                {
                    NetworkWanDnsServerData currentPageData;
                    GetCurrentPageData(currentPageData);

                    //保存网络类型
                    g_pNetworkController->SetWanStaticDnsServerData(currentPageData);
                }
                break;
            case IPv6_WAN_STATIC:
                {
                    NetworkWanStaticIpData currentPageDataStaticIp;
                    GetCurrentPageData(currentPageDataStaticIp);
                    GetCurrentPageData(m_eInitStaticIp);

                    g_pNetworkController->SetWanStaticIPv6Data(currentPageDataStaticIp);
                }
                break;
            default:
                break;
            }
        }

        //网络设置有改变并进行了保存，设置网络改变的标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return true;
}

//IP type配置选择类型变化处理操作
void CDlgNetworkWANPort::OnCmbBoxTypeChanged(int nIndex)
{
    //默认隐藏
    MakeReadyForRelayout();

    QString strLab = "";
    QWidgetPair qPair;

    if (nIndex < 0 || nIndex >= m_pCmbBoxType->count())
    {
        return;
    }

    if (m_nMode == IPAM_IPV4)
    {
        //记录当前ip 配置类型
        m_eCurrentPageWanType.m_eType = m_pCmbBoxType->itemData(nIndex).value<WanType>();
    }
    else
    {
        //记录当前ip 配置类型
        m_eCurrentPageWanType.m_eType = (WanType)m_pCmbBoxType->itemData(nIndex).value<IPv6WanType>();
    }

    strLab = LANG_TRANSLATE(TRID_TYPE);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxType;
    m_vecWidgetPair.push_back(qPair);

    if (m_nMode == IPAM_IPV4)
    {
        switch (m_eCurrentPageWanType.m_eType)
        {
        case WAN_DHCP:
            {
                strLab = LANG_TRANSLATE(TRID_STATIC_DNS);
                qPair.first = strLab;
                qPair.second = m_pCmbBoxEnable;
                m_vecWidgetPair.push_back(qPair);

                int nIndex = m_pCmbBoxEnable->currentIndex();
                if (m_pCmbBoxEnable->itemData(nIndex, Qt::UserRole).value<bool>())
                {
                    strLab = LANG_TRANSLATE(TRID_RRIMARY_DNS);
                    qPair.first = strLab;
                    qPair.second = m_pEditDHCPPriDNS;
                    m_vecWidgetPair.push_back(qPair);

                    strLab = LANG_TRANSLATE(TRID_SECONDARY_DNS);
                    qPair.first = strLab;
                    qPair.second = m_pEditDHCPSecDNS;
                    m_vecWidgetPair.push_back(qPair);
                }
            }
            break;
#if IF_FEATURE_PPPOE
        case WAN_PPPOE:
            {
                strLab = LANG_TRANSLATE(TRID_PPPOE_USER);
                qPair.first = strLab;
                qPair.second = m_pEditPPPoEUser;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_PPPOE_PWD);
                qPair.first = strLab;
                qPair.second = m_pEditPPPoEPWD;
                m_vecWidgetPair.push_back(qPair);
            }
            break;
#endif
        case WAN_STATIC:
            {
                strLab = LANG_TRANSLATE(TRID_IP_ADDRESS);
                qPair.first = strLab;
                qPair.second = m_pEditIP;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_SUBNET_MASK);
                qPair.first = strLab;
                qPair.second = m_pEditSubnetMask;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_DEFAULT_GATEWAY);
                qPair.first = strLab;
                qPair.second = m_pEditGateway;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_RRIMARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditPriDNS;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_SECONDARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditSecDNS;
                m_vecWidgetPair.push_back(qPair);
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (m_eCurrentPageWanType.m_eType)
        {
        case IPv6_WAN_DHCP:
            {
                strLab = LANG_TRANSLATE(TRID_STATIC_DNS);
                qPair.first = strLab;
                qPair.second = m_pCmbBoxEnable;
                m_vecWidgetPair.push_back(qPair);

                int nIndex = m_pCmbBoxEnable->currentIndex();
                if (m_pCmbBoxEnable->itemData(nIndex, Qt::UserRole).value<bool>())
                {
                    strLab = LANG_TRANSLATE(TRID_RRIMARY_DNS);
                    qPair.first = strLab;
                    qPair.second = m_pEditDHCPPriDNS;
                    m_vecWidgetPair.push_back(qPair);

                    strLab = LANG_TRANSLATE(TRID_SECONDARY_DNS);
                    qPair.first = strLab;
                    qPair.second = m_pEditDHCPSecDNS;
                    m_vecWidgetPair.push_back(qPair);
                }
            }
            break;
        case IPv6_WAN_STATIC:
            {
                strLab = LANG_TRANSLATE(TRID_IP_ADDRESS);
                qPair.first = strLab;
                qPair.second = m_pEditIP;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_IPV6_PREFIX);
                qPair.first = strLab;
                qPair.second = m_pEditSubnetMask;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_DEFAULT_GATEWAY);
                qPair.first = strLab;
                qPair.second = m_pEditGateway;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_RRIMARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditPriDNS;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_SECONDARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditSecDNS;
                m_vecWidgetPair.push_back(qPair);
            }
            break;
        default:
            break;
        }
    }

    if (m_ePageMode != PAGE_MODE_ZERO)
    {
        AddWidget(m_vecWidgetPair);
        this->UpdateWnd();
    }
}

void CDlgNetworkWANPort::OnSwitchDnsServerStatus(int nIndex)
{
    bool bRelayout = false;
    int iCbTypeIndex = m_pCmbBoxType->currentIndex();

    if (m_nMode == IPAM_IPV4)
    {
        bRelayout = WAN_DHCP == m_pCmbBoxType->itemData(iCbTypeIndex).value<WanType>();
    }
    else
    {
        bRelayout = IPv6_WAN_DHCP == m_pCmbBoxType->itemData(iCbTypeIndex).value<IPv6WanType>();
    }

    if (!bRelayout)
    {
        return;
    }

    OnCmbBoxTypeChanged(iCbTypeIndex);

    // 设置焦点
    if (m_pCmbBoxEnable != NULL
            && m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->FocusItemByDataIndex(1);
        //m_pCmbBoxEnable->setFocus();
    }
}

//用于设置配置类型项
void CDlgNetworkWANPort::SetCurrentPageData(NetworkWanTypeData & currentPageData)
{
    int nCount = 0;
    nCount = m_pCmbBoxType->count();
    for (int i = 0; i < nCount; i++)
    {
        if (m_nMode == IPAM_IPV4
                && (currentPageData.m_eType
                    == m_pCmbBoxType->itemData(i).value<WanType>()))
        {
            m_pCmbBoxType->setCurrentIndex(i);
            //确保隐藏显示操作正确
            //OnCmbBoxTypeChanged(i);
            break;
        }
        else if ((currentPageData.m_eType
                  == m_pCmbBoxType->itemData(i).value<IPv6WanType>()))
        {
            m_pCmbBoxType->setCurrentIndex(i);
            //确保隐藏显示操作正确
            //OnCmbBoxTypeChanged(i);
            break;
        }
    }
}

#if IF_FEATURE_PPPOE
//比较页面前后数据
bool CDlgNetworkWANPort::ComparePageData(const NetworkWanPppoeData & beforePageData,
        const NetworkWanPppoeData & afterPageData) const
{
    if (beforePageData.m_strPppoeUser != afterPageData.m_strPppoeUser
            || beforePageData.m_strPppoePin != afterPageData.m_strPppoePin)
    {
        //如果不相等那么返回flase
        return false;
    }
    else
    {
        //相等返回true
        return true;
    }
}

//取得当前页面数据
NetworkWanPppoeData & CDlgNetworkWANPort::GetCurrentPageData(
    NetworkWanPppoeData & currentPageData) const
{
    currentPageData.m_strPppoeUser =
        m_pEditPPPoEUser->text().trimmed().toUtf8().data();

    currentPageData.m_strPppoePin =
        m_pEditPPPoEPWD->text().trimmed().toUtf8().data();
    return currentPageData;
}


//设置当前页面数据
void CDlgNetworkWANPort::SetCurrentPageData(NetworkWanPppoeData & currentPageData)
{
    m_pEditPPPoEUser->setText(toQString(currentPageData.m_strPppoeUser.c_str()));
    m_pEditPPPoEPWD->setText(toQString(currentPageData.m_strPppoePin.c_str()));
}
#endif


//比较页面前后数据
bool CDlgNetworkWANPort::ComparePageData(const NetworkWanStaticIpData & beforePageData,
        const NetworkWanStaticIpData & afterPageData) const
{
    if (strcmp(beforePageData.m_strWanIp.c_str(), afterPageData.m_strWanIp.c_str())
            || strcmp(beforePageData.m_strWanGateWay.c_str(), afterPageData.m_strWanGateWay.c_str())
            || strcmp(beforePageData.m_strWanMask.c_str(), afterPageData.m_strWanMask.c_str())
            || strcmp(beforePageData.m_strWanPrimaryDns.c_str(), afterPageData.m_strWanPrimaryDns.c_str())
            || strcmp(beforePageData.m_strWanSendaryDns.c_str(), afterPageData.m_strWanSendaryDns.c_str()))
    {
        //如果不相等那么返回flase
        return false;
    }
    else
    {
        //相等返回true
        return true;
    }
}

//取得当前页面数据
NetworkWanStaticIpData & CDlgNetworkWANPort::GetCurrentPageData(NetworkWanStaticIpData &
        currentPageData) const
{
    currentPageData.m_strWanIp =
        m_pEditIP->text().trimmed().toUtf8().data();

    currentPageData.m_strWanGateWay =
        m_pEditGateway->text().trimmed().toUtf8().data();

    currentPageData.m_strWanMask =
        m_pEditSubnetMask->text().trimmed().toUtf8().data();

    currentPageData.m_strWanPrimaryDns =
        m_pEditPriDNS->text().trimmed().toUtf8().data();

    currentPageData.m_strWanSendaryDns =
        m_pEditSecDNS->text().trimmed().toUtf8().data();

    return currentPageData;
}

//设置当前页面数据
void CDlgNetworkWANPort::SetCurrentPageData(NetworkWanStaticIpData & currentPageData)
{
    m_pEditIP->setText(toQString(currentPageData.m_strWanIp.c_str()));
    m_pEditGateway->setText(toQString(currentPageData.m_strWanGateWay.c_str()));
    m_pEditPriDNS->setText(toQString(currentPageData.m_strWanPrimaryDns.c_str()));
    m_pEditSecDNS->setText(toQString(currentPageData.m_strWanSendaryDns.c_str()));
    m_pEditSubnetMask->setText(toQString(currentPageData.m_strWanMask.c_str()));
}

//比较页面前后数据
bool CDlgNetworkWANPort::ComparePageData(const NetworkWanDnsServerData & beforePageData,
        const NetworkWanDnsServerData & afterPageData) const
{
    //比较配置是否有变化
    if (beforePageData.m_bStaticDns != afterPageData.m_bStaticDns
            || beforePageData.m_strWanPrimaryDns != afterPageData.m_strWanPrimaryDns
            || beforePageData.m_strWanSendaryDns != afterPageData.m_strWanSendaryDns)
    {
        return false;
    }

    return true;
}

//取得当前页面数据
NetworkWanDnsServerData & CDlgNetworkWANPort::GetCurrentPageData(NetworkWanDnsServerData &
        currentPageData) const
{
    currentPageData.m_nMode = m_nMode;

    currentPageData.m_bStaticDns = m_pCmbBoxEnable
                                   ->itemData(m_pCmbBoxEnable->currentIndex(), Qt::UserRole).value<bool>();

    currentPageData.m_strWanPrimaryDns =
        m_pEditDHCPPriDNS->text().trimmed().toUtf8().data();

    currentPageData.m_strWanSendaryDns =
        m_pEditDHCPSecDNS->text().trimmed().toUtf8().data();
    return currentPageData;
}

//设置DHCP下的静态DNS
void CDlgNetworkWANPort::SetCurrentPageData(NetworkWanDnsServerData & currentPageData)
{
    //设置当前选中
    m_pCmbBoxEnable->setCurrentIndex(m_pCmbBoxEnable
                                     ->findData(currentPageData.m_bStaticDns, Qt::UserRole));

    //设置数据
    m_pEditDHCPPriDNS->setText(toQString(currentPageData.m_strWanPrimaryDns.c_str()));
    m_pEditDHCPSecDNS->setText(toQString(currentPageData.m_strWanSendaryDns.c_str()));
}

// 对输入框的数据进行验证
bool CDlgNetworkWANPort::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_nMode == IPAM_IPV4)
    {
        switch (m_eCurrentPageWanType.m_eType)
        {
        case WAN_DHCP:
            {
                //判断首DNS地址
                if (!m_pEditDHCPPriDNS->text().trimmed().isEmpty()
                        && !qLineEditUtility::IsFormatValid(m_pEditDHCPPriDNS->text(), EDIT_TYPE_IPV4))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_PRIMARY_DNS);
                    objErrorContent.pLineEdit = m_pEditDHCPPriDNS;
                    return false;
                }

                //sec dns可以为空，但如果不为空则必须是可用的IP
                if (m_pEditDHCPSecDNS->text().trimmed().isEmpty())
                {
                    return true;
                }
                else
                {
                    //判断备用DNS地址
                    if (!qLineEditUtility::IsFormatValid(m_pEditDHCPSecDNS->text(), EDIT_TYPE_IPV4))
                    {
                        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_SECONDARY_DNS);
                        objErrorContent.pLineEdit = m_pEditDHCPSecDNS;
                        return false;
                    }
                }
            }
            break;
#if IF_FEATURE_PPPOE
        case WAN_PPPOE:
            {
                //判断PPPoE用户名
                if (m_pEditPPPoEUser->text().trimmed().isEmpty()
                        || !qLineEditUtility::IsFormatValid(m_pEditPPPoEUser->text(), EDIT_TYPE_NAME_SHORT))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PPPOE_USER_IS_INVALID);
                    objErrorContent.pLineEdit = m_pEditPPPoEUser;
                    return false;
                }

                if (!m_pEditPPPoEPWD->text().trimmed().isEmpty()
                        && !qLineEditUtility::IsFormatValid(m_pEditPPPoEPWD->text(), EDIT_TYPE_PASSWORD))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_PPPOE_PASSWORD_IS_INVALID);
                    objErrorContent.pLineEdit = m_pEditPPPoEUser;
                    return false;
                }
            }
            break;
#endif
        case WAN_STATIC:
            {
                //判断IP地址
                if (!qLineEditUtility::IsFormatValid(m_pEditIP->text(), EDIT_TYPE_IPV4))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_IP_ADDRESS);
                    objErrorContent.pLineEdit = m_pEditIP;
                    return false;
                }

                //判断子网掩码地址
                if (!qLineEditUtility::IsFormatValid(m_pEditSubnetMask->text(), EDIT_TYPE_IPV4_NETMASK))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_SUBNET_MASK);
                    objErrorContent.pLineEdit = m_pEditSubnetMask;
                    return false;
                }

                //判断网关地址
                if (!qLineEditUtility::IsFormatValid(m_pEditGateway->text(), EDIT_TYPE_IPV4))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_DEFAULT_GATEWAY);
                    objErrorContent.pLineEdit = m_pEditGateway;
                    return false;
                }

                //判断首DNS地址
                if (!m_pEditPriDNS->text().trimmed().isEmpty()
                        && !qLineEditUtility::IsFormatValid(m_pEditPriDNS->text(), EDIT_TYPE_IPV4))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_PRIMARY_DNS);
                    objErrorContent.pLineEdit = m_pEditPriDNS;
                    return false;
                }

                //sec dns可以为空，但如果不为空则必须是可用的IP
                if (m_pEditSecDNS->text().trimmed().isEmpty())
                {
                    return true;
                }
                else
                {
                    //判断备用DNS地址
                    if (!qLineEditUtility::IsFormatValid(m_pEditSecDNS->text(), EDIT_TYPE_IPV4))
                    {
                        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_SECONDARY_DNS);
                        objErrorContent.pLineEdit = m_pEditSecDNS;
                        return false;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (m_eCurrentPageWanType.m_eType)
        {
        case IPv6_WAN_DHCP:
            {
                //判断首DNS地址
                if (!m_pEditDHCPPriDNS->text().trimmed().isEmpty()
                        && !qLineEditUtility::IsFormatValid(m_pEditDHCPPriDNS->text(), EDIT_TYPE_IPV6))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_PRIMARY_DNS);
                    objErrorContent.pLineEdit = m_pEditDHCPPriDNS;
                    return false;
                }

                //sec dns可以为空，但如果不为空则必须是可用的IP
                if (m_pEditDHCPSecDNS->text().trimmed().isEmpty())
                {
                    return true;
                }
                else
                {
                    //判断备用DNS地址
                    if (!qLineEditUtility::IsFormatValid(m_pEditDHCPSecDNS->text(), EDIT_TYPE_IPV6))
                    {
                        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_SECONDARY_DNS);
                        objErrorContent.pLineEdit = m_pEditDHCPSecDNS;
                        return false;
                    }
                }
            }
            break;
        case IPv6_WAN_STATIC:
            {
                //判断IP地址
                if (!qLineEditUtility::IsFormatValid(m_pEditIP->text(), EDIT_TYPE_IPV6))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_IP_ADDRESS);
                    objErrorContent.pLineEdit = m_pEditIP;
                    return false;
                }

                //判断子网掩码地址
                if (!qLineEditUtility::IsFormatValid(m_pEditSubnetMask->text(), EDIT_TYPE_IPV6_NETMASK))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_IPV6_IP_PREFIX);
                    objErrorContent.pLineEdit = m_pEditSubnetMask;
                    return false;
                }

                //判断网关地址
                if (!qLineEditUtility::IsFormatValid(m_pEditGateway->text(), EDIT_TYPE_IPV6_GATEWAY))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_DEFAULT_GATEWAY);
                    objErrorContent.pLineEdit = m_pEditGateway;
                    return false;
                }

                //判断首DNS地址
                if (!m_pEditPriDNS->text().trimmed().isEmpty()
                        && !qLineEditUtility::IsFormatValid(m_pEditPriDNS->text(), EDIT_TYPE_IPV6))
                {
                    objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_PRIMARY_DNS);
                    objErrorContent.pLineEdit = m_pEditPriDNS;
                    return false;
                }

                //sec dns可以为空，但如果不为空则必须是可用的IP
                if (m_pEditSecDNS->text().trimmed().isEmpty())
                {
                    return true;
                }
                else
                {
                    //判断备用DNS地址
                    if (!qLineEditUtility::IsFormatValid(m_pEditSecDNS->text(), EDIT_TYPE_IPV6))
                    {
                        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_SECONDARY_DNS);
                        objErrorContent.pLineEdit = m_pEditSecDNS;
                        return false;
                    }
                }
            }
            break;
        default:
            break;
        }
    }


    return true;
}

//用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
void CDlgNetworkWANPort::InitData()
{
    m_pCmbBoxType = new QComboBox(this);
#if IF_FEATURE_PPPOE
    m_pEditPPPoEUser = new QLineEdit(this);
    m_pEditPPPoEPWD = new QLineEdit(this);
#endif
    m_pEditIP = new QLineEdit(this);
    m_pEditSubnetMask = new QLineEdit(this);
    m_pEditGateway = new QLineEdit(this);
    m_pEditPriDNS = new QLineEdit(this);
    m_pEditSecDNS = new QLineEdit(this);
    m_pCmbBoxEnable = new QComboBox(this);
    m_pEditDHCPPriDNS = new QLineEdit(this);
    m_pEditDHCPSecDNS = new QLineEdit(this);

    if (NULL == m_pCmbBoxType
#if IF_FEATURE_PPPOE
            || NULL == m_pEditPPPoEUser
            || NULL == m_pEditPPPoEPWD
#endif
            || NULL == m_pEditIP
            || NULL == m_pEditSubnetMask
            || NULL == m_pEditGateway
            || NULL == m_pEditPriDNS
            || NULL == m_pEditSecDNS
            || NULL == m_pCmbBoxEnable
            || NULL == m_pEditDHCPPriDNS
            || NULL == m_pEditDHCPSecDNS)
    {
        return;
    }

#if IF_FEATURE_PPPOE
    m_pEditPPPoEPWD->setEchoMode(QLineEdit::Password);
#endif

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxType, Qt::black);

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxEnable, Qt::black);
    m_pCmbBoxEnable->addItem(LANG_TRANSLATE(TRID_DISABLED), false);
    m_pCmbBoxEnable->addItem(LANG_TRANSLATE(TRID_ENABLED), true);
#if IF_FEATURE_PPPOE
    qLineEditUtility::SetLineEditStyle(m_pEditPPPoEUser, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditPPPoEPWD, Qt::black);
#endif
    qLineEditUtility::SetLineEditStyle(m_pEditIP, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditSubnetMask, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditGateway, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditPriDNS, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditSecDNS, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditDHCPPriDNS, Qt::black);
    qLineEditUtility::SetLineEditStyle(m_pEditDHCPSecDNS, Qt::black);

#if IF_FEATURE_PPPOE
    qInstallIME(m_pEditPPPoEUser, IME_DIAL);
    qInstallIME(m_pEditPPPoEPWD, IME_PWD);
#endif
    qInstallIME(m_pEditIP, IME_DIGIT);
    qInstallIME(m_pEditSubnetMask, IME_DIGIT);
    qInstallIME(m_pEditGateway, IME_DIGIT);
    qInstallIME(m_pEditPriDNS, IME_DIGIT);
    qInstallIME(m_pEditSecDNS, IME_DIGIT);
    qInstallIME(m_pEditDHCPPriDNS, IME_DIGIT);
    qInstallIME(m_pEditDHCPSecDNS, IME_DIGIT);

    //设置输入框最大输入长度
#if IF_FEATURE_PPPOE
    qLineEditUtility::SetMaxLength(m_pEditPPPoEUser, EDIT_TYPE_NAME_SHORT);
    qLineEditUtility::SetMaxLength(m_pEditPPPoEPWD, EDIT_TYPE_PASSWORD);
#endif
    qLineEditUtility::SetMaxLength(m_pEditIP, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditSubnetMask, EDIT_TYPE_IPV4_NETMASK);
    qLineEditUtility::SetMaxLength(m_pEditGateway, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditPriDNS, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditSecDNS, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditDHCPPriDNS, EDIT_TYPE_IPV4);
    qLineEditUtility::SetMaxLength(m_pEditDHCPSecDNS, EDIT_TYPE_IPV4);

    //为界面上各控件安装事件过滤器
    m_pCmbBoxType->installEventFilter(this);
#if IF_FEATURE_PPPOE
    m_pEditPPPoEUser->installEventFilter(this);
    m_pEditPPPoEPWD->installEventFilter(this);
#endif
    m_pEditIP->installEventFilter(this);
    m_pEditSubnetMask->installEventFilter(this);
    m_pEditGateway->installEventFilter(this);
    m_pEditPriDNS->installEventFilter(this);
    m_pEditSecDNS->installEventFilter(this);
    m_pCmbBoxEnable->installEventFilter(this);
    m_pEditDHCPPriDNS->installEventFilter(this);
    m_pEditDHCPSecDNS->installEventFilter(this);
    this->installEventFilter(this);

#if IF_FEATURE_PPPOE
    m_pEditPPPoEUser->hide();
    m_pEditPPPoEPWD->hide();
#endif
    m_pEditIP->hide();
    m_pEditSubnetMask->hide();
    m_pEditGateway->hide();
    m_pEditPriDNS->hide();
    m_pEditSecDNS->hide();
    m_pCmbBoxEnable->hide();
    m_pEditDHCPPriDNS->hide();
    m_pEditDHCPSecDNS->hide();

    m_pCmbBoxType->setCurrentIndex(-1);
    connect(m_pCmbBoxType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbBoxTypeChanged(int)));

    m_pCmbBoxEnable->setCurrentIndex(-1);
    connect(m_pCmbBoxEnable, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnSwitchDnsServerStatus(int)));
}


//添加内容
void CDlgNetworkWANPort::AddPageContent()
{
    if (m_nMode == IPAM_IPV4)
    {
        m_pCmbBoxType->addItem(LANG_TRANSLATE(TRID_DHCP),
                               QVariant::fromValue(WAN_DHCP));

#if IF_FEATURE_PPPOE
        m_pCmbBoxType->addItem(LANG_TRANSLATE(TRID_PPPOE),
                               QVariant::fromValue(WAN_PPPOE));
#endif

        m_pCmbBoxType->addItem(LANG_TRANSLATE(TRID_STATIC_IP),
                               QVariant::fromValue(WAN_STATIC));
    }
    else
    {
        m_pCmbBoxType->addItem(LANG_TRANSLATE(TRID_DHCP),
                               QVariant::fromValue(IPv6_WAN_DHCP));

        m_pCmbBoxType->addItem(LANG_TRANSLATE(TRID_STATIC_IP),
                               QVariant::fromValue(IPv6_WAN_STATIC));
    }

}
