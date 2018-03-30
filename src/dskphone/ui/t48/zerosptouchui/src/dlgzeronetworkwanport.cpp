#include "dlgzeronetworkwanport.h"

#include "qtcommon/qmisc.h"
#include "setting/src/networkcontroller.h"
#include "setting/include/common.h"
#include "settingui/src/networksettingmanager.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qlineeditutility.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qcheckbox.h"
#include "inputmethod/inputmethod.h"
#include "messagebox/modmessagebox.h"
#include "baseui/fontcolor.h"

#include "devicelib/phonedevice.h"
#include "devicelib/networkserver.h"

#include "dsklog/log.h"

Q_DECLARE_METATYPE(WanType);
Q_DECLARE_METATYPE(IPv6WanType);
//!!!!!之前setting模块包括很多模块，现在已经隔离了
#define doNetworkControl (NS_SETTING::CNetworkController::GetInstance())


CDlgZeroNetworkWANPort::CDlgZeroNetworkWANPort(QWidget * parent)
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
    , m_pCheckBoxDHCP(NULL)
    , m_pEditDHCPPriDNS(NULL)
    , m_pEditDHCPSecDNS(NULL)
    , m_nMode(0)
{
    InitData();
}

#ifdef DEBUG_IME_WGT
void CDlgZeroNetworkWANPort::Print(bool bSt /*= true*/)
{
    SETTINGUI_INFO("------------------- %s -----------------", bSt ? "InstallIME" : "UninstallIME");
#if IF_FEATURE_PPPOE
    SETTINGUI_INFO("PPPoEUser[%d]", m_pEditPPPoEUser);
    SETTINGUI_INFO("PPPoEPWD[%d]", m_pEditPPPoEPWD);
#endif
    SETTINGUI_INFO("PriDNS[%d]", m_pEditPriDNS);
    SETTINGUI_INFO("SecDNS[%d]", m_pEditSecDNS);
    SETTINGUI_INFO("SubnetMask[%d]", m_pEditSubnetMask);
    SETTINGUI_INFO("IP[%d]", m_pEditIP);
    SETTINGUI_INFO("Gateway[%d]", m_pEditGateway);
    SETTINGUI_INFO("DHCPPriDNS[%d]", m_pEditDHCPPriDNS);
    SETTINGUI_INFO("DHCPSecDNS[%d]", m_pEditDHCPSecDNS);
    SETTINGUI_INFO("------------------- end -----------------");
}
#endif

CDlgZeroNetworkWANPort::~CDlgZeroNetworkWANPort()
{
    UninstallAllIME();
#ifdef DEBUG_IME_WGT
    Print(false);
#endif
    m_vecWidgetPair.clear();
}

//设置网络类型
void CDlgZeroNetworkWANPort::SetMode(int iMode)
{
    m_nMode = iMode;
    //根据网络类型决定是非要在wanType的选项中添加PPPOE
    AddPageContent();
}

//通过C层获取已有配置数据，通过V层在界面上显示出来
bool CDlgZeroNetworkWANPort::LoadPageData()
{
    ZEROUI_INFO("CDlgZeroNetworkWANPort::LoadPageData()");

    if (JudgetWgtNull())
    {
        return true;
    }

    //this->blockSignals(true);
    //设置首选输入法

#if IF_FEATURE_PPPOE
    // IPV4 0 or IPV6 1 ipv6没有pppoe
    if (IPAM_IPV4 == m_nMode)
    {
        qLineEditUtility::SetCurrentInputMethod(m_pEditPPPoEUser, EDIT_TYPE_LOWER);
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
    //根据网络类型调用不同接口
    if (IPAM_IPV4 == m_nMode)
    {
        bIsSuccess = doNetworkControl->GetWanTypeData(&m_eInitWanType)
#if IF_FEATURE_PPPOE
                     && doNetworkControl->GetWanPppoeData(&m_eInitPPPoE)
#endif
                     && doNetworkControl->GetWanStaticData(&m_eInitStaticIp)
                     && doNetworkControl->GetWanStaticDnsServerData(&m_eInitDnsServerData);
    }
    else
    {
        bIsSuccess = doNetworkControl->GetIPv6WanTypeData(&m_eInitWanType)
#if IF_FEATURE_PPPOE
                     && doNetworkControl->GetWanPppoeData(&m_eInitPPPoE)
#endif
                     && doNetworkControl->GetWanStaticIPv6Data(&m_eInitStaticIp)
                     && doNetworkControl->GetWanStaticDnsServerData(&m_eInitDnsServerData);
    }

    if (bIsSuccess)
    {
        if (IPAM_IPV4 == m_nMode)
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
        this->blockSignals(false);
        return true;
    }
    else
    {
        this->blockSignals(false);
        //加载数据失败，返回false
        return false;
    }
}


//通过C层获取配置数据，与GetPageData值比较，有改动时返回true
bool CDlgZeroNetworkWANPort::IsStatusChanged()
{
    // 先判断WAN类型是否改变
    if (m_eInitWanType.m_eType != m_eCurrentPageWanType.m_eType)
    {
        return true;
    }

    //配置类型没有变化,则比较下面的配置是否有变化
    bool bResult = false;
    if (IPAM_IPV4 == m_nMode)
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
        //IPv6没有PPPOE
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

//将预编辑的字符输入到控件中
void CDlgZeroNetworkWANPort::ReturnAllEdit()
{
    qReturnEdit(m_pEditIP);
    qReturnEdit(m_pEditSubnetMask);
    qReturnEdit(m_pEditGateway);
    qReturnEdit(m_pEditPriDNS);
    qReturnEdit(m_pEditSecDNS);
    qReturnEdit(m_pEditDHCPPriDNS);
    qReturnEdit(m_pEditDHCPSecDNS);

#if IF_FEATURE_PPPOE
    if (IPAM_IPV4 == m_nMode)
    {
        qReturnEdit(m_pEditPPPoEUser);
        qReturnEdit(m_pEditPPPoEPWD);
    }
#endif
}

// 卸载界面上所有的输入法
void CDlgZeroNetworkWANPort::UninstallAllIME()
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
bool CDlgZeroNetworkWANPort::SavePageData()
{
    if (JudgetWgtNull())
    {
        return true;
    }
    //提起页面数据
    if (IsStatusChanged())
    {
        QString strContent;
        QString strTitle;

        m_eInitWanType.m_eType = m_eCurrentPageWanType.m_eType;

        if (IPAM_IPV4 == m_nMode)
        {
            //保存网络类型
            doNetworkControl->SetWanTypeData(m_eCurrentPageWanType);
            //调用控制层：保存数据
            switch (m_eCurrentPageWanType.m_eType)
            {
            case WAN_DHCP:
            {
                NetworkWanDnsServerData currentPageData;
                GetCurrentPageData(currentPageData);
                if (!currentPageData.m_bStaticDns)
                {
                    m_eInitDnsServerData.m_bStaticDns = currentPageData.m_bStaticDns;

                    doNetworkControl->SetWanStaticDnsServerData(m_eInitDnsServerData);
                }
                else
                {
                    //保存网络类型
                    doNetworkControl->SetWanStaticDnsServerData(currentPageData);
                }
            }
            break;
#if IF_FEATURE_PPPOE
            case WAN_PPPOE:
            {
                //先判断VLAN状态，如果是开启的，则要将其关闭
                NetworkVlanData configVlanData;
                bool bIsSuccess = doNetworkControl->GetVlanData(&configVlanData);
                if (bIsSuccess)
                {
                    if (configVlanData.m_isWanEnable)
                    {
                        configVlanData.m_isWanEnable = false;
                        doNetworkControl->SetVlanData(configVlanData);
                    }
                }

                NetworkWanPppoeData currentPageDataPPPoE;
                GetCurrentPageData(currentPageDataPPPoE);
                GetCurrentPageData(m_eInitPPPoE);

                doNetworkControl->SetWanPppoeData(currentPageDataPPPoE);
            }
            break;
#endif
            case WAN_STATIC:
            {
                NetworkWanStaticIpData currentPageDataStaticIp;
                GetCurrentPageData(currentPageDataStaticIp);
                GetCurrentPageData(m_eInitStaticIp);

                doNetworkControl->SetWanStaticData(currentPageDataStaticIp);
            }
            default:
                break;
            }
        }
        else
        {
            //保存网络类型
            doNetworkControl->SetIPv6WanTypeData(m_eCurrentPageWanType);

            //调用控制层：保存数据
            switch (m_eCurrentPageWanType.m_eType)
            {
            case IPv6_WAN_DHCP:
            {
                NetworkWanDnsServerData currentPageData;
                GetCurrentPageData(currentPageData);
                if (!currentPageData.m_bStaticDns)
                {
                    m_eInitDnsServerData.m_bStaticDns = currentPageData.m_bStaticDns;

                    doNetworkControl->SetWanStaticDnsServerData(m_eInitDnsServerData);
                }
                else
                {
                    //保存网络类型
                    doNetworkControl->SetWanStaticDnsServerData(currentPageData);
                }
            }
            break;
            case IPv6_WAN_STATIC:
            {
                NetworkWanStaticIpData currentPageDataStaticIp;
                GetCurrentPageData(currentPageDataStaticIp);
                GetCurrentPageData(m_eInitStaticIp);

                doNetworkControl->SetWanStaticIPv6Data(currentPageDataStaticIp);
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

//WAN type配置选择类型变化处理操作
void CDlgZeroNetworkWANPort::OnCmbBoxTypeChanged(int nIndex)
{
    ZEROUI_INFO("CDlgZeroNetworkWANPort::OnCmbBoxTypeChanged");
    //默认隐藏
#if IF_FEATURE_PPPOE
    m_pEditPPPoEUser->hide();
    m_pEditPPPoEPWD->hide();
#endif
    m_pEditIP->hide();
    m_pEditSubnetMask->hide();
    m_pEditGateway->hide();
    m_pEditPriDNS->hide();
    m_pEditSecDNS->hide();
    m_pEditDHCPPriDNS->hide();
    m_pEditDHCPSecDNS->hide();

    QString strLab = "";
    QWidgetPair qPair;
    //确保页面控件总数为0
    MakeReadyForRelayout();
    if (IPAM_IPV4 == m_nMode)
    {
        //记录当前ip 配置类型
        m_eCurrentPageWanType.m_eType = m_pCmbBoxType->itemData(nIndex).value<WanType>();
    }
    else
    {
        //记录当前ip 配置类型
        m_eCurrentPageWanType.m_eType = (WanType)m_pCmbBoxType->itemData(nIndex).value<IPv6WanType>();
    }

    //根据WANType类型重新加入要显示的控件
    if (IPAM_IPV4 == m_nMode)
    {
        strLab = LANG_TRANSLATE(TRID_IPV4_WAN_TYPE);
        qPair.first = strLab;
        qPair.second = m_pCmbBoxType;
        m_vecWidgetPair.push_back(qPair);
        switch (m_eCurrentPageWanType.m_eType)
        {
        case WAN_DHCP:
        {
            strLab = LANG_TRANSLATE(TRID_IPV4_STATIC_DNS);
            qPair.first = strLab;
            qPair.second = m_pCheckBoxDHCP;
            m_vecWidgetPair.push_back(qPair);

            if (m_pCheckBoxDHCP->GetData())
            {
                strLab = LANG_TRANSLATE(TRID_IPV4_PRIMARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditDHCPPriDNS;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_IPV4_SECONDARY_DNS);
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
            strLab = LANG_TRANSLATE(TRID_IPV4_ADDRESS);
            qPair.first = strLab;
            qPair.second = m_pEditIP;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV4_SUBNET_MASK);
            qPair.first = strLab;
            qPair.second = m_pEditSubnetMask;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV4_DEFAULT_GATEWAY);
            qPair.first = strLab;
            qPair.second = m_pEditGateway;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV4_PRIMARY_DNS);
            qPair.first = strLab;
            qPair.second = m_pEditPriDNS;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV4_SECONDARY_DNS);
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
        //为了区分IPv4和IPv4，在属性前面加上类型以标示
        strLab = LANG_TRANSLATE(TRID_IPV6_WAN_TYPE);
        qPair.first = strLab;
        qPair.second = m_pCmbBoxType;
        m_vecWidgetPair.push_back(qPair);
        switch (m_eCurrentPageWanType.m_eType)
        {
        case IPv6_WAN_DHCP:
        {
            strLab = LANG_TRANSLATE(TRID_IPV6_STATIC_DNS);
            qPair.first = strLab;
            qPair.second = m_pCheckBoxDHCP;
            m_vecWidgetPair.push_back(qPair);

            if (m_pCheckBoxDHCP->GetData())
            {
                strLab = LANG_TRANSLATE(TRID_IPV6_PRIMARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditDHCPPriDNS;
                m_vecWidgetPair.push_back(qPair);

                strLab = LANG_TRANSLATE(TRID_IPV6_SECONDARY_DNS);
                qPair.first = strLab;
                qPair.second = m_pEditDHCPSecDNS;
                m_vecWidgetPair.push_back(qPair);
            }
        }
        break;
        case IPv6_WAN_STATIC:
        {
            strLab = LANG_TRANSLATE(TRID_IPV6_IP_ADDRESS);
            qPair.first = strLab;
            qPair.second = m_pEditIP;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV6_PREFIX);
            qPair.first = strLab;
            qPair.second = m_pEditSubnetMask;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV6_DEFAULT_GATEWAY);
            qPair.first = strLab;
            qPair.second = m_pEditGateway;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV6_PRIMARY_DNS);
            qPair.first = strLab;
            qPair.second = m_pEditPriDNS;
            m_vecWidgetPair.push_back(qPair);

            strLab = LANG_TRANSLATE(TRID_IPV6_SECONDARY_DNS);
            qPair.first = strLab;
            qPair.second = m_pEditSecDNS;
            m_vecWidgetPair.push_back(qPair);
        }
        break;
        default:
            break;
        }
    }

    emit ReLayout(this);
}

void CDlgZeroNetworkWANPort::OnSwitchDnsServerStatus(bool bOn)
{
    bool bRelayout = false;
    int iCbTypeIndex = m_pCmbBoxType->currentIndex();

    if (IPAM_IPV4 == m_nMode)
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
}

//用于设置配置类型项
void CDlgZeroNetworkWANPort::SetCurrentPageData(NetworkWanTypeData & currentPageData)
{
    int nCount = 0;
    nCount = m_pCmbBoxType->count();
    for (int i = 0; i < nCount; i++)
    {
        if (IPAM_IPV4 == m_nMode
                && (currentPageData.m_eType
                    == m_pCmbBoxType->itemData(i).value<WanType>()))
        {
            m_pCmbBoxType->setCurrentIndex(i);
            break;
        }
        else if ((currentPageData.m_eType
                  == m_pCmbBoxType->itemData(i).value<IPv6WanType>()))
        {
            m_pCmbBoxType->setCurrentIndex(i);
            break;
        }
    }
}

//比较页面前后数据
#if IF_FEATURE_PPPOE
bool CDlgZeroNetworkWANPort::ComparePageData(const NetworkWanPppoeData & beforePageData,
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
NetworkWanPppoeData & CDlgZeroNetworkWANPort::GetCurrentPageData(
    NetworkWanPppoeData & currentPageData) const
{
    currentPageData.m_strPppoeUser =
        m_pEditPPPoEUser->text().trimmed().toUtf8().data();

    currentPageData.m_strPppoePin =
        m_pEditPPPoEPWD->text().trimmed().toUtf8().data();
    return currentPageData;
}

//设置当前页面数据
void CDlgZeroNetworkWANPort::SetCurrentPageData(NetworkWanPppoeData & currentPageData)
{
    m_pEditPPPoEUser->setText(toQString(currentPageData.m_strPppoeUser.c_str()));
    m_pEditPPPoEPWD->setText(toQString(currentPageData.m_strPppoePin.c_str()));
}
#endif

//比较页面前后数据
bool CDlgZeroNetworkWANPort::ComparePageData(const NetworkWanStaticIpData & beforePageData,
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
NetworkWanStaticIpData & CDlgZeroNetworkWANPort::GetCurrentPageData(NetworkWanStaticIpData &
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
void CDlgZeroNetworkWANPort::SetCurrentPageData(NetworkWanStaticIpData & currentPageData)
{
    m_pEditIP->setText(toQString(currentPageData.m_strWanIp.c_str()));
    m_pEditGateway->setText(toQString(currentPageData.m_strWanGateWay.c_str()));
    m_pEditPriDNS->setText(toQString(currentPageData.m_strWanPrimaryDns.c_str()));
    m_pEditSecDNS->setText(toQString(currentPageData.m_strWanSendaryDns.c_str()));
    m_pEditSubnetMask->setText(toQString(currentPageData.m_strWanMask.c_str()));
}

//比较页面前后数据
bool CDlgZeroNetworkWANPort::ComparePageData(const NetworkWanDnsServerData & beforePageData,
        const NetworkWanDnsServerData & afterPageData) const
{
    //比较配置是否有变化
    if (beforePageData.m_bStaticDns != afterPageData.m_bStaticDns
            || (afterPageData.m_bStaticDns
                && beforePageData.m_strWanPrimaryDns != afterPageData.m_strWanPrimaryDns)
            || (afterPageData.m_bStaticDns
                && beforePageData.m_strWanSendaryDns != afterPageData.m_strWanSendaryDns))
    {
        return false;
    }

    return true;
}

//取得当前页面数据
NetworkWanDnsServerData & CDlgZeroNetworkWANPort::GetCurrentPageData(NetworkWanDnsServerData &
        currentPageData) const
{
    currentPageData.m_nMode = m_nMode;

    currentPageData.m_bStaticDns = m_pCheckBoxDHCP->GetData();

    currentPageData.m_strWanPrimaryDns =
        m_pEditDHCPPriDNS->text().trimmed().toUtf8().data();

    currentPageData.m_strWanSendaryDns =
        m_pEditDHCPSecDNS->text().trimmed().toUtf8().data();
    return currentPageData;
}

//设置DHCP下的静态DNS
void CDlgZeroNetworkWANPort::SetCurrentPageData(NetworkWanDnsServerData & currentPageData)
{
    //设置当前选中
    m_pCheckBoxDHCP->SetData(currentPageData.m_bStaticDns);

    //设置数据
    m_pEditDHCPPriDNS->setText(toQString(currentPageData.m_strWanPrimaryDns.c_str()));
    m_pEditDHCPSecDNS->setText(toQString(currentPageData.m_strWanSendaryDns.c_str()));
}

// 对输入框的数据进行验证
bool CDlgZeroNetworkWANPort::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (IPAM_IPV4 == m_nMode)
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
                objErrorContent.strErrorContent = TRID_PPPOE_USER_IS_INVALID;
                objErrorContent.pLineEdit = m_pEditPPPoEUser;
                return false;
            }

            if (!m_pEditPPPoEPWD->text().trimmed().isEmpty()
                    && !qLineEditUtility::IsFormatValid(m_pEditPPPoEPWD->text(), EDIT_TYPE_PASSWORD))
            {
                objErrorContent.strErrorContent = TRID_PPPOE_PASSWORD_IS_INVALID;
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
                objErrorContent.strErrorContent = TRID_INVALID_IP_ADDRESS;
                objErrorContent.pLineEdit = m_pEditIP;
                return false;
            }

            //判断子网掩码地址
            if (!qLineEditUtility::IsFormatValid(m_pEditSubnetMask->text(), EDIT_TYPE_IPV4_NETMASK))
            {
                objErrorContent.strErrorContent = TRID_INVALID_SUBNET_MASK;
                objErrorContent.pLineEdit = m_pEditSubnetMask;
                return false;
            }

            //判断网关地址
            if (!qLineEditUtility::IsFormatValid(m_pEditGateway->text(), EDIT_TYPE_IPV4))
            {
                objErrorContent.strErrorContent = TRID_INVALID_DEFAULT_GATEWAY;
                objErrorContent.pLineEdit = m_pEditGateway;
                return false;
            }

            //判断首DNS地址
            if (!m_pEditPriDNS->text().trimmed().isEmpty()
                    && !qLineEditUtility::IsFormatValid(m_pEditPriDNS->text(), EDIT_TYPE_IPV4))
            {
                objErrorContent.strErrorContent = TRID_INVALID_PRIMARY_DNS;
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
                    objErrorContent.strErrorContent = TRID_INVALID_SECONDARY_DNS;
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
                objErrorContent.strErrorContent = TRID_INVALID_IP_ADDRESS;
                objErrorContent.pLineEdit = m_pEditIP;
                return false;
            }

            //判断子网掩码地址
            if (!qLineEditUtility::IsFormatValid(m_pEditSubnetMask->text(), EDIT_TYPE_IPV6_NETMASK))
            {
                objErrorContent.strErrorContent = TRID_INVALID_IPV6_IP_PREFIX;
                objErrorContent.pLineEdit = m_pEditSubnetMask;
                return false;
            }

            //判断网关地址
            if (!qLineEditUtility::IsFormatValid(m_pEditGateway->text(), EDIT_TYPE_IPV6_GATEWAY))
            {
                objErrorContent.strErrorContent = TRID_INVALID_DEFAULT_GATEWAY;
                objErrorContent.pLineEdit = m_pEditGateway;
                return false;
            }

            //判断首DNS地址
            if (!m_pEditPriDNS->text().trimmed().isEmpty()
                    && !qLineEditUtility::IsFormatValid(m_pEditPriDNS->text(), EDIT_TYPE_IPV6))
            {
                objErrorContent.strErrorContent = TRID_INVALID_PRIMARY_DNS;
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
                    objErrorContent.strErrorContent = TRID_INVALID_SECONDARY_DNS;
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
void CDlgZeroNetworkWANPort::InitData()
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
    m_pCheckBoxDHCP = new qCheckBox(this);
    m_pEditDHCPPriDNS = new QLineEdit(this);
    m_pEditDHCPSecDNS = new QLineEdit(this);

    if (JudgetWgtNull())
    {
        return;
    }

#if IF_FEATURE_PPPOE
    m_pEditPPPoEPWD->setEchoMode(QLineEdit::Password);
#endif

    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxType, Qt::black);

#if IF_FEATURE_PPPOE
    SetLineEditDefaultStyle(m_pEditPPPoEUser);
    SetLineEditDefaultStyle(m_pEditPPPoEPWD);
#endif
    SetLineEditDefaultStyle(m_pEditIP);
    SetLineEditDefaultStyle(m_pEditSubnetMask);
    SetLineEditDefaultStyle(m_pEditGateway);
    SetLineEditDefaultStyle(m_pEditPriDNS);
    SetLineEditDefaultStyle(m_pEditSecDNS);
    SetLineEditDefaultStyle(m_pEditDHCPPriDNS);
    SetLineEditDefaultStyle(m_pEditDHCPSecDNS);

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
#ifdef DEBUG_IME_WGT
    Print(true);
#endif
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
    m_pCheckBoxDHCP->installEventFilter(this);
    m_pEditDHCPPriDNS->installEventFilter(this);
    m_pEditDHCPSecDNS->installEventFilter(this);
    this->installEventFilter(this);

    m_pCmbBoxType->setCurrentIndex(-1);
    connect(m_pCmbBoxType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnCmbBoxTypeChanged(int)));

    connect(m_pCheckBoxDHCP, SIGNAL(checkChanged(bool)), this, SLOT(OnSwitchDnsServerStatus(bool)));
}

void CDlgZeroNetworkWANPort::hideWidget()
{
    if (!JudgetWgtNull())
    {
        m_pCmbBoxType->hide();
#if IF_FEATURE_PPPOE
        m_pEditPPPoEUser->hide();
        m_pEditPPPoEPWD->hide();
#endif
        m_pEditIP->hide();
        m_pEditSubnetMask->hide();
        m_pEditGateway->hide();
        m_pEditPriDNS->hide();
        m_pEditSecDNS->hide();
        m_pEditDHCPPriDNS->hide();
        m_pEditDHCPSecDNS->hide();
    }
}

//根据网络类型决定是非要在wanType的选项中添加PPPOE
void CDlgZeroNetworkWANPort::AddPageContent()
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

bool CDlgZeroNetworkWANPort::JudgetWgtNull()
{
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
            || NULL == m_pCheckBoxDHCP
            || NULL == m_pEditDHCPPriDNS
            || NULL == m_pEditDHCPSecDNS)
    {
        return true;
    }
    else
    {
        return false;
    }
}
